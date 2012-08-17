#pragma once

#include "CVector2.h"
#include "CVector3.h"
#include "CQuaternion.h"
#include "CMatrix.h"
#include "CColor4.h"
#include "CColor.h"
#include "BaseDefs.h"
#include "IDName.h"
#include "CPlane.h"
#include "TSingleton.h"
#include "GraphicClassDefine.h"
#include "CShaderConstParams.h"
#include "GraphicRenderStyle.h"
#include "CRefObject.h"
#include "CRectangle.h"
#include "CAxisAlignedBox.h"
#include "CSphere.h"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"

namespace sqr
{

#ifdef  GERROR
#undef  GERROR
#endif
#define GERROR				-1

#ifdef  GRESET
#undef  GRESET
#endif
#define GRESET				-2

#define MAX_DEPTH			0.99999994f
#define MAX_TRI_INDEXNUM	1024	

extern const wstring PATH_ALIAS_RES;
extern const wstring PATH_ALIAS_LAN;
extern const wstring PATH_ALIAS_SHD;
extern const wstring PATH_ALIAS_EMT;
extern const wstring PATH_ALIAS_GUITEX;

enum IntersectFlag
{
	IF_IMPOSSIBLE	= 0,
	IF_POSSIBLE		= 1,
	IF_EXCELLENT	= 2
};


//描述顶点语义
enum CVertexElementSemantic
{
	/// 坐标,由3个实数组成
	VES_POSITION = 1,
	/// XYZRHW,多用于2D渲染
	VES_POSITIONT = 2,
	/// 混合权重
	VES_BLEND_WEIGHTS = 3,
	/// 混合索引
	VES_BLEND_INDICES = 4,
	/// 法向量,有三个实数组成
	VES_NORMAL = 5,
	/// 散射光
	VES_DIFFUSE = 6,
	/// 镜面反射光
	VES_SPECULAR = 7,
	/// 纹理坐标
	VES_TEXTURE_COORDINATES = 8,
	/// 次法线 (Y axis if normal is Z)
	VES_BINORMAL = 9,
	/// 切线  (X axis if normal is Z)
	VES_TANGENT = 10,
	/// 点的尺寸
	VES_PSIZE  = 11,
	/// 顶点颜色(固定管线只能使用0,1号COLOR)
	VES_COLOR  = 12,
};

enum CVertexElementType
{
	VET_FLOAT1 = 0,
	VET_FLOAT2 = 1,
	VET_FLOAT3 = 2,
	VET_FLOAT4 = 3,
	///颜色格式 RGBA，参照DWORD一个，这下爽了吧
	VET_COLOUR = 4,
	VET_SHORT1 = 5, // D3D不可用
	VET_SHORT2 = 6,
	VET_SHORT3 = 7, // D3D不可用
	VET_SHORT4 = 8,
	VET_UBYTE4 = 9,
};

enum ClearModeType
{
	CMT_CLEAR_NONE			=  0x00000000,
	CMT_CLEAR_TARGET		=  0x00000001,
	CMT_CLEAR_ZBUFFER		=  0x00000002,
	CMT_CLEAR_TARGE_ZBUFFER	=  0x00000003,
	CMT_CLEAR_STENCI		=  0x00000004,
	CMT_CLEAR_TARGET_STENCI	=  0x00000005,
	CMT_CLEAR_ZBUFFER_STENCI=  0x00000006,
	CMT_CLEAR_ALL			=  0x00000007,
	CMT_FORCE_DWORD			=  0xFFFFFFFF
};

enum FogModeType
{
	FMT_NONE	= 0,
	FMT_EXP		= 1,
	FMT_EXP2	= 2,
	FMT_LINEAR	= 3,
	FMT_FORCE_DWORD = 0x7fffffff,
};

enum ModifyOperator
{
	MO_ADD		= 0,
	MO_DELETE	= 1,
	MO_EDIT		= 2,
};

//enum ParamType
//{
//	NONE_PARAM		= 0,
//	BONE_LIST		= 1,
//	TEXTURE_MATRIX	= 2,
//	VERTEX_ANI		= 3,
//};

enum EShadowType
{
	ST_NONE						= 0,
	//ST_PLANE					,
	ST_SHADOWMAP				,
	ST_SHADOW_EX				,	//强行全物体接受阴影
	ST_TOOLEX_SHADOW			,	///< 工具用
	ST_COUNT					,
	ST_ERROR					= 0xffffffff
};

enum EWaterType
{
	WT_Low,
	WT_Default,
	WT_High,

	WT_COUNT
};

enum ERenderObjType
{
	ROT_MODEL					= 0,
	ROT_EFFECT					= 1,
	ROT_SOFT					= 2,
	ROT_GEO_INST				= 3,
	ROT_LIGHT					= 4,
	ROT_AUDIO					= 5,
	ROT_UNKNOW					= 0xffffffff
};

enum eLinkType
{
	LT_CENTER					= 0,
	LT_SOCKET					= 1,
	LT_SKELETAL					= 2,
	LT_SKELETAL_OFFSET			= 3,
	LT_UNKNOW					= 4,
};

enum ERenderStyle
{
	RS_DEFAULT					= 0,
	RS_DEFAULT1					,
	RS_PONITSAMPLE				,
	RS_FONT						,
	RS_GUI						,
	RS_GUI_COLORADD				,
	RS_MAP1L					,
	RS_MAP2L					,
	RS_MAP3L					,
	RS_MAP4L					,
	RS_SHADOW					,
	RS_ALPHA2GRAY				,
	RS_NONETEXTALPHA			,
	RS_NONETEXTALPHALINER		,
	RS_GUI_MIRROR               ,
	RS_GUI_SHADOW				,
	RS_GUI_ALPHA				,
	RS_GUI_ALPHABLEND			,
	RS_GUI_BIGMAP				,
	RS_EMT						,
	RS_LINE						,
	RS_COUNT					,
	RS_UNKNOWN					= 0xFFFFFFFF
};


enum ESqrStencilOperation
{
	STENCILOP_KEEP           = 1,
	STENCILOP_ZERO           = 2,
	STENCILOP_REPLACE        = 3,
	STENCILOP_INCRSAT        = 4,
	STENCILOP_DECRSAT        = 5,
	STENCILOP_INVERT         = 6,
	STENCILOP_INCR           = 7,
	STENCILOP_DECR           = 8,
	STENCILOP_FORCE_DWORD    = 0x7fffffff, /* force 32-bit size enum */
};

struct StencilParam
{
	bool				m_bEnable;
	int					m_nRefrence;
	DWORD				m_nCompFun;
	DWORD				m_nFailedFlag;
	DWORD				m_nZFailedFlag;
	DWORD				m_nPassFlag;
	DWORD				m_nReadMask;
	DWORD				m_nWriteMask;

