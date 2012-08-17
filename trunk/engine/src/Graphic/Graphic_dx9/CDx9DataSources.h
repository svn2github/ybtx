#pragma once
#include "CDataSources.h"

namespace sqr
{
	class CTexture2DImp;
	class ITexture;

	class CDx9DataSources : public CDataSources
	{
	public:
		CDx9DataSources(void);
		~CDx9DataSources(void);

	public:
		CVertexHardwareBuffer*	NewVertexBuffer(size_t vertexSize, size_t numVerts, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer);
		CIndexHardwareBuffer*	NewIndexBuffer(size_t numIndexes, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer);

	public:
		IResTexture*			NewTexture(const string& szName, const wstring& Alias);
		CRenderTexture*			NewRenderTexture(void);
		CRenderTexture*			NewDepthTexture(void);
		CWindowTarget*			NewWindow(const RenderSettings& settings);
		CTexture2DImp*			NewTex2DImp(const ITexture* pParent);

	public:
		CVertexDeclaration*		NewVertexDeclaration(DWORD FVF);
	};
}