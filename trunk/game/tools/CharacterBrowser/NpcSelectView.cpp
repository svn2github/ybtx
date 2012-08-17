// NpcSelectView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "NpcSelectView.h"
#include "GTRenderMgr.h"
#include "ScriptMgr.h"
#include "CDataCache.h"


// CNpcSelectView

IMPLEMENT_DYNCREATE(CNpcSelectView, CFormView)

CNpcSelectView::CNpcSelectView()
	: CFormView(CNpcSelectView::IDD)
	, m_LastChatSelectItem(NULL)
	, m_LastObjSelectItem(NULL)
	, m_isInitialized(false)
{

}

CNpcSelectView::~CNpcSelectView()
{
}

void CNpcSelectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_CharacterList);
	DDX_Control(pDX, IDC_EDIT1, m_EditFind);
	DDX_Control(pDX, IDC_TREE2, m_IntObjList);
}

BEGIN_MESSAGE_MAP(CNpcSelectView, CFormView)
	ON_BN_CLICKED(IDC_LOADTABLE, &CNpcSelectView::OnBnClickedLoadtable)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CNpcSelectView::OnNMClickTree1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CNpcSelectView::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_RADIO2, &CNpcSelectView::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO1, &CNpcSelectView::OnBnClickedRadio1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE2, &CNpcSelectView::OnTvnSelchangedTree2)
	ON_BN_CLICKED(IDC_FIND_NPC, &CNpcSelectView::OnBnClickedFindNpc)
	ON_COMMAND(ID_CHECK_ANIMATE, &CNpcSelectView::OnCheckAnimate)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CNpcSelectView::OnNMDblclkTree1)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CNpcSelectView::OnBnClickedLoadtable()
{
	// TODO: Add your control notification handler code here

	//现在用双击实现展示模型 所以被注掉了
	// 	if(CGTRenderMgr::Inst()->IsScene())
	// 	{
	// 		string  name = m_characterList.GetItemText(m_characterList.GetSelectedItem());
	// 		CGTRenderMgr::Inst()->CreateCharacter(name);
	// 	}
	// 	else
	// 	{
	// 		MessageBox("请载入场景");
	// 	}
}

void CNpcSelectView::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CNpcSelectView::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM selectTtem = m_CharacterList.GetSelectedItem();
	string  name = m_CharacterList.GetItemText(m_CharacterList.GetSelectedItem());
	name = CDataCache::GetNpc_ObjRealName(name);
	if (m_CharacterList.GetItemData(selectTtem) == 0)
	{		
		gs_pCahce->AddAnimateGroupName(CGTRenderMgr::GetInst()->CreateCharacter(name,eMT_Character),m_CharacterList);
		::SetWindowText(GetSafeHwnd(),   CGTRenderMgr::GetInst()->GetModelFileName().c_str());
		if(m_LastChatSelectItem != NULL)
		{
			DeleteChildrenItem(m_CharacterList, m_LastChatSelectItem);
		}		
		m_LastChatSelectItem = m_CharacterList.GetSelectedItem();
	}
	else
	{
		CGTRenderMgr::GetInst()->DoAnimate(name);
	}
	*pResult = 0;
}

void CNpcSelectView::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CNpcSelectView::DeleteChildrenItem(CTreeCtrl& tree ,HTREEITEM item)
{
	HTREEITEM hChildItem;

	while (tree.ItemHasChildren(item))
	{
		hChildItem = tree.GetChildItem(item);
		tree.DeleteItem(hChildItem);
	}

}

void CNpcSelectView::OnBnClickedRadio2()
{
	ShowIntObjList();
}

void CNpcSelectView::OnBnClickedRadio1()
{
	ShowCharacterList();
}

void CNpcSelectView::OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM selectTtem = m_IntObjList.GetSelectedItem();
	string  name = m_IntObjList.GetItemText(m_IntObjList.GetSelectedItem());
	name = CDataCache::GetNpc_ObjRealName(name);
	if (m_IntObjList.GetItemData(selectTtem) == 0)
	{

		gs_pCahce->AddAnimateGroupName(CGTRenderMgr::GetInst()->CreateCharacter(name, eMT_IntObj),m_IntObjList);
		::SetWindowText(GetSafeHwnd(),   CGTRenderMgr::GetInst()->GetModelFileName().c_str());
		if(m_LastObjSelectItem != NULL)
		{
			DeleteChildrenItem(m_IntObjList,m_LastObjSelectItem);
		}		
		m_LastObjSelectItem = m_IntObjList.GetSelectedItem();
	}
	else
	{
		CGTRenderMgr::GetInst()->DoAnimate(name);
	}


	*pResult = 0;
}

