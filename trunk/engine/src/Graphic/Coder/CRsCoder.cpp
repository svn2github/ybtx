#include "stdafx.h"
#include "GraphicCommon.h"
#include "CRsCoder.h"
#include "CRenderStyle.h"
#include "GraphicBase.h"
#include "SyncSystem.h"
#include "CGpuProgramMgr.h"

CRsFormat::CRsFormat()
	: CFileFormatMgr('MAT\0', 0)
{
}

CRsFormat::~CRsFormat()
{
}

BEGIN_VERSION_MAP(CRsCoder)
	ON_VERSION(2, DeCode_old_10007)
	ON_VERSION(3, DeCode_old_10008)
END_VERSION_MAP()

template< class DataType>
void SaveRsMapValue( WriteDataInf& wdi, DataType& data_map )
{
	int num = (int)data_map.size();
	wdi.Write( &num, sizeof( num ) );
	for ( typename DataType::const_iterator it = data_map.begin(); it != data_map.end(); it++ )
	{
		wdi.Write( &it->first, sizeof( float ) );
		wdi.Write( &it->second, sizeof( typename DataType::mapped_type ) );
	}
}

template< class DataType >
inline void LoadRsMapValue( CBufFile& file, DataType& data_map )
{
	data_map.clear();
	int num = 0;
	file.SafeRead( &num, sizeof( num ) );
	for ( int i = 0; i < num; ++i )
	{
		typename DataType::mapped_type data;
		float pos = 0.0f;
		file.SafeRead( &pos,  sizeof( float ) );
		file.SafeRead( &data, sizeof( typename DataType::mapped_type ) );
		data_map[pos] = data;
	}
}