	///默认构造函数.
	StencilParam()
			: m_bEnable(true)
			, m_nRefrence(0)
			, m_nCompFun(CMP_ALWAYS)
			, m_nFailedFlag(STENCILOP_KEEP)
			, m_nZFailedFlag(STENCILOP_KEEP)
			, m_nPassFlag(STENCILOP_KEEP)
			, m_nReadMask(0xFFFFFFFF)
			, m_nWriteMask(0xFFFFFFFF) {}
};

struct FramePair
{
	FramePair():nStart(0),nEnd(-1) {};
	FramePair( int16 nS, int16 nE ):nStart(nS),nEnd(nE) {}

	int16	nStart;
	int16	nEnd;

	int16 Space()
	{
		return nEnd - nStart;
	}
};

//模型渲染返回结构
struct AniResult : public CGraphicMallocObject
{
	AniResult() : m_FrameString(NULL), m_SepFrameString(NULL), m_CurFrame(0.0f), m_IsAniEnd(false), m_InAniBlend(false){}
	AniResult( const IDNAME* pFrameString,  bool bIsAniEnd, bool bInAniBlend , const IDNAME& pAName = NULL)
			: m_FrameString(pFrameString), m_IsAniEnd(bIsAniEnd), m_InAniBlend(bInAniBlend)
			, m_PreAnimationName(pAName),m_IsReset(false)
			/*, m_AniframeIndex(999999), m_SepAniframeIndex(999999)*/{}
	IDNAME			m_PreAnimationName;
	//GList<IDNAME>	m_PreSepAnimationName;
	const IDNAME*	m_FrameString;		//帧特征字符串
	const IDNAME*	m_SepFrameString;	//帧特征字符串
	/*int32			m_AniframeIndex;
	int32			m_SepAniframeIndex;*/
	float			m_CurFrame;		//渲染的当前帧
	bool			m_IsAniEnd;		//动画播放完毕
	bool			m_IsReset;		//动画重新播放
	bool			m_InAniBlend;   //动画单元融合中
};

//
#define FVF_RESERVED0        0x001
#define FVF_POSITION_MASK    0x400E
#define FVF_XYZ              0x002
#define FVF_XYZRHW           0x004
#define FVF_XYZB1            0x006
#define FVF_XYZB2            0x008
#define FVF_XYZB3            0x00a
#define FVF_XYZB4            0x00c
#define FVF_XYZB5            0x00e
#define FVF_XYZW             0x4002

#define FVF_NORMAL           0x010
#define FVF_PSIZE            0x020
#define FVF_DIFFUSE          0x040
#define FVF_SPECULAR         0x080

#define FVF_TEXCOUNT_MASK    0xf00
#define FVF_TEXCOUNT_SHIFT   8
#define FVF_TEX0             0x000
#define FVF_TEX1             0x100
#define FVF_TEX2             0x200
#define FVF_TEX3             0x300
#define FVF_TEX4             0x400
#define FVF_TEX5             0x500
#define FVF_TEX6             0x600
#define FVF_TEX7             0x700
#define FVF_TEX8             0x800

#define FVF_LASTBETA_UBYTE4   0x1000
#define FVF_LASTBETASQRD3DCOLOR 0x8000

#define FVF_RESERVED2         0x6000  // 2 reserved bits
#define FVF_MULTI_STREAM	  0xf0000000
#define SOURCE_MULTI_STREAM(s) (s<<28) //最大16
//////////////////////////////////////////////////////////////////////////
//定义模型渲染类型
enum ERenderObjStyle
{
	ROS_LIGHTMAT1							=0x001,///受场景光源物体
	ROS_LIGHTMAT2							=0x002,///受人物光源

	ROS_ALPHA								=0x004,///带alpha混合
	ROS_ALPHA_LIGHT1						=0x005,///带alpha混合 | 受场景光源物体
	ROS_ALPHA_LIGHT2						=0x006,///带alpha混合 | 受人物光源

	///产生阴影
	ROS_SHADOW								=0x008,///产生阴影物体
	ROS_SHADOW_LIGHT1						=0x009,///产生阴影物体 | 受场景光源物体
	ROS_SHADOW_LIGHT2						=0x00A,///产生阴影物体 | 受人物光源物体
	ROS_SHADOW_ALPHALIGHT1					=0x00D,///产生阴影物体 | 带alpha混合 | 受场景光源物体
	ROS_SHADOW_ALPHALIGHT2					=0x00E,///产生阴影物体 | 带alpha混合 | 受人物光源

	///折射
	ROS_REFRACT								=0x010,///带折射的物体

	///消隐
	ROS_TRANS								=0x024,///消隐物体
	ROS_TRANS_LIGHTMAT1						=0x025,///消隐 | 受场景光源物体
	ROS_TRANS_LIGHTMAT2						=0x026,///消隐 | 受人物光源

	///接收阴影
	ROS_SHADOWRECEIVE						=0x040,///接收阴影
	ROS_SHADOWRECEIVE_LIGHT1				=0x041,///接收阴影 | 受场景光源物体
	ROS_SHADOWRECEIVE_LIGHT2				=0x042,///接收阴影 | 受人物光源
	ROS_SHADOWRECEIVE_ALPHALIGHT1			=0x045,///接收阴影 | 带alpha混合 | 受场景光源物体
	ROS_SHADOWRECEIVE_ALPHALIGHT2			=0x046,///接收阴影 | 带alpha混合 | 受人物光源

