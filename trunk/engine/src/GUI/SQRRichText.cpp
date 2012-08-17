#include "stdafx.h"
#include "SQRRichText.h"
#include "BaseHelper.h"
#include "CodeCvs.h"
#include "SQREdit.h"
#include "SQRGUIManager.h"
#include "StringHelper.h"
#include "ErrLogHelper.h"
#include "CodeCvs.h"
#include "CColor.inl"
#include "CGraphic.h"

#include "TSqrAllocator.inl"

namespace sqr
{
	enum EAtomType
	{
		EAtom_None,
		EAtom_CIcon,
		EAtom_CText,
	};

	enum EAtomCTextTpye
	{
		EAtom_CText_None,
		EAtom_CText_Let,		// such as: a,b
		EAtom_CText_Num,		// such as: 1,2
		EAtom_CText_Pun,		// such as: , . !
		EAtom_CText_Cha,		// such as: 蓝港
	};

	struct CAtom
	{
	public:
		CFPos	m_Pos;
		float	m_fWidth;
		float	m_fHeight;
		int32	m_nClickID;
		float   m_fCurLineLength;
		float   m_fLayoutHeight;
	protected:
		EAtomType m_eAtomType;
	public:
		CAtom() 
			: m_fWidth(0)
			, m_fHeight(0)
			, m_nClickID(-1)
			, m_eAtomType(EAtom_None)
			, m_fLayoutHeight(0)
			, m_fCurLineLength(0)
		{
		}
		virtual ~CAtom(){}
		virtual float GetWidth()	{ return m_fWidth; };
		virtual float GetHeight()	{ return m_fHeight; };

		EAtomType GetAtomType()const{return m_eAtomType;}
	};

	struct CIcon : public CAtom
	{
		const UIMap<int32,WND_IMAGE_LIST>&	m_ImageList;
		int32								m_nImageIndex;
		uint32								m_uStartTime;
		

		const CIcon& operator= ( const CIcon& ){ return *this; }

		CIcon( const UIMap<int32,WND_IMAGE_LIST>& ImageList, int32 nImageIndex )
			: m_ImageList( ImageList )
			, m_nImageIndex( nImageIndex )
		{
			m_eAtomType = EAtom_CIcon;
			m_uStartTime	= uint32(GetProcessTime());
			UIMap<int32,WND_IMAGE_LIST>::const_iterator it = m_ImageList.find( nImageIndex );
			if( it == m_ImageList.end() )
			{
				m_nImageIndex = -1;
				return;
			}

			for( int32 i = 0; i < it->second.GetImageCount(); i++ )
			{
				m_fWidth = max( it->second.GetImage(i).rtTexture.Width() + 1, m_fWidth );
				m_fHeight = max( it->second.GetImage(i).rtTexture.Height() + 1, m_fHeight );
			}
		}

		void Draw(SQRWnd* pWndDraw, float x, float y, bool highLight)
		{
			if( m_nImageIndex < 0 )
				return;

			x += m_Pos.x;
			y += m_Pos.y;
			UIMap<int32,WND_IMAGE_LIST>::const_iterator it = m_ImageList.find( m_nImageIndex );
			int32 nCount = it->second.GetImageCount();
			if( nCount )
			{
				const IMAGE& Image = it->second.GetImage( ( ( uint32(GetProcessTime()) - m_uStartTime )/100 )%nCount );
				float Zoom		= pWndDraw->GetRootZoomSize();
				float dx		= GetXBorder( pWndDraw->GetStyle() ) * Zoom;
				float dy		= GetYBorder( pWndDraw->GetStyle() ) * Zoom;
				uint32 wndStyle = pWndDraw->GetStyle() ;
				if(wndStyle & WS_TEXTXC  || wndStyle & WS_TEXTXR || 
					wndStyle & WS_TEXTYC || wndStyle & WS_TEXTYB)
				{
					float Width = m_fCurLineLength + 2*dx;
					float Hight = m_fLayoutHeight + 2*dy;
					
					if( wndStyle & WS_TEXTXC )
						x += pWndDraw->GetWndHandle()->m_Width/2 - Width/2;
					else if( wndStyle & WS_TEXTXR )
						x += pWndDraw->GetWndHandle()->m_Width - Width ;
					if( wndStyle & WS_TEXTYC )
						y += pWndDraw->GetWndHandle()->m_Height/2 - Hight/2;
					else if( wndStyle & WS_TEXTYB )
						y += pWndDraw->GetWndHandle()->m_Height - Hight;
				}
				
				CFRect rtWnd( x, y, (x + Image.rtTexture.Width()), (y + Image.rtTexture.Height()));
				pWndDraw->DrawRect( Image.pTexture, rtWnd, Image.dwColor, &Image.rtTexture, false );
			}
		}
		UIWString GetText()const
		{
			return L"";
		}
	};


	struct CText : public CAtom
	{
		UIWString	m_sText;
		uint32	m_uColor;		// 正常颜色
		uint32	m_uClkDColor;	// click down color
		uint32	m_uMSOVColor;	// mouse over color
		EventStateMask m_eCurState;
		uint32	m_uBKColor;	
		int32	m_nBlink;	//-1一直闪烁　０不闪烁　＞０闪烁秒数
		uint32	m_uBlinkColor;
		uint32	m_uBlinkTime;
		uint32	m_uStartTime;
		bool	m_bUnderline;
		bool    m_bHyperlink;
		UIWString	m_sAttr;
		UIWString	m_sUrl;		//超链接 
		EAtomCTextTpye	m_eCTextType;		// 该文本段的类型
		uint32	m_uGroup;

		CText()
			: m_uColor(CColor::Black)
			, m_uClkDColor(CColor::Black)
			, m_uMSOVColor(CColor::Black)
			, m_eCurState(IP_ENABLE)
			, m_uBKColor(CColor::White)
			, m_nBlink(0)
			, m_uBlinkColor(0x0)
			, m_uBlinkTime(1000)
			, m_uStartTime(0)
			, m_bUnderline(false)
			, m_bHyperlink(false)
			, m_uGroup(0)
		{
			m_eAtomType = EAtom_CText;
			m_eCTextType = EAtom_CText_Cha;
		}
		~CText()
		{
			
		}

		void CTextFullCopy(CText* Dest)
		{
			Dest->m_fWidth		= m_fWidth;
			Dest->m_fHeight		= m_fHeight;
			Dest->m_nClickID	= m_nClickID;
			Dest->m_eAtomType	= m_eAtomType;

			Dest->m_uColor		= m_uColor;
			Dest->m_uClkDColor	= m_uClkDColor;
			Dest->m_uMSOVColor	= m_uMSOVColor;
			Dest->m_eCurState	= m_eCurState;
			Dest->m_uBKColor	= m_uBKColor;	
			Dest->m_nBlink		= m_nBlink ;	//-1一直闪烁　０不闪烁　＞０闪烁秒数
			Dest->m_uBlinkColor = m_uBlinkColor;
			Dest->m_uBlinkTime	= m_uBlinkTime;
			Dest->m_uStartTime	= m_uStartTime;
			Dest->m_bUnderline	= m_bUnderline;
			Dest->m_bHyperlink	= m_bHyperlink;
			Dest->m_eCTextType	= m_eCTextType;	
			Dest->m_sText		= m_sText;
			Dest->m_sAttr		= m_sAttr;
			Dest->m_sUrl		= m_sUrl;
			Dest->m_uGroup		= m_uGroup;
		}


		EAtomCTextTpye GetAtomCTextType() const
		{
			return m_eCTextType;
		}

		void AddChar( SQRWnd *pWnd, int32 c, float fCharWidth )
		{
			m_sText.insert( m_sText.size(), (const wchar_t*)&c, 1 );
			m_fWidth	   += fCharWidth;
			m_uStartTime	= uint32(GetProcessTime());
		}

		int32 RemoveCharEnd( SQRWnd *pWnd )
		{
			int32 c = m_sText[m_sText.length() - 1];
			m_sText = m_sText.substr( 0, m_sText.length() - 1 );
			float fCharWidth	 = pWnd->GetFont()->CalculateSize((const wchar_t*)&c).x;
			m_fWidth		-= fCharWidth;
			return c;
		}

		void Draw(SQRWnd *pWndDraw, float x, float y, bool highLight, int strBegin = 0, int strEnd = UIString::npos)
		{
			limit(strBegin, 0, static_cast<int>(m_sText.size()));
			if(strEnd < 0)
				strEnd = m_sText.size();
			else
				limit(strEnd, 0, static_cast<int>(m_sText.size()));
			const wstring utf16_str = m_sText.substr(strBegin, strEnd).c_str();
			if(utf16_str.empty())
				return;

			x += m_Pos.x;
			y += m_Pos.y;
			CColor CurColor = m_uColor;
			CFPos pt = pWndDraw->GetCursorPos();
			if (pWndDraw->IsInFrameArea(pt.x, pt.y))
			{
				if (m_eCurState == IP_MOUSEOVER)
					CurColor = m_uMSOVColor;
				else if (m_eCurState == IP_CLICKDOWN)
					CurColor = m_uClkDColor;
			}

			// 高亮背景绘制并对前景色反色
			if(highLight)
			{
				float rect_width = pWndDraw->GetFont()->CalculateSize(utf16_str.c_str()).x;
				CFRect rt(x, y, x +	rect_width, y + m_fHeight);
				pWndDraw->DrawRect( NULL, rt, CurColor, NULL, true );
				CurColor = CurColor.Reverse();
			}
			// 只有在非高亮显示时才启用闪烁功能
			else if(m_nBlink)
			{
				uint32 disTime = (uint32)(GetProcessTime() - m_uStartTime);
				if ( m_uBlinkTime <= 0 )
					m_uBlinkTime = 1000;
				float density = ( disTime % m_uBlinkTime * 1.0f - m_uBlinkTime / 2.0f ) / ( m_uBlinkTime / 2.0f );
				CColor des = m_uBlinkColor;
				CurColor = CurColor * density + des * (1 - density);
				// 透明度的计算
				/*density = abs(density - 0.5f) * 2;
				limit(density, 0.0f, 1.0f);
				CurColor.A = (uint8)(CurColor.A  * density);*/
				// 处理闪烁次数
				if ( m_nBlink > 0 )
				{
					int32 ct = (int32)(disTime / m_uBlinkTime);
					if (ct > m_nBlink)
						m_nBlink = 0;		// 不闪烁
				}
			}

			// 文字的绘制
			SQRRichText* pRichText = dynamic_cast<SQRRichText*>(pWndDraw);
			if (pRichText)
			{
				pRichText->DrawText( utf16_str, x, y, CurColor, m_uBKColor, false, NULL ,m_fCurLineLength);
			}
			else
			{
				pWndDraw->DrawText( utf16_str, x, y, CurColor, m_uBKColor, false, NULL );
			}
			
			// 下划线的绘制
			if( m_bUnderline )
			{
				if(pWndDraw->GetStyle() & WS_TEXTXC  || pWndDraw->GetStyle() & WS_TEXTXR || pWndDraw->GetStyle() & WS_TEXTYC || pWndDraw->GetStyle() & WS_TEXTYB)
				{
					float Zoom		= pWndDraw->GetRootZoomSize();
					float dx		= GetXBorder( pWndDraw->GetStyle() ) * Zoom;
					float dy		= GetYBorder( pWndDraw->GetStyle() ) * Zoom;
					float Width		= m_fCurLineLength + 2*dx;
					float Height	= m_fLayoutHeight + 2*dy;

					if( pWndDraw->GetStyle() & WS_TEXTXC )
						x += pWndDraw->GetWndHandle()->m_Width/2 - Width/2; 
					else if(pWndDraw->GetStyle() & WS_TEXTXR )
						x += pWndDraw->GetWndHandle()->m_Width - Width;
					if( pWndDraw->GetStyle() & WS_TEXTYC )
						y += pWndDraw->GetWndHandle()->m_Height/2 -	Height/2;
					else if(pWndDraw->GetStyle() & WS_TEXTYB )
						y += pWndDraw->GetWndHandle()->m_Height - Height;
				}
				float Zoom = pWndDraw->GetRootZoomSize();
				float dx = GetXBorder( pWndDraw->GetStyle() ) * Zoom;
				float dy = GetYBorder( pWndDraw->GetStyle() ) * Zoom;
				float underline_len = pWndDraw->GetFont()->CalculateSize(utf16_str.c_str()).x;
				CFRect rt(x, y+m_fHeight, x + underline_len, y+m_fHeight+1);
				//pWndDraw->DrawRect( NULL, rt.Offset(dx,dy), CurColor, NULL, true );
				rt.top = rt.top - 0.5f * Zoom;
				pWndDraw->DrawRect( NULL, rt.Offset(dx,dy), CurColor, NULL, true );
			}
		}
	};

