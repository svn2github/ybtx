#include "stdafx.h"
#include "SQREdit.h"
#include "SQRGUIManager.h"
#include "CodeCvs.h"
#include "CRenderFont.h"
#include "GUITextUtils.h"
#include "StringHelper.h"
#include "TSqrAllocator.inl"


namespace sqr
{
	inline bool IsWordChar( char n )
	{
		return is_alnum(n);// || n < 0;
	}

	inline bool IsWordChar( wchar_t n )
	{
		return is_alnum(n);// || n < 0;
	}

	//////////////////////////////////////////////////////////////////////////

	DYNAMIC_CREATE_WND( SQREdit, SQRWnd )

	const UIWString	SQREdit::m_lineBreakChars(L"\n");
	const float		SQREdit::m_fCaratWidth(2.0f);

	SQREdit::SQREdit(void)
		: m_uMaxTextLen( uint32(-1) )
		, m_ShowTime(GetProcessTime())
		, m_IsSendMsgToParent(true)
		, m_caratIndex(0)
		, m_selectStart(0)
		, m_selectEnd(0)
		, m_dragAnchorIdx(0)
		, m_widestExtent(0.0f)
		, m_bOnCommand(false)
		, m_bEnterSend(false)
	{
		// 再文本末尾加上一个换行,目的是方便换行算法的实现
		//GetEditString().append(1, L'\n');
	}

	SQREdit::~SQREdit(void)
	{
	}

	int32 SQREdit::Create( const WndCreateParam& param )
	{
		WndCreateParam newParam = param;
		newParam.uStyle |= WS_CHILD;
		if ( FAILED( SQRWnd::Create( newParam ) ) )
			return GERROR;

		WndCreateParam hParam = param;
		hParam.szTitle = "";
		hParam.uStyle = SS_HSCROLL;
		hParam.x = 0;
		hParam.y = GetWndHeight();
		hParam.width = GetWndWidth();
		hParam.height = 16;
		hParam.pParentWnd = this;
		m_sbHorizon.Create( hParam );
		m_sbHorizon.SetLockFlag( LOCK_BOTTOM|LOCK_LEFT|LOCK_RIGHT );

		WndCreateParam vParam = param;
		vParam.szTitle = "";
		vParam.uStyle = 0;
		vParam.x = GetWndWidth();
		vParam.y = 0;
		vParam.width = 16;
		vParam.height = GetWndHeight();
		vParam.pParentWnd = this;
		m_sbVertical.Create( vParam );
		m_sbVertical.SetLockFlag( LOCK_RIGHT|LOCK_TOP|LOCK_BOTTOM );

		m_sbHorizon.SetRange( 1 );
		m_sbHorizon.SetPage( 1 );
		m_sbHorizon.SetPos( 0 );

		m_sbVertical.SetRange( 1 );
		m_sbVertical.SetPage( 1 );
		m_sbVertical.SetPos( 0 );

		m_sbHorizon.ShowWnd( (GetStyle()&WS_HORSCROLL) != 0 );
		m_sbVertical.ShowWnd( (GetStyle()&WS_VERSCROLL) != 0 );

		UIString 
		
		SetWndText( SQRWnd::GetWndText() );
		SetWndCursor( IDC_IBEAM );
		if ( GetStyle()&ECS_PASSWORD )
			DisableIme( true );

		return 0;
	}


	const char* SQREdit::GetWndText()
	{
		if ( GetStyle() & ECS_MULTILINE )
		{
			SQRWnd::GetWndText();
			if ( (m_hWnd->m_WindowTitle.length() != 0) && 
				(m_hWnd->m_WindowTitle[m_hWnd->m_WindowTitle.length() - 1] != '\n') )
			{
				return m_hWnd->m_WindowTitle.c_str();
			}
			else if ( m_hWnd->m_WindowTitle.length() == 0 )
			{
				return m_hWnd->m_WindowTitle.c_str();
			}
			else
			{
				m_hWnd->m_WindowTitle = m_hWnd->m_WindowTitle.substr(0, m_hWnd->m_WindowTitle.length() - 1);
				return m_hWnd->m_WindowTitle.c_str();
			}
		}
		else
			return SQRWnd::GetWndText();
	}

	int32 SQREdit::DestroyWnd()
	{
		if ( GetStyle()&ECS_DISABLEIME )
			SQRGUIManager::GetGUI( this )->EnableIme( true );
		return SQRWnd::DestroyWnd();
	}

	void SQREdit::SetWndText( const char* str )
	{
		UIWString& windowText = GetEditString();
		wstring tstr  = utf8_to_utf16(str).c_str();

		// 非多行模式下，将所有的回车符删除
		
		if ( !( GetStyle() & ECS_MULTILINE ) )
			erase(tstr, L"\n");

		m_hWnd->m_WindowTitle = utf16_to_utf8(tstr).c_str();
		windowText = tstr.c_str();

		formatText();

		m_caratIndex = windowText.length();
		onTextChanged();

		SetCurShow();

		SetSelection(0, 0);
	}

	void SQREdit::SetWndRect( const CFRect& rt )
	{
		SQRWnd::SetWndRect( rt );
		configureScrollbars();
	}

	void SQREdit::ResizeRect( const CFRect& rt , bool refreshbak )
	{
		SQRWnd::ResizeRect( rt ,refreshbak);
		configureScrollbars();
	}

	void SQREdit::_SetWndRect( const CFRect& rt )
	{
		SQRWnd::_SetWndRect( rt );
		configureScrollbars();
	}

	void SQREdit::_ResizeRect( const CFRect& rt , bool refreshbak )
	{
		SQRWnd::_ResizeRect( rt ,refreshbak);
		configureScrollbars();
	}

	void SQREdit::EnableWnd( bool bEnable )
	{
		if ( bEnable == m_hWnd->m_bEnable )
			return;
		configureScrollbars();

		SQRWnd::EnableWnd( bEnable );
	}

	void SQREdit::ShowWnd( bool bShow )
	{
		SetCurShow();
		configureScrollbars();
		if ( !bShow && m_hWnd->m_pGUI->IsFocus(this) )
			LoseFocusByShowFalse();
		SQRWnd::ShowWnd( bShow );
	}

