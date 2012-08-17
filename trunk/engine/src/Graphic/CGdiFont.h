#pragma once
#include "CFontBase.h"
/**
	@class CGdiFont

	采用GDI内置的Font实现的字体.
	采用8位灰度级的字体点阵
	因为GDI字体的大小在创建时就已经决定了, 所以放缩时需要重新创建合适字号的.
	由于可以直接读取系统TTF字体, 这样就不受字体数量的限制, 更换起来也更方便.
	同时也避免了预生成类字体需要内存来缓存所有字符信息所带来的内存占用问题.
	为了兼容GUI系统(不支持非等宽字符的排版), 所有字号都是偶数(这样汉字才能与ASCII对齐)

	@todo 利用AddFontMemResourceEx添加自定义字体(客户端自带TTF文件)
 */

namespace sqr
{

class CGdiFont : public CFontBase
{
public:
	CGdiFont();

private:
	bool OnCreate();
	void OnResize();
	void OnDestroy();
	const CharInfo* GetCharInfo(wchar_t ch, uint8 idx = 0);
	void CacheCharacter(wchar_t ch, int x, int y);
	DWORD GetShaderID();
	virtual void	PreProcessStr();

	virtual float	GetLineSpacing() const;
	virtual float	GetFontHeight() const;
	virtual float	GetFontGlyphHeight(wchar_t c)	const;

private:
	struct CharInfoEx : public CharInfo
	{
		uint size;
	};
	typedef GHashMap<wchar_t, CharInfoEx> CharInfoMap;
private:
	HDC memDC;
	HFONT font;
	HFONT m_defFont;
	float m_ascender;
	float m_descender;
	//! (ascender - descender) + linegap
	float m_height;

	CharInfoMap charInfoMap;
	GSet<wchar_t> invalidChar;	///< 非正常可显示字符
};

}// namespace sqr
