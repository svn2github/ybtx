#include "stdafx.h"
#include "CCallBackCmd.h"
#include "CmdMgr.h"

DEF_UI_IMP(CCallBackCmd);
CCallBackCmd::CCallBackCmd(void* pObject)
{
	m_pExeCallBack = NULL;
	m_pUnExeCallBack = NULL;
	m_pObject = pObject;
}

CCallBackCmd::~CCallBackCmd(void)
{

}

void CCallBackCmd::SetCallBack(CmdCallBack* pExeFun,CmdCallBack* pUnExeFun,void* pObject)
{
	m_pExeCallBack = pExeFun;
	m_pUnExeCallBack = pUnExeFun;
	if(pObject!=NULL)
		m_pObject = pObject;
}

void CCallBackCmd::_Execute(void)
{
	if(m_pExeCallBack)
		m_pExeCallBack(m_pObject);
}

void CCallBackCmd::_UnExecute(void)
{
	if(m_pUnExeCallBack)
		m_pUnExeCallBack(m_pObject);
}

void CCallBackCmd::Execute(void)
{
	CCmd::Execute();
}

void CCallBackCmd::MgrUnDo(void)
{
	CCmdMgr::GetInst()->UnDo();
}

void CCallBackCmd::MgrReDo(void)
{
	CCmdMgr::GetInst()->ReDo();
}