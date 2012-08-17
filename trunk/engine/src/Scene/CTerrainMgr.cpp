#include "stdafx.h"
#include "CTerrain.h"
#include "CTerrainMgr.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "CShadowFilter.h"
#include "CCamera.h"
#include "CIndexHardwareBuffer.h"
#include "CVertexHardwareBuffer.h"
#include "CVertexDeclaration.h"
#include "CRenderPipeline.h"
#include "CDataSources.h"
#include "CTextureQualityMgr.h"
#include "CJPGDecoder.h"
#include "CSceneConfig.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------


//------------------------------------------------------------------------------

GVector<CVertexDeclaration*>	CTerrainMgr::st_TerrainDecl;// = {0,vTerrain::ShaderFormat,vTerrain::NormalFormat};
CIndexHardwareBuffer*			CTerrainMgr::st_pIndex = NULL;
GVector<CVector2f>				CTerrainMgr::st_apTextUV;
DWORD							CTerrainMgr::st_Terrain_VS_Shader_ID = MakeSysVS_ID(MakeGroupID(1));
DWORD							CTerrainMgr::st_Terrain_PS_Shader_ID = MakeSysPS_ID(MakeGroupID(1) + 1);
bool							CTerrainMgr::st_isEditor = false;
CTerrainMgr::LightMapPool		CTerrainMgr::st_LightMapBuffer;
ITexture*						CTerrainMgr::st_TerrainTex = NULL;

const float CTerrainMgr::REGION_SIZE		= 4096.0f;
const uint	CTerrainMgr::MAP_SIZE			= 32;
const uint	CTerrainMgr::MAP_SIZE_SQR		= CTerrainMgr::MAP_SIZE * CTerrainMgr::MAP_SIZE;
const uint	CTerrainMgr::MAX_TEXTURE		= 2048;
const uint	CTerrainMgr::MAX_ROW_INDEX		= 16;
const uint	CTerrainMgr::TEXTURE_SIZE		= 256;
const uint	CTerrainMgr::MIN_TEXTURE_SIZE	= 128;
const float CTerrainMgr::MAGIC_A			= 0.0000244f;
const uint	CTerrainMgr::EXPAND_SIZE		= 8;
const uint	CTerrainMgr::LIGHT_MAP			= 0x0f000000;
const uint  CTerrainMgr::SPECULAR			= 0x00f00000;

class CTerrainTexHandler
	: public IResTexture
{
public:
	CTerrainTexHandler()
		: IResTexture("", L"")
		, m_pTerrMgr(NULL)
	{
		Release();	
	}

	~CTerrainTexHandler()
	{
		m_pTerrMgr = NULL;
	}

	virtual void   Free()
	{
	}

	void CreateTerrainTex( uint quality_level )
	{
		uint tex_size		= CTerrainMgr::MAX_TEXTURE +CTerrainMgr::EXPAND_SIZE*CTerrainMgr::MAX_ROW_INDEX*2;
		uint tex_level		= 3;

		CGraphic::GetInst()->CreateEmptyTexture(tex_size, tex_size, tex_level, TF_XRGB32, &CTerrainMgr::st_TerrainTex);
		CGraphic::GetInst()->RegisterGfkManageTex(CTerrainMgr::st_TerrainTex);
		CJPGDecoder::SetLod(quality_level);
		if ( m_pTerrMgr )
			m_pTerrMgr->LoadTexture(CTerrainMgr::st_TerrainTex);
	}

	int	ReLoadPic()
	{
		CJPGDecoder::SetLod(CTextureQualityMgr::GetInst()->GetQualityLevel());
		if ( m_pTerrMgr )
			m_pTerrMgr->LoadTexture(CTerrainMgr::st_TerrainTex);
		return 1;
	}

	void RegisterTerrainMgr( CTerrainMgr* pMgr )
	{
		m_pTerrMgr = pMgr;
	}

	void UnRegisterTerrainMgr( CTerrainMgr* pMgr )
	{
		if ( pMgr == m_pTerrMgr )
			m_pTerrMgr = NULL;
	}

private:
	CTerrainMgr* m_pTerrMgr;
};

