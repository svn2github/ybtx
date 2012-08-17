#pragma once

namespace sqr_tools
{
	enum BRUSH_TYPE
	{
		BRUSH_SQUARE = 0, 
		BRUSH_CIRCLE,
		BRUSH_REGULAR,
		BRUSH_POLYGON
	};

	enum EBlockType			
	{ 
		EBT_NONE = 0,  
		EBT_LOW = 1, 
		EBT_MID = 2, 
		EBT_HIGH = 3, 
		EBT_CNT = 4
	};

	enum EBlockEditType			
	{ 
		EET_WRITE = 0, 
		EET_ERASE 
	};

	enum EEditBrushStyle	
	{ 
		EBT_NORMAL = 0,
		EBT_EXCAT = 1
	};///刷地表的类型 普通1*1 精确 2*2

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

	enum EHEIGHT_TEMPLATE_TYPE
	{
		EHTT_GAUSSCUT = 0,
		EHTT_LINEARCUT,
		EHTT_CNT
	};

	enum EEdit_State
	{
		EES_NONE = 0,
		EES_MOVING,
		EES_PLACED,
		EES_DELETE
	};

	///EMT_XWAY前后镜像 EMT_ZWAY左右镜像
	enum EMirrorType		
	{ 
		EMT_NONE = 0,   
		EMT_XWAY,    
		EMT_ZWAY 
	};

	///ECT_ALL: copy terrain and object
	enum ECopyType		
	{ 
		ECT_OBJECT = 0, 
		ECT_TERRAIN, ECT_ALL,	  
		ECT_GENERATE 
	};

	//地表操作类型
	enum ETERRAIN_OP_TYPE
	{	
		EOP_LINE_INCX = 0,
		EOP_LINE_INCZ,
		EOP_LINE_INCB,
		EOP_LINE_DECX,
		EOP_LINE_DECZ,
		EOP_LINE_DECB,
		EOP_GAUS_INC,
		EOP_GAUS_DEC,
		EOP_SMOOTH,
		EOP_ALIGN,
		EOP_NONE
	};

}
