#include "stdafx.h"
#include "GUIEditor.h"
#include "CFlashEditor.h"
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

IMPLEMENT_DYNCREATE(CFlashEditor, CFormView)

CFlashEditor::CFlashEditor()
: CFormView(CFlashEditor::IDD)
{
}

CFlashEditor::~CFlashEditor()
{
}

void CFlashEditor::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FLASH_NAME_CHOOSE, m_FlashSel);
	DDX_Control(pDX, IDC_FLASH_TEX, m_Texture);
	DDX_Control(pDX, IDC_FLASH_NAME_EDIT, m_FlashName);

	DDX_Control(pDX, IDC_FLASH_POSX, m_Flashx);
	DDX_Control(pDX, IDC_FLASH_POSY, m_Flashy);
	DDX_Control(pDX, IDC_FLASH_POSW, m_Flashw);
	DDX_Control(pDX, IDC_FLASH_POSH, m_Flashh);

	DDX_Control(pDX, IDC_FLASH_REPOS_LX, m_FlashLx);
	DDX_Control(pDX, IDC_FLASH_REPOS_RX, m_FlashRx);
	DDX_Control(pDX, IDC_FLASH_REPOS_TY, m_FlashTy);
	DDX_Control(pDX, IDC_FLASH_REPOS_BY, m_FlashBy);

	DDX_Control(pDX, IDC_FLASH_AMAX, m_AMax);
	DDX_Control(pDX, IDC_FLASH_AMIN, m_AMin);
	DDX_Control(pDX, IDC_FLASH_TIME, m_VTime);
	DDX_Control(pDX, IDC_FLASH_COUNT, m_Count);
}

BEGIN_MESSAGE_MAP(CFlashEditor, CFormView)
	ON_BN_CLICKED(IDC_FLASH_CHOOSE_TEX, OnBnClickedChooseTex)
	ON_BN_CLICKED(IDC_FLASH_ADD, OnBnClickedAddFlash)
	ON_BN_CLICKED(IDC_FLASH_DEL, OnBnClickedDelFlash)
	ON_BN_CLICKED(IDC_FLASH_EDIT_OK, OnBnClickedEditOk)
	ON_BN_CLICKED(IDC_FLASH_SAVE_ALL, OnBnClickedSaveALL)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_FLASH_NAME_CHOOSE, OnCbnSelchangeFlashCur)
END_MESSAGE_MAP()


BOOL CFlashEditor::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL re = CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	UpdateData( FALSE );
	m_pTex = NULL;

	m_FlashMgr = FlashDataManager::GetInst();
	RefreshFlashShow();
	m_FlashSel.SetItemHeight(-1,12);
	m_pTex = NULL;
	return re;
}

#ifdef _DEBUG
void CFlashEditor::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFlashEditor::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CFlashEditor::OnBnClickedChooseTex()
{
	if( NULL == FlashDataManager::GetInst() )
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

	RefreshFlashShow();
}

void CFlashEditor::RefreshFlashShow()
{
	if (!m_pTex)
		return;
	RefreshView();
}

void CFlashEditor::RefreshFlashAll()
{
	while( m_FlashSel.DeleteString( 0 ) >=0 );
	if ( m_FlashMgr )
	{
		FlashDataManager::FlashInfoMap::iterator it,eit = m_FlashMgr->FlashEnd();
		for ( it = m_FlashMgr->FlashBegin(); it != eit; ++it )
			m_FlashSel.AddString(utf8_to_utf16(it->first.c_str()).c_str());
	}
	RefreshView();
}

void CFlashEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		RefreshFlashAll();
		m_FlashRender.BeginRenderer();
	}
	else
		m_FlashRender.EndRenderer();
	RefreshView();
}

void CFlashEditor::RefreshView(void)
{
	m_FlashRender.SetBackTexture(m_pTex);
	m_FlashRender.SetDrawRect(m_Rect);
}

void CFlashEditor::OnBnClickedDelFlash()
{
	CString temp_wstr;
	m_FlashSel.GetWindowText(temp_wstr);
	string stridx = utf16_to_utf8(temp_wstr.GetString()).c_str();
	uint32 curinx = m_FlashSel.GetCurSel();
	m_FlashSel.DeleteString( curinx );

	FlashDataManager::FlashInfoMap::iterator it = m_FlashMgr->Flashmap().find(stridx.c_str());
	if (it != m_FlashMgr->FlashEnd())
		m_FlashMgr->Flashmap().erase(it);

	// 刷新
	RefreshFlashAll();
}