static CTerrainTexHandler g_TerrainTexHandler;

void CTerrainMgr::RegiserVertexDecl()
{	
	CVertexDeclaration* pVD;
	//顶点的复合格式
	/*
	0 - 顶点 法线 Diffuse(顶点烘培色)
	1 - specular 
	2 - original   vertex color 
	3 - Level0UV   Level0UV 第一层取址纹理坐标
	4 - Level1UV   Level1UV 第二层取址纹理坐标
	5 - Shader0UV  第一层悬崖帖计算坐标
	6 - Level2UV   第三层取址纹理坐标;
	7 - lightmap
	*/
	//普通地表格式
	st_TerrainDecl.resize(VTX_DECL_NUM);
	if(!st_isEditor)
	{
		pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::NormalFormat);
		pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
		pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
		pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

		pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
		pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标
		st_TerrainDecl[TerrainFlag::Normal] = pVD;

		///add specular
		pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::NormalSpecularFormat);
		pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
		pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
		pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

		pVD->AddElement(1,0,					VET_COLOUR,VES_SPECULAR);
		pVD->AddElement(2,0,					VET_COLOUR,VES_COLOR,2);

		pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
		pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标
		st_TerrainDecl[TerrainFlag::NormalSpecular] = pVD;


		//普通地表格式- lightmap
		pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::NormalFormat|LIGHT_MAP);
		pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
		pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
		pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

		pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);;//第一层纹理坐标
		pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标

		pVD->AddElement(MIN_SHARE_BUF,0,		VET_FLOAT2,VES_TEXTURE_COORDINATES,2);
		st_TerrainDecl[TerrainFlag::LightMap] = pVD;

		//普通地表格式- lightmap + sprcular
		pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::NormalFormat|LIGHT_MAP|SPECULAR);
		pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
		pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
		pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

		pVD->AddElement(1,0,					VET_COLOUR,VES_SPECULAR);
		pVD->AddElement(2,0,					VET_COLOUR,VES_COLOR,2);

		pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
		pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标

		pVD->AddElement(MIN_SHARE_BUF,0,		VET_FLOAT2,VES_TEXTURE_COORDINATES,2);
		st_TerrainDecl[TerrainFlag::NormalSpecular|TerrainFlag::LightMap] = pVD;

	}
	else
	{
		pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::NormalFormat);
		pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
		pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
		pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

		pVD->AddElement(2,0,					VET_COLOUR,VES_SPECULAR);

		pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
		pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标
		st_TerrainDecl[TerrainFlag::Normal] = pVD;

		//////////////////////////////////////////////////////////////////////////add specular
		pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::NormalSpecularFormat);
		pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
		pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
		pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

		pVD->AddElement(1,0,					VET_COLOUR,VES_SPECULAR);
		pVD->AddElement(2,0,					VET_COLOUR,VES_COLOR,2);

		pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
		pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标

		pVD->AddElement(5,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,2);
		st_TerrainDecl[TerrainFlag::NormalSpecular] = pVD;

		//普通地表格式- lightmap
		pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::NormalFormat|LIGHT_MAP);
		pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
		pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
		pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

		pVD->AddElement(2,0,					VET_COLOUR,VES_SPECULAR);

		pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
		pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标

		pVD->AddElement(MIN_SHARE_BUF,0,VET_FLOAT2,			VES_TEXTURE_COORDINATES,2);//lightmap
		st_TerrainDecl[TerrainFlag::LightMap] = pVD;

		//普通地表格式- lightmap + sprcular
		pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::NormalSpecularFormat|LIGHT_MAP);
		pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
		pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
		pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

		pVD->AddElement(1,0,					VET_COLOUR,VES_SPECULAR);
		pVD->AddElement(2,0,					VET_COLOUR,VES_COLOR,2);

		pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
		pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标

		pVD->AddElement(MIN_SHARE_BUF,0,		VET_FLOAT2,VES_TEXTURE_COORDINATES,2);
		st_TerrainDecl[TerrainFlag::NormalSpecular|TerrainFlag::LightMap] = pVD;
	}

	//Shader
	pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::ShaderFormat);
	pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

	pVD->AddElement(2,0,					VET_COLOUR,VES_SPECULAR);

	pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
	pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标

	pVD->AddElement(5,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,2);
	st_TerrainDecl[TerrainFlag::Cliff] = pVD;

	///cliff+specular
	pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::ShaderFormat|SPECULAR);
	pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

	pVD->AddElement(1,0,					VET_COLOUR,VES_SPECULAR);
	pVD->AddElement(2,0,					VET_COLOUR,VES_COLOR,2);

	pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
	pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标

	pVD->AddElement(5,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,2);
	st_TerrainDecl[TerrainFlag::Cliff|TerrainFlag::NormalSpecular] = pVD;


	//Shader - lightmap
	pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::ShaderFormat|LIGHT_MAP);
	pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

	pVD->AddElement(2,0,					VET_COLOUR,VES_SPECULAR);

	pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
	pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标

	pVD->AddElement(5,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,2);

	pVD->AddElement(MIN_SHARE_BUF,0,		VET_FLOAT2,VES_TEXTURE_COORDINATES,3);
	st_TerrainDecl[TerrainFlag::Cliff|TerrainFlag::LightMap] = pVD;

	///cliff+specular+lightmap
	pVD = CDataSources::GetInst()->NewVertexDeclaration(vTerrain::ShaderFormat|LIGHT_MAP|SPECULAR);
	pVD->AddElement(0,0,					VET_FLOAT3,VES_POSITION);
	pVD->AddElement(0,sizeof(CVector3f),	VET_FLOAT3,VES_NORMAL);
	pVD->AddElement(0,pVD->GetVertexSize(0),VET_COLOUR,VES_DIFFUSE);

	pVD->AddElement(1,0,					VET_COLOUR,VES_SPECULAR);
	pVD->AddElement(2,0,					VET_COLOUR,VES_COLOR,2);

	pVD->AddElement(3,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,0);//第一层纹理坐标
	pVD->AddElement(4,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,1);//第二层纹理坐标

	pVD->AddElement(5,0,					VET_FLOAT2,VES_TEXTURE_COORDINATES,2);

	pVD->AddElement(MIN_SHARE_BUF,0,		VET_FLOAT2,VES_TEXTURE_COORDINATES,3);
	st_TerrainDecl[TerrainFlag::Cliff|TerrainFlag::NormalSpecular|TerrainFlag::LightMap] = pVD;

	//---------借用一方宝地，初始化其他静态变量---------------------
	uint PerNum = eSRegionSpan / MAP_SIZE;
	uint SqrNum = PerNum * PerNum;


	vector<int16>		p(MAP_SIZE_SQR*6);
	vector<CVector2f>	uv(MAP_SIZE_SQR*4*SqrNum);
	float Magic = float(EXPAND_SIZE)/float(MAX_TEXTURE+EXPAND_SIZE*MAX_ROW_INDEX*2);
	float offset = 1.0f / MAX_ROW_INDEX;
	
	st_apTextUV.resize(256);
	for ( int i = 0;i<256;++i )
	{
		st_apTextUV[i].x = (i%MAX_ROW_INDEX)*offset + Magic;
		st_apTextUV[i].y = (i/MAX_ROW_INDEX)*offset + Magic;
	}

	Magic = 0.0009765625f;
	float uvStep = 1.0f / float(eSRegionSpan);//0.015625f;// 1/64.0f;
	
	for (uint i = 0,tempi = 0,ii = 0; i<MAP_SIZE; ++i,tempi+=MAP_SIZE*4,ii +=6*MAP_SIZE )
	{
		for (uint j = 0,tempj = 0,ij = 0; j<MAP_SIZE; ++j,tempj +=4,ij +=6 )
		{
			int	no = tempi + tempj;
			int ino = ii + ij;

			for(uint q = 0; q < SqrNum; ++q)
			{
				int delta = q * MAP_SIZE_SQR * 4;
				float dx = (q % PerNum) * uvStep * MAP_SIZE;
				float dy = (q / PerNum) * uvStep * MAP_SIZE;
				uv[no + 1 + delta].x = uv[no + 0 + delta].x = j*uvStep  + Magic + dx;///64.0f*/;
				uv[no + 2 + delta].y = uv[no + 0 + delta].y = 1-i*uvStep+ Magic - dy;///64.0f;
				uv[no + 3 + delta].x = uv[no + 2 + delta].x = uv[no + delta].x + uvStep;
				uv[no + 3 + delta].y = uv[no + 1 + delta].y = uv[no + delta].y - uvStep;
			}

			if (1 ==(i+j)%2)
			{
				p[ino] = p[ino+4] = no + 3;
				p[ino+1] = p[ino+3] = no;
				p[ino+2] = no + 1;
				p[ino+5] = no + 2;
			}
			else
			{
				p[ino] = no;
				p[ino+1] = p[ino+5] = no + 1;
				p[ino+2] = p[ino+4] = no + 2;
				p[ino+3] = no + 3;
			}
		}
	}

	Ast(NULL==st_pIndex);
	st_pIndex		= CHardwareBufferMgr::GetInst()->createIndexBuffer(
		MAP_SIZE_SQR*6,HBU_STATIC_WRITE_ONLY,&(p[0]));

	st_LightMapBuffer.resize(SqrNum);
	for(uint q = 0; q < SqrNum; ++q)
	{
		st_LightMapBuffer[q]	=	CHardwareBufferMgr::GetInst()->createVertexBuffer(
			sizeof(CVector2f),
			MAP_SIZE_SQR*4,
			HBU_STATIC_WRITE_ONLY,
			(&uv[q*MAP_SIZE_SQR * 4]));
	}

	g_TerrainTexHandler.CreateTerrainTex(CSceneConfig::GetInst()->GetTexQualityLevel());
	CTextureQualityMgr::GetInst()->_AddTexture(&g_TerrainTexHandler);
}