	void SQREdit::GetCutAreaRect( CFRect& rt )
	{
		StyleStruct* pStyle = GetStyleStruct();
		uint32 Style = m_hWnd->m_StyleIndex;
		GUI_BLOCK* pBlock = NULL;

		GetWndRect(rt);
		if ( Style>0 && NULL != pStyle )
		{
			pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
			if(pBlock == NULL)
				pBlock = pStyle->m_vecPool[IP_ENABLE][1];
			if( pBlock )
			{
				GUI_BLOCK& Block = *pBlock;
				float t_Zoom = m_hWnd->m_pGUI->GetZoomSize();
				float TopLeftH		= Block[GBM_TOP_LEFT].Height()* t_Zoom;
				float TopLeftW		= Block[GBM_TOP_LEFT].Width() * t_Zoom;
				float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width() * t_Zoom;
				float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height()* t_Zoom;

				rt.left += TopLeftW;
				rt.top += TopLeftH;
				rt.bottom -= BottomRightH;
				rt.right -= BottomRightW;
			}
		}
	}


	bool SQREdit::IsHasEventState(EventStateMask Mask)
	{
		return SQRWnd::IsHasEventState(Mask);
	}

	bool SQREdit::ObjHasEventState(EventStateMask Mask)
	{
		return SQREdit::IsHasEventState(Mask);
	}

	size_t SQREdit::getNextTokenLength(const wstring &text, size_t start_idx) const
	{
		size_t pos = text.find_first_of(GUITextUtils::DefaultWrapDelimiters.c_str(), start_idx);

		// 处理找不到空格符的情况(表明这是最后一个token)
		if ( pos == UIWString::npos )
		{
			return (text.length() - start_idx);
		}
		// 处理分隔符
		else if ( (pos - start_idx) == 0 )
		{
			return 1;
		}
		else
		{
			return (pos - start_idx);
		}
	}

	void SQREdit::formatText()
	{
		// clear old format data
		m_lines.clear();
		m_widestExtent = 0.0f;

		wstring		 paraText;
		CRenderFont	*fnt = GetFont();

		if ( fnt )
		{
			float areaWidth = getTextDrawArea().Width();

			UIWString::size_type	currPos = 0;
			UIWString::size_type	paraLen = 0;
			LineInfo			line;

			wstring text = GetStyle() & ECS_PASSWORD ? m_szPassWord.c_str() : GetEditString().c_str();
			while ( currPos < text.length() )
			{
				// 以"\n"划分段落(paraText)
				if ( (paraLen = text.find_first_of(m_lineBreakChars.c_str(), currPos)) == wstring::npos )
					paraLen = text.length() - currPos;
				else
					++paraLen -= currPos;

				paraText = text.substr(currPos, paraLen);

				// ---- 单行模式 
				if ( !(GetStyle() & ECS_MULTILINE) )
				{
					line.startIdx	= currPos;
					line.length		= paraLen;
					line.extent		= getTextPixelLen(paraText.c_str());
					m_lines.push_back(line);

					if ( line.extent > m_widestExtent )
						m_widestExtent = line.extent;
				}
				// ---- 多行模式
				else if ( !(GetStyle() & WS_HORSCROLL))
				{
					SetMouseWheel(true);
					size_t paraIndex = 0;	// 段落中的索引
					
					// 在段落(paraText)中遍历
					while ( paraIndex < paraLen )
					{
						size_t lineLen = 0;
						float lineExtent = 0.0f;		// 保存当前的行像素宽度

						while ( lineLen < (paraLen-paraIndex) )
						{
							// 取得下一个token的字符数
							size_t nextTokenLen = getNextTokenLength(paraText, paraIndex + lineLen);

							// 取得token的像素宽度
							float tokenExtent = getTextPixelLen(paraText.substr(paraIndex + lineLen, nextTokenLen).c_str());

							// 检测加上这个字符后当前行是否超出显示范围
							if ( (lineExtent+tokenExtent) > areaWidth )
							{
								lineLen = fnt->GetCharAtPixel(paraText.substr(paraIndex, paraLen).c_str(), areaWidth);
								break;
							}

							lineLen		+= nextTokenLen;
							lineExtent	+= tokenExtent;
						}
						
						// 设置并保存当前行信息
						line.startIdx	= currPos + paraIndex;
						line.length		= lineLen;
						line.extent		= lineExtent;
						m_lines.push_back(line);

						if ( line.extent > m_widestExtent )
							m_widestExtent = line.extent;

						paraIndex += lineLen;
					}
				}

				currPos += paraLen;
			}
		}

		configureScrollbars();
	}

	void SQREdit::DrawWndText()
	{
		if ( GetStyle() & ECS_PASSWORD )
		{
			m_szPassWord = SQRWnd::GetWndTextW();
			// 密码符号替换
			for ( size_t i = 0; i < m_szPassWord.size(); i++ )
			{
				if (m_szPassWord[i] != L'\n')
					m_szPassWord[i] = L'*';
			}
			formatText();
		}

	#ifdef _DEBUG
		UIWString& windowText = GetEditString();
		if (!( GetStyle() & ECS_MULTILINE ))
		{
			Ast(contains(windowText.c_str(), L"\n") == false);
		}
	#endif

		CFRect drawArea = getTextDrawArea();

		// 画文字		
		drawTextLines(drawArea);

		// 画光标
		if ( IsActive() && ( GetProcessTime() - m_ShowTime )%1000 < 500 )
			drawCarat(drawArea);
	}

	void SQREdit::SetCurShow()
	{
		UIWString& windowText = GetEditString();
		//超长则裁剪
		if ( windowText.size() > m_uMaxTextLen )
		{
			GetEditString().erase(m_uMaxTextLen, UIWString::npos);
			formatText();
			setCaratIndex(GetEditString().length());
		}
	}

	void SQREdit::configureScrollbars()
	{
		SQRScrollBar* vertScrollbar = GetVertScrollbar();
		SQRScrollBar* horzScrollbar = GetHorzScrollbar();
		float totalHeight	= (float)m_lines.size() * GetFont()->GetLineSpacing();
		float widestItem	= m_widestExtent;

		//
		// First show or hide the scroll bars as needed (or requested)
		//
		// show or hide vertical scroll bar as required (or as specified by option)
		if ( (totalHeight > getTextDrawArea().Height()) && (GetStyle() & WS_VERSCROLL) )
		{
			vertScrollbar->ShowWnd(true);

			// show or hide horizontal scroll bar as required (or as specified by option)
			if ( (widestItem > getTextDrawArea().Width()) && (GetStyle() & WS_HORSCROLL) )
			{
				horzScrollbar->ShowWnd(true);
			}
			else
			{
				horzScrollbar->ShowWnd(false);
			}

		}
		else
		{
			// show or hide horizontal scroll bar as required (or as specified by option)
			if ( (widestItem > getTextDrawArea().Width()) && (GetStyle() & WS_HORSCROLL) )
			{
				horzScrollbar->ShowWnd(true);

				// show or hide vertical scroll bar as required (or as specified by option)
				if ( (totalHeight > getTextDrawArea().Height()) && (GetStyle() & WS_VERSCROLL) )
				{
					vertScrollbar->ShowWnd(true);
				}
				else
				{
					vertScrollbar->ShowWnd(false);
				}

			}
			else
			{
				vertScrollbar->ShowWnd(false);
				horzScrollbar->ShowWnd(false);
			}

		}

		//
		// Set up scroll bar values
		//
		CFRect renderArea(getTextDrawArea());

		vertScrollbar->SetRange((int32)totalHeight);
		vertScrollbar->SetPage((int32)renderArea.Height());
		vertScrollbar->SetPos((int32)vertScrollbar->GetPos());

		horzScrollbar->SetRange((int32)widestItem);
		horzScrollbar->SetPage((int32)renderArea.Width());
		horzScrollbar->SetPos((int32)horzScrollbar->GetPos());
	}