void CFlashEditor::OnBnClickedEditOk()
{
	// 这个不是添加，是修改
	CString temp_wstr;
	m_FlashSel.GetWindowText(temp_wstr);
	string stridx = utf16_to_utf8(temp_wstr.GetString()).c_str();
	FlashDataManager::FlashInfoMap::iterator it = m_FlashMgr->Flashmap().find(stridx.c_str());
	if (it != m_FlashMgr->FlashEnd())
	{
		m_Flashx.GetWindowText(temp_wstr);
		string str_x = utf16_to_utf8(temp_wstr.GetString()).c_str();
		m_Flashy.GetWindowText(temp_wstr);
		string str_y = utf16_to_utf8(temp_wstr.GetString()).c_str();
		m_Flashw.GetWindowText(temp_wstr);
		string str_w = utf16_to_utf8(temp_wstr.GetString()).c_str();
		m_Flashh.GetWindowText(temp_wstr);
		string str_h = utf16_to_utf8(temp_wstr.GetString()).c_str();
		uint32 x, y, w, h;
		uinteger(str_x, x); uinteger(str_y, y); uinteger(str_w, w); uinteger(str_h, h);

		m_FlashLx.GetWindowText(temp_wstr);
		string str_lx = utf16_to_utf8(temp_wstr.GetString()).c_str();
		m_FlashRx.GetWindowText(temp_wstr);
		string str_rx = utf16_to_utf8(temp_wstr.GetString()).c_str();
		m_FlashTy.GetWindowText(temp_wstr);
		string str_ty = utf16_to_utf8(temp_wstr.GetString()).c_str();
		m_FlashBy.GetWindowText(temp_wstr);
		string str_by = utf16_to_utf8(temp_wstr.GetString()).c_str();
		int32 lx, rx, ty, by;
		integer(str_lx, lx); integer(str_rx, rx); integer(str_ty, ty); integer(str_by, by);

		m_AMax.GetWindowText(temp_wstr);
		string str_amax = utf16_to_utf8(temp_wstr.GetString()).c_str();
		m_AMin.GetWindowText(temp_wstr);
		string str_amin = utf16_to_utf8(temp_wstr.GetString()).c_str();
		m_VTime.GetWindowText(temp_wstr);
		string str_vtime = utf16_to_utf8(temp_wstr.GetString()).c_str();
		m_Count.GetWindowText(temp_wstr);
		string str_count = utf16_to_utf8(temp_wstr.GetString()).c_str();
		uint32 amax, amin, vtime, count;
		uinteger(str_amax, amax); uinteger(str_amin, amin); uinteger(str_vtime, vtime); uinteger(str_count, count);

		it->second->m_count					= count;
		it->second->m_time					= vtime;

		float ato = (float)(amin / 1000.0f);
		float afrom = (float)(amax / 1000.0f);
		if (ato > afrom)
		{
			it->second->m_bAdd = false;			// 透明度减小
			it->second->m_Amax = ato;
			it->second->m_Amin = afrom;
		}
		else
		{
			it->second->m_bAdd = true;			// 透明度增加
			it->second->m_Amax = afrom;
			it->second->m_Amin = ato;
		}

		it->second->m_repos.top				= (float)(ty);
		it->second->m_repos.bottom			= (float)(by);
		it->second->m_repos.left			= (float)(lx);
		it->second->m_repos.right			= (float)(rx);

		it->second->m_image.rtTexture.top	= (float)(y);
		it->second->m_image.rtTexture.bottom= (float)(y + h);
		it->second->m_image.rtTexture.left	= (float)(x);
		it->second->m_image.rtTexture.right	= (float)(x + w);

		m_Rect = it->second->m_image.rtTexture;
	}
	RefreshView();
}