	struct RICH_TEXT
	{
	public:
		UIMap<int32,WND_IMAGE_LIST>	m_ImageList;
		UIVector<CAtom*>			m_Layout;
		UIVector<CText*>			m_KRText;
		float						m_fLineHeight;
		float						m_fCurWidth;
		float						m_fCurHeight;
		float						m_fWidth;
		float						m_fMaxWidth;
		float						m_fRowDist;
		UIVector<CText*>			m_UrlList;
		UIVector<CAtom*>			m_CurLineList;	// 当前行上所有Text atom节点的列表
		
		CText*						m_pCurText;
		uint32						m_uColor;
		uint32						m_uClkDColor;	// click down color
		uint32						m_uMSOVColor;	// mouse over color
		EventStateMask				m_eCurState;
		uint32						m_uBKColor;
		int32						m_nBlink;			// -1一直闪烁　０不闪烁　＞０闪烁秒数
		uint32						m_uBlinkColor;		// 闪烁目的颜色
		uint32						m_uBlinkTime;		// 单程时间 豪秒
		bool						m_bUnderline;
		bool						m_bHyperlink;
		bool						m_bAutoPlay;
		bool						m_bChanged;
		uint32						m_uStyle;
		uint32						m_uTimeLast;
		int32						m_nClickID;
		bool						m_bTextXR;
		bool						m_bAtomy;
		bool						m_beAutoFormat;
		bool						m_bWndTextXR;	

		CPos						m_highLightBegin;	// 高亮选择的开始。x为CAtom的下标，y为CText内的文字偏移（包括该文字）
		CPos						m_highLightEnd;		// 高亮选择的结束
		
		SQRRichText*				m_pCRichText;
		CText*						m_pTempText;

	public:
		RICH_TEXT()
			: m_fRowDist(2)
			, m_pCRichText(NULL)
			, m_fMaxWidth(0)
			, m_beAutoFormat(true)
			, m_bWndTextXR(false)
		{
			m_pTempText = new CText;
		}
		RICH_TEXT(SQRRichText *pCRichText)
			: m_fRowDist(2)
			, m_pCRichText(pCRichText)
			, m_beAutoFormat(true)
			, m_bWndTextXR(false)
		{
			m_pTempText = new CText;
		}
		~RICH_TEXT()
		{ 
			delete m_pTempText;
			m_pTempText = NULL;
			Free();
		}
		void Init()
		{
			Free();
			m_fCurHeight	= 0;
			m_fCurWidth		= 0;
			m_uColor		= 0xffffffff;
			m_uClkDColor	= m_uColor;	// click down color
			m_uMSOVColor	= m_uColor;	// mouse over color
			m_eCurState		= IP_ENABLE;
			m_uBKColor		= 0xff000000;
			m_nBlink		= 0;
			m_uBlinkColor	= 0x0;
			m_uBlinkTime	= 1000;
			m_bUnderline	= 0;
			m_bHyperlink	= 0;
			m_bAtomy		= 0;
			m_nClickID		= -1;
			m_bTextXR       = false;
			m_bChanged		= false;
			m_pCurText		= NULL;
			m_fLineHeight	= 0;
			m_uStyle		= 0;
			m_fMaxWidth     = 0;
		}
		void SetRowDist(float rowDist)
		{
			m_fRowDist = rowDist;
		}

		void SetAutoFormated(bool b)
		{
			m_beAutoFormat = b;
		}

		void SetWndTextXR(bool b)
		{
			m_bWndTextXR = b;
		}
		void OnHighLightBegin(int32 x, int32 y)
		{
			m_highLightEnd = m_highLightBegin = HitCheck(x, y);
		}
		void OnHighLightEnd(int32 x, int32 y)
		{
			m_highLightEnd = HitCheck(x, y);
		}
		void SetTextXR(float fWndWidth)
		{
			UIVector<CText *>& vecKRText = m_KRText;
			if(vecKRText.empty())
				return;
			fWndWidth = m_bWndTextXR ? max(m_fMaxWidth,fWndWidth) : m_fMaxWidth;
			for ( UIVector<CText *>::iterator iter = vecKRText.begin() ; iter != vecKRText.end(); ++iter)
			{
				((CText *)(*iter))->m_Pos.x = fWndWidth - ((CText *)(*iter))->m_fWidth;   
			}
			vecKRText.clear();
		}
		/// @brief 得到高亮的字符串
		const wchar_t* GetHighLightText() const
		{
			// 未选择状态
			if(m_highLightBegin == m_highLightEnd)
			{
				return L"";
			}
			else
			{
				// 整理选择点，保证起点在终点之前
				CPos hlBegin(m_highLightBegin);
				CPos hlEnd(m_highLightEnd);
				if(hlBegin.x > hlEnd.x)
				{
					hlBegin = m_highLightEnd;
					hlEnd = m_highLightBegin;
				}
				else if(hlBegin.x == hlEnd.x)
				{
					if(hlBegin.y > hlEnd.y)
					{
						hlBegin.y = m_highLightEnd.y;
						hlEnd.y = m_highLightBegin.y;
					}
				}
				Ast(hlBegin.x <= hlEnd.x);

				std::wostringstream oss;
				// 得到选中的文字
				for(int i = hlBegin.x; i <= hlEnd.x; i++)
				{
					if(m_Layout[i]->GetAtomType() == EAtom_CText)
					{
						CText* pText = static_cast<CText*>(m_Layout[i]);
						// 完全捕获状态
						if(i > hlBegin.x && i < hlEnd.x)
							oss << pText->m_sText;
						// 一个Atom内的不完全捕获状态
						else if(hlBegin.x == hlEnd.x && i == hlBegin.x)
							oss << pText->m_sText.substr(hlBegin.y, hlEnd.y - hlBegin.y);
						// 左不完全捕获状态
						else if(i == hlBegin.x)
							oss << pText->m_sText.substr(hlBegin.y);
						// 右不完全捕获状态
						else// if(i == hlEnd.x)
							oss << pText->m_sText.substr(0, hlEnd.y);
					}
					else if(m_Layout[i]->GetAtomType() == EAtom_CIcon)
					{
						oss << static_cast<CIcon*>(m_Layout[i])->GetText();
					}
				}
				return oss.str().c_str();
			}
		}
		void Draw(SQRWnd* pWnd, float x, float y)
		{
			if(m_Layout.empty())
				return;

			// 未选择状态   （该分支仅仅是下面分支的等价加速）
			if(m_highLightBegin == m_highLightEnd)
			{
				for(UIVector<CAtom*>::iterator it = m_Layout.begin(); it != m_Layout.end(); ++it)
				{
					if((*it)->GetAtomType() == EAtom_CIcon)
						static_cast<CIcon*>(*it)->Draw(pWnd, x, y, false);
					else if((*it)->GetAtomType() == EAtom_CText)
						static_cast<CText*>(*it)->Draw(pWnd, x, y, false);
				}
			}
			else
			{
				// 整理选择点，保证起点在终点之前
				CPos hlBegin(m_highLightBegin);
				CPos hlEnd(m_highLightEnd);
				if(hlBegin.x > hlEnd.x)
				{
					hlBegin = m_highLightEnd;
					hlEnd = m_highLightBegin;
				}
				else if(hlBegin.x == hlEnd.x)
				{
					if(hlBegin.y > hlEnd.y)
					{
						hlBegin.y = m_highLightEnd.y;
						hlEnd.y = m_highLightBegin.y;
					}
				}
				Ast(hlBegin.x <= hlEnd.x);

				// 进行绘制
				for(int i = 0; i < static_cast<int>(m_Layout.size()); i++)
				{
					if(m_Layout[i]->GetAtomType() == EAtom_CIcon)
					{
						CIcon* pIcon = static_cast<CIcon*>(m_Layout[i]);
						// 未捕获状态
						if(i < hlBegin.x || i > hlEnd.x)
							pIcon->Draw(pWnd, x, y, false);
						else
							pIcon->Draw(pWnd, x, y, true);
					}
					else if(m_Layout[i]->GetAtomType() == EAtom_CText)
					{
						CRenderFont *pFont		= m_pCRichText->GetFont();
						float		 preTextLen	= pWnd->GetRootZoomSize();
						CText		*pText		= static_cast<CText*>(m_Layout[i]);

						// 未捕获状态
						if(i < hlBegin.x || i > hlEnd.x)
						{
							pText->Draw(pWnd, x, y, false);
						}
						// 完全捕获状态
						else if(i > hlBegin.x && i < hlEnd.x)
						{
							pText->Draw(pWnd, x, y, true);
						}
						// 一个Atom内的不完全捕获状态
						else if(hlBegin.x == hlEnd.x && i == hlBegin.x)
						{
							pText->Draw(pWnd, x, y, false, 0, hlBegin.y);
							
							preTextLen *= pFont->CalculateSize(pText->m_sText.substr(0, hlBegin.y).c_str()).x;
							pText->Draw(pWnd, x + preTextLen, y, true, hlBegin.y, hlEnd.y - hlBegin.y);

							preTextLen *= pFont->CalculateSize(pText->m_sText.substr(0, hlEnd.y).c_str()).x;
							pText->Draw(pWnd, x + preTextLen, y, false, hlEnd.y);
						}
						// 左不完全捕获状态
						else if(i == hlBegin.x)
						{
							pText->Draw(pWnd, x, y, false, 0, hlBegin.y);

							preTextLen *= pFont->CalculateSize(pText->m_sText.substr(0, hlBegin.y).c_str()).x;
							pText->Draw(pWnd, x + preTextLen, y, true, hlBegin.y);
						}
						// 右不完全捕获状态
						else if(i == hlEnd.x)
						{
							pText->Draw(pWnd, x, y, true, 0, hlEnd.y);

							preTextLen *= pFont->CalculateSize(pText->m_sText.substr(0, hlEnd.y).c_str()).x;
							pText->Draw(pWnd, x + preTextLen, y, false, hlEnd.y);
						}
					}
				}
			}
		}
		int32 AddText( SQRWnd *pWnd, const wchar_t *str )
		{
			pWnd->GetFont()->UpdateCharInfo(str);
			int32 i = 0;
			for( ; str[i]; ++i ) 
			{
				if (m_beAutoFormat)	// 自动布局
				{
					if (m_bAtomy)
					{
						if ( str[i] == L'#' && str[i+1] == L'a'/*end of atomy*/ )
						{
							m_bAtomy =! m_bAtomy;
							m_bChanged = true;
							i++; 
						}
						else if (str[i] == L'#' && str[i+1] == 0)
						{
							m_bAtomy =! m_bAtomy;
							m_bChanged = true;
							AddAtomy(pWnd, str[i]);
						}
						else
						{
							AddAtomy(pWnd, str[i]);
						}
					}
					else
					{
						if ( str[i] == L'#' ) 			// 特殊指令
						{
							++i;
							int32 a;
							SQR_TRY
							{
								a = AddSpecial( pWnd, str+i );
							}
							SQR_CATCH(exp)
							{
								string errMsg = exp.ErrorMsg();
								string errMsgPt2 = utf16_to_utf8(str);
								errMsg = errMsg + errMsgPt2;
								GenErr(exp.ErrorTitle(),	errMsg);
							}
							SQR_TRY_END;
							if( a < 0 )
								return i - 1;
							i += a - 1;
						}
						else if ( ( str[i] >= L'0' && str[i] <=L'9' ) 
							|| str[i] == L'/'
							|| str[i] == L'.' )	// 浮点数,分数,.
						{
							AddNumOrLet( pWnd, str[i], EAtom_CText_Num );
						}
						else if ( (str[i] >= L'a' && str[i] <= L'z')
							||(str[i] >= L'A' && str[i] <= L'Z'))
						{
							AddNumOrLet( pWnd, str[i], EAtom_CText_Let );
						}
						else if ( str[i] == L';' || str[i] == L'；' 
							||str[i] == L'。' 
							||str[i] == L',' || str[i] == L'，' 
							||str[i] == L'!' || str[i] == L'！')
						{
							AddPun( pWnd, str[i] );
						}
						else 
						{
							AddChar( pWnd, str[i] );
						}
					}
				}
				else				// 非自动布局
				{
					if (m_bAtomy)
					{
						if ( str[i] == L'#' && str[i+1] == L'a'/*end of atomy*/ )
						{
							m_bAtomy =! m_bAtomy;
							m_bChanged = true;
							i++; 
						}
						else if (str[i] == L'#' && str[i+1] == 0)
						{
							m_bAtomy =! m_bAtomy;
							m_bChanged = true;
							AddAtomy(pWnd, str[i]);
						}
						else
						{
							AddAtomy(pWnd, str[i]);
						}
					}
					else
					{
						if ( str[i] == L'#' ) 			// 特殊指令
						{
							++i;
							int32 a = AddSpecial( pWnd, str+i );
							if( a < 0 )
								return i - 1;
							i += a - 1;
						}
						else 
						{
							AddChar( pWnd, str[i] );
						}
					}
				}
			}
			for( UIVector<CAtom *>::iterator iter = m_CurLineList.begin(); iter != m_CurLineList.end(); ++iter )
			{
				if((*iter)->GetAtomType() == EAtom_CText)
					static_cast<CText*>(*iter)->m_fCurLineLength = m_fCurWidth;
				else if ((*iter)->GetAtomType() == EAtom_CIcon)
					static_cast<CIcon*>(*iter)->m_fCurLineLength = m_fCurWidth;
			}
			for(UIVector<CAtom*>::iterator it = m_Layout.begin(); it != m_Layout.end(); ++it)
			{
				if((*it)->GetAtomType() == EAtom_CText)
					static_cast<CText*>(*it)->m_fLayoutHeight = m_fCurHeight + m_fLineHeight;
				else if ((*it)->GetAtomType() == EAtom_CIcon)
				{
					static_cast<CIcon*>(*it)->m_fLayoutHeight =  m_fCurHeight + m_fLineHeight;
				}
			}

			float Zoom = pWnd->GetRootZoomSize();
			float dx = GetXBorder(pWnd->GetStyle())*2*Zoom;
			SetTextXR(pWnd->GetWndWidth() - dx);

			if (m_UrlList.size() > 0)
				GenErr("RichText Error",utf16_to_utf8(str));
			return i;
		}