	int32 SQREdit::GetLineCount( int32* pMaxLineLength )
	{
		if ( pMaxLineLength != NULL )
		{
			*pMaxLineLength = 0;
			for ( size_t i=0; i<m_lines.size(); ++i )
			{
				if ( m_lines[i].length > (size_t)(*pMaxLineLength) )
					*pMaxLineLength = m_lines[i].length;
			}
		}

		return m_lines.size();
	}

	void SQREdit::CtrlC()
	{
		if ( OpenClipboard( NULL ) )
		{
			EmptyClipboard();
			
			wstring utf16_str = GetEditString().substr(getSelectionStartIndex(), getSelectionLength()).c_str();
			int32	nStrLen = utf16_str.length()*2 + 2;
			char*	pStrBuf = (char*)VirtualAlloc(0, nStrLen, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			if ( pStrBuf == 0 )
			{
				CloseClipboard();
				return;
			}

			CharToOemBuffW( utf16_str.c_str(), pStrBuf, nStrLen );
			
			if ( strlen(pStrBuf) != 0 && pStrBuf[nStrLen-1] == '\n' )
				pStrBuf[nStrLen-1] = '\0';

			HANDLE hMem = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, nStrLen );
			char* Buf = (char*)GlobalLock( hMem );
			memset(Buf, 0, nStrLen);
			memcpy( Buf, pStrBuf, nStrLen);
			GlobalUnlock( hMem );
			SetClipboardData( CF_TEXT, hMem );
			CloseClipboard();
			GlobalFree( hMem );

			VirtualFree(pStrBuf, 0, MEM_RELEASE);
		}
	}

	void SQREdit::CtrlV()
	{
		if ( OpenClipboard( NULL ) )
		{
			eraseSelectedText();
			char* Buf = (char*)GetClipboardData( CF_TEXT );
			if ( Buf != NULL )
			{
				int32 utf16_str_size = strlen(Buf)*2 + 2;
				wchar_t* utf16_str = (wchar_t*)VirtualAlloc(0, utf16_str_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
				if ( utf16_str == 0 )
				{
					CloseClipboard();
					return;
				}
				
				OemToCharW(Buf, utf16_str);

				UIString  utf8_str = utf16_to_utf8(utf16_str).c_str();

				//int32 index = find_sbc_on_utf8( utf8_str.c_str() );
				//if ( index > 0 )
				//{
				//	MessageBox(NULL, "输入的字符中含有禁止使用的全角符号", NULL, 0);
				//	return;
				//}

				InsertStr( utf8_str.c_str() );
				CloseClipboard();

				VirtualFree(utf16_str, 0, MEM_RELEASE);
			}
		}
	}

	void SQREdit::InsertStr( const char* str )
	{
		wstring utf16_str;
		utf8_to_utf16(utf16_str, str);
		GetFont()->UpdateCharInfo(utf16_str);
		UIWString& windowText = GetEditString();
		eraseSelectedText();
		const wchar_t* Buf = utf16_str.c_str();
		if ( Buf )
		{
			while ( *Buf != 0 )
			{
				if ( *Buf == L'\n' )
				{
					if ( GetStyle()&ECS_MULTILINE )
					{
						if ( Buf[1] == 0x0a )
							++Buf;
						windowText.insert( m_caratIndex, 1, L'\n' );
					}
					else
						break;
				}
				else
					windowText.insert( m_caratIndex, 1, *Buf );

				m_caratIndex++;
				Buf++;
			}
		}

		formatText();
		onTextChanged();

		SetCurShow();
	}

	void SQREdit::DisableIme( bool bDisable )
	{
		if ( bDisable )
			SetStyle( GetStyle()|ECS_DISABLEIME );
		else
			SetStyle( GetStyle()&(~ECS_DISABLEIME) );

		if ( IsActive() )
			SQRGUIManager::GetGUI( this )->EnableIme( !bDisable );
	}

	void SQREdit::OnDrag( const CFPos& HeldPoint, const CFPos& CurPoint )
	{
		if ( GetLineCount() == 0 )
			return SQRWnd::OnDrag( HeldPoint, CurPoint );

		size_t start_char_idx = getTextIndexFromPosition(HeldPoint);
		size_t end_char_idx	  = getTextIndexFromPosition(CurPoint);
		setCaratIndex(end_char_idx);
		SetSelection(start_char_idx, end_char_idx);

		SetCurShow();

		return SQRWnd::OnDrag( HeldPoint, CurPoint );
	}

	void SQREdit::OnActive( bool bActive, SQRWnd* Wnd )
	{
		if ( this->IsShow() )
		{
			m_hWnd->m_pGUI->SetIsEditActive( bActive );
			if ( !bActive )
				SendCtrlMsg( EDIT_LOSEFOCUS, 0, 0 );
		}
		else
			m_hWnd->m_pGUI->SetIsEditActive( false );

		if ( GetStyle()&ECS_DISABLEIME )
			SQRGUIManager::GetGUI( this )->EnableIme( !bActive );
		else if ( bActive )
			SQRGUIManager::GetGUI( this )->EnableIme( true );
	}

	void SQREdit::LoseFocusByShowFalse()
	{
		m_hWnd->m_pGUI->SetIsEditActive( false );

		if ( GetStyle()&ECS_DISABLEIME )
			SQRGUIManager::GetGUI( this )->EnableIme( true );
	}

	void SQREdit::OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 )
	{
		if ( SCROLL_MOVE == uMsgID )
		{
		}

		if ( SCROLL_RELEASE == uMsgID && ( pChild == &m_sbHorizon || pChild == &m_sbVertical ) )
			SetFocus();
	}


