#include "StdAfx.h"
#include "CMapEditorManage.h"
//Important
#include "CEditContext.h"
#include "CEditDataScene.h"
#include "CMapMgr.h"

using namespace sqr;

CMapEditorManage::CMapEditorManage(void)
{
	m_Map=NULL;
}

CMapEditorManage::~CMapEditorManage(void)
{
}

void CMapEditorManage::InitMapEditorEngine()
{
	m_Map=new CMapMgr();
}

void CMapEditorManage::SetRenderScene(CEditContext* cont)
{
	m_pContext=cont;
	m_pDataScene=m_pContext->GetEditDataScene();
}

CEditContext* CMapEditorManage::GetRenderScene()
{
	return m_pContext;
}

CEditDataScene* CMapEditorManage::GetDataScene( void )
{
	return m_pDataScene;
}

CMapMgr* CMapEditorManage::GetMapMgr()
{
	return m_Map;
}

