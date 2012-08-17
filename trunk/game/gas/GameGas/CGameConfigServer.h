#pragma once
#include "CEntityMallocObject.h"

namespace sqr
{
	class CXmlConfig;
}

class CGameConfigServer : CEntityMallocObject
{
public:
	CGameConfigServer();
	~CGameConfigServer();

	static CGameConfigServer* Inst();
	static CGameConfigServer* IntInst();
	bool BePreventStateMachineCyc() const;
	uint32 GetStateMachineCycCheckStep() const;
	uint32 GetStateMachineCycTimeLimit() const;
	bool BePreventStateMoveCyc() const;
	uint32 GetStateMoveCycCheckStep() const;
	uint32 GetStateMoveCycTimeLimit() const;
	bool GetCheckSkillValueSafeState()const;
	bool GetCheckNpcEnmity() const;
	float GetPlayerViewAoiEyeSight() const;
	float GetPlayerAoiSize() const;
	float GetPlayerAoiStealth() const;
	float GetPlayerAoiKeenness() const;
	uint32 GetPlayerAoiViewGroup() const;
	float GetPlayerSleepDimSize() const;
	float GetNpcSleepEyeSigh() const;
private:
	bool	m_bPreventStateMachineCyc;
	uint32	m_uPreventStateMachineCycStep;
	uint32	m_uPreventStateMachineTimeLimit;
	bool	m_bPreventStateMoveCyc;
	uint32	m_uPreventStateMoveCycStep;
	uint32	m_uPreventStateMoveTimeLimit;
	bool	m_bCheckSkillValueSafe;
	bool	m_bCheckNpcEnmity;
	float	m_fPlayerViewAoiEyeSight;
	float	m_fPlayerAoiSize;
	float	m_fPlayerAoiStealth;
	float	m_fPlayerAoiKeenness;
	uint32	m_uPlayerAoiViewGroup;
	float	m_fPlayerSleepDimSize;
	float	m_fNpcSleepDimEyeSigh;
	CXmlConfig*	m_pConfig;
};