	void SQREdit::OnChar( uint32 nChar, uint32 nFlags )
	{
		if ( GetIsOnCommand() )
		{
			SetIsOnCommand(false);
			return;
		}

		static bool s_bWuBiChar	  = false;
		static bool	s_bDoubleChar = false;
		static char s_input[2] = {0};

		char cur_char = static_cast<char>(nChar);

		if ( s_bDoubleChar )
		{
			s_input[1] = cur_char;
			s_bDoubleChar = false;
		}
		else
		{
			if ( IsDBCSLeadByte(cur_char) )
			{
				s_bWuBiChar = true;
				s_bDoubleChar = true;
				s_input[0] = cur_char;
			}
			else
			{
				s_bWuBiChar = false;
				s_input[0] = cur_char;
				s_input[1] = 0;
			}
		}

		// 将UTF-8字符或五笔输入的标点字符转换成UTF-16字符
		wstring utf16_char;
		if ( !s_bDoubleChar )
		{
			string	src_char(1, (char)nChar);
			src_char.assign(s_input, 2);

			if ( s_bWuBiChar )
				utf16_char = gbk_to_utf16(src_char);
			else
				utf16_char = utf8_to_utf16(src_char);

			s_input[0] = 0;
			s_input[1] = 0;
		}

		GetFont()->UpdateCharInfo(utf16_char);

		UIWString& windowText = GetEditString();
		UpdateShowTime();
		const wchar_t c = utf16_char[0];
		if ( c >= 32 )
		{
			int32 nMinPos = min( m_selectStart, m_selectEnd );
			if ( ECS_DIGITAL & GetStyle() )
			{
				if ( !is_numeric(c) &&
						!( ( ( c == '+' || c == '-' ) && !nMinPos && ( windowText.empty() || is_numeric( windowText[0] ) ) ) ) &&
						!( c == '.' && windowText.find( '.' ) == UIString::npos && nMinPos && is_numeric( windowText[ nMinPos - 1 ] ) ) )
					return;
				int32 nMaxPos = max( m_selectStart, m_selectEnd );
				if ( is_numeric(c) && nMaxPos < (int32)windowText.size() && windowText[nMaxPos] == '-' )
					return;
			}
			else if( ECS_UNSIGNEDINT & GetStyle() )
			{
				if ( !is_numeric(c) || ( 0 == m_caratIndex && c == '0' ) )
					return;
				if ( 1 == windowText.length() && 1 == m_caratIndex && '0' == windowText[0] )
					handleBackspace();
			}

			eraseSelectedText();
			windowText.insert( m_caratIndex, 1, c );
			++m_caratIndex;
			onTextChanged();
			SetCurShow();
		}
		else if ( c == CTRL_A )
		{
			SetSelection(0, GetEditString().length());
			SetCurShow();
		}
		else if ( ECS_ENABLECLIPBD&GetStyle() )
		{
			if ( c == CTRL_C )
				CtrlC();
			else if ( c == CTRL_V )
				CtrlV();
			else if ( c == CTRL_X )
			{
				CtrlC();
				eraseSelectedText();
			}
			SetCurShow();
		}
		SendCtrlMsg( EDIT_ONCHAR, m_caratIndex, m_caratIndex );
	}

	void SQREdit::OnImeChar( uint32 nChar, uint32 nFlags )
	{
		if ( ECS_DIGITAL&GetStyle() || ECS_UNSIGNEDINT&GetStyle() )
			return;

		//屏蔽全角输入
		//if ( is_sbc(nChar) )
			//return;

		eraseSelectedText();

		// 将转换好的字符插入到文本中
		GetEditString().insert( m_caratIndex, (wchar_t*)&nChar, 1 );
		++m_caratIndex;
		onTextChanged();
		SetCurShow();
		UpdateShowTime();
		SendCtrlMsg( EDIT_ONCHAR, m_selectStart, m_selectEnd );
	}

	void SQREdit::OnKeyDown( uint32 nChar, uint32 nFlags )
	{
		UIWString& windowText = GetEditString();

		switch ( nChar )
		{
		case VK_SHIFT:
			{
				if ( getSelectionLength() == 0 )
				{
					m_dragAnchorIdx = getCaratIndex();
				}
			}
			break;
		case VK_ESCAPE:
			{
				SendCtrlMsg( VK_ESCAPE, 0, 0 );
			}
			break;
		case VK_TAB:
			{
				if (this->GetIsSendMsgToParent())
				{
					SendCtrlMsg( VK_TAB, 0, 0 );
				}
				else
				{
					OnChar( ' ', 0 );
					OnChar( ' ', 0 );
					OnChar( ' ', 0 );
					OnChar( ' ', 0 );
				}
			}
			break;
		case VK_RETURN:
			{
				int temp = GetSendType() ? ( GetKeyState( VK_CONTROL ) & 0x80 ) : ( !( GetKeyState( VK_CONTROL ) & 0x80 ) );

				if ( GetStyle()&ECS_MULTILINE && temp )
				{
					UpdateShowTime();
					handleNewLine();
				}
				else if( GetKeyState( VK_CONTROL ) & 0x80 )
				{
					SendCtrlMsg( EDIT_CTRLRETURN, 0, 0 );
				}
				else
				{
					SendCtrlMsg( EDIT_RETURN, 0, 0 );
				}
			}
			break;
		case VK_BACK:
			{
				handleBackspace();
				UpdateShowTime();
				SendCtrlMsg( EDIT_BACK, m_selectStart, m_selectEnd );
			}
			break;
		case VK_DELETE:
			{
				handleDelete();

				SetCurShow();
				UpdateShowTime();
				SendCtrlMsg( EDIT_DELETE, m_selectStart, m_selectEnd );
			}
			break;
		case VK_LEFT:
			{
				// 左右移动光标一个单词，如果是向后选则包括空格
				if ( GetKeyState(VK_CONTROL) & 0x80 )
					handleWordLeft();
				// 左右移动光标一个字符或汉字
				else
					handleCharLeft();

				SetCurShow();
				UpdateShowTime();
			}
			break;
		case VK_RIGHT:
			{
				// 左右移动光标一个单词，如果是向后选则包括空格
				if ( GetKeyState(VK_CONTROL) & 0x80 )
					handleWordRight();
				// 左右移动光标一个字符或汉字
				else
					handleCharRight();

				SetCurShow();
				UpdateShowTime();
			}
			break;
		case VK_UP:
			{
				SendCtrlMsg( VK_UP, 0, 0 );
				if ( GetKeyState( VK_CONTROL ) & 0x80 )
				{
					SendCtrlMsg( EDIT_CTRLUP, 0, 0 );
					break;
				}
				if ( GetStyle()&ECS_MULTILINE )
				{
					handleLineUp();
					SetCurShow();
					UpdateShowTime();
				}
			}
			break;
		case VK_DOWN:
			{
				SendCtrlMsg( VK_DOWN, 0, 0 );
				if ( GetKeyState( VK_CONTROL ) & 0x80 )
				{
					SendCtrlMsg( EDIT_CTRLDOWN, 0, 0 );
					break;
				}
				if ( GetStyle()&ECS_MULTILINE )
				{
					handleLineDown();
					SetCurShow();
					UpdateShowTime();
				}
			}
			break;
		case VK_HOME:
			{
				if ( GetKeyState(VK_CONTROL) & 0x80 )
				{
					handleDocHome();
				}
				else
				{
					handleLineHome();
				}

				SetCurShow();
				UpdateShowTime();
			}
			break;
		case VK_END:
			{
				if (GetKeyState(VK_CONTROL) & 0x80)
				{
					handleDocEnd();
				}
				else
				{
					handleLineEnd();
				}

				SetCurShow();
				UpdateShowTime();
			}
			break;
		default:
			break;
		}
	}