void CNpcSelectView::OnBnClickedFindNpc()
{
	// TODO: Add your control notification handler code here
	const int MAXNUM = 128;
	TCHAR szBuffer[MAXNUM];
	m_EditFind.GetWindowText(szBuffer,MAXNUM);
	string strFindName(szBuffer);

	static HTREEITEM hLastCharacterFindItem = m_CharacterList.GetRootItem();
	static HTREEITEM hLastIntObjFindItem = m_IntObjList.GetRootItem();

	HTREEITEM hLastFindItem;
	CTreeCtrl* pTree;
	UINT state  = static_cast<CButton*>(GetDlgItem(IDC_RADIO1))->GetState();
	if ((state & 0x0003) == 1)
	{
		pTree = &m_CharacterList;
		hLastFindItem = hLastCharacterFindItem;
	}
	else
	{
		pTree = &m_IntObjList;
		hLastFindItem = hLastIntObjFindItem;
	}
	HTREEITEM hCurrent = hLastFindItem;
	do
	{
		TVITEM item;
		TCHAR szText[MAXNUM];
		item.hItem = hCurrent;
		item.mask = TVIF_TEXT | TVIF_HANDLE;
		item.pszText = szText;
		item.cchTextMax = MAXNUM;
		hCurrent = pTree->GetNextItem(hCurrent, TVGN_NEXT);
		if(!hCurrent)
		{
			hCurrent =  pTree->GetRootItem();
		}
		if( pTree->GetItem(&item))
		{
			string strItemText(item.pszText);
			size_t nIndex = strItemText.find(strFindName);
			if (strItemText.find(strFindName)!=string::npos)
			{
				pTree->SelectItem(item.hItem);
				pTree->SetFocus();
				break;
			}
		}
	}while(hCurrent != hLastFindItem);

	if ((state & 0x0003) == 1)
	{
		hLastCharacterFindItem = hCurrent;
	}
	else
	{
		hLastIntObjFindItem = hCurrent;
	}
}

void CNpcSelectView::OnCheckAnimate()
{
	const string NEED_ANIMATES[] = {"stand01","stand02","attack01","walk01","run01","suffer01","die01","dead01"};
	const int ANI_COUNT = sizeof(NEED_ANIMATES)/sizeof(NEED_ANIMATES[0]);
	bool checkeMark[ANI_COUNT];

	string name;
	HTREEITEM hCurrent = m_CharacterList.GetRootItem();
	while (hCurrent != NULL)
	{
		name =  m_CharacterList.GetItemText(hCurrent);
		if (gs_pCahce->getNpcAi(name) == "任务NPC" )
		{
			//cout <<"\""<< name << "\"  任务NPC"<<endl;
			hCurrent = m_CharacterList.GetNextItem(hCurrent, TVGN_NEXT);
			continue;
		}
		else if(gs_pCahce->getNpcAi(name) == "")
		{
			cout <<"\""<< name << "\"  没有在 npc_common 表中 "<< endl;
			hCurrent = m_CharacterList.GetNextItem(hCurrent, TVGN_NEXT);
			continue;
		}
		CRenderObject* pRenderObj = CGTRenderMgr::GetInst()->CreateCharacter(name,eMT_Character);
		CAnimationGroup* pAni = pRenderObj->GetAnimateGroup();

		//重置checkeMark 以便进行新人物动作检测
		for (int i = 0; i < ANI_COUNT; i++)
		{
			checkeMark[i] = false;
		}

		int num = pAni->GetAnimationNum();
		for(int i=0;i<num;i++)
		{
			string aniName = pAni->GetAnimationName(i);
			for (int j = 0; j < ANI_COUNT; j++)
			{
				if(aniName == NEED_ANIMATES[j])
				{
					checkeMark[j] = true;
					break;
				}
			}
		}

		bool bIsOk = true;
		for (int i = 0; i < ANI_COUNT; i++)
		{
			if(!checkeMark[i])
			{
				if(bIsOk)
				{
					cout <<"\""<< name << "\" 缺少必要动作: ";
					bIsOk = false;
				}
				cout << NEED_ANIMATES[i] << " ";
			}
		}
		if(bIsOk)
		{
			//cout <<"\""<< name << "\"  必要动作完整" << endl;
		}
		else
		{
			cout << endl;
		}

		hCurrent = m_CharacterList.GetNextItem(hCurrent, TVGN_NEXT);
	}
}


void CNpcSelectView::ShowCharacterList(bool isShow)
{
	if (isShow)
	{
		m_CharacterList.ShowWindow(SW_SHOW);
		m_IntObjList.ShowWindow(SW_HIDE);
	}
	else
	{
		m_CharacterList.ShowWindow(SW_HIDE);
		m_IntObjList.ShowWindow(SW_SHOW);
	}
}

void CNpcSelectView::ShowIntObjList(bool isShow)
{
	if (isShow)
	{
		m_IntObjList.ShowWindow(SW_SHOW);
		m_CharacterList.ShowWindow(SW_HIDE);
	}
	else
	{
		m_IntObjList.ShowWindow(SW_HIDE);
		m_CharacterList.ShowWindow(SW_SHOW);
	}
}

// CNpcSelectView diagnostics

#ifdef _DEBUG
void CNpcSelectView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CNpcSelectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CNpcSelectView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);
	if( !m_isInitialized )
	{
		static_cast<CButton*>(GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);
		ShowCharacterList();
		gs_pCahce->ShowCharacterName(m_CharacterList);
		gs_pCahce->ShowIntObjName(m_IntObjList);
		m_isInitialized = true;


		RECT rect;
		GetClientRect(&rect);
		int cy = rect.bottom - rect.top;
		m_CharacterList.GetWindowRect(&rect);
		m_CharacterList.SetWindowPos(NULL,0,0,rect.right-rect.left,cy - 70,SWP_NOZORDER|SWP_NOMOVE );
		m_IntObjList.SetWindowPos(NULL,0,0,rect.right-rect.left,cy - 70,SWP_NOZORDER|SWP_NOMOVE );
	}
}


void CNpcSelectView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	if( m_CharacterList.m_hWnd )
	{
		RECT rect;
		//int	 Bottom = cy - 80;
		m_CharacterList.GetWindowRect(&rect);
		m_CharacterList.SetWindowPos(NULL,0,0,rect.right-rect.left,cy - 70,SWP_NOZORDER|SWP_NOMOVE );
		m_IntObjList.SetWindowPos(NULL,0,0,rect.right-rect.left,cy - 70,SWP_NOZORDER|SWP_NOMOVE );
	}
}
