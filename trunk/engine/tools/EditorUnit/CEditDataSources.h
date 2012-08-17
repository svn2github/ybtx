#pragma once
#include "CDataSources.h"

namespace sqr_tools
{
	class CEditDataSources : public CDataSources
	{
	public:
		CEditDataSources(CDataSources* pDataSource);
		~CEditDataSources(void);
	public: //MOD
		virtual CPieceGroup*			NewPieceGroup(const string& szName, const wstring& Alias);
		virtual CPiece*					NewPiece(CPieceGroup* pGrp);
		virtual CMesh*					NewVertexAniMesh(void);
		virtual CMesh*					NewStaticMesh(void);
		virtual CMesh*					NewSkeletalMesh(void);

		virtual CAnimationGroup*		NewAnimationGroup(const string& szName, const wstring& Alias);
		virtual CAnimate*				NewAnimate(CAnimationGroup* pAniGrp, const string& szFileName, IDNAME& AniName);
		virtual CSkeletalFrame*			NewSkeletalFrame(const string& szName);
		virtual CSkeletal*				NewSkeletal(void);
		virtual CSkeletalUnit*			NewSkeletalUnit(void);

		virtual	IResTexture*			NewTexture(const string& szName, const wstring& Alias);
		virtual CRenderStyle*			NewRenderStyle(const string& szName, const wstring& Alias);
		virtual CPieceRenderStyle*		NewPieceRenderStyle();

		CVertexHardwareBuffer*			NewVertexBuffer(size_t vertexSize, size_t numVerts, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer);
		CIndexHardwareBuffer*			NewIndexBuffer(size_t numIndexes, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer);
		CRenderTexture*					NewRenderTexture(void);
		CRenderTexture*					NewDepthTexture(void);
		CWindowTarget*					NewWindow(const RenderSettings& settings);
		CVertexDeclaration*				NewVertexDeclaration(DWORD FVF);
	protected:
		CDataSources* m_pOrgSources;
	};
}