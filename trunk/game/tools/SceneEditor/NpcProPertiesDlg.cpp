// NpcProPertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EnumMgr.h"
#include "NewSceneEditor.h"
#include "NpcProPertiesDlg.h"


// CNpcProPertiesDlg dialog

IMPLEMENT_DYNAMIC(CNpcProPertiesDlg, CDialog)

CNpcProPertiesDlg::CNpcProPertiesDlg(PPlaceNpc pNpc,const CPoint& dlgPos,CWnd* pParent /*=NULL*/)
	: CDialog(CNpcProPertiesDlg::IDD, pParent), m_pNpc(pNpc),m_dlgPos(dlgPos)
{

}

CNpcProPertiesDlg::~CNpcProPertiesDlg()
{
}

void CNpcProPertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AISET, m_ComboAISet);
	DDX_Control(pDX, IDC_COMBO3, m_ComboMoveType);
	DDX_Control(pDX, IDC_COMBO1, m_ComboMoveState);
	DDX_Control(pDX, IDC_COMBO4, m_ComboColonyType);
	DDX_Control(pDX, IDC_IDLEACT, m_ComboIdleAct);
}


BEGIN_MESSAGE_MAP(CNpcProPertiesDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CNpcProPertiesDlg::OnCbnSelchangeCombo3)
	ON_BN_CLICKED(IDC_BUTTON2, &CNpcProPertiesDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CNpcProPertiesDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_IGNORE_CLOG, &CNpcProPertiesDlg::OnBnClickedIgnoreClog)
END_MESSAGE_MAP()


// CNpcProPertiesDlg message handlers