	///产生阴影 | 接收阴影
	ROS_SHADOWRECEIVE_LIGHT1_SHADOW			=0x04A,///接收阴影 | 受场景光源物体 | 产生阴影
	ROS_SHADOWRECEIVE_LIGHT2_SHADOW			=0x04B,///接收阴影 | 受人物光源 | 产生阴影
	ROS_SHADOWRECEIVE_ALPHALIGHT1_SHADOW    =0x052,///接收阴影 | 带alpha混合 | 受场景光源物体 | 产生阴影
	ROS_SHADOWRECEIVE_ALPHALIGHT2_SHADOW	=0x054,///接收阴影 | 带alpha混合 | 受人物光源 | 产生阴影

	ROS_ZFAILD								=0x080,
	ROS_ZFAILD_LIGHT1						=0x081,
	ROS_ZFAILD_LIGHT2						=0x082,
	ROS_ZFAILD_ALPHAT_LIGHT1				=0x085,
	ROS_ZFAILD_ALPHAT_LIGHT2				=0x086,
	ROS_ZFAILD_SHADOW						=0x088,///产生阴影物体
	ROS_ZFAILD_SHADOW_LIGHT1				=0x089,///产生阴影物体 | 受场景光源物体
	ROS_ZFAILD_SHADOW_LIGHT2				=0x08A,///产生阴影物体 | 受人物光源物体
	ROS_ZFAILD_SHADOW_ALPHALIGHT1			=0x08D,///产生阴影物体 | 带alpha混合 | 受场景光源物体
	ROS_ZFAILD_SHADOW_ALPHALIGHT2			=0x08E,///产生阴影物体 | 带alpha混合 | 受人物光源
	ROS_ZFAILD_SHADOWRECEIVE				=0x0C0,///接收阴影
	ROS_ZFAILD_SHADOWRECEIVE_LIGHT1			=0x0C1,///接收阴影 | 受场景光源物体
	ROS_ZFAILD_SHADOWRECEIVE_LIGHT2			=0x0C2,///接收阴影 | 受人物光源
	ROS_ZFAILD_SHADOWRECEIVE_ALPHALIGHT1	=0x0C5,///接收阴影 | 带alpha混合 | 受场景光源物体
	ROS_ZFAILDS_SHADOWRECEIVE_ALPHALIGHT2	=0x0C6,///接收阴影 | 带alpha混合 | 受人物光源
	ROS_ZFAILDS_SHADOWRECEIVE_LIGHT1_SHADOW	=0x0CA,///接收阴影 | 受场景光源物体 | 产生阴影
	ROS_ZFAILDS_SHADOWRECEIVE_LIGHT2_SHADOW	=0x0CB,///接收阴影 | 受人物光源 | 产生阴影
	ROS_UNKNOWN						= 0xffffffff
};

struct RenderObjStyle
{
	ERenderObjStyle m_eType;
	bool	 m_bRenderLink;
	bool     m_bRenderMesh;
	DWORD	 m_nParam1;
	DWORD	 m_nParam2;
};

//////////////////////////////////////////////////////////////////////////
//define for ROS_SHADOW
#define	DELTASH_ZTEST			0x00000001
#define	DELTASH_ZWRITE			0x00000002

struct DELTASH
{
	DWORD	dwColor;
	CMatrix matShadow;
};

//////////////////////////////////////////////////////////////////////////
//define for ROS_LIGHTMAT
#define	DELTARS_ALPHADIF		0x00000001
#define	DELTARS_DIFFUSE			0x00000002
#define	DELTARS_AMBIENT			0x00000004
#define	DELTARS_SPECULAR		0x00000008
#define	DELTARS_SPECPOWER		0x00000010
#define	DELTARS_EMISSIVE		0x00000020

//////////////////////////////////////////////////////////////////////////
enum
{
	PIECERS_DIFFUSE		= 1<<0,
	PIECERS_AMBIENT		= 1<<1,
	PIECERS_SPECULAR	= 1<<2,
	PIECERS_SECONDTEX	= 1<<3,
	PIECERS_CLIPPLANE	= 1<<4,
	PIECERS_REFRACT		= 1<<5,
};

enum FADE_MODE
{
	FADE_IN          =-1,
	FADE_NULL        = 0,
	FADE_OUT         = 1,
	FADE_OUTEX       = 2,
	FADE_INEX        = 3
};


#define MATAMBIENT              0.5f
#define MODULATETYPE            TOP_MODULATE2X

class ITexture;
struct CPieceRS
{
	uint32				m_nMask;
	CColor				m_Diffuse;
	CColor				m_Ambient;
	CColor				m_Specular;
	float				m_fPower;
	uint32				m_BlendAlpha;
	CMatrix				m_MatrixText;
	ITexture*			m_pTexture;
	CPlane				m_ClipPlane;
	float				m_fRefractIndex;
};

//////////////////////////////////////////////////////////////////////////

enum EDisplayFormat
{
	DF_X8R8G8B8             = 0,
	DF_A8R8G8B8				= 1,
	DF_X1R5G5B5             = 2,
	DF_R5G6B5               = 3,
	DF_UNKNOW				= 4,
	DF_COUNT
};

// Primitives supported by draw-primitive API
enum EPrimitiveType
{
	PT_POINTLIST             = 1,
	PT_LINELIST              = 2,
	PT_LINESTRIP             = 3,
	PT_TRIANGLELIST          = 4,
	PT_TRIANGLESTRIP         = 5,
	PT_TRIANGLEFAN           = 6,
	PT_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
};

/// 反锯齿质量.
enum EAntiAliasQuality
{
	AA_None	= 0,	///< 关闭
	AA_Low	= 2,	///< 除了intel那种太监卡, 一般都支持
	AA_High	= 4, 	///< 一般low支持high就支持(查Card caps. v4.2 Sep 2007表得)
	AA_FORCE_DWORD           = 0x7fffffff, /* force 32-bit size enum */
	//再高闲得蛋疼的人才用
};

enum GpuProgramType
{
	GPT_VERTEX_PROGRAM		= 1,
	GPT_FRAGMENT_PROGRAM	= 2,
	GPT_EFFECTS_PROGRAM		= 3,
};

enum ShaderVer
{
	Shader_V_0 = 0, //固定管线
	Shader_V_1 = 1,
	Shader_V_2 = 2,
	Shader_V_3 = 3,
	Shader_V_4 = 4,
	ShaderVerCount,
};

/// 顶点(坐标)
struct VerPos
{
	VerPos() {};
	VerPos(float sx,float sy,float sz)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
	};

