#include "StdAfx.h"
#include "CDebugText.h"
#include "CCamera.h"
#include "TSqrAllocator.inl"
#include "StringHelper.h"
#include "CWindowTarget.h"
#include "CGraphic.h"
#include "CRenderFont.h"
#include "BaseHelper.h"
#include "CodeCvs.h"
#include "CFontMgr.h"
#include "CFontCfg.h"

#pragma warning(disable:4996)

//------------------------------------------------------------------------------
namespace sqr
{

	static const int	LINE_SPACE = 2;
	static const float	TEXT_SIZE  = 14.5f;

	CDebugText::CDebugText()
		: m_font(0)
	{
		Open();
		if(CMainWindowTarget::HasInst())
			CMainWindowTarget::GetInst()->InsertRenderer(this);
	}

	CDebugText::~CDebugText()
	{
		if(CMainWindowTarget::HasInst())
			CMainWindowTarget::GetInst()->DeleteRenderer(this);
		Close();
	}

	//------------------------------------------------------------------------------
	bool CDebugText::Open()
	{
		if(!CFontCfg::HasInst())
			return false;

		size_t font_count = CFontCfg::GetInst()->GetGDIFontCount();
		for ( size_t font_index = 0; font_index < font_count; ++font_index )
		{
			wstring szFontName = CFontCfg::GetInst()->GetGDIFontName(font_index);
			string  szFontFile = CFontCfg::GetInst()->GetGDIFontFileName(font_index);
			CFontMgr::GetInst()->AddFontInfo(szFontName.c_str(), szFontFile.c_str());
		}

		HRESULT hr = CGraphic::GetInst()->CreateFont(0, false, TEXT_SIZE, &m_font);
		if ( FAILED(hr) )
		{
			GfkLogErrOnce("DebugTextRenderer´´½¨×ÖÌåÊ§°Ü");
		}

		return true;
	}

	//------------------------------------------------------------------------------
	void CDebugText::Close()
	{
		this->textBuffer.clear();
		SafeRelease(m_font);
	}

	//------------------------------------------------------------------------------
	void CDebugText::DrawDebugText( const string& text, DWORD color/* = 0xffffffff*/ )
	{
		TextUnit newUnit;
		newUnit.text = text.c_str();
		newUnit.color = color;
		this->textBuffer.push_back(newUnit);
	}

	//------------------------------------------------------------------------------
	void CDebugText::DrawDebugText(DWORD color, const char* format, ...)
	{
		char strBuffer[512];

		va_list args;
		va_start(args, format);
		_vsnprintf( strBuffer, 512, format, args );
		strBuffer[511] = '\0';
		va_end(args);

		return this->DrawDebugText( strBuffer, color );

	}

	//------------------------------------------------------------------------------
	void CDebugText::Render()
	{
		if ( 0 != m_font )
		{
			CIRect Vrect;
			CMainWindowTarget::GetInst()->GetRect(Vrect);
			float top = (float)Vrect.top;
			for (TextBuffer::iterator it = this->textBuffer.begin() ; it != this->textBuffer.end(); ++it)
			{
				TextUnit& textUnit = (*it);
				RenderTextParam	textParam(TEXT_SIZE);
				textParam.SetColor(textUnit.color);
				textParam.SetText(gbk_to_utf16(textUnit.text.c_str()));
				textParam.SetPosition((float)Vrect.left, top);
				m_font->DrawText(textParam);
				top += m_font->GetFontHeight() + LINE_SPACE;
			}
			//CGraphic::GetInst()->DrawStack();
		}
		this->textBuffer.clear();
	}

	//------------------------------------------------------------------------------
	void
	DebugText( uint color, const char* fmt, ... )
	{
		if ( !CDebugText::HasInst() )
		{
			return;
		}

		string strBuffer;
		va_list args;
		va_start(args, fmt);

		//_vsnprintf( strBuffer, 512, fmt, args );
		//strBuffer[511] = '\0';
		vformat(strBuffer, fmt, args);
		va_end(args);

		while ( strBuffer.find('\n') != string::npos )
		{
			string szCurLine = strBuffer.substr(0, strBuffer.find('\n'));
			strBuffer = strBuffer.substr(strBuffer.find('\n') + 1, strBuffer.size());
			CDebugText::GetInst()->DrawDebugText(szCurLine, color);
		}
		if ( strBuffer.size() != 0 )
		{
			CDebugText::GetInst()->DrawDebugText(strBuffer, color);
		}
	}
}
