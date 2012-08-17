#include "StdAfx.h"
#include "CFontBase.h"
#include "BaseHelper.h"
#include "CFontRenderer.h"
#include "CGraphic.h"
#include "TSqrAllocator.inl"
#include "CodeCvs.h"
#include "CGpuProgramMgr.h"

namespace sqr
{
	// ----------------------------------------------------------------------------------------
	// 局部变量与函数

	const int32 CFontBase::st_CacheTextureSize = 512;
	const int32 CachtTexRightBound	= CFontBase::st_CacheTextureSize - 10; // 用于解决文字UV超出范围的问题
	const float inv_tex_size	 = 1.0f/CFontBase::st_CacheTextureSize; 

	bool is_space(wchar_t ch)
	{
		return (' ' == ch || '　' == ch || 12288 == ch );
	}

	void RectangleClip( CFRect& rect, CFRect& uvRect, const CFRect& clipRect )
	{
		if (floor(rect.top) < clipRect.top)
		{
			uvRect.top += (clipRect.top - rect.top) / rect.Height() * uvRect.Height();
			rect.top = clipRect.top;
		}
		if (ceil(rect.bottom) > clipRect.bottom)
		{
			uvRect.bottom -= (rect.bottom - clipRect.bottom) / rect.Height() * uvRect.Height();
			rect.bottom = clipRect.bottom;
		}
		if (floor(rect.left) < clipRect.left)
		{
			uvRect.left += (clipRect.left - rect.left) / rect.Width() * uvRect.Width();
			rect.left = clipRect.left;
		}
		if (ceil(rect.right) > clipRect.right)
		{
			uvRect.right -= (rect.right - clipRect.right) / rect.Width() * uvRect.Width();
			rect.right = clipRect.right;
		}
	}

	void RectangleVerticalClip( CFRect& rect, CFRect& uvRect, const CFRect& clipRect )
	{
		if (floor(rect.top) < clipRect.top)
		{
			uvRect.left += (clipRect.top - rect.top) / rect.Height() * uvRect.Width();
			rect.top = clipRect.top;
		}
		if (ceil(rect.bottom) > clipRect.bottom)
		{
			uvRect.right -= (rect.bottom - clipRect.bottom) / rect.Height() * uvRect.Width();
			rect.bottom = clipRect.bottom;
		}
		if (floor(rect.left) < clipRect.left)
		{
			uvRect.top += (clipRect.left - rect.left) / rect.Width() * uvRect.Height();
			rect.left = clipRect.left;
		}
		if (ceil(rect.right) > clipRect.right)
		{
			uvRect.bottom -= (rect.right - clipRect.right) / rect.Width() * uvRect.Height();
			rect.right = clipRect.right;
		}
	}

	void RectangleTexClip( CFRect& rect, CFRect& texRect, const CFRect& clipRect, const float& scale )
	{
		if (floor(rect.top) < clipRect.top)
		{
			texRect.top += ( clipRect.top - rect.top ) / scale;
			rect.top = clipRect.top;
		}
		if (ceil(rect.bottom) > clipRect.bottom)
		{
			texRect.bottom -= ( rect.bottom - clipRect.bottom ) / scale;
			rect.bottom = clipRect.bottom;
		}
		if (floor(rect.left) < clipRect.left)
		{
			texRect.left += ( clipRect.left - rect.left ) / scale;
			rect.left = clipRect.left;
		}
		if (ceil(rect.right) > clipRect.right)
		{
			texRect.right -= ( rect.right - clipRect.right ) / scale;
			rect.right = clipRect.right;
		}
	}

	// -----------------------------------------------------------------------------
	// 成员函数定义

	CFontBase::CFontBase()
			: m_font_size(0.0f)
			, isBold(false)
			, isScalable(false)
			, curX(0)
			, curY(0)
			, isBegin(false)
			, cacheTex(NULL)
			, fontRenderer(NULL)
			, m_bAlignToPixel(true)
			, m_PixelSize(1)
			, m_curChar(NULL)
			, m_beFrameFirst(true)
			, m_FirstCharItr(NULL)
	{
		m_disChar.clear();
	}

