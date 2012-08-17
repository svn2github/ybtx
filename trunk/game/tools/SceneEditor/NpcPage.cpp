// NpcPage.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "NewSceneEditor.h"
#include "NpcPage.h"
#include "EnumMgr.h"
#include "StateMgr.h"
#include "PlaceMgr.h"
#include "ColonyEditDlg.h"

// CNpcPage dialog

IMPLEMENT_DYNAMIC(CNpcPage, CPropertyPage)

CNpcPage::CNpcPage()
	: CPropertyPage(CNpcPage::IDD)
	, m_strName(_T(""))
	, m_iDirection(0)
	, m_iColonyId(0)
	, m_strFindNpcName(_T(""))
	, m_iMoveRange(0)
	, m_iMinDelayTime(0)
	, m_iMaxDelayTime(0)
	, m_eCurType(eNPT_NORMAL)
{

}

CNpcPage::~CNpcPage()
{
}

void CNpcPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NORMAL_LIST, m_listCtrlNormalInfo);
	DDX_Control(pDX, IDC_BOSS_LIST, m_listCtrlBossInfo);
	DDX_Control(pDX, IDC_SERVANT_LIST, m_listCtrlServantInfo);
	DDX_Text(pDX, IDC_NPC_NAME, m_strName);
	DDX_Text(pDX, IDC_Direction, m_iDirection);
	//DDX_Text(pDX, IDC_EDIT4, m_iColonyId);
	DDX_Control(pDX, IDC_AISET, m_ComboAISet);
	DDX_Control(pDX, IDC_COMBO3, m_ComboMoveType);
	DDX_Control(pDX, IDC_COMBO1, m_ComboMoveState);
	DDX_Text(pDX, IDC_EDIT3, m_strFindNpcName);
	//DDX_Control(pDX, IDC_COMBO4, m_ComboColony);
	DDX_Control(pDX, IDC_IDLEACT, m_ComboIdleAct);
	DDX_Text(pDX, IDC_MOVE_RANGE, m_iMoveRange);
	DDX_Text(pDX, IDC_minDelayTime, m_iMinDelayTime);
	DDX_Text(pDX, IDC_maxDelayTime, m_iMaxDelayTime);
}


BEGIN_MESSAGE_MAP(CNpcPage, CPropertyPage)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_NORMAL_LIST, &CNpcPage::OnLvnItemchangedList)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_BOSS_LIST, &CNpcPage::OnLvnItemchangedList)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVANT_LIST, &CNpcPage::OnLvnItemchangedList)
ON_NOTIFY(NM_CLICK, IDC_NORMAL_LIST, &CNpcPage::OnClickList)
ON_NOTIFY(NM_CLICK, IDC_BOSS_LIST, &CNpcPage::OnClickList)
ON_NOTIFY(NM_CLICK, IDC_SERVANT_LIST, &CNpcPage::OnClickList)
ON_BN_CLICKED(IDC_DEFAULTAI, &CNpcPage::OnBnClickedDefaultai)
ON_BN_CLICKED(IDC_Search, &CNpcPage::OnBnClickedSearch)
ON_CBN_SELCHANGE(IDC_COMBO3, &CNpcPage::OnCbnSelchangeCombo3)
ON_CBN_SELCHANGE(IDC_AISET, &CNpcPage::OnCbnSelchangeAiset)
ON_NOTIFY(NM_DBLCLK, IDC_NORMAL_LIST, &CNpcPage::OnNMDblclkList)
ON_NOTIFY(NM_DBLCLK, IDC_BOSS_LIST, &CNpcPage::OnNMDblclkList)
ON_NOTIFY(NM_DBLCLK, IDC_SERVANT_LIST, &CNpcPage::OnNMDblclkList)
ON_BN_CLICKED(IDC_INSERT, &CNpcPage::OnBnClickedInsert)
ON_BN_CLICKED(IDC_IGNORE_CLOG, &CNpcPage::OnBnClickedIgnoreClog)
END_MESSAGE_MAP()


// CNpcPage message handlers

