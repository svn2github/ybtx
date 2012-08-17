#include "stdafx.h"
#include "CDataSources.h"
#include "CPieceGroup.h"
#include "CSkeletalMesh.h"
#include "CStaticMesh.h"
#include "CVertexAniMesh.h"
#include "CCpuSkeletalMesh.h"
#include "CShadowBuffer.h"
#include "CGpuProgramMgr.h"
#include "CRenderFont.h"
#include "CAnimation.h"
#include "TSqrAllocator.inl"

//coder
#include "CModCoder.h"

static list<CFileFormatMgr*> st_FormatList;

#define INIT_FORMAT(class,format) \
	class::st_pFormat = new format(); \
	st_FormatList.push_back( class::st_pFormat ) \

#define RELEASE_FORMAT() \
	list<CFileFormatMgr*>::iterator it,eit = st_FormatList.end(); \
	for( it = st_FormatList.begin(); it!=eit; ++it )delete *it; \
	st_FormatList.clear(); \
		

CDataSources::CDataSources()
{
	INIT_FORMAT( CPieceGroup,CModFormat );

}

CDataSources::~CDataSources()
{
	RELEASE_FORMAT();
}

CPieceGroup*		CDataSources::NewPieceGroup(const string& szName, const wstring& Alias)
{ return new CPieceGroup(szName, Alias); }

CPiece*				CDataSources::NewPiece(CPieceGroup* pGrp)
{ return new CPiece(pGrp); }

CMesh*				CDataSources::NewVertexAniMesh(void)
{ return new CVertexAniMesh; }

CMesh*				CDataSources::NewStaticMesh(void)
{ return new CStaticMesh; }

CMesh*				CDataSources::NewSkeletalMesh(void)
{ return CGpuProgramMgr::GetInst()->GetShaderVer()<Shader_V_2?((CMesh*)new CCPUSkeletalMesh) : ((CMesh*)new CSkeletalMesh); }

CHardwareBuffer*	CDataSources::NewBuffer(size_t bufferSize)
{ return new ShadowBuffer(bufferSize); }

CVertexHardwareBuffer* CDataSources::NewVertexBuffer(size_t vertexSize, size_t numVerts, Usage usage,
			void* pInitData , bool useSysMem  , bool useShadowBuffer )
{ return NULL; }

CIndexHardwareBuffer*	CDataSources::NewIndexBuffer(size_t numIndexes, Usage usage,
			void* pInitData , bool useSysMem , bool useShadowBuffer )
{ return NULL; }


CAnimationGroup*	CDataSources::NewAnimationGroup(const string& szName, const wstring& Alias)
{ return new CAnimationGroup(szName,Alias); }

CAnimate*			CDataSources::NewAnimate(CAnimationGroup* pAniGrp,const string& szFileName, IDNAME& AniName)
{ 
	return new CAnimate(pAniGrp,szFileName, pAniGrp->GetAlias(),AniName); 
}

CSkeletalFrame*		CDataSources::NewSkeletalFrame(const string& szName, const wstring& Alias)
{ return new CSkeletalFrame(szName,Alias); }

CSkeletal*			CDataSources::NewSkeletal(void)
{ return new CSkeletal(); }

CSkeletalUnit*		CDataSources::NewSkeletalUnit(void)
{ return new CSkeletalUnit(); }

IResTexture*		CDataSources::NewTexture(const string& szName, const wstring& Alias)
{ return NULL; }

CRenderTexture*		CDataSources::NewRenderTexture(void)
{ return NULL; }

CRenderTexture*		CDataSources::NewDepthTexture(void)
{ return NULL; }

CWindowTarget*		CDataSources::NewWindow(const RenderSettings& settings)
{ return NULL; }

CPieceRenderStyle*	CDataSources::NewPieceRenderStyle(void)
{ return new CPieceRenderStyle(); }

CRenderFont*		CDataSources::NewRenderFont( const string& szName, const wstring& Alias)
{ return new CRenderFont(szName,Alias); }

CRenderStyle*		CDataSources::NewRenderStyle(const string& szName, const wstring& Alias)
{ return new CRenderStyle(szName,Alias); }

CVertexDeclaration* CDataSources::NewVertexDeclaration(DWORD FVF)
{ return NULL; }

CTexture2DImp* sqr::CDataSources::NewTex2DImp( const ITexture* pParent )
{
	return 0;
}