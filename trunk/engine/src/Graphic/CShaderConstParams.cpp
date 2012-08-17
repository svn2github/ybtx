#include "stdafx.h"
#include "CShaderConstParams.h"
#include "TSqrAllocator.inl"

namespace sqr
{

//VS
InitSystemMacro(SYS_VERTEX_TRAN);
InitSystemMacro(SYS_LIGHT_TRAN);
InitSystemMacro(SYS_TRANSFORM_TRAN);
InitSystemMacro(SYS_DIFFUSE_TRAN);
InitSystemMacro(SYS_TEXTURE_TRAN);
InitSystemMacro(SYS_INPUT_VAR);
//PS
InitSystemMacro(SYS_PS_SHADOW_TRAN);

const DWORD ShaderMacroMgr::System_Macro_ID[] =
{
	0,
	SSI_LIGHT_MAP,
	SSI_SHADOW_MAP,
	SSI_INSTANCE,
	SSI_LIGHT_MAP|SSI_INSTANCE,
	SSI_SHADOW_MAP|SSI_INSTANCE,

	SSI_SKELETAL_ANI,
	SSI_LIGHT_MAP|SSI_SKELETAL_ANI,
	SSI_SHADOW_MAP|SSI_SKELETAL_ANI,
	SSI_INSTANCE|SSI_SKELETAL_ANI,
	SSI_LIGHT_MAP|SSI_INSTANCE|SSI_SKELETAL_ANI,
	SSI_SHADOW_MAP|SSI_INSTANCE|SSI_SKELETAL_ANI,

	SSI_VERTEX_ANI,
	SSI_LIGHT_MAP|SSI_VERTEX_ANI,
	SSI_SHADOW_MAP|SSI_VERTEX_ANI,
	SSI_INSTANCE|SSI_VERTEX_ANI,
	SSI_LIGHT_MAP|SSI_INSTANCE|SSI_VERTEX_ANI,
	SSI_SHADOW_MAP|SSI_INSTANCE|SSI_VERTEX_ANI,

	SSI_PRE_RENDER_LIGHT,
	SSI_LIGHT_MAP|SSI_PRE_RENDER_LIGHT,
	SSI_SHADOW_MAP|SSI_PRE_RENDER_LIGHT,
	SSI_INSTANCE|SSI_PRE_RENDER_LIGHT,
	SSI_LIGHT_MAP|SSI_INSTANCE|SSI_PRE_RENDER_LIGHT,
	SSI_SHADOW_MAP|SSI_INSTANCE|SSI_PRE_RENDER_LIGHT,

	SSI_SKELETAL_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_LIGHT_MAP|SSI_SKELETAL_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_SHADOW_MAP|SSI_SKELETAL_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_INSTANCE|SSI_SKELETAL_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_LIGHT_MAP|SSI_INSTANCE|SSI_SKELETAL_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_SHADOW_MAP|SSI_INSTANCE|SSI_SKELETAL_ANI|SSI_PRE_RENDER_LIGHT,

	SSI_VERTEX_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_LIGHT_MAP|SSI_VERTEX_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_SHADOW_MAP|SSI_VERTEX_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_INSTANCE|SSI_VERTEX_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_LIGHT_MAP|SSI_INSTANCE|SSI_VERTEX_ANI|SSI_PRE_RENDER_LIGHT,
	SSI_SHADOW_MAP|SSI_INSTANCE|SSI_VERTEX_ANI|SSI_PRE_RENDER_LIGHT,
};

const uint ShaderMacroMgr::Num_System_Macro = sizeof(ShaderMacroMgr::System_Macro_ID)/sizeof(DWORD);

ShaderMacroMgr::ShaderMacroMgr()
{
	Initialize();
}

ShaderMacroMgr::~ShaderMacroMgr()
{

}

void ShaderMacroMgr::Initialize(void)
{
	InitMacro(MakeSysVS_ID(SSI_BASE_VS_SYSTEM_ID)	,SCT_VERTEX_TRAN);
	InitMacro(MakeSysVS_ID(SSI_SKELETAL_ANI)		,SCT_VERTEX_TRAN);
	InitMacro(MakeSysVS_ID(SSI_VERTEX_ANI)			,SCT_VERTEX_TRAN);

	InitMacro(Input_Macro_ID(SSI_BASE_VS_SYSTEM_ID)	,SCT_INPUT_VAR);
	InitMacro(Input_Macro_ID(SSI_SKELETAL_ANI)		,SCT_INPUT_VAR);
	InitMacro(Input_Macro_ID(SSI_VERTEX_ANI)		,SCT_INPUT_VAR);

	InitMacro(Input_Macro_ID(SSI_PRE_RENDER_LIGHT)	,SCT_LIGHT_TRAN);//默认公式
	InitMacro(MakeSysVS_ID(SSI_PRE_RENDER_LIGHT)	,SCT_LIGHT_TRAN);

	InitMacro(Input_Macro_ID(SSI_INSTANCE)	,SCT_TRANSFORM_TRAN);//默认公式
	InitMacro(MakeSysVS_ID(SSI_INSTANCE)	,SCT_TRANSFORM_TRAN);

	InitMacro(MakeSysVS_ID(SSI_LIGHT_MAP)	,SCT_TEXTURE_TRAN);
	InitMacro(MakeSysVS_ID(SSI_SHADOW_MAP)	,SCT_TEXTURE_TRAN);

	InitMacro(MakeSysPS_ID(SSI_LIGHT_MAP) ,SCT_PS_SHADOW);
	InitMacro(MakeSysPS_ID(SSI_SHADOW_MAP)	,SCT_PS_SHADOW);

	InitMacroMap(SCT_VERTEX_TRAN,SYS_VERTEX_TRAN);
	InitMacroMap(SCT_TRANSFORM_TRAN,SYS_TRANSFORM_TRAN);
	InitMacroMap(SCT_LIGHT_TRAN,SYS_LIGHT_TRAN);
	InitMacroMap(SCT_DIFFUSE_TRAN,SYS_DIFFUSE_TRAN);
	InitMacroMap(SCT_TEXTURE_TRAN,SYS_TEXTURE_TRAN);
	InitMacroMap(SCT_INPUT_VAR,SYS_INPUT_VAR);
	InitMacroMap(SCT_PS_SHADOW,SYS_PS_SHADOW_TRAN);
}

void ShaderMacroMgr::LoadMacro(DWORD id , char* str)
{
	MacroMap::iterator it =  m_mapMacro.find(id);
	if (it!=m_mapMacro.end())
		it->second.Definition = str;
}

void ShaderMacroMgr::DefineMacro( const char* mac, const char* val )
{
	m_macDefMap[mac] = val;
}

void ShaderInclude::InsertInclude(const string& FileName, const string& Code )
{
	m_incFile.insert(IncludeMap::value_type(FileName.c_str(),Code.c_str()));
}

void ShaderInclude::Clear()
{
	m_incFile.clear();
}

//---------------------------------------------------------
const uint64 SHADER_HANDLE::L_NUM = 0x00000000ffffffff;
const uint64 SHADER_HANDLE::H_NUM = 0xffffffff00000000;
const uint32 SHADER_HANDLE::bit_num = 32;
}