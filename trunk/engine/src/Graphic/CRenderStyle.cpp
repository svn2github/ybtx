#include "stdafx.h"
#include "CGraphic.h"
#include "CRenderStyle.h"
#include "TimeHelper.h"
#include "CBufFile.h"
#include "Algorithm.h"
#include "CColor4.inl"
#include "CGpuProgramMgr.h"
#include "SyncSystem.h"
#include "TSqrAllocator.inl"
#include "CColor4.inl"


STATIC_RS::STATIC_RS()
// 自身成员初始化
		: m_Texture_S0(NULL)
		, m_Texture_S1(NULL)
		, m_Texture_S2(NULL)
		, m_Texture_S3(NULL)
		, m_fRefractIndex(0.0f)
		, m_fZBias(0.0f)
		//, m_fSlopeScaleZBias(0.0f)
{
	// 基类成员初始化
	m_Material.Ambient	= CColor4( 0.5f, 0.5f, 0.5f, 1.0f );
	m_Material.Diffuse	= CColor4( 0.5f, 0.5f, 0.5f, 1.0f );
	m_Material.Specular	= CColor4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_Material.Emissive	= CColor4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_Material.Power	= 0.0f;
	m_Alpharef			= 0x00000001;
	m_Texturefactor		= 0xFFFFFFFF;

	m_AlphaBlendEnable	= TRUE;
	m_SrcBlend 			= BLEND_SRCALPHA;
	m_DestBlend 		= BLEND_INVSRCALPHA;

	m_AlphaTestEnable	= TRUE;
	m_AlphaTestFun 		= CMP_GREATEREQUAL;

	m_LightEnable 		= FALSE;
	m_SpecularEnable	= FALSE;

	m_ZTestEnable 		= ZB_TRUE;
	m_ZTestFun 			= CMP_LESSEQUAL;
	m_ZWrite 			= TRUE;

	m_Cull 				= CULLT_CCW ;
	m_FillMode 			= FILL_SOLID;

	m_Uv_S0				= 0x00000000;
	m_Uv_S1				= 0x00000000;
	m_Uv_S2				= 0x00000000;

	m_Sample_S0 		= TEXF_LINEAR;
	m_Sample_S1 		= TEXF_LINEAR;
	m_Sample_S2 		= TEXF_LINEAR;
	m_Sample_S3 		= TEXF_LINEAR;
	m_TexOP_S0 			= TOP_MODULATE;
	m_TexOP_S1 			= TOP_DISABLE;
	m_TexOP_S2 			= TOP_DISABLE;
	m_TexOP_S3 			= TOP_DISABLE;
	m_TexArg1_S0 		= TA_TEXTURE;
	m_TexArg1_S1 		= TA_TEXTURE;
	m_TexArg1_S2 		= TA_TEXTURE;
	m_TexArg1_S3 		= TA_TEXTURE;
	m_TexArg2_S0 		= TA_DIFFUSE;
	m_TexArg2_S1 		= TA_DIFFUSE;
	m_TexArg2_S2 		= TA_DIFFUSE;
	m_TexArg2_S3 		= TA_DIFFUSE;
	m_AlpOP_S0 			= TOP_SELECTARG1;
	m_AlpOP_S1 			= TOP_DISABLE;
	m_AlpOP_S2 			= TOP_DISABLE;
	m_AlpOP_S3 			= TOP_DISABLE;
	m_AlpArg1_S0 		= TA_TEXTURE;
	m_AlpArg1_S1 		= TA_TEXTURE;
	m_AlpArg1_S2 		= TA_TEXTURE;
	m_AlpArg1_S3 		= TA_TEXTURE;
	m_AlpArg2_S0 		= TA_DIFFUSE;
	m_AlpArg2_S1 		= TA_DIFFUSE;
	m_AlpArg2_S2 		= TA_DIFFUSE;
	m_AlpArg2_S3 		= TA_DIFFUSE;
	m_UVSrc_S0 			= TSS_TCI_PASSTHRU;
	m_UVSrc_S1 			= TSS_TCI_PASSTHRU;
	m_UVSrc_S2 			= TSS_TCI_PASSTHRU;
	m_UVSrc_S3 			= TSS_TCI_PASSTHRU;
	m_UAddr_S0 			= TADDRESS_WRAP;
	m_UAddr_S1 			= TADDRESS_WRAP;
	m_UAddr_S2 			= TADDRESS_WRAP;
	m_UAddr_S3 			= TADDRESS_WRAP;
	m_VAddr_S0 			= TADDRESS_WRAP;
	m_VAddr_S1 			= TADDRESS_WRAP;
	m_VAddr_S2 			= TADDRESS_WRAP;
	m_VAddr_S3 			= TADDRESS_WRAP;
	m_MipMapSample_S0	= TEXF_LINEAR;
	m_MipMapSample_S1	= TEXF_LINEAR;
	m_MipMapSample_S2	= TEXF_LINEAR;
	m_MipMapSample_S3	= TEXF_LINEAR;
	m_MipMapBias_S0		= 0;
	m_MipMapBias_S1		= 0;
	m_MipMapBias_S2		= 0;
	m_MipMapBias_S3		= 0;
	m_bPreLight			= false;
}

