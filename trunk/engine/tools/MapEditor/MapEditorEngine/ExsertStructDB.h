#pragma once
#include "MEGraphic.h"
#include "MathAlgorithm.h"


namespace sqr
{
	#define MINIMAP_SPACE	256
	#define GRID_SPACE		64.0f
	#define REGION_SPACE	16
	#define FIX_GRID		(REGION_SPACE*REGION_SPACE/2+REGION_SPACE/2)
	#define REGION_STEP_DIVIDE	2
	#define TILE_REGION_STEP_DIVIDE	2

	const int TILE_WIDTH_IN_REGION		= TILE_REGION_STEP_DIVIDE * TILE_REGION_STEP_DIVIDE ;
	const int TILE_REGION_SPACE			= REGION_SPACE * TILE_WIDTH_IN_REGION;
	const int REGION_DIVIDE				= REGION_STEP_DIVIDE * REGION_STEP_DIVIDE;
	const int REGION_DIVIDESPACE		= REGION_SPACE / REGION_STEP_DIVIDE;
	const float fRegHalfDivideWidth		= GRID_SPACE * ( REGION_SPACE / REGION_STEP_DIVIDE ) * 0.5f;
	const float fRegDivideStepRecipocal = 1 / REGION_STEP_DIVIDE;

	enum EAUTO_GENERATION_TYPE
	{
		EAGT_NONE = 0,
		EAGT_GRASS = 1,
		EAGT_TREE = 2,
	};

	enum ELOD_LEVEL
	{
		ELL_0 = 0,
		ELL_1,
		ELL_2
	};

	enum RESSETGROUP_TYPE
	{
		RSG_COMMON = 0, //普通图素包
		RSG_NPC    = 1, //npc图素包
		RSG_FRUTEX = 2, //树图素包
		RSG_TREE   = 3, //灌木图素包
		RSG_GRASS  = 4, //草图素包
		RSG_CNT    = 5, 
	};

	enum ETRANS_TYPE
	{
		ETT_VERTEX = 0,
		ETT_TEXTURE,
		ETT_NONE
	};

	enum ETrans_Type
	{
		ETT_TRANSLUCENT = 0,
		ETT_SOLID,
		ETT_COUNT
	};

	enum EEdit_State
	{
		EES_MOVING = 0,
		EES_PLACED,
		EES_DANGLING,
		EES_DELETE
	};

	enum ETrans_State
	{
		EES_AUTO   = 0, ///自动计算是否消隐
		EES_TRANS     , ///一定消隐，可以根据自动计算得到也可以利用强制性设置得到
		EES_UNTRANS     ///一定不消隐，可以根据自动计算得到也可以利用强制性设置得到
	};

	enum EEdit_State_Placed
	{
		EESP_ROTATE = 0,
		EESP_SCALE,
		EESP_LIFT,
		EESP_SHIFTX,
		EESP_SHIFTZ,
		EESP_NONE
	};

	enum EEdit_Scale_Type
	{
		EES_SCALEXYZ = 0,
		EES_SCALEX,
		EES_SCALEY,
		EES_SCALEZ,
		EES_SCALEXY,
		EES_SCALEXZ,
		EES_SCALEYZ,
	};

	enum ECenter_Type
	{
		ECT_CENTER = 0,
		ECT_CORNER,
		ECT_XMIDWAY,
		ECT_ZMIDWAY,
		ECT_NONE
	};

	enum EHEIGHT_TEMPLATE_TYPE
	{
		EHTT_GAUSSCUT = 0,
		EHTT_LINEARCUT,
		EHTT_CNT
	};

	enum EHEIGHT_TYPE
	{
		EHT_LINEAR = 0,
		EHT_GAUSS,
		EHT_CNT
	};

	enum EWAY_TYPE
	{
		EWT_INC = 0,
		EWT_DEC,
		EWT_CUT,
		EWT_SMOOTH,
		EWT_CNT
	};

	enum EDIR_TYPE
	{
		EDT_X = 0,
		EDT_Z,
		EDT_BOTH,
		EDT_CNT
	};

	///alpha brush type
	enum EALPHA_TYPE
	{
		EAT_LINEAR = 0,		///方形
		EAT_SPHERE = 1,		///球形
		EAT_CROSS  = 2,		///十字形
		EAT_MI	   = 3,		///米字形
	};

	enum EEDIT_MESH
	{
		EEM_TERRAIN = 0,
		EEM_WATER
	};

	struct SBlockInfo
	{
		SBlockInfo::SBlockInfo()
		{
			nBlockMask = 0;
			nBlockType = 3; // high
		}

		int nBlockMask;
		int nBlockType;
	};

	struct SLogicHeightInfo
	{
		SLogicHeightInfo::SLogicHeightInfo()
		{
			fLogicHeight = 0.0f;
		}
		float fLogicHeight;
	};

	struct STextureNodeInfo
	{
		STextureNodeInfo()
		{
			strTextureNames[0] = strTextureNames[1] = strTextureNames[2] = strTextureNames[3] = "";
			dwTexIndex[0] = dwTexIndex[1] = dwTexIndex[2] = dwTexIndex[3] = 0xff;
		}

		DWORD dwGridIndex;
		BYTE dwTexIndex[4];
		string strTextureNames[4];
	};

	struct vTerrainBuffer
	{
		//enum{Format=FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE| FVF_SPECULAR | FVF_TEX4};
		//enum{LightMap_Format = FVF_XYZ|FVF_NORMAL|FVF_DIFFUSE| FVF_SPECULAR | FVF_TEX4 |FVF_MULTI_STREAM};

		struct VertexBuf0
		{
			CVector3f p;
			CVector3f np;
			DWORD diffuse;
		};

		struct VertexBuf1
		{
			DWORD specular;
			float tu0,tv0;
			float tu1,tv1;
		};

		struct VertexUV
		{
			float tu,tv;
		};
	};

	//typedef map<ModelName, vector<PieceClassColor> > ModelGroupColorMap;
}