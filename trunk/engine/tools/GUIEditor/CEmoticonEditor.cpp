#include "stdafx.h"
#include "GUIEditor.h"
#include "CEmoticonEditor.h"
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

IMPLEMENT_DYNCREATE(CTextureFontEditor, CFormView)

CTextureFontEditor::CTextureFontEditor()
: CFormView(CTextureFontEditor::IDD)
{
}

CTextureFontEditor::~CTextureFontEditor()
{
}

void CTextureFontEditor::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FACE_SELECT_CUR, m_EmtSel);
	DDX_Control(pDX, IDC_FACE_SELECT_ALL, m_EmtAllSel);

	DDX_Control(pDX, IDC_FACE_TEXTURE, m_Texture);
	DDX_Control(pDX, IDC_FACE_X, m_Emtx);
	DDX_Control(pDX, IDC_FACE_Y, m_Emty);
	DDX_Control(pDX, IDC_FACE_W, m_Emtw);
	DDX_Control(pDX, IDC_FACE_H, m_Emth);
	DDX_Control(pDX, IDC_FACE_NEW, m_EmtNew);
	

	DDX_Control(pDX, IDC_FACE_ID_FROM, m_IdFrom);
	DDX_Control(pDX, IDC_FACE_ID_TO, m_IdTo);
	DDX_Control(pDX, IDC_FACE_W_ALL, m_WAll);
	DDX_Control(pDX, IDC_FACE_H_ALL, m_HAll);
	DDX_Control(pDX, IDC_FACE_COUNT, m_EmtCount);
}

BEGIN_MESSAGE_MAP(CTextureFontEditor, CFormView)
	ON_BN_CLICKED(IDC_FACE_CHOOSE_TEX, OnBnClickedChooseTex)
	ON_BN_CLICKED(IDC_FACE_DEL, OnBnClickedDelEmt)
	ON_BN_CLICKED(IDC_FACE_OK, OnBnClickedAddEmt)
	ON_BN_CLICKED(IDC_FACE_DEL_ALL, OnBnClickedDelAllEmt)
	ON_BN_CLICKED(IDC_FACE_ADD_ALL, OnBnClickedAddAllEmt)
	ON_BN_CLICKED(IDC_FACE_SAVE, OnBnClickedSaveALL)
	ON_BN_CLICKED(IDC_FACE_ADD_NEW, OnBnClickedAddNew)

	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_FACE_SELECT_CUR, OnCbnSelchangeEmtCur)
	ON_CBN_SELCHANGE(IDC_FACE_SELECT_ALL, OnCbnSelchangeEmtAll)
	
END_MESSAGE_MAP()


BOOL CTextureFontEditor::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL re = CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	UpdateData( FALSE );
	m_pTex = NULL;

	m_EmtMgr = CTextureFontMgr::GetInst();
	RefreshEmtAll();
	m_EmtSel.SetItemHeight(-1,12);
	m_EmtAllSel.SetItemHeight(-1,12);
	m_pTex = NULL;
	return re;
}

#ifdef _DEBUG
void CTextureFontEditor::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTextureFontEditor::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CTextureFontEditor::OnBnClickedChooseTex()
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

	RefreshEmtShow();
}

void CTextureFontEditor::RefreshEmtShow()
{
	if (!m_pTex)
		return;
	// 刷新表情列表
	while( m_EmtSel.DeleteString( 0 ) >=0 );
	CString temp_wstr;
	m_Texture.GetWindowText(temp_wstr);
	m_strTex = utf16_to_utf8(temp_wstr.GetString()).c_str();
	if( m_EmtMgr )
	{
		CTextureFontMgr::EmtIndexMap::iterator itIdx,eitIdx = m_EmtMgr->EmtIdxEnd();
		CTextureFontMgr::TextureFontMap::iterator it,eit = m_EmtMgr->EmtEnd();

		for ( itIdx = m_EmtMgr->EmtIdxBegin(); itIdx != eitIdx; ++itIdx )
		{
			uint32 idx = (uint32)((itIdx->second)[0]);
			it = m_EmtMgr->TextureFontmap().find(idx);
			if (it != eit)
			{
				if (m_strTex == it->second->EmtTexName.c_str())
				{
					m_EmtSel.AddString(utf8_to_utf16(itIdx->first.c_str()).c_str());
				}
			}
		}
	}
	RefreshView();
}

