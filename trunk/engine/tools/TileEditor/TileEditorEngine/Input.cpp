//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "input.h"
#include "../TileEditorEngine/Console.h"
#include "../TileEditorEngine/Timer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace bc::ut;

namespace bg
{
namespace ip
{

CInputDevice * CInputDevice::one = NULL;

CInputDevice::CInputDevice()
{
	m_iEventQueueFront = 0;
	m_iEventQueueRear  = 0;
	InitializeCriticalSection(&m_csCriticalSectionEvent);

	m_xOffset = m_yOffset = m_xMouse = m_yMouse = 0;

	m_curPt.x = m_curPt.y = m_lastPt.x = m_lastPt.y = 0;
	
	ZeroMemory(m_pPreKeyStateBuffer,256);
	ZeroMemory(m_pCurKeyStateBuffer,256);
		
	m_pDInput = NULL;
	m_pDInputKeyboardDevice = NULL;

	m_iQueueCount = 0;
//	m_bMouseMoved = false;
	m_nMouseButtons = 0;
}

CInputDevice::~CInputDevice()
{
	DeleteCriticalSection(&m_csCriticalSectionEvent);
}

void CInputDevice::ParseMsg(UINT uiMessage,WPARAM wParam,LPARAM lParam)
{
	SEvent Event;
	Event.wParam = wParam;
	Event.lParam = lParam;
	Event.dwData = 0;

	switch(uiMessage)
	{
	case WM_LBUTTONDOWN:
		Event.eType =  EIET_MS_BTNDOWN;
		Event.dwData = MK_LBUTTON;
		m_nMouseButtons |= 1;
		break;
	
	case WM_MBUTTONDOWN:
		Event.eType = EIET_MS_BTNDOWN;
		Event.dwData = MK_MBUTTON;
		m_nMouseButtons |= 16;
		break;
	
	case WM_RBUTTONDOWN:
		Event.eType = EIET_MS_BTNDOWN;
		Event.dwData = MK_RBUTTON;
		m_nMouseButtons |= 2;
		break;

	case WM_LBUTTONUP:
		Event.eType = EIET_MS_BTNUP;
		Event.dwData = MK_LBUTTON;
		m_nMouseButtons &= ~1;
		break;

	case WM_MBUTTONUP:
		Event.eType = EIET_MS_BTNUP;
		Event.dwData= MK_MBUTTON;
		m_nMouseButtons &= ~16;
		break;

	case WM_MOUSEMOVE:
		Event.eType = EIET_MS_MOVE;
		Event.wParam = m_xOffset;
		Event.lParam = m_yOffset;
		break;

	case WM_RBUTTONUP:
		Event.eType = EIET_MS_BTNUP;
		Event.dwData= MK_RBUTTON;
		m_nMouseButtons &= ~2;
    	break;

	case 0x020a:
		Event.eType = EIET_MS_WHEEL;
		Event.fData = ((short)HIWORD(wParam)) / WHEEL_DELTA_RATIO;
		break;

	//case WM_CHAR:
	//	Event.eType = EIET_KB_CHAR;
	//	Event.dwData = (DWORD)wParam;
	//	break;

	default:
		return ;
	}

	PushEvent(Event.eType,Event.dwData,Event.wParam,Event.lParam);
}

void CInputDevice::PushEvent(EINPUT_EVENT_TYPE eType,DWORD dwData,WPARAM wParam,LPARAM lParam)
{
	EnterCriticalSection(&m_csCriticalSectionEvent);

	if ( (m_iEventQueueRear + 1) % MAX_EVENT_QUEUE_SIZE == m_iEventQueueFront ) 
		return ; // ignore extra input	
	
	SEvent & Event = m_EventQueue[m_iEventQueueRear];

	Event.eType = eType;
	Event.dwData= dwData;
	Event.wParam = wParam;
	Event.lParam = lParam;

	m_iEventQueueRear++;
	m_iEventQueueRear = m_iEventQueueRear % MAX_EVENT_QUEUE_SIZE;

	m_iQueueCount++;

	LeaveCriticalSection(&m_csCriticalSectionEvent);
}

bool CInputDevice::PopEvent( SEvent * pEvent)
{
	pEvent->eType = EIET_NONE;

	bool bResult = false;
	EnterCriticalSection(&m_csCriticalSectionEvent);
	
	if ( m_iEventQueueFront != m_iEventQueueRear ) // not empty
	{
		bResult = true;
		
		SEvent &Event = m_EventQueue[m_iEventQueueFront];
		pEvent->eType = Event.eType;
		pEvent->dwData= Event.dwData;
		pEvent->wParam= Event.wParam;
		pEvent->lParam= Event.lParam;

		m_iQueueCount--;

		m_iEventQueueFront++;
		m_iEventQueueFront = m_iEventQueueFront % MAX_EVENT_QUEUE_SIZE;
	}
	
	LeaveCriticalSection(&m_csCriticalSectionEvent);
	return bResult;
}

void CInputDevice::UpdateInput(void)
{
	// keyboard
	HRESULT  hResult; 
	
	if ( m_pDInputKeyboardDevice )
	{
		//////////////////////////////////////////////////////////////////////////
		
		// immediate mode
		memcpy(m_pPreKeyStateBuffer,m_pCurKeyStateBuffer,256);
		hResult = m_pDInputKeyboardDevice->GetDeviceState(256,(LPVOID)m_pCurKeyStateBuffer); 

		if ( FAILED(hResult) ) 
		{ 
			// if it failed, the device has probably been lost. 
			while ( hResult == DIERR_INPUTLOST || hResult == DIERR_NOTACQUIRED ) 
			{
				hResult = m_pDInputKeyboardDevice->Acquire();
			}

			m_pDInputKeyboardDevice->GetDeviceState(256,(LPVOID)m_pCurKeyStateBuffer); 
		}
		
		//////////////////////////////////////////////////////////////////////////
		
		// cache mode
		int nTicksCur = bc::ut::CTimer::GetSystemTime();

		// Loop through all the bufferedkeyboard events
		for (;;)
		{
			DIDEVICEOBJECTDATA od;
			DWORD dwElements = 1;
			
			hResult = m_pDInputKeyboardDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
			
			if ( FAILED(hResult) ) 
			{ 
				// if it failed, the device has probably been lost. 
				while ( hResult == DIERR_INPUTLOST || hResult == DIERR_NOTACQUIRED ) 
				{
					hResult = m_pDInputKeyboardDevice->Acquire();
				}
			
				m_pDInputKeyboardDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
			}
			
			if ( FAILED(hResult) || dwElements == 0)
				break;

			// fire a key event
			if (od.dwData & 0x80)
			{
				PushEvent(EIET_KB_KEYDOWN,od.dwOfs,0,0);
				m_nKeyLast = od.dwOfs;
				m_nKeyTicksPressed = nTicksCur;
				m_nKeyTicksRepeat = nTicksCur;
			}
			else
			{	
				PushEvent(EIET_KB_KEYUP,od.dwOfs,0,0); 
				if (od.dwOfs == m_nKeyLast)
				{
					m_nKeyLast = 0;
				}
			}
		}
	
		// auto-repeat the last key pressed
		if ( 0 != m_nKeyLast )
		{
			// issue the first repeat message?
			if ( m_nKeyTicksRepeat == m_nKeyTicksPressed )
			{
				if ( nTicksCur >= m_nKeyTicksPressed + m_nKeyboardDelay )
				{
					PushEvent(EIET_KB_KEYDOWN,m_nKeyLast,0,0);
					m_nKeyTicksRepeat = nTicksCur;
				}
			}
			// check if we need to repeat again
			else
			{
				if (nTicksCur >= m_nKeyTicksRepeat + m_nKeyboardSpeed)
				{
					PushEvent(EIET_KB_KEYDOWN,m_nKeyLast,0,0);
					m_nKeyTicksRepeat = nTicksCur;
				}
			}
		}

	} // end of keyboard

	//////////////////////////////////////////////////////////////////////////
	
	// mouse...

	::GetCursorPos(&m_curPt); 
	ScreenToClient(m_hWndD3D,&m_curPt);
	m_xMouse = m_curPt.x;
	m_yMouse = m_curPt.y;
	m_xOffset = m_curPt.x - m_lastPt.x;
	m_yOffset = m_curPt.y - m_lastPt.y;
	m_lastPt.x = m_curPt.x;
	m_lastPt.y = m_curPt.y;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//	const float WHEEL_SPEED = 20.0f;
//	// Loop through all the buffered mouse events
//
//	for (;;)
//	{
//		DIDEVICEOBJECTDATA od;
//		DWORD dwElements = 1;
//
//		hResult = m_pDInputMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
//		
//		if ( FAILED(hResult) ) 
//		{ 
//			// if it failed, the device has probably been lost. 
//			while ( hResult == DIERR_INPUTLOST || hResult == DIERR_NOTACQUIRED ) 
//			{
//				hResult = m_pDInputKeyboardDevice->Acquire();
//			}
//		
//			m_pDInputMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
//		}
//
//		if ( FAILED(hResult) || dwElements == 0 ) 
//			break;
//
//		switch (od.dwOfs) 
//		{
//		case DIMOFS_X: 
//			{
////				m_xMouse += od.dwData;
//				m_xOffset = od.dwData;
//				m_bMouseMoved = true;
//			}
//		break;
//
//		case DIMOFS_Y: 
//			{
////				m_yMouse += od.dwData;
//				m_yOffset = od.dwData;
//				m_bMouseMoved = true;
//			}
//		break; 
//
//		case DIMOFS_Z:
//			{
//				if ((int)od.dwData > 0)
//				{
//					PushEvent(EIET_MS_WHEEL,WHEEL_SPEED,0,0);
//				}
//				else
//				{
//					PushEvent(EIET_MS_WHEEL,-WHEEL_SPEED,0,0);
//				}
//			}
//		break;
//		
//		case DIMOFS_BUTTON0:
//			{
//				if (od.dwData != 0)
//				{
//					PushEvent(EIET_MS_BTNDOWN,MK_LBUTTON,0,0);
//					m_nMouseButtons |= 1;
//				}
//				else
//				{
//					PushEvent(EIET_MS_BTNUP,MK_LBUTTON,0,0);
//					m_nMouseButtons &= ~1;
//				}
//			}
//		break;
//
//		case DIMOFS_BUTTON1:
//			{
//				if (od.dwData != 0)
//				{
//					PushEvent(EIET_MS_BTNDOWN,MK_RBUTTON,0,0);
//					m_nMouseButtons |= 2;
//				}
//				else
//				{
//					PushEvent(EIET_MS_BTNUP,MK_RBUTTON,0,0);
//					m_nMouseButtons &= ~2;
//				}
//			}
//		break;
//
//		case DIMOFS_BUTTON2:
//			{
//				if (od.dwData != 0)
//				{
//					PushEvent(EIET_MS_BTNDOWN,MK_MBUTTON,0,0);
//					m_nMouseButtons |= 4;
//				}
//				else
//				{
//					PushEvent(EIET_MS_BTNUP,MK_MBUTTON,0,0);
//					m_nMouseButtons &= ~4;
//				}
//			}
//		break;
//
//		case DIMOFS_BUTTON3:
//			{
//				if (od.dwData != 0)
//				{
//					m_nMouseButtons |= 8;
//				}
//				else
//				{
//					m_nMouseButtons &= ~8;
//				}
//			}
//		break;
//
//		case DIMOFS_BUTTON4:
//			{
//				if (od.dwData != 0)
//				{
//					m_nMouseButtons |= 16;
//				}
//				else
//				{
//					m_nMouseButtons &= ~16;
//				}
//			}
//		break;
//
//		}
//	}
	
	////@ move msg may lose
	//if ( m_bMouseMoved )
	//{
	//	PushEvent(EIET_MS_MOVE,0,m_xOffset,m_yOffset);
	//	m_bMouseMoved = false;
	//}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
}

bool CInputDevice::Create(HWND hWnd,HWND hWndD3D)
{
	m_hWnd = hWnd;
	m_hWndD3D = hWndD3D;
	bool bResult = CreateDevice();
	assert(bResult);
	return bResult;
}

void CInputDevice::Destroy()
{
	DestroyDevice();
}

bool CInputDevice::CreateDevice()
{	
	#pragma warning( disable : 4312)

    HINSTANCE hInstance = (HINSTANCE)GetWindowLong(m_hWnd, GWL_HINSTANCE );
	
	// create dinput8 object for keyboard and mouse
	HRESULT hResult;
	hResult  = DirectInput8Create(hInstance,DIRECTINPUT_VERSION, IID_IDirectInput8,
			   (void **)&m_pDInput, NULL); 
    
	if ( FAILED(hResult) )  
	{
		LogConsole("error : CInputDevice : create DirectInput8 failed...");
		assert(0);
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	
	// create keyboard device
	hResult = m_pDInput->CreateDevice( GUID_SysKeyboard, &m_pDInputKeyboardDevice, NULL );
	
	if ( FAILED(hResult) )  
	{
		LogConsole("error : CInputDevice : create Input Device failed...");
		assert(0);
		return false;
	}
	
	// setup Format
	hResult = m_pDInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard); 
	if ( FAILED(hResult) )
	{ 
		LogConsole("error : CInputDevice : set keyboard input data format failed...");
		assert(0);
		return false; 
	}
	
	// set cooperative level
	hResult = m_pDInputKeyboardDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY); 
	if ( FAILED(hResult) )
	{
		LogConsole("error : CInputDevice : set keyboard copperative level failed...");
		assert(0);
		return false; 
	}

