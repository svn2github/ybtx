//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// revise the input engine using dxinput mouse... 4.9 2004
//

#pragma once

#include "BluelyG.h"

#define  WHEEL_DELTA_RATIO 60.0f

namespace bg
{
namespace ip
{

enum EINPUT_EVENT_TYPE
{
	EIET_NONE,
	
	// keyboard
	EIET_KB_KEYDOWN,
	EIET_KB_KEYUP,  
	EIET_KB_CHAR,   
	
	// mouse
	EIET_MS_BTNDOWN,
	EIET_MS_BTNUP,  
	EIET_MS_MOVE,   
	EIET_MS_WHEEL
};

#define MAX_EVENT_QUEUE_SIZE 32

class CInputDevice
{
public:
	static CInputDevice * GetInstance() { if ( !one ) one = new CInputDevice; return one;} 

private:
	CInputDevice();
	virtual ~CInputDevice();
	static CInputDevice * one;

	int m_iQueueCount;	
	CRITICAL_SECTION m_csCriticalSectionEvent;

	// keyboard
	IDirectInput8	 	* m_pDInput;
	IDirectInputDevice8 * m_pDInputKeyboardDevice;

	// keyboard stuff
	int m_nKeyboardDelay;    // # of ticks before we start repeating keys
	int m_nKeyboardSpeed;    // # of ticks between key repeats
	unsigned int m_nKeyLast; // last key the user pressed (for auto-repeat)
	int m_nKeyTicksPressed;  // time the key was pressed
	int	m_nKeyTicksRepeat;   // last time we repeated the key
	unsigned char m_pPreKeyStateBuffer[256];
	unsigned char m_pCurKeyStateBuffer[256];

	// mouse
	IDirectInputDevice8 * m_pDInputMouseDevice; 
	
	// mouse stuff
	int m_xMouse;           // current position of the mouse
	int m_yMouse;
	int m_xOffset;
	int m_yOffset;
	
	POINT m_curPt;
	POINT m_lastPt;
	
	// bool m_bMouseMoved;
	
	int m_nMouseButtons;    // current button state

	// other stuff
	HWND m_hWnd;
	HWND m_hWndD3D;
	int m_iEventQueueFront;
	int m_iEventQueueRear;

public:
	struct SEvent									
	{
		EINPUT_EVENT_TYPE eType;
		
		union
		{
			DWORD dwData;
			float fData;
		};
		
		WPARAM wParam;
		LPARAM lParam;
	};
private:
	SEvent m_EventQueue[MAX_EVENT_QUEUE_SIZE];

	bool CreateDevice();
	void DestroyDevice();
	void GetSystemKeyboardSettings();

public:
	void UpdateInput();
	
	void ParseMsg(UINT uiMessage,WPARAM wParam,LPARAM lParam);
	
	void PushEvent(EINPUT_EVENT_TYPE eType,DWORD dwData,WPARAM wParam,LPARAM lParam);
	
	bool PopEvent(SEvent * pEvent);
	
	unsigned char GetKeyState(unsigned char byKey) const { return m_pCurKeyStateBuffer[byKey];}
	
	const unsigned char * GetKeyStateBuffer() const { return m_pCurKeyStateBuffer;}
	
	int GetMouseButtons() const { return m_nMouseButtons;}
	
	bool GetButtonState(int n)
	{
		return (m_nMouseButtons & n)!=0;
	}
	
	void GetMousePosition(int & x, int & y)	;
	void GetMouseDelta(int & x, int & y);
	
	bool Create(HWND hWnd, HWND hWndD3D);
	
	void Destroy();
};

}
}