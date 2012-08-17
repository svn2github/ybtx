#pragma once


#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#include <windows.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <math.h>
#include "PkgStream.h"
#include "CRectangle.h"
#include "CPos.h"
#include "Algorithm.h"
#include "TRefPointer.h"
#include "CGraphicExp.h"
////////////////////////////////////////////////////////////////                 //需要更改

#define LPVERBUF			LPDIRECT3DVERTEXBUFFER9
#define LPINDBUF			LPDIRECT3DINDEXBUFFER9
#define LPDIRDEV			LPDIRECT3DDEVICE9
#define LPDIRTEX			LPDIRECT3DBASETEXTURE9
#define LPSURFACE			LPDIRECT3DSURFACE9
#define LPDIRSWAP			LPDIRECT3DSWAPCHAIN9
#define LPDIRDECLR			LPDIRECT3DVERTEXDECLARATION9
#define LPDIRSHADE			LPDIRECT3DVERTEXSHADER9
#define RENDSTATE			D3DRENDERSTATETYPE
#define TANSSTATE			D3DTRANSFORMSTATETYPE
#define VERTYPE				D3DPRIMITIVETYPE
#define D3DCAPS				D3DCAPS9
#define D3DVIEWPORT			D3DVIEWPORT9
#define D3DMATERIAL			D3DMATERIAL9
#define D3DLIGHT			D3DLIGHT9
#define D3DDISMD			D3DDISPLAYMODE
#define D3DVERELM			D3DVERTEXELEMENT9
#define D3DPRS_PARAM		D3DPRESENT_PARAMETERS

typedef TRefPointer<IDirect3D9>						ID3DPtr;
typedef TRefPointer<IDirect3DDevice9>				ID3DDevicePtr;
typedef TRefPointer<IDirect3DTexture9>				ID3DTexturePtr;
typedef TRefPointer<IDirect3DVolumeTexture9>		ID3DVolumeTexturePtr;
typedef TRefPointer<IDirect3DCubeTexture9>			ID3DCubeTexturePtr;
typedef TRefPointer<IDirect3DBaseTexture9>			ID3DBaseTexturePtr;
typedef TRefPointer<IDirect3DSurface9>				ID3DSurfacePtr;
typedef TRefPointer<IDirect3DVolume9>				ID3DVolumePtr;
typedef TRefPointer<IDirect3DIndexBuffer9>			ID3DIndexBufferPtr;
typedef TRefPointer<IDirect3DVertexBuffer9>			ID3DVertexBufferPtr;
typedef TRefPointer<IDirect3DVertexDeclaration9>	ID3DVertexDeclarationPtr;
typedef TRefPointer<IDirect3DQuery9>				ID3DQueryPtr;
typedef TRefPointer<IDirect3DSwapChain9>			ID3DSwapChainPtr;
typedef TRefPointer<IDirect3DVertexShader9>			ID3DVertexShaderPtr;
typedef TRefPointer<IDirect3DPixelShader9>			ID3DPixelShaderPtr;
typedef TRefPointer<ID3DXFont>						ID3DXFontPtr;
typedef TRefPointer<ID3DXSprite>					ID3DXSpritePtr;
typedef TRefPointer<ID3DXEffect>					ID3DXEffectPtr;
typedef TRefPointer<ID3DXEffectPool>				ID3DXEffectPoolPtr;
typedef TRefPointer<ID3DXInclude>					ID3DXIncludePtr;


#ifdef _DEBUG
#define D3DPerfBegin(msg) D3DPERF_BeginEvent(0xffffffff, msg)
#define D3DPerfEnd() D3DPERF_EndEvent()
#define DxTrace(hr, msg) { if (FAILED(hr)) DXTraceW(__FILE__,__LINE__,hr,msg,TRUE); }
#define VRBool(hr) { if( FAILED(hr) ) { DxTrace(hr, __FUNCTIONW__); return false; } }
#define V(hr) { if( FAILED(hr) ) { DxTrace(hr, __FUNCTIONW__); } }
#else
#define D3DPerfBegin(msg)
#define D3DPerfEnd()
#define DxTrace(hr, msg) hr
#define VRBool(hr) hr
#define V(hr) hr
#endif // _DEBUG