bool CNpcPage::Init()
{
	m_listCtrlNormalInfo.InsertColumn(0,"名字");
	m_listCtrlNormalInfo.SetColumnWidth(0, 120);
	m_listCtrlNormalInfo.InsertColumn(1,"类型");
	m_listCtrlNormalInfo.SetColumnWidth(1, 125);
	m_listCtrlNormalInfo.InsertColumn(2,"等级下限");
	m_listCtrlNormalInfo.SetColumnWidth(2, 60);
	m_listCtrlNormalInfo.InsertColumn(3,"等级上限");
	m_listCtrlNormalInfo.SetColumnWidth(3, 60);

	m_listCtrlBossInfo.InsertColumn(0,"名字");
	m_listCtrlBossInfo.SetColumnWidth(0, 120);
	m_listCtrlBossInfo.InsertColumn(1,"类型");
	m_listCtrlBossInfo.SetColumnWidth(1, 125);
	m_listCtrlBossInfo.InsertColumn(2,"等级下限");
	m_listCtrlBossInfo.SetColumnWidth(2, 60);
	m_listCtrlBossInfo.InsertColumn(3,"等级上限");
	m_listCtrlBossInfo.SetColumnWidth(3, 60);

	m_listCtrlServantInfo.InsertColumn(0,"名字");
	m_listCtrlServantInfo.SetColumnWidth(0, 120);
	m_listCtrlServantInfo.InsertColumn(1,"类型");
	m_listCtrlServantInfo.SetColumnWidth(1, 125);
	m_listCtrlServantInfo.InsertColumn(2,"等级下限");
	m_listCtrlServantInfo.SetColumnWidth(2, 60);
	m_listCtrlServantInfo.InsertColumn(3,"等级上限");
	m_listCtrlServantInfo.SetColumnWidth(3, 60);
	loadData();
	return true;
}


bool CNpcPage::loadData()
{
	m_bIsLoading = true;
	int index = 0;
	COriginalData::NpcData_Iter it = g_pOriginalData->m_mapNpcData.begin();
	CString  minLevel,maxLevel, AIType;
	CListCtrl* pListCtrl;
	for (; it != g_pOriginalData->m_mapNpcData.end(); ++it)
	{
		AIType = it->second->_AI类型;
		minLevel.Format("%d", it->second->_等级下限);
		maxLevel.Format("%d", it->second->_等级上限);
		if (AIType == "虚拟Npc支持巡逻")
		{
			pListCtrl = GetListCtrl(eNPT_BOSS);
		}
		else if (AIType.Find("群组成员") >= 0)
		{
			pListCtrl = GetListCtrl(eNPT_SERVANT);
		}
		else
		{
			pListCtrl = GetListCtrl(eNPT_NORMAL);
		}
		index = pListCtrl->GetItemCount();
		pListCtrl->InsertItem(index, it->second->_名字);
		pListCtrl->SetItemText(index,1,AIType);
		pListCtrl->SetItemText(index,2,minLevel);
		pListCtrl->SetItemText(index,3,minLevel);
		pListCtrl->SetItemData(index,(DWORD)it->second);
	}
	m_bIsLoading = false;
	return true;
}

CListCtrl* CNpcPage::GetListCtrl(ENpcPageType type)
{
	switch(type)
	{
	case eNPT_NORMAL:
		return &m_listCtrlNormalInfo;
	case eNPT_BOSS:
		return &m_listCtrlBossInfo;
	case eNPT_SERVANT:
		return &m_listCtrlServantInfo;
	}
	return NULL;
}

void CNpcPage::ChangeType(ENpcPageType type, int iColonyId)
{
	m_eCurType = type;
	m_iColonyId = iColonyId;
	m_pCurListCtrl = GetListCtrl(type);
	UpdateSelNpc(m_pCurListCtrl->GetSelectionMark());
	for (int i= 0; i < eNPT_COUNT; ++i)
	{
		if (i == type)
		{
			GetListCtrl((ENpcPageType) i)->EnableWindow(true);
			GetListCtrl((ENpcPageType) i)->ShowWindow(true);
			
		}
		else
		{
			GetListCtrl((ENpcPageType)i)->EnableWindow(false);
			GetListCtrl((ENpcPageType) i)->ShowWindow(false);
		}
	}
	switch(type)
	{
	case eNPT_NORMAL:
		GetDlgItem(IDC_INSERT)->SetWindowText("摆放");
		break;
	case eNPT_BOSS:
		GetDlgItem(IDC_INSERT)->SetWindowText("设置虚拟NPC");
		break;
	case eNPT_SERVANT:
		GetDlgItem(IDC_INSERT)->SetWindowText("添加群组NPC");
		break;
	}
}

