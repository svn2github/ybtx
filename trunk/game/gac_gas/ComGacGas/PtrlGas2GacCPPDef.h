#pragma once
#include "TBasePrtlMsg.h"
#include "PtrlGas2GacCPPDcl.h"
#include "CDir.h"
#include "GameDef.h"
#include "FightDef.h"
#include "ActorDef.h"
#include "TPackedPos.h"

#pragma pack(push,1)

	template<typename ImpClass,EGas2GacCPPInvoke uId>
	class TBaseGas2GacCPP
		:public TBasePrtlMsg<ImpClass,uint16,uId>
	{
	};

#define Cmd_Begin(CommandName,CommandId) \
	class CommandName\
	:public TBaseGas2GacCPP<CommandName,CommandId>\
	{\
	public:\
	static const TCHAR* GetName()\
	{\
	return #CommandName;\
	}

#define Cmd_End };


	/////////////////////////////////////////////////Protocol definition begin//////////////////////////////////////////////////////////

	//Fighter的RPC
	Cmd_Begin( CGas2GacCPP_UpdateSmashRate, eGas2GacCPP_UpdateSmashRate );
	uint32	uObjID;		// 面板打开者
	uint32	uTargetID;	// 面板被打开者
	float	fValue;
	Cmd_End

	Cmd_Begin( CGas2GacCPP_OnCalValueChanged, eGas2GacCPP_OnCalValueChanged );
	uint32	uObjID;		// 面板打开者
	uint32	uTargetID;	// 面板被打开者
	uint32	uValueTypeID;
	float	fValue;
	uint8	uAttackType;
	Cmd_End

	Cmd_Begin( CGas2GacCPP_OnAttackTypeChanged, eGas2GacCPP_OnAttackTypeChanged );
	uint32	uObjID;		// 面板打开者
	uint32	uTargetID;	// 面板被打开者
	uint8	uAttackType;
	Cmd_End

	//Cmd_Begin( CGas2GacCPP_OnCtrlValueChanged, eGas2GacCPP_OnCtrlValueChanged);
	//uint32	uObjID;			//ID
	//uint32	uValue;			//AllCtrlValue
	//uint32	uValueTypeID;	//OneCtrlValue
	//bool	bSet;			//OneCtrlValue's Value
	//bool	bIsAlive;
	//Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnLevelChanged, eGas2GacCPP_OnLevelChanged);
	uint32	uObjID;
	uint32	uLevel;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnClassChanged,eGas2GacCPP_OnClassChanged);
	uint32	uObjID;
	uint8	uClass;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnCampChanged, eGas2GacCPP_OnCampChanged);
	uint32	uObjID;
	uint8	uCamp;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnPKStateChanged, eGas2GacCPP_OnPKStateChanged);
	uint32	uObjID;
	bool	bPKState;
	//uint8	uPKState;
	Cmd_End;

	//Cmd_Begin( CGas2GacCPP_OnZoneTypeChanged, eGas2GacCPP_OnZoneTypeChanged);
	//uint32	uObjID;
	//uint8	uZoneType;
	//Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnDisplayFightInfo, eGas2GacCPP_OnDisplayFightInfo);
	uint32 uObjID;	
	int32 iValue;
	bool bOverTime;
	uint8 uHurtType;
	uint8 eAglieType;
	bool bIsSkill;
	uint32 uSrcEntityID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnDisplayMagicFightInfo, eGas2GacCPP_OnDisplayMagicFightInfo);
	uint32 uObjID;	
	int32 iValue;
	uint8 uHurtType;
	bool bIsSkill;
	uint32 uCoreObjID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnDisplayMsg, eGas2GacCPP_OnDisplayMsg);
	uint32 uObjID;	
	uint32 uMsgID;
	uint16 uMsgFXLen;
	uint32 GetPayloadSizeDerived()const{return uMsgFXLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnTalentChanged,eGas2GacCPP_OnTalentChanged);
	uint32	uObjID;
	int32	iPoint;
	uint16	udbTalentNameLen;

	uint32 GetPayloadSizeDerived()const{return udbTalentNameLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnRemoveTalent, eGas2GacCPP_OnRemoveTalent);
	uint32	uObjID;
	uint16	udbTalentNameLen;
	uint32 GetPayloadSizeDerived()const{return udbTalentNameLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnSetNoSingTime, eGas2GacCPP_OnSetNoSingTime);
	uint32 uObjID;
	uint32 uSkillID;
	bool bNoSingTime;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnSkillBegin, eGas2GacCPP_OnSkillBegin);
	uint32 uObjID;
	uint32 uTargetID;
	uint32 uSkillID;
	uint8 uSkillLevel;
	uint8 uTargetType;
	uint8 uActionDir;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnMagicCondFail, eGas2GacCPP_OnMagicCondFail);
	uint32 uObjID;
	uint32 uMessageID;
	uint16	udbSkillNameLen;

	uint32 GetPayloadSizeDerived()const{return udbSkillNameLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnCoolDownBegin, eGas2GacCPP_OnCoolDownBegin);
	uint32 uObjID;
	uint8 uSkillLevel;
	uint32 uLeftTime;
	bool bIsSwitchEquipSkill;
	uint16	udbSkillNameLen;

	uint32 GetPayloadSizeDerived()const{return udbSkillNameLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnCoolDownEnd, eGas2GacCPP_OnCoolDownEnd);
	uint32 uObjID;
	uint16	udbSkillNameLen;

	uint32 GetPayloadSizeDerived()const{return udbSkillNameLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnClearCommonCD, eGas2GacCPP_OnClearCommonCD);
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnAllCoolDownEnd, eGas2GacCPP_OnAllCoolDownEnd);
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnDoMagicOp, eGas2GacCPP_OnDoMagicOp);
	uint32 uObjID;
	uint32 uMagicOpID;
	uint32 uCreatorID;
	uint32 uTargetID;
	bool bNotPosition;
	bool bPlayFx;
	CPackedPosFloat fPos;
	uint8 uHurtResult;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnCastMagicSkill, eGas2GacCPP_OnCastMagicSkill);
	uint32 uObjID;
	uint32 uMagicID;
	uint8 uType;
	bool bLoop;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnCancelMagicSkill, eGas2GacCPP_OnCancelMagicSkill);
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin(CGas2GacCPP_OnLostTarget,eGas2GacCPP_OnLostTarget)
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnBurstSoul, eGas2GacCPP_OnBurstSoul);
	uint32	uCategoryID;
	uint32	uFromEntityID;
	uint32	uTargetEntityID;
	uint32  uBurstSoulCount;
	uint8	uBurstSoulType;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnArrayCreate, eGas2GacCPP_OnArrayCreate);
	uint32 uObjID;
	uint16		udbNameLen;
	uint16		udbPosLen;
	uint16		udbEntityIDLen;

	uint32 GetPayloadSizeDerived()const{return udbNameLen+udbPosLen+udbEntityIDLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnArrayValueChange, eGas2GacCPP_OnArrayValueChange);
	uint32	uObjID;
	uint32	uTotalValue;
	uint32	uCurrentValue;
	uint32	uSpeed;
	uint16		udbNameLen;

	uint32 GetPayloadSizeDerived()const{return udbNameLen;}
	Cmd_End;

	Cmd_Begin(CGas2GacCPP_OnCancelNormalAttack, eGas2GacCPP_OnCancelNormalAttack);
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnSetAutoNormalAttack, eGas2GacCPP_OnSetAutoNormalAttack);
	uint32	uObjID;
	bool	bIsAutoNA;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnAddServant, eGas2GacCPP_OnAddServant);
	uint32	uObjID;
	uint32	uServantObjID;
	uint8	uType;
	Cmd_End;

	Cmd_Begin(CGas2GacCPP_OnGameCampChanged, eGas2GacCPP_OnGameCampChanged)
	uint32 uObjID;
	int32 iGameCamp;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnDestroyServant, eGas2GacCPP_OnDestroyServant);
	uint32	uObjID;
	uint32	uServantID;
	uint8	uType;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnCanBeTakeOver, eGas2GacCPP_OnCanBeTakeOver);
	uint32 uObjID;
	uint32 uEntityID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnCanNotTakeOver, eGas2GacCPP_OnCanNotTakeOver);
	uint32 uObjID;
	uint32 uEntityID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnDoNormalAttack, eGas2GacCPP_OnDoNormalAttack);
	uint32 uObjID;
	uint32 uTargetID;
	bool bAutoTrack;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnPrintFightInfo, eGas2GacCPP_OnPrintFightInfo);
	uint32 uInfoIndex;
	uint32 uObjID;
	uint32 uObj1ID;
	uint32 uObj2ID;
	int32 iHPChanged; 
	uint32 uHPExtra;
	uint32 uHPRealChange;
	uint8 uAttackType;
	uint8 uHour;
	uint8 uMinute;
	uint8 uSecond;
	uint16 uMillisecond;
	uint32 uStateId;
	bool bSkill;
	uint32 uCfgId;
	uint16 uArgNameLen;
	
	uint32 GetPayloadSizeDerived()const{return uArgNameLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnRealNameChange, eGas2GacCPP_OnRealNameChange);
	uint32 uObjID;
	uint16 udbRealNameLen;

	uint32 GetPayloadSizeDerived()const{return udbRealNameLen;}
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnSyncToSelfCalValueChanged, eGas2GacCPP_OnSyncToSelfCalValueChanged );
	uint32	uObjID;
	uint32	uValueTypeID;
	float	fValue;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnReplaceSkill, eGas2GacCPP_OnReplaceSkill);
	uint32 uObjID;
	bool   bReplace; //是替换还是撤销替换
	uint16 uSkillNameLen;
	uint16 uReplaceSkillNameLen;

	uint32 GetPayloadSizeDerived()const{return uSkillNameLen + uReplaceSkillNameLen;}
	Cmd_End;

	Cmd_Begin(CGas2GacCPP_OnUseSkillStateChanged, eGas2GacCPP_OnUseSkillStateChanged);
	uint32 uObjID;
	uint16 uSkillCtrlState;
	bool   bSet;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnRevertSkillCtrlState, eGas2GacCPP_OnRevertSkillCtrlState);
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnBurstSoulTimesChanged, eGas2GacCPP_OnBurstSoulTimesChanged);
	uint32 uObjID;
	uint32 uBurstSoulTimes;
	Cmd_End;
