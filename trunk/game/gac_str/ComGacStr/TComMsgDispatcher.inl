#pragma once 
#include "TComMsgDispatcher.h"
#include "TMsgDispatcher.inl"


template <typename ImpClass, typename Traits, typename ConnClass>
void TComMsgDispatcher<ImpClass, Traits, ConnClass>::RegisterMsgHandler()
{
	//For Character
	Traits::Character::SetHandlerNumber(eGas2GacCPP_Character_End, -eGas2GacCPP_Character_Begin);

	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_InitFollower> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_InitFollowerSyncInfo> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_InitFollowerEnd> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_OnDead> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_OnRemoved> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_OnReborn> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_ClearFollowerSyncInfo> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_InitDirector> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_InitDirectorEnd> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_BeginTransfer> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_OnMoveSpeedChange> ();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_OnCancelAutoTrack>();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_DestoryBattleHorse>();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_RemoveTeamMember>();
	Traits::Character::template RegisterCharacterCommand<CGas2GacCPP_TeamSyncInfo>();


	//for Fighter
	Traits::Fighter::SetHandlerNumber(eGas2GacCPP_Fighter_End, -eGas2GacCPP_Fighter_Begin);

	//Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnCtrlValueChanged>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnLevelChanged>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnClassChanged>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnCampChanged>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnPKStateChanged>();
	//Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnZoneTypeChanged>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnDisplayFightInfo>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnDisplayMagicFightInfo>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnDisplayMsg>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnDoMagicOp>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnClearState>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnDeleteState>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnSetState>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnSkillBegin>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnCastMagicSkill>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnCancelMagicSkill>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnSetNormalSkillTarget>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnCanBeTakeOver>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnCanNotTakeOver>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnLostTarget>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnRealNameChange>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnGameCampChanged>();
	Traits::Fighter::template RegisterFighterFollowerCommand <CGas2GacCPP_OnSyncCastingProcess>();

	//For Director
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnMagicCondFail>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnCoolDownBegin>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnCoolDownEnd>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnAllCoolDownEnd>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnClearCommonCD>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnCancelNormalAttack>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnDoNormalAttack>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnSetAutoNormalAttack>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnCalValueChanged>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnArrayValueChange>(); 
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnArrayCreate>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnPrintFightInfo>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnAttackTypeChanged>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnSetNoSingTime>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_UpdateSmashRate>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnSyncToSelfCalValueChanged>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnReplaceSkill>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnUseSkillStateChanged>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnRevertSkillCtrlState>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnTalentChanged>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnRemoveTalent>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnBurstSoulTimesChanged>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnAddServant>();
	Traits::Fighter::template RegisterFighterDirectorCommand <CGas2GacCPP_OnDestroyServant>();
	

	//For Magic
	Traits::Fighter::template RegisterMagicCommand<CGas2GacCPP_OnBurstSoul>();

	Traits::Apex::SetHandlerNumber(eGas2GacCpp_ApexProxyMsg_End, -eGas2GacCpp_ApexProxyMsg_Begin);

	Traits::Apex::template RegisterApexCommand<CGas2GacCpp_ApexProxyMsg> ();
}

template <typename ImpClass, typename Traits, typename ConnClass>
template <typename CmdClass> 
void TComMsgDispatcher<ImpClass, Traits, ConnClass>::RegisterFighterCommand(){}

template <typename ImpClass, typename Traits, typename ConnClass>
template <typename CmdClass> 
void TComMsgDispatcher<ImpClass, Traits, ConnClass>::RegisterCharacterCommand(){}

template <typename ImpClass, typename Traits, typename ConnClass>
template <typename CmdClass> 
void TComMsgDispatcher<ImpClass, Traits, ConnClass>::RegisterActorCommand(){}



template <typename ImpClass, typename Traits, typename ConnClass>
template <typename CmdClass> 
void TComMsgDispatcher<ImpClass, Traits, ConnClass>::RegisterFighterFollowerCommand(){}


template <typename ImpClass, typename Traits, typename ConnClass>
template <typename CmdClass> 
void TComMsgDispatcher<ImpClass, Traits, ConnClass>::RegisterFighterDirectorCommand(){}


template <typename ImpClass, typename Traits, typename ConnClass>
template <typename CmdClass> 
void TComMsgDispatcher<ImpClass, Traits, ConnClass>::RegisterMagicCommand(){}


template <typename ImpClass, typename Traits, typename ConnClass>
template <typename CmdClass> 
void TComMsgDispatcher<ImpClass, Traits, ConnClass>::RegisterApexCommand(){}