CRenderStyle::CRenderStyle( const string& szName, const wstring& szAlias )
		: CResUnit( szName, szAlias, eRT_eRs )
{
	m_bOnRead = false;
	m_Ref = 1;
	m_TimeRang = 1000;
	m_bSyn     = false;

	m_Ambient[0.0f]			= m_Ambient[1.0f]		= CColor4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_Diffuse[0.0f]			= m_Diffuse[1.0f]		= CColor4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_Specular[0.0f]		= m_Specular[1.0f]		= CColor4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_Emissive[0.0f]		= m_Emissive[1.0f]		= CColor4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_Specularpower[0.0f]	= m_Specularpower[1.0f] = 0.0f;

	m_Alpharef[0.0f]		= m_Alpharef[1.0f]		= 0x00000000;
	m_Texturefactor[0.0f]	= m_Texturefactor[1.0f]	= 0xFFFFFFFF;

	m_AlphaBlendEnable		= FALSE;
	m_SrcBlend				= BLEND_SRCALPHA;
	m_DestBlend				= BLEND_INVSRCALPHA;

	m_AlphaTestEnable		= FALSE;
	m_AlphaTestFun			= CMP_GREATEREQUAL;

	m_LightEnable			= TRUE;
	m_SpecularEnable		= FALSE;

	m_ZTestEnable			= ZB_TRUE;
	m_ZWrite				= TRUE;
	m_ZTestFun				= CMP_LESSEQUAL;

	m_Cull					= CULLT_CCW ;
	m_FillMode				= FILL_SOLID;


	m_Uv_S0				= 0;
	m_Uv_S1				= 0;
	m_Uv_S2				= 0;

	m_Texture_S0[0.0f]			= 0;
	m_Texture_S1[0.0f]			= 0xffffffff;
	m_Texture_S2[0.0f]			= 0xffffffff;
	m_Texture_S3[0.0f]			= 0xffffffff;
	m_Sample_S0			= TEXF_LINEAR;
	m_Sample_S1			= TEXF_LINEAR;
	m_Sample_S2			= TEXF_LINEAR;
	m_Sample_S3			= TEXF_LINEAR;
	m_TexOP_S0			= TOP_MODULATE;
	m_TexOP_S1			= TOP_DISABLE;
	m_TexOP_S2			= TOP_DISABLE;
	m_TexOP_S3			= TOP_DISABLE;
	m_TexArg1_S0			= TA_TEXTURE;
	m_TexArg1_S1			= TA_TEXTURE;
	m_TexArg1_S2			= TA_TEXTURE;
	m_TexArg1_S3			= TA_TEXTURE;
	m_TexArg2_S0			= TA_DIFFUSE;
	m_TexArg2_S1			= TA_DIFFUSE;
	m_TexArg2_S2			= TA_DIFFUSE;
	m_TexArg2_S3			= TA_DIFFUSE;
	m_AlpOP_S0			= TOP_SELECTARG1;
	m_AlpOP_S1			= TOP_DISABLE;
	m_AlpOP_S2			= TOP_DISABLE;
	m_AlpOP_S3			= TOP_DISABLE;
	m_AlpArg1_S0			= TA_TEXTURE;
	m_AlpArg1_S1			= TA_TEXTURE;
	m_AlpArg1_S2			= TA_TEXTURE;
	m_AlpArg1_S3			= TA_TEXTURE;
	m_AlpArg2_S0			= TA_DIFFUSE;
	m_AlpArg2_S1			= TA_DIFFUSE;
	m_AlpArg2_S2			= TA_DIFFUSE;
	m_AlpArg2_S3			= TA_DIFFUSE;
	m_UVSrc_S0			= TSS_TCI_PASSTHRU;
	m_UVSrc_S1			= TSS_TCI_PASSTHRU;
	m_UVSrc_S2			= TSS_TCI_PASSTHRU;
	m_UVSrc_S3			= TSS_TCI_PASSTHRU;
	m_UAddr_S0			= TADDRESS_WRAP;
	m_UAddr_S1			= TADDRESS_WRAP;
	m_UAddr_S2			= TADDRESS_WRAP;
	m_UAddr_S3			= TADDRESS_WRAP;
	m_VAddr_S0			= TADDRESS_WRAP;
	m_VAddr_S1			= TADDRESS_WRAP;
	m_VAddr_S2			= TADDRESS_WRAP;
	m_VAddr_S3			= TADDRESS_WRAP;
	m_MipMapSample_S0		= TEXF_LINEAR;
	m_MipMapSample_S1		= TEXF_LINEAR;
	m_MipMapSample_S2		= TEXF_LINEAR;
	m_MipMapSample_S3		= TEXF_LINEAR;
	m_MipMapBias_S0		= 0;
	m_MipMapBias_S1		= 0;
	m_MipMapBias_S2		= 0;
	m_MipMapBias_S3		= 0;
}