//魔法状态的RPC
	Cmd_Begin( CGas2GacCPP_OnClearState, eGas2GacCPP_OnClearState );
	uint32 uObjID;
	//uint32 uCmdOrder;
	//uint32 uSendBy;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnDeleteState, eGas2GacCPP_OnDeleteState );
	uint32 uObjID;
	uint32 uCategoryId;
	uint32 uDynamicId;
	//uint32 uCmdOrder;
	//uint32 uSendBy;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnSetState, eGas2GacCPP_OnSetState );
	uint32 uObjID;
	uint32 uCategoryId;
	uint32 uDynamicId;
	uint32 uCount;
	int64 iTime;
	int64 iGuessStartTime;
	uint32 uSkillLevel;
	uint32 uInstallerID;
	int32 iCalcValue;
	//uint32 uCmdOrder;
	//uint32 uSendBy;
	Cmd_End;

	Cmd_Begin(CGas2GacCPP_OnSetNormalSkillTarget, eGas2GacCPP_OnSetNormalSkillTarget);
	uint32 uObjID;
	uint32 uTargetID;
	Cmd_End;

	Cmd_Begin(CGas2GacCPP_OnSyncCastingProcess, eGas2GacCPP_OnSyncCastingProcess);
	uint32 uObjID;
	uint32 uFromID;
	uint16 uCastID;
	uint16 uSkillID;
	float fCastTime;
	uint64 uStartTime;
	Cmd_End;

