// CWndProp.cpp : implementation file
//

#include "stdafx.h"
#include "GUIEditor.h"
#include "CWndProp.h"
#include "CWndGroup.h"
#include "SelectSound.h"
#include "CodeCvs.h"
#include "TraceHelper.h"

#include "../src/gui/SQRGUIManager.h"

#define WND_MSGFX_WIDTH			60

wstring TextureType[IP_COUNT] =
{
	L"MASK",
	L"MOUSEDESC",
	L"ENABLE",
	L"DISABLE",
	L"MOUSEOVER",
	L"CLICKDOWN",
	L"UNCKECK_ENABLE",
	L"UNKECK_DISABLE",
	L"UNCKECK_MOUSEOVER",
	L"UNCKECK_CLICKDOWN",
	L"PROGRESS_FILL",
	L"TR_ENABLE",
	L"TR_DISABLE",
	L"LS_SEL",
	L"LS_UNSEL",
	L"FILL_FULL",
	L"LBTNCLICK_ACTION",
	L"RBTNCLICK_ACTION"
};


// CWndProp

IMPLEMENT_DYNCREATE(CWndProp, CFormView)

CWndProp::CWndProp()
		: CFormView(CWndProp::IDD)
		, m_pWndGroup(NULL)
		, m_pCurEditWnd(NULL)
		, m_strAudioCueName(_T(""))
{
}

CWndProp::~CWndProp()
{
}

void CWndProp::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROP, m_PropList);
	DDX_Control(pDX, IDC_WND_BKTYPE, m_ImageListType);
	DDX_Control(pDX, IDC_LIST_IMAGE, m_ImageList);
	DDX_Control(pDX, IDC_LIST_IMAGEPROP, m_ImageProp);
	DDX_Control(pDX, IDC_AEDIT_TEXTCOLOR1, m_TextColor1);
	DDX_Control(pDX, IDC_AEDIT_TEXTCOLOR2, m_TextColor2);
	DDX_Control(pDX, IDC_AEDIT_TEXTBCCOLOR, m_TextBcColor);
	DDX_Control(pDX, IDC_IS_SHOW_BACK, m_isShowBack);
	DDX_Control(pDX, IDC_ALPHA_EDIT, m_TextAlphaEdit);
	DDX_Control(pDX, IDC_AUDIO_TYPE, m_AudioTypeComBox);
	DDX_Text(pDX, IDC_AUDIONAME_EDIT, m_strAudioCueName);
	DDX_Control(pDX, IDC_FLASH_STATA_CHOOSE, m_FlashStateComBox);
	DDX_Control(pDX, IDC_FLASH_NAME_CHOOSE, m_FlashNameComBox);
}