CRenderStyle::~CRenderStyle(void)
{
	if(m_bOnRead)
	{
		CSyncSystem::SafeDelCoder((SyncCoder*)0x1); //等Coder出现就好了
		//GfkLogErr("有读取终止","CRenderStyle");
	}
}

void CRenderStyle::GetCurRenderStyle( STATIC_RS& RenderStyle, ITexture** pTextureList, int nListNum, uint32 FrameTime ,uint32 CurTime, void* pUser) const
{
	float Pos = (m_bSyn?FrameTime:(CurTime + uint32( pUser )))/m_TimeRang;
	Pos -= (int)Pos;

	uint32 Texture0					= GetMapState( m_Texture_S0, Pos );
	uint32 Texture1					= GetMapState( m_Texture_S1, Pos );
	uint32 Texture2					= GetMapState( m_Texture_S2, Pos );
	uint32 Texture3					= GetMapState( m_Texture_S3, Pos );
	RenderStyle.m_Texture_S0		= Texture0 >= (uint32)nListNum ? NULL : pTextureList[Texture0];
	RenderStyle.m_Texture_S1		= Texture1 >= (uint32)nListNum ? NULL : pTextureList[Texture1];
	RenderStyle.m_Texture_S2		= Texture2 >= (uint32)nListNum ? NULL : pTextureList[Texture2];
	RenderStyle.m_Texture_S3		= Texture3 >= (uint32)nListNum ? NULL : pTextureList[Texture3];

	RenderStyle.m_Material.Ambient	= GetMapValue( m_Ambient, Pos );
	RenderStyle.m_Material.Diffuse	= GetMapValue( m_Diffuse, Pos );
	RenderStyle.m_Material.Specular	= GetMapValue( m_Specular, Pos );
	RenderStyle.m_Material.Emissive	= GetMapValue( m_Emissive, Pos );
	RenderStyle.m_Material.Power	= GetMapValue( m_Specularpower, Pos );
	RenderStyle.m_Alpharef			= GetMapValue( m_Alpharef, Pos );
	RenderStyle.m_Texturefactor		= GetMapValue( m_Texturefactor, Pos );

	RenderStyle.m_AlphaBlendEnable	= m_AlphaBlendEnable;
	RenderStyle.m_SrcBlend			= m_SrcBlend;
	RenderStyle.m_DestBlend			= m_DestBlend;

	RenderStyle.m_AlphaTestEnable	= m_AlphaTestEnable;
	RenderStyle.m_AlphaTestFun		= m_AlphaTestFun;

	RenderStyle.m_LightEnable		= m_LightEnable;
	RenderStyle.m_SpecularEnable	= m_SpecularEnable;

	RenderStyle.m_ZTestEnable		= m_ZTestEnable;
	RenderStyle.m_ZTestFun			= m_ZTestFun;
	RenderStyle.m_ZWrite			= m_ZWrite;

	RenderStyle.m_Cull				= m_Cull;
	RenderStyle.m_FillMode			= m_FillMode;

	RenderStyle.m_Uv_S0				= m_Uv_S0;//.begin()->second;//GetMapState(m_Uv_S0,Pos);
	RenderStyle.m_Uv_S1				= m_Uv_S1;//.begin()->second;//GetMapState(m_Uv_S1,Pos);
	RenderStyle.m_Uv_S2				= m_Uv_S2;//.begin()->second;//GetMapState(m_Uv_S2,Pos);

	RenderStyle.m_ShaderType		= m_ShaderType;

	RenderStyle.m_Sample_S0			= m_Sample_S0;//.begin()->second;//GetMapState( m_Sample_S0, Pos );
	RenderStyle.m_Sample_S1			= m_Sample_S1;//.begin()->second;//GetMapState( m_Sample_S1, Pos );
	RenderStyle.m_Sample_S2			= m_Sample_S2;//.begin()->second;//GetMapState( m_Sample_S2, Pos );
	RenderStyle.m_Sample_S3			= m_Sample_S3;//.begin()->second;//GetMapState( m_Sample_S3, Pos );

	RenderStyle.m_TexOP_S0			= m_TexOP_S0;//.begin()->second;//GetMapState( m_TexOP_S0, Pos );
	RenderStyle.m_TexOP_S1			= m_TexOP_S1;//.begin()->second;//GetMapState( m_TexOP_S1, Pos );
	RenderStyle.m_TexOP_S2			= m_TexOP_S2;//.begin()->second;//GetMapState( m_TexOP_S2, Pos );
	RenderStyle.m_TexOP_S3			= m_TexOP_S3;//.begin()->second;//GetMapState( m_TexOP_S3, Pos );
	RenderStyle.m_TexArg1_S0		= m_TexArg1_S0;//.begin()->second;//GetMapState( m_TexArg1_S0, Pos );
	RenderStyle.m_TexArg1_S1		= m_TexArg1_S1;//.begin()->second;//GetMapState( m_TexArg1_S1, Pos );
	RenderStyle.m_TexArg1_S2		= m_TexArg1_S2;//.begin()->second;//GetMapState( m_TexArg1_S2, Pos );
	RenderStyle.m_TexArg1_S3		= m_TexArg1_S3;//.begin()->second;//GetMapState( m_TexArg1_S3, Pos );
	RenderStyle.m_TexArg2_S0		= m_TexArg2_S0;//.begin()->second;//GetMapState( m_TexArg2_S0, Pos );
	RenderStyle.m_TexArg2_S1		= m_TexArg2_S1;//.begin()->second;//GetMapState( m_TexArg2_S1, Pos );
	RenderStyle.m_TexArg2_S2		= m_TexArg2_S2;//.begin()->second;//GetMapState( m_TexArg2_S2, Pos );
	RenderStyle.m_TexArg2_S3		= m_TexArg2_S3;//.begin()->second;//GetMapState( m_TexArg2_S3, Pos );
	RenderStyle.m_AlpOP_S0			= m_AlpOP_S0;//.begin()->second;//GetMapState( m_AlpOP_S0, Pos );
	RenderStyle.m_AlpOP_S1			= m_AlpOP_S1;//.begin()->second;//GetMapState( m_AlpOP_S1, Pos );
	RenderStyle.m_AlpOP_S2			= m_AlpOP_S2;//.begin()->second;//GetMapState( m_AlpOP_S2, Pos );
	RenderStyle.m_AlpOP_S3			= m_AlpOP_S3;//.begin()->second;//GetMapState( m_AlpOP_S3, Pos );
	RenderStyle.m_AlpArg1_S0		= m_AlpArg1_S0;//.begin()->second;//GetMapState( m_AlpArg1_S0, Pos );
	RenderStyle.m_AlpArg1_S1		= m_AlpArg1_S1;//.begin()->second;//GetMapState( m_AlpArg1_S1, Pos );
	RenderStyle.m_AlpArg1_S2		= m_AlpArg1_S2;//.begin()->second;//GetMapState( m_AlpArg1_S2, Pos );
	RenderStyle.m_AlpArg1_S3		= m_AlpArg1_S3;//.begin()->second;//GetMapState( m_AlpArg1_S3, Pos );
	RenderStyle.m_AlpArg2_S0		= m_AlpArg2_S0;//.begin()->second;//GetMapState( m_AlpArg2_S0, Pos );
	RenderStyle.m_AlpArg2_S1		= m_AlpArg2_S1;//.begin()->second;//GetMapState( m_AlpArg2_S1, Pos );
	RenderStyle.m_AlpArg2_S2		= m_AlpArg2_S2;//.begin()->second;//GetMapState( m_AlpArg2_S2, Pos );
	RenderStyle.m_AlpArg2_S3		= m_AlpArg2_S3;//.begin()->second;//GetMapState( m_AlpArg2_S3, Pos );
	RenderStyle.m_UVSrc_S0			= m_UVSrc_S0;//.begin()->second;//GetMapState( m_UVSrc_S0, Pos );
	RenderStyle.m_UVSrc_S1			= m_UVSrc_S1;//.begin()->second;//GetMapState( m_UVSrc_S1, Pos );
	RenderStyle.m_UVSrc_S2			= m_UVSrc_S2;//.begin()->second;//GetMapState( m_UVSrc_S2, Pos );
	RenderStyle.m_UVSrc_S3			= m_UVSrc_S3;//.begin()->second;//GetMapState( m_UVSrc_S3, Pos );
	RenderStyle.m_UAddr_S0			= m_UAddr_S0;//.begin()->second;//GetMapState( m_UAddr_S0, Pos );
	RenderStyle.m_UAddr_S1			= m_UAddr_S1;//.begin()->second;//GetMapState( m_UAddr_S1, Pos );
	RenderStyle.m_UAddr_S2			= m_UAddr_S2;//.begin()->second;//GetMapState( m_UAddr_S2, Pos );
	RenderStyle.m_UAddr_S3			= m_UAddr_S3;//.begin()->second;//GetMapState( m_UAddr_S3, Pos );
	RenderStyle.m_VAddr_S0			= m_VAddr_S0;//.begin()->second;//GetMapState( m_VAddr_S0, Pos );
	RenderStyle.m_VAddr_S1			= m_VAddr_S1;//.begin()->second;//GetMapState( m_VAddr_S1, Pos );
	RenderStyle.m_VAddr_S2			= m_VAddr_S2;//.begin()->second;//GetMapState( m_VAddr_S2, Pos );
	RenderStyle.m_VAddr_S3			= m_VAddr_S3;//.begin()->second;//GetMapState( m_VAddr_S3, Pos );
	RenderStyle.m_MipMapSample_S0	= m_MipMapSample_S0;//.begin()->second;//GetMapState( m_MipMapSample_S0, Pos );
	RenderStyle.m_MipMapSample_S1	= m_MipMapSample_S1;//.begin()->second;//GetMapState( m_MipMapSample_S1, Pos );
	RenderStyle.m_MipMapSample_S2	= m_MipMapSample_S2;//.begin()->second;//GetMapState( m_MipMapSample_S2, Pos );
	RenderStyle.m_MipMapSample_S3	= m_MipMapSample_S3;//.begin()->second;//GetMapState( m_MipMapSample_S3, Pos );
	RenderStyle.m_MipMapBias_S0		= m_MipMapBias_S0;//.begin()->second;//GetMapState( m_MipMapBias_S0, Pos );
	RenderStyle.m_MipMapBias_S1		= m_MipMapBias_S1;//.begin()->second;//GetMapState( m_MipMapBias_S1, Pos );
	RenderStyle.m_MipMapBias_S2		= m_MipMapBias_S2;//.begin()->second;//GetMapState( m_MipMapBias_S2, Pos );
	RenderStyle.m_MipMapBias_S3		= m_MipMapBias_S3;//.begin()->second;//GetMapState( m_MipMapBias_S3, Pos );
}

