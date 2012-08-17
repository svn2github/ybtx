#pragma once
#include "CDistanceFont.h"
#define TEX_FONT_COLOR_DEF 9
/*
	贴图字体，支持自定义的材质字体
	如果材质中没有定义该文字，用distance font作为默认字体
	关与材质字体的编辑，在GUI工具中
*/

namespace sqr
{
	class CTextureFont : public CDistanceFont
	{
	public:
		CTextureFont();
		virtual	void		InitTextKey(RenderTextParam& param, bool bInKey);
		virtual CVector2f	CalculateSize(const wstring& text);

	private:
		const CharInfo*		GetCharInfo(wchar_t ch, uint8 idx = 0);
		uint8				GetSimilarColor(const CColor& color);
		DWORD				GetShaderID();
	};
}