CTerrainMgr::CTerrainMgr(CRenderGroup* pGroup)
:m_isInit(false)
,m_HotIndex(0)
,m_Mask(TerrainFlag::Normal)/*,m_isWire(false)*/
,m_LightIndex(0)
,m_pGroup(pGroup)
,m_Texture(NULL)
{
	AddRef();
	m_Param.m_RS = CGraphic::GetInst()->GetStaticRS(RS_MAP1L);

	const float mipMapBias = 0;//-0.5f;

	m_Param.m_RS.m_Sample_S0 =			TEXF_LINEAR;
	m_Param.m_RS.m_MipMapSample_S0 =	TEXF_LINEAR;

	//m_Param.m_RS.m_ShaderType = 0;//2000<<16 + 2001;
	m_Param.m_RS.m_AlphaBlendEnable = false;
	//混合细节设置

	m_Param.m_RS.m_TexOP_S0 = TOP_SELECTARG1;
	m_Param.m_RS.m_TexArg1_S0 = TA_TEXTURE;

	m_Param.m_RS.m_TexOP_S1 = TOP_BLENDDIFFUSEALPHA;
	m_Param.m_RS.m_TexArg1_S1 = TA_TEXTURE;
	m_Param.m_RS.m_TexArg2_S1 = TA_CURRENT;

	m_Param.m_RS.m_TexOP_S2 = TOP_MODULATE;
	m_Param.m_RS.m_TexArg1_S2 = TA_TEXTURE;
	m_Param.m_RS.m_TexArg2_S2 = TA_CURRENT;

	m_Param.m_RS.m_TexOP_S3 = TOP_MODULATE2X;
	m_Param.m_RS.m_TexArg1_S3 = TA_DIFFUSE;
	m_Param.m_RS.m_TexArg2_S3 = TA_CURRENT;
	//m_Param.m_RS.m_UAddr_S3 = m_Param.m_RS.m_VAddr_S3 = TADDRESS_BORDER;

	m_Param.m_RS.m_AlpOP_S0 = TOP_DISABLE;

	m_Param.m_RS.m_MipMapBias_S0 = *(DWORD*)&(mipMapBias);
	m_Param.m_RS.m_MipMapBias_S1 = *(DWORD*)&(mipMapBias);
	m_Param.m_RS.m_MipMapBias_S2 = *(DWORD*)&(mipMapBias);
	m_Param.m_RS.m_LightEnable = false;
	m_Param.m_Mat.Identity();

	m_Param.m_RS.m_SpecularEnable = false;

	m_pGroup->RegisterBatch( ID_TERRAIN_FILTER , this );

	g_TerrainTexHandler.RegisterTerrainMgr(this);
}

