// ObjPage.cpp : implementation file
//

#include "stdafx.h"
#include "NewSceneEditor.h"
#include "ObjPage.h"
#include "StateMgr.h"
#include "PlaceMgr.h"

// CObjPage dialog

IMPLEMENT_DYNAMIC(CObjPage, CPropertyPage)

CObjPage::CObjPage()
	: CPropertyPage(CObjPage::IDD)
{

}

CObjPage::~CObjPage()
{
}

void CObjPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrlObj);
}


BEGIN_MESSAGE_MAP(CObjPage, CPropertyPage)
	ON_BN_CLICKED(IDC_Search, &CObjPage::OnBnClickedSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CObjPage::OnLvnItemchangedList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CObjPage::OnClickList1)
END_MESSAGE_MAP()


// CObjPage message handlers

bool CObjPage::Init()
{
	m_listCtrlObj.InsertColumn(0,"Ãû×Ö");
	m_listCtrlObj.SetColumnWidth(0, 140);
	loadData();
	return true;
}


bool CObjPage::loadData()
{
	int index = 0;
	COriginalData::ObjData_Iter it = g_pOriginalData->m_mapObjData.begin();
	while (it != g_pOriginalData->m_mapObjData.end())
	{
		m_listCtrlObj.InsertItem(index, it->first);
		index++;
		it++;
	}
	m_iCount = index;
	return true;
}

PPlaceObj CObjPage::GetSelObj()
{
	m_selectObj.name = m_listCtrlObj.GetItemText(m_listCtrlObj.GetSelectionMark(),0);
	m_selectObj.direction = 0;
	return &m_selectObj;
}


BOOL CObjPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	Init();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CObjPage::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	CStateMgr::Inst().DoEvent(eEVENT_PLACE_ELEMENT);
	*pResult = 0;
}

void CObjPage::OnClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CStateMgr::Inst().DoEvent(eEVENT_PLACE_ELEMENT);
	*pResult = 0;
}



void CObjPage::OnBnClickedSearch()
{
	if (m_iCount <= 0)
	{
		return;
	}
	CString strSearchName;
	GetDlgItem(IDC_SEARCH_NAME)->GetWindowText(strSearchName);
	int iStart = m_listCtrlObj.GetSelectionMark()+1;
	if (iStart >= m_iCount)
	{
		iStart = 0;
	}
	int i = iStart;
	CString nameTemp;
	do 
	{
		nameTemp = m_listCtrlObj.GetItemText(i,0);
		if (nameTemp.Find(strSearchName) != -1)
		{
			m_listCtrlObj.SetItemState(i, LVIS_SELECTED,LVIS_SELECTED);
			m_listCtrlObj.SetSelectionMark(i);
			m_listCtrlObj.EnsureVisible(i, false);
			m_listCtrlObj.SetFocus();
			return;
		}
		i++;
		if (i>= m_iCount)
		{
			i = 0;
		}
	} while(i != iStart);
}
