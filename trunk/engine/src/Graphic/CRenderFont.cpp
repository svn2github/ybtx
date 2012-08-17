#include "stdafx.h"
#include "CRenderFont.h"
#include "CDistanceFont.h"
#include "CGdiFont.h"
#include "CTextureFont.h"
#include "CGpuProgramMgr.h"
#include "CFontMgr.h"
#include "CodeCvs.h"

//------------------------------------------------------------------------------
namespace sqr
{

//------------------------------------------------------------------------------
CRenderFont::CRenderFont( const string& szName, const wstring& szAlias )
		: CResUnit( szName, szAlias, eRT_eFont )
		, m_bAlignToPixel(true)
		, m_urefCount(0)
		, m_bFreed(false)
{
	AddRef();
}

//------------------------------------------------------------------------------
CRenderFont::~CRenderFont(void)
{
	this->realFont = NULL;
}

//------------------------------------------------------------------------------
int CRenderFont::Create(int32 id, bool bBold, float fSize)
{
	if (id == CFontMgr::DISTANCE_FONT_ID)
	{
		// scaleable typeface
		if ( CGpuProgramMgr::GetInst()->GetShaderVer() < Shader_V_2 )
		{
			this->realFont = new CGdiFont();
			id = 0;
		}
		else
			this->realFont = new CDistanceFont();
	}
	else if (id == CFontMgr::TEXTURE_FONT_ID)
	{
		// scaleable typeface
		this->realFont = new CTextureFont();
	}
	else
	{
		this->realFont = new CGdiFont();
	}

	if (this->realFont->Create(id, bBold, fSize))
		return S_OK;
	else
		return E_FAIL;
}

int32 CRenderFont::GetFontID()
{
	return this->realFont->GetID();
}

void CRenderFont::DrawTextBatch(const RenderTextParam& param)
{
	if ( wcslen(param.GetText())==0 )
		return;
	this->realFont->Resize(param.GetSize());
	this->realFont->DrawText(param);
}

//------------------------------------------------------------------------------
void
CRenderFont::Begin()
{
	this->realFont->Begin();
}

//------------------------------------------------------------------------------
void
CRenderFont::End()
{
	this->realFont->End();
}

//------------------------------------------------------------------------------
void 
CRenderFont::SetFrameFirst()
{
	this->realFont->SetFrameFirst();
}

//------------------------------------------------------------------------------
CVector2f CRenderFont::CalculateSize( const wchar_t* text )
{
	return this->realFont->CalculateSize(text);
}

bool CRenderFont::IsBold()
{
	return this->realFont->IsBold();
}

void CRenderFont::Free()
{
	if (m_urefCount > 0)
	{
		m_bFreed = true;
		GfkLogErr("CRenderFont", "Delete in wrong time");
	}
	else
	{
		m_bFreed = false;
		FreeRes();
	}
}

void CRenderFont::SubRenderRef()
{
	--m_urefCount;
	if (m_urefCount <=0 && m_bFreed)
		Free();
}

#define CATCH_DRAWTEXT_EXCEPTION

void CRenderFont::DrawText( const RenderTextParam& param )
{
	if (wcslen(param.GetText())==0)
		return;
	this->realFont->SetAlignToPixel(m_bAlignToPixel);
	this->realFont->Resize(param.GetSize());
	this->Begin();

#ifdef CATCH_DRAWTEXT_EXCEPTION
	SQR_TRY
	{
		this->realFont->DrawText(param);
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg(" - realFont->DrawText(param) 抛了异常\n");
		const wstring drawText = param.GetText();
		const string u8drawText = utf16_to_utf8(drawText.c_str());
		exp.AppendMsg(u8drawText.c_str());
		GfkLogExp(exp);
	}
	SQR_TRY_END
#else
	this->realFont->DrawText(param);
#endif

	this->End();
}

void CRenderFont::DrawText(RenderTextParam& param, bool bInKey)
{
	this->realFont->InitTextKey(param, bInKey);
	DrawText( param );
}

void CRenderFont::Resize(float new_size)
{
	this->realFont->Resize(new_size);
}

void CRenderFont::BeginFlush(float size)
{
	this->realFont->SetAlignToPixel(m_bAlignToPixel);
	this->realFont->Resize(size);
	Begin();
}

void CRenderFont::AddDrawContent(const RenderTextParam& param)
{
	if (wcslen(param.GetText())==0)
		return;
#ifdef CATCH_DRAWTEXT_EXCEPTION
	SQR_TRY
	{
		this->realFont->DrawText(param);
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg(" - realFont->DrawText(param) 抛了异常");
		GfkLogExp(exp);
	}
	SQR_TRY_END
#else
	this->realFont->DrawText(param);
#endif

}

void CRenderFont::EndFlush()
{
	End();
}


size_t CRenderFont::GetCharAtPixel( const wstring &text, size_t start_char, float pixel ) const
{
	return this->realFont->GetCharAtPixel(text, start_char, pixel);
}

size_t CRenderFont::GetCharAtPixel(const wstring& text, float pixel) const
{ 
	return GetCharAtPixel(text, 0, pixel); 
}

float CRenderFont::GetLineSpacing()	const
{
	return realFont->GetLineSpacing();
}

float CRenderFont::GetFontHeight() const
{
	return realFont->GetFontHeight();
}

float CRenderFont::GetFontGlyphHeight(wchar_t c)	const
{
	return realFont->GetFontGlyphHeight(c);
}

float CRenderFont::GetScale(float size)
{
	return realFont->GetScale(size);
}

void CRenderFont::UpdateCharInfo( const wstring& text )
{
	if ( text.empty() || text.length() == 0 )
		return;

	realFont->UpdateCharInfo(text);
}

}// namespace sqr