CTerrainMgr::~CTerrainMgr()
{
	DestroyBuffer();
	SafeRelease(m_Texture);
	m_pGroup->UnRegisterBatch( ID_TERRAIN_FILTER , this );
	g_TerrainTexHandler.UnRegisterTerrainMgr(this);
}

void CTerrainMgr::Render()
{
	if ( !m_Param.m_RS.m_ShaderType.IsNull() || st_isEditor ||	HasRenderFlag(TerrainFlag::Cliff) )
	{
		DWORD handle = (st_Terrain_PS_Shader_ID + (GetVtxDeclIndex(m_Mask) & ~(TerrainFlag::NormalSpecular)) );
		m_Param.m_RS.m_ShaderType |= st_Terrain_VS_Shader_ID;
		m_Param.m_RS.m_ShaderType |= handle;
	}

	CRenderPipeline::GetInst()->SetRenderStyle(m_Param.m_RS);
	CRenderPipeline::GetInst()->SetWorld(m_Param.m_Mat);
	m_ShadowFilter->UpdateShadowParams();
	RenderEx();
}

void CTerrainMgr::RenderEx(void)
{
	ElementPool::iterator it = m_ElePool.begin();
	ElementPool::iterator ite = m_ElePool.end();
	CRenderPipeline* rp = CRenderPipeline::GetInst();
	CCamera* pCamera = m_pGroup->GetCamera();
	TerrainNode* pNode;

	CRenderPipeline::GetInst()->_SetSamplerState(2,SAMP_ADDRESSU,TADDRESS_CLAMP );
	CRenderPipeline::GetInst()->_SetSamplerState(2,SAMP_ADDRESSV,TADDRESS_CLAMP );
	CRenderPipeline::GetInst()->_SetSamplerState(2, SAMP_MINFILTER, TEXF_LINEAR);
	CRenderPipeline::GetInst()->_SetSamplerState(2, SAMP_MAGFILTER, TEXF_LINEAR);

	for (;it!=ite;++it)
	{
		pNode = (TerrainNode*)it->second;

		if( !pNode->IsValid() || pCamera->cull(pNode->m_TAAB) == ClipStatus::Outside )
			continue;

		for( uint i = 0; i<pNode->m_TerrainTiles.size(); ++i )
		{
			if ( pCamera->cull(pNode->m_TerrainTiles[i].aab) > 0 )
			{
				rp->_SetTexture(2,pNode->text);
				rp->RenderEx(&(pNode->m_TerrainTiles[i].op));
			}
		}
	}
}

