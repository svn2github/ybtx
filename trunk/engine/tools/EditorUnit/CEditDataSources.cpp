#include "stdafx.h"
#include "CEditDataSources.h"
#include "CEditPieceGroup.h"
#include "CEditAnimationsRes.h"
#include "CEditMesh.h"
#include "CEditRenderStyle.h"
#include "CEditTexture.h"

CEditDataSources::CEditDataSources(CDataSources* pDataSource)
: m_pOrgSources(pDataSource)
{
}

CEditDataSources::~CEditDataSources(void)
{ delete m_pOrgSources; }

CPieceGroup*		CEditDataSources::NewPieceGroup(const string& szName, const wstring& Alias)
{ return new CEditPieceGroup(szName,Alias); }

CPiece*				CEditDataSources::NewPiece(CPieceGroup* pGrp)
{ return new CEditPiece(pGrp);  }

CMesh*				CEditDataSources::NewVertexAniMesh(void)
{ return new CEditVertexAniMesh;  }

CMesh*				CEditDataSources::NewStaticMesh(void)
{ return new CEditStaticMesh;   }

CMesh*				CEditDataSources::NewSkeletalMesh(void)
{ return new CEditSkeletalMesh; }

CAnimationGroup*	CEditDataSources::NewAnimationGroup(const string& szName, const wstring& Alias)
{ return new CEditAnimationGroup(szName,Alias); }

CAnimate*				CEditDataSources::NewAnimate(CAnimationGroup* pAniGrp, const string& szFileName, IDNAME& AniName)
{  return new CEditAnimate(pAniGrp,szFileName, pAniGrp->GetAlias(),AniName); }

CSkeletalFrame*		CEditDataSources::NewSkeletalFrame(const string& szName)
{ return new CEditSkeletalFrame(szName); }

CSkeletal*			CEditDataSources::NewSkeletal(void)
{ return new CEditSkeletal(); }

CSkeletalUnit*		CEditDataSources::NewSkeletalUnit(void)
{ return new CEditSkeletalUnit(); }

IResTexture*		CEditDataSources::NewTexture( const string& szName, const wstring& Alias)
{ return new CEditTexture( szName, m_pOrgSources->NewTexture(szName,Alias) ); }

CRenderStyle*		CEditDataSources::NewRenderStyle(const string& szName, const wstring& Alias)
{return new CEditRenderStyle(szName,Alias);}

CPieceRenderStyle*	CEditDataSources::NewPieceRenderStyle()
{ return new CEditPieceRenderStyle(); }


CVertexHardwareBuffer*			CEditDataSources::NewVertexBuffer(size_t vertexSize, size_t numVerts, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer)
{ return m_pOrgSources->NewVertexBuffer(vertexSize,numVerts,usage,pInitData,useSysMem,useShadowBuffer); }

CIndexHardwareBuffer*			CEditDataSources::NewIndexBuffer(size_t numIndexes, Usage usage,void* pInitData, bool useSysMem, bool useShadowBuffer)
{ return m_pOrgSources->NewIndexBuffer(numIndexes,usage,pInitData,useSysMem,useShadowBuffer); }

CRenderTexture*					CEditDataSources::NewRenderTexture(void)
{ return m_pOrgSources->NewRenderTexture(); }

CRenderTexture*					CEditDataSources::NewDepthTexture(void)
{ return m_pOrgSources->NewDepthTexture(); }

CWindowTarget*					CEditDataSources::NewWindow(const RenderSettings& settings)
{ return m_pOrgSources->NewWindow(settings); }

CVertexDeclaration*				CEditDataSources::NewVertexDeclaration(DWORD FVF)
{ return m_pOrgSources->NewVertexDeclaration(FVF); }
