#include "stdafx.h"
#include "CDx9DataSources.h"
#include "CDx9IndexHardwareBuffer.h"
#include "CDx9VertexHardwareBuffer.h"
#include "CDx9Texture.h"
#include "CDx9RenderTexture.h"
#include "CDx9DepthTexture.h"
#include "CDX9WindowTarget.h"
#include "CDX9VertexDeclaration.h"
#include "CDx9GPUProgramMgr.h"
#include "CDx9MainWindow.h"
#include "CDx9RenderPipeline.h"
#include "TSqrAllocator.inl"
#include "Graphic_dx9/CDx9Texture2DImp.h"
#include "CTexture.h"

#pragma comment( lib, "ddraw" )
#pragma comment( lib, "d3d9" )
#pragma comment( lib, "d3dx9" )
#pragma comment( lib, "d3dxof" )
#pragma comment( lib, "Dxerr" )
#pragma comment( lib, "dxguid" )
#pragma comment( lib, "Winmm" )

CDx9DataSources::CDx9DataSources(void)
{
	new CDx9GpuProgramMgr;
	new DX9MainWindowTarget;
	new DX9RenderPipeline;
}

CDx9DataSources::~CDx9DataSources(void)
{
}

CVertexHardwareBuffer*	CDx9DataSources::NewVertexBuffer(size_t vertexSize, size_t numVerts, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer)
{
	if(0==numVerts || 0==vertexSize)
		return NULL;
	return new DX9VertexHardwareBuffer(vertexSize,numVerts,usage,pInitData,useSysMem,useShadowBuffer);
}

CIndexHardwareBuffer*	CDx9DataSources::NewIndexBuffer(size_t numIndexes, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer)
{
	if(0==numIndexes)
		return NULL;
	return new DX9IndexHardwareBuffer(numIndexes,usage,pInitData,useSysMem,useShadowBuffer);
}

IResTexture*			CDx9DataSources::NewTexture(const string& szName, const wstring& Alias)
{
	return new IResTexture( szName, Alias );
}

CRenderTexture*			CDx9DataSources::NewRenderTexture(void)
{
	return new DX9RenderTexture();
}

CRenderTexture*			CDx9DataSources::NewDepthTexture(void)
{
	return new DX9DepthTexture();
}

CWindowTarget*			CDx9DataSources::NewWindow(const RenderSettings& settings)
{
	return new DX9WindowTarget(settings);
}

CVertexDeclaration*		CDx9DataSources::NewVertexDeclaration(DWORD FVF)
{
	return new DX9VertexDeclaration(FVF);
}

CTexture2DImp* sqr::CDx9DataSources::NewTex2DImp( const ITexture* pParent )
{
	return new CDx9Texture2DImp(pParent);
}