//Character
	Cmd_Begin( CGas2GacCPP_InitFollower, eGas2GacCPP_InitFollower );
	uint32 uObjID;
	uint32 uEntityID;
	uint32 uDirectorID;
	bool bIsBattleHorse;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_InitFollowerSyncInfo, eGas2GacCPP_InitFollowerSyncInfo );
	uint32 uObjID;
	uint32 uDirectorEntityID;
	uint32 uEntityID;
	uint32 uTargetID;
	uint8 uMoveState;
	uint8 uMoveDir;
	uint8 uActionState;
	uint8 uActionDir;
	uint8 uActionIndex;
	uint8 uWalkBackRadius;
	float fNADistance;
	bool bCanTakeOver;
	float fMoveSpeed;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_InitFollowerEnd, eGas2GacCPP_InitFollowerEnd );
	uint32 uObjID;
	uint32 uEntityID;
	uint32 uDirectorID;
	uint8 uNpcAIType;
	uint8 uNpcType;
	uint8 uNpcAttackType;
	bool bActiveNpc;
	int32 iGameCamp;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnDead, eGas2GacCPP_OnDead );
	uint32	uEntityID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnRemoved, eGas2GacCPP_OnRemoved);
	uint32 uEntityID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnReborn, eGas2GacCPP_OnReborn );
	uint32	uEntityID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_ClearFollowerSyncInfo, eGas2GacCPP_ClearFollowerSyncInfo );
	uint32 uObjID;
	uint32 uDirectorEntityID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_OnMoveSpeedChange, eGas2GacCPP_OnMoveSpeedChange);
	uint32	uEntityID;
	float fSpeed;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_InitDirector, eGas2GacCPP_InitDirector );
	uint32 uObjID;
	uint32 uEntityID;
	uint32	uLevel;
	uint8	uClass;
	uint8	uBirthCamp;
	uint8	uCamp;
	bool	bPKState;
	//uint8	uPKState;
	//uint8	uZoneType;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_InitDirectorEnd, eGas2GacCPP_InitDirectorEnd );
	uint32 uObjID;
	uint32 uEntityID;
	Cmd_End;

	Cmd_Begin( CGas2GacCPP_BeginTransfer, eGas2GacCPP_BeginTransfer );
	uint32 uEntityID;
	bool bTransfer;
	uint16			udbSceneNameLen;
	uint32 GetPayloadSizeDerived()const{return udbSceneNameLen;}
	Cmd_End;

	Cmd_Begin(CGas2GacCPP_OnCancelAutoTrack, eGas2GacCPP_OnCancelAutoTrack);
	uint32 uObjID;
	bool bCancelNAOnly;	//只取消普攻的自动追踪
	Cmd_End;

	Cmd_Begin(CGas2GacCPP_DestoryBattleHorse, eGas2GacCPP_DestroyBattleHorse);
	uint32 uEntityID;
	Cmd_End;

//Team
	Cmd_Begin(CGas2GacCPP_RemoveTeamMember, eGas2GacCPP_RemoveTeamMember);
	uint32 uEntityID;
	Cmd_End;

	Cmd_Begin(CGas2GacCPP_TeamSyncInfo, eGas2GacCPP_TeamSyncInfo);
	uint32 uEntityID;
	uint32 uLevel;
	uint32 uBaseHP;
	uint32 uBaseMP;
	uint32 uBaseEP;
	uint32 uBaseRP;
	uint32 uHP;
	uint32 uMP;
	uint32 uEP;
	uint32 uRP;
	uint32 uHairResID;
	uint32 uFaceResID;
	uint32 uHeadIndexID;
	uint32 uBodyIndexID;
	CPackedPosFloat fPos;
	bool bInBattle;
	Cmd_End;

// ApexProxy
	Cmd_Begin(CGas2GacCpp_ApexProxyMsg, eGas2GacCpp_ApexProxyMsg);
	uint16 uLen;
	uint32 GetPayloadSizeDerived() const
	{
		return uLen;
	}
	Cmd_End
	/////////////////////////////////////////////////Protocol definition end////////////////////////////////////////////////////////////


#undef Cmd_End
#undef Cmd_Begin

#pragma pack(pop)