void CTerrainMgr::Update(void)
{
	m_ShadowFilter = (CShadowFilter*)m_pGroup->GetBatch(ID_SHADOW_FILTER);
	m_Param.m_RS.m_ShaderType.Clear();
	m_Param.m_RS.m_Texture_S3 = NULL;
	m_ShadowFilter->SetShadowParams(m_Param.m_RS);
}

CVector2f&	CTerrainMgr::getUV(uint index)
{
	return st_apTextUV[index];
}

uint CTerrainMgr::maxTextureIndex(void)
{
	return TEXTURE_SIZE;
}

void CTerrainMgr::DestroyBuffer()
{
	TerrainNode* pNode;
	ElementPool::iterator it = m_ElePool.begin();
	ElementPool::iterator ite = m_ElePool.end();
	for( ; it != ite; ++it )
	{
		pNode = (TerrainNode*)it->second;
		SafeRelease(pNode->text);
		for( uint i = 0; i<pNode->m_TerrainTiles.size(); ++i )
		{
			for (int j = 0;j<5;++j)
			{
				CHardwareBufferMgr::GetInst()->DestroyBuffer(
					pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding->GetBuffer(j));
			}
			pNode->m_TerrainTiles[i].op.Destroy();
		}
	}
	m_ElePool.clear();
	m_width = 0;
	m_height = 0;
	m_isInit = false;
}

