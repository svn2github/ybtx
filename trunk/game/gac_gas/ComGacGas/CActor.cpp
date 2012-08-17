#include "stdafx.h"
#include "CActor.h"
#include "ExpHelper.h"
#include <ostream>
#include <iostream>

CActor::CActor()
	: m_ePreMoveState(eMS_Stop)
	, m_eMoveState(eMS_Stop)
	, m_ePreActionState(eAS_Idle_BackWpn)
	, m_eActionState(eAS_Idle_BackWpn)
{
}

bool CActor::ChangeMoveState(EMoveState eMoveState, bool bForce)
{
	if (!CheckMoveState(eMoveState))
		return false;

	if ((eMS_LockStop == m_eMoveState || eMS_LockMove == m_eMoveState) && !bForce)
		return false;

	m_ePreMoveState = m_eMoveState;
	m_eMoveState = eMoveState;
	return true;
}

bool CActor::ChangeActionState(EActionState eActionState)
{
	if (!CheckActionState(eActionState))
		return false;

	m_ePreActionState = m_eActionState;
	m_eActionState = eActionState;
	return true;
}

const TCHAR* CActor::GetMoveStateStr(EMoveState eMoveState) const
{
	switch (eMoveState)
	{
		case eMS_LockStop:		return "LockStop";
		case eMS_Stop:			return "Stop";
		case eMS_Move:			return "Move";
		case eMS_LockMove:		return "LockMove";
		default: break;
	}
	return "[Error] : eMoveState Is Inexistent!";
}

const TCHAR* CActor::GetActionStateStr(EActionState eActionState) const
{	
	switch (eActionState)
	{
		case eAS_Die:			return "Die"; 
		case eAS_Dead:			return "Dead"; 
		case eAS_Idle_BackWpn:	return "Idle_BackWpn"; 
		case eAS_Idle_HoldWpn:	return "Idle_HoldWpn"; 
		case eAS_Idle_Battle:	return "Idle_Battle"; 
		case eAS_Walk_BackWpn:	return "Walk_BackWpn"; 
		case eAS_Walk_HoldWpn:	return "Walk_HoldWpn"; 
		case eAS_Walk_Battle:	return "Walk_Battle"; 
		case eAS_Run_BackWpn:	return "Run_BackWpn"; 
		case eAS_Run_HoldWpn:	return "Run_HoldWpn"; 
		case eAS_Run_Battle:	return "Run_Battle"; 
		case eAS_Attack_Right:	return "Attack_Right"; 
		case eAS_Attack_Left:	return "Attack_Left"; 
		case eAS_Attack_All:	return "Attack_All"; 
		case eAS_Strike:		return "Strike"; 
		case eAS_Attack_Ride:	return "Attack_Ride"; 
		case eAS_Attack_Assist: return "Attack_Assist"; 
		case eAS_Attack_Fist:	return "Attack_Fist";
		case eAS_Suffer_Back:	return "Suffer_Back"; 
		case eAS_Suffer_Left:	return "Suffer_Left"; 
		case eAS_Suffer_Right:	return "Suffer_Right"; 
		case eAS_Dodge:			return "Dodge"; 
		case eAS_Block:			return "Block"; 
		case eAS_Parry:			return "Parry"; 
		case eAS_Stun:			return "Stun"; 
		case eAS_HitDown:		return "HitDown";
		case eAS_Sleep:			return "Sleep"; 
		case eAS_Combo:			return "Combo";
		case eAS_Whirl:			return "Whirl";
		case eAS_HoldStill:		return "HoldStill"; 
		case eAS_ResumeAni:		return "ResumeAni"; 
		case eAS_HoldWeapon:	return "HoldWeapon"; 
		case eAS_BackWeapon:	return "BackWeapon"; 
		case eAS_ReachUp:		return "ReachUp"; 	 
		case eAS_Channel:		return "Channel"; 
		case eAS_Sing:			return "Sing"; 
		case eAS_Cast:			return "Cast";
		case eAS_MoveStart:		return "MoveStart";
		case eAS_MoveProcess:	return "MoveProcess";
		case eAS_MoveEnd:		return "MoveEnd";

		case eAS_Take:			return "Take"; 
		case eAS_Fire:			return "Fire"; 
		case eAS_Kick:			return "Kick"; 
		case eAS_Gather:		return "Gather"; 
		case eAS_PickUp:		return "PickUp";
		case eAS_SitDown:		return "SitDown"; 
		case eAS_Sitting:		return "Sitting";
		case eAS_TakeFood:		return "TakeFood";
		case eAS_Item_Use:		return "Item_Use"; 
		case eAS_Item_Sing:		return "Item_Sing"; 
		case eAS_Item_Cast:		return "Item_Cast"; 
		case eAS_Item_Channel:	return "Item_Channel"; 
		case eAS_Fun_Start:		return "Fun_Start"; 
		case eAS_Fun_Keep:		return "Fun_Keep"; 

		case eAS_Birth:			return "Birth"; 
		case eAS_Still_1:		return "Still_1"; 
		case eAS_Still_2:		return "Still_2"; 
		case eAS_Still_3:		return "Still_3"; 
		case eAS_Respond_Enter: return "Respond_Enter"; 
		case eAS_Respond_Keep:	return "Respond_Keep"; 
		case eAS_Respond_Leave: return "espond_Leave"; 
		case eAS_Idle_Special:	return "Idle_Special";
		case eAS_Idle_Special_Loop:	return "Idle_Special_Loop"; 
		case eAS_Die_Special:	return "Die_Special"; 
		case eAS_Dead_Special:	return "Dead_Special"; 
		default: break;
	}
	return "[Error] : eAtionState Is Inexistent!";
}

bool CActor::CheckMoveState(EMoveState eMoveState)
{
	if (eMoveState >= eMS_Max || eMoveState < 0) 
	{
		std::ostringstream oss;
		oss << "无效枚举值：" << eMoveState;
		GenErr(oss.str());
		return false;
	}
	return true;
}

bool CActor::CheckActionState(EActionState eActionState)
{
	if (eActionState >= eAS_Max || eActionState < 0) 
	{
		std::ostringstream oss;
		oss << "无效枚举值：" << eActionState;
		GenErr(oss.str());
		return false;
	}
	return true;
}
