#pragma once
#include "TBasePrtlMsg.h"
#include "PtrlGac2GasCPPDcl.h"
#include "CDir.h"
#include "FightDef.h"
#include "ActorDef.h"
#include "TPackedPos.h"

#pragma pack(push,1)

	template<typename ImpClass,EGac2GasCPPInvoke uId>
	class TBaseGac2GasCPP
		:public TBasePrtlMsg<ImpClass,uint16,uId>
	{
	};

#define Cmd_Begin(CommandName,CommandId) \
	class CommandName\
	:public TBaseGac2GasCPP<CommandName,CommandId>\
	{\
	public:\
	static const TCHAR* GetName()\
	{\
	return #CommandName;\
	}

#define Cmd_End };


	/////////////////////////////////////////////////Protocol definition begin//////////////////////////////////////////////////////////

	// Fighter Begin
	Cmd_Begin( CGac2GasCPP_DoSkill, eGac2GasCPP_DoSkill );
	uint8	uMagicDir;
	CPackedPosFloat fPosFrom;
	uint32	uqbSkillID;
	uint32	uSkillLevel;
	uint8	uPos;
	uint8	uDir;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_DoNormalAttack, eGac2GasCPP_DoNormalAttack);
	bool bToBeStarted; // true为开始普攻, false为取消普攻
	uint32 uTargetID;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_StartOrRestartNADueToSkillFailure, eGac2GasCPP_StartOrRestartNADueToSkillFailure);
	uint32 uqbSkillID;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_CancelState, eGac2GasCPP_CancelState);
	uint32 uID;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_CancelCoerceMove, eGac2GasCPP_CancelCoerceMove);
	uint32 uID;
	Cmd_End;

	Cmd_Begin( CGac2GasCPP_StopNormalAttack, eGac2GasCPP_StopNormalAttack );
	Cmd_End;

	Cmd_Begin( CGac2GasCPP_SetTargetPos, eGac2GasCPP_SetTargetPos )
	CPackedPosFloat Pos;
	Cmd_End;

	Cmd_Begin( CGac2GasCPP_OpenObjPanel, eGac2GasCPP_OpenObjPanel );
	bool bIsTargetPanel;
	bool IsOpenpanel;
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_DestroyServant, eGac2GasCPP_DestroyServant);
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_SetAutoNormalAttack, eGac2GasCPP_SetAutoNormalAttack);
	bool bIsAuto;
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_CancelCastingProcess, eGac2GasCPP_CancelCastingProcess);
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_GetTargetCastingProcess, eGac2GasCPP_GetTargetCastingProcess);
	uint32 uTargetID;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_GetTargetOfTarget, eGac2GasCPP_GetTargetOfTarget);
	uint32 uTargetID;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_NotifyBurstSoulArrived, eGac2GasCPP_NotifyBurstSoulArrived)
	uint8 uBurstSoulType;
	Cmd_End;
	// Fighter End

	// Character Begin
	Cmd_Begin(CGac2GasCPP_LockObj, eGac2GasCPP_LockObj);
	uint32 uLockObjId;
	Cmd_End;

	Cmd_Begin( CGac2GasCPP_MoveByDirection, eGac2GasCPP_MoveByDirection );
	uint8 uMoveDir;
	uint32 uObjID;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_SyncActionDir, eGac2GasCPP_SyncActionDir)
	bool bSyncToGac;
	uint8 uActionDir;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_ChangeMoveMode, eGac2GasCPP_ChangeMoveMode)
	bool bIsWalkMode;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_ChangeDrawWeaponMode, eGac2GasCPP_ChangeDrawWeaponMode)
	bool bForceDraw;
	bool bIsDrawWeaponMode;
	Cmd_End;

	Cmd_Begin(CGac2GasCPP_ChangeNormalAttacking, eGac2GasCPP_ChangeNormalAttacking)
	bool bIsNormalAttacking;
	Cmd_End;
	// Character End

	/////////////////////////////////////////////////Protocol definition end////////////////////////////////////////////////////////////

	// ApexProxy
	Cmd_Begin(CGac2GasCPP_ApexProxyMsg, eGac2GasCPP_ApexProxyMsg)
	uint16 uLen;
	uint32 GetPayloadSizeDerived() const
	{
		return uLen;
	}
	Cmd_End

#undef Cmd_End
#undef Cmd_Begin

#pragma pack(pop)
