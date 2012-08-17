#include "stdafx.h"
#include "PlaceMgr.h"
#include "StateMgr.h"

CPlaceMgr::CPlaceMgr(void):m_pSheet(NULL)
{

}

CPlaceMgr::~CPlaceMgr(void)
{
	if (m_pSheet != NULL)
	{
		delete m_pSheet;
		m_pSheet = NULL;
	}
}

CPlaceMgr& CPlaceMgr::Inst()
{
	static CPlaceMgr instance;
	return instance;
}

bool CPlaceMgr::Init(CWnd* pwnd)
{
	if (m_pSheet != NULL)
	{
		delete m_pSheet;
		m_pSheet = NULL;
	}
	m_pSheet = new CPlaceDataSheet("°Ú¹Ö×ÊÔ´");
	m_pSheet->AddPage(&m_pageNpc);
	m_pSheet->AddPage(&m_pageObj);
	m_pSheet->AddPage(&m_pageTrap);
	m_pSheet->Create(pwnd);
	m_pSheet->ShowWindow(false);
	return true;
}

void CPlaceMgr::Hide()
{
	m_pSheet->ShowWindow(SW_HIDE);
}

void CPlaceMgr::NormalShow()
{
	int index = m_pSheet->GetActiveIndex();
	NormalShow(index);
}

void CPlaceMgr::NormalShow(int index)
{
	m_pSheet->ShowWindow(SW_SHOW);
	if (index == 0)
	{
		m_pageNpc.ChangeType(eNPT_NORMAL, 0);
		if (m_pSheet->GetPageCount() == 1)
		{
			m_pSheet->AddPage(&m_pageObj);
			m_pSheet->AddPage(&m_pageTrap);
		}
	}
	m_pSheet->SetActivePage(index);
}

void CPlaceMgr::ShowBossPage(int iColonyId)
{
	m_pSheet->ShowWindow(SW_SHOW);
	m_pageNpc.ChangeType(eNPT_BOSS, iColonyId);
	if (m_pSheet->GetPageCount() != 1)
	{
		m_pSheet->RemovePage(&m_pageObj);
		m_pSheet->RemovePage(&m_pageTrap);
	}
	m_pSheet->SetActivePage(0);
}

void CPlaceMgr::ShowServantPage(int iColonyId)
{
	m_pSheet->ShowWindow(SW_SHOW);
	m_pageNpc.ChangeType(eNPT_SERVANT, iColonyId);
	if (m_pSheet->GetPageCount() != 1)
	{
		m_pSheet->RemovePage(&m_pageObj);
		m_pSheet->RemovePage(&m_pageTrap);
	}
	m_pSheet->SetActivePage(0);
}

BOOL CPlaceMgr::IsShow()
{
	return m_pSheet->IsWindowVisible();
}

CElement* CPlaceMgr::GetPlaceElement()
{
	if (g_eModeType == eMT_PATH_EDIT)
	{
		return static_cast<CElement*>(m_pageNpc.GetSelectNpc());
	}
	int curPlaceType = m_pSheet->GetActiveIndex();
	switch(curPlaceType)
	{
	case 0:
		return static_cast<CElement*>(m_pageNpc.GetSelectNpc());
	case 1:
		return static_cast<CElement*>(m_pageObj.GetSelObj());
	case 2:
		return static_cast<CElement*>(m_pageTrap.GetSelTrap());
	}
	return NULL;
}

void CPlaceMgr::SetPathName(const CString& pathName)
{
	m_pageNpc.CreateSelPath(pathName);
}