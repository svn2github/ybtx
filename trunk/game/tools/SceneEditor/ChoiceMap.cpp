// ChoiceMap.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "NewSceneEditor.h"
#include "ChoiceMap.h"


// CChoiceMap dialog

IMPLEMENT_DYNAMIC(CChoiceMap, CDialog)

CChoiceMap::CChoiceMap(CWnd* pParent /*=NULL*/)
	: CDialog(CChoiceMap::IDD, pParent),m_bIsAutoOpenScene(false),m_bIsAutoOpenArea(false)
{

}

CChoiceMap::~CChoiceMap()
{
}

void CChoiceMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAP_LIST, m_listMapName);
}


BEGIN_MESSAGE_MAP(CChoiceMap, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CChoiceMap::OnBnClickedButton1)
	ON_LBN_DBLCLK(IDC_MAP_LIST, &CChoiceMap::OnLbnDblclkMapList)
	ON_BN_CLICKED(IDC_AUTO_OPEN_FILE, &CChoiceMap::OnBnClickedAutoOpenFile)
	ON_BN_CLICKED(IDC_Search, &CChoiceMap::OnBnClickedSearch)
END_MESSAGE_MAP()


// CChoiceMap message handlers

BOOL CChoiceMap::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	COriginalData::SceneCfg_Map::iterator iter= g_pOriginalData->m_mapSceneCfg.begin();
	for (; iter != g_pOriginalData->m_mapSceneCfg.end(); ++iter)
	{
		m_listMapName.AddString(iter->first);
	}
	CString styCurSceneName = g_pMainDlg->GetCurSceneName();
	m_listMapName.SelectString(-1,styCurSceneName);

	if(g_eModeType != eMT_NORMAL)
	{
		GetDlgItem(IDC_AUTO_OPEN_AREA_FILE)->ShowWindow(false);
	}
	if(g_eModeType != eMT_NORMAL && g_eModeType != eMT_AREA_FB)
	{
		GetDlgItem(IDC_AUTO_OPEN_FILE)->ShowWindow(false);
	}
	else
	{
		static_cast<CButton*>(GetDlgItem(IDC_AUTO_OPEN_FILE))->SetCheck(BST_CHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


CString CChoiceMap::GetSceneName()
{
	return m_strSceneName;
}
void CChoiceMap::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	int nIndex = m_listMapName.GetCurSel();
	if (nIndex != LB_ERR)
	{
		m_listMapName.GetText(nIndex,m_strSceneName);
		UINT state  = static_cast<CButton*> (GetDlgItem(IDC_AUTO_OPEN_FILE))->GetState();
		if ((state & 0x0003) == 1)
		{
			m_bIsAutoOpenScene = true;
		}
		state  = static_cast<CButton*> (GetDlgItem(IDC_AUTO_OPEN_AREA_FILE))->GetState();
		if ((state & 0x0003) == 1)
		{
			m_bIsAutoOpenArea = true;
		}
		OnOK();
	}
	else
	{
		OnCancel();
	}
}

void CChoiceMap::OnLbnDblclkMapList()
{
	// TODO: Add your control notification handler code here
	OnBnClickedButton1();
}

void CChoiceMap::OnBnClickedAutoOpenFile()
{
	// TODO: Add your control notification handler code here
}

bool CChoiceMap::IsAutoOpenFile()
{
	return m_bIsAutoOpenScene;
}

bool CChoiceMap::IsAutoOpenAreaFile()
{
	return m_bIsAutoOpenArea;
}
void CChoiceMap::OnBnClickedSearch()
{
	CString strSearchName;
	GetDlgItem(IDC_SEARCH_NAME)->GetWindowText(strSearchName);
	if (m_listMapName.GetCount() <= 0)
	{
		return;
	}
	int iStart = m_listMapName.GetCurSel() + 1;
	if (iStart >= m_listMapName.GetCount())
	{
		iStart = 0;
	}
	int i = iStart;
	CString nameTemp;
	do 
	{
		m_listMapName.GetText(i,nameTemp);
		if (nameTemp.Find(strSearchName) != -1)
		{
			m_listMapName.SetCurSel(i);
			//m_listboxElement.SetFocus();
			return;
		}
		i++;
		if (i>= m_listMapName.GetCount())
		{
			i = 0;
		}
	} while(i != iStart);
}
