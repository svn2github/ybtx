#include "stdafx.h"
#include "GUIEditor.h"
#include "CTexFontEditor.h"
#include "CWindowTarget.h"
#include "StringHelper.h"
#include "BaseHelper.h"

#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#define  MAX_PATH  PATH_MAX
#else
#include <direct.h>
#endif

#define FontMaxKey	10

IMPLEMENT_DYNCREATE(CTexFontEditor, CFormView)

CTexFontEditor::CTexFontEditor()
: CFormView(CTexFontEditor::IDD)
{
}

CTexFontEditor::~CTexFontEditor()
{
}

void CTexFontEditor::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FONT_KEY_CHOOSE, m_TexKeySel);
	DDX_Control(pDX, IDC_FONT_KEY_CHOOSE_CUR, m_TexKeySelCur);	// 当前KEY
	DDX_Control(pDX, IDC_FONT_WORD_CHOOSE, m_TexFontSel);	// 当前字

	DDX_Control(pDX, IDC_FONT_TEX, m_Texture);
	DDX_Control(pDX, IDC_FLASH_NAME_EDIT, m_FontName);	// 添加字

	DDX_Control(pDX, IDC_FONT_POSX, m_Fontx);
	DDX_Control(pDX, IDC_FONT_POSY, m_Fonty);
	DDX_Control(pDX, IDC_FONT_POSW, m_Fontw);
	DDX_Control(pDX, IDC_FONT_POSH, m_Fonth);
}

BEGIN_MESSAGE_MAP(CTexFontEditor, CFormView)
	ON_BN_CLICKED(IDC_FONT_CHOOSE_TEX, OnBnClickedChooseTex)
	ON_BN_CLICKED(IDC_FONT_ADD, OnBnClickedAddFont)
	ON_BN_CLICKED(IDC_FONT_DEL, OnBnClickedDelFont)
	ON_BN_CLICKED(IDC_FONT_EDIT_OK, OnBnClickedEditOk)
	ON_BN_CLICKED(IDC_FONT_SAVE_ALL, OnBnClickedSaveALL)
	ON_WM_SHOWWINDOW()

	ON_CBN_SELCHANGE(IDC_FONT_WORD_CHOOSE, OnCbnSelchangeFontCur)
	ON_CBN_SELCHANGE(IDC_FONT_KEY_CHOOSE_CUR, OnCbnSelchangeFontKeyCur)
END_MESSAGE_MAP()


BOOL CTexFontEditor::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL re = CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	UpdateData( FALSE );
	m_pTex = NULL;

	m_FontMgr = CTextureFontMgr::GetInst();
	RefreshFontShow();
	m_TexKeySel.SetItemHeight(-1,12);
	m_TexKeySelCur.SetItemHeight(-1,12);
	m_TexFontSel.SetItemHeight(-1,12);

	string temp_str;
	for (int i = 1; i <= FontMaxKey; ++i)
	{
		tostring(temp_str, i);
		m_TexKeySel.AddString(utf8_to_utf16(temp_str).c_str());
		m_TexKeySelCur.AddString(utf8_to_utf16(temp_str).c_str());
	}

	m_pTex = NULL;
	return re;
}

#ifdef _DEBUG
void CTexFontEditor::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTexFontEditor::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CTexFontEditor::OnBnClickedChooseTex()
{
	if( NULL == CTextureFontMgr::GetInst() )
		return;
	// TODO: Add your control notification handler code here
	//CString FileName = GetFileName(true, L"GUI Texture (*.tex *.dds)|*.tex;*.dds||");

	vector<CString> vecFiles;
	vecFiles = GetMutiFileNameTex( L"GUI Texture (*.tex *.dds)|*.tex;*.dds||" );

	if (m_pTex)
	{
		SafeRelease(m_pTex);
		m_pTex = NULL;
	}
	if( vecFiles.size() && !vecFiles[0].IsEmpty() )
	{
		m_Texture.SetWindowText( vecFiles[0] );
		CString temp_wstr;
		m_Texture.GetWindowText(temp_wstr);
		CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str(), utf16_to_utf8(temp_wstr.GetString()).c_str(), &m_pTex );
		m_strTex = utf16_to_utf8(temp_wstr.GetString()).c_str();
	}
	else
	{
		m_Texture.SetWindowText(L"");
		m_strTex = "";
	}

	RefreshFontShow();
}

void CTexFontEditor::RefreshFontShow()
{
	if (!m_pTex)
		return;
	RefreshView();
}