	// use buffered input
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	dipdw.dwData            = 32;

	hResult = m_pDInputKeyboardDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if ( FAILED(hResult) )
	{
		LogConsole("error : CInputDevice : set keyboard property failed...");
		assert(0);
		return false; 
	}
	
	// acquire ownership
	hResult = m_pDInputKeyboardDevice->Acquire();
	
	if ( FAILED(hResult) ) 
	{ 
		// if it failed, the device has probably been lost. 
		while ( hResult == DIERR_INPUTLOST || hResult == DIERR_NOTACQUIRED ) 
		{
			hResult = m_pDInputKeyboardDevice->Acquire();
		}
	}
	
	// get system values for key repeat
	GetSystemKeyboardSettings();
	
	// set up keyboard globals
	m_nKeyLast = 0;

	////////////////////////////////////////////////////////////////////////////////

	//////// create mouse device
	//////hResult = m_pDInput->CreateDevice( GUID_SysMouse, &m_pDInputMouseDevice, NULL );

	//////if ( FAILED(hResult) )  
	//////{
	//////	LogConsole("error : CInputDevice : create mouse device failed...");
	//////	assert(0);
	//////	return false;
	//////}

	//////// setup Format
	//////hResult = m_pDInputMouseDevice->SetDataFormat(&c_dfDIMouse2); 
	//////if ( FAILED(hResult) )
	//////{ 
	//////	LogConsole("error : CInputDevice : set mouse input data format failed...");
	//////	assert(0);
	//////	return false; 
	//////}