BEGIN_MESSAGE_MAP(CWndProp, CFormView)
	ON_CBN_SELENDOK(IDC_WND_BKTYPE, OnCbnSelendokWndBktype)
	ON_BN_CLICKED(IDC_FLIPH, OnBnClickedFliph)
	ON_BN_CLICKED(IDC_FLIPV, OnBnClickedFlipv)
	ON_EN_CHANGE(IDC_AEDIT_TEXTCOLOR1, OnEnChangeEditTextcolor1)
	ON_EN_CHANGE(IDC_AEDIT_TEXTBCCOLOR, OnEnChangeEditTextbccolor)
	ON_EN_CHANGE(IDC_AEDIT_TEXTCOLOR2, &CWndProp::OnEnChangeAeditTextcolor2)
	ON_EN_KILLFOCUS(IDC_AEDIT_TEXTCOLOR1, OnEnKillfocusEditTextcolor1)
	ON_EN_KILLFOCUS(IDC_AEDIT_TEXTCOLOR2, &CWndProp::OnEnKillfocusAeditTextcolor2)
	ON_EN_KILLFOCUS(IDC_AEDIT_TEXTBCCOLOR, OnEnKillfocusEditTextbccolor)
	ON_BN_CLICKED(IDC_IS_SHOW_BACK, &CWndProp::OnBnClickedIsShowBack)
	ON_BN_CLICKED(IDC_BTN_CLR1, &CWndProp::OnBnClickedBtnClr1)
	ON_BN_CLICKED(IDC_BTN_CLR2, &CWndProp::OnBnClickedBtnClr2)
	ON_BN_CLICKED(IDC_BTN_BKCLR, &CWndProp::OnBnClickedBtnBkclr)
	ON_BN_CLICKED(IDC_CHECK_ITALIC, &CWndProp::OnBnClickedCheckItalic)
	ON_BN_CLICKED(IDC_CHECK_SHADOW, &CWndProp::OnBnClickedCheckShadow)
	ON_BN_CLICKED(IDC_CHECK_OUTLINE, &CWndProp::OnBnClickedCheckOutline)
	ON_BN_CLICKED(IDC_CHECK_GRADUAL, &CWndProp::OnBnClickedCheckGradual)
	ON_BN_CLICKED(IDC_CHOOSE_TEXTURE, &CWndProp::OnBnClickedChooseTexture)
	ON_BN_CLICKED(IDC_CHOOSE_AUDIO, &CWndProp::OnBnClickedChooseAudio)
	ON_CBN_SELCHANGE(IDC_AUDIO_TYPE, &CWndProp::OnCbnSelchangeAudioType)
	ON_CBN_SELCHANGE(IDC_FLASH_STATA_CHOOSE, &CWndProp::OnCbnSelchangeFlashState)
	ON_CBN_SELCHANGE(IDC_FLASH_NAME_CHOOSE, &CWndProp::OnCbnSelchangeFlashName)
	ON_BN_CLICKED(IDC_CLEAR_AUDIO, &CWndProp::OnBnClickedClearAudio)
END_MESSAGE_MAP()


// CWndProp uMessage handlers

BOOL CWndProp::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	BOOL re = CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	UpdateData( FALSE );

	return re;
}

