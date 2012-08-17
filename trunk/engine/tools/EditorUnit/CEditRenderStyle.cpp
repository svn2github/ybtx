#include "stdafx.h"
#include "CEditRenderStyle.h"
#include "GraphicCommon.h"
#include <fstream>

CEditRsFormat CEditRenderStyle::Format;

CEditRenderStyle::CEditRenderStyle( const string& szName, const wstring& szAlias )
	: CRenderStyle( szName, szAlias )
	, CEditInfo(&Format)
{

}

CEditRenderStyle::CEditRenderStyle(void)
	: CRenderStyle( "" ,L"")
	, CEditInfo(&Format)
{
}

CEditRenderStyle::CEditRenderStyle( const CRenderStyle& Style )
	: CRenderStyle( Style.GetName(), Style.GetAlias() )
	, CEditInfo(&Format)
{
	m_Ref = Style.m_Ref;
	m_TimeRang = Style.m_TimeRang;
	m_bSyn     = Style.m_bSyn;

	m_Ambient		= Style.m_Ambient;	
	m_Diffuse		= Style.m_Diffuse;
	m_Specular		= Style.m_Specular;
	m_Emissive		= Style.m_Emissive;;
	m_Specularpower	= Style.m_Specularpower;

	m_Alpharef		= Style.m_Alpharef;
	m_Texturefactor	= Style.m_Texturefactor;

	m_AlphaBlendEnable	= Style.m_AlphaBlendEnable;
	m_SrcBlend			= Style.m_SrcBlend;
	m_DestBlend			= Style.m_DestBlend;

	m_AlphaTestEnable	= Style.m_AlphaTestEnable;
	m_AlphaTestFun		= Style.m_AlphaTestFun;
	m_LightEnable		= Style.m_LightEnable;
	m_SpecularEnable	= Style.m_SpecularEnable;

	m_ZTestEnable		= Style.m_ZTestEnable;
	m_ZWrite			= Style.m_ZWrite;
	m_ZTestFun			= Style.m_ZTestFun;

	m_Cull				= Style.m_Cull ;
	m_FillMode			= Style.m_FillMode;


	m_Uv_S0				= Style.m_Uv_S0;
	m_Uv_S1				= Style.m_Uv_S1;
	m_Uv_S2				= Style.m_Uv_S2;

	m_Texture_S0		= Style.m_Texture_S0;
	m_Texture_S1		= Style.m_Texture_S1;
	m_Texture_S2		= Style.m_Texture_S2;
	m_Texture_S3		= Style.m_Texture_S3;
	m_Sample_S0			= Style.m_Sample_S0;
	m_Sample_S1			= Style.m_Sample_S1;
	m_Sample_S2			= Style.m_Sample_S2;
	m_Sample_S3			= Style.m_Sample_S3;
	m_TexOP_S0			= Style.m_TexOP_S0;
	m_TexOP_S1			= Style.m_TexOP_S1;
	m_TexOP_S2			= Style.m_TexOP_S2;
	m_TexOP_S3			= Style.m_TexOP_S3;
	m_TexArg1_S0		= Style.m_TexArg1_S0;
	m_TexArg1_S1		= Style.m_TexArg1_S1;
	m_TexArg1_S2		= Style.m_TexArg1_S2;
	m_TexArg1_S3		= Style.m_TexArg1_S3;
	m_TexArg2_S0		= Style.m_TexArg2_S0;
	m_TexArg2_S1		= Style.m_TexArg2_S1;
	m_TexArg2_S2		= Style.m_TexArg2_S2;
	m_TexArg2_S3		= Style.m_TexArg2_S3;
	m_AlpOP_S0			= Style.m_AlpOP_S0;
	m_AlpOP_S1			= Style.m_AlpOP_S1;
	m_AlpOP_S2			= Style.m_AlpOP_S2;
	m_AlpOP_S3			= Style.m_AlpOP_S3;
	m_AlpArg1_S0		= Style.m_AlpArg1_S0;
	m_AlpArg1_S1		= Style.m_AlpArg1_S1;
	m_AlpArg1_S2		= Style.m_AlpArg1_S2;
	m_AlpArg1_S3		= Style.m_AlpArg1_S3;
	m_AlpArg2_S0		= Style.m_AlpArg2_S0;
	m_AlpArg2_S1		= Style.m_AlpArg2_S1;
	m_AlpArg2_S2		= Style.m_AlpArg2_S2;
	m_AlpArg2_S3		= Style.m_AlpArg2_S3;
	m_UVSrc_S0			= Style.m_UVSrc_S0;
	m_UVSrc_S1			= Style.m_UVSrc_S1;
	m_UVSrc_S2			= Style.m_UVSrc_S2;
	m_UVSrc_S3			= Style.m_UVSrc_S3;
	m_UAddr_S0			= Style.m_UAddr_S0;
	m_UAddr_S1			= Style.m_UAddr_S1;
	m_UAddr_S2			= Style.m_UAddr_S2;
	m_UAddr_S3			= Style.m_UAddr_S3;
	m_VAddr_S0			= Style.m_VAddr_S0;
	m_VAddr_S1			= Style.m_VAddr_S1;
	m_VAddr_S2			= Style.m_VAddr_S2;
	m_VAddr_S3			= Style.m_VAddr_S3;
	m_MipMapSample_S0	= Style.m_MipMapSample_S0;
	m_MipMapSample_S1	= Style.m_MipMapSample_S1;
	m_MipMapSample_S2	= Style.m_MipMapSample_S2;
	m_MipMapSample_S3	= Style.m_MipMapSample_S3;
	m_MipMapBias_S0		= Style.m_MipMapBias_S0;
	m_MipMapBias_S1		= Style.m_MipMapBias_S1;
	m_MipMapBias_S2		= Style.m_MipMapBias_S2;
	m_MipMapBias_S3		= Style.m_MipMapBias_S3;
}