	private:
		/// @brief 得到鼠标点击位置所对应的文字逻辑位置
		CPos HitCheck(int32 x, int32 y)
		{
			if(this->m_Layout.empty())
				return CPos(-1, -1);

			UIList<size_t> lineHitList;
			// 得到鼠标点击所在的行
			for(size_t i = 0; i < this->m_Layout.size(); ++i)
			{
				if(y >= this->m_Layout[i]->m_Pos.y && y < this->m_Layout[i]->m_Pos.y + this->m_Layout[i]->m_fHeight)
					lineHitList.push_back(i);
			}
			// 点在最后一行之下则认为是点在最后一行
			if(lineHitList.empty())
				lineHitList.push_back(this->m_Layout.size() - 1);

			// 检测鼠标在行中的位置
			float curr_len = 0.0f;
			for(UIList<size_t>::const_iterator it = lineHitList.begin(); it != lineHitList.end(); ++it)
			{
				// 如果是CText则详细确定字符位置
				if ( m_Layout[*it]->GetAtomType() == EAtom_CText )
				{
					// 鼠标在该控件内
					CText *pText = static_cast<CText*>(m_Layout[*it]);
					if(x >= m_Layout[*it]->m_Pos.x && x < m_Layout[*it]->m_Pos.x + m_Layout[*it]->m_fWidth)
					{
						// 取得鼠标所在字符的位置
						float charSize = 0.0f;
						for(size_t j = 0; j < pText->m_sText.size(); j++, curr_len += charSize)
						{
							UIWString curr_char(1, pText->m_sText[j]);
							charSize = m_pCRichText->GetFont()->CalculateSize(curr_char.c_str()).x;

							if(x >= curr_len && x < curr_len + charSize)
								return  CPos(*it, j);
						}

						return CPos(*it, static_cast<int32>(pText->m_sText.size()));
					}
					// 鼠标不再该控件内，当前长度以当前控件长度为值增长
					else
						curr_len += m_pCRichText->GetFont()->CalculateSize(pText->m_sText.c_str()).x;
				}
				// 其他类型的就只返回有意义的控件下标
				else
					return CPos(*it, -1);	
			}

			// 没有检测到该行内的任何CAtom，则查看CAtom空隙
			for(size_t i = 0; i < lineHitList.size() - 1; i++)
			{
				// 发现在控件间隙
				if(x >= this->m_Layout[i]->m_Pos.x + this->m_Layout[i]->m_fWidth && x <= this->m_Layout[i + 1]->m_Pos.x)
				{
					if(this->m_Layout[i]->GetAtomType() == EAtom_CText)
					{
						CText* pText = static_cast<CText*>(this->m_Layout[i]);
						return CPos(i, static_cast<int>(pText->m_sText.size()));
					}
					else
					{
						return CPos(i, -1);
					}
				}
			}
			// 实在啥都没找到就返回最后的位置
			if(this->m_Layout[lineHitList.back()]->GetAtomType() == EAtom_CText)
			{
				CText* pText = static_cast<CText*>(this->m_Layout[lineHitList.back()]);
				return CPos(lineHitList.back(), static_cast<int>(pText->m_sText.size()));
			}
			else
			{
				return CPos(lineHitList.back(), -1);
			}
		}
		void Free()
		{
			for( UIVector<CAtom *>::iterator iter = m_Layout.begin(); iter != m_Layout.end(); ++iter ) 
				delete (*iter);
			m_Layout.clear();

			m_KRText.clear();

			m_CurLineList.clear();

			m_UrlList.clear();

			m_pCurText=NULL;
		}
		void NewLine()
		{
			float dx = 0;
			if( m_uStyle == 1 )
				dx = ( m_fWidth - m_fCurWidth )/2;
			else if( m_uStyle == 2 )
				dx = m_fWidth - m_fCurWidth;
			else
				dx = 0;

			for( UIVector<CAtom *>::reverse_iterator iter = m_Layout.rbegin(); iter != m_Layout.rend(); ++iter ) 
			{
				CFPos pos = (*iter)->m_Pos;
				if( pos.y == m_fCurHeight )
				{
					pos.x += dx;
					pos.y += m_fLineHeight - (*iter)->GetHeight();
					(*iter)->m_Pos = pos;
				}
				else
					break;
			}

			for( UIVector<CAtom*>::iterator iter = m_CurLineList.begin(); iter != m_CurLineList.end(); ++iter )
			{
				if((*iter)->GetAtomType() == EAtom_CText)
					static_cast<CText*>(*iter)->m_fCurLineLength = m_fCurWidth;
				
				else if ((*iter)->GetAtomType() == EAtom_CIcon)
					static_cast<CIcon*>(*iter)->m_fCurLineLength = m_fCurWidth;
			}				
			if( !m_CurLineList.empty())
				m_CurLineList.clear();

			if( m_fMaxWidth < m_fCurWidth)
				m_fMaxWidth = m_fCurWidth;

			m_fCurHeight	+= m_fLineHeight+m_fRowDist;
			m_fCurWidth		= 0;
			m_fLineHeight	= 0;
			m_pCurText		= NULL;
		}

