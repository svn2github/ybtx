// TrapPage.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "NewSceneEditor.h"
#include "TrapPage.h"
#include "StateMgr.h"
#include "PlaceMgr.h"

// CTrapPage dialog

IMPLEMENT_DYNAMIC(CTrapPage, CPropertyPage)

CTrapPage::CTrapPage()
	: CPropertyPage(CTrapPage::IDD)
{

}

CTrapPage::~CTrapPage()
{
}

void CTrapPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrlTrap);
}


BEGIN_MESSAGE_MAP(CTrapPage, CPropertyPage)
	ON_BN_CLICKED(IDC_Search, &CTrapPage::OnBnClickedSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CTrapPage::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CTrapPage::OnClickList1)
END_MESSAGE_MAP()


// CTrapPage message handlers

bool CTrapPage::Init()
{
	m_listCtrlTrap.InsertColumn(0,"Ãû×Ö");
	m_listCtrlTrap.SetColumnWidth(0, 140);
	loadData();
	return true;
}


bool CTrapPage::loadData()
{
	int index = 0;
	COriginalData::TrapData_Iter it = g_pOriginalData->m_mapTrapData.begin();
	while (it != g_pOriginalData->m_mapTrapData.end())
	{
		m_listCtrlTrap.InsertItem(index, it->first);
		index++;
		it++;
	}
	m_iCount = index;
	return true;
}

PPlaceTrap CTrapPage::GetSelTrap()
{
	m_selectTrap.name = m_listCtrlTrap.GetItemText(m_listCtrlTrap.GetSelectionMark(),0);
	m_selectTrap.direction = 0;
	return &m_selectTrap;
}

BOOL CTrapPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	Init();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTrapPage::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CStateMgr::Inst().DoEvent(eEVENT_PLACE_ELEMENT);
	*pResult = 0;
}

void CTrapPage::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CStateMgr::Inst().DoEvent(eEVENT_PLACE_ELEMENT);
	*pResult = 0;
}


void CTrapPage::OnBnClickedSearch()
{
	if (m_iCount <= 0)
	{
		return;
	}
	CString strSearchName;
	GetDlgItem(IDC_SEARCH_NAME)->GetWindowText(strSearchName);
	int iStart = m_listCtrlTrap.GetSelectionMark()+1;
	if (iStart >= m_iCount)
	{
		iStart = 0;
	}
	int i = iStart;
	CString nameTemp;
	do 
	{
		nameTemp = m_listCtrlTrap.GetItemText(i,0);
		if (nameTemp.Find(strSearchName) != -1)
		{
			m_listCtrlTrap.SetItemState(i, LVIS_SELECTED,LVIS_SELECTED);
			m_listCtrlTrap.SetSelectionMark(i);
			m_listCtrlTrap.EnsureVisible(i, false);
			m_listCtrlTrap.SetFocus();
			return;
		}
		i++;
		if (i>= m_iCount)
		{
			i = 0;
		}
	} while(i != iStart);
}