BOOL CNpcPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here

	Init();
	m_ComboMoveState.SetCurSel(0);
	m_ComboIdleAct.SetCurSel(0);
	//m_ComboColony.SetCurSel(1);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CNpcPage::UpdateSelNpc(int iIndex)
{
	if (iIndex < 0 || iIndex > m_pCurListCtrl->GetItemCount())
	{
		m_strName = "";
		UpdateData(false);
	}
	else
	{
		PNPC pNpc= (PNPC) m_pCurListCtrl->GetItemData(iIndex);
		Ast(pNpc);
		m_strName = pNpc->_名字;
		UpdateAIComboBox(pNpc->_AI类型);
		SetAi(pNpc->_AI类型);
		UpdateData(false);
	}
}

 PPlaceNpc CNpcPage::GetSelectNpc()
{
	if (g_eModeType == eMT_PATH_EDIT)
	{
		return &m_SelNpc;
	}
	UpdateData(true);
	m_SelNpc.name =	m_strName;
	//m_SelNpc.fPos.x = 0; //由鼠标确定
	//m_SelNpc.fPos.y = 0;
	CString sTemp;

	m_ComboMoveType.GetWindowText(sTemp);
	m_SelNpc.moveType = CEnumMgr::Inst().GetMoveTypeID(sTemp);

	m_SelNpc.moveRange = m_iMoveRange;
	m_SelNpc.direction = m_iDirection;
	m_SelNpc.minDelayTime = m_iMinDelayTime * 1000;
	m_SelNpc.maxDelayTime = m_iMaxDelayTime * 1000;

	m_ComboMoveState.GetWindowText(sTemp);
	m_SelNpc.speed = CEnumMgr::Inst().GetMoveStateID(sTemp);

	m_ComboIdleAct.GetWindowText(sTemp);
	m_SelNpc.idleType = CEnumMgr::Inst().GetIdleTypeID(sTemp);

	m_ComboAISet.GetWindowText(sTemp);
	m_SelNpc.AIType = sTemp;

	m_SelNpc.colonyId = m_iColonyId;

	//m_ComboColony.GetWindowText(sTemp);
	if (m_eCurType == eNPT_NORMAL)
	{
		m_SelNpc.colonyType = CEnumMgr::Inst().GetColonyTypeID("无关人士");
	}
	else if (m_eCurType == eNPT_BOSS)
	{
		m_SelNpc.colonyType = CEnumMgr::Inst().GetColonyTypeID("BOSS");
	} 
	else
	{
		m_SelNpc.colonyType = CEnumMgr::Inst().GetColonyTypeID("小弟");
	}

	m_SelNpc.type = "普通";
	if (m_SelNpc.AIType.Find("群组成员") >= 0)
	{
		m_SelNpc.type = "团队成员";
	}
	else if (m_SelNpc.AIType == "虚拟Npc支持巡逻")
	{
		m_SelNpc.type = "虚拟Npc";
	}
	CString errorMessage;
	if(CPlaceNpc::CheckNpc(m_SelNpc, errorMessage, false))
	{
		return &m_SelNpc;
	}
	else
	{
		g_pMainDlg->MessageBox(errorMessage);
		return NULL;
	}
}

 

void CNpcPage::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (!m_bIsLoading)
	{
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		UpdateSelNpc(pNMLV->iItem);
		CStateMgr::Inst().DoEvent(eEVENT_PLACE_ELEMENT);
	}
	*pResult = 0;
}

void CNpcPage::OnClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CStateMgr::Inst().DoEvent(eEVENT_PLACE_ELEMENT);
	*pResult = 0;
}


void CNpcPage::OnBnClickedDefaultai()
{
	PNPC pNpc= (PNPC) m_pCurListCtrl->GetItemData(m_pCurListCtrl->GetSelectionMark());
	Ast(pNpc);
	int index = m_ComboAISet.FindString(-1, pNpc->_AI类型);
	m_ComboAISet.SetCurSel(index);
}


void CNpcPage::UpdateAIComboBox(CString sAIType)
{
	m_ComboAISet.Clear();
	m_ComboAISet.AddString(sAIType);
	m_ComboAISet.SetCurSel(0);
}