		void AddNumOrLet( SQRWnd* pWnd, int32 c, EAtomCTextTpye E )
		{
			CText* t_CurText = NULL;
			WndHandle* pHandle	= pWnd->GetWndHandle();
			float fNumWidth		= pWnd->GetFont()->CalculateSize((const wchar_t*)&c).x;
			float fLineSpacing	= pWnd->GetFont()->GetFontGlyphHeight(c);
			if ( m_pCurText && ( m_pCurText->GetAtomCTextType() == E || IsCTextInTextXR(m_pCurText) ) )
			{
				if (m_fCurWidth + fNumWidth <= m_fWidth - 0.5f)// 能够放下数字
				{
					if ( !m_bChanged )
					{
						m_fCurWidth += fNumWidth;
						m_fMaxWidth = max( m_fMaxWidth, m_fCurWidth );
						m_pCurText->AddChar( pWnd, c, fNumWidth );
						if( m_fLineHeight < fLineSpacing)
						{
							m_fLineHeight = fLineSpacing;
							m_pCurText->m_fHeight = m_fLineHeight;
						}
						if ( m_bTextXR )
						{
							m_bTextXR = false;
							m_KRText.push_back( m_pCurText );
						}

						return;
					}
				}
				else if ( 1 == m_CurLineList.size() || IsCTextInTextXR(m_pCurText))	// 数字一行放不下或是右对齐的，强制换行
				{
					NewLine();
					m_fLineHeight = fLineSpacing;
				}
				else									// 本行放不下，整体强制换行
				{
					// 保存数据
					t_CurText = m_pTempText;
					m_pCurText->CTextFullCopy(t_CurText);
					// 清理数据
					UIVector<CAtom *>::iterator iter = m_Layout.end();

					if (m_Layout.size() != 0)
					{
						iter--;
						delete (*iter);
					}

					m_CurLineList.pop_back();
					m_Layout.pop_back();
					if (m_bHyperlink)
					{
						m_UrlList.pop_back();
					}
					// 整理当前行长度
					m_fCurWidth -= t_CurText->GetWidth();

					NewLine();
					m_fLineHeight = fLineSpacing;
				}
			}
			else if ( m_pCurText && c == L'.' )
			{
				AddPun( pWnd, c );
				return;
			}
			if( m_fLineHeight < fLineSpacing)
				m_fLineHeight = fLineSpacing;


			m_pCurText					= new CText;
			// 创建数字原子
			if ( !t_CurText )
			{
				m_pCurText->m_uColor		= m_uColor;
				m_pCurText->m_uClkDColor	= m_uClkDColor;	// click down color
				m_pCurText->m_uMSOVColor	= m_uMSOVColor;	// mouse over color
				m_pCurText->m_eCurState		= IP_ENABLE;
				m_pCurText->m_uBKColor		= m_uBKColor;
				m_pCurText->m_nBlink		= m_nBlink;
				m_pCurText->m_uBlinkColor	= m_uBlinkColor;
				m_pCurText->m_uBlinkTime	= m_uBlinkTime;
				m_pCurText->m_bUnderline	= m_bUnderline;
				m_pCurText->m_bHyperlink	= m_bHyperlink;
				m_pCurText->m_nClickID		= m_nClickID;
				m_pCurText->m_Pos			= CFPos( m_fCurWidth, m_fCurHeight );
				m_pCurText->m_fHeight		= fLineSpacing;
				m_pCurText->m_eCTextType	= E;

				m_pCurText->AddChar( pWnd, c, fNumWidth );
				m_fCurWidth		   += fNumWidth;
				m_fMaxWidth			= max( m_fMaxWidth, m_fCurWidth ); 

				if ( m_bChanged )
				{
					if ( m_bTextXR )
					{
						m_bTextXR = false;
						m_KRText.push_back( m_pCurText );
					}
				}

				m_bChanged			= false;
			}
			else
			{
				t_CurText->CTextFullCopy(m_pCurText);
				m_pCurText->m_Pos			= CFPos( m_fCurWidth, m_fCurHeight );
				m_pCurText->AddChar( pWnd, c, fNumWidth );
				// 计算m_pCurText的长度
				m_fCurWidth			= m_pCurText->m_fWidth;
				m_fMaxWidth			= max( m_fMaxWidth, m_fCurWidth ); 

				if ( m_bChanged )
				{
					if ( m_bTextXR )
					{
						m_bTextXR = false;
						m_KRText.push_back( m_pCurText );
					}
				}

				m_bChanged			= false;
			}


			m_CurLineList.push_back(m_pCurText);
			m_Layout.push_back( m_pCurText );
			if (m_bHyperlink)
				m_UrlList.push_back( m_pCurText );
		}


		void AddPun( SQRWnd* pWnd, int32 c )
		{
			CText*	t_CurText = NULL;

			int32 t_c = -1;
			float t_CharWidth = 0;
			WndHandle* pHandle = pWnd->GetWndHandle();
			float fCharWidth   = pWnd->GetFont()->CalculateSize((const wchar_t*)&c).x;
			float fLineSpacing = pWnd->GetFont()->GetFontGlyphHeight(c);

			if( m_pCurText )
			{
				// 可以放下该标点，万事大吉
				if (m_fCurWidth + fCharWidth <= m_fWidth - 0.5f)
				{
					if( !m_bChanged )
					{
						m_pCurText->m_eCTextType	= EAtom_CText_Pun;
						m_fCurWidth += fCharWidth;
						m_fMaxWidth = max( m_fMaxWidth, m_fCurWidth ); 
						m_pCurText->AddChar( pWnd, c, fCharWidth );
						if( m_fLineHeight < fLineSpacing)
						{
							m_fLineHeight = fLineSpacing;
							m_pCurText->m_fHeight = m_fLineHeight;
						}
						if ( m_bTextXR )
						{
							m_bTextXR = false;
							m_KRText.push_back( m_pCurText );
						}
						return;
					}
				}
				else if (IsCTextInTextXR(m_pCurText) || m_pCurText->GetAtomCTextType() == EAtom_CText_Pun 
					|| ( m_pCurText->m_bHyperlink && !m_bHyperlink))	
				{
					NewLine();
					m_fLineHeight = fLineSpacing;
				}
				else if ( m_pCurText->GetAtomCTextType() == EAtom_CText_Let || m_pCurText->GetAtomCTextType() == EAtom_CText_Num )
				{
					// 保存数据
					t_CurText = m_pTempText;
					m_pCurText->CTextFullCopy(t_CurText);
					// 清理数据
					m_CurLineList.pop_back();
					UIVector<CAtom *>::iterator iter = m_Layout.end();
					
					if (m_Layout.size() != 0)
					{
						iter--;
						delete (*iter);
					}
					m_Layout.pop_back();
					if (m_pCurText->m_bHyperlink)
					{
						m_UrlList.pop_back();
					}

					// 整理当前行长度
					m_fCurWidth -= t_CurText->GetWidth();

					NewLine();
					m_fLineHeight = fLineSpacing;
				}
				else
				{
					// 取出一个字符，清理原来的数据
					t_c = m_pCurText->RemoveCharEnd( pWnd );
					t_CharWidth   = pWnd->GetFont()->CalculateSize((const wchar_t*)&c).x;
					m_fCurWidth		   -= t_CharWidth;
					m_fMaxWidth			= max( m_fMaxWidth, m_fCurWidth ); 

					NewLine();
					m_fLineHeight = fLineSpacing;
				}
			}

			if( m_fLineHeight < fLineSpacing)
				m_fLineHeight = fLineSpacing;
			
			m_pCurText					= new CText;
			// 创建数字原子
			if (!t_CurText)
			{
				m_pCurText->m_uColor		= m_uColor;
				m_pCurText->m_uClkDColor	= m_uClkDColor;	// click down color
				m_pCurText->m_uMSOVColor	= m_uMSOVColor;	// mouse over color
				m_pCurText->m_eCurState		= IP_ENABLE;
				m_pCurText->m_uBKColor		= m_uBKColor;
				m_pCurText->m_nBlink		= m_nBlink;
				m_pCurText->m_uBlinkColor	= m_uBlinkColor;
				m_pCurText->m_uBlinkTime	= m_uBlinkTime;
				m_pCurText->m_bUnderline	= m_bUnderline;
				m_pCurText->m_bHyperlink	= m_bHyperlink;
				m_pCurText->m_nClickID		= m_nClickID;
				m_pCurText->m_Pos			= CFPos( m_fCurWidth, m_fCurHeight );
				m_pCurText->m_fHeight		= fLineSpacing;
				m_pCurText->m_eCTextType	= EAtom_CText_Pun;

				if (t_c != -1)
				{
					m_pCurText->AddChar( pWnd, t_c, t_CharWidth );
					m_fCurWidth		   += t_CharWidth;
				}
				m_pCurText->AddChar( pWnd, c, fCharWidth );
				m_fCurWidth		   += fCharWidth;
				m_fMaxWidth			= max( m_fMaxWidth, m_fCurWidth ); 

				if ( m_bChanged )
				{
					if ( m_bTextXR )
					{
						m_bTextXR = false;
						m_KRText.push_back( m_pCurText );
					}
				}

				m_bChanged			= false;
			}
			else
			{
				t_CurText->CTextFullCopy(m_pCurText);
				m_pCurText->m_Pos			= CFPos( m_fCurWidth, m_fCurHeight );

				m_pCurText->AddChar( pWnd, c, fCharWidth );
				// 计算m_pCurText的长度
				m_fCurWidth			= m_pCurText->m_fWidth;
				m_fMaxWidth			= max( m_fMaxWidth, m_fCurWidth ); 

				if ( m_bChanged )
				{
					if ( m_bTextXR )
					{
						m_bTextXR = false;
						m_KRText.push_back( m_pCurText );
					}
				}

				m_bChanged			= false;
			}

			m_CurLineList.push_back(m_pCurText);
			m_Layout.push_back( m_pCurText );
			if (m_bHyperlink)
				m_UrlList.push_back( m_pCurText );
			
		}
		
		void AddAtomy(SQRWnd* pWnd, int32 c)
		{
			// 往该原子上放，放不下，整体换行
			// 把原子的内容都当作letter来处理
			AddNumOrLet( pWnd, c, EAtom_CText_Let );
		}

		void AddChar( SQRWnd* pWnd, int32 c )
		{
			WndHandle* pHandle = pWnd->GetWndHandle();
			float fCharWidth   = pWnd->GetFont()->CalculateSize((const wchar_t*)&c).x;
			float fLineSpacing = pWnd->GetFont()->GetFontGlyphHeight(c);

			if( m_pCurText )
			{
				m_pCurText->m_eCTextType	= EAtom_CText_Cha;
				if( m_fCurWidth + fCharWidth <= m_fWidth - 0.5f)
				{
					if( !m_bChanged )
					{
						m_fCurWidth += fCharWidth;
						m_fMaxWidth = max( m_fMaxWidth, m_fCurWidth ); 
						m_pCurText->AddChar( pWnd, c, fCharWidth );
						if( m_fLineHeight < fLineSpacing)
						{
							m_fLineHeight = fLineSpacing;
							m_pCurText->m_fHeight = m_fLineHeight;
						}
						if ( m_bTextXR )
						{
							m_bTextXR = false;
							m_KRText.push_back( m_pCurText );
						}
						
						return;
					}
				}
				else
				{
					NewLine();
					m_fLineHeight = fLineSpacing;
				}
			}
			if( m_fLineHeight < fLineSpacing)
				m_fLineHeight = fLineSpacing;

			m_pCurText					= new CText;
			m_pCurText->m_uColor		= m_uColor;
			m_pCurText->m_uClkDColor	= m_uClkDColor;	// click down color
			m_pCurText->m_uMSOVColor	= m_uMSOVColor;	// mouse over color
			m_pCurText->m_eCurState		= IP_ENABLE;
			m_pCurText->m_uBKColor		= m_uBKColor;
			m_pCurText->m_nBlink		= m_nBlink;
			m_pCurText->m_uBlinkColor	= m_uBlinkColor;
			m_pCurText->m_uBlinkTime	= m_uBlinkTime;
			m_pCurText->m_bUnderline	= m_bUnderline;
			m_pCurText->m_bHyperlink	= m_bHyperlink;
			m_pCurText->m_nClickID		= m_nClickID;
			m_pCurText->m_Pos			= CFPos( m_fCurWidth, m_fCurHeight );
			m_pCurText->m_fHeight		= fLineSpacing;
			m_pCurText->m_eCTextType	= EAtom_CText_Cha;

			m_pCurText->AddChar( pWnd, c, fCharWidth );
			m_fCurWidth		   += fCharWidth;
			m_fMaxWidth			= max( m_fMaxWidth, m_fCurWidth ); 

			if ( m_bChanged )
			{
				if ( m_bTextXR )
				{
					m_bTextXR = false;
					m_KRText.push_back( m_pCurText );
				}
			}

			m_bChanged			= false;

			m_CurLineList.push_back(m_pCurText);
			m_Layout.push_back( m_pCurText );
			if (m_bHyperlink)
				m_UrlList.push_back( m_pCurText );
		}

