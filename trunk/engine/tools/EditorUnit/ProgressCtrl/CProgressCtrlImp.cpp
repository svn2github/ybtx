#include "StdAfx.h"
#include "ProgressCtrl/CProgressCtrlImp.h"

DEF_UI_IMP(CProgressCtrlImp);
CProgressCtrlImp::CProgressCtrlImp(void* param)
{
	m_nStep  = 1;
	m_nValue = 0;
	m_nMax	 = 0;
	m_nMin	 = 0;
}

CProgressCtrlImp::~CProgressCtrlImp(void)
{
}

void CProgressCtrlImp::Update()
{
	CCtrlBase::Update();
}

void CProgressCtrlImp::SetProgressStep( const int nStep )
{
	m_nStep = nStep;
}

int CProgressCtrlImp::GetProgressStep() const
{
	return m_nStep;
}

void CProgressCtrlImp::SetProgressValue( const int nValue )
{
	m_nValue = nValue;
}

int CProgressCtrlImp::GetProgressValue() const
{
	return m_nValue;
}

void CProgressCtrlImp::SetProgressMaxValue( const int nMax )
{
	m_nMax = nMax;
}

int CProgressCtrlImp::GetProgressMaxValue() const
{
	return m_nMax;
}

void CProgressCtrlImp::SetProgressMinValue( const int nMin )
{
	m_nMin = nMin;
}

int CProgressCtrlImp::GetProgressMinValue() const
{
	return m_nMin;
}