bool CRsCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CRenderStyle* rs = (CRenderStyle*)handle;

	//wdi.Write( &rs->m_TimeRang, sizeof(float) );
	//wdi.Write( &rs->m_bSyn, sizeof(bool) );

	//SaveRsMapValue( wdi, rs->m_Ambient );
	//SaveRsMapValue( wdi, rs->m_Diffuse );
	//SaveRsMapValue( wdi, rs->m_Specular );
	//SaveRsMapValue( wdi, rs->m_Emissive );
	//SaveRsMapValue( wdi, rs->m_Specularpower );
	//SaveRsMapValue( wdi, rs->m_Alpharef );
	//SaveRsMapValue( wdi, rs->m_Texturefactor );

	//wdi.Write(&rs->m_AlphaBlendEnable, sizeof(rs->m_AlphaBlendEnable));
	//wdi.Write(&rs->m_SrcBlend, sizeof(rs->m_SrcBlend));
	//wdi.Write(&rs->m_DestBlend, sizeof(rs->m_DestBlend));
	//wdi.Write(&rs->m_AlphaTestEnable, sizeof(rs->m_AlphaTestEnable));
	//wdi.Write(&rs->m_AlphaTestFun, sizeof(rs->m_AlphaTestFun));
	//wdi.Write(&rs->m_LightEnable, sizeof(rs->m_LightEnable));
	//wdi.Write(&rs->m_SpecularEnable, sizeof(rs->m_SpecularEnable));
	//wdi.Write(&rs->m_ZTestEnable, sizeof(rs->m_ZTestEnable));
	//wdi.Write(&rs->m_ZTestFun, sizeof(rs->m_ZTestFun));
	//wdi.Write(&rs->m_ZWrite, sizeof(rs->m_ZWrite));
	//wdi.Write(&rs->m_Cull, sizeof(rs->m_Cull));
	//wdi.Write(&rs->m_FillMode, sizeof(rs->m_FillMode));

	//SaveRsMapValue( wdi, rs->m_Uv_S0);
	//SaveRsMapValue( wdi, rs->m_Uv_S1);
	//SaveRsMapValue( wdi, rs->m_Uv_S2);

	//wdi.Write( &rs->m_ShaderType, sizeof(rs->m_ShaderType) );

	//SaveRsMapValue( wdi, rs->m_Texture_S0 );
	//SaveRsMapValue( wdi, rs->m_Texture_S1 );
	//SaveRsMapValue( wdi, rs->m_Texture_S2 );
	//SaveRsMapValue( wdi, rs->m_Texture_S3 );

	//SaveRsMapValue( wdi, rs->m_Sample_S0 );
	//SaveRsMapValue( wdi, rs->m_Sample_S1 );
	//SaveRsMapValue( wdi, rs->m_Sample_S2 );
	//SaveRsMapValue( wdi, rs->m_Sample_S3 );

	//SaveRsMapValue( wdi, rs->m_TexOP_S0 );
	//SaveRsMapValue( wdi, rs->m_TexOP_S1 );
	//SaveRsMapValue( wdi, rs->m_TexOP_S2 );
	//SaveRsMapValue( wdi, rs->m_TexOP_S3 );
	//SaveRsMapValue( wdi, rs->m_TexArg1_S0 );
	//SaveRsMapValue( wdi, rs->m_TexArg1_S1 );
	//SaveRsMapValue( wdi, rs->m_TexArg1_S2 );
	//SaveRsMapValue( wdi, rs->m_TexArg1_S3 );
	//SaveRsMapValue( wdi, rs->m_TexArg2_S0 );
	//SaveRsMapValue( wdi, rs->m_TexArg2_S1 );
	//SaveRsMapValue( wdi, rs->m_TexArg2_S2 );
	//SaveRsMapValue( wdi, rs->m_TexArg2_S3 );
	//SaveRsMapValue( wdi, rs->m_AlpOP_S0 );
	//SaveRsMapValue( wdi, rs->m_AlpOP_S1 );
	//SaveRsMapValue( wdi, rs->m_AlpOP_S2 );
	//SaveRsMapValue( wdi, rs->m_AlpOP_S3 );
	//SaveRsMapValue( wdi, rs->m_AlpArg1_S0 );
	//SaveRsMapValue( wdi, rs->m_AlpArg1_S1 );
	//SaveRsMapValue( wdi, rs->m_AlpArg1_S2 );
	//SaveRsMapValue( wdi, rs->m_AlpArg1_S3 );
	//SaveRsMapValue( wdi, rs->m_AlpArg2_S0 );
	//SaveRsMapValue( wdi, rs->m_AlpArg2_S1 );
	//SaveRsMapValue( wdi, rs->m_AlpArg2_S2 );
	//SaveRsMapValue( wdi, rs->m_AlpArg2_S3 );
	//SaveRsMapValue( wdi, rs->m_UVSrc_S0 );
	//SaveRsMapValue( wdi, rs->m_UVSrc_S1 );
	//SaveRsMapValue( wdi, rs->m_UVSrc_S2 );
	//SaveRsMapValue( wdi, rs->m_UVSrc_S3 );
	//SaveRsMapValue( wdi, rs->m_UAddr_S0 );
	//SaveRsMapValue( wdi, rs->m_UAddr_S1 );
	//SaveRsMapValue( wdi, rs->m_UAddr_S2 );
	//SaveRsMapValue( wdi, rs->m_UAddr_S3 );
	//SaveRsMapValue( wdi, rs->m_VAddr_S0 );
	//SaveRsMapValue( wdi, rs->m_VAddr_S1 );
	//SaveRsMapValue( wdi, rs->m_VAddr_S2 );
	//SaveRsMapValue( wdi, rs->m_VAddr_S3 );
	//SaveRsMapValue( wdi, rs->m_MipMapSample_S0 );
	//SaveRsMapValue( wdi, rs->m_MipMapSample_S1 );
	//SaveRsMapValue( wdi, rs->m_MipMapSample_S2 );
	//SaveRsMapValue( wdi, rs->m_MipMapSample_S3 );

	//SaveRsMapValue( wdi, rs->m_MipMapBias_S0 );
	//SaveRsMapValue( wdi, rs->m_MipMapBias_S1 );
	//SaveRsMapValue( wdi, rs->m_MipMapBias_S2 );
	//SaveRsMapValue( wdi, rs->m_MipMapBias_S3 );

	return true;
}