	enum { Format = FVF_XYZ };

	CVector3f p;
};

/// 顶点(坐标, 颜色)
struct VerColor3D
{
	VerColor3D() {};
	VerColor3D(float sx,float sy,float sz,DWORD dif)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		diffuse=dif;
	};

	enum { Format=FVF_XYZ|FVF_DIFFUSE };

	CVector3f p;
	DWORD diffuse;
};

/// 顶点(坐标，颜色，大小)
struct VerColorSize
{
	VerColorSize() {};
	VerColorSize(float sx,float sy,float sz,DWORD dif,float s)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		diffuse=dif;
		size = s;
	};

	enum { Format=FVF_XYZ|FVF_DIFFUSE|FVF_PSIZE };

	CVector3f p;
	DWORD diffuse;
	float size;
};

/// 顶点(坐标, 法线, 纹理坐标)
struct VerNorDiffuseTex
{
	VerNorDiffuseTex()
	{
		p.x=0;
		p.y=0;
		p.z=0;
		np.x=0;
		np.y=0;
		np.z=0;
		tu=0;
		tv=0;
	};
	VerNorDiffuseTex(float x,float y,float z,float nx,float ny,float nz,
	                 float stu,float stv)
	{
		p.x=x;
		p.y=y;
		p.z=z;
		np.x=nx;
		np.y=ny;
		np.z=nz;
		tu=stu;
		tv=stv;
	};

	enum { Format=FVF_XYZ | FVF_NORMAL | FVF_DIFFUSE | FVF_TEX1 };

	CVector3f p;
	CVector3f np;
	DWORD diffuse;
	float tu,tv; // The vertex colour.
	BOOL operator==(VerNorDiffuseTex& ver)
	{
		return (p==ver.p&&np==ver.np&&tu==ver.tu&&tv==ver.tv==diffuse==ver.diffuse);
	}
};

/// 顶点(坐标, 法线, 纹理坐标)
struct VerNorTex
{
	VerNorTex()
	{
		p.x=0;
		p.y=0;
		p.z=0;
		np.x=0;
		np.y=0;
		np.z=0;
		tu=0;
		tv=0;
	};
	VerNorTex(float x,float y,float z,float nx,float ny,float nz,
	          float stu,float stv)
	{
		p.x=x;
		p.y=y;
		p.z=z;
		np.x=nx;
		np.y=ny;
		np.z=nz;
		tu=stu;
		tv=stv;
	};

	enum { Format=FVF_XYZ | FVF_NORMAL | FVF_TEX1 };

	CVector3f p;
	CVector3f np;
	float tu,tv; // The vertex colour.
	BOOL operator==(VerNorTex& ver)
	{
		return (p==ver.p&&np==ver.np&&tu==ver.tu&&tv==ver.tv);
	}
};
/// 顶点(坐标, 法线, 纹理坐标)
struct VerNorTex1
{
	VerNorTex1()
	{
		p.x=0;
		p.y=0;
		p.z=0;
		np.x=0;
		np.y=0;
		np.z=0;
		tu=0;
		tv=0;
		tu1 = 0;
		tv1=0 ;
	};
	VerNorTex1(float x,float y,float z,float nx,float ny,float nz,
	           float stu,float stv,float stu1,float stv1)
	{
		p.x=x;
		p.y=y;
		p.z=z;
		np.x=nx;
		np.y=ny;
		np.z=nz;
		tu=stu;
		tv=stv;
		tu1=stu1;
		tv1=stv1;
	};

	enum { Format=FVF_XYZ|FVF_NORMAL|FVF_TEX2 };

	CVector3f p;
	CVector3f np;
	float tu,tv; // The vertex colour.
	float tu1,tv1;

	BOOL operator==(VerNorTex1& ver)
	{
		return (p==ver.p&&np==ver.np&&tu==ver.tu&&tv==ver.tv&&tu1==ver.tu1&&tv1==ver.tv1);
	}
};

/// 顶点(坐标, 法线, 纹理坐标)
struct VerNorTex2
{
	VerNorTex2()
	{
		p.x=0;
		p.y=0;
		p.z=0;
		np.x=0;
		np.y=0;
		np.z=0;
		tu=0;
		tv=0;
		tv=0;
		tu1 = 0;
		tv1=0 ;
		tu2=0;
		tv2=0;
	};
	VerNorTex2(float x,float y,float z,float nx,float ny,float nz,
	           float stu,float stv,float stu1,float stv1,float stu2,float stv2)
	{
		p.x=x;
		p.y=y;
		p.z=z;
		np.x=nx;
		np.y=ny;
		np.z=nz;
		tu=stu;
		tv=stv;
		tu1=stu1;
		tv1=stv1;
		tu2=stu2;
		tv2=stv2;
	};

	enum { Format=FVF_XYZ|FVF_NORMAL | FVF_TEX3 };
	CVector3f p;
	CVector3f np;
	float tu,tv; // The vertex colour.
	float tu1,tv1;
	float tu2,tv2;
	
