#pragma once
#include "CEditorClassPre.h"
#include "CEditAnimate.h"
#include "CEditAnimationsRes.h"
#include "CEditModel.h"
#include "CEditPieceGroup.h"
#include "CEditRenderStyle.h"
#include "CEditSkeletal.h"

namespace sqr_tools
{
	//工具将用到一些公用模型，渲染类型等等
	//TP_	: Tools Public data
	//EPRS_	: CEditPieceRenderStyle 类型
	//RO_	: CRenderOperation		类型
	class CEditorCore
	{
	public:
		static bool Initialize(void);
		static bool Destroy(void);
	public:
		static	float					TP_F_GoldenSection;	// 黄金分割参数

	public: //公用RS
		static CEditRenderStyle	TR_EPRS_SELECT;		// 选择物件显示网格_RS
		static CEditRenderStyle	TR_EMRS_SELECT;		// 选择物件显示_RS
		static CEditRenderStyle	TR_EMRS_HOLD;		// 选择物件悬停显示_RS
	
	public:
		static CRenderOperation			TP_RO_BONE;			// 骨头模型
		static CRenderOperation			TP_RO_CONE;			// 三角锥
		static CRenderOperation			TP_RO_SPHERE;		// 球模型

	public:
		static ITexture*				TP_TX_BLACKWHITE;	//棋盘格黑白图
		static ITexture*				TP_TX_VIOLETWHITE;	//棋盘格紫白图
		static ITexture*				TP_TX_BLACK;		//黑图
		static ITexture*				TP_TX_GRAY;			//灰图
		static ITexture*				TP_TX_WHITE;		//白图
	};
}