void CTextureFontEditor::RefreshEmtAll()
{
	while( m_EmtAllSel.DeleteString( 0 ) >=0 );
	if ( m_EmtMgr )
	{
		CTextureFontMgr::EmtIndexMap::iterator it,eit = m_EmtMgr->EmtIdxEnd();
		for ( it = m_EmtMgr->EmtIdxBegin(); it != eit; ++it )
			m_EmtAllSel.AddString(utf8_to_utf16(it->first.c_str()).c_str());
	}
	RefreshView();
}

void CTextureFontEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		RefreshEmtAll();
		RefreshEmtShow();
		m_EmtRender.BeginRenderer();
	}
	else
		m_EmtRender.EndRenderer();
	RefreshView();
}

void CTextureFontEditor::RefreshView(void)
{
	m_EmtRender.SetBackTexture(m_pTex);
	m_EmtRender.SetDrawRect(m_Rect);
}

void CTextureFontEditor::OnBnClickedDelEmt()
{
	CString temp_wstr;
	m_EmtSel.GetWindowText(temp_wstr);
	string stridx = utf16_to_utf8(temp_wstr.GetString()).c_str();
	uint32 curinx = m_EmtSel.GetCurSel();
	m_EmtSel.DeleteString( curinx );

	CTextureFontMgr::EmtIndexMap::iterator it = m_EmtMgr->EmtIndexmap().find(stridx.c_str());
	if (it != m_EmtMgr->EmtIdxEnd())
	{
		uint32 emtidx = (uint32)((it->second)[0]);
		m_EmtMgr->EmtIndexmap().erase(it);

		CTextureFontMgr::TextureFontMap::iterator itr = m_EmtMgr->TextureFontmap().find(emtidx);
		if (itr != m_EmtMgr->EmtEnd())
		{
			m_EmtMgr->EraserTextureFont(itr);
		}
	}

	// 刷新
	RefreshEmtAll();
	RefreshEmtShow();
}

void CTextureFontEditor::OnBnClickedAddEmt()
{
	// 这个不是添加，是修改
	CString temp_wstr;
	m_EmtSel.GetWindowText(temp_wstr);
	string stridx = utf16_to_utf8(temp_wstr.GetString()).c_str();
	CTextureFontMgr::EmtIndexMap::iterator it = m_EmtMgr->EmtIndexmap().find(stridx.c_str());
	if (it != m_EmtMgr->EmtIdxEnd())
	{
		uint32 emtidx = (uint32)((it->second)[0]);
		CTextureFontMgr::TextureFontMap::iterator itr = m_EmtMgr->TextureFontmap().find(emtidx);

		if (itr != m_EmtMgr->EmtEnd())
		{
			m_Emtx.GetWindowText(temp_wstr);
			string str_x = utf16_to_utf8(temp_wstr.GetString()).c_str();
			m_Emty.GetWindowText(temp_wstr);
			string str_y = utf16_to_utf8(temp_wstr.GetString()).c_str();
			m_Emtw.GetWindowText(temp_wstr);
			string str_w = utf16_to_utf8(temp_wstr.GetString()).c_str();
			m_Emth.GetWindowText(temp_wstr);
			string str_h = utf16_to_utf8(temp_wstr.GetString()).c_str();
			
			uint32 x, y, w, h;
			uinteger(str_x, x); uinteger(str_y, y); uinteger(str_w, w); uinteger(str_h, h);
			itr->second->EmtImg.texRt.top		= (float)y;
			itr->second->EmtImg.texRt.bottom	= (float)(y + h);
			itr->second->EmtImg.texRt.left		= (float)x;
			itr->second->EmtImg.texRt.right		= (float)(x + w);

			m_Rect = itr->second->EmtImg.texRt;
		}
	}
	RefreshView();
}



