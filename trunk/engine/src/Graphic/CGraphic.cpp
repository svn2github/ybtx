#include "stdafx.h"
#include "CMath.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "CPostProcessManager.h"
#include "CAnimationGroup.h"
#include "CSkeletalFrame.h"
#include "CVertexDeclaration.h"
#include "CRenderGroup.h"
#include "CRenderFont.h"
#include "CPkgFile.h"
#include "CRenderStyle.h"
#include "CHardwareCapacity.h" 
#include "SyncSystem.h"
#include "CRefractFilter.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"
#include "CTextureQualityMgr.h"
#include "CHardwareBufferMgr.h"
#include "CSkeletalsMgr.h"
#include "CRenderPipeline.h"
#include "CDataSources.h"
#include "CFontMgr.h"
#include "CTextureFontMgr.h"
#include "CFileFormatMgr.h"
#include "CTexFmtMapper.h"
#include "CColor4.inl"

#include "Graphic_Dx9/CDx9DataSources.h"

//#define ENABLE_DEBUG_HELPER
#ifdef ENABLE_DEBUG_HELPER
#	include "CDebugHelper.h"
#endif

#define		GpuConfigPath		"Shader/"
#define		GpuConfigFile		"Config.xml"

#define		DYNAMIC_VB_SIZE		0x100000

CGraphic::CGraphic(ECreateType eCT)
: m_CreateType(eCT)
{
	switch(eCT)
	{
	case eRCT_DX9:
		new CDx9DataSources;
		break;
	case eRCT_DX10:
	case eRCT_DX11:
	case eRCT_OPENGL:
	case eRCT_OTHER:
		new CDataSources;
		break;
	}

	CMath::buildTrigTables(4096,true);
	GraphicErr::InitializeGraphicErr();

	m_pActiveTarget		= NULL;
	m_uFrameCount		= 0;
	m_pSetEffect		= NULL;
	m_pGetEffect		= NULL;
	m_pSetCameraShake	= NULL;
	m_pGetCameraShake	= NULL;

	m_RS[RS_DEFAULT1] = DEFAULT1RS();
	m_RS[RS_PONITSAMPLE] = PONITSAMPLERS();
	m_RS[RS_FONT] = FONTRS();
	m_RS[RS_GUI] = GUIRS();
	m_RS[RS_GUI_COLORADD] = GUI_COLORADDRS();
	m_RS[RS_MAP1L] = MAP1LRS();
	m_RS[RS_MAP2L] = MAP2LRS();
	m_RS[RS_MAP4L] = MAP4LRS();
	m_RS[RS_SHADOW] = SHADOWRS();
	m_RS[RS_ALPHA2GRAY] = ALPHA2GRAYRS();
	m_RS[RS_NONETEXTALPHA] = NONETEXTALPHA();
	m_RS[RS_NONETEXTALPHALINER] = NONETEXTALPHALINEAR();
	m_RS[RS_GUI_MIRROR] = GUIMIRRORRS();
	m_RS[RS_GUI_SHADOW] = GUISHADOWRS();
	m_RS[RS_GUI_ALPHA] = GUIALPHA();
	m_RS[RS_GUI_ALPHABLEND] = GUIALPHABLEND();
	m_RS[RS_GUI_BIGMAP] = GUIBIGMAP();
	m_RS[RS_EMT] = EMTRS();
	m_RS[RS_LINE] = LINERS();
	m_pActiveVertexBuffer = &m_DynamicVB;
	CSyncSystem::Create();
	CFontMgr::Create();
	CTextureFontMgr::Create();
	CTextureQualityMgr::Create();
	CHardwareBufferMgr::Create();
	m_pDynamicPool = new CDynamicPool();

#ifdef ENABLE_DEBUG_HELPER
	CDebugHelper::Create();
	//CDebugHelper::GetInst()->EatMemory(1500);
	CDebugHelper::GetInst()->EatMemoryUntil(500);
#endif
}

CGraphic::~CGraphic()
{
#ifdef ENABLE_DEBUG_HELPER
	if ( CDebugHelper::HasInst() )
		CDebugHelper::Destroy();
#endif

	SafeRelCtn1(m_depthTexList);
	CTextureQualityMgr::Destroy();
	if (CPostProcessManager::HasInst())
		CPostProcessManager::Destroy();
	
	if (CSkeletalsMgr::HasInst())
		CSkeletalsMgr::Destroy();

	if (CFontMgr::HasInst())
	{
		CFontMgr::GetInst()->UnloadGdiFonts();
		CFontMgr::Destroy();
	}

	if (CTextureFontMgr::HasInst())
	{
		CTextureFontMgr::Destroy();
	}

	m_DynamicVB.Destroy();
	m_EffectNpcVB.Destroy();
	m_EffectSceneVB.Destroy();
	m_TempVB.Destroy();
	GfkSafeDelete(m_pDynamicPool);
	m_ResMgr.Clear();
	
	for(int i = 0; i<esTT_COUNT; ++i )
		SafeRelease(m_pSysTexture[i]);

	for ( GraphicManageTexList::iterator it = m_gfkManageTexList.begin(); it!=m_gfkManageTexList.end(); ++it )
		SafeRelease(*it);
	m_gfkManageTexList.clear();

	CVertexDeclaration::RELEASE_ALL_VERDECL();
	CSyncSystem::Destroy();
	CDataSources::Destroy();
	CMainWindowTarget::Destroy();
	CHardwareBufferMgr::Destroy();
	CGpuProgramMgr::Destroy();
	CRenderPipeline::Destroy();
	CTexFmtMapper::Destroy();

	if ( m_RenderWindowList.size() > 0 )
		OutputDebugString( "存在RenderTarget没有被释放\n" );
}