	BOOL operator==(VerNorTex2& ver)
	{
		return (p==ver.p&&np==ver.np&&tu==ver.tu&&tv==ver.tv&&tu1==ver.tu1&&tv1==ver.tv1&&tu2==ver.tu2&&tv2==ver.tv2);
	}
};
/// 顶点(坐标, 法线)
struct VerNor
{
	VerNor()
	{
		p.x=0;
		p.y=0;
		p.z=0;
		np.x=0;
		np.y=0;
		np.z=0;
	};
	VerNor(float sx,float sy,float sz,float snx,float sny,float snz)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
	};

	enum { Format=FVF_XYZ|FVF_NORMAL };

	CVector3f p;
	CVector3f np;// The transformed position for the vertex.
};
/// 顶点(坐标, 法线, 颜色)
struct VerNorDif
{
	VerNorDif()
	{
		p.x=0;
		p.y=0;
		p.z=0;
		np.x=0;
		np.y=0;
		np.z=0;
		diffuse=0xffffffff;
	};
	VerNorDif(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
		diffuse=dif;
	};

	enum { Format=FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE };

	CVector3f p;
	CVector3f np;// The transformed position for the vertex.
	DWORD diffuse;
};
/// 2D顶点(屏幕坐标, 纹理坐标, 颜色)
struct Vertex2D
{
	Vertex2D()
	{
		x=0;
		y=0;
		z=0;
		w=1.0f;
		diffuse=0;
		tu=0;
		tv=0;
	}
	Vertex2D(float sx,float sy,float sz,DWORD dif,float stu,float stv)
	{
		x=sx;
		y=sy;
		z=sz;
		w=1.0f;
		diffuse=dif;
		tu=stu;
		tv=stv;
	}

	void Set(float sx,float sy,float sz,DWORD dif,float stu,float stv)
	{
		x=sx;
		y=sy;
		z=sz;
		w=1.0f;
		diffuse=dif;
		tu=stu;
		tv=stv;
	}

	enum { Format=FVF_XYZRHW|FVF_TEX1|FVF_DIFFUSE };

	float x,y,z,w;
	DWORD diffuse;
	float tu,tv;
};

/// 2D顶点(屏幕坐标, 纹理坐标, 颜色)
struct Vertex2DAlpha
{
	Vertex2DAlpha()
	{
		x=0;
		y=0;
		z=0;
		w=1.0f;
		diffuse=0;
		tu0=0;
		tv0=0;
		tu1=0;
		tv1=0;
	}
	Vertex2DAlpha(float sx,float sy,float sz,DWORD dif,float stu0,float stv0,float stu1,float stv1)
	{
		x=sx;
		y=sy;
		z=sz;
		w=1.0f;
		diffuse=dif;
		tu0=stu0;
		tv0=stv0;
		tu1=stu1;
		tv1=stv1;
	}

	void Set(float sx,float sy,float sz,DWORD dif,float stu0,float stv0,float stu1,float stv1)
	{
		x=sx;
		y=sy;
		z=sz;
		w=1.0f;
		diffuse=dif;
		tu0=stu0;
		tv0=stv0;
		tu1=stu1;
		tv1=stv1;
	}

	enum { Format=FVF_XYZRHW|FVF_TEX2|FVF_DIFFUSE };

	float x,y,z,w;
	DWORD diffuse;
	float tu0,tv0;
	float tu1,tv1;
};
/// 顶点(坐标, 纹理坐标, 颜色)
struct Vertex3D
{
	Vertex3D()
	{
		x=0;
		y=0;
		z=0;
		diffuse=0;
		tu=0;
		tv=0;
	}
	Vertex3D(float sx,float sy,float sz,DWORD dif,float stu,float stv)
	{
		x=sx;
		y=sy;
		z=sz;
		diffuse=dif;
		tu=stu;
		tv=stv;
	}

	void Set(float sx,float sy,float sz,DWORD dif,float stu,float stv)
	{
		x=sx;
		y=sy;
		z=sz;
		diffuse=dif;
		tu=stu;
		tv=stv;
	}

	enum { Format=FVF_XYZ|FVF_TEX1|FVF_DIFFUSE };

	float x,y,z;
	DWORD diffuse;
	float tu,tv;
};
/// 2D顶点(屏幕坐标, 颜色)
struct VerColor2D
{
	VerColor2D()
	{
		x=0;
		y=0;
		z=0;
		w=1.0f;
		diffuse=0;
	}
	VerColor2D(float sx,float sy,float sz,DWORD dif)
	{
		x=sx;
		y=sy;
		z=sz;
		w=1.0f;
		diffuse=dif;
	}

	void Set(float sx,float sy,float sz,DWORD dif)
	{
		x=sx;
		y=sy;
		z=sz;
		w=1.0f;
		diffuse=dif;
	}

	enum { Format=FVF_XYZRHW|FVF_DIFFUSE };

	float x,y,z,w;
	DWORD diffuse;
};
/// 2D顶点(屏幕坐标, 纹理坐标)
struct VerText2D
{
	VerText2D()
	{
		x=0;
		y=0;
		z=0;
		w=1.0f;
		tu=0;
		tv=0;
	}
	VerText2D(float sx,float sy,float sz,float stu,float stv)
	{
		x=sx;
		y=sy;
		z=sz;
		w=1.0f;
		tu=stu;
		tv=stv;
	}

	enum { Format=FVF_XYZRHW|FVF_TEX1 };

	float x,y,z,w;
	float tu,tv;
};

struct VerText2D1
{
	VerText2D1()
	{
		p = CVector3f(0,0,0);
		diffuse=0xff000000;
		tu0=0;
		tv0=0;
	}
	VerText2D1(float sx,float sy,float sz,DWORD color, float stu0,float stv0, float stu1,float stv1)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		diffuse=color;
		tu0=stu0;
		tv0=stv0;
		tu1=stu1;
		tv1=stv1;
	};
	void Set(float sx,float sy,float sz,DWORD color, float stu0,float stv0, float stu1,float stv1)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		diffuse=color;
		tu0=stu0;
		tv0=stv0;
		tu1=stu1;
		tv1=stv1;
	};

	enum { Format=FVF_XYZ|FVF_DIFFUSE|FVF_TEX2 };

	CVector3f p;
	DWORD diffuse;
	float tu0,tv0;
	float tu1,tv1;
};

