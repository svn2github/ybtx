#pragma once

enum EGac2GasCPPInvoke
{
	eGac2GasCPP_Invoke_Begin = 32768,

	eGac2GasCPP_Fighter_Begin = eGac2GasCPP_Invoke_Begin,
	eGac2GasCPP_DestroyServant,
	eGac2GasCPP_StartOrRestartNADueToSkillFailure,
	eGac2GasCPP_CancelState,
	eGac2GasCPP_NotifyBurstSoulArrived,
	eGac2GasCPP_SetTargetPos,
	eGac2GasCPP_CancelCoerceMove,
	eGac2GasCPP_DoSkill,
	eGac2GasCPP_OpenObjPanel,
	eGac2GasCPP_StopNormalAttack,
	eGac2GasCPP_SetAutoNormalAttack,
	eGac2GasCPP_CancelCastingProcess,
	eGac2GasCPP_GetTargetOfTarget,
	eGac2GasCPP_GetTargetCastingProcess,
	eGac2GasCPP_DoNormalAttack,
	eGac2GasCPP_Fighter_End,

	eGac2GasCPP_Character_Begin = eGac2GasCPP_Fighter_End,
	eGac2GasCPP_ChangeMoveMode,
	eGac2GasCPP_ChangeDrawWeaponMode,
	eGac2GasCPP_ChangeNormalAttacking,
	eGac2GasCPP_MoveByDirection,
	eGac2GasCPP_LockObj,
	eGac2GasCPP_SyncActionDir,
	eGac2GasCPP_Character_End,

	eGac2GasCPP_ApexProxy_Begin = eGac2GasCPP_Character_End,
	eGac2GasCPP_ApexProxyMsg,
	eGac2GasCPP_ApexProxy_End,

	eGac2GasCPP_Invoke_End
};
