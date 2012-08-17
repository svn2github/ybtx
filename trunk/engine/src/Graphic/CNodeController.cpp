#include "stdafx.h"
#include "CNodeController.h"
#include "CCtrlNode.h"

const uint32 CNodeController::INVALID_INDEX = 0xffffffff;

CNodeController::CNodeController()
: m_uCurTime(0)
, m_isInCtrl(false)
, m_pCtrlNode(NULL)
, m_uStartTime(0)
{

}

CNodeController::~CNodeController()
{
	if(m_pCtrlNode)
		m_pCtrlNode->m_pController = NULL;
}

void CNodeController::Bind(CCtrlNode* pNode)
{
	Ast(NULL!=pNode);
	if(pNode->m_pController)
		pNode->m_pController->UnBind();

	if(m_pCtrlNode)
	{
		m_pCtrlNode->m_pController = NULL;
		m_pCtrlNode = NULL;
	}

	if(_IsValid(pNode))
	{
		pNode->m_pController = this;
		m_pCtrlNode = pNode;
		_InitBind();
	}
}

void CNodeController::UnBind(void)
{
	if(m_pCtrlNode)
	{
		m_pCtrlNode->m_pController = NULL;
		m_pCtrlNode = NULL;
	}
}

void CNodeController::BeginCtrl(uint32 uCurTime)
{
	m_isInCtrl = true;
	m_uStartTime = uCurTime;
	m_uCurTime = uCurTime;
}

void CNodeController::EndCtrl(void)
{
	m_isInCtrl = false;
}

CtrlState CNodeController::DoCtrl( uint32 uCurTime )
{
	if(NULL==m_pCtrlNode || !m_isInCtrl)
		return CS_STOP;

	CtrlState ret = _DoCtrl(uCurTime);
	m_uCurTime = uCurTime;
	return ret;
}