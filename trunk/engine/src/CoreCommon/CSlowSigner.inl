#pragma once
#include "CoreCommon.h"
#include "CSlowSigner.h"
#include "CQuickRand.h"


template<typename CmdType>
void CSlowSigner::SlowSignOnData(const void* pData,size_t stDataSize,CmdType* pCmd)
{
	CQuickRand rand(0);
	
	uint16	udbSeed=0;
	uint8	usbCheckSum=0;
	
	sqr::SlowSignOnData(m_guidConn,pData,stDataSize,m_uModSignCounter,udbSeed,usbCheckSum);

	/*cout<<typeid(CmdType).name()<<","
		<<uint32(m_uModSignCounter)<<","
		<<uint32(usbCheckSum)<<","
		<<uint32(udbSeed)<<endl;*/


	pCmd->usbModCheckSum	= usbCheckSum;
	pCmd->udbSeed			= udbSeed;
	pCmd->usbCounter		= m_uModSignCounter;

	++m_uModSignCounter;
}

template<typename CmdType>
void CSlowSigner::SlowSignOnMediatorCmd(CmdType* pCmd)
{
	CQuickRand rand(0);
	
	uint16	udbSeed=0;
	uint8	usbCheckSum=0;

	const size_t stDataSize=reinterpret_cast<const char*>(&pCmd->udbSeed) - reinterpret_cast<const char*>(pCmd);

	//cout<<typeid(CmdType).name()<<uint32(m_uModSignCounter)<<endl;

	sqr::SlowSignOnData(m_guidConn,pCmd,stDataSize,m_uModSignCounter,udbSeed,usbCheckSum);

	pCmd->usbModCheckSum	= usbCheckSum;
	pCmd->udbSeed			= udbSeed;
	pCmd->usbCounter		= m_uModSignCounter;

	++m_uModSignCounter;
}
