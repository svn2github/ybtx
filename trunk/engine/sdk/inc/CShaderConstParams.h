#pragma once
#include "TSingleton.h"
#include "CGraphicMallocObject.h"
namespace sqr
{
enum SystemShaderID
{
	SSI_BASE_VS_SYSTEM_ID	= 0x40000000,
	SSI_BASE_PS_SYSTEM_ID	= 0x80000000,
	SSI_BASE_FX_SYSTEM_ID	= 0xC0000000,

	SSI_LIGHT_MAP			= 0x000100,
	SSI_SHADOW_MAP			= 0x000200,
	SSI_SKELETAL_ANI		= 0x000400,
	SSI_VERTEX_ANI			= 0x000800,
	SSI_INSTANCE			= 0x001000,
	SSI_PRE_RENDER_LIGHT	= 0x002000,
	SSI_SPECULAR			= 0x004000,

	SSI_END_SYSTEM_ID		= 0xFFFFFF00,
	SSI_FORCE_DWORD			= 0xFFFFFFFF
};

inline DWORD MakeSysVS_ID(DWORD ID)
{
	return (SSI_BASE_VS_SYSTEM_ID|ID);
}

inline DWORD MakeSysPS_ID(DWORD ID)
{
	return SSI_BASE_PS_SYSTEM_ID|ID;
}

inline DWORD MakeGroupID(DWORD ID)
{
	return (ID&0x3F)<<24;
}

inline DWORD Input_Macro_ID(DWORD ID)
{
	return ID&0xFFFF00;
}

inline bool ID_Test(DWORD ID,SystemShaderID sID)
{
	return (ID&sID) == sID;
}

#define MAX_LIGHT_NUM	6

enum AutoConstantType
{
	ACT_PROJECTION_MATRIX		= 0,	//投影矩阵
	ACT_VIEW_MATRIX				= 4,	//视矩阵
	ACT_WORLD_MATRIX			= 8,	//世界矩阵
	ACT_VIEWPROJ_MATRIX			= 12,	//VP矩阵, 13, 14, 15

	ACT_INVERSE_VIEW_SIZE		= 16,
	ACT_CAMERA_POSITION			= 17,	//摄像机位置

	ACT_FOG_ENABLE				= 18,	//雾效开关
	ACT_TEXTURE_FACTOR			= 19,	// PS？
	
	// 材质
	ACT_MAT_DIFFUSE_COLOR		= 20,	//散射材质
	ACT_MAT_AMBIENT_COLOR		= 21,   //环境材质
	ACT_MAT_SPECULAR_COLOR		= 22,	//镜面材质float4(rgb, specularPower)
	ACT_MAT_EMISSIVE_COLOR		= 23,	//自发光材质

	ACT_REFRACT_SCALAR			= 24,
	ACT_UV_INDEX				= 25,	//UV数据源

	//太阳光参数

#ifndef AMBIENT_SHADOW_COLOR
	ACT_GLOBAL_SHADOW_COLOR		= 30,	//阴影颜色
	ACT_GLOBAL_AMBIENT			= 35,	//环境光
#else
	ACT_GLOBAL_AMBIENT			= 30,	//环境光
#endif

	ACT_GLOBAL_SHADOW_MATRIX	= 31,	//FOR_SHADOW, 32, 33, 34
	ACT_GLOBAL_LIGHT_COLOR		= 36,	//灯光散射光
	ACT_GLOBAL_LIGHT_DIRECTION	= 37,	//灯光方向

	//灯光参数
	ACT_LIGHT_COUNT				= 40,	//灯光数目
	ACT_LIGHT_COLOR_ARRAY		= 41,	//灯光散射光
	ACT_LIGHT_POSITION_ARRAY = ACT_LIGHT_COLOR_ARRAY + MAX_LIGHT_NUM,		//灯光位置
	ACT_LIGHT_DIRECTION_ARRAY = ACT_LIGHT_POSITION_ARRAY + MAX_LIGHT_NUM,		//灯光方向
	ACT_LIGHT_RANGE_INFO_ARRAY = ACT_LIGHT_DIRECTION_ARRAY + MAX_LIGHT_NUM,		//Range,Attenuation1,Attenuation1,Attenuation3
	ACT_LIGHT_SPOT_INFO_ARRAY = ACT_LIGHT_RANGE_INFO_ARRAY + MAX_LIGHT_NUM,		//isSpot,Falloff,Theta,Phi