void CTextureFontEditor::OnBnClickedDelAllEmt()
{
	CString temp_wstr;
	m_IdFrom.GetWindowText(temp_wstr);
	string str_f = utf16_to_utf8(temp_wstr.GetString()).c_str();
	m_IdTo.GetWindowText(temp_wstr);
	string str_t = utf16_to_utf8(temp_wstr.GetString()).c_str();
	
	uint32 int_f, int_t;
	uinteger(str_f, int_f); uinteger(str_t, int_t);
	for(uint32 i = int_f; i <= int_t; i++)
	{
		string str_idx;
		tostring( str_idx, i );
		ReBuiltU32StrToIdx( str_idx );

		// str_idx del
		CTextureFontMgr::EmtIndexMap::iterator it = m_EmtMgr->EmtIndexmap().find(str_idx.c_str());
		if (it != m_EmtMgr->EmtIdxEnd())
		{
			uint32 emtidx = (uint32)((it->second)[0]);
			m_EmtMgr->EmtIndexmap().erase(it);

			CTextureFontMgr::TextureFontMap::iterator itr = m_EmtMgr->TextureFontmap().find(emtidx);
			if (itr != m_EmtMgr->EmtEnd())
			{
				m_EmtMgr->EraserTextureFont(itr);
			}
		}
	}

	// 刷新
	RefreshEmtAll();
	RefreshEmtShow();
}

void CTextureFontEditor::ReBuiltU32StrToIdx(string& val)
{
	size_t pos = val.find_first_of(',');
	while( pos != string::npos)
	{
		val.erase(pos,1);
		pos = val.find_first_of(',');
	}

	string head = EMT_IDX_STR;
	size_t lth = EMT_IDX_LTH - val.size() + 1;
	val = head.substr(0,lth) + val;
	//val = head + val;
}

void CTextureFontEditor::OnBnClickedAddNew()
{
	CString temp_wstr;
	m_Texture.GetWindowText(temp_wstr);
	m_strTex = utf16_to_utf8(temp_wstr.GetString()).c_str();
	if (m_strTex.size() == 0)
		return;

	m_EmtNew.GetWindowText(temp_wstr);
	string temp_str = utf16_to_utf8(temp_wstr.GetString()).c_str();
	int32 temp_int;
	integer(temp_str, temp_int);
	tostring( temp_str, temp_int );
	ReBuiltU32StrToIdx( temp_str );

	uint32	temp_idx = temp_int + EMT_FONT_BEGIN;

	wstring		temp_strId = L"F";
	CFRect		temp_pos; 
	wstring		temp_des = L"表情";

	// default
	temp_pos.top = 0;
	temp_pos.bottom = 10;
	temp_pos.left = 0;
	temp_pos.right = 10;
	temp_strId[0] = temp_idx;

	if ( CGraphic::GetInst() )
	{
		TextureFontInfo* pEmt = new TextureFontInfo();
		pEmt->Init(CGraphic::GetInst(), temp_idx, m_strTex.c_str(), temp_pos, temp_des.c_str());
		// emt map
		m_EmtMgr->TextureFontmap().insert(pair<uint32, TextureFontInfo*>(pEmt->EmtIdx, pEmt));
		// id map
		m_EmtMgr->EmtIndexmap().insert(pair<GString, GWString>(temp_str.c_str(), temp_strId.c_str()));
	}

	// 刷新
	RefreshEmtAll();
	RefreshEmtShow();
}