	CFontBase::~CFontBase()
	{
		this->OnDestroy();
	}

	bool CFontBase::Create( int32 id, bool bold, float fSize )
	{
		this->id		= id;
		this->isBold	= bold;
		m_font_size		= fSize;

		return this->OnCreate();
	}

	void CFontBase::Resize( float n )
	{
		if (this->GetSize() == n)
			return;

		m_font_size = n;

		this->OnResize();
	}

	//------------------------------------------------------------------------------
	bool CFontBase::OnCreate()
	{
		this->fontRenderer = CFontRenderer::GetInst();
		return true;
	}

	void CFontBase::OnResize()
	{
	}

	void CFontBase::OnDestroy()
	{
		this->fontRenderer = NULL;
		SafeRelease(this->cacheTex);
		this->charRectMap.clear();
		this->charLRU.clear();
		m_beFrameFirst = true;
		m_lruFindMap.clear();
		this->composeSizeMap.clear();
		this->curX = 0;
		this->curY = 0;
		this->m_disChar.clear();
		this->m_curChar = NULL;
	}

	void CFontBase::UpdateCacheTexture( const wchar_t& ch, const float size, uint8 idx )
	{
		const CharInfo* charInfo = this->GetCharInfo(ch, idx);
		if (NULL == charInfo)
		{
			if ( CTextureFontMgr::GetInst() )
			{
				TextureFontInfo* EmtInfo = CTextureFontMgr::GetInst()->GetTexFontInfo(ch, idx);
				if ( EmtInfo )
				{
					if (size == -1.0f)
						return;
					else
					{
						float scale = this->IsScalable() ? (size / this->GetSize()) : 1;
						scale *= CTextureFontMgr::Inst()->GetImgScale();
						m_formatHeight = (abs(EmtInfo->EmtImg.texRt.bottom - EmtInfo->EmtImg.texRt.top) * scale - GetLineSpacing())
										 > m_formatHeight ? (abs(EmtInfo->EmtImg.texRt.bottom - EmtInfo->EmtImg.texRt.top) * scale - GetLineSpacing()) : m_formatHeight;
					}
				}
			}
			return;
		}

		RectMap::iterator charRectIt = this->charRectMap.find(ch);
		if (this->charRectMap.end() != charRectIt)
		{
			// already in the cache texture, so update the LRU queue
			CFontListNode* itNode = m_lruFindMap[ch];
			if (this->charLRU.head() != itNode)
			{
				// move to the first
				if (!m_beFrameFirst)
				{
					// check if the ch is in first frame char
					if (ch != m_FirstCharItr->m_Node)
						this->charLRU.movetohead(itNode);
				}
				else
					this->charLRU.movetohead(itNode);
			}

			if (m_beFrameFirst)
			{
				m_FirstCharItr = this->charLRU.head();
				m_beFrameFirst = false;
			}
		}
		else
		{
			// not exist in the cache texture
			CFRect newCharRect;

			int nCharWidth = static_cast<int>( GetSize() + 1 );
			if ( nCharWidth < charInfo->width )
				nCharWidth = charInfo->width + 1;

			if (this->curX + nCharWidth > CachtTexRightBound)
			{
				// move to the next line
				this->curX = 0;
				this->curY += (int)this->GetSize() + 2;
			}

			if (this->curY + this->GetSize() + 2 < st_CacheTextureSize )
			{
				// there is still some space left
				newCharRect.left	= float(this->curX+0.5f) * inv_tex_size;
				newCharRect.top		= float(this->curY+0.5f) * inv_tex_size;
				newCharRect.right	= float(this->curX+0.5f + charInfo->width) * inv_tex_size;
				newCharRect.bottom	= float(this->curY+0.5f + charInfo->height) * inv_tex_size;
				this->curX += nCharWidth + 1;
			}
			else
			{
				// the cache texture is full, so replace the latest unused one
				CFontListNode*	lru_it = this->charLRU.tail();
				RectMap::iterator it = this->charRectMap.find(lru_it->m_Node);
				wchar_t wc = lru_it->m_Node;
				Ast(this->charRectMap.end() != it);
				newCharRect.left = it->second.left;
				newCharRect.top = it->second.top;
				newCharRect.right = it->second.left + float(charInfo->width) * inv_tex_size;
				newCharRect.bottom = it->second.top + float(charInfo->height) * inv_tex_size;
				// remove the latest unused character
				m_lruFindMap.erase(lru_it->m_Node);
				this->charLRU.pop_back();
				this->charRectMap.erase(it);

				// check if the texture if full in on frame
				if (!m_beFrameFirst)
				{
					// check if the ch is in first frame char
					if (wc == m_FirstCharItr->m_Node)		// full
					{
						// render the text that have push
						FlushOneFrame();
						// reset new data
						m_beFrameFirst = true;
					}
				}
			}

			if ( newCharRect.right > 1.0f || newCharRect.bottom > 1.0f )
			{
				stringstream ss;
				wstring szErrChar;
				szErrChar.append(1, ch);
				ss << "width:" << newCharRect.right << " height:" << newCharRect.bottom << " char:" << UCS2ToUtf8(szErrChar);
				string err_msg = ss.str();
				GfkLogErr("字体UV超出范围", err_msg.c_str());

				if ( newCharRect.right > 1.0f )
					newCharRect.right = 1.0f;

				if ( newCharRect.bottom > 1.0f )
					newCharRect.bottom = 1.0F;
			}

			// write the character's pixel data into the cache texture
			this->CacheCharacter(ch, int(newCharRect.left * CFontBase::st_CacheTextureSize),
				int(newCharRect.top * CFontBase::st_CacheTextureSize));
			// add character to charRectMap and LRU queue
			this->charRectMap.insert(make_pair(ch, newCharRect));
			this->charLRU.push_front(ch);
			m_lruFindMap.insert(make_pair(ch, this->charLRU.head()));
			
			if (m_beFrameFirst)
			{
				m_FirstCharItr = this->charLRU.head();
				m_beFrameFirst = false;
			}
		}
	}

