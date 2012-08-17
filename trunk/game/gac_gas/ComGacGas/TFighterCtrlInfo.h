#pragma once
#include "FightDef.h"
#include <set>

class BoolType{};
class CountType{};

class CFighterCtrlStateMgr
{
public:
	template<typename FighterInfoType>
	void SetState(FighterInfoType* pInfo,EFighterCtrlState eState,bool bSet);
	bool GetState(EFighterCtrlState eState)const
	{
		return eState == eFCS_None ? (int32)m_uCtrlState == eState : ((int32)m_uCtrlState & eState) == eState;
	}
protected:
	CFighterCtrlStateMgr():m_uCtrlState(0){};
private:
	template<typename ImpClass>
	friend class TFighterCtrlInfo;
	uint32 m_uCtrlState;
};

template<typename CtrlType>
class TFighterCtrlStateMgr {};

template<>
class TFighterCtrlStateMgr<BoolType>:public CFighterCtrlStateMgr {};




template<>
class TFighterCtrlStateMgr<CountType>:public CFighterCtrlStateMgr
{
public:
	template<typename FighterInfoType>
	void SetState(FighterInfoType* pInfo,EFighterCtrlState eState,bool bSet);
private:
	typedef map<EFighterCtrlState,uint32> MapCtrlCount;
	typedef map<EFighterCtrlState,uint32>::iterator MapCtrlCount_Iterator;
	MapCtrlCount m_mapCtrlCount;
};

template<typename ImpClass>
class TPropertyHolder;

template<typename CtrlType>
class TFighterCtrlInfo
{
public:
	bool CppIsAlive()const;
	void CppSetAlive(bool bAlive);
	bool GetCtrlState(EFighterCtrlState eState)const;
	uint32 GetAllCtrlState()const 
	{
		if (m_pAliveState != NULL)
		{
			return m_pAliveState->m_uCtrlState; 
		}	
		return 0;
	}
	void SetAllCtrlState(uint32 uAllCtrlValue)	{m_pAliveState->m_uCtrlState = uAllCtrlValue;}
protected:
	TFighterCtrlInfo(void);
	~TFighterCtrlInfo(void);
private:
	template<typename ImpClass>
	friend class TPropertyHolder;

	TFighterCtrlStateMgr<CtrlType> *	m_pAliveState;	//如果为NULL,则死亡

	template<typename FighterInfoType>
	bool IntSetCtrlState(FighterInfoType* pInfo,EFighterCtrlState eState,bool bSet);
};