		void AddIcon( SQRWnd* pWnd, int32 nIndex )
		{
			CIcon* pIcon = new CIcon( m_ImageList, nIndex );
			if( m_fCurWidth !=0 && m_fCurWidth + pIcon->GetWidth() > m_fWidth )
				NewLine();

			pIcon->m_nClickID	=m_nClickID;
			pIcon->m_Pos		= CFPos( m_fCurWidth, m_fCurHeight );
			m_fCurWidth			+= pIcon->GetWidth();
			m_fMaxWidth			= max( m_fMaxWidth, m_fCurWidth ); 
			if( m_fLineHeight < pIcon->GetHeight() )
				m_fLineHeight = pIcon->GetHeight();
			m_CurLineList.push_back(pIcon);
			m_Layout.push_back( pIcon );
			m_bChanged = true;
		}

		void SetColor( uint32 uColor )
		{
			if( uColor != m_uColor )
			{
				m_uColor	= uColor;
				m_bChanged	= true;
			}
		}

		int32 AddSpecial( SQRWnd* pWnd, const wchar_t *s )
		{
			static uint32 t_uGroup = 0;
			switch( s[0] ) 
			{
			case 0:
				return -1;
			case L'#':
				AddChar( pWnd, L'#' );
				return 1;
			case L'P':
				m_uStyle = limit2( s[1] - L'0', 0, 9 );
				return 1 + ( s[1] <= L'9' && s[1] >= L'0' );
			case L'N':
				SetColor( 0xfffffe91 );
				return 1;
			case L'M':
				SetColor( 0xffff1200 );
				return 1;
			case L'A':
				SetColor( 0xff9CF46C );
				return 1;
			case L'G':
				SetColor( 0xff00ff00 );
				return 1;
			case L'B':
				SetColor( 0xff0000ff );
				return 1;
			case L'Y':
				SetColor( 0xffffff00 );
				return 1;
			case L'W':
				SetColor( 0xffffffff );
				return 1;
			case L'K':
				SetColor( 0xff000000 );
				return 1;
			case L'V': //紫罗兰
				SetColor( 0xff8000ff );
				return 1;
			case L'r':		
				if( m_fLineHeight < pWnd->GetFont()->GetLineSpacing() )
				{
					m_fLineHeight = pWnd->GetFont()->GetLineSpacing();
				}
				NewLine();
				return 1;
			case L'u':
				m_bUnderline =! m_bUnderline;
				m_bChanged = true;
				return 1;
			case L'l':
				m_bHyperlink =! m_bHyperlink;
				m_bChanged = true;
				if ( m_bHyperlink == false )
				{	
					if (++t_uGroup > 0xFFFFFF)
						t_uGroup = 1;
					UIWString sUrl = L"";
					UIWString temp = L"";
					for( UIVector<CText *>::iterator iter = m_UrlList.begin(); iter != m_UrlList.end(); ++iter )
					{
						sUrl += ((CText*)(*iter))->m_sText; 
						temp = ((CText*)(*iter))->m_sAttr;
					}
					for( UIVector<CText *>::iterator iter = m_UrlList.begin(); iter != m_UrlList.end(); ++iter )
					{
						((CText*)(*iter))->m_sAttr = temp;
						((CText*)(*iter))->m_sUrl = sUrl; 
						((CText*)(*iter))->m_uGroup = t_uGroup;
					}
					m_UrlList.clear();
				}
				else
				{
					if ( s[1] == L'#' && s[2] == L's' )
					{
						int32 m;
						uint32 col =  (uint32)(0xff000000|GetNumber( s + 3, m)) ;
						m_uMSOVColor = col;
						if (s[m + 3] == L'#' && s[m + 4] == L's')
						{
							int32 n;
							uint32 col =  (uint32)(0xff000000|GetNumber( s + 5 + m, n)) ;
							m_uClkDColor = col;
							return m + n + 5;
						}
						else
						{
							m_uClkDColor = m_uColor;
							return m + 3;
						}
					}
					else
					{
						m_uClkDColor = m_uColor;	// click down color
						m_uMSOVColor = m_uColor;
						return 1;
					}
				}
				return 1;			
			case L'b':
				m_nBlink = !m_nBlink;  
				m_bChanged = true;
				if( m_nBlink )
				{
					// 闪烁次数
					int32 n;
					m_nBlink = GetNumber( s + 1, n, 3 );
					m_nBlink = m_nBlink ? m_nBlink : -1;
					// 闪烁颜色
					if ( s[n + 1] == L'#' )	
					{
						int32 m;
						// 带透明度值的
						uint32 col =  (uint32)(0xff000000|GetNumber( s + n + 2, m)) ;
						m_uBlinkColor = col;

						if (s[m + n + 2] == L'#')
						{
							int p;
							uint32 ctm = GetNumber( s + n + m + 3, p, 5);
							limit(ctm, (uint32)0, (uint32)100000);
							m_uBlinkTime = ctm;
							return m + n + p + 3;
						}
						else
						{
							m_uBlinkTime = 1000;
							return m + n + 2;
						}
					}
					else
					{
						m_uBlinkColor = 0x0;
						m_uBlinkTime = 1000;
						return n + 1;
					}
				}
				return 1;
			case L'n':
				if( m_bUnderline )
					m_bChanged = true;
				if( m_nBlink )
					m_bChanged = true;
				m_bUnderline = false;
				m_nBlink = 0;
				m_uBlinkColor = 0x0;
				m_uBlinkTime = 1000;

				SetColor( 0xffffffff );
				return 1;
			case L'0':
			case L'1':
			case L'2':
			case L'3':
			case L'4':
			case L'5':
			case L'6':
			case L'7':
			case L'8':
			case L'9':
				{
					int32 n;
					AddIcon( pWnd, GetNumber( s, n, 4 ) );
					return n;
				}
			case L'e':
				{
					int32 i;
					uint32 uColor = 0xff000000|GetNumber( s + 1, i );
					if( m_uBKColor != uColor )
					{
						m_uBKColor = uColor;
						m_bChanged = true;
					}
					return i + 1;
				}
			case L'c':
				if ( s[1] != L'(' ) 
				{
					int32 i;
					SetColor( 0xff000000|GetNumber( s + 1, i ) );
					return i + 1;
				}
				else
				{
					int32 i;
					int32 nClickID = GetNumber( s + 2, i, 10 );
					if( i == 0 )
						nClickID = -1;
					if( s[i+2] == L')' ) 
						++i;
					if( m_nClickID != nClickID )
					{
						m_nClickID = nClickID;
						m_bChanged = true;
					}
					return i + 2;
				}
			case L'i':
				if (s[1] == L'[' )
				{	
					int32 i = 2;
					UIWString temp;
					for( ; s[i]; ++i ) 
					{	 
						if (s[i] == L']')
						{
							if (m_pCurText)
								m_pCurText->m_sAttr = temp;
							return i+1;
						}
						temp += s[i];
					}
					return i;
				}
				return 1;
			case L'R': //部分文字靠右功能
				m_bChanged = true;
				m_bTextXR = true;
				return 1;
			case L'a': 
				//取atom，原子之意。#a...#a ...作为一个整体，
				//里面不可以嵌套任何其他的特殊含义字符，图片等（字体表情除外）
				m_bAtomy =! m_bAtomy;
				m_bChanged = true;
				return 1;

			}
			return 0;
		}

		bool IsCTextInTextXR(CText* T)
		{
			UIVector<CText *>& vecKRText = m_KRText;
			if(vecKRText.empty())
				return false;
			for ( UIVector<CText *>::iterator iter = vecKRText.begin() ; iter != vecKRText.end(); ++iter)
			{
				if (((CText *)(*iter)) == T)
				{
					return true;
				}
			}
			return false;
		}

	private:
		static inline int32 GetNumber( const wchar_t* szStr, int32& n, int32 nMaxChar )
		{		
			int64 nNumber = 0;
			for( n = 0; n < nMaxChar && szStr[n] <= L'9' && szStr[n] >= L'0'; n++ )
				nNumber = nNumber*10 + ( szStr[n] - L'0' );
			return (int32)nNumber;
		}

		static inline uint32 GetNumber( const wchar_t* szStr, int32& n )
		{
			uint32 uNumber = 0;
			for( n = 0; n < 6; n++ )
			{
				if( szStr[n] >= L'0' && szStr[n] <= L'9' )
					uNumber = ( uNumber<<4 ) + ( szStr[n] - L'0' ); 
				else if( szStr[n] >= L'a' && szStr[n] <= L'f' )
					uNumber = ( uNumber<<4 ) + ( szStr[n] - 'a' + 10 ); 
				else if( szStr[n] >= L'A' && szStr[n] <= L'F' )
					uNumber = ( uNumber<<4 ) + ( szStr[n] - L'A' + 10 ); 
				else
					return uNumber;
			}
			return uNumber;
		}
	};

	RICH_TEXT* _R( void* hRichText ){ return (RICH_TEXT*)hRichText; };

	// ----------------------------------------------------------------------------------------
	// SQRRichString
	// ----------------------------------------------------------------------------------------
	SQRRichString::SQRRichString()
		: m_hRichText(new RICH_TEXT())	
	{
	}

	SQRRichString::SQRRichString(SQRRichText *pCRichText) 
		: m_hRichText(new RICH_TEXT(pCRichText))
	{
	}

	SQRRichString::~SQRRichString(void)
	{
		Free();
	}

	void SQRRichString::ReleaseWndTex()
	{
		// 释放可以释放的UI贴图资源
		UIMap<int32,WND_IMAGE_LIST>::iterator itr = _R( m_hRichText )->m_ImageList.begin();
		UIMap<int32,WND_IMAGE_LIST>::iterator itrend = _R( m_hRichText )->m_ImageList.end();

		while(itr != itrend)
		{
			WND_IMAGE_LIST& CurImage = itr->second;
			CurImage.ReleaseTex();
			++itr;
		}
	}
	void SQRRichString::CreateWndTex()
	{
		// 显示之前申请资源
		UIMap<int32,WND_IMAGE_LIST>::iterator itr = _R( m_hRichText )->m_ImageList.begin();
		UIMap<int32,WND_IMAGE_LIST>::iterator itrend = _R( m_hRichText )->m_ImageList.end();

		while(itr != itrend)
		{
			WND_IMAGE_LIST& CurImage = itr->second;
			CurImage.CreatTex( CGraphic::GetInst() );
			++itr;
		}
	}

	void SQRRichString::Free()
	{
		UIMap<int32,WND_IMAGE_LIST>::iterator itr = _R( m_hRichText )->m_ImageList.begin();
		UIMap<int32,WND_IMAGE_LIST>::iterator itrend = _R( m_hRichText )->m_ImageList.end();

		while(itr != itrend)
		{
			WND_IMAGE_LIST& CurImage = itr->second;
			AdvDelRef(&CurImage);

			++itr;
		}

		delete _R( m_hRichText );
	}