	void CFontBase::UpdateCache( const wstring& text, const float size, uint8 idx )
	{
		// cache the text
		m_curChar = text.c_str();

		for (index_t i = 0; i < text.length(); ++i)
		{
			wchar_t ch = text[i];
			this->GetCharInfo(ch, idx);
		}
	}

	float CFontBase::GetScale(float size)
	{
		if (this->GetSize() == 0.0f)
			return 1;
		return this->IsScalable() ? (size / this->GetSize()) : 1;
	}

	/**
	\brief
		描边与阴影效果不能共存
	*/
	void CFontBase::DrawText( const RenderTextParam& param )
	{
		Ast(this->isBegin);
		
		fontRenderer->SetAlignToPixel(m_bAlignToPixel);
		m_formatHeight = 0.0f;
		this->UpdateCache(param.GetText(), param.GetSize(), param.GetIdx()); //这行太消耗性能了
	
		if (param.IsOutline() && CGpuProgramMgr::GetInst()->GetShaderVer() != Shader_V_0 )
		{
			RenderTextParam newParam = param;
			newParam.SetColor(param.GetBackColor());
			newParam.SetGradualColor(param.GetBackColor());
			const CVector3f& pos = param.GetPosition();
			// left, right, top, bottom
			newParam.SetPosition(CVector3f(pos.x-1.0f, pos.y, pos.z));
			this->AddText(newParam);
			newParam.SetPosition(CVector3f(pos.x+1.0f, pos.y, pos.z));
			this->AddText(newParam);
			newParam.SetPosition(CVector3f(pos.x, pos.y-1.0f, pos.z));
			this->AddText(newParam);
			newParam.SetPosition(CVector3f(pos.x, pos.y+1.0f, pos.z));
			this->AddText(newParam);
		}
		else if (param.IsShadow() && CGpuProgramMgr::GetInst()->GetShaderVer() != Shader_V_0)
		{
			RenderTextParam newParam = param;
			newParam.SetColor(param.GetBackColor());
			newParam.SetGradualColor(param.GetBackColor());
			const CVector3f& pos = param.GetPosition();
			// right-bottom
			newParam.SetPosition(CVector3f(pos.x+1.0f, pos.y+1.0f, pos.z));
			this->AddText(newParam);
		}

		this->AddText(param);
	}