void CFlashEditor::OnBnClickedAddFlash()
{
	CString temp_wstr;
	m_Texture.GetWindowText(temp_wstr);
	m_strTex = utf16_to_utf8(temp_wstr.GetString()).c_str();
	if (m_strTex.size() == 0)
		return;

	m_FlashName.GetWindowText(temp_wstr);
	string temp_str = utf16_to_utf8(temp_wstr.GetString()).c_str();

	// default
	CFRect		temp_pos; 
	CFRect		temp_repos;
	float		temp_amax = 1.0f, temp_amin = 0.0f;
	temp_pos.top = 0;	temp_pos.bottom = 32;	temp_pos.left = 0;	temp_pos.right = 32;
	temp_repos.top = 0; temp_repos.bottom = 0;	temp_repos.left = 0;temp_repos.right = 0;
	uint32		temp_vtime = 1000, temp_count = 0;

	if ( CGraphic::GetInst() )
	{
		FlashWndInfo* pFlash = new FlashWndInfo();
		pFlash->Init(CGraphic::GetInst(), temp_str.c_str(), m_strTex.c_str(), temp_pos, 
					 temp_vtime, temp_count, temp_amin, temp_amax, temp_repos);
		// flash map
		m_FlashMgr->Flashmap().insert(pair<UIString, FlashWndInfo*>(temp_str.c_str(), pFlash));
	}

	// 刷新
	RefreshFlashAll();
}

void CFlashEditor::OnBnClickedSaveALL()
{
	// save the data that the editor have
	string save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) 
		+ "/" + utf16_to_gbk(m_FlashPath);
	ClearResData(eRes_T_Flash);
	TiXmlDocument		FlashDoc;
	TiXmlElement		FlashRoot(FlashDataManager::Flash_Root.c_str());

	FlashDataManager::FlashInfoMap::iterator it = m_FlashMgr->FlashBegin();
	FlashDataManager::FlashInfoMap::iterator eit = m_FlashMgr->FlashEnd();
	while( it != eit )
	{
		TiXmlElement		FlashNode(FlashDataManager::Flash_Info.c_str());
		TiXmlElement		FlashTex(FlashDataManager::Flash_Tex.c_str());
		
		string temp_name = it->second->m_FlashName.c_str();
		int32 x = (int32)(it->second->m_image.rtTexture.left + 0.5f);
		int32 y = (int32)(it->second->m_image.rtTexture.top + 0.5f);
		int32 w = (int32)(it->second->m_image.rtTexture.right + 0.5f);
		int32 h = (int32)(it->second->m_image.rtTexture.bottom + 0.5f);

		string str_x, str_y, str_w, str_h;
		tostring(str_x, x); tostring(str_y, y); tostring(str_w, w); tostring(str_h, h);
		string temp_pos = str_x + "," + str_y + "," + str_w + "," + str_h;

		int32 lf = (int32)(it->second->m_repos.left + 0.5f);
		int32 tp = (int32)(it->second->m_repos.top + 0.5f);
		int32 rt = (int32)(it->second->m_repos.right + 0.5f);
		int32 bm = (int32)(it->second->m_repos.bottom + 0.5f);

		string str_lf, str_tp, str_rt, str_bm;
		tostring(str_lf, lf); tostring(str_tp, tp); tostring(str_rt, rt); tostring(str_bm, bm);
		string temp_repos = str_lf + "," + str_tp + "," + str_rt + "," + str_bm;
		
		string temp_vtime, temp_count, temp_amax, temp_amin;
		int32 vtime = (int32)(it->second->m_time);
		int32 count = (int32)(it->second->m_count);
		int32 amax = (int32)(it->second->m_Amax * 1000);
		int32 amin = (int32)(it->second->m_Amin * 1000);

		int32 ato, afrom;
		if (it->second->m_bAdd)
		{
			ato = amin;
			afrom = amax;
		}
		else
		{
			ato = amax;
			afrom = amin;
		}

		tostring(temp_vtime, vtime); tostring(temp_count, count); tostring(temp_amax, afrom); tostring(temp_amin, ato);
		string temp_tex = it->second->m_image.texName.c_str();
		FlashNode.SetAttribute(FlashDataManager::Flash_Name.c_str(),temp_name);
		FlashNode.SetAttribute(FlashDataManager::Flash_Ato.c_str(),temp_amin);
		FlashNode.SetAttribute(FlashDataManager::Flash_Afrom.c_str(),temp_amax);
		FlashNode.SetAttribute(FlashDataManager::Flash_V.c_str(),temp_vtime);
		FlashNode.SetAttribute(FlashDataManager::Flash_Count.c_str(),temp_count);
		FlashNode.SetAttribute(FlashDataManager::Flash_Pos.c_str(),temp_pos);
		FlashNode.SetAttribute(FlashDataManager::Flash_RePos.c_str(),temp_repos);

		TiXmlText			FlashText(temp_tex);
		AddResData(eRes_T_Flash, temp_tex);
		FlashTex.InsertEndChild(FlashText);
		FlashNode.InsertEndChild(FlashTex);
		FlashRoot.InsertEndChild(FlashNode);

		it++;
	}

	FlashDoc.InsertEndChild(FlashRoot);
	FlashDoc.SaveFile(save_path);

	// save texture res
	SaveResData();
}