	void SQRRichString::InsertImage( int32 nIndex, WND_IMAGE_LIST& ImageAni )
	{
		WND_IMAGE_LIST& CurImage=_R( m_hRichText )->m_ImageList[nIndex];

		AdvDelRef(&CurImage);
		CurImage=ImageAni;
		AdvAddRef(&CurImage);
	}

	float SQRRichString::GetCurHeight()
	{
		return _R( m_hRichText )->m_fCurHeight + _R( m_hRichText )->m_fLineHeight
			 + _R( m_hRichText )->m_fRowDist/2.0f;
	}

	float SQRRichString::GetMaxWidth()
	{
		return _R( m_hRichText )->m_fMaxWidth;
	}

	void SQRRichString::SetMaxWidth( float fMaxWidth )
	{
		_R( m_hRichText )->m_fMaxWidth = fMaxWidth;
	}

	void SQRRichString::SetWidth( float fWidth )
	{
		_R( m_hRichText )->m_fWidth = fWidth;
	}

	int32 SQRRichString::SetText( SQRWnd* pWnd, const wchar_t* str )
	{
		if ( !str )
			return 0;
		_R( m_hRichText )->Init();
		return _R( m_hRichText )->AddText( pWnd, str );
	}

	int32 SQRRichString::AddText( SQRWnd* pWnd, const wchar_t* str )	
	{
		return _R( m_hRichText )->AddText( pWnd, str );
	}

	void SQRRichString::SetBeAutoFormat(bool b)
	{
		_R( m_hRichText )->SetAutoFormated( b );
	}

	void SQRRichString::SetWndTextXR(bool b)
	{
		_R( m_hRichText )->SetWndTextXR( b);
	}

	void SQRRichString::Draw( SQRWnd* pWnd, float x, float y )
	{
		_R(m_hRichText)->Draw(pWnd, x, y);
	}
	int32 SQRRichString::Click( float x, float y, uint32 sty, float wndwidth ,float wndheight, uint32 uMsgID)
	{
		bool need_modify = false;
		if(sty & WS_TEXTXC  || sty & WS_TEXTXR || sty & WS_TEXTYC)
			if( (WS_MULLINETEXT&sty) != WS_MULLINETEXT )
				need_modify = true;
		int succed = -1;
		float temp_x;
		float temp_y;
		RICH_TEXT* pRichText = _R( m_hRichText );
		EventStateMask T_ESM = IP_ENABLE;
		uint32	t_group = (uint32)(-1);
		for(UIVector<CAtom *>::iterator iter = pRichText->m_Layout.begin(); iter != pRichText->m_Layout.end(); ++iter) 
		{
			temp_x = x;
			temp_y = y;
			if( need_modify )
			{
				if( sty & WS_TEXTXC )
					temp_x -= (int32)(wndwidth/2 - (*iter)->m_fCurLineLength/2);
				else if( sty & WS_TEXTXR )
					temp_x -= (int32)(wndwidth - (*iter)->m_fCurLineLength);
				if( sty & WS_TEXTYC )
					temp_y -= (int32)(wndheight/2 - (*iter)->m_fLayoutHeight/2);
				else if( sty & WS_TEXTYB )
					temp_y -= (int32)(wndheight - (*iter)->m_fLayoutHeight);
			}
			if( temp_x >= (*iter)->m_Pos.x && 
				temp_y >= (*iter)->m_Pos.y &&
				temp_x < (*iter)->m_fWidth + (*iter)->m_Pos.x &&
				temp_y <= (*iter)->m_fHeight + (*iter)->m_Pos.y + 1 )
			{
				if (((CText*)(*iter))->m_bHyperlink == true)
				{
					if (uMsgID == RICHWND_MOUSEOVER)
					{
						((CText*)(*iter))->m_eCurState = IP_MOUSEOVER;
						t_group = ((CText*)(*iter))->m_uGroup;
						T_ESM = IP_MOUSEOVER;
					}
					else if ( uMsgID == RICHWND_MOUSEOVERDOWN )
					{
						((CText*)(*iter))->m_eCurState = IP_CLICKDOWN;
						t_group = ((CText*)(*iter))->m_uGroup;
						T_ESM = IP_CLICKDOWN;
					}
					else
					{
						m_ChooseStr = ((CText*)(*iter))->m_sUrl;
						m_sChooseStrAttr = ((CText*)(*iter))->m_sAttr;
						if ( uMsgID == RICHWND_LCLICKUP 
						  || uMsgID == RICHWND_RCLICKUP 
						  || uMsgID == RICH_CLICK 
						  || uMsgID == RICH_RCLICK )
						{
							((CText*)(*iter))->m_eCurState = IP_MOUSEOVER;
							t_group = ((CText*)(*iter))->m_uGroup;
							T_ESM = IP_MOUSEOVER;
						}
						else if ( uMsgID == RICHWND_LCLICKDOWN 
							   || uMsgID == RICHWND_RCLICKDOWN )
						{
							((CText*)(*iter))->m_eCurState = IP_CLICKDOWN;
							t_group = ((CText*)(*iter))->m_uGroup;
							T_ESM = IP_CLICKDOWN;
						}
						else
							((CText*)(*iter))->m_eCurState = IP_ENABLE;
						succed = 0;
					}
				}
			}
			else
				((CText*)(*iter))->m_eCurState = IP_ENABLE;
		}
		if( T_ESM != IP_ENABLE )
		{
			for(UIVector<CAtom *>::iterator iter = pRichText->m_Layout.begin(); iter != pRichText->m_Layout.end(); ++iter) 
				if (((CText*)(*iter))->m_uGroup == t_group)
					((CText*)(*iter))->m_eCurState = T_ESM;
		}
		return succed;
	}

	void SQRRichString::SetRowDist(float rowDist)
	{
		_R(m_hRichText)->SetRowDist(rowDist);
	}

	void SQRRichString::OnHighLightBegin(int32 x, int32 y)
	{
		_R(m_hRichText)->OnHighLightBegin(x, y);
	}

	void SQRRichString::OnHighLightEnd(int32 x, int32 y)
	{
		_R(m_hRichText)->OnHighLightEnd(x, y);
	}

	const wchar_t* SQRRichString::GetHighLightText()const
	{
		return _R(m_hRichText)->GetHighLightText();
	}

	// ----------------------------------------------------------------------------------------
	// SQRRichText
	// ----------------------------------------------------------------------------------------
	DYNAMIC_CREATE_WND( SQRRichText, SQRWnd )

	SQRRichText::SQRRichText(void)
		: m_fMinWidth(0)
		, m_fMaxWidth(0)
		, m_fLastCurHeight(0)
		, m_fMinHeight(0)
		,m_fInitMinWidth(0)
		,m_fInitMaxWidth(0)
	{
		m_pRichString = new SQRRichString(this);
	}

	SQRRichText::~SQRRichText(void)
	{
		for (size_t i = 0; i < mClrWnd.size(); ++i)
		{
			SQRRichWndItem *p = mClrWnd[i];
			delete p;
		}
		mClrWnd.clear();

		if ( m_pRichString )
			delete m_pRichString;
	}

	int32 SQRRichText::Create( const WndCreateParam& param )
	{
		WndCreateParam newParam = param;
		newParam.uStyle |= WS_CHILD;
		int32 nRet = SQRWnd::Create( newParam );

		WndCreateParam sbParam;
		sbParam.x = GetWndWidth();
		sbParam.width = 16;
		sbParam.height = GetWndHeight();
		sbParam.pParentWnd = this;
		sbParam.nFontID = param.nFontID;
		sbParam.font_size = param.font_size;
		m_sbVertical.Create(sbParam); 
		//cy 在GuiEditor里面把滚动条的锁定设置成动态的最好
		m_sbVertical.SetLockFlag( LOCK_LEFT|LOCK_TOP|LOCK_BOTTOM );

		m_sbVertical.SetRange( int32(GetWndHeight() + GetYBorder(GetStyle())*2) );
		m_sbVertical.SetPage( int32(GetWndHeight()) );
		m_sbVertical.SetPos( 0 );

		m_sbVertical.ShowWnd( (GetStyle()&WS_VERSCROLL) != 0 );
		SetWndText( param.szTitle );
		return nRet;
	}