	ACT_CUSTOM_PARAMS_LIST		= 100,//自定义参数开始
};

enum AutoConstantSize
{
	ACS_VECTOR		=	1,
	ACS_VECTOR2		=	2,
	ACS_VECTOR3		=	3,
	ACS_MATRIX		=	4,
	ACS_MATRIX2		=	8,
	ACS_MATRIX3		=	12,
	ACS_MATRIX4		=	16,
};

class ShaderMacroMgr 
	: public Singleton<ShaderMacroMgr>
	, public CGraphicMallocObject
{
public:
	static const DWORD		System_Macro_ID[];
	static const uint		Num_System_Macro;
public:
	enum SystemCtrlTrans
	{
		SCT_INPUT_VAR		= 0,
		SCT_VERTEX_TRAN		= 1,
		SCT_TRANSFORM_TRAN	= 2,
		SCT_LIGHT_TRAN		= 3,
		SCT_DIFFUSE_TRAN	= 4,
		SCT_TEXTURE_TRAN	= 5,
		SCT_PS_SHADOW		= 6,
		SCT_COUNT,
	};
public:
#define InitMacro(id,name) m_mapMacro.insert(MacroMap::value_type(id,ShaderMacro(name)))
#define InitMacroMap(id,name) SYS_MACRO_MAP[id] = &name;
#define DefineSystemMacro(name) const static GString name
#define InitSystemMacro(name) const GString ShaderMacroMgr::name = #name;

	//VS
	DefineSystemMacro(SYS_VERTEX_TRAN);
	DefineSystemMacro(SYS_TRANSFORM_TRAN);
	DefineSystemMacro(SYS_LIGHT_TRAN);
	DefineSystemMacro(SYS_DIFFUSE_TRAN);
	DefineSystemMacro(SYS_TEXTURE_TRAN);
	DefineSystemMacro(SYS_INPUT_VAR);

	//PS
	DefineSystemMacro(SYS_PS_SHADOW_TRAN);

	struct ShaderMacro
	{
		ShaderMacro(SystemCtrlTrans flag):MacroFlag(flag) {};
		ShaderMacro(const ShaderMacro& mac)
				:MacroFlag(mac.MacroFlag),Definition(mac.Definition) {};
		SystemCtrlTrans MacroFlag;
		GString Definition;
	};

public:
	typedef GMap<DWORD , ShaderMacro> MacroMap;
	
	void	Initialize(void);
	void	LoadMacro(DWORD id , char* str);
	void	DefineMacro(const char* mac, const char* val);

public:
	ShaderMacroMgr();
	~ShaderMacroMgr();

protected:
	typedef GMap<GString, GString> MacroDefineMap;

	MacroMap m_mapMacro;
	const GString* SYS_MACRO_MAP[SCT_COUNT];
	MacroDefineMap	m_macDefMap;
};


class ShaderInclude 
	: public Singleton<ShaderInclude>
	, public CGraphicMallocObject
{
public:
	void InsertInclude(const string& FileName, const string& Code );
	void Clear();
public:
	typedef GMap< GString, GString > IncludeMap;
	IncludeMap	m_incFile;
};

#pragma region SHADER_HANDLE
class SHADER_HANDLE : public CGraphicMallocObject
{
public:
	SHADER_HANDLE():handle(0) {};
public:
	static const uint64 L_NUM;
	static const uint64 H_NUM;
	static const uint32	bit_num;
public:
	uint64 handle;

public:
	inline const DWORD GetPSShaderID() const
	{
		return static_cast<DWORD>(handle&L_NUM);
	};
	inline const DWORD GetVSShaderID() const
	{
		return static_cast<DWORD>((handle&H_NUM)>>bit_num);
	};

	inline SHADER_HANDLE& operator = (DWORD sid);
	inline SHADER_HANDLE& operator = (uint64 fhandle);
	inline SHADER_HANDLE& operator |= (DWORD sid);
	
	friend bool	operator < (const SHADER_HANDLE& lhs, const SHADER_HANDLE& rhs)
	{
		return lhs.handle < rhs.handle;
	}

	inline bool	operator != (const SHADER_HANDLE& other);

	inline void ClearPSShaderID()
	{
		handle&=H_NUM;
	}
	inline void ClearVSShaderID()
	{
		handle&=L_NUM;
	}
	inline void Clear()
	{
		handle = 0;
	}
	inline bool IsNull() const
	{
		return handle==0;
	}

	inline bool IsValid() const
	{
		return handle!=0;
	}
};

SHADER_HANDLE& SHADER_HANDLE::operator = (DWORD sid)
{
	switch (sid&SSI_BASE_FX_SYSTEM_ID)
	{
		case SSI_BASE_VS_SYSTEM_ID:
			handle = ((static_cast<uint64>(sid))<<bit_num);
			break;
		case SSI_BASE_PS_SYSTEM_ID:
			handle = static_cast<uint64>(sid);
			break;
		case SSI_BASE_FX_SYSTEM_ID:
			handle = 0xffffffff00000000;
			handle |=  static_cast<uint64>(sid);
			break;
	}
	return *this;
}

SHADER_HANDLE& SHADER_HANDLE::operator = (uint64 fhandle)
{
	handle = fhandle;
	return *this;
}

SHADER_HANDLE& SHADER_HANDLE::operator |= (DWORD sid)
{
	switch (sid&SSI_BASE_FX_SYSTEM_ID)
	{
		case SSI_BASE_VS_SYSTEM_ID:
			handle |= ((static_cast<uint64>(sid))<<bit_num);
			break;
		case SSI_BASE_PS_SYSTEM_ID:
			handle |= static_cast<uint64>(sid);
			break;
		case SSI_BASE_FX_SYSTEM_ID:
			handle |= 0xffffffff00000000;
			handle |=  static_cast<uint64>(sid);
			break;
	}
	return *this;
}

bool SHADER_HANDLE::operator != (const SHADER_HANDLE& other)
{
	return handle != other.handle;
}

#pragma endregion 
}