bool CRsCoder::DeCode_old_10007( const CDataChunk& chunk, OBJ_HANDLE handle )
{
	CRenderStyle* rs = (CRenderStyle*)handle;
	CBufFile chunk_buf( chunk.GetBuffer(), chunk.GetSize() );

	chunk_buf.SafeRead( &rs->m_TimeRang, sizeof(float) );
	chunk_buf.SafeRead( &rs->m_bSyn, sizeof( bool ) );

	LoadRsMapValue( chunk_buf, rs->m_Ambient );
	LoadRsMapValue( chunk_buf, rs->m_Diffuse );
	LoadRsMapValue( chunk_buf, rs->m_Specular );
	LoadRsMapValue( chunk_buf, rs->m_Emissive );
	LoadRsMapValue( chunk_buf, rs->m_Specularpower );
	LoadRsMapValue( chunk_buf, rs->m_Alpharef );
	LoadRsMapValue( chunk_buf, rs->m_Texturefactor );

	chunk_buf.SafeRead(&rs->m_AlphaBlendEnable, sizeof(rs->m_AlphaBlendEnable));
	chunk_buf.SafeRead(&rs->m_SrcBlend,			sizeof(rs->m_SrcBlend));
	chunk_buf.SafeRead(&rs->m_DestBlend,		sizeof(rs->m_DestBlend));
	chunk_buf.SafeRead(&rs->m_AlphaTestEnable,	sizeof(rs->m_AlphaTestEnable));
	chunk_buf.SafeRead(&rs->m_AlphaTestFun,		sizeof(rs->m_AlphaTestFun));
	chunk_buf.SafeRead(&rs->m_LightEnable,		sizeof(rs->m_LightEnable));
	chunk_buf.SafeRead(&rs->m_SpecularEnable,	sizeof(rs->m_SpecularEnable));
	chunk_buf.SafeRead(&rs->m_ZTestEnable,		sizeof(rs->m_ZTestEnable));
	chunk_buf.SafeRead(&rs->m_ZTestFun,			sizeof(rs->m_ZTestFun));
	chunk_buf.SafeRead(&rs->m_ZWrite,			sizeof(rs->m_ZWrite));
	chunk_buf.SafeRead(&rs->m_Cull,				sizeof(rs->m_Cull));
	chunk_buf.SafeRead(&rs->m_FillMode,			sizeof(rs->m_FillMode));
	CSyncSystem::TryToStop();

	//LoadRsMapValue( chunk_buf, rs->m_Uv_S0);
	//LoadRsMapValue( chunk_buf, rs->m_Uv_S1);
	//LoadRsMapValue( chunk_buf, rs->m_Uv_S2);

	DWORD tempID;
	chunk_buf.SafeRead( &tempID, sizeof(DWORD) );
	rs->m_ShaderType = tempID;
	
	if(CGpuProgramMgr::GetInst()->GetShaderVer() < Shader_V_2 ) // 以后想办法
		rs->m_ShaderType = uint64(0);

	//LoadRsMapValue( chunk_buf, rs->m_Texture_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_Texture_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_Texture_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_Texture_S3 );

	//LoadRsMapValue( chunk_buf, rs->m_Sample_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_Sample_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_Sample_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_Sample_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_TexOP_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_TexOP_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_TexOP_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_TexOP_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_TexArg1_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_TexArg1_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_TexArg1_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_TexArg1_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_TexArg2_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_TexArg2_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_TexArg2_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_TexArg2_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpOP_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpOP_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpOP_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpOP_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpArg1_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpArg1_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpArg1_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpArg1_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpArg2_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpArg2_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpArg2_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_AlpArg2_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_UVSrc_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_UVSrc_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_UVSrc_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_UVSrc_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_UAddr_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_UAddr_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_UAddr_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_UAddr_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_VAddr_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_VAddr_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_VAddr_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_VAddr_S3 );
	//CSyncSystem::TryToStop();
	//LoadRsMapValue( chunk_buf, rs->m_MipMapSample_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_MipMapSample_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_MipMapSample_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_MipMapSample_S3 );
	//LoadRsMapValue( chunk_buf, rs->m_MipMapBias_S0 );
	//LoadRsMapValue( chunk_buf, rs->m_MipMapBias_S1 );
	//LoadRsMapValue( chunk_buf, rs->m_MipMapBias_S2 );
	//LoadRsMapValue( chunk_buf, rs->m_MipMapBias_S3 );

	return true;
}