struct VerSkinC
{
	VerSkinC()
	{
		b1=b2=b3=0;
	}

	enum { Format=FVF_XYZB4|FVF_LASTBETA_UBYTE4|FVF_DIFFUSE };

	CVector3f p;
	float b1,b2,b3;
	DWORD Index;
	DWORD diffuse;
};

struct VerSkinNCTNoInd
{
	VerSkinNCTNoInd()
	{
		b1=b2=b3=0;
	}

	enum { Format=FVF_XYZB3|FVF_NORMAL|FVF_TEX1|FVF_DIFFUSE };

	CVector3f p;
	float b1,b2,b3;
	CVector3f np;
	DWORD diffuse;
	float tu,tv;
};

struct VerSkinNT
{
	VerSkinNT()
	{
		b1=b2=b3=0;
		Index[0] = Index[1] = Index[2] = Index[3] = 0;
	}

	enum { Format=FVF_XYZB4|FVF_LASTBETA_UBYTE4|FVF_NORMAL|FVF_TEX1 };

	CVector3f p;
	float b1,b2,b3;
	BYTE Index[4];
	CVector3f np;
	float tu,tv;
};

struct VerSkinN
{
	VerSkinN()
	{
		b1=b2=b3=0;
		Index[0] = Index[1] = Index[2] = Index[3] = 0;
	}

	enum { Format=FVF_XYZB4|FVF_LASTBETA_UBYTE4|FVF_NORMAL };

	CVector3f p;
	float b1,b2,b3;
	BYTE Index[4];
	CVector3f np;
};

struct VerNCT
{
	VerNCT()
	{
		diffuse=0xff000000;
		p = np = CVector3f(0,0,0);
	}
	VerNCT(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif,float stu,float stv)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
		diffuse=dif;
		tu=stu;
		tv=stv;
	};

	void Set(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif,float stu,float stv)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
		diffuse=dif;
		tu=stu;
		tv=stv;
	};

	enum { Format=FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE|FVF_TEX1 };

	CVector3f p;
	CVector3f np;
	DWORD diffuse;
	float tu,tv;
};

struct VerNCT1
{
	VerNCT1()
	{
		diffuse=0xff000000;
		p = np = CVector3f(0,0,0);
	}
	VerNCT1(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif,float stu0,float stv0,float stu1,float stv1)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
		diffuse=dif;
		tu0=stu0;
		tv0=stv0;
		tu1=stu1;
		tv1=stv1;
	};

	void Set(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif,float stu0,float stv0,float stu1,float stv1)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
		diffuse=dif;
		tu0=stu0;
		tv0=stv0;
		tu1=stu1;
		tv1=stv1;
	};

	enum { Format=FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE|FVF_TEX2 };

	CVector3f p;
	CVector3f np;
	DWORD diffuse;
	float tu0,tv0;
	float tu1,tv1;
};

struct VerNCST
{
	VerNCST()
	{
		diffuse=0;
		specular = 0;
		p = np = CVector3f(0,0,0);
	}
	VerNCST(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif,DWORD spe,float stu,float stv)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
		diffuse=dif;
		specular=spe;
		tu=stu;
		tv=stv;
	};

	void Set(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif,DWORD spe,float stu,float stv)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
		diffuse=dif;
		specular=spe;
		tu=stu;
		tv=stv;
	};

	enum { Format=FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE| FVF_SPECULAR | FVF_TEX1 };

	CVector3f p;
	CVector3f np;
	DWORD diffuse;
	DWORD specular;
	float tu,tv;
};

//地编2层纹理坐标
struct VerNCST1
{
	VerNCST1()
	{
		diffuse=0;
		specular = 0;
		p = np = CVector3f(0,0,0);
	}
	VerNCST1(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif,DWORD spe,
	         float stu,float stv, float stu1,float stv1)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
		diffuse=dif;
		specular=spe;
		tu=stu;
		tv=stv;
		tu1=stu1;
		tv1=stv1;
	};

	void Set(float sx,float sy,float sz,float snx,float sny,float snz,DWORD dif,DWORD spe
	         ,float stu,float stv, float stu1,float stv1)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		np.x=snx;
		np.y=sny;
		np.z=snz;
		diffuse=dif;
		specular=spe;
		tu=stu;
		tv=stv;
		tu1=stu1;
		tv1=stv1;
	};

	enum { Format=FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE| FVF_SPECULAR | FVF_TEX2 };

	CVector3f p;
	CVector3f np;
	DWORD diffuse;
	DWORD specular;
	float tu,tv;
	float tu1,tv1;
};

//顶点的复合格式
/*
0 - 顶点 法线 Diffuse
1 - specular ShaderUV1 ShaderUV2
2 - NormalUV1
3 - Level2UV;
4 - Level3UV;
5 - lightmap
*/

struct vTerrain
{
	enum
	{
		ShaderFormat		 = FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE|FVF_SPECULAR|FVF_TEX4|SOURCE_MULTI_STREAM(4),
		ShaderSpecularFormat = FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE|FVF_SPECULAR|FVF_SPECULAR|FVF_TEX4|SOURCE_MULTI_STREAM(5),
		NormalFormat		 = FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE|FVF_TEX2|SOURCE_MULTI_STREAM(3),
		NormalSpecularFormat = FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE|FVF_SPECULAR|FVF_TEX2|SOURCE_MULTI_STREAM(4),
	};
	//enum{LightMap_Format = FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE| FVF_TEX2 |FVF_MULTI_STREAM};
	struct Stream0
	{
		CVector3f p;
		CVector3f np;
		DWORD	  color;	///烘培色
	};

	struct Color
	{
		DWORD     color;   ///高光或者顶点色
	};

