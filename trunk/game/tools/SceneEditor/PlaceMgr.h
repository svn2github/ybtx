#pragma once
#include "NewSceneEditor.h"
#include "PlaceDataSheet.h"
#include "NpcPage.h"
#include "ObjPage.h"
#include "TrapPage.h"



//管理摆放npc,obj,trap数据
class CPlaceMgr
{
private:
	CPlaceMgr(void);
public:
	static CPlaceMgr& Inst();
	~CPlaceMgr(void);
	
	bool Init(CWnd* pwnd);
	void NormalShow();
	void NormalShow(int index);
	void Hide();
	void ShowBossPage(int iColonyId);
	void ShowServantPage(int iColonyId);

	BOOL IsShow();

	CElement* GetPlaceElement();
	
	//在动态路径编辑模式下, 摆放新路径时候为了 统一的使用 GetPlaceNpc 获得路径(其实把npc当成路径)
	//通过 SetPathName 会在 m_pageNpc 中创建一个选择的npc;
	void SetPathName(const CString&  pathName); 
private:
	CPlaceDataSheet*	m_pSheet;
	CNpcPage			m_pageNpc;
	CObjPage			m_pageObj;
	CTrapPage			m_pageTrap;
};