	void SQREdit::OnLButtonDown( uint32 nFlags, int32 x, int32 y )
	{
		CFPos point(static_cast<float>(x), static_cast<float>(y));

		clearSelection();
		m_dragAnchorIdx = getTextIndexFromPosition(point);
		setCaratIndex(m_dragAnchorIdx);

		SetCurShow();
		UpdateShowTime();
	}

	void SQREdit::OnLButtonDblClk( uint32 nFlags, int32 x, int32 y )
	{
		CFPos point(static_cast<float>(x), static_cast<float>(y));
		size_t m_caratIndex = getTextIndexFromPosition(point);

		m_dragAnchorIdx = GUITextUtils::getWordStartIdx(GetEditString(), (m_caratIndex == GetEditString().length()) ? m_caratIndex : m_caratIndex + 1);
		m_caratIndex    = GUITextUtils::getNextWordStartIdx(GetEditString(), m_caratIndex);

		// perform actual selection operation.
		SetSelection(m_dragAnchorIdx, m_caratIndex);

		SetCurShow();
		UpdateShowTime();
	}

	void SQREdit::OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y )
	{
		if (IsActive())
			m_sbVertical.SendMsg( WM_MOUSEWHEEL, MAKELONG( nFlags, zDelta ), MAKELONG( x, y ) );
	}

	SQRWnd* SQREdit::GetResWnd( int32 nCtrlMask )
	{
		if( 0 == nCtrlMask )
			return NULL;
		else if( SM_CE_BK == nCtrlMask )
		{
			nCtrlMask -= SM_CE_BK - SM_BS_BK;
			return SQRWnd::GetResWnd( nCtrlMask );
		}
		else
		{
			nCtrlMask -= SM_CE_SCV_BK - SM_SC_BK;
			return m_sbVertical.GetResWnd( nCtrlMask );
		}
	}

	WND_IMAGE_LIST* SQREdit::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
	{
		IMAGE_PARAM Flag = ImageFlag;
		if( 0 == Flag.CtrlMask )
			return NULL;
		else if( SM_CE_BK == Flag.CtrlMask )
		{
			Flag.CtrlMask -= SM_CE_BK - SM_BS_BK;
			return SQRWnd::GetWndBkImage( Flag );
		}
		else
		{
			Flag.CtrlMask -= SM_CE_SCV_BK - SM_SC_BK;
			return m_sbVertical.GetWndBkImage( Flag );
		}
	}
	//------------------------------------------------------------------------------
	/**
		UI缩放, 字不缩放, 你说咋对齐...
	 */
	float
	sqr::SQREdit::GetFontZoomSize()
	{
		float zoomSize = m_hWnd->m_pGUI->GetZoomSize();
		float fontSize = GetFontSize();
		if (0 != fontSize)
		{
			fontSize = zoomSize * fontSize;
			zoomSize = fontSize;	// zoomSize;
		}
		return zoomSize;
	}

	float SQREdit::getTextPixelLen( const wstring &text, size_t start_idx, size_t len )
	{
		CRenderFont *fnt = GetFont();
		CVector2f	size = fnt->CalculateSize(text.substr(start_idx, len).c_str());
		return size.x;
	}

	float SQREdit::getTextPixelLen( const wstring &text )
	{
		CRenderFont *fnt = GetFont();
		CVector2f size = fnt->CalculateSize(text.c_str());
		return size.x;
	}

	void sqr::SQREdit::SetSelection( size_t start_pos, size_t end_pos )
	{
		if ( GetStyle() & ECS_MULTILINE )
		{
			if ( start_pos > GetEditString().length() - 1 )
				start_pos = GetEditString().length() - 1;

			if ( end_pos > GetEditString().length() - 1 )
				end_pos = GetEditString().length() - 1;
		}
		else
		{
			if ( start_pos > GetEditString().length() )
				start_pos = GetEditString().length();

			if ( end_pos > GetEditString().length() )
				end_pos = GetEditString().length();
		}
		

		if ( start_pos > end_pos )
		{
			size_t tmp	= start_pos;
			start_pos	= end_pos;
			end_pos		= tmp;
		}

		if ( (start_pos != m_selectStart) || (end_pos != m_selectEnd) )
		{
			m_selectStart	= start_pos;
			m_selectEnd		= end_pos;
		}
	}

	size_t SQREdit::getTextIndexFromPosition( CFPos pt ) const
	{
		if ( m_lines.size() == 0 )
			return 0;

		pt.x -= getTextDrawArea().left;
		pt.y -= getTextDrawArea().top;

		// factor in scroll bar values
		pt.x += static_cast<float>(m_sbHorizon.GetPos());
		pt.y += static_cast<float>(m_sbVertical.GetPos());

		size_t lineNumber = static_cast<size_t>(pt.y / this->GetFont()->GetLineSpacing());

		if (lineNumber >= m_lines.size())
		{
			lineNumber = m_lines.size() - 1;
		}

		wstring text = GetStyle() & ECS_PASSWORD ? m_szPassWord.c_str() : GetEditString().c_str();
		wstring lineText(text.substr(m_lines[lineNumber].startIdx, m_lines[lineNumber].length));

		size_t lineIdx = this->GetFont()->GetCharAtPixel(lineText, pt.x);

		if (lineIdx >= lineText.length() - 1 && (GetStyle() & ECS_MULTILINE))
		{
			lineIdx = lineText.length() - 1;
		}

		return m_lines[lineNumber].startIdx + lineIdx;
	}

	CFRect SQREdit::getTextDrawArea() const
	{
		CFRect drawArea( 0.0f, 0.0f, 
			static_cast<float>(GetWndWidth()), static_cast<float>(GetWndHeight()) 
			);
		StyleStruct* pStyle = GetStyleStruct();
		uint32 Style = m_hWnd->m_StyleIndex;
		GUI_BLOCK* pBlock = NULL;
		if ( Style>0 && NULL != pStyle )
		{
			pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
			if(pBlock == NULL)
				pBlock = pStyle->m_vecPool[IP_ENABLE][1];
			if( pBlock )
			{
				GUI_BLOCK& Block = *pBlock;
				float t_Zoom = m_hWnd->m_pGUI->GetZoomSize();
				float TopLeftH		= Block[GBM_TOP_LEFT].Height()* t_Zoom;
				float TopLeftW		= Block[GBM_TOP_LEFT].Width() * t_Zoom;
				float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width() * t_Zoom;
				float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height()* t_Zoom;

				drawArea.left	+= TopLeftW;
				drawArea.right	-= BottomRightW;
				drawArea.top	+= TopLeftH;
				drawArea.bottom	-= BottomRightH;
			}
			else
			{
				float xBorder = static_cast<float>(GetXBorder(GetStyle()));
				float yBorder = static_cast<float>(GetYBorder(GetStyle()));
				drawArea.left	+= xBorder;
				drawArea.right	-= xBorder;
				drawArea.top	+= yBorder;
				drawArea.bottom	-= yBorder;
			}
		}
		else
		{
			float xBorder = static_cast<float>(GetXBorder(GetStyle()));
			float yBorder = static_cast<float>(GetYBorder(GetStyle()));
			drawArea.left	+= xBorder;
			drawArea.right	-= xBorder;
			drawArea.top	+= yBorder;
			drawArea.bottom	-= yBorder;
		}
		
		return drawArea;
	}

	size_t SQREdit::getLineNumberFromIndex(size_t index) const
	{
		size_t lineCount = m_lines.size();

		if (lineCount == 0)
		{
			return 0;
		}
		else if (index >= GetEditString().length() - 1)
		{
			return lineCount - 1;
		}
		else
		{
			size_t indexCount = 0;
			size_t caratLine = 0;

			for (; caratLine < lineCount; ++caratLine)
			{
				indexCount += m_lines[caratLine].length;

				if (index < indexCount)
				{
					return caratLine;
				}
			}
			return lineCount - 1;
		}
	}

	size_t SQREdit::getSelectionStartIndex() const
	{
		return (m_selectStart != m_selectEnd) ? m_selectStart : m_caratIndex;
	}

	size_t SQREdit::getSelectionEndIndex() const
	{
		return (m_selectEnd != m_selectStart) ? m_selectEnd : m_caratIndex;
	}

	void SQREdit::setCaratIndex( size_t carat_index )
	{
		if ( GetStyle() & ECS_MULTILINE )
		{
			if ( carat_index > GetEditString().length() - 1 )
				carat_index = GetEditString().length() - 1;
		}
		else
		{
			if ( carat_index > GetEditString().length() )
				carat_index = GetEditString().length();
		}

		if ( m_caratIndex != carat_index )
		{
			m_caratIndex = carat_index;
			ensureCaratIsVisible();
		}
	}

	void SQREdit::drawTextLines(const CFRect &dest_area)
	{
		// text is already formatted, we just grab the lines and render them with the required alignment.
		CFRect drawArea(dest_area);
		float vertScrollPos = (float)GetVertScrollbar()->GetPos();
		drawArea.Offset(-(float)GetHorzScrollbar()->GetPos(), -vertScrollPos);

		CRenderFont* fnt = GetFont();

		if (fnt)
		{
			// calculate final colours to use.
			uint32 normalTextCol = IsEnable() ? m_hWnd->m_EnableColor : m_hWnd->m_DisableColor;
			uint32 backgroundCol = IsEnable() ? m_hWnd->m_EnableBcColor : m_hWnd->m_DisableBcColor;
			uint32 selectAreaCol = ( normalTextCol - 0xff111111) | 0xff000000;

			const LineList& lines = getFormattedLines();
			const size_t numLines = lines.size();

			// calculate the range of visible lines
			size_t sidx,eidx;
			sidx = static_cast<size_t>(vertScrollPos / fnt->GetLineSpacing());
			eidx = 1 + sidx + static_cast<size_t>(dest_area.Height() / fnt->GetLineSpacing());
			eidx = min(eidx, numLines);
			drawArea.top += fnt->GetLineSpacing()*static_cast<float>(sidx);

			// for each formatted line.
			for (size_t i = sidx; i < eidx; ++i)
			{
				CFRect lineRect(drawArea);
				const LineInfo& currLine = lines[i];

				wstring lineText = (GetStyle() & ECS_PASSWORD) ? m_szPassWord.c_str() : GetEditString().substr(currLine.startIdx, currLine.length).c_str();

				// offset the font little down so that it's centered within its own spacing
				float old_top = lineRect.top;
				lineRect.top += (fnt->GetLineSpacing() - fnt->GetFontHeight()) * 0.5f;

				// if it is a simple 'no selection area' case
				if ((currLine.startIdx >= getSelectionEndIndex()) ||
					((currLine.startIdx + currLine.length) <= getSelectionStartIndex()))
				{
					// render the complete line.
					DrawText(lineText, 
						lineRect.left, 
						lineRect.top, 
						normalTextCol, backgroundCol, false, NULL);
				}
				// we have at least some selection highlighting to do
				else
				{
					// Start of actual rendering section.
					wstring sect;
					size_t sectIdx = 0, sectLen;
					float selStartOffset = 0.0f, selAreaWidth = 0.0f;

					// render any text prior to selected region of line.
					if (currLine.startIdx < getSelectionStartIndex())
					{
						// calculate length of text section
						sectLen = getSelectionStartIndex() - currLine.startIdx;

						// get text for this section
						sect = lineText.substr(sectIdx, sectLen);
						sectIdx += sectLen;

						// get the pixel offset to the beginning of the selection area highlight.
						selStartOffset = getTextPixelLen(sect.c_str());

						// draw this portion of the text
						DrawText(sect, 
							lineRect.left, 
							lineRect.top, 
							normalTextCol, backgroundCol, false, NULL);

						// set position ready for next portion of text
						lineRect.left += selStartOffset;
					}

					// calculate the length of the selected section
					sectLen = min(getSelectionEndIndex() - currLine.startIdx, currLine.length) - sectIdx;

					// get the text for this section
					sect = lineText.substr(sectIdx, sectLen);
					sectIdx += sectLen;

					// get the extent to use as the width of the selection area highlight
					selAreaWidth = getTextPixelLen(sect);

					const float text_top = lineRect.top;
					lineRect.top = old_top;

					// calculate area for the selection brush on this line
					lineRect.left = drawArea.left + selStartOffset;
					lineRect.right = lineRect.left + selAreaWidth;
					lineRect.bottom = lineRect.top + fnt->GetLineSpacing();

					// render the selection area brush for this line
					DrawRect( NULL, lineRect, selectAreaCol, NULL, true );

					// draw the text for this section
					DrawText(sect, 
						lineRect.left, 
						lineRect.top, 
						normalTextCol, backgroundCol, false, NULL);

					lineRect.top = text_top;

					// render any text beyond selected region of line
					if (sectIdx < currLine.length)
					{
						// update render position to the end of the selected area.
						lineRect.left += selAreaWidth;

						// calculate length of this section
						sectLen = currLine.length - sectIdx;

						// get the text for this section
						sect = lineText.substr(sectIdx, sectLen);

						// render the text for this section.
						DrawText(sect, 
							lineRect.left, 
							lineRect.top, 
							normalTextCol, backgroundCol, false, NULL);
					}
				}

				// update master position for next line in paragraph.
				drawArea.top += fnt->GetLineSpacing();
			}
		}
	}

	void SQREdit::drawCarat( const CFRect &textArea )
	{
		CRenderFont* fnt = GetFont();

		// require a font so that we can calculate carat position.
		if (fnt)
		{
			// get line that carat is in
			size_t caratLine = getLineNumberFromIndex(getCaratIndex());

			const LineList& lines = getFormattedLines();

			// if carat line is valid.
			size_t line_counts = lines.size();
			if (caratLine < line_counts || line_counts == 0)
			{
				// if edit text is empty, line_start is 0
				size_t line_start = (line_counts==0) ? 0 : lines[caratLine].startIdx;

				// calculate pixel offsets to where carat should be drawn
				size_t caratLineIdx = getCaratIndex() - line_start;
				UIWString text = GetStyle() & ECS_PASSWORD ? m_szPassWord : GetEditString();
				float ypos = caratLine * fnt->GetLineSpacing();
				float xpos = getTextPixelLen(
					text.substr(line_start, caratLineIdx).c_str()
					);
				float fLineSpace = GetFont()->GetLineSpacing();
				ypos -= 0.5f;
				if (xpos != 0.0f)
					xpos -= 0.5f;
				// calculate finat destination area for carat
				CFRect caratArea;
				caratArea.left    = textArea.left + xpos;
				caratArea.top     = textArea.top + ypos;
				caratArea.right	  = caratArea.left + m_fCaratWidth;
				caratArea.bottom  = caratArea.top + fLineSpace;
				caratArea.Offset(-static_cast<float>(m_sbHorizon.GetPos()), 
					-static_cast<float>(m_sbVertical.GetPos())
					);

				// cache the carat image for rendering.
				uint32 uColor = IsEnable() ? m_hWnd->m_EnableColor : m_hWnd->m_DisableColor;
				DrawRect( NULL, caratArea, uColor, NULL, true );
			}
		}
	}

	void SQREdit::onTextChanged()
	{
		// 确保多行文本以"\n"结束
		if ( GetStyle() & ECS_MULTILINE )
		{
			if ( (GetEditString().length() == 0) || 
				 (GetEditString()[GetEditString().length() - 1] != L'\n') )
			{
				GetEditString().append(1, L'\n');
			}
		}
		formatText();
		// 确保光标在显示区域内
		setCaratIndex(getCaratIndex());
		ensureCaratIsVisible();
	}

	void SQREdit::eraseSelectedText()
	{
		if ( getSelectionLength() != 0 )
		{
			setCaratIndex(getSelectionStartIndex());
			GetEditString().erase(getSelectionStartIndex(), getSelectionLength());
			onTextChanged();

			clearSelection();
		}
	}

	void SQREdit::clearSelection()
	{
		if ( getSelectionLength() != 0 )
		{
			SetSelection(0,0);
		}
	}

	void SQREdit::ensureCaratIsVisible()
	{
		SQRScrollBar* vertScrollbar = GetVertScrollbar();
		SQRScrollBar* horzScrollbar = GetHorzScrollbar();

		// calculate the location of the carat
		CRenderFont* fnt = GetFont();
		size_t caratLine = getLineNumberFromIndex(m_caratIndex);

		if (caratLine < m_lines.size())
		{
			CFRect textArea(getTextDrawArea());

			size_t caratLineIdx = m_caratIndex - m_lines[caratLine].startIdx;

			float ypos = caratLine * fnt->GetLineSpacing();
			float xpos = getTextPixelLen(GetEditString().substr(m_lines[caratLine].startIdx, caratLineIdx).c_str());

			// adjust position for scroll bars
			xpos -= horzScrollbar->GetPos();
			ypos -= vertScrollbar->GetPos();

			// if carat is above window, scroll up
			if (fnt->GetLineSpacing() > textArea.Height())
			{
				vertScrollbar->SetPos(0);
			}
			else if (ypos < 0)
			{
				vertScrollbar->SetPos(vertScrollbar->GetPos() + (int32)ypos);
			}
			// if carat is below the window, scroll down
			else if ((ypos += fnt->GetLineSpacing()) > textArea.Height())
			{
				vertScrollbar->SetPos(vertScrollbar->GetPos() + (int32)(ypos + fnt->GetLineSpacing() - textArea.Height()) );
			}

			// if carat is left of the window, scroll left
			if (xpos < 0)
			{
				horzScrollbar->SetPos(horzScrollbar->GetPos() + (int32)xpos);
			}
			// if carat is right of the window, scroll right
			else if (xpos > textArea.Width()-m_fCaratWidth)
			{
				horzScrollbar->SetPos(horzScrollbar->GetPos() + (int32)(xpos - textArea.Width() + m_fCaratWidth));
			}
		}
	}

	void SQREdit::handleNewLine()
	{
		// erase selected text
		eraseSelectedText();

		// if there is room
		if ( GetEditString().length() - 1 < m_uMaxTextLen )
		{
			GetEditString().insert(getCaratIndex(), 1, 0x0a);
			m_caratIndex++;
			onTextChanged();
		}
	}

	void SQREdit::handleBackspace()
	{
		if (getSelectionLength() != 0)
		{
			eraseSelectedText();
		}
		else if ( m_caratIndex > 0 )
		{
			GetEditString().erase(m_caratIndex-1, 1);
			setCaratIndex(m_caratIndex-1);
			onTextChanged();
		}
	}

	void SQREdit::handleDelete()
	{
		if (getSelectionLength() != 0)
		{
			eraseSelectedText();
		}
		else 
		{
			size_t end_pos;
			if ( GetStyle() & ECS_MULTILINE )
				end_pos = GetEditString().length() - 1;
			else
				end_pos = GetEditString().length();

			if (getCaratIndex() < end_pos)
			{
				GetEditString().erase(m_caratIndex, 1);
				onTextChanged();
				ensureCaratIsVisible();
			}
		}
	}

	void SQREdit::handleCharLeft()
	{
		if (m_caratIndex > 0)
			setCaratIndex(m_caratIndex - 1);

		if (GetKeyState(VK_SHIFT) & 0x80)
			SetSelection(m_caratIndex, m_dragAnchorIdx);
		else
			clearSelection();
	}

	void SQREdit::handleCharRight()
	{
		if ( GetStyle() & ECS_MULTILINE )
		{
			if ( m_caratIndex < GetEditString().length() - 1 )
				setCaratIndex(m_caratIndex + 1);
		}
		else
		{
			if ( m_caratIndex < GetEditString().length() )
				setCaratIndex(m_caratIndex + 1);
		}
		

		if ( GetKeyState(VK_SHIFT) & 0x80 )
			SetSelection(m_caratIndex, m_dragAnchorIdx);
		else
			clearSelection();
	}

	void SQREdit::handleWordLeft()
	{
		if ( m_caratIndex > 0 )
			setCaratIndex(GUITextUtils::getWordStartIdx(GetEditString(), m_caratIndex));

		if ( GetKeyState(VK_SHIFT) & 0x80 )
			SetSelection(m_caratIndex, m_dragAnchorIdx);
		else
			clearSelection();
	}

	void SQREdit::handleWordRight()
	{
		if ( GetStyle() & ECS_MULTILINE )
		{
			if ( m_caratIndex < GetEditString().length() - 1 )
				setCaratIndex(GUITextUtils::getNextWordStartIdx(GetEditString(), m_caratIndex));
		}
		else
		{
			if ( m_caratIndex < GetEditString().length() )
				setCaratIndex(GUITextUtils::getNextWordStartIdx(GetEditString(), m_caratIndex));
		}
		
		if ( GetKeyState(VK_SHIFT) & 0x80 )
			SetSelection(m_caratIndex, m_dragAnchorIdx);
		else
			clearSelection();
	}

	void SQREdit::handleLineUp()
	{
		size_t caratLine = getLineNumberFromIndex(m_caratIndex);

		if (caratLine > 0)
		{
			float caratPixelOffset = getTextPixelLen(GetEditString().substr(m_lines[caratLine].startIdx, m_caratIndex - m_lines[caratLine].startIdx).c_str());
			--caratLine;
			size_t newLineIndex = GetFont()->GetCharAtPixel(GetEditString().substr(m_lines[caratLine].startIdx, m_lines[caratLine].length).c_str(), caratPixelOffset);
			setCaratIndex(m_lines[caratLine].startIdx + newLineIndex);
		}

		if (GetKeyState(VK_SHIFT) & 0x80)
		{
			SetSelection(m_caratIndex, m_dragAnchorIdx);
		}
		else
		{
			clearSelection();
		}
	}

	void SQREdit::handleLineDown()
	{
		size_t caratLine = getLineNumberFromIndex(m_caratIndex);

		if ((m_lines.size() > 1) && (caratLine < (m_lines.size() - 1)))
		{
			float caratPixelOffset = getTextPixelLen(GetEditString().substr(m_lines[caratLine].startIdx, m_caratIndex - m_lines[caratLine].startIdx).c_str());
			++caratLine;
			size_t newLineIndex = GetFont()->GetCharAtPixel(GetEditString().substr(m_lines[caratLine].startIdx, m_lines[caratLine].length).c_str(), caratPixelOffset);
			setCaratIndex(m_lines[caratLine].startIdx + newLineIndex);
		}

		if (GetKeyState(VK_SHIFT) & 0x80)
		{
			SetSelection(m_caratIndex, m_dragAnchorIdx);
		}
		else
		{
			clearSelection();
		}
	}

	void SQREdit::handleLineHome()
	{
		size_t line = getLineNumberFromIndex(m_caratIndex);

		if (line < m_lines.size())
		{
			size_t lineStartIdx = m_lines[line].startIdx;

			if (m_caratIndex > lineStartIdx)
			{
				setCaratIndex(lineStartIdx);
			}

			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				SetSelection(m_caratIndex, m_dragAnchorIdx);
			}
			else
			{
				clearSelection();
			}
		}
	}

	void SQREdit::handleLineEnd()
	{
		size_t line = getLineNumberFromIndex(m_caratIndex);

		if (line < m_lines.size())
		{
			size_t lineEndIdx;

			if ( GetStyle() & ECS_MULTILINE )
				lineEndIdx = m_lines[line].startIdx + m_lines[line].length - 1;
			else
				lineEndIdx = m_lines[line].startIdx + m_lines[line].length;

			if (m_caratIndex < lineEndIdx)
			{
				setCaratIndex(lineEndIdx);
			}

			if (GetKeyState(VK_SHIFT) & 0x80)
			{
				SetSelection(m_caratIndex, m_dragAnchorIdx);
			}
			else
			{
				clearSelection();
			}
		}
	}

	void SQREdit::handleDocHome()
	{
		if (m_caratIndex > 0)
		{
			setCaratIndex(0);
		}

		if (GetKeyState(VK_SHIFT) & 0x80)
		{
			SetSelection(m_caratIndex, m_dragAnchorIdx);
		}
		else
		{
			clearSelection();
		}
	}

	void SQREdit::handleDocEnd()
	{
		if ( GetStyle() & ECS_MULTILINE )
		{
			if (m_caratIndex < GetEditString().length() - 1)
				setCaratIndex(GetEditString().length() - 1);
		}
		else
		{
			if (m_caratIndex < GetEditString().length())
				setCaratIndex(GetEditString().length() - 1);
		}

		if (GetKeyState(VK_SHIFT) & 0x80)
		{
			SetSelection(m_caratIndex, m_dragAnchorIdx);
		}
		else
		{
			clearSelection();
		}
	}
}