void CTextureFontEditor::OnBnClickedAddAllEmt()
{
	CString temp_wstr;
	m_IdFrom.GetWindowText(temp_wstr);
	string str_f = utf16_to_utf8(temp_wstr.GetString()).c_str();
	m_IdTo.GetWindowText(temp_wstr);
	string str_t = utf16_to_utf8(temp_wstr.GetString()).c_str();

	m_WAll.GetWindowText(temp_wstr);
	string str_w = utf16_to_utf8(temp_wstr.GetString()).c_str();
	m_HAll.GetWindowText(temp_wstr);
	string str_h = utf16_to_utf8(temp_wstr.GetString()).c_str();
	
	m_EmtCount.GetWindowText(temp_wstr);
	string str_count = utf16_to_utf8(temp_wstr.GetString()).c_str();

	uint32 int_f, int_t, int_w, int_h, int_count;
	uint32 counter = 0;

	uinteger(str_count, int_count);
	uinteger(str_f, int_f); uinteger(str_t, int_t); uinteger(str_w, int_w); uinteger(str_h, int_h);

	for(uint32 i = int_f; i <= int_t; i++)
	{
		string str_idx;
		tostring( str_idx, i );
		ReBuiltU32StrToIdx( str_idx );

		// str_idx add
		CTextureFontMgr::EmtIndexMap::iterator it = m_EmtMgr->EmtIndexmap().find(str_idx.c_str());
		if (it == m_EmtMgr->EmtIdxEnd())
		{
			uint32		temp_idx = i + EMT_FONT_BEGIN;
			if (temp_idx > EMT_FONT_END)
				break;
			
			m_Texture.GetWindowText(temp_wstr);
			string		temp_texname = utf16_to_utf8(temp_wstr.GetString()).c_str();
			wstring		temp_strId = L"F";
			CFRect		temp_pos; 
			wstring		temp_des = L"表情";

			temp_pos.top = (float)(counter/int_count)*int_h;
			temp_pos.bottom = (float)(temp_pos.top + int_h);
			temp_pos.left = (float)(counter%int_count)*int_w;
			temp_pos.right = (float)(temp_pos.left + int_w);

			// STRING IDX
			temp_strId[0] = temp_idx;

			if ( CGraphic::GetInst() )
			{
				TextureFontInfo* pEmt = new TextureFontInfo();
				pEmt->Init(CGraphic::GetInst(), temp_idx, temp_texname.c_str(), temp_pos, temp_des.c_str());

				// emt map
				 m_EmtMgr->TextureFontmap().insert(pair<uint32, TextureFontInfo*>(pEmt->EmtIdx, pEmt));
				// id map
				m_EmtMgr->EmtIndexmap().insert(pair<GString, GWString>(str_idx.c_str(), temp_strId.c_str()));
				counter++;
			}
		}
		else
		{
			// notice
		}
	}

	// 刷新
	RefreshEmtAll();
	RefreshEmtShow();
}

void CTextureFontEditor::OnBnClickedSaveALL()
{
	// save the data that the editor have
	string save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) 
						   + "/" + utf16_to_gbk(m_emtPath);
	ClearResData(eRes_T_Emt);
	TiXmlDocument		EmtDoc;
	TiXmlElement		EmtRoot(::sqr::CTextureFontMgr::Emt_Root.c_str());

	CTextureFontMgr::EmtIndexMap::iterator it = m_EmtMgr->EmtIndexmap().begin();
	CTextureFontMgr::EmtIndexMap::iterator eit = m_EmtMgr->EmtIndexmap().end();
	while( it != eit )
	{
		
		string	temp_idx = it->first.c_str();
		uint32	temp_id = (uint32)((it->second)[0]);

		CTextureFontMgr::TextureFontMap::iterator itr = m_EmtMgr->TextureFontmap().find(temp_id);
		if (itr != m_EmtMgr->EmtEnd())
		{
			TiXmlElement		EmtFace(::sqr::CTextureFontMgr::Emt_Face.c_str());
			TiXmlElement		EmtTex(::sqr::CTextureFontMgr::Emt_Fc_Tex.c_str());
			

			int32 x = (int32)(itr->second->EmtImg.texRt.left + 0.5f);
			int32 y = (int32)(itr->second->EmtImg.texRt.top + 0.5f);
			int32 w = (int32)(itr->second->EmtImg.texRt.right + 0.5f);
			int32 h = (int32)(itr->second->EmtImg.texRt.bottom + 0.5f);

			string str_x, str_y, str_w, str_h;
			tostring(str_x, x); tostring(str_y, y); tostring(str_w, w); tostring(str_h, h);
			string temp_pos = str_x + "," + str_y + "," + str_w + "," + str_h;
			string temp_tip = utf16_to_gbk(itr->second->EmtDes.c_str());
			string temp_tex = itr->second->EmtTexName.c_str();

			EmtFace.SetAttribute(::sqr::CTextureFontMgr::Emt_Fc_Id.c_str(),(int32)temp_id);
			EmtFace.SetAttribute(::sqr::CTextureFontMgr::Emt_Fc_Idx.c_str(),temp_idx);
			EmtFace.SetAttribute(::sqr::CTextureFontMgr::Emt_Fc_Pos.c_str(),temp_pos);
			EmtFace.SetAttribute(::sqr::CTextureFontMgr::Emt_Fc_Tip.c_str(),temp_tip);
			TiXmlText			EmtText(temp_tex);
			AddResData(eRes_T_Emt, temp_tex);
			
			EmtTex.InsertEndChild(EmtText);
			EmtFace.InsertEndChild(EmtTex);
			EmtRoot.InsertEndChild(EmtFace);
		}
		it++;
	}

	EmtDoc.InsertEndChild(EmtRoot);
	EmtDoc.SaveFile(save_path);
	
	// save texture res
	SaveResData();
}