bool CGraphic::Initialize(void)
{
	CVertexDeclaration* pVD;

	//VerPos顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerPos::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);;

	//VerTex2顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerTex2::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,1);
	
	//VerTex顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerTex::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	//VerColor3D顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerColor3D::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_COLOUR,VES_DIFFUSE);

	//VerColorSize顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerColorSize::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_COLOUR,VES_DIFFUSE);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT1,VES_PSIZE);

	//Vertex3D顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(Vertex3D::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_COLOUR,VES_DIFFUSE);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	//VerColorTex1顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerColorTex1::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_COLOUR,VES_DIFFUSE);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,1);

	//VerNCST顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNCST::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_SPECULAR);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	//VerNCST1顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNCST1::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_SPECULAR);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,1);

	////VerTerrain顶点格式FVF 顶点描述化
	//pVD = CDataSources::GetInst()->NewVertexDeclaration();
	//pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	//pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	//pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);
	//pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_SPECULAR);
	//pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	//pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,1);
	//pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,2);
	//pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,3);

	//VerNorTex顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNorTex::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	//VerNorTex顶点格式FVF 顶点描述化 // 加入的了顶点颜色了 2009/3/23
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNorDiffuseTex::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT3,VES_DIFFUSE,0);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	//VerNorTex1顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNorTex1::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,1);

	//VerNorTex2顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNorTex2::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,1);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,2);

	//VerNorTex2顶点格式FVF + 洪培 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNorTex2::Format| CVertexElement::GetBaseFormatID(1,VES_DIFFUSE,0));
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,1);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,2);
	pVD->AddElement(1,0,VET_COLOUR,VES_DIFFUSE);


	//VerNorDif顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNorDif::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

	//VerNCT顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNCT::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	//VerNor顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerNor::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);

	//VerText2D顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerText2D::Format);
	pVD->AddElement(0,0,VET_FLOAT4,VES_POSITIONT);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	//Vertex2D顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(Vertex2D::Format);
	pVD->AddElement(0,0,VET_FLOAT4,VES_POSITIONT);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	//Vertex2DAlpha顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(Vertex2DAlpha::Format);
	pVD->AddElement(0,0,VET_FLOAT4,VES_POSITIONT);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,1);

	//VerColor2D顶点格式FVF 顶点描述化
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VerColor2D::Format);
	pVD->AddElement(0,0,VET_FLOAT4,VES_POSITIONT);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

	//VertexSkeletalFormat顶点描述
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VertexSkeletalFormat::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,1);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT2,VES_TEXTURE_COORDINATES,2);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_FLOAT4,VES_BLEND_WEIGHTS);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_UBYTE4,VES_BLEND_INDICES);

	//VertexAniFormat::Tex1顶点描述
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VertexAniFormat::Tex1::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(1,0,VET_FLOAT3,VES_POSITION,1);
	pVD->AddElement(1,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL,1);
	pVD->AddElement(2,0,VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	//VertexAniFormat::Tex2顶点描述
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VertexAniFormat::Tex2::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(1,0,VET_FLOAT3,VES_POSITION,1);
	pVD->AddElement(1,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL,1);
	pVD->AddElement(2,0,VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(3,0,VET_FLOAT2,VES_TEXTURE_COORDINATES,1);

	//VertexAniFormat::Tex3顶点描述
	pVD = CDataSources::GetInst()->NewVertexDeclaration(VertexAniFormat::Tex3::Format);
	pVD->AddElement(0,0,VET_FLOAT3,VES_POSITION,0);
	pVD->AddElement(0,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL,0);
	pVD->AddElement(1,0,VET_FLOAT3,VES_POSITION,1);
	pVD->AddElement(1,sizeof(CVector3f),VET_FLOAT3,VES_NORMAL,1);
	pVD->AddElement(2,0,VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	pVD->AddElement(3,0,VET_FLOAT2,VES_TEXTURE_COORDINATES,1);
	pVD->AddElement(4,0,VET_FLOAT2,VES_TEXTURE_COORDINATES,2);

	if ( FAILED( m_DynamicVB.Create( DYNAMIC_VB_SIZE/2, DYNAMIC_VB_SIZE/4, 256 ) ) )
		return false;
	if ( FAILED( m_EffectNpcVB.Create( DYNAMIC_VB_SIZE/2, DYNAMIC_VB_SIZE/4, 1024 ) ) )
		return false;
	if ( FAILED( m_EffectSceneVB.Create( DYNAMIC_VB_SIZE/2, DYNAMIC_VB_SIZE/4, 1024 ) ) )
		return false;
	if ( FAILED( m_TempVB.Create( DYNAMIC_VB_SIZE/4, DYNAMIC_VB_SIZE/8 ) ) )
		return false;
	CPostProcessManager::Create();

	if(!CSkeletalsMgr::HasInst())
		CSkeletalsMgr::Create();
	m_pActiveTarget = CMainWindowTarget::GetInst();


	//CreateDefautTexture
	uint32	uTexData = 0;
	if ( FAILED(CGraphic::GetInst()->CreateEmptyTexture(1, 1, 1, TF_ARGB32, &m_pSysTexture[eSTT_BLACK])) )
		GfkLogErrOnce(GraphicErr::GetErrTypeStr(RenderRuntime_Err), "创建黑色纹理失败");
	else
	{
		uTexData = CColor4::Black.MakeDWordSafe();
		m_pSysTexture[eSTT_BLACK]->FillFromMemory(&uTexData, TF_ARGB32, 1, CIRect(0,0,1,1), CIRect(0,0,1,1));	
	}

	if ( FAILED(CGraphic::GetInst()->CreateEmptyTexture(1, 1, 1, TF_ARGB32, &m_pSysTexture[eSTT_GRAY])) )
		GfkLogErrOnce(GraphicErr::GetErrTypeStr(RenderRuntime_Err), "创建灰色纹理失败");
	else
	{
		uTexData = CColor4::Gray.MakeDWordSafe();
		m_pSysTexture[eSTT_GRAY]->FillFromMemory(&uTexData, TF_ARGB32, 1, CIRect(0,0,1,1), CIRect(0,0,1,1));	
	}

	if ( FAILED(CGraphic::GetInst()->CreateEmptyTexture(1, 1, 1, TF_ARGB32, &m_pSysTexture[eSTT_WHITE])) )
		GfkLogErrOnce(GraphicErr::GetErrTypeStr(RenderRuntime_Err), "创建白色纹理失败");
	else
	{
		uTexData = CColor4::White.MakeDWordSafe();
		m_pSysTexture[eSTT_WHITE]->FillFromMemory(&uTexData, TF_ARGB32, 1, CIRect(0,0,1,1), CIRect(0,0,1,1));	
	}

	CRenderPipeline::GetInst()->_SetDefaultTexture(m_pSysTexture[eSTT_GRAY]);
	return true;
}


void CGraphic::ActiveRenderTarget(CRenderTarget* pTarget)
{
	if (pTarget!=NULL)
		m_pActiveTarget = pTarget;
	else
		m_pActiveTarget = CMainWindowTarget::GetInst();

	m_pActiveTarget->UpdateAspect();
}

bool CGraphic::DeleteWindowTarget(CWindowTarget* pTarget)
{
	TARGET_LIST::iterator it = m_RenderWindowList.find(pTarget);
	if (it!=m_RenderWindowList.end())
	{
		m_RenderWindowList.erase(it);
		return true;
	}
	return false;
}

void CGraphic::RenderGroup(void)
{
	m_pActiveTarget->Render();
}


bool CGraphic::IsGroupOptimiz(void)
{
	return m_pActiveTarget->GetGroup()->isRenderOptimize();
}


FilterNode*  CGraphic::Insert(FilterNode* fn)
{
	return m_pActiveTarget->GetGroup()->InsertRender(fn);
}

void  CGraphic::InsertGeometry(CGeometryInst* pInst, ERenderObjStyle eObjStyle)
{
	m_pActiveTarget->GetGroup()->InsertGeometry(pInst,eObjStyle);
}

void CGraphic::BeginModel(CModel* pModel)
{
	m_pActiveTarget->GetGroup()->Begin(pModel);
}

void CGraphic::EndModel(void)
{
	m_pActiveTarget->GetGroup()->End();
}


void CGraphic::SetEffectLevel(float level)
{
	//这个设计是不好的,但如果做的中庸我们可能永远都不会再想到这种不合理的存在
	//矛盾是，想把系统的策略数据封装在各自系统内部的同时（已完成）
	//又不想让Graphic和其他系统造成不必要的依赖 （待完成）
	//对外接口又只想有Graphic这一种接口（已完成）
	//解决方案已经有了，我们需要一个专门管理系统运行参数设置回调的机制
	//有时间就会做，暂时这样。不好的设计是因为时间太紧，大家多多忍耐
	if (NULL!=m_pSetEffect)
		(*m_pSetEffect)(level);
}

float CGraphic::GetEffectLevel(void)
{
	if (NULL!=m_pGetEffect)
		return (*m_pGetEffect)();
	return 1.0f;
}

void CGraphic::SetEffectCallBack(SetFun sfun , GetFun gfun)
{
	m_pSetEffect = sfun;
	m_pGetEffect = gfun;
}

void CGraphic::SetCameraShake(bool b)
{
	if (NULL!=m_pSetCameraShake)
		(*m_pSetCameraShake)(b);
}

bool CGraphic::GetCameraShake(void)
{
	if (NULL!=m_pGetCameraShake)
		return (*m_pGetCameraShake)();
	return true;
}

void CGraphic::SetCameraShakeCallBack( SetboolFun sfun , GetboolFun gfun )
{
	m_pSetCameraShake = sfun;
	m_pGetCameraShake = gfun;
}

//void CGraphic::GetPrimitiveInfo( uint32& uPrimitiveCount, uint32& uPrimitiveGroup )
//{
//	uPrimitiveCount = m_pRenderPipeline->GetPrimitiveCount();
//	uPrimitiveGroup = m_pRenderPipeline->GetFaceCount();
//}


//void CGraphic::SetZDelta( float Z )
//{
//	m_pRenderPipeline->SetZDelta( Z );
//}

//void CGraphic::SetAmbient( uint32 Color )
//{
//	m_pRenderPipeline->SetAmbient(Color);
//}

//void CGraphic::SetTextureMatrix( uint32 uTextStage, const CMatrix* pMatrix )
//{
//	if ( uTextStage < 8 )
//		m_pRenderPipeline->SetTextureMatrix( uTextStage, pMatrix );
//}

//void CGraphic::ResetPipeline()
//{
//	m_pRenderPipeline->InitPipeline();
//}
/*
void CGraphic::Render()
{
	SQR_TRY
	{
		if(RenderBegin())
		{
			RenderEnd();
			return;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CGraphic::Render");
		GfkLogExp(exp);
	}
	SQR_TRY_END;

	m_pDynamicPool->ResetPool();
	m_TempVB.ResetStack();
	m_DynamicVB.ResetStack();
	m_EffectNpcVB.ResetStack();
	m_EffectSceneVB.ResetStack();
}
*/

void CGraphic::UpdateCamera(void) //过渡期函数，现在新老结构交替时使用
{
	CRenderPipeline::GetInst()->SetCamera(m_pActiveTarget->GetCamera());
}

void CGraphic::UpdateRenderTarget(void)
{
	CIRect Rect;
	TARGET_LIST::iterator it,eit = m_RenderWindowList.end();
	for (it=m_RenderWindowList.begin();it!=eit;++it)
	{
		if ((*it)->IsAutoRender())
		{
			m_pActiveTarget = *it;
			(*it)->Render();
			DrawStack();
		}
	}
	m_pActiveTarget = CMainWindowTarget::GetInst();
}

//////////////////////////////////////////////////////////////////////////
//功能：清空堆栈
void CGraphic::ClearStack()
{
	m_pDynamicPool->ResetPool();
	m_TempVB.ResetStack();
	m_DynamicVB.ResetStack();
	m_EffectNpcVB.ResetStack();
	m_EffectSceneVB.ResetStack();
}

void CGraphic::DrawStack()
{
	m_DynamicVB.Render();
}

//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
// 功能：渲染
// 参数：D3DPRIMITIVETYPE PrimitiveType,
//	     uint32 VertexNum,
//	     uint32 PrimitiveCount,
//	     uint32 VerFormat,
//	     uint32 VerStride,
//	     void** ppVB,
//	     void** ppIB
// 返回：要填充的渲染矩阵
//////////////////////////////////////////////////////////////////
CMatrix* CGraphic::GetRenderStack( ERenderStyle RenderStyle,
									ITexture* Text0,
									ITexture* Text1,
									EPrimitiveType PrimitiveType,
									uint32 VertexNum,
									uint32 PrimitiveCount,
									uint32 VerFormat,
									uint32 VerStride,
									void** ppVB,
									void** ppIB,
									void** ppRS,
									DWORD ShaderID
									)
{
	RenderParam& RP = LockRenderStack( PrimitiveType, VertexNum, PrimitiveCount, VerFormat, VerStride, ppVB, ppIB );
	RP.m_RS = GetStaticRS( RenderStyle );
	RP.m_RS.m_Texture_S0 = Text0;// ? Text0->GetBuffer() : NULL; //这里设置dx纹理对象
	RP.m_RS.m_Texture_S1 = Text1;// ? Text1->GetBuffer() : NULL;
	RP.m_RS.m_Texture_S2 = NULL;
	RP.m_RS.m_Texture_S3 = NULL;
	if (RP.m_RS.m_ShaderType.IsNull())RP.m_RS.m_ShaderType = ShaderID;
	if ( ppRS )
		*ppRS = &RP.m_RS;

	UnlockRenderStack( RP );

	return ( CMatrix* )&RP.m_Mat;
}


CMatrix* CGraphic::GetRenderStack( ERenderStyle RenderStyle,
									ERenderObjStyle RenderObjStyle,
									ITexture* Text0,
									ITexture* Text1,
									EPrimitiveType PrimitiveType,
									uint32 VertexNum,
									uint32 PrimitiveCount,
									uint32 VerFormat,
									uint32 VerStride,
									void** ppVB,
									void** ppIB,
									void** ppRS,
									DWORD ShaderID
									)
{
	if(RenderObjStyle == ROS_REFRACT)
	{
		CRefractFilter* refractFilter = (CRefractFilter*)(CMainWindowTarget::GetInst()->GetGroup()->GetBatch(ID_REFRACT_FILTER));
		return refractFilter->GetRenderStack(RenderStyle, RenderObjStyle,
			Text0, Text1, PrimitiveType, VertexNum, PrimitiveCount,
			VerFormat, VerStride, ppVB, ppIB, ppRS, ShaderID);
	}

	CDynamicVB* dynamicVB = NULL;
	if ((RenderObjStyle&ROS_LIGHTMAT1))
		dynamicVB = &m_EffectSceneVB;
	else
		dynamicVB = &m_EffectNpcVB;
	RenderParam& RP = dynamicVB->GetRenderStack( PrimitiveType, VertexNum, PrimitiveCount, VerFormat, VerStride, ppVB, ppIB );
	RP.m_RS = GetStaticRS( RenderStyle );
	RP.m_RS.m_Texture_S0 = Text0;// ? Text0->GetBuffer() : NULL; //这里设置dx纹理对象
	RP.m_RS.m_Texture_S1 = Text1;// ? Text1->GetBuffer() : NULL;
	RP.m_RS.m_Texture_S2 = NULL;
	RP.m_RS.m_Texture_S3 = NULL;
	if (RP.m_RS.m_ShaderType.IsNull())	RP.m_RS.m_ShaderType = ShaderID;
	if ( ppRS )
		*ppRS = &RP.m_RS;

	UnlockRenderStack( RP );
	return ( CMatrix* )&RP.m_Mat;
}
//
//void  CGraphic::OptimizeDP(ERenderObjStyle RenderObjStyle)
//{
//	if(RenderObjStyle == ROS_REFRACT)
//		return;
//	
//	if ((RenderObjStyle&ROS_LIGHTMAT1))
//		m_EffectSceneVB.OptimizeDP();
//	else
//		m_EffectNpcVB.OptimizeDP();
//}

//////////////////////////////////////////////////////////////////////////

//3 layer
CMatrix* CGraphic::GetRenderThreeLayerStack( ERenderStyle RenderStyle,
											  ITexture* Text0,
											  ITexture* Text1,
											  ITexture* Text2,
											  EPrimitiveType PrimitiveType,
											  uint32 VertexNum,
											  uint32 PrimitiveCount,
											  uint32 VerFormat,
											  uint32 VerStride,
											  void** ppVB,
											  void** ppIB,
											  void** ppRS,
											  DWORD ShaderID )
{
	RenderParam& RP = LockRenderStack( PrimitiveType, VertexNum, PrimitiveCount, VerFormat, VerStride, ppVB, ppIB );
	RP.m_RS = GetStaticRS( RenderStyle );
	RP.m_RS.m_Texture_S0 = Text0;// ? (LPDIRECT3DTEXTURE9)Text0->GetBuffer() : NULL; //这里设置dx纹理对象
	RP.m_RS.m_Texture_S1 = Text1;// ? (LPDIRECT3DTEXTURE9)Text1->GetBuffer() : NULL;
	RP.m_RS.m_Texture_S2 = Text2;// ? (LPDIRECT3DTEXTURE9)Text2->GetBuffer() : NULL;
	RP.m_RS.m_Texture_S3 = NULL;
	if (RP.m_RS.m_ShaderType.IsNull())	RP.m_RS.m_ShaderType = ShaderID;
	if ( ppRS )
		*ppRS = &RP.m_RS;
	UnlockRenderStack( RP );

	return ( CMatrix* )&RP.m_Mat;
}

//4 layer
CMatrix* CGraphic::GetRenderFourLayerStack( ERenderStyle RenderStyle,
											 ITexture* Text0,
											 ITexture* Text1,
											 ITexture* Text2,
											 ITexture* Text3,
											 EPrimitiveType PrimitiveType,
											 uint32 VertexNum,
											 uint32 PrimitiveCount,
											 uint32 VerFormat,
											 uint32 VerStride,
											 void** ppVB,
											 void** ppIB,
											 void** ppRS,
											 DWORD ShaderID )
{
	RenderParam& RP = LockRenderStack( PrimitiveType, VertexNum, PrimitiveCount, VerFormat, VerStride, ppVB, ppIB );
	RP.m_RS = GetStaticRS( RenderStyle );
	RP.m_RS.m_Texture_S0 = Text0;// ? (LPDIRECT3DTEXTURE9)Text0->GetBuffer()  : NULL; //这里设置dx纹理对象
	RP.m_RS.m_Texture_S1 = Text1;// ? (LPDIRECT3DTEXTURE9)Text1->GetBuffer()  : NULL;
	RP.m_RS.m_Texture_S2 = Text2;// ? (LPDIRECT3DTEXTURE9)Text2->GetBuffer()  : NULL;
	RP.m_RS.m_Texture_S3 = Text3;// ? (LPDIRECT3DTEXTURE9)Text3->GetBuffer()  : NULL;
	if (RP.m_RS.m_ShaderType.IsNull())	RP.m_RS.m_ShaderType = ShaderID;
	if ( ppRS )
		*ppRS = &RP.m_RS;
	UnlockRenderStack( RP );

	return ( CMatrix* )&RP.m_Mat;
}



void CGraphic::DrawRect( const CFRect& rt, uint32 Color, ITexture* pTexture, const CFRect* rtText, ERenderStyle RS, float z )
{
	if ( Color == 0 )
		return;

	float x1 = (float)rt.left;
	float y1 = (float)rt.top;
	float x2 = (float)rt.right;
	float y2 = (float)rt.bottom;

	if ( pTexture )
	{
		Vertex2D* pVB;
		GetRenderStack( RS, pTexture, NULL, PT_TRIANGLESTRIP, 4, 2,
			Vertex2D::Format, sizeof(Vertex2D), (void**)&pVB, NULL );

		float u1 = 0.00001f;
		float v1 = 0.00001f;
		float u2 = 1.00001f;
		float v2 = 1.00001f;
		if ( rtText )
		{
			u1 = ( rtText->left + 0.25f )/pTexture->GetWidth();
			v1 = ( rtText->top + 0.25f )/pTexture->GetHeight();
			u2 = ( rtText->right + 0.25f )/pTexture->GetWidth();
			v2 = ( rtText->bottom + 0.25f )/pTexture->GetHeight();
		}

		pVB[0] = Vertex2D( x1, y2, z, Color, u1, v2 ); //对点写数据
		pVB[1] = Vertex2D( x1, y1, z, Color, u1, v1 );
		pVB[2] = Vertex2D( x2, y2, z, Color, u2, v2 );
		pVB[3] = Vertex2D( x2, y1, z, Color, u2, v1 );
	}
	else
	{
		VerColor2D* pVB;
		GetRenderStack( RS, NULL, NULL, PT_TRIANGLESTRIP, 4, 2,
			VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL );

		pVB[0] = VerColor2D( x1, y2, z, Color );
		pVB[1] = VerColor2D( x1, y1, z, Color );
		pVB[2] = VerColor2D( x2, y2, z, Color );
		pVB[3] = VerColor2D( x2, y1, z, Color );
	}
}

void CGraphic::DrawRect( const CFRect& rt, uint32 Color, ITexture* pTexture, const CPosRect& rtText, ERenderStyle RS, float z )
{
	if ( Color == 0 )
		return;
	float x1 = rt.left;
	float y1 = rt.top;
	float x2 = rt.right;
	float y2 = rt.bottom;

	if ( pTexture )
	{
		Vertex2D* pVB;
		GetRenderStack( RS, pTexture, NULL, PT_TRIANGLESTRIP, 4, 2,
			Vertex2D::Format, sizeof(Vertex2D), (void**)&pVB, NULL );

		float uLT = ( rtText.ptLT.x + 0.25f )/pTexture->GetWidth();
		float vLT = ( rtText.ptLT.y + 0.25f )/pTexture->GetHeight();
		float uRT = ( rtText.ptRT.x + 0.25f )/pTexture->GetWidth();
		float vRT = ( rtText.ptRT.y + 0.25f )/pTexture->GetHeight();
		float uLB = ( rtText.ptLB.x + 0.25f )/pTexture->GetWidth();
		float vLB = ( rtText.ptLB.y + 0.25f )/pTexture->GetHeight();
		float uRB = ( rtText.ptRB.x + 0.25f )/pTexture->GetWidth();
		float vRB = ( rtText.ptRB.y + 0.25f )/pTexture->GetHeight();

		pVB[0] = Vertex2D( x1, y2, z, Color, uLB, vLB );
		pVB[1] = Vertex2D( x1, y1, z, Color, uLT, vLT );
		pVB[2] = Vertex2D( x2, y2, z, Color, uRB, vRB );
		pVB[3] = Vertex2D( x2, y1, z, Color, uRT, vRT );
	}
	else
	{
		VerColor2D* pVB;
		GetRenderStack( RS, NULL, NULL, PT_TRIANGLESTRIP, 4, 2,
			VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL );

		pVB[0] = VerColor2D( x1, y2, z, Color );
		pVB[1] = VerColor2D( x1, y1, z, Color );
		pVB[2] = VerColor2D( x2, y2, z, Color );
		pVB[3] = VerColor2D( x2, y1, z, Color );
	}
}

void CGraphic::DrawSkyBoard( const CFRect& rt, uint32 Color, ITexture* pTexture, const float& uoffset, const float& voffset, ERenderStyle rs, float z )
{
	if ( Color == 0 )
		return;

	float x1 = (float)rt.left;
	float y1 = (float)rt.top;
	float x2 = (float)rt.right;
	float y2 = (float)rt.bottom;

	if ( pTexture )
	{
		Vertex2D* pVB;
		GetRenderStack( rs, pTexture, NULL, PT_TRIANGLESTRIP, 4, 2,
			Vertex2D::Format, sizeof(Vertex2D), (void**)&pVB, NULL );

		float u1 = 0.00001f+uoffset;
		float v1 = 0.00001f+voffset;
		float u2 = 1.00001f+uoffset;
		float v2 = 1.00001f+voffset;

		pVB[0] = Vertex2D( x1, y2, z, Color, u1, v2 ); //对点写数据
		pVB[1] = Vertex2D( x1, y1, z, Color, u1, v1 );
		pVB[2] = Vertex2D( x2, y2, z, Color, u2, v2 );
		pVB[3] = Vertex2D( x2, y1, z, Color, u2, v1 );
	}
}

void CGraphic::Draw3DPlane(ITexture* pTexture, CVector3f *vec3d, DWORD Color, ERenderStyle RS, CVector2f *tex_uv)
{
	if ( pTexture )
	{
		VerColorTex vs[4] =
		{
			VerColorTex( vec3d[0].x, vec3d[0].y, vec3d[0].z, Color, tex_uv[0].x, tex_uv[0].y),
			VerColorTex( vec3d[1].x, vec3d[1].y, vec3d[1].z, Color, tex_uv[1].x, tex_uv[1].y),
			VerColorTex( vec3d[2].x, vec3d[2].y, vec3d[2].z, Color, tex_uv[2].x, tex_uv[2].y),
			VerColorTex( vec3d[3].x, vec3d[3].y, vec3d[3].z, Color, tex_uv[3].x, tex_uv[3].y),
		};
		VerColorTex* pVB;
		RenderState * pRS;

		CMatrix view;
		CCamera *pCamera = m_pActiveTarget->GetCamera();
		CMatrix	matBillboard;
		view = pCamera->getViewMatrix();
		matBillboard.Identity();
		matBillboard._11 = view._11;
		matBillboard._12 = view._12;
		matBillboard._13 = view._13;
		matBillboard._21 = view._21;
		matBillboard._22 = view._22;
		matBillboard._23 = view._23;
		matBillboard._31 = view._31;
		matBillboard._32 = view._32;
		matBillboard._33 = view._33;
		matBillboard.Invert();

		CMatrix *m_Mat = GetRenderStack( RS, pTexture, NULL, PT_TRIANGLESTRIP, 4, 2,
			VerColorTex::Format , sizeof(VerColorTex), (void**)&pVB, NULL, (void**)&pRS );
		m_Mat->Identity();
		m_Mat->_11 = matBillboard._11;
		m_Mat->_12 = matBillboard._12;
		m_Mat->_13 = matBillboard._13;
		m_Mat->_21 = matBillboard._21;
		m_Mat->_22 = matBillboard._22;
		m_Mat->_23 = matBillboard._23;
		m_Mat->_31 = matBillboard._31;
		m_Mat->_32 = matBillboard._32;
		m_Mat->_33 = matBillboard._33;
		m_Mat->_41 = vec3d[4].x;
		m_Mat->_42 = vec3d[4].y;
		m_Mat->_43 = vec3d[4].z;

		pRS->m_TexOP_S0 = TOP_SELECTARG1;
		pRS->m_TexArg1_S0 = TA_TEXTURE;
		memcpy(pVB,vs,sizeof(vs));
	}

}

void CGraphic::DrawRectGroup( ITexture* pTexture, ERenderStyle RS, const RectGroup* rt, int nNum )
{
	static TRIAGLE_INDEX Index;
	if ( pTexture && pTexture->IsValid() )
	{
		float fInvWidth  = 1.0f/pTexture->GetWidth();
		float fInvHeight = 1.0f/pTexture->GetHeight();

		void* pIB;
		Vertex2D* pVB;
		int tempNum = nNum;
		do 
		{
			nNum = tempNum;
			if ( nNum == 1 )
				GetRenderStack( RS, pTexture, NULL, PT_TRIANGLESTRIP, 4*nNum, 2*nNum,
					Vertex2D::Format, sizeof(Vertex2D), (void**)&pVB, NULL );
			else
			{
				if ( nNum > MAX_TRI_INDEXNUM )
					nNum = MAX_TRI_INDEXNUM;
				GetRenderStack( RS, pTexture, NULL, PT_TRIANGLELIST, 4*nNum, 2*nNum,
					Vertex2D::Format, sizeof(Vertex2D), (void**)&pVB, &pIB );
				memcpy( pIB, Index, nNum*12 );
			}
			for ( int i = 0; i < nNum; i++ )
			{
				int n = i<<2;

				CVector2f vecText[4] =
				{
					CVector2f( 0.00001f, 1.00001f ), CVector2f( 0.00001f, 0.00001f ),
					CVector2f( 1.00001f, 1.00001f ), CVector2f( 1.00001f, 0.00001f )
				};

				if ( rt[i].bFullText == false )
				{
					vecText[0].x = ( rt[i].rtText[0].x + 0.25f )*fInvWidth;
					vecText[0].y = ( rt[i].rtText[0].y + 0.25f )*fInvHeight;
					vecText[1].x = ( rt[i].rtText[1].x + 0.25f )*fInvWidth;
					vecText[1].y = ( rt[i].rtText[1].y + 0.25f )*fInvHeight;
					vecText[2].x = ( rt[i].rtText[2].x + 0.25f )*fInvWidth;
					vecText[2].y = ( rt[i].rtText[2].y + 0.25f )*fInvHeight;
					vecText[3].x = ( rt[i].rtText[3].x + 0.25f )*fInvWidth;
					vecText[3].y = ( rt[i].rtText[3].y + 0.25f )*fInvHeight;
				}
				pVB[ n + 0 ].Set( rt[i].vec[0].x, rt[i].vec[0].y, rt[i].z, rt[i].Color, vecText[0].x, vecText[0].y );
				pVB[ n + 1 ].Set( rt[i].vec[1].x, rt[i].vec[1].y, rt[i].z, rt[i].Color, vecText[1].x, vecText[1].y );
				pVB[ n + 2 ].Set( rt[i].vec[2].x, rt[i].vec[2].y, rt[i].z, rt[i].Color, vecText[2].x, vecText[2].y );
				pVB[ n + 3 ].Set( rt[i].vec[3].x, rt[i].vec[3].y, rt[i].z, rt[i].Color, vecText[3].x, vecText[3].y );
			}

			tempNum -= nNum;
			rt += nNum;
		} while (tempNum > 0);
	}
	else
	{
		VerColor2D* pVB;
		void* pIB;
		int tempNum = nNum;
		do 
		{
			nNum = tempNum;
			if ( nNum == 1 )
				GetRenderStack( RS, NULL, NULL, PT_TRIANGLESTRIP, 4*nNum, 2*nNum,
				VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL );
			else
			{
				if ( nNum > MAX_TRI_INDEXNUM )
					nNum = MAX_TRI_INDEXNUM;
				GetRenderStack( RS, NULL, NULL, PT_TRIANGLELIST, 4*nNum, 2*nNum,
					VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, &pIB );
				memcpy( pIB, Index, nNum*12 );
			}

			for ( int i = 0; i < nNum; i++ )
			{
				int n = i<<2;
				pVB[ n + 0 ].Set( rt[i].vec[0].x, rt[i].vec[0].y, rt[i].z, rt[i].Color );
				pVB[ n + 1 ].Set( rt[i].vec[1].x, rt[i].vec[1].y, rt[i].z, rt[i].Color );
				pVB[ n + 2 ].Set( rt[i].vec[2].x, rt[i].vec[2].y, rt[i].z, rt[i].Color );
				pVB[ n + 3 ].Set( rt[i].vec[3].x, rt[i].vec[3].y, rt[i].z, rt[i].Color );
			}
			tempNum -= nNum;
			rt += nNum;
		} while (tempNum > 0);
		
	}
}



void CGraphic::DrawLine( const PointGroup* pPointGroup, int nNum, bool b2D, ERenderStyle RS )
{
	if ( nNum < 1 || !pPointGroup )
		return;

	if ( b2D )
	{
		VerColor2D* pVB;
		GetRenderStack( RS, NULL, NULL, PT_LINESTRIP, nNum, nNum-1,
			VerColor2D::Format, sizeof(VerColor2D), (void**)&pVB, NULL );

		for ( int i = 0; i < nNum; i++ )
		{
			pVB[i].Set( pPointGroup[i].vPoint.x, pPointGroup[i].vPoint.y, pPointGroup[i].vPoint.z, pPointGroup[i].uColor );
		}
	}

	else
	{
		VerColor3D* pVB;
		GetRenderStack( RS, NULL, NULL, PT_LINESTRIP, nNum, nNum-1,
			VerColor3D::Format, sizeof(VerColor3D), (void**)&pVB, NULL )->Identity();
		memcpy( pVB, pPointGroup, sizeof(PointGroup)*nNum );
	}
}

//------------------------------------------------------------------------------
void CGraphic::DrawPrimitive( EPrimitiveType type, const VerColor3D* points, int32 numPoints )
{
	Ast(NULL != points);
	Ast(numPoints > 0);
	int numPrimitives = 0;
	switch (type)
	{
	case PT_POINTLIST:
		numPrimitives = numPoints;
		break;
	case PT_LINELIST:
		numPrimitives = numPoints / 2;
		break;
	case PT_LINESTRIP:
		numPrimitives = numPoints - 1;
		break;
	case PT_TRIANGLELIST:
		numPrimitives = numPoints / 3;
		break;
	case PT_TRIANGLESTRIP:
	case PT_TRIANGLEFAN:
		numPrimitives = numPoints - 2;
		break;
	default:
		return;
	}
	VerColor2D* vertexBuffer = NULL;
	RenderState* renderState = NULL;
	this->GetRenderStack(RS_LINE,ROS_LIGHTMAT1, NULL, NULL, type, numPoints, numPrimitives, VerColor3D::Format,
		sizeof(VerColor3D), (void**)&vertexBuffer, NULL, (void**)&renderState)->Identity();
	renderState->m_LightEnable = 0;
	renderState->m_ZTestEnable = TRUE;
	renderState->m_ZTestFun  =CMP_LESSEQUAL;
	renderState->m_Cull = CULLT_NONE;
	memcpy(vertexBuffer, points, numPoints * sizeof(PointGroup));
}

uint32 CGraphic::GetMaxSystemBuf()
{
	return m_ResMgr.SystemMemRecycle()->MaxBufSize();
}

uint32 CGraphic::GetCurSystemBuf()
{
	return m_ResMgr.SystemMemRecycle()->CurBufSize();
}

uint32 CGraphic::GetMaxVideoBuf()
{
	return m_ResMgr.VideoMemRecycle()->MaxBufSize();
}

uint32 CGraphic::GetCurVideoBuf()
{
	return m_ResMgr.VideoMemRecycle()->CurBufSize();
}

bool CGraphic::DrawImmediate()
{
	m_pActiveVertexBuffer->Render();
	return true;
}

bool CGraphic::DestroyDepthTexture(CRenderTexture* tex)
{
	DepthTextureList::iterator it = m_depthTexList.find(tex);
	if (it != m_depthTexList.end())
	{
		m_depthTexList.erase(it);
		SafeRelease(tex);
		return true;
	}
	return false;
}

HRESULT CGraphic::CreatePieceGroup( const char* szName, CPieceGroup** pPieceGroup )
{
	if ( szName == 0 || szName[0] == 0 )
	{
		*pPieceGroup = NULL;
		return GERROR;
	}

	*pPieceGroup = (CPieceGroup*)m_ResMgr.FindResInSystemMem( szName, eRT_eArp );
	if ( *pPieceGroup )
	{
		( (CPieceGroup*)(*pPieceGroup) )->Reclaim();
		return 0;
	}

	CPieceGroup* Temp = NULL;
	Temp = CDataSources::GetInst()->NewPieceGroup( szName, PATH_ALIAS_RES );
	//if ( SUCCEEDED( CPieceGroup::FORMAT()->Load(szName,Temp) ) )

	if ( SUCCEEDED( Temp->BinLoad() ) )
	{
		*pPieceGroup = Temp;
		Temp->SetContainerAndRecycle(m_ResMgr.SystemMemContainer(), m_ResMgr.SystemMemRecycle());
		return 0;
	}

	Temp->Release();
	return GERROR;
}

HRESULT CGraphic::CreateAnimationGroup( const TCHAR* szName, CAnimationGroup** pAnimationGroup )
{
	if ( szName == 0 || szName[0] == 0 )
	{
		*pAnimationGroup = NULL;
		return GERROR;
	}

	*pAnimationGroup = (CAnimationGroup*)m_ResMgr.FindResInSystemMem( szName, eRT_eAgp );
	if ( *pAnimationGroup )
	{
		( (CAnimationGroup*)(*pAnimationGroup) )->Reclaim();
		return 0;
	}

	CAnimationGroup* Temp = CDataSources::GetInst()->NewAnimationGroup( szName, PATH_ALIAS_RES );
	if ( !Temp )
		return GERROR;

	if ( SUCCEEDED( Temp->BinLoad() ) )
	{		
		*pAnimationGroup = Temp;
		Temp->SetContainerAndRecycle(m_ResMgr.SystemMemContainer(), m_ResMgr.SystemMemRecycle());
		return 0;
	}

	Temp->Release();
	return GERROR;
}

HRESULT CGraphic::CreateFont( int32 id, bool bBold, float fSize, CRenderFont** pFont )
{
	stringstream ss;
	ss << id << "-" << (int)bBold << "-" << fSize;
	string szFontRes = ss.str();

	*pFont = (CRenderFont*)m_ResMgr.FindResInVedioMem( szFontRes, eRT_eFont );
	if ( *pFont )
	{
		( (CRenderFont*)(*pFont) )->Reclaim();
		return 0;
	}

	CRenderFont* Temp = CDataSources::GetInst()->NewRenderFont( szFontRes,PATH_ALIAS_RES );
	if ( !Temp )
		return GERROR;
	if ( SUCCEEDED( Temp->Create(id, bBold, fSize) ) )
	{
		*pFont = Temp;
		Temp->SetContainerAndRecycle(m_ResMgr.VideoMemContainer(), m_ResMgr.VideoMemRecycle());
		return 0;
	}

	Temp->Release();
	return GERROR;
};

void CGraphic::DeleteTexture(const char* szName )
{
	m_ResMgr.VideoMemContainer()->Erase(szName, eRT_eTex );
}

HRESULT CGraphic::CreateRenderStyle( const char* szName, CRenderStyle** pRS )
{
	*pRS = (CRenderStyle*)m_ResMgr.FindResInSystemMem( szName, eRT_eRs );
	if ( *pRS )
	{
		( (CRenderStyle*)(*pRS) )->Reclaim();
		return 0;
	}

	CRenderStyle* Temp = CDataSources::GetInst()->NewRenderStyle( szName,PATH_ALIAS_RES );
	
	if ( SUCCEEDED( Temp->BinLoad() ) )
	{
		*pRS = Temp;
		Temp->SetContainerAndRecycle(m_ResMgr.SystemMemContainer(), m_ResMgr.SystemMemRecycle());
		return 0;
	}
	Temp->Release();

	return GERROR;
};

void CGraphic::SwapBegin(void)
{
	m_pActiveVertexBuffer = &m_TempVB;
}

void CGraphic::SwapEnd(void)
{
	m_TempVB.Render();
	m_pActiveVertexBuffer = &m_DynamicVB;
}

void CGraphic::SetAsynSwitch(bool bEnable)
{
	CPkgFile::EnableGlobeAsyncRead(bEnable);
}

void CGraphic::EnableAsynRead(bool bEnable)
{
	CPkgFile::EnableAsyncRead(bEnable);
}

bool CGraphic::InitShader(const wstring& path, ShaderVer maxVer)
{
	m_strConfigShaderPath = path.c_str();
	m_strConfigShaderPath.append(L"/");

	bool result = false;
	ShaderVer sVer = Shader_V_0;
/*	if ( CHardwareCapacity::GetInst()->IsSupportPS(4, 0) )
	{
		sVer = Shader_V_4;
	}*/ 
	if ( CHardwareCapacity::GetInst()->IsSupportPS(3, 0) )
	{
		sVer = Shader_V_3;
	}
	else if ( CHardwareCapacity::GetInst()->IsSupportPS(2, 0) )
	{		
		sVer = Shader_V_2;
	}
/*	else if ( CHardwareCapacity::GetInst()->IsSupportPS(1, 0) )
	{		
		sVer = Shader_V_1;
	}*/	
	sVer = min( sVer , maxVer );

	if ( CHardwareCapacity::GetInst()->GetVendor() == CHardwareCapacity::VT_NVIDIA )
	{
		ShaderMacroMgr::GetInst()->DefineMacro("_NVIDIA", "");
	}
	else if ( CHardwareCapacity::GetInst()->GetVendor() == CHardwareCapacity::VT_ATI )
	{
		ShaderMacroMgr::GetInst()->DefineMacro("_ATI", "");
	}
	else if ( CHardwareCapacity::GetInst()->GetVendor() == CHardwareCapacity::VT_INTEL )
	{
		ShaderMacroMgr::GetInst()->DefineMacro("_INTEL", "");
	}

	result =  CGpuProgramMgr::GetInst()->load(utf16_to_utf8(m_strConfigShaderPath.c_str()),GpuConfigFile,sVer);
	CPostProcessManager::GetInst()->Setup();
	return result;
}

bool CGraphic::RefreshShader(void)
{
	bool result = CGpuProgramMgr::GetInst()->load(utf16_to_utf8(m_strConfigShaderPath.c_str()),GpuConfigFile,CGpuProgramMgr::GetInst()->GetShaderVer());
	CPostProcessManager::GetInst()->Setup();
	return result;
}

void CGraphic::ReloadTexture( const char* szName )
{
	IResTexture* pText = (IResTexture*)m_ResMgr.FindResInVedioMem( szName, eRT_eTex );
	if ( pText )
		pText->ReLoadPic();
}


//UINT CGraphic::GetAvailableVidMem(void)
//{
//	return CHardwareCapacity::GetInst()->GetVideoMem();
//}
//

CRenderTexture* CGraphic::CreateDepthTexture(uint width, uint height)
{
	CRenderTexture* pTexture = CDataSources::GetInst()->NewDepthTexture();
	pTexture->SetRect(CIRect(0,0,width,height));
	pTexture->SetUseDepthBuffer( false );
	pTexture->Create();
	pTexture->AddRef();
	m_depthTexList.insert(pTexture);
	return pTexture;
}

//------------------------------------------------------------------------------


CWindowTarget* CGraphic::CreateNewWindow(const RenderSettings& settings)
{
	CWindowTarget* pTarget;
	pTarget =  CDataSources::GetInst()->NewWindow(settings);//new DX9WindowTarget(settings);
	pTarget->AddRef();
	m_RenderWindowList.insert(pTarget);
	return pTarget;
}

CRenderTexture*	CGraphic::CreateRenderTarget(void)
{
	CRenderTexture* pTarget;
	pTarget =  CDataSources::GetInst()->NewRenderTexture();
	pTarget->AddRef();
	m_RenderWindowList.insert(pTarget);
	return pTarget;
}

HRESULT CGraphic::CreateTexture( int32 nWidth, int32 nHeight, eTextureFormat eFormat, ITexture** pText )
{
	*pText = NULL;
	IResTexture* Temp = CDataSources::GetInst()->NewTexture( "", L"" );
	if ( !Temp )
		return GERROR;
	if ( SUCCEEDED( Temp->CreateEmpty( nWidth, nHeight, 1, eFormat, IResTexture::ETT_OLD ) ) )
	{
		*pText = Temp;
		return 0;
	}

	Temp->Release();
	return GERROR;
}

HRESULT CGraphic::CreateEmptyTexture( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat, ITexture** pText )
{
	*pText = NULL;
	IResTexture* Temp = CDataSources::GetInst()->NewTexture( "", L"" );
	if ( !Temp )
		return GERROR;
	if ( SUCCEEDED( Temp->CreateEmpty( nWidth, nHeight, nLevel, eFormat, IResTexture::ETT_OLD ) ) )
	{
		*pText = Temp;
		return 0;
	}

	*pText = NULL;
	Temp->Release();
	return GERROR;
}

HRESULT CGraphic::CreateTexture( const wchar_t* alias, const char* szName, ITexture** pText )
{
	int codestate = 0;
	SQR_TRY
	{
		if ( szName == 0 || szName[0] == 0 || NULL == strstr(szName, ".") )
		{
			*pText = NULL;
			return GERROR;
		}
		codestate = 1;
		GString str = szName;
		size_t npos = 0;

		IResTexture* pRes = (IResTexture*)m_ResMgr.FindResInVedioMem( szName, eRT_eTex );
		*pText = pRes;
		if ( *pText )
		{
			pRes->Reclaim();
			return 0;
		}
		codestate = 2;
		IResTexture* Temp = CDataSources::GetInst()->NewTexture( szName, alias );

		if ( !Temp )
		{
			*pText = NULL;
			return GERROR;
		}

		codestate = 3;
		if ( SUCCEEDED( Temp->LoadPic() ) )
		{
			*pText = Temp;
			Temp->SetContainerAndRecycle(m_ResMgr.VideoMemContainer(), m_ResMgr.VideoMemRecycle());
			return 0;
		}

		codestate = 4;
		*pText = NULL;
		Temp->Release();
	}
	SQR_CATCH(exp)
	{
		*pText = NULL;
		stringstream errmsg;
		errmsg	<< "     error drop line: " << codestate  << "\n"
				<< "     texture name: " << szName		<< "\n"
				<< "     Texture pointer address:" << pText << "\n";
		exp.AppendMsg(errmsg.str());
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;

	return GERROR;
};

HRESULT CGraphic::CreateTexture( const wchar_t* alias, const char* szName, uint Width, uint Height, ITexture** pText
								  ,uint MinMap , eTextureFormat eFormat ,DWORD Filter ,DWORD MipFilter,bool UseResRecycle )
{
	if ( szName == 0 || szName[0] == 0 || NULL == strstr(szName, ".") )
	{
		*pText = NULL;
		return GERROR;
	}

	GString str = szName;
	size_t npos = 0;

	IResTexture* pRes = (IResTexture*)m_ResMgr.FindResInVedioMem( szName, eRT_eTex );
	*pText = pRes;
	if ( *pText )
	{
		pRes->Reclaim();
		return 0;
	}
	IResTexture* Temp = CDataSources::GetInst()->NewTexture( szName,alias );

	if ( SUCCEEDED( Temp->LoadPic( Width, Height, MinMap , eFormat , Filter, MipFilter ) ) )
	{
		*pText = Temp;
		if(UseResRecycle)
			Temp->SetContainerAndRecycle(m_ResMgr.VideoMemContainer(), m_ResMgr.VideoMemRecycle());
		else
			Temp->SetContainerAndRecycle(m_ResMgr.VideoMemContainer(), NULL);
		
		return 0;
	}

	*pText = NULL;
	Temp->Release();
	return GERROR;
}

void CGraphic::ClearFrameCount()
{
	m_uFrameCount = 0;
}

ECreateType CGraphic::GetCreateType( void )
{
	return m_CreateType;
}

void CGraphic::RegisterGfkManageTex( ITexture* pTex )
{
	m_gfkManageTexList.insert(pTex);
}


void CGraphic::RemoveGfkManageTex( ITexture* pTex )
{
	GraphicManageTexList::iterator it = m_gfkManageTexList.find(pTex);
	if ( it != m_gfkManageTexList.end() )
	{
		m_gfkManageTexList.erase(it);
		pTex->Release();
	}
}