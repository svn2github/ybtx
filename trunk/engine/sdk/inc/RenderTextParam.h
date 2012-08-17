#pragma once
#include "CColor.h"
#include "CVector3.h"
#include "CRectangle.h"
#include "CGraphicMallocObject.h"
namespace sqr
{
/// 字体绘制效果.表情是一种特殊的字体
struct FontEffect
{
	enum
	{
		Italic	= 1<<0,	///< 斜体
		Outline	= 1<<1,	///< 描边
		Shadow	= 1<<2,	///< 阴影
		Gradual	= 1<<3, ///< 渐变色

		Vertical = 1<<29,	///< 文字竖排
		AutoAdapy = 1<<30,	///< 表情自适应
		Multiline = 1<<31,	///< 多行
	};

	typedef uint32 Mask;
};

struct RenderTextParam : public CGraphicMallocObject
{
public:
	RenderTextParam(float size);
	RenderTextParam();
public:
	void SetPosition(float x, float y, float z = 0.0f, float offset = 0.0f);
	void SetPosition(const CVector3f& pos);
	void SetOffset(float offset);
	const CVector3f& GetPosition() const;
	float GetSize() const;
	void SetText(const char* text);
	void SetText(const char* text, size_t length);
	void SetText(const string& text);
	void SetText(const wstring& text);
	const wchar_t* GetText() const;
	void SetColor(const CColor& color);
	const CColor& GetColor() const;
	void SetGradualColor(const CColor& color);
	const CColor& GetGradualColor() const;
	void SetBackColor(const CColor& color);
	const CColor& GetBackColor() const;
	void SetRect(const CFRect& rect);
	const CFRect& GetRect() const;
	void SetMultiline(bool multiline);
	bool IsMultiline() const;
	void SetVertical(bool Vertical);
	bool IsVertical() const;
	void SetOutline(bool outline);
	bool IsOutline() const;
	void SetShadow(bool shadow);
	bool IsShadow() const;
	void SetItalic(bool italic);
	bool IsItalic() const;
	void SetFontEffect(FontEffect::Mask mask);
	FontEffect::Mask GetFontEffect() const;
	void SetIdx(uint8 idx);
	const uint8 GetIdx() const;
private:
	CVector3f m_position;
	CFRect m_rect;
	GWString m_text;
	CColor m_color;
	CColor m_gradualColor;
	CColor m_backColor;
	float  m_size;
	uint8 m_idx;
	FontEffect::Mask m_effectMask;
};


//------------------------------------------------------------------------------
inline void
RenderTextParam::SetText( const string& text )
{
	SetText(text.c_str());
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetText( const char* text, size_t length )
{
	if(length != -1)
		SetText(string(text, length));
	else
		SetText(text);
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetText( const wstring& text )
{
	m_text = text.c_str();
}

//------------------------------------------------------------------------------
inline float
RenderTextParam::GetSize() const
{
	return m_size;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetPosition( float x, float y, float z /*= 0.0f*/, float offset /*= 0.0f*/ )
{
	m_position.Init(x - offset, y, z);
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetOffset( float offset )
{
	m_position.x -= offset;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetGradualColor( const CColor& color )
{
	m_gradualColor = color;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetBackColor( const CColor& color )
{
	m_backColor = color;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetRect( const CFRect& rect )
{
	m_rect = rect;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetMultiline( bool multiline )
{
	if (multiline)
		m_effectMask |= FontEffect::Multiline;
	else
		m_effectMask &= ~FontEffect::Multiline;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetVertical( bool Vertical )
{
	if (Vertical)
		m_effectMask |= FontEffect::Vertical;
	else
		m_effectMask &= ~FontEffect::Vertical;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetOutline( bool outline )
{
	if (outline)
		m_effectMask |= FontEffect::Outline;
	else
		m_effectMask &= ~FontEffect::Outline;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetShadow( bool shadow )
{
	if (shadow)
		m_effectMask |= FontEffect::Shadow;
	else
		m_effectMask &= ~FontEffect::Shadow;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetItalic( bool italic )
{
	if (italic)
		m_effectMask |= FontEffect::Italic;
	else
		m_effectMask &= ~FontEffect::Italic;
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetPosition( const CVector3f& pos )
{
	m_position = pos;
}

//------------------------------------------------------------------------------
inline const CVector3f&
RenderTextParam::GetPosition() const
{
	return m_position;
}

//------------------------------------------------------------------------------
inline const wchar_t*
RenderTextParam::GetText() const
{
	return m_text.c_str();
}

//------------------------------------------------------------------------------
inline const CColor&
RenderTextParam::GetColor() const
{
	return m_color;
}
//------------------------------------------------------------------------------
inline 
void RenderTextParam::SetColor( const CColor& color)
{
	m_color = color;
}

//------------------------------------------------------------------------------
inline const CColor&
RenderTextParam::GetGradualColor() const
{
	if (0 == m_gradualColor || (0 == (m_effectMask & FontEffect::Gradual)))
		return m_color;
	return m_gradualColor;
}

//------------------------------------------------------------------------------
inline const CColor&
RenderTextParam::GetBackColor() const
{
	return m_backColor;
}

//------------------------------------------------------------------------------
inline const CFRect&
RenderTextParam::GetRect() const
{
	return m_rect;
}

//------------------------------------------------------------------------------
inline bool
RenderTextParam::IsMultiline() const
{
	return 0 != (m_effectMask & FontEffect::Multiline);
}

//------------------------------------------------------------------------------
inline bool
RenderTextParam::IsVertical() const
{
	return 0 != (m_effectMask & FontEffect::Vertical);
}

//------------------------------------------------------------------------------
inline bool
RenderTextParam::IsOutline() const
{
	return 0 != (m_effectMask & FontEffect::Outline);
}

//------------------------------------------------------------------------------
inline bool
RenderTextParam::IsShadow() const
{
	return 0 != (m_effectMask & FontEffect::Shadow);
}

//------------------------------------------------------------------------------
inline bool
RenderTextParam::IsItalic() const
{
	return 0 != (m_effectMask & FontEffect::Italic);
}

//------------------------------------------------------------------------------
inline void
RenderTextParam::SetFontEffect( FontEffect::Mask mask )
{
	m_effectMask = mask;
}

//------------------------------------------------------------------------------
inline FontEffect::Mask
RenderTextParam::GetFontEffect() const
{
	return m_effectMask;
}

//-------------------------------------------------------------------------------
inline void 
RenderTextParam::SetIdx(uint8 idx)
{
	m_idx = idx;
}

//-------------------------------------------------------------------------------
inline const uint8 
RenderTextParam::GetIdx() const
{
	return m_idx;
}

}