	size_t CFontBase::GetCharAtPixel( const wstring &text, size_t start_char, float pixel ) const
	{
		float  cur_extent = 0;
		size_t char_count = text.length();

		if ( pixel <=0 || (char_count <= start_char) )
			return start_char;

		for ( size_t c = start_char; c<char_count; ++c )
		{
			if ( is_space(text[c]) )
			{
				cur_extent += GetSize() * 0.5f;
				if ( cur_extent > pixel )
					return c;
				continue;
			}

			ComposeSizeMap::const_iterator it = this->composeSizeMap.find(text[c]);
			if ( it != this->composeSizeMap.end() )
			{
				cur_extent += it->second.x;
				if ( cur_extent > pixel )
					return c;
			}
		}

		return char_count;
	}

	CVector2f CFontBase::CalculateSize( const wstring& text )
	{
		// compute text rect area
		GVector<float> lines(1, 0.0f);
		for (index_t i = 0; i < text.length(); ++i)
		{
			wchar_t ch = text[i];
			if ('\n' == ch)
			{
				// new line
				lines.push_back(0.0f);
			}
			else
			{
				float charWidth = iswascii(ch) ? this->GetSize()*0.5f : this->GetSize();
				if (is_space(ch))
				{
					lines.back() += charWidth;
					continue;
				}
				// add a character's width
				// here need a pre load, or the width will be dirty.
				ComposeSizeMap::iterator it = this->composeSizeMap.find(ch);
				if (this->composeSizeMap.end() != it)
				{
					lines.back() += it->second.x;
				}
				else
				{
					UpdateCache( text );
					it = this->composeSizeMap.find(ch);
					if ( this->composeSizeMap.end() != it )
					{
						lines.back() += it->second.x;
					}
				}
			}
		}

		return CVector2f(*std::max_element(lines.begin(), lines.end()),
						 lines.size() * GetLineSpacing());
	}
	
