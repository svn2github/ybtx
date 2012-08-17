//------------------------------------------------------------------------------
//  CDx9DeviceCapacity.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CDx9DeviceCapacity.h"
#include "CDx9Adapter.h"
#include "CDx9RenderPipeline.h"
#include <ddraw.h>
#include "StringHelper.h"
namespace sqr
{
//------------------------------------------------------------------------------
bool
CDx9DeviceCapacity::IsSupportHwTandL()
{
	return (this->d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
}

//------------------------------------------------------------------------------
bool
CDx9DeviceCapacity::IsSupportVertexDeclType( DWORD type )
{
	return (this->d3dcaps.DeclTypes & type) != 0;
}

//------------------------------------------------------------------------------
uint
CDx9DeviceCapacity::GetMaxVertexIndex()
{
	return this->d3dcaps.MaxVertexIndex;
}

//------------------------------------------------------------------------------
uint
CDx9DeviceCapacity::GetMaxTextureWidth()
{
	return this->d3dcaps.MaxTextureWidth;
}

//------------------------------------------------------------------------------
uint
CDx9DeviceCapacity::GetMaxTextureHeight()
{
	return this->d3dcaps.MaxTextureHeight;
}

//------------------------------------------------------------------------------
bool
CDx9DeviceCapacity::IsTextureCanAutoGenMipmap()
{
	return (this->d3dcaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) != 0;
}

//------------------------------------------------------------------------------
bool
CDx9DeviceCapacity::IsTextureSupportNonPowerOf2()
{
	return (this->d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0;
}

//------------------------------------------------------------------------------
bool
CDx9DeviceCapacity::IsSupportVS( uint major, uint minor )
{
	return (D3DVS_VERSION(major, minor) <= this->d3dcaps.VertexShaderVersion);
}

//------------------------------------------------------------------------------
bool
CDx9DeviceCapacity::IsSupportPS( uint major, uint minor )
{
	return (D3DPS_VERSION(major, minor) <= this->d3dcaps.PixelShaderVersion);
}

//------------------------------------------------------------------------------
uint
CDx9DeviceCapacity::GetVS2DynamicFlowControlDepth()
{
	return this->d3dcaps.VS20Caps.DynamicFlowControlDepth;
}

//------------------------------------------------------------------------------
uint
CDx9DeviceCapacity::GetPS2DynamicFlowControlDepth()
{
	return this->d3dcaps.PS20Caps.DynamicFlowControlDepth;
}

//------------------------------------------------------------------------------
uint
CDx9DeviceCapacity::GetVS2StaticFlowControlDepth()
{
	return this->d3dcaps.VS20Caps.StaticFlowControlDepth;
}

//------------------------------------------------------------------------------
uint
CDx9DeviceCapacity::GetPS2StaticFlowControlDepth()
{
	return this->d3dcaps.PS20Caps.StaticFlowControlDepth;
}

//------------------------------------------------------------------------------
bool
CDx9DeviceCapacity::IsSupportTexture( texFORMAT format )
{
	HRESULT hr = this->d3d9->CheckDeviceFormat(this->adapter->GetAdapterNo(),
	             D3DDEVTYPE_HAL, this->adapter->GetDesktopMode().Format, 0,
				 D3DRTYPE_TEXTURE, DX9RenderPipeline::get(format));
	return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
bool
CDx9DeviceCapacity::IsSupportRenderTarget( texFORMAT format )
{
	HRESULT hr = this->d3d9->CheckDeviceFormat(this->adapter->GetAdapterNo(),
	             D3DDEVTYPE_HAL, this->adapter->GetDesktopMode().Format,
				 D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE,DX9RenderPipeline::get(format) );
	return SUCCEEDED(hr);
}

bool CDx9DeviceCapacity::IsSupportDepthTexture(texFORMAT format)
{
	HRESULT hr = this->d3d9->CheckDeviceFormat( this->adapter->GetAdapterNo(), 
				 D3DDEVTYPE_HAL, this->adapter->GetDesktopMode().Format,
				 D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, DX9RenderPipeline::get(format) );
	return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
/**
	一般支持反锯齿的显卡, 2x和4x都支持.
	我们不使用更高的(不实用).
	所以, 支持不支持就以4x为准吧.
 */
bool
CDx9DeviceCapacity::IsCanAntiAlias( texFORMAT format )
{
	HRESULT hr = this->d3d9->CheckDeviceMultiSampleType(this->adapter->GetAdapterNo(),
		D3DDEVTYPE_HAL, DX9RenderPipeline::get(format), TRUE, D3DMULTISAMPLE_4_SAMPLES, NULL);
	return SUCCEEDED(hr);
}

struct DDRAW_MATCH
{
	GUID guid;
	HMONITOR hMonitor;
	//GString strDriverName;
	bool bFound;
};

BOOL WINAPI DDEnumCallbackEx( GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm )
{
	DDRAW_MATCH* pDDMatch = ( DDRAW_MATCH* ) lpContext;
	if( pDDMatch->hMonitor == hm )
	{
		pDDMatch->bFound = true;
		//pDDMatch->strDriverName = lpDriverName;
		memcpy( &pDDMatch->guid, lpGUID, sizeof( GUID ) );
	}
	return TRUE;
}

HRESULT GetVideoMemoryViaDirectDraw( HMONITOR hMonitor, DWORD* pdwAvailableVidMem, DWORD* pdwAvailableAgpMem )
{
	typedef HRESULT ( WINAPI* LPDIRECTDRAWCREATE )( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
	LPDIRECTDRAW pDDraw = NULL;
	LPDIRECTDRAWENUMERATEEXA pDirectDrawEnumerateEx = NULL;
	HRESULT hr;
	bool bGotMemory = false;
	*pdwAvailableVidMem = 0;

	HINSTANCE hInstDDraw;
	LPDIRECTDRAWCREATE pDDCreate = NULL;

	hInstDDraw = LoadLibrary( "ddraw.dll" );
	if( hInstDDraw )
	{
		DDRAW_MATCH match;
		ZeroMemory( &match, sizeof( DDRAW_MATCH ) );
		match.hMonitor = hMonitor;

		pDirectDrawEnumerateEx = ( LPDIRECTDRAWENUMERATEEXA )GetProcAddress( hInstDDraw, "DirectDrawEnumerateExA" );

		if( pDirectDrawEnumerateEx )
		{
			hr = pDirectDrawEnumerateEx( DDEnumCallbackEx, ( VOID* )&match, DDENUM_ATTACHEDSECONDARYDEVICES );
		}

		pDDCreate = ( LPDIRECTDRAWCREATE )GetProcAddress( hInstDDraw, "DirectDrawCreate" );
		if( pDDCreate )
		{
			pDDCreate( &match.guid, &pDDraw, NULL );
			if(pDDraw)
			{
				LPDIRECTDRAW7 pDDraw7;
				if( SUCCEEDED( pDDraw->QueryInterface( IID_IDirectDraw7, ( VOID** )&pDDraw7 ) ) )
				{
					DDSCAPS2 ddscaps;
					ZeroMemory( &ddscaps, sizeof( DDSCAPS2 ) );
					ddscaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
					hr = pDDraw7->GetAvailableVidMem( &ddscaps, pdwAvailableVidMem, NULL );
					if( SUCCEEDED( hr ) )
					{
						ddscaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM;
						hr = pDDraw7->GetAvailableVidMem( &ddscaps, pdwAvailableAgpMem, NULL );
						if( SUCCEEDED( hr ) )
							bGotMemory = true;
					}

					pDDraw7->Release();
				}
			}
		}
		FreeLibrary( hInstDDraw );
	}


	if( bGotMemory )
		return S_OK;
	else
		return E_FAIL;
}

//------------------------------------------------------------------------------
void
CDx9DeviceCapacity::Initialize()
{
	Ast(NULL != this->adapter);
	string vendorDes = this->adapter->GetDescription();

	ErrLogHeadMsg("D3D Desc",vendorDes.c_str());

	toupper(vendorDes);
	if (GString::npos != vendorDes.find("NVIDIA"))
		this->vendor = VT_NVIDIA;
	else if (GString::npos != vendorDes.find("ATI"))
		this->vendor = VT_ATI;
	else if (GString::npos != vendorDes.find("INTEL"))
		this->vendor = VT_INTEL;
	else
		this->vendor = VT_OTHER;

	SQR_TRY
	{
		HMONITOR hMonitor = d3d9->GetAdapterMonitor( adapter->GetAdapterNo() );
		if( SUCCEEDED( GetVideoMemoryViaDirectDraw( hMonitor, (DWORD*)&VideoMem,(DWORD*)&AGPMem  ) ) )
		{
			VideoMem /= (1024*1024);
			AGPMem /= (1024*1024);
		}
		else
		{
			VideoMem = 0;
			AGPMem = 0;
			string ErrStr = "获取显存失败,程序将无法自动调整显示设置。\n请根据机器状况手动调整。";
			::MessageBox(NULL,ErrStr.c_str(),"自动调整失败",0);
		}
	}
	SQR_CATCH(exp)
	{
		VideoMem = 0;
		AGPMem = 0;
		exp.AppendMsg("获取显存失败");
		GfkLogExp(exp);	
	}
	SQR_TRY_END

	format(vendorDes,"VID:%d , AGP:%d",VideoMem,AGPMem );
	ErrLogHeadMsg("D3D Mem",vendorDes.c_str());
}

//------------------------------------------------------------------------------
texFORMAT
CDx9DeviceCapacity::GetHwDepthTextureFormat() const
{
	HRESULT hr;
	hr = this->d3d9->CheckDeviceFormat(this->adapter->GetAdapterNo(),
	                                   D3DDEVTYPE_HAL, this->adapter->GetDesktopMode().Format,
	                                   D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, D3DFMT_D24S8);
	if (SUCCEEDED(hr))
		return TFMT_D24S8;
	return TFMT_UNKNOWN;
}
}// namespace sqr

