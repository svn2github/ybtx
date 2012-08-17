#include "stdafx.h"
#include "CEditorCore.h"
#include "CVertexDeclaration.h"
#include "CHardwareBufferMgr.h"
#include "CSkeletalsMgr.h"
#include "CEditSkeletalsMgr.h"
#include "BaseHelper.h"
#include "CMath.h"
#include "ModelHelper.h"
#include "CFileFormatMgr.h"
#include "CGraphic.h"
#include "CPkgFile.h"
#include "ExpHelper.h"
#include "CDebugText.h"
#include "CFontCfg.h"
#include "CEditRenderPipeline.h"
#include "CColor.inl"
#include "CEventState.h"
#include "CEditEffectManager.h"
float				CEditorCore::TP_F_GoldenSection = float(0.61803398874989484820458683436564);
CEditRenderStyle	CEditorCore::TR_EPRS_SELECT;
CEditRenderStyle	CEditorCore::TR_EMRS_SELECT;
CEditRenderStyle	CEditorCore::TR_EMRS_HOLD;

CRenderOperation	CEditorCore::TP_RO_BONE;
CRenderOperation	CEditorCore::TP_RO_CONE;
CRenderOperation	CEditorCore::TP_RO_SPHERE;

ITexture*			CEditorCore::TP_TX_BLACKWHITE;	//棋盘格黑白图
ITexture*			CEditorCore::TP_TX_VIOLETWHITE; //棋盘格紫白图
ITexture*			CEditorCore::TP_TX_BLACK;		//黑图
ITexture*			CEditorCore::TP_TX_GRAY;		//灰图
ITexture*			CEditorCore::TP_TX_WHITE;		//白图