	//////// set cooperative level
	//////hResult = m_pDInputMouseDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY); 
	//////if ( FAILED(hResult) )
	//////{
	//////	LogConsole("error : CInputDevice : set mouse copperative level failed...");
	//////	assert(0);
	//////	return false; 
	//////}

	//////// use buffered input
	//////dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	//////dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	//////dipdw.diph.dwObj        = 0;
	//////dipdw.diph.dwHow        = DIPH_DEVICE;
	//////dipdw.dwData            = 32;

	//////hResult = m_pDInputMouseDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	//////if ( FAILED(hResult) )
	//////{
	//////	LogConsole("error : CInputDevice : set mouse property failed...");
	//////	assert(0);
	//////	return false; 
	//////}

	//////// acquire ownership
	//////hResult = m_pDInputMouseDevice->Acquire();
	//////if ( FAILED(hResult) ) 
	//////{ 
	//////	// if it failed, the device has probably been lost. 
	//////	while ( hResult == DIERR_INPUTLOST || hResult == DIERR_NOTACQUIRED ) 
	//////	{
	//////		hResult = m_pDInputMouseDevice->Acquire();
	//////	}
	//////}

	// set up our mouse globals
	::GetCursorPos(&m_curPt); 
	ScreenToClient(m_hWndD3D,&m_curPt);
	m_xMouse = m_curPt.x;
	m_yMouse = m_curPt.y;