void CTextureFontEditor::OnCbnSelchangeEmtCur()
{
	CString temp_wstr;
	m_EmtSel.GetWindowText(temp_wstr);
	string str_idx = utf16_to_utf8(temp_wstr.GetString()).c_str();
	CTextureFontMgr::EmtIndexMap::iterator it = m_EmtMgr->EmtIndexmap().find(str_idx.c_str());
	if (it != m_EmtMgr->EmtIdxEnd())
	{
		uint32 emtidx = (uint32)((it->second)[0]);

		CTextureFontMgr::TextureFontMap::iterator itr = m_EmtMgr->TextureFontmap().find(emtidx);
		if (itr != m_EmtMgr->EmtEnd())
		{
			// refresh the content
			int32 x = (int32)(itr->second->EmtImg.texRt.left + 0.5f);
			int32 y = (int32)(itr->second->EmtImg.texRt.top + 0.5f);
			int32 w = (int32)(itr->second->EmtImg.texRt.right - itr->second->EmtImg.texRt.left + 0.5f);
			int32 h = (int32)(itr->second->EmtImg.texRt.bottom - itr->second->EmtImg.texRt.top + 0.5f);

			string str_x, str_y, str_w, str_h;
			tostring(str_x, x); tostring(str_y, y); tostring(str_w, w); tostring(str_h, h);
			m_Emtx.SetWindowText(utf8_to_utf16(str_x).c_str());
			m_Emty.SetWindowText(utf8_to_utf16(str_y).c_str());
			m_Emtw.SetWindowText(utf8_to_utf16(str_w).c_str());
			m_Emth.SetWindowText(utf8_to_utf16(str_h).c_str());

			m_Rect = itr->second->EmtImg.texRt;
		}
	}

	RefreshView();
}

void CTextureFontEditor::OnCbnSelchangeEmtAll()
{
	CString temp_wstr;
	m_Texture.GetWindowText(temp_wstr);
	m_strTex = utf16_to_utf8(temp_wstr.GetString()).c_str();

	m_EmtAllSel.GetWindowText(temp_wstr);
	string str_idx = utf16_to_utf8(temp_wstr.GetString()).c_str();
	CTextureFontMgr::EmtIndexMap::iterator it = m_EmtMgr->EmtIndexmap().find(str_idx.c_str());
	if (it != m_EmtMgr->EmtIdxEnd())
	{
		uint32 emtidx = (uint32)((it->second)[0]);

		CTextureFontMgr::TextureFontMap::iterator itr = m_EmtMgr->TextureFontmap().find(emtidx);
		if (itr != m_EmtMgr->EmtEnd())
		{
			// refresh the content
			string temp_str = itr->second->EmtTexName.c_str();
			if (temp_str != m_strTex)
			{
				if (m_pTex)
				{
					SafeRelease(m_pTex);
					m_pTex = NULL;
				}
				if( temp_str.size() )
				{
					m_Texture.SetWindowText( utf8_to_utf16(temp_str).c_str() );
					CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str(), temp_str.c_str(), &m_pTex );
					m_strTex = temp_str;
				}
			}
		}
	}

	RefreshEmtShow();
}
