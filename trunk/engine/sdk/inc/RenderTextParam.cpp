#include "stdafx.h"
#include "RenderTextParam.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"


namespace sqr
{
	void RenderTextParam::SetText(const char* text)
	{
		if(text)
			m_text = utf8_to_utf16(text).c_str();
		else
			m_text.clear();
	}

	RenderTextParam::RenderTextParam(){}

	//------------------------------------------------------------------------------
	RenderTextParam::RenderTextParam( float size )
		: m_color(CColor::White)
		, m_gradualColor(0)
		, m_backColor(CColor::Black)
		, m_rect(0, 0, 0xffff, 0xffff)
		, m_effectMask(0)
		, m_size(size)
		, m_idx(0)
	{
		//emtpy
	}
}