void CWndProp::Update( uint32 nFlag )
{
	if ( nFlag & EDIT_POSITION )
	{
		m_PropList.SetToItem();
		// 更新可选指示
		while ( m_FlashNameComBox.DeleteString( 0 ) != CB_ERR );
		FlashDataManager::FlashInfoMap::iterator it = FlashDataManager::GetInst()->FlashBegin();
		FlashDataManager::FlashInfoMap::iterator itend = FlashDataManager::GetInst()->FlashEnd();
		while ( it != itend )
		{
			wstring wname = utf8_to_utf16(it->second->m_FlashName.c_str());
			m_FlashNameComBox.AddString(wname.c_str());
			++it;
		}
		m_FlashNameComBox.AddString(L"NULL");
		
		SQRWnd* pCurEdit = m_pWndGroup->GetEditWnd();
		// 更新指示状态m_FlashStateComBox
		while ( m_FlashStateComBox.DeleteString( 0 ) != CB_ERR );
		for ( int i = IP_MASK; i < IP_COUNT; i++ )
		{
			if (pCurEdit && pCurEdit->ObjHasEventState(static_cast<EventStateMask>(i)) )
			{
				int nIndex = m_FlashStateComBox.AddString( TextureType[i].c_str() );
				if ( nIndex < 0 )
					continue;
				m_FlashStateComBox.SetItemData( nIndex, i );
			}
		}
		if (pCurEdit)
			m_FlashStateComBox.SetCurSel(0);
		
	}

	if ( nFlag & EDIT_TEXTURE )
	{
		if ( m_pCurEditWnd != m_pWndGroup->GetEditWnd() )
		{
			SQRWnd* pPreEdit = m_pCurEditWnd;
			m_pCurEditWnd = m_pWndGroup->GetEditWnd();
			while ( m_ImageListType.DeleteString( 0 ) != CB_ERR );
			if ( !m_pCurEditWnd )
			{
				m_ImageProp.m_pImage = NULL;
				return;
			}

			IMAGE_PARAM IP( 1, IP_MASK );
			for ( int i = IP_MASK; i < IP_COUNT; i++ )
			{
				IP.StateMask = i;
				WND_IMAGE_LIST* pImage = m_pCurEditWnd->GetWndBkImage( IP );
				if ( pImage )
				{
					int nIndex = m_ImageListType.AddString( TextureType[i].c_str() );
					if ( nIndex < 0 )
						continue;
					m_ImageListType.SetItemData( nIndex, i );
				}
			}
			m_ImageListType.SetWindowText( TextureType[IP_ENABLE].c_str() );
			m_ImageList.SetEditImageType( IP_ENABLE );
			// 颜色
			wchar_t Buf[256];
			IMAGE_PARAM IP1 ( SM_BS_BK, IP_ENABLE );
			m_FontEffectMask = m_pCurEditWnd->GetFontEffectMask();
			m_TextColor1.SetWindowText( _ultow( m_pCurEditWnd->GetWndTextColor( IP1 ), Buf, 16 ) );
			m_TextBcColor.SetWindowText( _ultow( m_pCurEditWnd->GetWndTextBcColor( IP1 ), Buf, 16 ) );
			m_TextColor2.SetWindowText( _ultow(m_pCurEditWnd->GetFontGradualColor(), Buf, 16 ) );
			m_TextAlphaEdit.SetWindowText(utf8_to_utf16(m_pCurEditWnd->GetWndHandle()->m_strTransName.c_str()).c_str());
			CheckDlgButton(IDC_CHECK_ITALIC, m_FontEffectMask & FontEffect::Italic);
			CheckDlgButton(IDC_CHECK_OUTLINE, m_FontEffectMask & FontEffect::Outline);
			CheckDlgButton(IDC_CHECK_SHADOW, m_FontEffectMask & FontEffect::Shadow);
			CheckDlgButton(IDC_CHECK_GRADUAL, m_FontEffectMask & FontEffect::Gradual);

			m_isShowBack.SetCheck( int(m_pCurEditWnd->IsDrawBackground()) );

			///sound
			ESoundMsg eSoundType = static_cast<ESoundMsg>(m_nAudioType);
			m_strAudioCueName = utf8_to_utf16(m_pCurEditWnd->GetMsgSound(eSoundType)).c_str();
			GetDlgItem(IDC_AUDIONAME_EDIT)->SetWindowText(m_strAudioCueName);
		}
		else
		{
			m_ImageList.SetEditImageType( m_ImageList.m_nImageType, false );
			CString szText;
			IMAGE_PARAM IP1 ( SM_BS_BK, m_ImageList.m_nImageType );
			if ( m_pCurEditWnd )
			{
				// 更新字体颜色
				GUI_FONT* pFont = m_pCurEditWnd->GetFontInfo(m_pCurEditWnd->GetStyleIndex());
				if (pFont)
				{
					wchar_t Buf[256];
					m_pCurEditWnd->SetWndTextColor( IP1, pFont->m_EventFont[(EventStateMask)(IP1.StateMask)].m_FontColor );
					m_pCurEditWnd->SetWndTextBcColor( IP1, pFont->m_EventFont[(EventStateMask)(IP1.StateMask)].m_uFontBkColor );
					m_pCurEditWnd->SetFontGradualColor( pFont->m_uFontGradualColor );
					m_pCurEditWnd->SetFontEffectMask(pFont->m_uFontEffectMask);

					m_FontEffectMask = m_pCurEditWnd->GetFontEffectMask();
					m_TextColor1.SetWindowText( _ultow( m_pCurEditWnd->GetWndTextColor( IP1 ), Buf, 16 ) );
					m_TextBcColor.SetWindowText( _ultow( m_pCurEditWnd->GetWndTextBcColor( IP1 ), Buf, 16 ) );
					m_TextColor2.SetWindowText( _ultow(m_pCurEditWnd->GetFontGradualColor(), Buf, 16 ) );
					m_TextAlphaEdit.SetWindowText(utf8_to_utf16(m_pCurEditWnd->GetWndHandle()->m_strTransName.c_str()).c_str());
					CheckDlgButton(IDC_CHECK_ITALIC, m_FontEffectMask & FontEffect::Italic);
					CheckDlgButton(IDC_CHECK_OUTLINE, m_FontEffectMask & FontEffect::Outline);
					CheckDlgButton(IDC_CHECK_SHADOW, m_FontEffectMask & FontEffect::Shadow);
					CheckDlgButton(IDC_CHECK_GRADUAL, m_FontEffectMask & FontEffect::Gradual);
				}
				else
				{
					m_TextColor1.GetWindowText( szText );
					m_pCurEditWnd->SetWndTextColor( IP1, wcstoul( L"0x" + szText, 0, 0 ) );
					m_TextBcColor.GetWindowText( szText );
					m_pCurEditWnd->SetWndTextBcColor( IP1, wcstoul( L"0x" + szText, 0, 0 ) );
					m_TextColor2.GetWindowText(szText);
					m_pCurEditWnd->SetFontGradualColor( wcstoul(L"0x" + szText, 0, 0) );
					m_pCurEditWnd->SetFontEffectMask(m_FontEffectMask);
				}
				

				m_pCurEditWnd->SetIsDrawBackground(m_isShowBack.GetCheck()==TRUE);
				m_TextAlphaEdit.GetWindowText(szText);
				m_pCurEditWnd->GetWndHandle()->m_strTransName = utf16_to_utf8(szText.GetBuffer()).c_str();
				SafeRelease(m_pCurEditWnd->GetWndHandle()->m_pTransparenTex);
				if(m_pCurEditWnd->GetWndHandle()->m_strTransName.length()>0)
					m_pCurEditWnd->GetGraphic()->CreateTexture(L"", utf16_to_utf8( szText.GetBuffer()).c_str() ,&(m_pCurEditWnd->GetWndHandle()->m_pTransparenTex));
				else
					m_pCurEditWnd->GetWndHandle()->m_pTransparenTex = NULL;

				///sound
				ESoundMsg eSoundType = static_cast<ESoundMsg>(m_nAudioType);
				m_strAudioCueName = utf8_to_utf16(m_pCurEditWnd->GetMsgSound(eSoundType)).c_str();
				GetDlgItem(IDC_AUDIONAME_EDIT)->SetWindowText(m_strAudioCueName);
			}
		}
	}
}

