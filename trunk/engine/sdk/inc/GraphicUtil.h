#pragma once

#include "CRenderStyle.h"
namespace sqr
{
	//默认渲染类型1
	struct DEFAULT1RS : public STATIC_RS
	{
		DEFAULT1RS();
	};

	//默认点取样渲染类型
	struct PONITSAMPLERS : public DEFAULT1RS
	{
		PONITSAMPLERS();
	};

	//字体渲染状态
	struct FONTRS : public STATIC_RS
	{
		FONTRS();
	};

	//GUI状态
	struct GUIRS : public STATIC_RS
	{
		GUIRS();
	};

	//字体渲染状态
	struct GUIMIRRORRS : public STATIC_RS
	{
		GUIMIRRORRS();
	};

	//字体渲染状态
	struct GUI_COLORADDRS : public STATIC_RS
	{
		GUI_COLORADDRS();
	};

	//单层地图渲染状态
	struct MAP1LRS : public STATIC_RS
	{
		MAP1LRS();
	};

	//2层地图渲染状态
	struct MAP2LRS : public STATIC_RS
	{
		MAP2LRS();
	};

	//4层
	struct MAP4LRS : public STATIC_RS
	{
		MAP4LRS();
	};

	//阴影渲染状态
	struct SHADOWRS : public STATIC_RS
	{
		SHADOWRS();
	};

	//半透明当灰度
	struct ALPHA2GRAYRS : public STATIC_RS
	{
		ALPHA2GRAYRS();
	};

	//ColorKey
	struct NONETEXTALPHA : public GUIRS
	{
		NONETEXTALPHA();
	};

	//ColorKey
	struct NONETEXTALPHALINEAR : public NONETEXTALPHA
	{
		NONETEXTALPHALINEAR();
	};

	//字体渲染状态
	struct GUISHADOWRS : public GUIRS
	{
		GUISHADOWRS();
	};


	//GUI专用
	struct GUIALPHA : public GUIRS
	{
		GUIALPHA();
	};

	struct GUIBIGMAP: public GUIRS
	{
		GUIBIGMAP();
	};

	struct EMTRS: public STATIC_RS
	{
		EMTRS();
	};

	struct LINERS : public STATIC_RS
	{
		LINERS();
	};

	struct GUIALPHABLEND : public GUIRS
	{
		GUIALPHABLEND();
	};

	//字体缓冲区索引
	struct TRIAGLE_INDEX
	{
		TRIAGLE_INDEX();
		GVector<WORD>		m_IndexBuf;

		operator const WORD*()
		{
			return &( m_IndexBuf[0] );
		}
	};
}
