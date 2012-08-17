//$Id$
#include "stdafx.h"
#include "CPipeState.h"
#include "CSyncPipe.h"
#include "ExpHelper.h"

CPipeState::CPipeState(CSyncPipe* pPipe)
:m_pPipe(pPipe)
{
}

CPipeState::~CPipeState(void)
{
}

bool CPipeState::IsBusy()const
{
	return false;
}

size_t CPipeState::Send(const void* pData,size_t stSize)
{
	return 0;
}

float CPipeState::GetDataFlowSpeed()
{
	return 0;
}

bool CPipeState::IsConnecting()const
{
	return false;
}

void CPipeState::ReportStateError(const char* szFunName,const char* szStateName)const
{
	stringstream strm;
	strm<<szFunName<<" pipe is in "<<szStateName<<" state.";
	GenErr(strm.str());
}