	struct Stream2
	{
		float tu0,tv0;
		float tu1,tv1;
	};

	struct StreamUV
	{
		float tu,tv;
	};
};

struct VerTex2
{
	VerTex2()
	{
		p = CVector3f(0,0,0);
		tu0=0;
		tv0=0;
		tu1=0;
		tv1=0;
	}
	VerTex2(float sx,float sy,float sz,float stu0,float stv0,float stu1,float stv1)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		tu0=stu0;
		tv0=stv0;
		tu1=stu1;
		tv1=stv1;
	};

	enum { Format=FVF_XYZ|FVF_TEX2 };

	CVector3f p;
	float tu0,tv0;
	float tu1,tv1;
};


struct VerTex
{
	VerTex()
	{
		p = CVector3f(0,0,0);
		tu=0;
		tv=0;
	}
	VerTex(float sx,float sy,float sz,float stu,float stv)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		tu=stu;
		tv=stv;
	};

	enum { Format=FVF_XYZ|FVF_TEX1 };

	CVector3f p;
	float tu,tv;
};

struct VerColorTex
{
	VerColorTex()
	{
		p = CVector3f(0,0,0);
		diffuse=0xff000000;
		tu=0;
		tv=0;
	}
	VerColorTex(float sx,float sy,float sz,DWORD color, float stu,float stv)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		diffuse=color;
		tu=stu;
		tv=stv;
	};
	void Set(float sx,float sy,float sz,DWORD color, float stu,float stv)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		diffuse=color;
		tu=stu;
		tv=stv;
	};

	enum { Format=FVF_XYZ|FVF_DIFFUSE|FVF_TEX1 };

	CVector3f p;
	DWORD diffuse;
	float tu,tv;
};

struct VerColorTex1
{
	VerColorTex1()
	{
		p = CVector3f(0,0,0);
		diffuse=0xff000000;
		tu0=0;
		tv0=0;
		tu1=0;
		tv1=0;
	}
	VerColorTex1(float sx,float sy,float sz,DWORD color, float stu0,float stv0, float stu1,float stv1)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		diffuse=color;
		tu0=stu0;
		tv0=stv0;
		tu1=stu1;
		tv1=stv1;
	};
	void Set(float sx,float sy,float sz,DWORD color, float stu0,float stv0, float stu1,float stv1)
	{
		p.x=sx;
		p.y=sy;
		p.z=sz;
		diffuse=color;
		tu0=stu0;
		tv0=stv0;
		tu1=stu1;
		tv1=stv1;
	};

	enum { Format=FVF_XYZ|FVF_DIFFUSE|FVF_TEX2 };

	CVector3f p;
	DWORD diffuse;
	float tu0,tv0;
	float tu1,tv1;
};

struct SkltIndex
{
	BYTE  b[4];
};

struct CWeight
{
	float _w[4];
public:
	CWeight() {}
	CWeight( float w0, float w1, float w2, float w3 )
	{
		//_w[0] = (uint8)( w0*200 + 0.5f );
		//_w[1] = (uint8)( w1*200 + 0.5f );
		//_w[2] = (uint8)( w2*200 + 0.5f );
		//_w[3] = (uint8)( w3*200 + 0.5f );
		_w[0] = w0;
		_w[1] = w1;
		_w[2] = w2;
		_w[3] = w3;
	}

	float GetWeight( uint32 nIndex ) const
	{
		return _w[nIndex];
	}
};

struct VertexSkeletalFormat
{
	float		x,y,z;
	float		n, ny, nz;
	float		u,v;
	float		u1,v1;
	float		u2,v2;
	CWeight		w;
	SkltIndex	i;
	enum { Format = FVF_XYZB4|FVF_LASTBETA_UBYTE4|FVF_NORMAL|FVF_TEX3 };//大概是这样把，唯一识别就行啊
};

struct VertexAniFormat
{
	struct Vertex
	{
		CVector3f p;
		CVector3f n;
	};

	struct Tex1
	{
		float u,v;
		enum { Format = FVF_XYZB2|FVF_XYZ|FVF_NORMAL|FVF_TEX1|SOURCE_MULTI_STREAM(2) };
	};

	struct Tex2
	{
		float u,v;
		float u1,v1;
		enum { Format = FVF_XYZB2|FVF_XYZ|FVF_NORMAL|FVF_TEX2|SOURCE_MULTI_STREAM(3) };
	};

	struct Tex3
	{
		float u,v;
		float u1,v1;
		float u2,v2;
		enum { Format = FVF_XYZB2|FVF_XYZ|FVF_NORMAL|FVF_TEX3|SOURCE_MULTI_STREAM(4) };
	};
};

struct RectGroup
{
	bool		bFullText;
	CVector2f	vec[4];
	CVector3f	vec3d[4];
	uint32		Color;
	float		z;
	CVector2f	rtText[4];
	RectGroup()
	{
		bFullText = true;
		for (int32 i = 0; i < 4; ++i)
		{
			vec[i].Init();
			vec3d[i].Init();
			rtText[i].Init();
		}
		Color = 0;
		z = 0.0f;
	}
};


struct PointGroup
{
	CVector3f   vPoint;
	uint32      uColor;
};

struct RenderObjInfo
{
	DWORD		dwSize;
	DWORD		dwType;
	DWORD		dwPrivateFlag[6];
};

//贴图格式
enum eTextureFormat
{
	TF_DXT1			= 0,
	TF_DXT5			= 1,
	TF_RGB24		= 2,
	TF_ARGB32		= 3,
	TF_R5G6B5		= 4,
	TF_A4R4G4B4		= 5,
	TF_A8			= 6,
	TF_XRGB32		= 7,
	TF_L8			= 8,
	TF_R32F			= 9,
	TF_DEPTH		= 10,
	TF_DXT3			= 11,
	TF_COUNT		= 12,
	TF_UNKNOW		= 0xffffffff,
};