#include "CParticleSystem.h"
bool CEditorCore::Initialize(void)
{
	if(!CEventState::HasInst())
		CEventState::Create();

	EnableCatchError(true);

	//当一切都设置完毕后 在切换渲染器Test
	CPkgFile::ReadThreadInit();
	CRenderPipeline* pRenderPipeline=CRenderPipeline::GetInst();
	pRenderPipeline->SetNull();
	new CEditRenderPipeline(pRenderPipeline);


	if (CEffectManager::GetInst())
		CEffectManager::GetInst()->Destroy();
	CEditEffectManager* pEditEffectManager = new CEditEffectManager();
	pEditEffectManager->ResetClassDesc();

	#pragma region PKG //打包资源和本地资源映射关系
	{
		CPkgFile::AddMapPkgName(L"engine", L"library");
		CPkgFile::AddMapPkgName(L"etc", L"other");
		CPkgFile::AddMapPkgName(L"game", L"logic");
		CPkgFile::AddMapPkgName(L"gui", L"gui");
		CPkgFile::AddMapPkgName(L"character", L"role");
		CPkgFile::AddMapPkgName(L"font", L"font");
		CPkgFile::AddMapPkgName(L"fx", L"effect");
		CPkgFile::AddMapPkgName(L"map", L"terrain");
		CPkgFile::AddMapPkgName(L"object", L"item");
		CPkgFile::AddMapPkgName(L"tile", L"scene");
	}
	#pragma endregion RS

	#pragma region RS //公用RS初始化
	{
		//TR_EPRS_SELECT
		TR_EPRS_SELECT.m_bSyn	= TRUE;
		TR_EPRS_SELECT.m_TimeRang = 1000.0f;
		TR_EPRS_SELECT.m_FillMode			= FILL_WIREFRAME;
		TR_EPRS_SELECT.m_Texturefactor[0.0]	= CColor::Blue;
		//TR_EPRS_SELECT.m_Texturefactor[0.5]	= CColor::LightBlue;
		TR_EPRS_SELECT.m_Texturefactor[1.0]	= CColor::Blue;
		TR_EPRS_SELECT.m_TexOP_S0			= TOP_SELECTARG1;
		TR_EPRS_SELECT.m_TexArg1_S0			= TA_TFACTOR;

		//TR_EMRS_SELECT
		TR_EMRS_SELECT.m_bSyn	= TRUE;
		TR_EMRS_SELECT.m_TimeRang = 1000.0f;
		TR_EMRS_SELECT.m_Texture_S0[0.0]		= 0;
		TR_EMRS_SELECT.m_Texturefactor[0.0]	= CColor::DarkSlateBlue;
		//TR_EMRS_SELECT.m_Texturefactor[0.5]	= CColor::Black;
		TR_EMRS_SELECT.m_Texturefactor[1.0]	= CColor::DarkSlateBlue;
		TR_EMRS_SELECT.m_AlphaTestEnable	= TRUE;
		TR_EMRS_SELECT.m_AlpOP_S0		= TOP_SELECTARG1;
		TR_EMRS_SELECT.m_AlpArg1_S0	= TA_TEXTURE;
		TR_EMRS_SELECT.m_TexOP_S0		= TOP_MODULATE2X;
		TR_EMRS_SELECT.m_TexArg1_S0	= TA_TEXTURE;
		TR_EMRS_SELECT.m_TexArg2_S0	= TA_DIFFUSE;
		TR_EMRS_SELECT.m_TexOP_S1		= TOP_ADD;
		TR_EMRS_SELECT.m_TexArg1_S1	= TA_CURRENT;
		TR_EMRS_SELECT.m_TexArg2_S1	= TA_TFACTOR;

		//TR_EMRS_HOLD
		TR_EMRS_HOLD.m_bSyn	= TRUE;
		TR_EMRS_HOLD.m_TimeRang = 1000.0f;
		TR_EMRS_HOLD.m_Texture_S0[0.0]		= 0;
		TR_EMRS_HOLD.m_Texturefactor[0.0]	= CColor::DarkBlue;
		//TR_EMRS_HOLD.m_Texturefactor[0.5]	= CColor::Black;
		TR_EMRS_HOLD.m_Texturefactor[1.0]	= CColor::DarkBlue;
		TR_EMRS_HOLD.m_AlphaTestEnable		= TRUE;
		TR_EMRS_HOLD.m_AlpOP_S0		= TOP_SELECTARG1;
		TR_EMRS_HOLD.m_AlpArg1_S0		= TA_TEXTURE;
		TR_EMRS_HOLD.m_TexOP_S0		= TOP_MODULATE2X;
		TR_EMRS_HOLD.m_TexArg1_S0		= TA_TEXTURE;
		TR_EMRS_HOLD.m_TexArg2_S0		= TA_DIFFUSE;
		TR_EMRS_HOLD.m_TexOP_S1		= TOP_ADD;
		TR_EMRS_HOLD.m_TexArg1_S1		= TA_CURRENT;
		TR_EMRS_HOLD.m_TexArg2_S1		= TA_TFACTOR;
	}
	#pragma endregion RS

	#pragma region RO //公用模型初始化
	{
		//TP_RO_BONE
		#pragma region BONE
		{		
			TP_RO_BONE.Create();
			TP_RO_BONE.operationType	= PT_TRIANGLELIST;
			TP_RO_BONE.useIndexes		= true;	
			TP_RO_BONE.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerNor::Format);
			
			float	tGShort = (1 - TP_F_GoldenSection) * 0.25f;
			float tBoneVB[] = { 
				0.0f,		0.0f,				0.0f,		 0.0f, -1.0f,  0.0f,
				0.0f,		1.0f,				0.0f,		 0.0f,  1.0f,  0.0f,		
				tGShort,	1.0f - TP_F_GoldenSection, 0.0f,		 1.0f,  0.0f,  0.0f,
				0.0f,		1.0f - TP_F_GoldenSection, tGShort,	 0.0f,  0.0f,  1.0f,
				-tGShort,	1.0f - TP_F_GoldenSection, 0.0f,		-1.0f,	0.0f,  0.0f,
				0.0f,		1.0f - TP_F_GoldenSection, -tGShort,	 0.0f,	0.0f, -1.0f,
			};
			WORD tBoneIB[]	= {
				0,	2,	3,
				0,	3,	4,
				0,	4,	5,
				0,	5,	2,
				1,	3,	2,
				1,	4,	3,
				1,	5,	4,
				1,	2,	5,
			};

			TP_RO_BONE.vertexData->vertexBufferBinding->SetBinding( 0,
				CHardwareBufferMgr::GetInst()->createVertexBuffer( sizeof(VerNor), 6, HBU_STATIC, tBoneVB ));
			TP_RO_BONE.indexData->indexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer( 24, HBU_STATIC, tBoneIB );
			TP_RO_BONE.indexData->indexCount = 24;
			TP_RO_BONE.indexData->indexStart = 0;
			TP_RO_BONE.vertexData->vertexCount = 6;
			TP_RO_BONE.vertexData->vertexStart = 0;
		}
		#pragma endregion BONE

		#pragma region CONE
		TP_RO_CONE.Create();
		TP_RO_CONE.operationType	= PT_TRIANGLELIST;
		TP_RO_CONE.useIndexes		= true;
		TP_RO_CONE.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerNor::Format);

		float h_dist	= (1.0f - TP_F_GoldenSection)*0.25f;
		float v_dist	= TP_F_GoldenSection;
		float cone_vb[] = 
		{
			0.0f,		0.0f,		0.0f,		0.0f,		0.0f,		1.0f,
			-h_dist,	0.0f,		-v_dist,	-1.0f,		0.0f,		0.0f,
			0.0f,		h_dist,		-v_dist,	0.0f,		1.0f,		0.0f,
			h_dist,		0.0f,		-v_dist,	1.0f,		0.0f,		0.0f,
			0.0f,		-h_dist,	-v_dist,	0.0f,		-1.0f,		0.0f
		};
		uint32 num_vertex = sizeof(cone_vb)/sizeof(cone_vb[0]);

		uint16 cone_ib[] = 
		{
			0, 2, 1,
			0, 3, 2,
			0, 4, 3,
			0, 1, 4,
			2, 3, 1,
			1, 3, 4
		};
		uint32 num_indexs = sizeof(cone_ib)/sizeof(cone_ib[0]);

		TP_RO_CONE.vertexData->vertexStart = 0;
		TP_RO_CONE.vertexData->vertexCount = num_vertex;
		TP_RO_CONE.vertexData->vertexBufferBinding->SetBinding(0, CHardwareBufferMgr::GetInst()->createVertexBuffer(
			sizeof(VerNor), num_vertex, HBU_STATIC, cone_vb)
			);

		TP_RO_CONE.indexData->indexStart  = 0;
		TP_RO_CONE.indexData->indexCount  = num_indexs;
		TP_RO_CONE.indexData->indexBuffer = CHardwareBufferMgr::GetInst()->createIndexBuffer(
			num_indexs, HBU_STATIC, cone_ib
			);
		#pragma endregion CONE

		#pragma region SPHERE
		{
			TP_RO_SPHERE.Create();
			TP_RO_SPHERE.operationType	= PT_TRIANGLELIST;
			TP_RO_SPHERE.useIndexes		= true;
			TP_RO_SPHERE.vertexData->vertexDeclaration = CVertexDeclaration::GET_VERDECL(VerNor::Format);
			CVector3f* pVertices = 0;
			uint16* pIndices  = 0;

			int num_vertices  = 0;
			int num_indices	  = 0;
			CreateSphere(1.0f, &pVertices, &pIndices, 50, 50, &num_vertices, &num_indices, 0.0f, CMath::two_pi, 0.0F, CMath::half_pi);

			TP_RO_SPHERE.vertexData->vertexBufferBinding->SetBinding( 0, CHardwareBufferMgr::GetInst()
				->createVertexBuffer( sizeof(CVector3f), num_vertices, HBU_STATIC, pVertices) 
				);
			TP_RO_SPHERE.indexData->indexBuffer	 = CHardwareBufferMgr::GetInst()->createIndexBuffer( num_indices, HBU_STATIC, pIndices );
			TP_RO_SPHERE.indexData->indexCount	 = num_indices;
			TP_RO_SPHERE.indexData->indexStart	 = 0;
			TP_RO_SPHERE.vertexData->vertexCount = num_vertices;
			TP_RO_SPHERE.vertexData->vertexStart = 0;

			SafeDelAry(pVertices);
			SafeDelAry(pIndices);
		}
		#pragma endregion SPHERE

		//Other
	}
	#pragma endregion RO

	#pragma region Texture //公用纹理初始化
	{
		const int Width	= 256;
		const int Boder	= 1;
		const int Grid	= 16;
		CColor GridColor1 = CColor::Silver;
		CColor GridColor2 = CColor::Gray;
		CColor GridColor3 = CColor::DarkRed;
		CColor BoderColor = CColor::Gainsboro;

		#pragma region BLACKWHITE
		{	
			//构建纹理数据
			uint32 DrTexData[Width*Width];
			uint32 VaTexData[Width*Width];

			int GridStep = Width / Grid;
			CColor TxDrColor[2][2] = { { GridColor1,BoderColor },{ GridColor2,BoderColor } };
			CColor TxVaColor[2][2] = { { GridColor1,BoderColor },{ GridColor3,BoderColor } };

			CGraphic::GetInst()->CreateEmptyTexture(Width,Width,1, TF_ARGB32,&TP_TX_BLACKWHITE);
			CGraphic::GetInst()->CreateEmptyTexture(Width,Width,1, TF_ARGB32,&TP_TX_VIOLETWHITE);
			
			for(int y =0 ;y<Width; ++y)
			{
				int tsy = y % GridStep;
				int tgy = y / GridStep;
				bool isBoderY = (tsy == 0 || tsy == GridStep - 1);
				for(int x = 0; x<Width; ++x)
				{
					int tsx = x % GridStep;
					int tgx = x /GridStep;
					bool isBoderX = (tsx == 0 || tsx == GridStep - 1);
					DrTexData[x + y*Width] = TxDrColor[(tgx+tgy)%2][isBoderY||isBoderX].ToARGB();
					VaTexData[x + y*Width] = TxVaColor[(tgx+tgy)%2][isBoderY||isBoderX].ToARGB();
				}
			}
			
			TP_TX_BLACKWHITE->FillFromMemory(DrTexData,TF_ARGB32,Width,CIRect(0,0,Width,Width),CIRect(0,0,Width,Width));
			TP_TX_VIOLETWHITE->FillFromMemory(VaTexData,TF_ARGB32,Width,CIRect(0,0,Width,Width),CIRect(0,0,Width,Width));
		}
		#pragma endregion BLACKWHITE

		TP_TX_BLACK = CGraphic::GetInst()->GetSystemTexture(eSTT_BLACK);
		TP_TX_GRAY = CGraphic::GetInst()->GetSystemTexture(eSTT_GRAY);
		TP_TX_WHITE = CGraphic::GetInst()->GetSystemTexture(eSTT_WHITE);
	}
	#pragma endregion Texture

	#pragma region SkeletalsMgr //骨架池编辑器初始化
	if (CSkeletalsMgr::HasInst())
	{
		CSkeletalsMgr::Destroy();
		new CEditSkeletalsMgr;
	}
	#pragma endregion SkeletalsMgr

	CRenderPipeline::GetInst()->_SetDefaultTexture(TP_TX_VIOLETWHITE);

	if ( !CFontCfg::HasInst() )
	{
		CFontCfg::Create();
		CFontCfg::GetInst()->UseDefault();
	}
	CDebugText::Create();

	return true;
}

bool CEditorCore::Destroy(void)
{
	#pragma region BONE //公用模型释放
	{
		TP_RO_BONE.Destroy();
	}
	#pragma endregion BONE

	if ( CFontCfg::HasInst() )
		CFontCfg::Destroy();

	SafeRelease(TP_TX_BLACKWHITE);
	SafeRelease(TP_TX_VIOLETWHITE);
	CPkgFile::ReadThreadUnit();
	CDebugText::Destroy();
	return true;
}