int CRenderStyle::BinLoad( void )
{
	if ( m_szName.empty() )
	{		
		return GERROR;
	}

	if ( FAILED( CreateResFile(  ) ) )
		return GERROR;

	return 0;
}

template< class DataType >
inline void LoadMapValue( CBufFile& QuickFile, GMap<float,DataType>& DataMap )
{
	DataMap.clear();
	int Num;
	QuickFile.read( &Num, sizeof( Num ) );
	for ( int i = 0; i < Num; i++ )
	{
		DataType Data;
		float Pos;
		QuickFile.read( &Pos, sizeof( float ) );
		QuickFile.read( &Data, sizeof( DataType ) );
		DataMap[Pos] = Data;
	}
}

void CRenderStyle::OnRead( CRefBufFile& GraphicFile )
{
	uint32 Version;
	uint32 Res;
	GraphicFile.read( &Res, sizeof(uint32) );
	GraphicFile.read( &Version, sizeof(uint32) );

	if ( Res != MAKEFOURCC( 'M','A', 'T', 0 ) )
		GenErr(GraphicErr::GetErrTypeStr(FileFormat_Err));

	m_bOnRead = true;
	GraphicFile.read( &m_TimeRang, sizeof(float) );
	if ( Version >= 10005 )
		GraphicFile.read( (char*)&m_bSyn, sizeof( bool ) );

	LoadMapValue( GraphicFile, m_Ambient );
	LoadMapValue( GraphicFile, m_Diffuse );
	LoadMapValue( GraphicFile, m_Specular );
	LoadMapValue( GraphicFile, m_Emissive );
	LoadMapValue( GraphicFile, m_Specularpower );
	LoadMapValue( GraphicFile, m_Alpharef );
	LoadMapValue( GraphicFile, m_Texturefactor );

	GMap<float, uint32> temp_map;

	LoadMapValue( GraphicFile, temp_map );
	m_AlphaBlendEnable = temp_map[0];
	LoadMapValue( GraphicFile, temp_map );
	m_SrcBlend = temp_map[0];
	LoadMapValue( GraphicFile, temp_map );
	m_DestBlend = temp_map[0];

	LoadMapValue( GraphicFile, temp_map );
	m_AlphaTestEnable = temp_map[0];
	LoadMapValue( GraphicFile, temp_map );
	m_AlphaTestFun = temp_map[0];

	LoadMapValue( GraphicFile, temp_map );
	m_LightEnable = temp_map[0];
	LoadMapValue( GraphicFile, temp_map );
	m_SpecularEnable = temp_map[0];

	LoadMapValue( GraphicFile, temp_map );
	m_ZTestEnable = temp_map[0];
	LoadMapValue( GraphicFile, temp_map );
	m_ZTestFun = temp_map[0];
	LoadMapValue( GraphicFile, temp_map );
	m_ZWrite = temp_map[0];

	LoadMapValue( GraphicFile, temp_map );
	m_Cull = temp_map[0];
	LoadMapValue( GraphicFile, temp_map );
	m_FillMode = temp_map[0];
	CSyncSystem::TryToStop();

	if (Version >= 10006)
	{
		LoadMapValue( GraphicFile, temp_map);
		m_Uv_S0 = temp_map.begin()->second;
		LoadMapValue( GraphicFile, temp_map);
		m_Uv_S1 = temp_map.begin()->second;
		LoadMapValue( GraphicFile, temp_map);
		m_Uv_S2 = temp_map.begin()->second;
	}
	if (Version == 10007)
	{
		DWORD tempID;
		GraphicFile.read( &tempID, sizeof(DWORD) );
		m_ShaderType = tempID;
	}

	if (Version>=10008)
	{
		GraphicFile.read( &m_ShaderType, sizeof(SHADER_HANDLE) );
	}
	if(CGpuProgramMgr::GetInst()->GetShaderVer() < Shader_V_2 ) // 以后想办法
		m_ShaderType = uint64(0);

	LoadMapValue( GraphicFile, m_Texture_S0 );
	LoadMapValue( GraphicFile, m_Texture_S1 );
	LoadMapValue( GraphicFile, m_Texture_S2 );
	LoadMapValue( GraphicFile, m_Texture_S3 );

	LoadMapValue( GraphicFile, temp_map );
	m_Sample_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_Sample_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_Sample_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_Sample_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_TexOP_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_TexOP_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_TexOP_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_TexOP_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_TexArg1_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_TexArg1_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_TexArg1_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_TexArg1_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_TexArg2_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_TexArg2_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_TexArg2_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_TexArg2_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_AlpOP_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_AlpOP_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_AlpOP_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_AlpOP_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_AlpArg1_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_AlpArg1_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_AlpArg1_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_AlpArg1_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_AlpArg2_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_AlpArg2_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_AlpArg2_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_AlpArg2_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_UVSrc_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_UVSrc_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_UVSrc_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_UVSrc_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_UAddr_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_UAddr_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_UAddr_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_UAddr_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_VAddr_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_VAddr_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_VAddr_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_VAddr_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_MipMapSample_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_MipMapSample_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_MipMapSample_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_MipMapSample_S3 = temp_map.begin()->second;

	LoadMapValue( GraphicFile, temp_map );
	m_MipMapBias_S0 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_MipMapBias_S1 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_MipMapBias_S2 = temp_map.begin()->second;
	LoadMapValue( GraphicFile, temp_map );
	m_MipMapBias_S3 = temp_map.begin()->second;

	m_bOnRead = false;
}

bool CRenderStyle::IsAlphaBlend( float Time )
{
	return m_AlphaBlendEnable != 0;
}
// void CRenderStyle::SetRenderStyleTexOP(_D3DTEXTUREOP type)
// {
// 	m_TexOP_S0[0.0f] = type;
// }

void STATIC_RS::TexAddRef()
{
	if ( m_Texture_S0 )
		m_Texture_S0->AddTexRef();

	if ( m_Texture_S1 )
		m_Texture_S1->AddTexRef();

	if ( m_Texture_S2 )
		 m_Texture_S2->AddTexRef();

	if ( m_Texture_S3 )
		m_Texture_S3->AddTexRef();
}

void STATIC_RS::TexRelease()
{
	if ( m_Texture_S0 )
		m_Texture_S0->ReleaseTexRef();

	if ( m_Texture_S1 )
		m_Texture_S1->ReleaseTexRef();

	if ( m_Texture_S2 )
		m_Texture_S2->ReleaseTexRef();

	if ( m_Texture_S3 )
		m_Texture_S3->ReleaseTexRef();
}