void CWndProp::MoveText( UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	m_ImageList.DetectImage( uMessage, wParam, lParam );
	m_ImageProp.MoveText( uMessage, wParam, lParam, m_pCurEditWnd );
}

void CWndProp::OnCbnSelendokWndBktype()
{
	m_ImageList.SetEditImageType( m_ImageListType.GetItemData( m_ImageListType.GetCurSel() ) );
	wchar_t Buf[256];
	IMAGE_PARAM IP1 ( SM_BS_BK, m_ImageList.m_nImageType );

	m_TextColor1.SetWindowText( _ultow( m_pCurEditWnd->GetWndTextColor( IP1 ), Buf, 16 ) );
	m_TextBcColor.SetWindowText( _ultow( m_pCurEditWnd->GetWndTextBcColor( IP1 ), Buf, 16 ) );

	m_isShowBack.SetCheck( int(m_pCurEditWnd->IsDrawBackground()) );
}

void CWndProp::OnBnClickedFliph()
{
	m_ImageProp.Flip( false );
}

void CWndProp::OnBnClickedFlipv()
{
	m_ImageProp.Flip( true );
}

void CWndProp::OnEnKillfocusEditTextcolor1()
{
	OnEnChangeEditTextcolor1();
}

void CWndProp::OnEnKillfocusEditTextbccolor()
{
	// TODO: Add your control notification handler code here
	OnEnChangeEditTextbccolor();
}

