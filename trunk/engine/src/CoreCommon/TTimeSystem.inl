#pragma once
#include "TTimeSystem.h"
#include "CTickMgr.h"
#include "TimeHelper.h"


template<typename ImpTimeSystem_t>
TTimeSystem<ImpTimeSystem_t>::TTimeSystem(uint32 uBaseCyc)
:m_pTickMgr(NULL)
,m_bOnTick(false)
{
	m_pTickMgr=new CTickMgr(uBaseCyc,1024);
	Reset();
}

template<typename ImpTimeSystem_t>
TTimeSystem<ImpTimeSystem_t>::~TTimeSystem()
{
	delete m_pTickMgr;
}

template<typename ImpTimeSystem_t>
void TTimeSystem<ImpTimeSystem_t>::Init(uint32 uBaseCyc)
{
	Inst()=new ImpTimeSystem_t(uBaseCyc);
}

template<typename ImpTimeSystem_t>
void TTimeSystem<ImpTimeSystem_t>::Unit()
{
	delete Inst();
	Inst()=NULL;
}

template<typename ImpTimeSystem_t>
ImpTimeSystem_t*& TTimeSystem<ImpTimeSystem_t>::Inst()
{
	static ImpTimeSystem_t* ls_Inst=NULL;
	return ls_Inst;
}

template<typename ImpTimeSystem_t>
uint64 TTimeSystem<ImpTimeSystem_t>::GetFrameTime()const
{
	return m_uLogicTime;
}

template<typename ImpTimeSystem_t>
uint64 TTimeSystem<ImpTimeSystem_t>::GetBaseTime()const
{
	return m_uLogicBaseTime;
}

template<typename ImpTimeSystem_t>
uint64 TTimeSystem<ImpTimeSystem_t>::GetPushTime()const
{
	return m_uPushTime-m_uLogicBaseTime;
}

template<typename ImpTimeSystem_t>
int32 TTimeSystem<ImpTimeSystem_t>::GetTimeError()const
{
	return int32(int64(m_uLogicTime)-int64(GetProcessTime()));
}

template<typename ImpTimeSystem_t>
void TTimeSystem<ImpTimeSystem_t>::Reset()
{
	Reset(GetProcessTime());
}

template<typename ImpTimeSystem_t>
void TTimeSystem<ImpTimeSystem_t>::Reset(uint64 uProcessTime)
{
	m_uLogicBaseTime=m_uLogicTime=m_uPushTime=m_uRealTime=uProcessTime;
}

template<typename ImpTimeSystem_t>
bool TTimeSystem<ImpTimeSystem_t>::PushLogicTime()
{
	return PushLogicTime(GetProcessTime());
}

template<typename ImpTimeSystem_t>
bool TTimeSystem<ImpTimeSystem_t>::IsOnTick(void)
{
	return m_bOnTick;
}

template<typename ImpTimeSystem_t>
bool TTimeSystem<ImpTimeSystem_t>::PushLogicTime(uint64 uRealTime)
{
	m_uPushTime = uRealTime;
	m_uRealTime = uRealTime;

	if( m_uLogicTime>uRealTime || m_bOnTick )
		return false;

	m_bOnTick = true;
	m_uLogicTime += m_pTickMgr->GetInterval();
	m_pTickMgr->OnTick();
	m_bOnTick = false;

	return m_uLogicTime>uRealTime;
}

template<typename ImpTimeSystem_t>
uint32 TTimeSystem<ImpTimeSystem_t>::GetBaseCyc()const
{
	return m_pTickMgr->GetInterval();
}

template<typename ImpTimeSystem_t>
void TTimeSystem<ImpTimeSystem_t>::Register(CTick* pTick,uint32 uCyc)
{
	m_pTickMgr->Register(pTick,uCyc);
}

template<typename ImpTimeSystem_t>
void TTimeSystem<ImpTimeSystem_t>::UnRegister(CTick* pTick)
{
	m_pTickMgr->UnRegister(pTick);
}

template<typename ImpTimeSystem_t>
size_t TTimeSystem<ImpTimeSystem_t>::GetRegisterTickNum()
{
	return m_pTickMgr->GetRegisterTickNum();
}

template<typename ImpTimeSystem_t>
void TTimeSystem<ImpTimeSystem_t>::GetRegisterTickInfo(std::string& strInfo)
{
	m_pTickMgr->GetRegisterTickInfo(strInfo);
}

template<typename ImpTimeSystem_t>
volatile uint64* TTimeSystem<ImpTimeSystem_t>::GetRealTimePtr()
{
	return &m_uRealTime;
}