void CTexFontEditor::RefreshFontAll()
{
	while( m_TexFontSel.DeleteString( 0 ) >=0 );
	if ( m_FontMgr )
	{
		// 通过当前选中的key
		CString temp_wstr;
		if (m_TexKeySelCur.GetCurSel() == -1)
		{
			RefreshView();
			return;
		}
		m_TexKeySelCur.GetLBText(m_TexKeySelCur.GetCurSel(),temp_wstr);
		string stridx = utf16_to_utf8(temp_wstr.GetString()).c_str();

		int curkey = integer(stridx);
		if (curkey > 0 && curkey <= FontMaxKey)
		{
			// 刷新列表
			CTextureFontMgr::TextureFontIIMap::iterator it,eit = m_FontMgr->TexEnd();
			for ( it = m_FontMgr->TexBegin(); it != eit; ++it )
			{
				if (it->first.second == curkey)
				{
					wstring		temp_Font = L"F";
					temp_Font[0] = it->first.first;
					m_TexFontSel.AddString(temp_Font.c_str());
				}
			}
		}
	}
	RefreshView();
}

void CTexFontEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		RefreshFontAll();
		m_FontRender.BeginRenderer();
	}
	else
		m_FontRender.EndRenderer();
	m_bIsButtonDown = false;
	RefreshView();
}

void CTexFontEditor::RefreshView(void)
{
	m_FontRender.SetBackTexture(m_pTex);
	m_FontRender.SetDrawRect(m_Rect);

	CString Temp;
	Temp.Format(L"%d",int(m_Rect.left));
	m_Fontx.SetWindowText(Temp.GetBuffer());

	Temp.Format(L"%d",int(m_Rect.top));
	m_Fonty.SetWindowText(Temp.GetBuffer());

	Temp.Format(L"%d",int(m_Rect.right - m_Rect.left));
	m_Fontw.SetWindowText(Temp.GetBuffer());

	Temp.Format(L"%d",int(m_Rect.bottom - m_Rect.top));
	m_Fonth.SetWindowText(Temp.GetBuffer());
}

void CTexFontEditor::OnBnClickedDelFont()
{
	// 通过当前选中的key
	CString temp_wstr;
	m_TexKeySelCur.GetWindowText(temp_wstr);
	string stridx = utf16_to_utf8(temp_wstr.GetString()).c_str();

	int curkey = integer(stridx);
	if (curkey > 0 && curkey <= FontMaxKey)
	{
		CString temp_word;
		m_TexFontSel.GetWindowText(temp_word);

		uint32 curinx = m_TexFontSel.GetCurSel();
		m_TexFontSel.DeleteString( curinx );

		uint32 curword;
		std::wstring wstr_word = temp_word.GetString();
		if (wstr_word.size() == 0)
			return;
		curword = wstr_word[0];

		IdxKey keyIdx = make_pair(curword, (uint32)curkey);
		CTextureFontMgr::TextureFontIIMap::iterator it = m_FontMgr->TexIImap().find(keyIdx);
		if (it != m_FontMgr->TexEnd())
			m_FontMgr->TexIImap().erase(it);
	}
	// 刷新
	RefreshFontAll();
}

void CTexFontEditor::OnBnClickedEditOk()
{
	// 这个不是添加，是修改
	CString temp_wstr;
	m_TexKeySelCur.GetWindowText(temp_wstr);
	string stridx = utf16_to_utf8(temp_wstr.GetString()).c_str();

	int curkey = integer(stridx);
	if (curkey > 0 && curkey <= FontMaxKey)
	{
		CString temp_word;
		m_TexFontSel.GetWindowText(temp_word);
		uint32 curinx = m_TexFontSel.GetCurSel();

		std::wstring temp_str_word = temp_word.GetString();
		if (temp_str_word.size() == 0)
			return;
		uint32 curword = temp_str_word[0];

		IdxKey keyIdx = make_pair(curword, curkey);
		CTextureFontMgr::TextureFontIIMap::iterator it = m_FontMgr->TexIImap().find(keyIdx);
		if (it != m_FontMgr->TexEnd())
		{
			m_Fontx.GetWindowText(temp_wstr);
			string str_x = utf16_to_utf8(temp_wstr.GetString()).c_str();
			m_Fonty.GetWindowText(temp_wstr);
			string str_y = utf16_to_utf8(temp_wstr.GetString()).c_str();
			m_Fontw.GetWindowText(temp_wstr);
			string str_w = utf16_to_utf8(temp_wstr.GetString()).c_str();
			m_Fonth.GetWindowText(temp_wstr);
			string str_h = utf16_to_utf8(temp_wstr.GetString()).c_str();
			uint32 x, y, w, h;
			uinteger(str_x, x); uinteger(str_y, y); uinteger(str_w, w); uinteger(str_h, h);

			it->second->EmtImg.texRt.top	= (float)(y);
			it->second->EmtImg.texRt.bottom= (float)(y + h);
			it->second->EmtImg.texRt.left	= (float)(x);
			it->second->EmtImg.texRt.right	= (float)(x + w);
			m_Rect = it->second->EmtImg.texRt;
		}
	}
	RefreshView();
}