void CWndProp::OnEnKillfocusAeditTextcolor2()
{
	OnEnChangeAeditTextcolor2();
}

void CWndProp::OnEnChangeEditTextcolor1()
{
	if (NULL == m_pCurEditWnd)
		return;
	GUI_FONT* pFont = m_pCurEditWnd->GetFontInfo(m_pCurEditWnd->GetStyleIndex());
	if (pFont)
		return;

	CString szText;
	IMAGE_PARAM IP ( SM_BS_BK, m_ImageList.m_nImageType );
	m_TextColor1.GetWindowText( szText );
	m_pCurEditWnd->SetWndTextColor( IP, wcstoul( L"0x" + szText, 0, 0 ) );
}

void CWndProp::OnEnChangeEditTextbccolor()
{
	if (NULL == m_pCurEditWnd)
		return;
	GUI_FONT* pFont = m_pCurEditWnd->GetFontInfo(m_pCurEditWnd->GetStyleIndex());
	if (pFont)
		return;

	CString szText;
	IMAGE_PARAM IP ( SM_BS_BK, m_ImageList.m_nImageType );

	m_TextBcColor.GetWindowText( szText );
	DWORD color = wcstoul( L"0x" + szText, 0, 0 );
	m_pCurEditWnd->SetWndTextBcColor( IP, color );

	if (0 == color)
	{
		CheckDlgButton(IDC_CHECK_OUTLINE, FALSE);
		CheckDlgButton(IDC_CHECK_SHADOW, FALSE);
		m_FontEffectMask &= ~FontEffect::Outline;
		m_FontEffectMask &= ~FontEffect::Shadow;
		m_pCurEditWnd->SetFontEffectMask(m_FontEffectMask);
	}
}

void CWndProp::OnEnChangeAeditTextcolor2()
{
	if (NULL == m_pCurEditWnd)
		return;
	GUI_FONT* pFont = m_pCurEditWnd->GetFontInfo(m_pCurEditWnd->GetStyleIndex());
	if (pFont)
		return;
	CString szText;
	m_TextColor2.GetWindowText( szText );
	DWORD color = wcstoul( L"0x" + szText, 0, 0 );
	m_pCurEditWnd->SetFontGradualColor( color );

	if (0 == color)
	{
		CheckDlgButton(IDC_CHECK_GRADUAL, FALSE);
		m_FontEffectMask &= ~FontEffect::Gradual;
	}
	else
	{
		CheckDlgButton(IDC_CHECK_GRADUAL, TRUE);
		m_FontEffectMask |= FontEffect::Gradual;
	}
	m_pCurEditWnd->SetFontEffectMask(m_FontEffectMask);
}

void CWndProp::Init( CWndGroup* pWndGroup )
{
	m_pWndGroup = pWndGroup;
	m_PropList .Init( pWndGroup );
	m_ImageList.Init( pWndGroup, &m_ImageProp );
	m_pWndGroup->SetImageProp( &m_ImageProp );

	m_AudioTypeComBox.Clear();
	m_AudioTypeComBox.AddString(L"Click_0");
	m_AudioTypeComBox.AddString(L"Open_1");
	m_AudioTypeComBox.AddString(L"Close_2");
	m_AudioTypeComBox.AddString(L"MouseOver_3");
	m_AudioTypeComBox.SetCurSel(0);

	m_FlashStateComBox.Clear();
	m_FlashNameComBox.Clear();
	m_nAudioType = 0;
}

void CWndProp::DrawEditLine( CGraphic* pGraphic, int n )
{
	m_ImageList.DrawEditLine();
	m_ImageProp.DrawEditLine( pGraphic, m_pCurEditWnd, n );
}

