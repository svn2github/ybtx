// ColonyEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ColonyEditDlg.h"
#include "ScriptMgr.h"
#include "PlaceMgr.h"
#include "EnumMgr.h"
#include "PlaceElement.h"
#include "StateMgr.h"

// CColonyEditDlg dialog

IMPLEMENT_DYNAMIC(CColonyEditDlg, CDialog)

static float POS_LIST[][2] = {{2,2}, {-2, -2}, {2, -2}, {-2, 2},{2, 0}, {0, 2}, {-2, 0}, {0, -2},
							 {4, 1}, { -4,-1}, {1, -4}, {-1, 4},{4,-1}, {-4, 1},{-1,-4}, {1, 4}};
static int POS_NUM = (sizeof(POS_LIST)/sizeof(float))/2;


CColonyEditDlg::CColonyEditDlg(int iColonyId, CWnd* pParent /*=NULL*/)
	: CDialog(CColonyEditDlg::IDD, pParent)
	,m_iColonyId(iColonyId)
{

}

CColonyEditDlg::~CColonyEditDlg()
{
	ClearNewNpc();
}

void CColonyEditDlg::ClearNewNpc()
{
	for (uint i=0; i < m_vecNewNpc.size(); ++i)
	{
		delete m_vecNewNpc[i];
	}
	m_vecNewNpc.clear();
}


void CColonyEditDlg::RemoveNewNpc(int nIndex)
{
	m_listboxMember.DeleteString(nIndex);
	Npc_Vec::iterator iter = m_vecNewNpc.begin();
	while (nIndex > 0)
	{
		++iter;
		--nIndex;
	}
	delete (*iter);
	m_vecNewNpc.erase(iter);
}

void CColonyEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MEMBER, m_listboxMember);
}


BEGIN_MESSAGE_MAP(CColonyEditDlg, CDialog)
	ON_BN_CLICKED(IDC_SET_BOSS, &CColonyEditDlg::OnBnClickedSetBoss)
	ON_BN_CLICKED(IDC_ADD_NPC, &CColonyEditDlg::OnBnClickedAddNpc)
	ON_BN_CLICKED(IDC_DELETE, &CColonyEditDlg::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_COPY_AND_PUT, &CColonyEditDlg::OnBnClickedCopyAndPut)
	ON_WM_CLOSE()
	ON_LBN_DBLCLK(IDC_LIST_MEMBER, &CColonyEditDlg::OnLbnDblclkListMember)
END_MESSAGE_MAP()


// CColonyEditDlg message handlers

BOOL CColonyEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CPlaceMgr::Inst().Hide();

	CString titleName;
	titleName.Format("群组编辑 -- ID: %d", m_iColonyId);
	SetWindowText(titleName);
	// TODO:  Add extra initialization here
	if (m_iColonyId != 0)
	{
		CCache::NpcBC_Iter iter = g_pCache->m_bcNpc.begin();
		PPlaceNpc pNpc;
		int index;
		while (iter !=  g_pCache->m_bcNpc.end())
		{
			pNpc = *iter;
			if ( pNpc->colonyId == m_iColonyId)
			{
				if (pNpc->colonyType == CEnumMgr::Inst().GetColonyTypeID("BOSS"))
				{
					index = m_listboxMember.InsertString(0, pNpc->name);
				}
				else
				{
					index = m_listboxMember.InsertString(-1, pNpc->name);
				}
				m_listboxMember.SetItemData(index, (DWORD_PTR)pNpc);
			}
			++iter;
		}
	}

	m_bIsNewColony = m_listboxMember.GetCount() == 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CColonyEditDlg::OnBnClickedSetBoss()
{
	CPlaceMgr::Inst().ShowBossPage(m_iColonyId);
}

void CColonyEditDlg::OnBnClickedAddNpc()
{
	CPlaceMgr::Inst().ShowServantPage(m_iColonyId);
}

void CColonyEditDlg::OnBnClickedDelete()
{
	int nIndex = m_listboxMember.GetCurSel();
	if (nIndex < 0)
	{
		MessageBox("请选择一个要删除的对象");
		return;
	}
	if (m_bIsNewColony)
	{
		RemoveNewNpc(nIndex);
	}
	else
	{
		if (nIndex == 0) //删除虚拟npc
		{
			g_pMapView->SelectColony(m_iColonyId);
			if( g_pMapView->DeleteAllSelectElement())
			{
				m_listboxMember.ResetContent();
				m_bIsNewColony = true;	//删除完了就变成编辑新的群组
			}
		}
		else
		{
			g_pMapView->DeleteElement((CElement*) m_listboxMember.GetItemData(nIndex));
			m_listboxMember.DeleteString(nIndex);
		}
		g_pMainDlg->SetFocus();
	}
}