	m_lastPt.x = m_curPt.x;
	m_lastPt.y = m_curPt.y;
	
	//////////////////////////////////////////////////////////////////////////
	
	return true;
}

void CInputDevice::DestroyDevice()
{
	if ( m_pDInputKeyboardDevice )
		m_pDInputKeyboardDevice->Unacquire();
	
	SAFE_RELEASE(m_pDInputKeyboardDevice);
	
	////if ( m_pDInputMouseDevice )
	////	m_pDInputMouseDevice->Unacquire();

	//SAFE_RELEASE(m_pDInputMouseDevice);

	SAFE_RELEASE(m_pDInput);
}

void CInputDevice::GetMousePosition(int & x, int & y)
{
	x = m_xMouse;
	y = m_yMouse;
}

void CInputDevice::GetMouseDelta(int & x, int & y)
{
	x = m_xOffset;
	y = m_yOffset;
}

void CInputDevice::GetSystemKeyboardSettings()
{
	// get the system key repeat delay.  The docs specify that a value of 0 equals a 
	// 250ms delay and a value of 3 equals a 1 sec delay.
	int nT;
	
	if ( SystemParametersInfo(SPI_GETKEYBOARDDELAY, NULL, &nT, 0) )
	{
		switch (nT)
		{
		case 0:
			m_nKeyboardDelay = 250;
			break;
		case 1:
			m_nKeyboardDelay = 500;
			break;
		case 2:
			m_nKeyboardDelay = 750;
			break;
		case 3:
		default:
			m_nKeyboardDelay = 1000;
			break;
		}
	}
	else
	{
		m_nKeyboardDelay = 1000;
	}

	// get the system key repeat rate.  The docs specify that a value of 0 equals a 
	// rate of 2.5 / sec (400 ms delay), and a value of 31 equals a rate of 30/sec
	// (33 ms delay).
	if ( SystemParametersInfo(SPI_GETKEYBOARDSPEED, NULL, &nT, 0) )
	{
		if (nT < 0)
			nT = 0;
		if (nT > 31)
			nT = 31;
		m_nKeyboardSpeed = 400 + nT * (33 - 400) / 31;
	}
	else
	{
		m_nKeyboardSpeed = 400;
	}
}

}
}