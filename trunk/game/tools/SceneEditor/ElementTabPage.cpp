// ElementTabPage.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "NewSceneEditor.h"
#include "ElementTabPage.h"
#include "PlaceMgr.h"
#include "StateMgr.h"
#include "ColonyEditDlg.h"


// CElementTabPage dialog

IMPLEMENT_DYNAMIC(CElementTabPage, CDialog)

CElementTabPage::CElementTabPage(CWnd* pParent /*=NULL*/)
	: CDialog(CElementTabPage::IDD, pParent)
{

}

CElementTabPage::~CElementTabPage()
{
}

void CElementTabPage::SetType(ETabPageType eType)
{
	m_eType = eType;
}

void CElementTabPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ELEMENT_LIST, m_listboxElement);
}


BEGIN_MESSAGE_MAP(CElementTabPage, CDialog)
	ON_BN_CLICKED(IDC_FIND, &CElementTabPage::OnBnClickedFind)
	ON_LBN_DBLCLK(IDC_ELEMENT_LIST, &CElementTabPage::OnLbnDblclkElementList)
	ON_BN_CLICKED(IDC_NEW_ELEMENT, &CElementTabPage::OnBnClickedNewElement)
	ON_BN_CLICKED(IDC_EDIT_ELEMENT, &CElementTabPage::OnBnClickedEditElement)
	ON_BN_CLICKED(IDC_DEL_ELEMENT, &CElementTabPage::OnBnClickedDelElement)
END_MESSAGE_MAP()


// CElementTabPage message handlers

void CElementTabPage::OnBnClickedFind()
{
	CString strSearchName;
	GetDlgItem(IDC_FINDSTRING)->GetWindowText(strSearchName);
	if (m_listboxElement.GetCount() <= 0)
	{
		return;
	}
	int iStart = m_listboxElement.GetCurSel() + 1;
	if (iStart >= m_listboxElement.GetCount())
	{
		iStart = 0;
	}
	int i = iStart;
	CString nameTemp;
	do 
	{
		m_listboxElement.GetText(i,nameTemp);
		if (nameTemp.Find(strSearchName) != -1)
		{
			m_listboxElement.SetCurSel(i);
			//m_listboxElement.SetFocus();
			return;
		}
		i++;
		if (i>= m_listboxElement.GetCount())
		{
			i = 0;
		}
	} while(i != iStart);
}
void CElementTabPage::SetCount(int count)
{
	CString strCount;
	strCount.Format("%d", count);
	GetDlgItem(IDC_ELEMENT_COUNT)->SetWindowText(strCount);
}


void CElementTabPage::AddElement(CElement* pElement)
{
	CString strShowName;
	if (m_eType == eTPT_COLONY)
	{
		PPlaceNpc pNpc= static_cast<PPlaceNpc> (pElement);
		strShowName.Format("[群组: %d] %s", pNpc->colonyId, pNpc->name);
	}
	else
	{
		strShowName = pElement->name;
	}
	int index = m_listboxElement.AddString(strShowName);
	if (index >= 0)
	{
		m_listboxElement.SetItemData(index, (DWORD_PTR)pElement);
		SetCount(m_listboxElement.GetCount());
	}
}

void CElementTabPage::DelElement(CElement* pElement)
{
	DWORD_PTR itemData = (DWORD_PTR) pElement;
	int count = m_listboxElement.GetCount();
	for (int i = 0; i < count ; ++i )
	{
		if(m_listboxElement.GetItemData(i) == itemData)
		{
			m_listboxElement.DeleteString(i);
			return;
		}
	}
}

void CElementTabPage::UpdateElement(CElement* pElement)
{
	DelElement(pElement);
	AddElement(pElement);
}

void CElementTabPage::ClearElement()
{
	if (m_listboxElement.m_hWnd == NULL) //已经关闭
	{
		return;
	}
	m_listboxElement.ResetContent();
}
void CElementTabPage::OnLbnDblclkElementList()
{
	int nIndex = m_listboxElement.GetCurSel();
	if (nIndex != LB_ERR)
	{
		if (m_eType != eTPT_COLONY)
		{
			CElement* pElement = (CElement*) m_listboxElement.GetItemData(nIndex);
			g_pMapView->MoveMapToCenter(pElement->fPos);
			g_pMapView->ClearSelectElement();
			g_pMapView->AddSelectElement(pElement);
			g_pMainDlg->SetFocus();
		}
		else
		{
			PPlaceNpc pNpc =PPlaceNpc (m_listboxElement.GetItemData(nIndex));
			g_pMapView->SelectColony(pNpc->colonyId);
			g_pMapView->MoveMapToSelectCenter();
		}

	}
}

void CElementTabPage::OnCancel()
{
	//CDialog::OnCancel();   --不执行关闭
}

void CElementTabPage::OnBnClickedNewElement()
{
	if (g_eModeType == eMT_PATH_EDIT)
	{
		return;
	}
	if(g_eModeType == eMT_DYNAMIC && g_pCache->GetCurEditDynamicName() == "")
	{
		MessageBox("请先在加载文件 中选择或新建一个 要编辑的动态文件");
		return;
	}
	if (m_eType == eTPT_COLONY)
	{
		CColonyEditDlg dlg(g_pCache->GetNextColonyId());
		g_pMapView->PauseRender();
		g_pMainDlg->SetCurColonyEditDlg(&dlg);
		CStateMgr::Inst().DoEvent(eEVENT_BACK_NORMAL);
		CStateMgr::Inst().DoEvent(eEVENT_COLONY_EDIT_BEGIN);
		dlg.DoModal();
		g_pMainDlg->SetCurColonyEditDlg(NULL);
		g_pMainDlg->SetFocus();
	}
	else
	{
		CPlaceMgr::Inst().NormalShow( m_eType);
	}
}

void CElementTabPage::OnBnClickedEditElement()
{
	if (g_eModeType == eMT_PATH_EDIT)
	{
		return;
	}
	int nIndex = m_listboxElement.GetCurSel();
	if (nIndex == LB_ERR)
	{
		MessageBox("请选择一个编辑对象");
		return;
	}
	if (m_eType == eTPT_COLONY)
	{
		PPlaceNpc pNpc = PPlaceNpc (m_listboxElement.GetItemData(nIndex));
		g_pMapView->SelectColony(pNpc->colonyId);
		g_pMapView->MoveMapToSelectCenter();
		CColonyEditDlg dlg(pNpc->colonyId);
		g_pMapView->PauseRender();
		g_pMainDlg->SetCurColonyEditDlg(&dlg);
		CStateMgr::Inst().DoEvent(eEVENT_BACK_NORMAL);
		CStateMgr::Inst().DoEvent(eEVENT_COLONY_EDIT_BEGIN);
		dlg.DoModal();
		g_pMainDlg->SetCurColonyEditDlg(NULL);
		g_pMainDlg->SetFocus();
	} 
	else
	{

	}
}

void CElementTabPage::OnBnClickedDelElement()
{
	if (g_eModeType == eMT_PATH_EDIT)
	{
		return;
	}
	int nIndex = m_listboxElement.GetCurSel();
	if (nIndex == LB_ERR)
	{
		MessageBox("请选择一个要删除的对象");
		return;
	}
	OnLbnDblclkElementList();
	CString selName;
	m_listboxElement.GetText(nIndex,selName);
	if(IDYES != MessageBox(  "确定要删除 " + selName + " 吗?", "提示", MB_YESNO))
	{
		return;
	}
	g_pMapView->DeleteAllSelectElement();
}