	void CFontBase::AddText( const RenderTextParam& param )
	{
		int codeline = 0;
		SQR_TRY{
			Ast(!wcslen(param.GetText())==0);
			const wstring& text = param.GetText();
			const CFRect& clipRect = param.GetRect();
			const CVector3f& pos = param.GetPosition();
			const uint8& idx = param.GetIdx();

			float scale = this->IsScalable() ? (param.GetSize() / this->GetSize()) : 1;
			float height = this->GetLineSpacing() * scale;
			float x = clipRect.left + pos.x;
			float y = clipRect.top + pos.y;
			++codeline;
			if (param.IsVertical())
			{
				FormatVerticalTex(text, clipRect , pos , scale, height, x, y, param );
				return;
			}
			++codeline;
			for (index_t i = 0; i < text.length(); ++i)
			{
				wchar_t ch = text[i];
				if ('\n' != ch && x < clipRect.right)
				{
					float charWidth = iswascii(ch) ? this->GetSize() * 0.5f : this->GetSize();
					const CharInfo* charInfo = NULL;
					// white space or invalid character
					if (is_space(ch) || NULL == (charInfo = this->GetCharInfo(ch, idx)))
					{
						if ( CTextureFontMgr::GetInst() )
						{
							CheckAndAddTex(ch, idx, x, y, scale, clipRect, charWidth, param );
						}
						continue;
					}
					// fill vertex buffer
					CFRect rect;
					rect.left = x + charInfo->left * scale;
					rect.top = y + charInfo->top * scale + m_formatHeight;
					rect.right = rect.left + charInfo->width * scale;
					rect.bottom = rect.top + charInfo->height * scale;
					if (rect.bottom > clipRect.top && rect.top < clipRect.bottom
						&& rect.left < clipRect.right && rect.right > clipRect.left)
					{
						UpdateCacheTexture( ch, param.GetSize(), param.GetIdx() );
						RectMap::iterator charRectIt = this->charRectMap.find(ch);
						if (this->charRectMap.end() == charRectIt)
						{
							// move to next character's position
							x += this->composeSizeMap.find(ch)->second.x * scale;
							continue;
						}

						CFRect uvRect = charRectIt->second;
						// rectangle clip
						RectangleClip(rect, uvRect, clipRect);

						float delta = (param.IsItalic()) ? (this->GetSize() * 0.4f) : 0;
						// add to renderer's queue
						this->fontRenderer->AddChar(rect, uvRect, pos.z,
							param.GetColor(), param.GetGradualColor(), delta);
					}
					// move to next character's position
					x += this->composeSizeMap.find(ch)->second.x * scale;
				}
				else
				{
					// move to next line
					if ( i == 0 )
						break;	// 防止死循环
					x = clipRect.left + pos.x;
					y += height;
					if ('\n' != ch)
						i--;
					// out of clip rect
					if ((!param.IsMultiline()) || y > clipRect.bottom)
						break;
				}
			}
		}
		SQR_CATCH(exp){
			stringstream ss;
			ss << "--addtext error at code line: " << codeline;
			string errmsg = ss.str();
			exp.AppendMsg( errmsg.c_str() );
			GfkLogExp(exp);
		}
		SQR_TRY_END
	}

	void CFontBase::FormatVerticalTex(const wstring& text, const CFRect& clipRect
									, const CVector3f& pos , float& scale, float& height
									, float& x, float& y, const RenderTextParam& param )
	{
		// 不支持贴图字
		for (index_t i = 0; i < text.length(); ++i)
		{
			wchar_t ch = text[i];
			if ('\n' != ch && y < clipRect.bottom)
			{
				float charWidth = iswascii(ch) ? this->GetSize() * 0.5f : this->GetSize();
				const CharInfo* charInfo = NULL;

				if (is_space(ch) || NULL == (charInfo = this->GetCharInfo(ch, param.GetIdx())))
					y += charWidth;
				else if (ch >= 0 && ch < 128)
				{
					// 当作英文来处理
					CFRect rect;
					rect.left = x;
					rect.top = y + charInfo->left * scale;
					rect.right = rect.left + charInfo->height * scale;
					rect.bottom = rect.top + charInfo->width * scale;

					if (rect.bottom > clipRect.top && rect.top < clipRect.bottom
						&& rect.left < clipRect.right && rect.right > clipRect.left)
					{
						UpdateCacheTexture( ch, param.GetSize(), param.GetIdx() );
						RectMap::iterator charRectIt = this->charRectMap.find(ch);
						if (this->charRectMap.end() == charRectIt)
						{
							// move to next character's position
							y += this->composeSizeMap.find(ch)->second.x * scale;
							continue;
						}

						CFRect uvRect = charRectIt->second;
						// rectangle clip
						RectangleVerticalClip(rect, uvRect, clipRect);

						float delta = (param.IsItalic()) ? (this->GetSize() * 0.4f) : 0;
						// add to renderer's queue
						this->fontRenderer->AddVerticalChar(rect, uvRect, pos.z,
							param.GetColor(), param.GetGradualColor(), delta);
					}
					// move to next character's position
					y += this->composeSizeMap.find(ch)->second.x * scale;
				}
				else
				{
					CFRect rect;
					rect.left = x/*+ charInfo->left * scale*/;
					rect.top = y + charInfo->top * scale + m_formatHeight;
					rect.right = rect.left + charInfo->width * scale;
					rect.bottom = rect.top + charInfo->height * scale;
					if (rect.bottom > clipRect.top && rect.top < clipRect.bottom
						&& rect.left < clipRect.right && rect.right > clipRect.left)
					{
						UpdateCacheTexture( ch, param.GetSize(), param.GetIdx() );
						RectMap::iterator charRectIt = this->charRectMap.find(ch);
						if (this->charRectMap.end() == charRectIt)
						{
							// move to next character's position
							y += this->composeSizeMap.find(ch)->second.x * scale;
							continue;
						}

						CFRect uvRect = charRectIt->second;
						// rectangle clip
						RectangleClip(rect, uvRect, clipRect);

						float delta = (param.IsItalic()) ? (this->GetSize() * 0.4f) : 0;
						// add to renderer's queue
						this->fontRenderer->AddChar(rect, uvRect, pos.z,
							param.GetColor(), param.GetGradualColor(), delta);
					}
					// move to next character's position
					y += height;
				}
			}
			else
				return;

		}
		return;
	}