void CTexFontEditor::OnBnClickedAddFont()
{
	CString temp_wstr;
	m_Texture.GetWindowText(temp_wstr);
	m_strTex = utf16_to_utf8(temp_wstr.GetString()).c_str();
	if (m_strTex.size() == 0)
		return;

	m_TexKeySel.GetWindowText(temp_wstr);
	string stridx = utf16_to_utf8(temp_wstr.GetString()).c_str();

	int curkey = integer(stridx);
	if (curkey > 0 && curkey <= FontMaxKey)
	{
		m_FontName.GetWindowText(temp_wstr);
		std::wstring temp_str = temp_wstr.GetString();
		if (temp_str.size() == 0)
			return;
		uint32 curword = temp_str[0];
		
		// default
		CFRect		temp_pos;
		wstring		temp_des = L"贴图字";
		temp_pos.top = 0;	temp_pos.bottom = 32;	temp_pos.left = 0;	temp_pos.right = 32;

		if ( CGraphic::GetInst() )
		{
			IdxKey keyIdx = make_pair(curword, curkey);
			if (m_FontMgr->TexIImap().find(keyIdx) != m_FontMgr->TexEnd() )
				return;

			TextureFontInfoII* pTex = new TextureFontInfoII();
			pTex->Init(CGraphic::GetInst(), keyIdx, m_strTex.c_str(), temp_pos, temp_des.c_str());

			m_FontMgr->TexIImap().insert(CTextureFontMgr::TextureFontIIMap::value_type(pTex->TexIdx, pTex));
		}
	}
	// 刷新
	RefreshFontAll();
}

void CTexFontEditor::OnBnClickedSaveALL()
{
	// save the data that the editor have
	string save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) 
		+ "/" + utf16_to_gbk(m_TexFontPath);
	ClearResData(eRes_T_TexFt);
	TiXmlDocument		TexFontDoc;
	TiXmlElement		TexFontRoot(CTextureFontMgr::Emt_Root.c_str());

	CTextureFontMgr::TextureFontIIMap::iterator it = m_FontMgr->TexBegin();
	CTextureFontMgr::TextureFontIIMap::iterator eit = m_FontMgr->TexEnd();
	while( it != eit )
	{
		TiXmlElement		TexFontNode(CTextureFontMgr::Emt_Face.c_str());
		TiXmlElement		TexFontTex(CTextureFontMgr::Emt_Fc_Tex.c_str());

		int32 font_id = (int32)(it->first.first);
		int32 font_key = (int32)(it->first.second);

		int32 x = (int32)(it->second->EmtImg.texRt.left + 0.5f);
		int32 y = (int32)(it->second->EmtImg.texRt.top + 0.5f);
		int32 w = (int32)(it->second->EmtImg.texRt.right + 0.5f);
		int32 h = (int32)(it->second->EmtImg.texRt.bottom + 0.5f);

		string str_x, str_y, str_w, str_h;
		tostring(str_x, x); tostring(str_y, y); tostring(str_w, w); tostring(str_h, h);
		string temp_pos = str_x + "," + str_y + "," + str_w + "," + str_h;
		string temp_tip = utf16_to_utf8(it->second->EmtDes.c_str()).c_str();
		string temp_tex = it->second->EmtTexName.c_str();

		TexFontNode.SetAttribute(CTextureFontMgr::Emt_Fc_Id.c_str(),font_id);
		TexFontNode.SetAttribute(CTextureFontMgr::Emt_Fc_Key.c_str(),font_key);
		TexFontNode.SetAttribute(CTextureFontMgr::Emt_Fc_Pos.c_str(),temp_pos);
		TexFontNode.SetAttribute(CTextureFontMgr::Emt_Fc_Tip.c_str(),temp_tip);

		TiXmlText			TexFontText(temp_tex);
		AddResData(eRes_T_TexFt, temp_tex);
		TexFontTex.InsertEndChild(TexFontText);
		TexFontNode.InsertEndChild(TexFontTex);
		TexFontRoot.InsertEndChild(TexFontNode);

		it++;
	}

	TexFontDoc.InsertEndChild(TexFontRoot);
	TexFontDoc.SaveFile(save_path);

	// save texture res
	SaveResData();
}

