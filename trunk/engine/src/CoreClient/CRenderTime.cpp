#include "stdafx.h"
#include "CRenderTime.h"
#include "BaseHelper.h"
#include "ExpHelper.h"

CRenderTime::CRenderTime( IDistortedTime* pDtObj)
: m_pDtObj(pDtObj)
{
	Ast(m_pDtObj&&"时间控制不能为空");
	m_pDtObj->AddRef();
}

CRenderTime::~CRenderTime()
{
	SafeRelease(m_pDtObj);
}

uint32	CRenderTime::GetCurTime(void)
{
	return (uint32)m_pDtObj->GetDistortedProcessTime();
}