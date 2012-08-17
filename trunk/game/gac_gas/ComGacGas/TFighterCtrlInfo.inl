#pragma once
#include "TFighterCtrlInfo.h"

template<typename FighterInfoType>
void CFighterCtrlStateMgr::SetState(FighterInfoType* pInfo,EFighterCtrlState eState,bool bSet)
{
	uint32 uState = m_uCtrlState;
	if( bSet )
	{
		m_uCtrlState |= eState;
	}
	else
	{
		m_uCtrlState &= ~eState;
	}
	if (uState != m_uCtrlState)
		pInfo->OnCtrlValueChanged(m_uCtrlState, eState, bSet);
}

template<typename FighterInfoType>
void TFighterCtrlStateMgr<CountType>::SetState(FighterInfoType* pInfo,EFighterCtrlState eState,bool bSet)
{
	if (eState<eFCS_CountCtrlValueBegin)
	{
		CFighterCtrlStateMgr::SetState(pInfo,eState,bSet);
	}
	else
	{
		MapCtrlCount_Iterator it = m_mapCtrlCount.find(eState);
		if (bSet)
		{
			if (it!=m_mapCtrlCount.end())
			{
				++it->second;
			}
			else
			{
				m_mapCtrlCount[eState]=1;
			}

			CFighterCtrlStateMgr::SetState(pInfo,eState,bSet);
		}
		else
		{
			if (it!=m_mapCtrlCount.end())
			{
				if(int32(--it->second)<=0)
				{
					CFighterCtrlStateMgr::SetState(pInfo,eState,bSet);
					if(int32(it->second)<0)
					{
							it->second=0;
							stringstream sErrArg;
							sErrArg << "¿ØÖÆÖµ" << eState << " ";
							GenErr("CFighterCtrlInfo::SetState´íÎó\n", sErrArg.str());
					};

				}
			}
		}
	}
}



template<typename CtrlType>
TFighterCtrlInfo<CtrlType>::TFighterCtrlInfo(void)
:m_pAliveState(NULL)
{
	m_pAliveState=new TFighterCtrlStateMgr<CtrlType>;
}

template<typename CtrlType>
TFighterCtrlInfo<CtrlType>::~TFighterCtrlInfo(void)
{
	delete m_pAliveState;
}

template<typename CtrlType>
bool TFighterCtrlInfo<CtrlType>::CppIsAlive()const
{
	return m_pAliveState != NULL;
}

template<typename CtrlType>
void TFighterCtrlInfo<CtrlType>::CppSetAlive(bool bAlive)
{
	if( m_pAliveState )
	{
		if( !bAlive )
		{
			delete m_pAliveState;
			m_pAliveState=NULL;
		}
	}
	else
	{
		if( bAlive )
		{
			m_pAliveState=new TFighterCtrlStateMgr<CtrlType>;
		}
	}
}

template<typename CtrlType>
bool TFighterCtrlInfo<CtrlType>::GetCtrlState(EFighterCtrlState eState)const
{
	if( !CppIsAlive() )
		return false;
	return m_pAliveState->GetState(eState);
}