void CFlashEditor::OnCbnSelchangeFlashCur()
{
	CString temp_wstr;
	m_Texture.GetWindowText(temp_wstr);
	m_strTex = utf16_to_utf8(temp_wstr.GetString()).c_str();


	m_FlashSel.GetLBText(m_FlashSel.GetCurSel(),temp_wstr);
	//m_FlashSel.GetWindowText(temp_wstr);
	string str_idx = utf16_to_utf8(temp_wstr.GetString()).c_str();
	FlashDataManager::FlashInfoMap::iterator it = m_FlashMgr->Flashmap().find(str_idx.c_str());
	if (it != m_FlashMgr->FlashEnd())
	{
		// refresh the content
		string temp_str = it->second->m_image.texName.c_str();
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
		// refresh the content
		int32 x = (int32)(it->second->m_image.rtTexture.left + 0.5f);
		int32 y = (int32)(it->second->m_image.rtTexture.top + 0.5f);
		int32 w = (int32)(it->second->m_image.rtTexture.Width() + 0.5f);
		int32 h = (int32)(it->second->m_image.rtTexture.Height() + 0.5f);
		string str_x, str_y, str_w, str_h;
		tostring(str_x, x); tostring(str_y, y); tostring(str_w, w); tostring(str_h, h);

		int32 lf = (int32)(it->second->m_repos.left + 0.5f);
		int32 tp = (int32)(it->second->m_repos.top + 0.5f);
		int32 rt = (int32)(it->second->m_repos.right + 0.5f);
		int32 bm = (int32)(it->second->m_repos.bottom + 0.5f);

		string str_lf, str_tp, str_rt, str_bm;
		tostring(str_lf, lf); tostring(str_tp, tp); tostring(str_rt, rt); tostring(str_bm, bm);

		string temp_vtime, temp_count, temp_amax, temp_amin;
		int32 vtime = (int32)(it->second->m_time);
		int32 count = (int32)(it->second->m_count);
		int32 amax = (int32)(it->second->m_Amax * 1000);
		int32 amin = (int32)(it->second->m_Amin * 1000);

		tostring(temp_vtime, vtime); tostring(temp_count, count); tostring(temp_amax, amax); tostring(temp_amin, amin);
		
		m_Flashx.SetWindowText( utf8_to_utf16(str_x).c_str() );
		m_Flashy.SetWindowText( utf8_to_utf16(str_y).c_str() );
		m_Flashw.SetWindowText( utf8_to_utf16(str_w).c_str() );
		m_Flashh.SetWindowText( utf8_to_utf16(str_h).c_str() );

		m_FlashLx.SetWindowText( utf8_to_utf16(str_lf).c_str() );
		m_FlashRx.SetWindowText( utf8_to_utf16(str_rt).c_str() );
		m_FlashTy.SetWindowText( utf8_to_utf16(str_tp).c_str() );
		m_FlashBy.SetWindowText( utf8_to_utf16(str_bm).c_str() );

		if (it->second->m_bAdd)
		{
			m_AMax.SetWindowText( utf8_to_utf16(temp_amax).c_str() );
			m_AMin.SetWindowText( utf8_to_utf16(temp_amin).c_str() );
		}
		else
		{
			m_AMax.SetWindowText( utf8_to_utf16(temp_amin).c_str() );
			m_AMin.SetWindowText( utf8_to_utf16(temp_amax).c_str() );
		}
		
		m_VTime.SetWindowText( utf8_to_utf16(temp_vtime).c_str() );
		m_Count.SetWindowText( utf8_to_utf16(temp_count).c_str() );
	}
	RefreshView();
}
