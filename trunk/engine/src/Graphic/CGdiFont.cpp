//------------------------------------------------------------------------------
//  CGdiFont.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CGdiFont.h"
#include "CGraphic.h"
#include "CFontMgr.h"
#include "CodeCvs.h"
#include "CHardwareCapacity.h"
#include "TSqrAllocator.inl"

namespace sqr
{
// 参考Photoshop中的"锐利"效果
const TBYTE AlphaLevel[65] =
{
	0,24,36, 46, 54, 62, 69, 75, 82, 88, 93, 99,104,110,114,120,124,
	129,133,138,142,147,151,155,159,163,167,171,174,178,181,185,189,
	192,195,198,202,205,208,211,213,216,219,222,224,227,229,231,234,
	236,238,240,242,244,245,247,248,250,251,252,253,254,254,255,255
};
const uint GdiShaderId = 1|MakeGroupID(4)|SSI_BASE_PS_SYSTEM_ID ;

//------------------------------------------------------------------------------
CGdiFont::CGdiFont()
		: memDC(NULL)
		, font(NULL)
		, m_defFont(NULL)
		, m_ascender(0)
{
	m_disChar.clear();
	this->composeSizeMap.clear();
}

//------------------------------------------------------------------------------
bool
CGdiFont::OnCreate()
{
	do
	{
		// create dc
		this->memDC = ::CreateCompatibleDC(NULL);
		if (NULL == this->memDC)
			break;
		::SetMapMode(this->memDC, MM_TEXT);
		::SetTextColor(this->memDC, RGB(255, 255, 255));
		::SetBkColor(this->memDC, RGB(0, 0, 0));

		// create font
		int		font_size = (int)GetSize();
		GWString szName	  = CFontMgr::GetInst()->GetGDIFontName(GetID()); 
		this->font = ::CreateFontW(-font_size, 0, 0, 0,
		                          this->IsBold() ? FW_BOLD : FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		                          OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								  FF_DONTCARE | DEFAULT_PITCH, szName.c_str());
		// create default font
		GWString szDefName = CFontMgr::GetInst()->GetGDIDefFontName();
		if (szDefName.length() > 0)
		{
			this->m_defFont = ::CreateFontW(-font_size, 0, 0, 0,
				this->IsBold() ? FW_BOLD : FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				FF_DONTCARE | DEFAULT_PITCH, szDefName.c_str());
		}


		if (NULL == this->font)
			break;
		::SelectObject(this->memDC, this->font);
		// m_ascender line
		TEXTMETRIC tm;
		::GetTextMetrics(this->memDC, &tm);
		m_ascender	= static_cast<float>(tm.tmAscent);
		m_descender = static_cast<float>(tm.tmDescent);
		m_height	= static_cast<float>(tm.tmHeight);

		bool InitText = false;
		if( !InitText && CHardwareCapacity::GetInst()->IsSupportTexture(TFMT_A8))
		{
			m_PixelSize = 1;
			InitText = SUCCEEDED(CGraphic::GetInst()->CreateEmptyTexture(
				CFontBase::st_CacheTextureSize, CFontBase::st_CacheTextureSize, 1, TF_A8, &this->cacheTex));
		}

		if( !InitText && CHardwareCapacity::GetInst()->IsSupportTexture(TFMT_A8R8G8B8) )
		{
			m_PixelSize = 4;
			InitText = SUCCEEDED(CGraphic::GetInst()->CreateEmptyTexture(
				CFontBase::st_CacheTextureSize, CFontBase::st_CacheTextureSize, 1, TF_ARGB32, &this->cacheTex));
		}

		return CFontBase::OnCreate() && InitText;
	}
	while (0);

	return false;
}

//------------------------------------------------------------------------------
void
CGdiFont::OnResize()
{
	this->OnDestroy();
	this->OnCreate();
}

//------------------------------------------------------------------------------
void
CGdiFont::OnDestroy()
{
	if (NULL != this->font)
	{
		::DeleteObject(this->font);
		this->font = NULL;
	}
	if (NULL != this->memDC)
	{
		::DeleteDC(this->memDC);
		this->memDC = NULL;
	}
	if (NULL != this->m_defFont)
	{
		::DeleteObject(this->m_defFont);
		this->m_defFont = NULL;
	}
	this->charInfoMap.clear();
	this->invalidChar.clear();
	CFontBase::OnDestroy();
}

//------------------------------------------------------------------------------
void
CGdiFont::CacheCharacter( wchar_t ch, int x, int y )
{
	int codeline = 0;
	CIRect charRect;
	SQR_TRY{
		if(cacheTex == NULL)
			return;

		GLYPHMETRICS gm;
		MAT2 mat2 = {{0,1},{0,0},{0,0},{0,1}};
		if (this->charInfoMap.find(ch) == this->charInfoMap.end())
			return;

		const CharInfoEx& charInfo = this->charInfoMap[ch];
		GVector<TBYTE> buffer(charInfo.size, 0);
		++codeline;
		bool beFtChg = false;
		if ( m_disChar.find(ch) != m_disChar.end() && this->m_defFont)
		{
			beFtChg = true;
			::SelectObject(this->memDC, this->m_defFont);
		}

		::GetGlyphOutlineW(this->memDC, ch, GGO_GRAY8_BITMAP, &gm, charInfo.size,
			&buffer.front(), &mat2);
		if ( beFtChg )
		{
			::SelectObject(this->memDC, this->font);
		}
		++codeline;
		// write to cache texture
		charRect.SetValue(x, y, x + (int)charInfo.width, y + (int)charInfo.height);
		TEXLOCKED_RECT lockedRect;
		HRESULT hr = this->cacheTex->Lock(0, &lockedRect, &charRect);
		++codeline;
		if (FAILED(hr))
		{
			GenErr(GraphicErr::GetErrTypeStr(Font_Err),"On hell! CGdiFont lock texture failed!");
			return;
		}
		++codeline;
		TBYTE* dstData = (TBYTE*)(lockedRect.pBits);
		TBYTE* srcData = &buffer.front();
		// DWORD-aligned, so minimal cell is 4 bytes
		int width = (charInfo.width / 4 + (charInfo.width % 4 == 0 ? 0 : 1)) * 4;
		int X = 0;
		int Y = 0;
		int write_size = (int)GetSize();
		for (Y = 0; Y < charInfo.height; ++Y)
		{
			for (X = 0; X < charInfo.width; ++X)
			{
				dstData[X*m_PixelSize + m_PixelSize - 1] = AlphaLevel[buffer[Y * width + X]];
			}

			for (; X<write_size; ++X)
			{
				dstData[X*m_PixelSize + m_PixelSize - 1] = 0;
			}

			dstData += lockedRect.Pitch;
		}
		++codeline;
		for ( ; Y < write_size; ++Y )
		{
			for ( X = 0; X <= write_size; ++X )
			{
				dstData[X*m_PixelSize + m_PixelSize - 1] = 0;
			}

			dstData += lockedRect.Pitch;
		}
		++codeline;
		this->cacheTex->Unlock(0);
		++codeline;
	}
	SQR_CATCH(exp){
		stringstream ss;
		ss << "--CacheCharacter error at code line: " << codeline
			<< "\n"
			<< "char rect left:" << charRect.left 
			<< " top:" << charRect.top
			<< " right:" << charRect.right 
			<< " bottom:" << charRect.bottom
			<< "\ntexture pointer:" << this->cacheTex;

		string errmsg = ss.str();
		exp.AppendMsg( errmsg.c_str() );
		GfkLogExp(exp);
	}
	SQR_TRY_END
	 
}

//------------------------------------------------------------------------------
const CFontBase::CharInfo*
CGdiFont::GetCharInfo( wchar_t ch, uint8 idx )
{
	int codestate = 0;
	SQR_TRY	
	{
		if (this->invalidChar.end() != this->invalidChar.find(ch))
			return NULL;
		codestate = 1;
		CharInfoMap::iterator it = this->charInfoMap.find(ch);
		codestate = 2;
		if (this->charInfoMap.end() == it)
		{
			// 处理该字符所在字符串
			PreProcessStr();
			
			int bufferSize;
			if ( ch > EMT_FONT_ID_BEGIN && ch < EMT_FONT_ID_END )
			{
				this->invalidChar.insert(ch);
				// get emt info

				if ( CTextureFontMgr::GetInst() )
				{
					float cellX, cellY;
					TextureFontInfo* EmtInfo = CTextureFontMgr::GetInst()->GetTexFontInfo(ch, idx);
					if (EmtInfo)
					{
						cellX = abs(EmtInfo->EmtImg.texRt.right - EmtInfo->EmtImg.texRt.left) * CTextureFontMgr::GetInst()->GetImgScale();
						cellY = abs(EmtInfo->EmtImg.texRt.bottom - EmtInfo->EmtImg.texRt.top) * CTextureFontMgr::GetInst()->GetImgScale();
						this->composeSizeMap.insert(make_pair(ch, CharComposeSize(cellX, cellY)));
					}
				}
				return NULL;
			}

			MAT2 mat2 = {{0,1},{0,0},{0,0},{0,1}};
			// 需要一个检测
			bool beFtChg = false;
			if ( m_disChar.find(ch) != m_disChar.end() && this->m_defFont)
			{
				beFtChg = true;
				::SelectObject(this->memDC, this->m_defFont);
			}
			codestate = 3;
			// get font glyph
			GLYPHMETRICS gm;
			bufferSize = ::GetGlyphOutlineW(this->memDC, ch, GGO_GRAY8_BITMAP,
				&gm, 0, NULL, &mat2);


			if ( beFtChg )
			{
				::SelectObject(this->memDC, this->font);
			}
			if (bufferSize <= 0)
			{
				this->invalidChar.insert(ch);
				return NULL;
			}
			codestate = 4;
			CharInfoEx newCharInfo;
			newCharInfo.left = int16(gm.gmptGlyphOrigin.x);
			newCharInfo.top = int16(this->m_ascender - gm.gmptGlyphOrigin.y);
			newCharInfo.width = gm.gmBlackBoxX;
			newCharInfo.height = gm.gmBlackBoxY;
			newCharInfo.size = bufferSize;
			pair<CharInfoMap::iterator, bool> ret = this->charInfoMap.insert(make_pair(ch, newCharInfo));
			this->composeSizeMap.insert(make_pair(ch, CharComposeSize(gm.gmCellIncX, gm.gmCellIncY)));
			return &ret.first->second;
		}
		else
		{
			return &it->second;
		}
	}
	SQR_CATCH(exp)
	{
		stringstream errmsg;
		errmsg	<< "     error drop line: " << codestate  << "\n";
		exp.AppendMsg(errmsg.str());
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;
	return NULL;
}

//------------------------------------------------------------------------------
void	
CGdiFont::PreProcessStr()
{
	// 处理字体中可能存在不存在的字体
	wstring text = m_curChar;
	if ( m_curChar && text != m_oldChar)
	{
		// dc, lpstr, c, pgi, fl
		int c = text.length();

		if (c > 0)
		{
			LPWORD pgi = new WORD[c];
			::GetGlyphIndicesW(this->memDC, text.c_str(), c, pgi, GGI_MARK_NONEXISTING_GLYPHS);

			// 判断这个字是否存在
			for (int i = 0; i < c; i++)
			{
				if ( pgi[i] == WORD(-1) )
				{
					// 分类，表情字体，生疏字体
					wchar_t ch = text[i];
					m_disChar.insert( ch );
				}
			}
			delete []pgi;
			m_oldChar = m_curChar;
		}
	}
}

//------------------------------------------------------------------------------
DWORD
CGdiFont::GetShaderID()
{
	return 0;//GdiShaderId;
}

float CGdiFont::GetLineSpacing() const
{
	return m_height;
}

float CGdiFont::GetFontHeight() const
{
	return m_height;
}

float CGdiFont::GetFontGlyphHeight(wchar_t c)	const
{
	TextureFontInfo* EmtInfo = CTextureFontMgr::GetInst()->GetTexFontInfo(c);
	if (EmtInfo)
	{
		return EmtInfo->EmtImg.texRt.bottom - EmtInfo->EmtImg.texRt.top;
	}
	else
	{
		return m_height; 
	}
}

}// namespace sqr