//贴图结构
struct sTextureInfo
{
	eTextureFormat	eFormat;		//文件格式
	uint32			uWidth;			//宽
	uint32			uHeight;		//高
	uint32			uMipMap;		//MipMap层数
	uint32			uFrameCount;	//额外帧帧数
	uint32			uFrameCircle;	//帧周期
};

struct sMaterial
{
	CColor4			Diffuse;
	CColor4			Ambient;
	CColor4			Specular;
	CColor4			Emissive;
	float			Power;
	bool operator !=(const sMaterial& mat)
	{
		return Diffuse != mat.Diffuse||Ambient!=mat.Ambient||Specular!=mat.Specular||Emissive!=mat.Emissive||Power!=mat.Power;
	}
};

enum eLightType
{
	ePointLight     = 1,
	eSpotLight      = 2,
	eDirectLight    = 3,
};

struct RenderState : public CGraphicMallocObject
{
	sMaterial		m_Material;
	DWORD			m_Alpharef;
	DWORD			m_Texturefactor;

	DWORD			m_AlphaBlendEnable;
	DWORD			m_SrcBlend;
	DWORD			m_DestBlend;

	DWORD			m_AlphaTestEnable;
	DWORD			m_AlphaTestFun;

	DWORD			m_LightEnable;
	DWORD			m_SpecularEnable;

	DWORD			m_ZTestEnable;
	DWORD			m_ZTestFun;
	DWORD			m_ZWrite;

	DWORD			m_Cull;
	DWORD			m_FillMode;

	DWORD			m_Uv_S0;
	DWORD			m_Uv_S1;
	DWORD			m_Uv_S2;

	SHADER_HANDLE	m_ShaderType;

	DWORD			m_Sample_S0;
	DWORD			m_Sample_S1;
	DWORD			m_Sample_S2;
	DWORD			m_Sample_S3;
	DWORD			m_TexOP_S0;
	DWORD			m_TexOP_S1;
	DWORD			m_TexOP_S2;
	DWORD			m_TexOP_S3;
	DWORD			m_TexArg1_S0;
	DWORD			m_TexArg1_S1;
	DWORD			m_TexArg1_S2;
	DWORD			m_TexArg1_S3;
	DWORD			m_TexArg2_S0;
	DWORD			m_TexArg2_S1;
	DWORD			m_TexArg2_S2;
	DWORD			m_TexArg2_S3;
	DWORD			m_AlpOP_S0;
	DWORD			m_AlpOP_S1;
	DWORD			m_AlpOP_S2;
	DWORD			m_AlpOP_S3;
	DWORD			m_AlpArg1_S0;
	DWORD			m_AlpArg1_S1;
	DWORD			m_AlpArg1_S2;
	DWORD			m_AlpArg1_S3;
	DWORD			m_AlpArg2_S0;
	DWORD			m_AlpArg2_S1;
	DWORD			m_AlpArg2_S2;
	DWORD			m_AlpArg2_S3;
	DWORD			m_UVSrc_S0;
	DWORD			m_UVSrc_S1;
	DWORD			m_UVSrc_S2;
	DWORD			m_UVSrc_S3;
	DWORD			m_UAddr_S0;
	DWORD			m_UAddr_S1;
	DWORD			m_UAddr_S2;
	DWORD			m_UAddr_S3;
	DWORD			m_VAddr_S0;
	DWORD			m_VAddr_S1;
	DWORD			m_VAddr_S2;
	DWORD			m_VAddr_S3;
	DWORD			m_MipMapSample_S0;
	DWORD			m_MipMapSample_S1;
	DWORD			m_MipMapSample_S2;
	DWORD			m_MipMapSample_S3;
	DWORD			m_MipMapBias_S0;
	DWORD			m_MipMapBias_S1;
	DWORD			m_MipMapBias_S2;
	DWORD			m_MipMapBias_S3;
};

typedef struct _TEXLOCKED_RECT
{
	int                 Pitch;
	void*               pBits;
} TEXLOCKED_RECT;

template< class DataType, class ofstr >
inline void SaveMapValue( ofstr& File, DataType& DataMap )
{
	int Num = (int)DataMap.size();
	File.write( (char*)&Num, sizeof( Num ) );
	for ( typename DataType::const_iterator it = DataMap.begin(); it != DataMap.end(); it++ )
	{
		File.write( (char*)&it->first, sizeof( float ) );
		File.write( (char*)&it->second, sizeof( typename DataType::mapped_type ) );
	}
}

template< class DataType, class ofstr >
inline void SaveMapValueWDI( ofstr& File, DataType& DataMap )
{
	int Num = (int)DataMap.size();
	File.Write( (char*)&Num, sizeof( Num ) );
	for ( typename DataType::const_iterator it = DataMap.begin(); it != DataMap.end(); it++ )
	{
		File.Write( (char*)&it->first, sizeof( float ) );
		File.Write( (char*)&it->second, sizeof( typename DataType::mapped_type ) );
	}
}

template< class DataType, class ifstr >
inline void LoadMapValue( ifstr& File, DataType& DataMap )
{
	DataMap.clear();
	int Num = 0;
	File.read( (char*)&Num, sizeof( Num ) );
	for ( int i = 0; i < Num; ++i )
	{
		typename DataType::mapped_type Data;
		float Pos = 0.0f;
		File.read( (char*)&Pos,  sizeof( float ) );
		File.read( (char*)&Data, sizeof( typename DataType::mapped_type ) );
		DataMap[Pos] = Data;
	}
}

template< class DataType, class ifstr >
inline void LoadMapValueSafe( ifstr& File, DataType& DataMap )
{
	DataMap.clear();
	int Num = 0;
	File.SafeRead(&Num, sizeof( Num ) );
	for ( int i = 0; i < Num; ++i )
	{
		typename DataType::mapped_type Data;
		float Pos = 0.0f;
		File.SafeRead(&Pos, sizeof( float ) );
		File.SafeRead(&Data, sizeof( typename DataType::mapped_type ) );
		DataMap[Pos] = Data;
	}
}
}