bool CRsCoder::DeCode_old_10008( const CDataChunk& chunk, OBJ_HANDLE handle )
{
	CRenderStyle* rs = (CRenderStyle*)handle;
	CBufFile chunk_buf( chunk.GetBuffer(), chunk.GetSize() );

	chunk_buf.SafeRead( &rs->m_TimeRang, sizeof(float) );
	chunk_buf.SafeRead( &rs->m_bSyn, sizeof( bool ) );

	LoadRsMapValue( chunk_buf, rs->m_Ambient );
	LoadRsMapValue( chunk_buf, rs->m_Diffuse );
	LoadRsMapValue( chunk_buf, rs->m_Specular );
	LoadRsMapValue( chunk_buf, rs->m_Emissive );
	LoadRsMapValue( chunk_buf, rs->m_Specularpower );
	LoadRsMapValue( chunk_buf, rs->m_Alpharef );
	LoadRsMapValue( chunk_buf, rs->m_Texturefactor );

	chunk_buf.SafeRead(&rs->m_AlphaBlendEnable, sizeof(rs->m_AlphaBlendEnable));
	chunk_buf.SafeRead(&rs->m_SrcBlend,			sizeof(rs->m_SrcBlend));
	chunk_buf.SafeRead(&rs->m_DestBlend,		sizeof(rs->m_DestBlend));
	chunk_buf.SafeRead(&rs->m_AlphaTestEnable,	sizeof(rs->m_AlphaTestEnable));
	chunk_buf.SafeRead(&rs->m_AlphaTestFun,		sizeof(rs->m_AlphaTestFun));
	chunk_buf.SafeRead(&rs->m_LightEnable,		sizeof(rs->m_LightEnable));
	chunk_buf.SafeRead(&rs->m_SpecularEnable,	sizeof(rs->m_SpecularEnable));
	chunk_buf.SafeRead(&rs->m_ZTestEnable,		sizeof(rs->m_ZTestEnable));
	chunk_buf.SafeRead(&rs->m_ZTestFun,			sizeof(rs->m_ZTestFun));
	chunk_buf.SafeRead(&rs->m_ZWrite,			sizeof(rs->m_ZWrite));
	chunk_buf.SafeRead(&rs->m_Cull,				sizeof(rs->m_Cull));
	chunk_buf.SafeRead(&rs->m_FillMode,			sizeof(rs->m_FillMode));
	CSyncSystem::TryToStop();

	//LoadRsMapValue( chunk_buf, rs->m_Uv_S0);
	//LoadRsMapValue( chunk_buf, rs->m_Uv_S1);
	//LoadRsMapValue( chunk_buf, rs->m_Uv_S2);
	
	chunk_buf.SafeRead( &rs->m_ShaderType, sizeof(SHADER_HANDLE) );
	
	if(CGpuProgramMgr::GetInst()->GetShaderVer() < Shader_V_2 ) // 以后想办法
		rs->m_ShaderType = uint64(0);

	/*LoadRsMapValue( chunk_buf, rs->m_Texture_S0 );
	LoadRsMapValue( chunk_buf, rs->m_Texture_S1 );
	LoadRsMapValue( chunk_buf, rs->m_Texture_S2 );
	LoadRsMapValue( chunk_buf, rs->m_Texture_S3 );

	LoadRsMapValue( chunk_buf, rs->m_Sample_S0 );
	LoadRsMapValue( chunk_buf, rs->m_Sample_S1 );
	LoadRsMapValue( chunk_buf, rs->m_Sample_S2 );
	LoadRsMapValue( chunk_buf, rs->m_Sample_S3 );
	LoadRsMapValue( chunk_buf, rs->m_TexOP_S0 );
	LoadRsMapValue( chunk_buf, rs->m_TexOP_S1 );
	LoadRsMapValue( chunk_buf, rs->m_TexOP_S2 );
	LoadRsMapValue( chunk_buf, rs->m_TexOP_S3 );
	LoadRsMapValue( chunk_buf, rs->m_TexArg1_S0 );
	LoadRsMapValue( chunk_buf, rs->m_TexArg1_S1 );
	LoadRsMapValue( chunk_buf, rs->m_TexArg1_S2 );
	LoadRsMapValue( chunk_buf, rs->m_TexArg1_S3 );
	LoadRsMapValue( chunk_buf, rs->m_TexArg2_S0 );
	LoadRsMapValue( chunk_buf, rs->m_TexArg2_S1 );
	LoadRsMapValue( chunk_buf, rs->m_TexArg2_S2 );
	LoadRsMapValue( chunk_buf, rs->m_TexArg2_S3 );
	LoadRsMapValue( chunk_buf, rs->m_AlpOP_S0 );
	LoadRsMapValue( chunk_buf, rs->m_AlpOP_S1 );
	LoadRsMapValue( chunk_buf, rs->m_AlpOP_S2 );
	LoadRsMapValue( chunk_buf, rs->m_AlpOP_S3 );
	LoadRsMapValue( chunk_buf, rs->m_AlpArg1_S0 );
	LoadRsMapValue( chunk_buf, rs->m_AlpArg1_S1 );
	LoadRsMapValue( chunk_buf, rs->m_AlpArg1_S2 );
	LoadRsMapValue( chunk_buf, rs->m_AlpArg1_S3 );
	LoadRsMapValue( chunk_buf, rs->m_AlpArg2_S0 );
	LoadRsMapValue( chunk_buf, rs->m_AlpArg2_S1 );
	LoadRsMapValue( chunk_buf, rs->m_AlpArg2_S2 );
	LoadRsMapValue( chunk_buf, rs->m_AlpArg2_S3 );
	LoadRsMapValue( chunk_buf, rs->m_UVSrc_S0 );
	LoadRsMapValue( chunk_buf, rs->m_UVSrc_S1 );
	LoadRsMapValue( chunk_buf, rs->m_UVSrc_S2 );
	LoadRsMapValue( chunk_buf, rs->m_UVSrc_S3 );
	LoadRsMapValue( chunk_buf, rs->m_UAddr_S0 );
	LoadRsMapValue( chunk_buf, rs->m_UAddr_S1 );
	LoadRsMapValue( chunk_buf, rs->m_UAddr_S2 );
	LoadRsMapValue( chunk_buf, rs->m_UAddr_S3 );
	LoadRsMapValue( chunk_buf, rs->m_VAddr_S0 );
	LoadRsMapValue( chunk_buf, rs->m_VAddr_S1 );
	LoadRsMapValue( chunk_buf, rs->m_VAddr_S2 );
	LoadRsMapValue( chunk_buf, rs->m_VAddr_S3 );
	CSyncSystem::TryToStop();
	LoadRsMapValue( chunk_buf, rs->m_MipMapSample_S0 );
	LoadRsMapValue( chunk_buf, rs->m_MipMapSample_S1 );
	LoadRsMapValue( chunk_buf, rs->m_MipMapSample_S2 );
	LoadRsMapValue( chunk_buf, rs->m_MipMapSample_S3 );
	LoadRsMapValue( chunk_buf, rs->m_MipMapBias_S0 );
	LoadRsMapValue( chunk_buf, rs->m_MipMapBias_S1 );
	LoadRsMapValue( chunk_buf, rs->m_MipMapBias_S2 );
	LoadRsMapValue( chunk_buf, rs->m_MipMapBias_S3 );*/

	return true;
}