void CWndProp::OnBnClickedIsShowBack()
{
	if (m_pCurEditWnd)
	{
		m_pCurEditWnd->SetIsDrawBackground(!m_pCurEditWnd->IsDrawBackground());
		m_isShowBack.SetCheck( int(m_pCurEditWnd->IsDrawBackground()) );
	}
}

void CWndProp::OnBnClickedBtnClr1()
{
	this->OnSelectColor(IDC_AEDIT_TEXTCOLOR1);
}

void CWndProp::OnBnClickedBtnClr2()
{
	this->OnSelectColor(IDC_AEDIT_TEXTCOLOR2);
}

void CWndProp::OnBnClickedBtnBkclr()
{
	this->OnSelectColor(IDC_AEDIT_TEXTBCCOLOR);
}

//------------------------------------------------------------------------------
void
CWndProp::OnSelectColor( int id )
{
	CColorDialog dialog(0, CC_FULLOPEN, this);
	if (IDOK == dialog.DoModal())
	{
		COLORREF color = dialog.GetColor();
		DWORD d3dColor = 0xff000000 | GetRValue(color) << 16 | GetGValue(color) << 8 | GetBValue(color);
		wchar_t text[9];
		_snwprintf(&text[0], 9, L"%x", d3dColor);
		SetDlgItemText(id, text);
	}
}

void CWndProp::OnBnClickedCheckItalic()
{
	GUI_FONT* pFont = m_pCurEditWnd->GetFontInfo(m_pCurEditWnd->GetStyleIndex());
	if (pFont)
		return;

	if (IsDlgButtonChecked(IDC_CHECK_ITALIC))
		m_FontEffectMask |= FontEffect::Italic;
	else
		m_FontEffectMask &= ~FontEffect::Italic;
	m_pCurEditWnd->SetFontEffectMask(m_FontEffectMask);
}

void CWndProp::OnBnClickedCheckShadow()
{
	GUI_FONT* pFont = m_pCurEditWnd->GetFontInfo(m_pCurEditWnd->GetStyleIndex());
	if (pFont)
		return;

	if (IsDlgButtonChecked(IDC_CHECK_SHADOW))
		m_FontEffectMask |= FontEffect::Shadow;
	else
		m_FontEffectMask &= ~FontEffect::Shadow;
	m_pCurEditWnd->SetFontEffectMask(m_FontEffectMask);
}

void CWndProp::OnBnClickedCheckOutline()
{
	GUI_FONT* pFont = m_pCurEditWnd->GetFontInfo(m_pCurEditWnd->GetStyleIndex());
	if (pFont)
		return;

	if (IsDlgButtonChecked(IDC_CHECK_OUTLINE))
		m_FontEffectMask |= FontEffect::Outline;
	else
		m_FontEffectMask &= ~FontEffect::Outline;
	m_pCurEditWnd->SetFontEffectMask(m_FontEffectMask);
}

void CWndProp::OnBnClickedCheckGradual()
{
	GUI_FONT* pFont = m_pCurEditWnd->GetFontInfo(m_pCurEditWnd->GetStyleIndex());
	if (pFont)
		return;

	if (IsDlgButtonChecked(IDC_CHECK_GRADUAL))
		m_FontEffectMask |= FontEffect::Gradual;
	else
		m_FontEffectMask &= ~FontEffect::Gradual;
	m_pCurEditWnd->SetFontEffectMask(m_FontEffectMask);
}