	bool CFontBase::CheckAndAddTex(wchar_t ch, uint8 idx, float& x, float& y, float& scale, const CFRect& clipRect, float& charWidth, const RenderTextParam& param )
	{
		TextureFontInfo* EmtInfo = CTextureFontMgr::GetInst()->GetTexFontInfo(ch, idx);
		TextureFontInfoII* TexInfo = dynamic_cast<TextureFontInfoII*>(EmtInfo);
		if (EmtInfo)
		{
			CFRect rect;
			float cellX, cellY;
			float imgscale = CTextureFontMgr::GetInst()->GetImgScale();

			rect.left = x;
			cellX = abs(EmtInfo->EmtImg.texRt.right - EmtInfo->EmtImg.texRt.left) * imgscale;
			cellY = abs(EmtInfo->EmtImg.texRt.bottom - EmtInfo->EmtImg.texRt.top) * imgscale;
			// 计算校正值, 如果图片比文字高，则按照正常的校正方式，使图片基底显示
			// 如果图片比文字矮，则图片的显示方式为布局到文字的正中央
			if ( cellY * scale - GetLineSpacing() > 0.0f )
				rect.top = y + ( m_formatHeight - ( cellY * scale - GetLineSpacing() ) );
			else
				rect.top = y + ( m_formatHeight - ( cellY * scale - GetLineSpacing() ) * 0.5f );
			rect.right = rect.left + cellX * scale;
			rect.bottom = rect.top + cellY * scale;

			if (rect.bottom > clipRect.top && rect.top < clipRect.bottom
				&& rect.left < clipRect.right && rect.right > clipRect.left)
			{
				// add to renderer's queue
				TextureFontDraw* p_emtD = new TextureFontDraw;
				p_emtD->pEmtImg = &CTextureFontMgr::GetInst()->GetTexFontInfo(ch, idx)->EmtImg;

				CFRect texRect = p_emtD->pEmtImg->texRt;
				// rectangle clip
				RectangleTexClip(rect, texRect, clipRect, scale * imgscale);
				p_emtD->posRect = rect;
				p_emtD->texRect = texRect;
				p_emtD->pEmtImg->dwColor = param.GetColor();
				CTextureFontMgr::GetInst()->m_EmtDList.push_back(p_emtD);
			}
			// move to next character's position
			x += this->composeSizeMap.find(ch)->second.x * scale;
			return true;
		}
		else
		{
			x += charWidth * scale;
		}
		return false;
	}

	void CFontBase::Begin()
	{
		this->fontRenderer->Begin(this->cacheTex, this->GetShaderID());
		this->isBegin = true;
	}

	void CFontBase::End()
	{
		this->fontRenderer->End();
		this->isBegin = false;
	}

	void CFontBase::FlushOneFrame()
	{
		this->fontRenderer->FlushOneFrame();
	}

	void CFontBase::UpdateCharInfo( const wstring& text )
	{
		// cache the text
		m_curChar = text.c_str();

		for (index_t i = 0; i < text.length(); ++i)
			const CharInfo* charInfo = this->GetCharInfo(text[i], 0);
	}

}