void CColonyEditDlg::OnBnClickedCopyAndPut()
{
	int count = m_listboxMember.GetCount();
	if (count == 0)
	{
		MessageBox("没有任何npc");
		return;
	}
	if (m_bIsNewColony)
	{
		if (count > POS_NUM)
		{
			CString msg;
			msg.Format("目前不支持超过 %d 个成员的群组", POS_NUM);
			MessageBox(msg);
			return;
		}
		Ast(m_vecNewNpc.size() == count);
		PPlaceNpc pBossNpc = (PPlaceNpc) m_listboxMember.GetItemData(0);
		if(pBossNpc->colonyType != CEnumMgr::Inst().GetColonyTypeID("BOSS"))
		{
			MessageBox("没有虚拟NPC");
			return;
		}
		pBossNpc->fPos.x = 0;
		pBossNpc->fPos.y = 0;

		for (int i = 1; i < count; ++i)
		{
			m_vecNewNpc[i]->fPos.x = POS_LIST[i-1][0];
			m_vecNewNpc[i]->fPos.y = POS_LIST[i-1][1];
		}
	}
	
	CMapView::Element_Vec vecElement;
	for (int i = 0; i < count; ++i)
	{
		vecElement.push_back((CElement*) m_listboxMember.GetItemData(i));
	}
	CStateMgr::Inst().DoEvent(eEVENT_COLONY_EDIT_END);
	if (!g_pMapView->UserCopyElement(&vecElement))
	{
		return;
	}
	g_pMapView->PasteElement();
	CPlaceMgr::Inst().Hide();
	OnOK();
	g_pMainDlg->SetFocus();
}

void CColonyEditDlg::SetDummyNpc(PPlaceNpc pNpc)
{
	if (m_bIsNewColony)
	{
		PPlaceNpc pNewNpc;
		Npc_Vec::iterator iter = m_vecNewNpc.begin();
		if (iter == m_vecNewNpc.end())
		{
			pNewNpc = new PlaceNpc();
			m_vecNewNpc.insert(iter, pNewNpc);
			
			m_listboxMember.InsertString(0, pNpc->name);
			m_listboxMember.SetItemData(0, (DWORD_PTR)pNewNpc);
		}
		else
		{
			pNewNpc = *iter;
			if (pNewNpc->colonyType != CEnumMgr::Inst().GetColonyTypeID("BOSS"))
			{
				pNewNpc = new PlaceNpc();
				m_vecNewNpc.insert(iter, pNewNpc);
				
				m_listboxMember.InsertString(0, pNpc->name);
				m_listboxMember.SetItemData(0, (DWORD_PTR)pNewNpc);
			}
			else
			{
				if (pNewNpc->name != pNpc->name)
				{
					m_listboxMember.DeleteString(0);
					m_listboxMember.InsertString(0, pNpc->name);
					m_listboxMember.SetItemData(0,(DWORD_PTR)pNewNpc);
				}
			}
		}
		CPlaceNpc::CopyeNpc(pNewNpc, pNpc);
	}
	else
	{
		PPlaceNpc pOldNpc = (PPlaceNpc) (m_listboxMember.GetItemData(0));
		Ast(pOldNpc->colonyType == CEnumMgr::Inst().GetColonyTypeID("BOSS"));
		if (pOldNpc->name != pNpc->name)
		{
			m_listboxMember.DeleteString(0);
			int nIndex = m_listboxMember.InsertString(0, pNpc->name);
			m_listboxMember.SetItemData(nIndex,(DWORD_PTR)pOldNpc);
		}
		pNpc->fPos = pOldNpc->fPos;
		CPlaceNpc::CopyeNpc(pOldNpc, pNpc);
		g_pMainDlg->OnUpdateElement(static_cast<CElement*>(pOldNpc), eTPT_NPC);
		g_pMainDlg->OnUpdateElement(static_cast<CElement*>(pOldNpc), eTPT_COLONY);
	}
}

void CColonyEditDlg::AddNpc(PPlaceNpc pNpc)
{
	if (m_bIsNewColony)
	{
		PPlaceNpc pNewNpc = new PlaceNpc();
		CPlaceNpc::CopyeNpc(pNewNpc, pNpc);
		m_vecNewNpc.push_back(pNewNpc);
		int nIndex = m_listboxMember.AddString(pNewNpc->name);
		m_listboxMember.SetItemData(nIndex, (DWORD_PTR)pNewNpc);
	} 
	else
	{
		FPos fPos;
		if (GetAutoPos(fPos))
		{
			CElement* pElement = g_pMapView->PutNpc(pNpc, fPos);
			if (pElement)
			{
				int nIndex = m_listboxMember.AddString(pElement->name);
				m_listboxMember.SetItemData(nIndex, (DWORD_PTR)pElement);
			}
			else
			{
				MessageBox("添加NPC失败");
			}
		}
		else
		{
			MessageBox("虚拟npc附近不够空旷, 未能自动非配坐标,无法添加NPC");
		}
	}
}

void CColonyEditDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	CStateMgr::Inst().DoEvent(eEVENT_COLONY_EDIT_END);
	CPlaceMgr::Inst().Hide();
	CDialog::OnClose();
	g_pMainDlg->SetFocus();
}

void CColonyEditDlg::OnLbnDblclkListMember()
{
	int nIndex = m_listboxMember.GetCurSel();
	CElement* pNpc = (CElement*) m_listboxMember.GetItemData(nIndex);
	g_pMapView->EditElement(pNpc);
}

bool CColonyEditDlg::GetAutoPos(OUT FPos& fPos)
{
	Ast(!m_bIsNewColony);
	PPlaceNpc pNpc = (PPlaceNpc)m_listboxMember.GetItemData(0);
	FPos fPosTemp ;
	for (int i =0; i < POS_NUM; ++i)
	{
		fPosTemp.x = pNpc->fPos.x + POS_LIST[i][0];
		fPosTemp.y = pNpc->fPos.y + POS_LIST[i][1];
		if (g_pMapView->CheckPos(fPosTemp, eET_NPC) && !g_pMapView->IsBlock(fPosTemp))
		{
			fPos = fPosTemp;
			return true;
		}
	}
	return false;
}