void CWndProp::OnBnClickedChooseTexture()
{
	// TODO: Add your control notification handler code here
	CString FileName = GetFileName(true, L"GUI Texture (*.tex *.dds)|*.tex;*.dds||");
	SafeRelease(m_pCurEditWnd->GetWndHandle()->m_pTransparenTex);
	ITexture* pText = NULL;
	m_pCurEditWnd->GetGraphic()->CreateTexture( L"", utf16_to_utf8( FileName.GetBuffer() ).c_str() ,&pText);

	if( NULL==pText )
	{
		MessageBox(L"纹理创建失败");
		m_TextAlphaEdit.SetWindowText(NULL);
		m_pCurEditWnd->GetWndHandle()->m_strTransName.clear();
		return;
	}
	if( pText->GetTexFormat() == TF_A8 )
	{
		m_pCurEditWnd->GetWndHandle()->m_pTransparenTex = pText;
		m_pCurEditWnd->GetWndHandle()->m_strTransName = utf16_to_utf8(FileName.GetBuffer()).c_str();
		m_TextAlphaEdit.SetWindowText(FileName.GetBuffer());
	}
	else
	{
		MessageBox(L"控件形状只能用A8格式的纹理");
		m_TextAlphaEdit.SetWindowText(NULL);
		m_pCurEditWnd->GetWndHandle()->m_strTransName.clear();
	}
}

void CWndProp::OnBnClickedChooseAudio()
{
	// TODO: Add your control notification handler code here
	static CSelectSound dialog;
	dialog.DoModal();
	wstring str = dialog.GetSelectSoundName();

	if( str != L"" )
	{
		ESoundMsg eSoundType = static_cast<ESoundMsg>(m_nAudioType);

		m_pCurEditWnd->SetMsgSound(eSoundType, utf16_to_utf8(str).c_str());

		m_strAudioCueName = str.c_str();
		UpdateData(FALSE);
	}
}

void CWndProp::OnCbnSelchangeAudioType()
{
	// TODO: Add your control notification handler code here
	CString sReceiveBuf;
	m_AudioTypeComBox.GetLBText(m_AudioTypeComBox.GetCurSel(),sReceiveBuf);
	int npos = sReceiveBuf.Find(L"_");
	int nLen = sReceiveBuf.GetLength();
	if( npos != -1 )
	{
		sReceiveBuf = sReceiveBuf.Right(nLen-npos-1);
		m_nAudioType = (int)_wtof(sReceiveBuf);

		ESoundMsg eSoundType = static_cast<ESoundMsg>(m_nAudioType);
		m_strAudioCueName = utf8_to_utf16(m_pCurEditWnd->GetMsgSound(eSoundType)).c_str();
		UpdateData(FALSE);
	}
}

void CWndProp::OnCbnSelchangeFlashState()
{
	// 更新指示名字的选中内容
	EventStateMask eFlashType = static_cast<EventStateMask>
								(m_FlashStateComBox.GetItemData( m_FlashStateComBox.GetCurSel() ));
	CString flashinfo = utf8_to_utf16(m_pCurEditWnd->GetFlashInfo(eFlashType)).c_str();

	if ( flashinfo.GetLength() <= 0 )
	{
		m_FlashNameComBox.SetCurSel(-1);
		return;
	}
		
	int i = m_FlashNameComBox.FindString(-1,flashinfo);
	m_FlashNameComBox.SetCurSel(i);

	UpdateData(FALSE);

}

void CWndProp::OnCbnSelchangeFlashName()
{
	EventStateMask eFlashType = static_cast<EventStateMask>
		(m_FlashStateComBox.GetItemData( m_FlashStateComBox.GetCurSel() ));

	CString strBuf;
	m_FlashNameComBox.GetLBText(m_FlashNameComBox.GetCurSel(),strBuf);
	m_pCurEditWnd->SetFlashInfo(eFlashType, utf16_to_utf8(strBuf.GetString()).c_str());
	UpdateData(FALSE);
}

void CWndProp::OnBnClickedClearAudio()
{
	// TODO: Add your control notification handler code here
	ESoundMsg eSoundType = static_cast<ESoundMsg>(m_nAudioType);
	string strCueName = m_pCurEditWnd->GetMsgSound(eSoundType);

	if( strCueName != "" )
	{
		static wstring nullCueName = L"";
		m_pCurEditWnd->SetMsgSound(eSoundType, utf16_to_utf8(nullCueName).c_str());
		
		m_strAudioCueName = nullCueName.c_str();
		UpdateData(FALSE);
	}
}