void CNpcPage::UpdateMoveComboBox(CString sAIType)
{
	for (int i = m_ComboMoveType.GetCount()-1; i >= 0; i--)
	{
		m_ComboMoveType.DeleteString( i );
	}
	if (sAIType.Find("巡逻") != -1)
	{
		m_ComboMoveType.AddString("往返巡逻");
		m_ComboMoveType.AddString("循环巡逻");
		m_ComboMoveType.AddString("固定");
		if(sAIType != "虚拟Npc支持巡逻")
		{
			m_ComboMoveType.AddString("单线巡逻");
			m_ComboMoveType.AddString("先单线后随机");
			m_ComboMoveType.AddString("组合路径巡逻");
			
			m_ComboMoveType.AddString("随机");
		}
	}
	else
	{
		m_ComboMoveType.AddString("固定");
		m_ComboMoveType.AddString("随机");
	}
	m_ComboMoveType.SetCurSel(0);
	OnCbnSelchangeCombo3();
}

void CNpcPage::SetAi(CString sAIType)
{
	int nIndex = m_ComboAISet.FindString(-1,sAIType);
	if (nIndex != -1)
	{
		m_ComboAISet.SetCurSel(nIndex);
	}
	UpdateMoveComboBox(sAIType);
}


void CNpcPage::OnBnClickedSearch()
{
	if (m_pCurListCtrl->GetItemCount() <= 0)
	{
		return;
	}
	UpdateData(true);
	int iStart =m_pCurListCtrl->GetSelectionMark() + 1;
	int iCount = m_pCurListCtrl->GetItemCount();
	if (iStart >= iCount)
	{
		iStart = 0;
	}
	int i = iStart;
	CString nameTemp;
	do 
	{
		nameTemp = m_pCurListCtrl->GetItemText(i,0);
		if (nameTemp.Find(m_strFindNpcName) != -1)
		{
			m_pCurListCtrl->SetItemState(i, LVIS_SELECTED,LVIS_SELECTED);
			m_pCurListCtrl->SetSelectionMark(i);
			m_pCurListCtrl->EnsureVisible(i, false);
			m_pCurListCtrl->SetFocus();
			return;
		}
		i++;
		if (i>= iCount)
		{
			i = 0;
		}
	} while(i != iStart);
}



void CNpcPage::OnCbnSelchangeCombo3()
{
	CString s;
	m_ComboMoveType.GetWindowText(s);
	if (s == "随机")
	{
		GetDlgItem(IDC_MOVE_RANGE)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_MOVE_RANGE)->EnableWindow(false);
	}
}

void CNpcPage::OnCbnSelchangeAiset()
{
	CString s;
	m_ComboAISet.GetWindowText(s);
	UpdateMoveComboBox(s);
}

void CNpcPage::CreateSelPath(const CString& pathName)
{
	m_SelNpc.name = pathName;
	m_SelNpc.moveType = CEnumMgr::Inst().GetMoveTypeID("往返巡逻");
	m_SelNpc.speed = CEnumMgr::Inst().GetMoveStateID("跑");
	m_SelNpc.direction = 0;
	m_SelNpc.minDelayTime = 0;
	m_SelNpc.maxDelayTime = 0;
	m_SelNpc.type = "普通";
	m_SelNpc.AIType = "支持巡逻";
}
void CNpcPage::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CNpcPage::OnBnClickedInsert()
{
	
	if (m_eCurType == eNPT_NORMAL)
	{
	} 
	else 
	{
		PPlaceNpc pNpc = GetSelectNpc();
		if (pNpc)
		{
			CColonyEditDlg* pDlg = g_pMainDlg->GetCurColonyEditDlg();
			if (m_eCurType == eNPT_BOSS)
			{
				pDlg->SetDummyNpc(pNpc);
			} 
			else
			{
				pDlg->AddNpc(pNpc);
			}
		}
	} 
}



void CNpcPage::OnBnClickedIgnoreClog()
{
	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_IGNORE_CLOG))->GetState();
	if ((state & 0x0003) == 1)
	{
		m_SelNpc.isIgnoreClog = 1;
	}
	else
	{
		m_SelNpc.isIgnoreClog = 0;
	}
}