	void SQRRichText::OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 )
	{
		if (CILCK_WNDER == uMsgID)
		{
			CFRect rt;
			for (size_t i = 0; i < mClrWnd.size(); ++i)
			{
				mClrWnd[i]->mWnd.GetWndRect(rt);
				if (static_cast<float>(uParam1) >= rt.left && static_cast<float>(uParam1) <= rt.right
					&& static_cast<float>(uParam2) >= rt.top && static_cast<float>(uParam2) <= rt.bottom) 
				{
					SendCtrlMsg(RICH_CHOOSE, 0, 0);
					return;
				}
			}
		}
		else if( SCROLL_RELEASE == uMsgID && pChild == &m_sbVertical && uParam1 != SM_SC_BAR )
		{
			m_sbVertical.SetPos( int32((m_sbVertical.GetPos() - uParam2)*GetFontSize() + uParam2) );
		}
		else 
		{
			SendCtrlMsg( RICH_SCROLL, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );	
		}

	}

	void SQRRichText::InsertImage( int32 nIndex, WND_IMAGE_LIST& ImageAni )
	{
		m_pRichString->InsertImage( nIndex, ImageAni );
	}

	void SQRRichText::Updata()
	{
		SetWndText( GetWndText() );
	}

	SQRWnd* SQRRichText::GetNameWnd(char *name)
	{
		for (size_t i = 0; i < mClrWnd.size(); ++i)
		{
			if (true == mClrWnd[i]->mWnd.IsShow() && 0 == strcmp(mClrWnd[i]->str.c_str(), name))
			{
				return &mClrWnd[i]->mWnd;
			}
		}
		return NULL;
	}

	void SQRRichText::AdjustAfterContentChanged()
	{
		m_sbVertical.SetRange( int32(m_pRichString->GetCurHeight() + GetYBorder(GetStyle())*2) );
		m_sbVertical.SetPage( int32(GetWndHeight()) );
		// 当为滚动条下粘滞风格时，若滑块在最下面或者窗口内容不超过一屏时，重置滑块的位置，这将导致内容自动向下翻滚
		if((GetStyle() & RCHW_STICK_BOTTOM) && 
			(Isbottom() &&  m_sbVertical.GetRange() - m_sbVertical.GetPage() <= (int32)m_pRichString->GetCurHeight()))
		{
			m_sbVertical.SetPos( int32(m_pRichString->GetCurHeight()) );
		}
		else if( (GetStyle() & RCHW_STICK_BOTTOM) && (GetWndHeight() - m_fLastCurHeight ) >= 0  && (m_pRichString->GetCurHeight() - GetWndHeight()) >= 0 ) 
		{
			m_sbVertical.SetPos( int32(m_pRichString->GetCurHeight()) );
		}
		m_fLastCurHeight = m_pRichString->GetCurHeight();
		//SetMouseWheel(m_sbVertical.GetRange() > m_sbVertical.GetPage());
		SetMouseWheel(true);
		if( GetStyle() & RCHW_AUTORESETH )
		{
			CFRect rt;
			GetWndRect( rt );
			rt.bottom = rt.top + max( GetFontSize(), m_pRichString->GetCurHeight() ) + GetYBorder( GetStyle() )*2*GetRootZoomSize();;
			SetWndRect( rt );
		}
	}

	void SQRRichText::SetWndText( const char* str )
	{
		SQR_TRY
		{
			if(str == NULL)
				str = "";
			// 将字符串中的“\n”替换成“#r”以处理多行显示问题
			string	utf8_str(str);
			wstring	utf16_str;
			utf8_to_utf16(utf16_str, utf8_str);
			replace(utf16_str, L"\n", L"#r");

			float Zoom = GetRootZoomSize();
			SQRWnd::SetWndText( utf8_str.c_str() );
			if( ( (GetStyle() & RCHW_AUTOSETRECT_DOWN) ) && wcscmp(utf16_str.c_str(), L"") != 0 )
			{
				CFRect rt;
				GetWndRect( rt );
				rt.right = rt.left + (GetMaxWidth() + GetXBorder( GetStyle())*2*Zoom);
				SetWndRect( rt );
			}
			float fRichStringWidth = GetWndWidth() - GetXBorder( GetStyle() )*2*Zoom;
			m_pRichString->SetWidth( fRichStringWidth );
			m_pRichString->SetMaxWidth( fRichStringWidth );
			m_pRichString->SetText( this, utf16_str.c_str() );	

			if( (GetStyle() & RCHW_AUTOSETRECT_DOWN) && wcscmp(utf16_str.c_str(), L"") != 0 )
			{
				CFRect rt;
				GetWndRect( rt );
				float oldBottom = rt.bottom;
				float newWidth = m_pRichString->GetMaxWidth() + GetXBorder( GetStyle() )*2*Zoom;
				if (newWidth < rt.Width())
					newWidth = min((newWidth + 1.0f), rt.Width());
				rt.right = rt.left + max(newWidth, GetMinWidth());

				float minHeight = max(m_pRichString->GetCurHeight(), GetMinHeight());
				rt.bottom = rt.top + minHeight + GetYBorder( GetStyle() )*2*Zoom;
				
				CFRect rtParent;
				GetParent()->GetWndRect(rtParent);
				rtParent.bottom += rt.bottom - oldBottom;
				GetParent()->SetWndRect(rtParent);
				SetWndRect( rt );
			}
			AdjustAfterContentChanged();

		}
		SQR_CATCH(exp)
		{
			stringstream errmsg;
			errmsg << exp.ErrorMsg();
			SQR_TRY
			{
				errmsg << "SQRRichText text:" << str << endl;
				errmsg << "SQRRichText GUI manager pointer:" << GetWndHandle()->m_pGUI << endl;
				errmsg << "GUI inst manager pointer:" <<  GetRootWnd()->GetWndHandle()->m_pGUI << endl;
			}
			SQR_CATCH(exp)
			{
				errmsg << "Head info dirty" << exp.ErrorMsg() << endl;
			}
			SQR_TRY_END;
			
			exp.AppendMsg(errmsg.str());
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}

	void SQRRichText::AddWndText( const char* str )
	{
		if(str == NULL)
			return;
		m_pRichString->SetWidth( GetWndWidth() - GetXBorder( GetStyle() )*2);
		// 将字符串中的“\n”替换成“#r”以处理多行显示问题
		string	utf8_str(str);
		wstring	utf16_str;
		replace(utf8_str, "\n", "#r");
		utf8_to_utf16(utf16_str, utf8_str);
		utf16_str.erase(m_pRichString->AddText( this, utf16_str.c_str()));
		string newUtf8_str;
		utf16_to_utf8(newUtf8_str, utf16_str);
		AppendWndText(newUtf8_str);
		AdjustAfterContentChanged();
	}

	void SQRRichText::SetToAutoFormat(bool b)
	{
		m_pRichString->SetBeAutoFormat( b );
	}

	void SQRRichText::SetWndTextXR( bool b)
	{
		m_pRichString->SetWndTextXR(b);
	}

	void SQRRichText::AddNewLineText(const char* str)
	{
		UIString text("#r");
		if(str)
			text.append(str);
		this->AddWndText(text.c_str());
	}

	void SQRRichText::SetWndRect( const CFRect& rt )
	{
		SQRWnd::SetWndRect( rt );
		float Zoom = GetRootZoomSize();
		m_pRichString->SetWidth( GetWndWidth() - GetXBorder( GetStyle() )* 2 * Zoom);

		wstring utf16_str;
		utf8_to_utf16(utf16_str, GetWndText());

		m_pRichString->SetText( this, utf16_str.c_str() );
		m_sbVertical.SetRange( int32(m_pRichString->GetCurHeight() + GetYBorder(GetStyle())*2 * Zoom) );
		m_sbVertical.SetPage( int32(GetWndHeight()) );
	}

	void SQRRichText::ResizeRect( const CFRect& rt, bool refreshbak  )
	{
		float zoomSize = GetRootZoomSize();	
		m_fMinWidth = m_fInitMinWidth * zoomSize;
		m_fMaxWidth = m_fInitMaxWidth * zoomSize;
		GetFont()->Resize(m_hWnd->m_FontSize*zoomSize);
		SQRWnd::ResizeRect( rt, refreshbak );
		SetWndText( GetWndText() );
	}

	bool SQRRichText::CheckDis(float wb, float hb, float w, float h)
	{
		if ( (wb - w) * (wb - w) > 0.99999f || (hb - h) * (hb - h) > 0.99999f)
			return true;
		return false;
	}

	void SQRRichText::_SetWndRect( const CFRect& rt )
	{
		SQRWnd::_SetWndRect( rt );
		m_pRichString->SetWidth( GetWndWidth() - GetXBorder( GetStyle() )*2);

		wstring utf16_str;
		utf8_to_utf16(utf16_str, GetWndText());

		m_pRichString->SetText( this, utf16_str.c_str() );
		m_sbVertical.SetRange( int32(m_pRichString->GetCurHeight() + GetYBorder(GetStyle())*2) );
		m_sbVertical.SetPage( int32(GetWndHeight()) );
	}

	void SQRRichText::_ResizeRect( const CFRect& rt, bool refreshbak  )
	{
		float zoomSize = GetRootZoomSize();	
		m_fMinWidth = m_fInitMinWidth * zoomSize;
		m_fMaxWidth = m_fInitMaxWidth * zoomSize;
		SQR_TRY
		{
			GetFont()->Resize(m_hWnd->m_FontSize*zoomSize);
		}
		SQR_CATCH(exp)
		{
			stringstream errmsg;
			errmsg << exp.ErrorMsg() << endl;
			errmsg << "font size:" << m_hWnd->m_FontSize << " zoom size:" << zoomSize << endl;

			exp.AppendMsg(errmsg.str());
			GfkLogExp(exp);
		}
		SQR_TRY_END;
		
		SQRWnd::_ResizeRect( rt, refreshbak );
		SetWndText( GetWndText() );
	}

	void SQRRichText::SetStyle( uint32 Style )
	{
		SQRWnd::SetStyle( Style );
		// xyt根据是否到底部的时候来显示滑条
		bool bShowVertical = (GetStyle() & WS_VERSCROLL) != 0;
		m_sbVertical.ShowWnd( bShowVertical );
		//SetMouseWheel( bShowVertical );
		// 如果是滚动条下粘滞，则将滚动条初始位置放到最下面
		if(GetStyle() & RCHW_STICK_BOTTOM)
			m_sbVertical.SetPos(m_sbVertical.GetRange());
	}

	void SQRRichText::DrawWndText()
	{
		if (m_hWnd->m_WndFont)
			m_hWnd->m_WndFont->SetAlignToPixel(m_bAlignToPixel);
		
		SQR_TRY
		{
			m_pRichString->Draw( this, 0, -(float)m_sbVertical.GetPos() );
		}
		SQR_CATCH(exp)
		{
			stringstream errmsg;
			errmsg << exp.ErrorMsg() << endl;
			errmsg << "Rich string pointer:" << m_pRichString << endl;

			exp.AppendMsg(errmsg.str());
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}

	void SQRRichText::OnChar( uint32 nChar, uint32 nFlags )
	{
		if(GetStyle() & RCHW_ENABLE_SELECT)
		{
			const char c = static_cast<char>(nChar);
			if(c == CTRL_C)
				CtrlC();
		}
	}

	void SQRRichText::CtrlC()
	{	
		wstring utf16_str = m_pRichString->GetHighLightText();
		string utf8_str;
		utf8_str = utf16_to_utf8(utf16_str);
		if(utf8_str.empty())
			return;
		if( OpenClipboard( NULL ) )
		{
			EmptyClipboard();
			HANDLE hMem = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, utf8_str.size() + 1 );
			char* buf = (char*)GlobalLock( hMem );
			buf[utf8_str.size()] = 0;
			memcpy( buf, utf8_str.c_str(), utf8_str.size());
			GlobalUnlock( hMem );
			SetClipboardData( CF_TEXT, hMem );
			CloseClipboard();
			GlobalFree( hMem );
		} 
	}

	void SQRRichText::DrawText( const wstring& text, float x, float y, CColor color, CColor bkColor, bool multiline, CFRect* rect, float curLineLenght )
	{
		float Zoom = GetRootZoomSize();
		float dx = GetXBorder( GetStyle() ) * Zoom;
		float dy = GetYBorder( GetStyle() ) * Zoom;
		CFRect rtWnd = rect ? *rect : CFRect( dx, dy, GetWndWidth() - dx, GetWndHeight() - dy );

		if(GetStyle() & WS_TEXTXC  || GetStyle() & WS_TEXTXR || GetStyle() & WS_TEXTYC || GetStyle() & WS_TEXTYB)
		{
			float Width = curLineLenght + dx*2; 
			float Height = m_pRichString->GetCurHeight()+dy*2;

			if( (WS_MULLINETEXT&GetStyle()) == WS_MULLINETEXT )
				return SQRWnd::DrawText( text, 0, 0, color, bkColor, true, NULL );
			else
			{
				if( GetStyle() & WS_TEXTXC )
					x += m_hWnd->m_Width/2 - Width/2;
				else if( GetStyle() & WS_TEXTXR )
					x += m_hWnd->m_Width - Width;
				if( GetStyle() & WS_TEXTYC )
					y += m_hWnd->m_Height/2 - Height/2;
				else if( GetStyle() & WS_TEXTYB )
					y += m_hWnd->m_Height - Height;

				return SQRWnd::DrawText( text, x, y, color, bkColor, true, &rtWnd );
			}
		}

		return SQRWnd::DrawText( text, x, y, color, bkColor, multiline, &rtWnd );
	}

	void SQRRichText::OnLButtonClick( uint32 nFlags, int32 x, int32 y )
	{
		int32 nClickID = m_pRichString->Click( float(x), float(y + m_sbVertical.GetPos()), GetStyle(), m_hWnd->m_Width,m_hWnd->m_Height, RICH_CLICK);

		if( nClickID != -1 )
			SendCtrlMsg( RICH_CLICK, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
		else
			SendCtrlMsg( RICHWND_LCLICKUP, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y);
	}

	//void SQRRichText::OnLButtonUp( uint32 nFlags, int32 x, int32 y )
	//{
	//	int32 nClickID = m_pRichString->Click( float(x), float(y + m_sbVertical.GetPos()) );
	//	if( nClickID != -1 )
	//		SendCtrlMsg( RICH_CLICK, GetCursorPos().x, GetCursorPos().y );
	//	else
	//		SendCtrlMsg( RICHWND_LCLICKUP, GetCursorPos().x, GetCursorPos().y);
	//}

	void SQRRichText::OnLButtonDown(uint32 nFlags,int32 x, int32 y)
	{
		if(GetStyle() & RCHW_ENABLE_SELECT)
			m_pRichString->OnHighLightBegin(x, y);

		int32 nClickID = m_pRichString->Click( float(x), float(y + m_sbVertical.GetPos()), GetStyle(), m_hWnd->m_Width, m_hWnd->m_Height, RICHWND_LCLICKDOWN);
		if (nClickID == -1)
			SendCtrlMsg( RICHWND_LCLICKDOWN, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y);
	}
	void SQRRichText::OnDrag( const CPos& HeldPoint, const CPos& CurPoint )
	{
		if(GetStyle() & RCHW_ENABLE_SELECT)
			m_pRichString->OnHighLightEnd(CurPoint.x, CurPoint.y);
	}

	void SQRRichText::OnRButtonUp(uint32 nFlags,int32 x, int32 y)
	{
		int32 nClickID = m_pRichString->Click( float(x), float(y + m_sbVertical.GetPos()),GetStyle(), m_hWnd->m_Width ,m_hWnd->m_Height, RICHWND_RCLICKUP);
		if (nClickID == -1)
			SendCtrlMsg( RICHWND_RCLICKUP, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y);
	}

	void SQRRichText::OnRButtonClick(uint32 nFlags,int32 x, int32 y)
	{
		int32 nClickID = m_pRichString->Click( float(x), float(y + m_sbVertical.GetPos()),GetStyle(), m_hWnd->m_Width,m_hWnd->m_Height, RICHWND_RCLICKUP);
		if (nClickID == -1)
			SendCtrlMsg( RICHWND_RCLICKUP, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y);
	}

	void SQRRichText::OnRButtonDown(uint32 nFlags, int32 x, int32 y )
	{
		int32 nClickID = m_pRichString->Click( float(x), float(y + m_sbVertical.GetPos()),GetStyle(), m_hWnd->m_Width,m_hWnd->m_Height,RICHWND_RCLICKDOWN) ;
		if( nClickID != -1 )
			SendCtrlMsg( RICH_RCLICK, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
		else
			SendCtrlMsg( RICHWND_RCLICKDOWN, (uint32)GetLogicCursorPos().x, (uint32)GetLogicCursorPos().y );
	}

	void SQRRichText::OnMouseMove( uint32 nFlags, int32 x, int32 y )
	{
		if ( nFlags&MK_LBUTTON || nFlags&MK_RBUTTON )
			m_pRichString->Click( float(x), float(y + m_sbVertical.GetPos()),GetStyle(), m_hWnd->m_Width,m_hWnd->m_Height,RICHWND_MOUSEOVERDOWN) ;
		else
			m_pRichString->Click( float(x), float(y + m_sbVertical.GetPos()),GetStyle(), m_hWnd->m_Width,m_hWnd->m_Height,RICHWND_MOUSEOVER) ;
		SQRWnd::OnMouseMove( nFlags, x, y );
	}

	void SQRRichText::OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y )
	{
		if ( (GetStyle() & WS_IGNOREWHEEL) )
			SQRWnd::OnMouseWheel( nFlags, zDelta, x, y );
		else
			m_sbVertical.SendMsg( WM_MOUSEWHEEL, MAKELONG( nFlags, zDelta ), MAKELONG( x, y ) );
	}

	WND_IMAGE_LIST* SQRRichText::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
	{
		IMAGE_PARAM Flag = ImageFlag;
		if( 0 == Flag.CtrlMask )
			return NULL;
		else if( SM_RW_BK == Flag.CtrlMask )
		{
			Flag.CtrlMask -= SM_RW_BK - SM_BS_BK;
			return SQRWnd::GetWndBkImage( Flag );
		}
		else
		{
			Flag.CtrlMask -= SM_RW_SCV_BK - SM_SC_BK;
			return m_sbVertical.GetWndBkImage( Flag );
		}
	}

	bool SQRRichText::ObjHasEventState(EventStateMask Mask)
	{
		return SQRRichText::IsHasEventState(Mask);
	}

	bool SQRRichText::IsHasEventState(EventStateMask Mask)
	{
		if(	IP_MASK			== Mask ||
			IP_MOUSEDESC	== Mask ||
			IP_ENABLE		== Mask ||
			IP_DISABLE		== Mask ||
			IP_MOUSEOVER		== Mask ||
			IP_CLICKDOWN		== Mask ||
			IP_UNCHECK_ENABLE	== Mask ||
			IP_UNCHECK_DISABLE	== Mask ||
			IP_UNCHECK_MOUSEOVER== Mask ||
			IP_UNCHECK_CLICKDOWN== Mask ||
			IP_FILL				== Mask ||
			IP_TR_ENABLE		== Mask ||
			IP_TR_DISABLE		== Mask ||
			IP_LS_SEL			== Mask ||
			IP_LS_UNSEL			== Mask )
			return true;
		return false;
	}

	uint32 SQRRichText::GetWndTextColor( const IMAGE_PARAM& ImageFlag )
	{
		IMAGE_PARAM Flag = ImageFlag;
		if( 0 == Flag.CtrlMask )
			return NULL;
		else if( SM_RW_BK == Flag.CtrlMask )
		{
			Flag.CtrlMask -= SM_RW_BK - SM_BS_BK;
			return SQRWnd::GetWndTextColor( Flag );
		}
		else
		{
			Flag.CtrlMask -= SM_RW_SCV_BK - SM_SC_BK;
			return m_sbVertical.GetWndTextColor( Flag );
		}
	}

	uint32 SQRRichText::GetWndTextBcColor( const IMAGE_PARAM& ImageFlag )
	{
		IMAGE_PARAM Flag = ImageFlag;
		if( 0 == Flag.CtrlMask )
			return NULL;
		else if( SM_RW_BK == Flag.CtrlMask )
		{
			Flag.CtrlMask -= SM_RW_BK - SM_BS_BK;
			return SQRWnd::GetWndTextBcColor( Flag );
		}
		else
		{
			Flag.CtrlMask -= SM_RW_SCV_BK - SM_SC_BK;
			return m_sbVertical.GetWndTextBcColor( Flag );
		}
	}

	void SQRRichText::SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
	{
		IMAGE_PARAM Flag = ImageFlag;
		if( SM_RW_BK == Flag.CtrlMask )
		{
			Flag.CtrlMask -= SM_RW_BK - SM_BS_BK;
			return SQRWnd::SetWndTextColor( Flag, uColor );
		}
		else
		{
			Flag.CtrlMask -= SM_RW_SCV_BK - SM_SC_BK;
			return m_sbVertical.SetWndTextColor( Flag, uColor );
		}
	}

	void SQRRichText::SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
	{
		IMAGE_PARAM Flag = ImageFlag;
		if( SM_RW_BK == Flag.CtrlMask )
		{
			Flag.CtrlMask -= SM_RW_BK - SM_BS_BK;
			return SQRWnd::SetWndTextBcColor( Flag, uColor );
		}
		else
		{
			Flag.CtrlMask -= SM_RW_SCV_BK - SM_SC_BK;
			return m_sbVertical.SetWndTextBcColor( Flag, uColor );
		}
	}

	SQRWnd* SQRRichText::GetResWnd( int32 nCtrlMask )
	{
		if( 0 == nCtrlMask )
			return NULL;
		else if( SM_RW_BK == nCtrlMask )
		{
			nCtrlMask -= SM_RW_BK - SM_BS_BK;
			return SQRWnd::GetResWnd( nCtrlMask );
		}
		else
		{
			nCtrlMask -= SM_RW_SCV_BK - SM_SC_BK;
			return m_sbVertical.GetResWnd( nCtrlMask );
		}
	}
	void SQRRichText::SetAutoWidth(float fMinWidth, float fMaxWidth)
	{
		float zoomSize = GetRootZoomSize();
		m_fInitMaxWidth = fMaxWidth;
		m_fInitMinWidth = fMinWidth;
		m_fMinWidth = m_fInitMinWidth * zoomSize;
		m_fMaxWidth = m_fInitMaxWidth * zoomSize;
	}


	SQRRichString* SQRRichText::GetRichStringInfo()
	{
		return m_pRichString;
	}

	float SQRRichText::GetMaxWidth()
	{
		return m_fMaxWidth;
	}

	float SQRRichText::GetMinWidth()
	{
		return m_fMinWidth;
	}

	void SQRRichText::SetRowDist(float rowDist)
	{
		m_pRichString->SetRowDist(rowDist);
	}

	void SQRRichText::SetMinHeight(float fMinHeight )
	{
		m_fMinHeight = fMinHeight; 
	}
	float SQRRichText::GetMinHeight()
	{
		return m_fMinHeight * m_hWnd->m_pGUI->GetZoomSize() * m_WndZoom;
	}

	void SQRRichText::SetPageUp()
	{
		int32 newPos = m_sbVertical.GetPos() - m_sbVertical.GetPage();
		m_sbVertical.SetPos( newPos, true );
	}

	void SQRRichText::SetPageDown()
	{
		int32 newPos = m_sbVertical.GetPos() + m_sbVertical.GetPage();
		m_sbVertical.SetPos( newPos, false );
	}

	void SQRRichText::SetPageBottom()
	{
		m_sbVertical.SetPos(m_sbVertical.GetRange(), false);
	}

	CFRect sqr::SQRRichText::getTextDrawArea()
	{
		CFRect drawArea( 0.0f, 0.0f, 
			GetWndWidth(), GetWndHeight() 
			);
		float xBorder = static_cast<float>(GetXBorder(GetStyle()));
		float yBorder = static_cast<float>(GetYBorder(GetStyle()));
		drawArea.left	+= xBorder;
		drawArea.right	-= xBorder;
		drawArea.top	+= yBorder;
		drawArea.bottom	-= yBorder;

		return drawArea;
	}

	bool SQRRichText::Isbottom() const
	{
		return m_sbVertical.Isbottom();
	}

	const char* SQRRichText::GetChooseStr()
	{
		m_szChooseStr = utf16_to_utf8(m_pRichString->GetChooseStr()).c_str();
		return m_szChooseStr.c_str();
	}

	const char* SQRRichText::GetChooseStrAttr()
	{
		m_szChooseStrAttr = utf16_to_utf8(m_pRichString->GetChooseStrAttr()).c_str();
		m_szChooseStrAttr  = MemH64_to_mbcs(m_szChooseStrAttr.c_str()).c_str();
		return m_szChooseStrAttr.c_str();
	}

	void SQRRichText::SetShowScrollBar(bool bvalue)
	{
		m_sbVertical.ShowWnd(bvalue);
	}
}