BOOL CNpcProPertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText("NPC 属性");
	CRect rc;
	GetWindowRect(&rc);
	MoveWindow(m_dlgPos.x, m_dlgPos.y - (rc.bottom - rc.top)/2, rc.right - rc.left, rc.bottom - rc.top);

	GetDlgItem(IDC_NPC_NAME)->SetWindowText(m_pNpc->name);
	UpdateAIComboBox(m_pNpc->AIType);
	SetAi(m_pNpc->AIType);
	CString sTemp;
	sTemp.Format("%d", m_pNpc->moveRange);
	GetDlgItem(IDC_MOVE_RANGE)->SetWindowText(sTemp);
	sTemp.Format("%d", m_pNpc->direction);
	GetDlgItem(IDC_Direction)->SetWindowText(sTemp);
	sTemp.Format("%d", m_pNpc->minDelayTime/1000);
	GetDlgItem(IDC_minDelayTime)->SetWindowText(sTemp);
	sTemp.Format("%d", m_pNpc->maxDelayTime/1000);
	GetDlgItem(IDC_maxDelayTime)->SetWindowText(sTemp);
	int index = m_ComboMoveState.FindString(-1,CEnumMgr::Inst().GetMoveStateStr( m_pNpc->speed));
	if (index == -1) 
	{
		m_ComboMoveState.SetCurSel(0);
	}
	else
	{
		m_ComboMoveState.SetCurSel(index);
	}
	index = m_ComboMoveType.FindString(-1,CEnumMgr::Inst().GetMoveTypeStr( m_pNpc->moveType));
	m_ComboMoveType.SetCurSel(index);
	sTemp.Format("%d", m_pNpc->colonyId);
	GetDlgItem(IDC_EDIT4)->SetWindowText(sTemp);
	
	index = m_ComboColonyType.FindString(-1,CEnumMgr::Inst().GetColonyTypeStr( m_pNpc->colonyType));
	m_ComboColonyType.SetCurSel(index);
	index = m_ComboIdleAct.FindString(-1,CEnumMgr::Inst().GetIdleTypeStr( m_pNpc->idleType));
	m_ComboIdleAct.SetCurSel(index);

	static_cast<CButton*>(GetDlgItem(IDC_IGNORE_CLOG))->SetCheck(m_pNpc->isIgnoreClog ? BST_CHECKED : BST_UNCHECKED);
	//动态路径编辑模式, 关闭一些属性修改
	if(g_eModeType == eMT_PATH_EDIT)
	{
		GetDlgItem(IDC_Direction)->EnableWindow(false);
		GetDlgItem(IDC_EDIT4)->EnableWindow(false);//团队id
		m_ComboIdleAct.EnableWindow(false);
		m_ComboColonyType.EnableWindow(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CNpcProPertiesDlg::UpdateAIComboBox(CString sAIType)
{
	m_ComboAISet.Clear();
	m_ComboAISet.AddString(sAIType);
	m_ComboAISet.SetCurSel(0);
}

void CNpcProPertiesDlg::SetAi(CString sAIType)
{
	int nIndex = m_ComboAISet.FindString(-1,sAIType);
	if (nIndex != -1)
	{
		m_ComboAISet.SetCurSel(nIndex);
	}
	UpdateMoveComboBox(sAIType);
}

void CNpcProPertiesDlg::UpdateMoveComboBox(CString sAIType)
{
	for (int i = m_ComboMoveType.GetCount()-1; i >= 0; i--)
	{
		m_ComboMoveType.DeleteString( i );
	}
	if (sAIType.Find("巡逻") != -1)
	{
		m_ComboMoveType.AddString("往返巡逻");
		m_ComboMoveType.AddString("循环巡逻");
		if(g_eModeType != eMT_PATH_EDIT) //路径编辑模式下 只能为巡逻模式
		{
			m_ComboMoveType.AddString("固定");
		}
		if(sAIType != "虚拟Npc支持巡逻")
		{
			m_ComboMoveType.AddString("单线巡逻");
			m_ComboMoveType.AddString("先单线后随机");
			m_ComboMoveType.AddString("组合路径巡逻");
			if(g_eModeType != eMT_PATH_EDIT) //路径编辑模式下 只能为巡逻模式
			{
				m_ComboMoveType.AddString("随机");
			}
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
void CNpcProPertiesDlg::OnCbnSelchangeCombo3()
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

void CNpcProPertiesDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	PlaceNpc npc;
	npc.fPos = m_pNpc->fPos;
	npc.isIgnoreClog = m_pNpc->isIgnoreClog;
	CString sTemp;
	GetDlgItem(IDC_NPC_NAME)->GetWindowText(sTemp);
	npc.name = sTemp;

	m_ComboMoveType.GetWindowText(sTemp);
	npc.moveType =CEnumMgr::Inst().GetMoveTypeID(sTemp);

	GetDlgItem(IDC_MOVE_RANGE)->GetWindowText(sTemp);
	npc.moveRange = StrToInt(sTemp.GetBuffer());

	GetDlgItem(IDC_Direction)->GetWindowText(sTemp);
	npc.direction = StrToInt(sTemp.GetBuffer());

	GetDlgItem(IDC_minDelayTime)->GetWindowText(sTemp);
	npc.minDelayTime = StrToInt(sTemp.GetBuffer()) * 1000;
	
	GetDlgItem(IDC_maxDelayTime)->GetWindowText(sTemp);
	npc.maxDelayTime = StrToInt(sTemp.GetBuffer()) * 1000;

	GetDlgItem(IDC_COMBO1)->GetWindowText(sTemp);
	npc.speed = CEnumMgr::Inst().GetMoveStateID(sTemp);

	GetDlgItem(IDC_EDIT4)->GetWindowText(sTemp);
	npc.colonyId = StrToInt(sTemp.GetBuffer());

	GetDlgItem(IDC_COMBO4)->GetWindowText(sTemp);
	npc.colonyType =  CEnumMgr::Inst().GetColonyTypeID(sTemp);

	GetDlgItem(IDC_IDLEACT)->GetWindowText(sTemp);
	npc.idleType = CEnumMgr::Inst().GetIdleTypeID(sTemp);

	UINT state  = static_cast<CButton*> (GetDlgItem(IDC_IGNORE_CLOG))->GetState();
	if ((state & 0x0003) == 1)
	{
		m_pNpc->isIgnoreClog = 1;
	}
	else
	{
		m_pNpc->isIgnoreClog = 0;
	}

	m_ComboAISet.GetWindowText(sTemp);
	npc.AIType = sTemp;

	npc.type = "普通";
	if (npc.AIType.Find("群组成员") >= 0)
	{
		npc.type = "团队成员";
	}
	else if (npc.AIType == "虚拟Npc支持巡逻")
	{
		npc.type = "虚拟Npc";
	}
	
	if (CPlaceNpc::CheckNpc(npc, sTemp, false))
	{
		if (npc.moveType != m_pNpc->moveType 
			&& (npc.moveType == CEnumMgr::Inst().GetMoveTypeID("固定") 
				|| npc.moveType == CEnumMgr::Inst().GetMoveTypeID("随机"))
			&& m_pNpc->pathList.size() != 0)
		{
			CString s = "移动类型改变为 ";
			s += CEnumMgr::Inst().GetMoveTypeStr(npc.moveType);
			if(MessageBox(s + " 将丢失巡逻路线信息,是否确定修改","",MB_OKCANCEL) == IDCANCEL)
			{
				return;
			}
			CPlaceNpc::CopyeNpc(m_pNpc, &npc);
			m_pNpc->DeletePath();
		}
		else
		{
			if(MessageBox("是否确定修改 NPC 属性","",MB_OKCANCEL) == IDCANCEL)
			{
				return;
			}
			CPlaceNpc::CopyeNpc(m_pNpc, &npc);
		}
		OnOK();
	}
	else
	{
		MessageBox(sTemp);
		return;
	}
}

void CNpcProPertiesDlg::OnBnClickedButton3()
{
	OnCancel();
}


void CNpcProPertiesDlg::OnBnClickedIgnoreClog()
{

}