int CEditRenderStyle::BinSave( const string& SaveName )
{
	SetFileAttributes( SaveName.c_str(), FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE );
	if( SaveName.empty() )
		return 0;
	locale loc = locale::global(locale("")); //要打开的文件路径包含中文，设置全局locale为本地环境
	ofstream File( SaveName.c_str(), ios::binary|ios::out );
	locale::global(loc);
	if( File.fail() )
		return GERROR;

	DWORD Res = MAKEFOURCC( 'M', 'A', 'T', 0 );
	DWORD Version = RS_VERSION;
	File.write( (char*)&Res, sizeof(DWORD) );
	File.write( (char*)&Version, sizeof(DWORD) );
	File.write( (char*)&m_TimeRang, sizeof(float) );
	File.write( (char*)&m_bSyn, sizeof(bool) );

	sqr::SaveMapValue( File, m_Ambient );
	sqr::SaveMapValue( File, m_Diffuse );
	sqr::SaveMapValue( File, m_Specular );
	sqr::SaveMapValue( File, m_Emissive );
	sqr::SaveMapValue( File, m_Specularpower );
	sqr::SaveMapValue( File, m_Alpharef );
	sqr::SaveMapValue( File, m_Texturefactor );

	map<float, uint32> temp_map;

	temp_map[0] = m_AlphaBlendEnable;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_SrcBlend;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_DestBlend;
	sqr::SaveMapValue( File, temp_map );

	temp_map[0] = m_AlphaTestEnable;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlphaTestFun;
	sqr::SaveMapValue( File, temp_map );

	temp_map[0] = m_LightEnable;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_SpecularEnable;
	sqr::SaveMapValue( File, temp_map );

	temp_map[0] = m_ZTestEnable;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_ZTestFun;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_ZWrite;
	sqr::SaveMapValue( File, temp_map );

	temp_map[0] = m_Cull;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_FillMode;
	sqr::SaveMapValue( File, temp_map );

	temp_map[0] = m_Uv_S0;
	sqr::SaveMapValue( File, temp_map);
	temp_map[0] = m_Uv_S1;
	sqr::SaveMapValue( File, temp_map);
	temp_map[0] = m_Uv_S2;
	sqr::SaveMapValue( File, temp_map);

	File.write( (TCHAR*)&m_ShaderType, sizeof(m_ShaderType) );

	sqr::SaveMapValue( File, m_Texture_S0 );
	sqr::SaveMapValue( File, m_Texture_S1 );
	sqr::SaveMapValue( File, m_Texture_S2 );
	sqr::SaveMapValue( File, m_Texture_S3 );

	temp_map[0] = m_Sample_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_Sample_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_Sample_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_Sample_S3;
	sqr::SaveMapValue( File, temp_map );

	//	std::GMap<float,DWORD>::iterator iter = m_TexOP_S0.begin();

	// 	for( iter; iter != m_TexOP_S0.end(); ++iter )
	// 	{
	// 		if( iter->second == 6 )
	// 			iter->second = 5;
	// 	}

	temp_map[0] = m_TexOP_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexOP_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexOP_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexOP_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexArg1_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexArg1_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexArg1_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexArg1_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexArg2_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexArg2_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexArg2_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_TexArg2_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpOP_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpOP_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpOP_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpOP_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpArg1_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpArg1_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpArg1_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpArg1_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpArg2_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpArg2_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpArg2_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_AlpArg2_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_UVSrc_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_UVSrc_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_UVSrc_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_UVSrc_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_UAddr_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_UAddr_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_UAddr_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_UAddr_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_VAddr_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_VAddr_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_VAddr_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_VAddr_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_MipMapSample_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_MipMapSample_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_MipMapSample_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_MipMapSample_S3;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_MipMapBias_S0;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_MipMapBias_S1;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_MipMapBias_S2;
	sqr::SaveMapValue( File, temp_map );
	temp_map[0] = m_MipMapBias_S3;
	sqr::SaveMapValue( File, temp_map );

	return 0;
}

int	CEditRenderStyle::ReLoad( const string& FileName )
{
	if( m_ppMyself == NULL )
	{
		m_ppMyself  = new CBaseGraphicRead*;
		*m_ppMyself = NULL;
	}
	m_szName = FileName.c_str();
	CRenderStyle::BinLoad();
	//if ( FAILED( CreateResFile() ) )
	//	return GERROR;

	return 0;
}

//CEditRenderStyle& CEditRenderStyle::operator = (const CEditRenderStyle& style)
//{
//	if( this != &style )
//		memcpy(this,&style,sizeof(CEditRenderStyle));
//	return *this;
//}