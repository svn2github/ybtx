#include "stdafx.h"
#include "DirectX9.h"
#include "Win32.h"

// ---- 全局变量 ------------------------------------------------------------------------------

	static LPDIRECT3D9				g_pD3D = NULL;
	static LPDIRECT3DDEVICE9		g_pD3DDevice = NULL;
	static D3DPRESENT_PARAMETERS	g_pD3DPresent;

// ---- 函数实现 ------------------------------------------------------------------------------

	bool InitGraphicsDeviceDX9( GraphicDeviceDesc *pDesc/*=NULL*/ )
	{
		// `打开窗口时就已获得这个用来代表窗口的指针, 直接拿来使用.`
		HWND hWnd = GetWindowHandleWin32();

		RECT rect;
		GetWindowRect(hWnd, &rect);

		float w = static_cast<float>(rect.right - rect.left);
		float h = static_cast<float>(rect.bottom - rect.top);
		if ( w==0 || h==0 )
		{
			return false;
		}

		int multisamples = 0;
		if ( pDesc )
		{
			multisamples = pDesc->m_nMultiSamples;
		}

		if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
			return false;

		ZeroMemory( &g_pD3DPresent, sizeof(g_pD3DPresent) );
		g_pD3DPresent.Windowed					= TRUE;
		g_pD3DPresent.SwapEffect				= D3DSWAPEFFECT_DISCARD;
		g_pD3DPresent.BackBufferFormat			= D3DFMT_UNKNOWN;
		g_pD3DPresent.BackBufferCount			= 1; 
		g_pD3DPresent.EnableAutoDepthStencil	= TRUE;
		g_pD3DPresent.AutoDepthStencilFormat	= D3DFMT_D24S8;
		g_pD3DPresent.MultiSampleType			= D3DMULTISAMPLE_4_SAMPLES;
		g_pD3DPresent.hDeviceWindow				= hWnd;

		struct sDeviceType
		{
			D3DDEVTYPE	type;
			DWORD		behavior;
		};
		const int	device_types = 4;
		sDeviceType device_type[device_types] = 
		{
			{D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING},
			{D3DDEVTYPE_HAL, D3DCREATE_MIXED_VERTEXPROCESSING},
			{D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING},
			{D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING}
		};

		bool device_initialized = false;
		for ( int type=0; type<device_types; type++ )
		{
			if( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, device_type[type].type, hWnd,
				device_type[type].behavior, &g_pD3DPresent, &g_pD3DDevice )==D3D_OK )
			{
				device_initialized = true;
				break;
			}
		}

		return device_initialized;
	}

	void ReleaseGraphicsDeviceDX9( void )
	{
		if ( g_pD3DDevice )
		{
			g_pD3DDevice->Release();
			g_pD3DDevice = NULL;
		}

		if ( g_pD3D )
		{
			g_pD3D->Release();
			g_pD3D = NULL;
		}
	}

	bool ResetGraphicsDeviceDX9( void )
	{
		RECT rect;
		GetClientRect( GetWindowHandleWin32(), &rect );	

		g_pD3DPresent.BackBufferWidth	= rect.right - rect.left;
		g_pD3DPresent.BackBufferHeight	= rect.bottom - rect.top;
		g_pD3DPresent.BackBufferCount	= 1; // `提供一块backbuffer`

		if ( D3D_OK==g_pD3DDevice->Reset(&g_pD3DPresent) )
			return true;
		else
			return false;
	}

	LPDIRECT3DDEVICE9 GetGraphicsDeviceDX9( void )
	{
		return g_pD3DDevice;
	}