void CTerrainMgr::TurnOnLightMap(bool isTurnOn)
{
	if (isTurnOn)
	{
		m_Mask |= TerrainFlag::LightMap;
		TerrainNode* pNode;
		ElementPool::iterator it = m_ElePool.begin();
		ElementPool::iterator ite = m_ElePool.end();
		for( ; it != ite; ++it )
		{
			pNode = (TerrainNode*)it->second;
			for( uint i = 0; i<pNode->m_TerrainTiles.size(); ++i )
			{
				pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding->SetBinding(7,st_LightMapBuffer[i]);
				pNode->m_TerrainTiles[i].op.vertexData->vertexDeclaration = st_TerrainDecl[GetVtxDeclIndex(m_Mask)];
			}
		}

		m_Param.m_RS.m_TexOP_S2   = TOP_MODULATE;
		m_Param.m_RS.m_TexArg1_S2 = TA_TEXTURE;
		m_Param.m_RS.m_TexArg2_S2 = TA_CURRENT;
		m_Param.m_RS.m_TexOP_S3	  = TOP_MODULATE2X;
	}
	else
	{
		m_Mask &= ~TerrainFlag::LightMap;
		TerrainNode* pNode;
		ElementPool::iterator it = m_ElePool.begin();
		ElementPool::iterator ite = m_ElePool.end();
		size_t size = m_ElePool.size();
		for( ; it != ite; ++it )
		{
			pNode = (TerrainNode*)it->second;
			for( uint i = 0; i<pNode->m_TerrainTiles.size(); ++i )
			{
				pNode->m_TerrainTiles[i].op.vertexData->vertexBufferBinding->UnsetBinding(7);
				pNode->m_TerrainTiles[i].op.vertexData->vertexDeclaration = st_TerrainDecl[GetVtxDeclIndex(m_Mask)];
			}
		}
		m_Param.m_RS.m_TexOP_S2   = TOP_MODULATE2X;
		m_Param.m_RS.m_TexArg1_S2 = TA_SPECULAR;
		m_Param.m_RS.m_TexArg2_S2 = TA_CURRENT;
		m_Param.m_RS.m_TexOP_S3	  = TOP_DISABLE;
	}
}

bool CTerrainMgr::SetRenderFlag(TerrainFlag::Mask mask )
{
	m_Mask = mask;
	return true;
}

bool CTerrainMgr::LoadTexture(ITexture* pText)
{
	if (NULL == pText)
		return false;

	SafeRelease(m_Texture);
	m_Texture = pText;
	m_Texture->AddRef();
	CJPGDecoder coder(m_Texture, m_szTerrTexName.c_str());

	m_Param.m_RS.m_Texture_S0 =  m_Texture;
	m_Param.m_RS.m_Texture_S1 =  HasRenderFlag(TerrainFlag::Cliff)?NULL:m_Texture;
	return true;
}

bool CTerrainMgr::LoadTexture(const char* filename)
{
	if (NULL==filename)
		return m_isInit = false;
	SafeRelease(m_Texture);

	m_szTerrTexName = filename;
	CTerrainMgr::st_TerrainTex->AddRef();
	m_Texture = CTerrainMgr::st_TerrainTex;
	CJPGDecoder coder(m_Texture, filename);

	m_Param.m_RS.m_Texture_S0 = m_Texture;
	m_Param.m_RS.m_Texture_S1 = HasRenderFlag(TerrainFlag::Cliff)?NULL:m_Texture;
	return true;
}

CElementNode* CTerrainMgr::_CreateNode( URegionID ID ) 
{
	return new TerrainNode(this,ID);
}

void sqr::CTerrainMgr::ClearRSShaderType()
{
	m_Param.m_RS.m_ShaderType.Clear();
}
