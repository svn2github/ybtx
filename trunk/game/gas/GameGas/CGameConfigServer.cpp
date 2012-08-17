#include "stdafx.h"
#include "CGameConfigServer.h"
#include "CAppConfigServer.h"
#include "CXmlConfig.inl"

CGameConfigServer* CGameConfigServer::IntInst()
{
	static CGameConfigServer ls_pInst;
	return &ls_pInst;
}

CGameConfigServer* CGameConfigServer::Inst()
{
	return IntInst();
}

CGameConfigServer::CGameConfigServer()
{
	m_pConfig = CAppConfigServer::Inst()->XmlConfig();

	m_bPreventStateMachineCyc		= m_pConfig->DftGet<bool>(true, "ShellConfig","AI_Infinite_Check" ,"State_Machine_Prevent_Cyc" );
	m_uPreventStateMachineCycStep	= m_pConfig->DftGet<uint32>(1000,"ShellConfig", "AI_Infinite_Check" ,"State_Machine_Prevent_Cyc_Step" );
	m_uPreventStateMachineTimeLimit	= m_pConfig->DftGet<uint32>(1500, "ShellConfig","AI_Infinite_Check" ,"State_Machine_Prevent_Cyc_Time_Limit" );
	m_bPreventStateMoveCyc			= m_pConfig->DftGet<bool>(true,"ShellConfig", "AI_Infinite_Check" ,"State_Move_Prevent_Cyc" );
	m_uPreventStateMoveCycStep		= m_pConfig->DftGet<uint32>(1000,"ShellConfig", "AI_Infinite_Check" ,"State_Move_Prevent_Cyc_Step" );
	m_uPreventStateMoveTimeLimit	= m_pConfig->DftGet<uint32>(1500,"ShellConfig", "AI_Infinite_Check" ,"State_Move_Prevent_Cyc_Time_Limit" );
	m_bCheckNpcEnmity				= m_pConfig->DftGet<bool>(true,"ShellConfig", "AI_Infinite_Check" ,"AI_Enmity_Check" );
	
	m_bCheckSkillValueSafe			= m_pConfig->DftGet<bool>(true,"ShellConfig", "Check_Skill_Value_Safe" );
	
	m_fPlayerViewAoiEyeSight	= m_pConfig->Get<float>("ShellConfig","Aoi_Setting", "PlayerViewAoiEyeSight");
	m_fPlayerAoiSize			= m_pConfig->Get<float>("ShellConfig","Aoi_Setting", "PlayerAoiSize");
	m_fPlayerAoiStealth			= m_pConfig->Get<float>("ShellConfig","Aoi_Setting", "PlayerAoiStealth");
	m_fPlayerAoiKeenness		= m_pConfig->Get<float>("ShellConfig","Aoi_Setting", "PlayerAoiKeenness");
	m_uPlayerAoiViewGroup		= m_pConfig->Get<uint32>("ShellConfig","Aoi_Setting", "PlayerAoiViewGroup");
	m_fPlayerSleepDimSize		= m_pConfig->Get<float>("ShellConfig","Aoi_Setting", "PlayerSleepDimSize");
	m_fNpcSleepDimEyeSigh		= m_pConfig->Get<float>("ShellConfig","Aoi_Setting", "NpcSleepDimEyeSigh");
}

CGameConfigServer::~CGameConfigServer()
{

}

bool CGameConfigServer::BePreventStateMachineCyc() const
{
	return m_bPreventStateMachineCyc;
}

uint32 CGameConfigServer::GetStateMachineCycCheckStep() const
{
	return m_uPreventStateMachineCycStep;
}

uint32 CGameConfigServer::GetStateMachineCycTimeLimit() const
{
	return m_uPreventStateMachineTimeLimit;
}

bool CGameConfigServer::BePreventStateMoveCyc() const
{
	return m_bPreventStateMoveCyc;
}

uint32 CGameConfigServer::GetStateMoveCycCheckStep() const
{
	return m_uPreventStateMoveCycStep;
}

uint32 CGameConfigServer::GetStateMoveCycTimeLimit() const
{
	return m_uPreventStateMoveTimeLimit;
}

bool CGameConfigServer::GetCheckSkillValueSafeState()const
{
	return m_bCheckSkillValueSafe;
}

bool CGameConfigServer::GetCheckNpcEnmity() const
{
	return m_bCheckNpcEnmity;
}

float CGameConfigServer::GetPlayerViewAoiEyeSight() const
{
	return m_fPlayerViewAoiEyeSight;
}

float CGameConfigServer::GetPlayerAoiSize() const
{
	return m_fPlayerAoiSize;
}

float CGameConfigServer::GetPlayerAoiStealth() const
{
	return m_fPlayerAoiStealth;
}

float CGameConfigServer::GetPlayerAoiKeenness() const
{
	return m_fPlayerAoiKeenness;
}

uint32 CGameConfigServer::GetPlayerAoiViewGroup() const
{	
	return m_uPlayerAoiViewGroup;
}

float CGameConfigServer::GetPlayerSleepDimSize() const
{
	return m_fPlayerSleepDimSize;
}

float CGameConfigServer::GetNpcSleepEyeSigh() const
{
	return m_fNpcSleepDimEyeSigh;
}