void CTexFontEditor::OnCbnSelchangeFontKeyCur()
{
	RefreshFontAll();
}

void CTexFontEditor::OnCbnSelchangeFontCur()
{
	CString temp_wstr;
	m_Texture.GetWindowText(temp_wstr);
	m_strTex = utf16_to_utf8(temp_wstr.GetString()).c_str();

	m_TexKeySelCur.GetLBText(m_TexKeySelCur.GetCurSel(),temp_wstr);
	string stridx = utf16_to_utf8(temp_wstr.GetString()).c_str();

	int curkey = integer(stridx);
	if (curkey > 0 && curkey <= FontMaxKey)
	{
		m_TexFontSel.GetLBText(m_TexFontSel.GetCurSel(),temp_wstr);
		std::wstring stridx = temp_wstr.GetString();
		if (stridx.size() == 0)
			return;
		uint32 curword = stridx[0];
		IdxKey keyIdx = make_pair(curword, (uint32)curkey);
		CTextureFontMgr::TextureFontIIMap::iterator it = m_FontMgr->TexIImap().find(keyIdx);
		if (it != m_FontMgr->TexEnd())
		{
			string temp_tex = it->second->EmtTexName.c_str();
			if (temp_tex != m_strTex)
			{
				if (m_pTex)
				{
					SafeRelease(m_pTex);
					m_pTex = NULL;
				}
				if( temp_tex.size() )
				{
					m_Texture.SetWindowText( utf8_to_utf16(temp_tex).c_str() );
					CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str(), 
							temp_tex.c_str(), &m_pTex );
					m_strTex = temp_tex;
				}
			}
			// refresh the content
			int32 x = (int32)(it->second->EmtImg.texRt.left + 0.5f);
			int32 y = (int32)(it->second->EmtImg.texRt.top + 0.5f);
			int32 w = (int32)(it->second->EmtImg.texRt.Width() + 0.5f);
			int32 h = (int32)(it->second->EmtImg.texRt.Height() + 0.5f);
			string str_x, str_y, str_w, str_h;
			tostring(str_x, x); tostring(str_y, y); tostring(str_w, w); tostring(str_h, h);
			m_Fontx.SetWindowText( utf8_to_utf16(str_x).c_str() );
			m_Fonty.SetWindowText( utf8_to_utf16(str_y).c_str() );
			m_Fontw.SetWindowText( utf8_to_utf16(str_w).c_str() );
			m_Fonth.SetWindowText( utf8_to_utf16(str_h).c_str() );

			m_Rect = it->second->EmtImg.texRt;
		}
	}
	RefreshView();
}

void CTexFontEditor::ProcEditRect(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	if (!m_pTex)
		return;

	if (!(m_FontRender.IsStateInRender()))
		return;
	float X = float( LOWORD(lParam) );
	float Y = float( HIWORD(lParam) );
	X = limit2(X,0.0f,float(m_pTex->GetWidth()));
	Y = limit2(Y,0.0f,float(m_pTex->GetHeight()));
	switch( uMessage ) 
	{
	case WM_MOUSEMOVE:
		if(m_bIsButtonDown)
		{
			if(Y>m_MouseButtonY)
			{
				m_Rect.top		= m_MouseButtonY;
				m_Rect.bottom	= Y;
			}
			else
			{
				m_Rect.top		= Y;
				m_Rect.bottom	= m_MouseButtonY;
			}


			if(X>m_MouseButtonX)
			{
				m_Rect.left	= m_MouseButtonX;
				m_Rect.right	= X;
			}
			else
			{
				m_Rect.left	= m_MouseButtonX;
				m_Rect.right	= X;
			}
			RefreshView();
		}
		break;
	case WM_LBUTTONUP:
		m_bIsButtonDown = false;
		break;

	case WM_LBUTTONDOWN:
		m_bIsButtonDown = true;
		m_MouseButtonX = X;
		m_MouseButtonY = Y;
		break;
	default:
		break;
	}
}
