#include "stdafx.h"
#include "CActorServer.h"
#include "PtrlGas2GacCPPDef.h"
#include "PtrlGac2GasCPPDef.h"
#include "IActorServerHandler.h"
#include "ISend.h"
#include "CEntityServer.h"
#include "TimeHelper.h"
#include "CAniKeyFrameCfg.h"
#include "CCharacterDictator.h"
#include "CFighterDictator.h"
#include "NormalAttack.h"
#include "TSqrAllocator.inl"

CActorServer::CActorServer()
	: m_pHandler(NULL)
	, m_uCascadeActionCount(0)
	, m_uServerActionDir(160)
	, m_pVariantStateForce(NULL)
	, m_pVariantMoveState(NULL)
	, m_pVariantMoveDir(NULL)
	, m_pVariantActionRhythm(NULL)
	, m_pVariantActionIndex(NULL)
	, m_pVariantHurtResult(NULL)
	, m_pVariantActionState(NULL)
	, m_pVariantDirForce(NULL)
	, m_pVariantDirGac2Gas(NULL)
	, m_pVariantActionDir(NULL)
	, m_pVariantWalkBackRadius(NULL)
	, m_pVariantNAActionRhythm(NULL)
{
}

CActorServer::~CActorServer()
{
}

void CActorServer::InitActorVariant()
{
	CSyncVariantServer* pActorVariant=GetEntity()->GetSyncVariant(eCTI_Agile)->SetMember("Actor");

	(m_pVariantStateForce=pActorVariant->SetMember("StateForce"))
		->SetNumber((bool)true);
	(m_pVariantMoveState=pActorVariant->SetMember("MoveState"))
		->SetNumber((uint8)eMS_Stop);
	(m_pVariantMoveDir=pActorVariant->SetMember("MoveDir"))
		->SetNumber((uint8)160);
	(m_pVariantActionRhythm=pActorVariant->SetMember("ActionRhythm"))
		->SetNumber((float)1.0f);
	(m_pVariantActionIndex=pActorVariant->SetMember("ActionIndex"))
		->SetNumber((uint8)0);
	(m_pVariantHurtResult=pActorVariant->SetMember("HurtResult"))
		->SetNumber((uint8)eHR_Hit);
	(m_pVariantActionState=pActorVariant->SetMember("ActionState"))
		->SetNumber((uint8)eAS_Idle_BackWpn);
	(m_pVariantDirForce=pActorVariant->SetMember("DirForce"))
		->SetNumber((bool)true);
	(m_pVariantDirGac2Gas=pActorVariant->SetMember("DirGac2Gas"))
		->SetNumber((bool)false);
	(m_pVariantActionDir=pActorVariant->SetMember("ActionDir"))
		->SetNumber((uint8)160);
	(m_pVariantWalkBackRadius=pActorVariant->SetMember("WalkBackRadius"))
		->SetNumber((uint8)GetCharacter()->GetFighter()->GetNormalAttackMgr()->GetAttackDistance());
	(m_pVariantNAActionRhythm=pActorVariant->SetMember("NAActionRhythm"))
		->SetNumber((float)1.0F);
}

CEntityServer* CActorServer::GetEntity()const
{
	Ast(GetHandler());
	return GetHandler()->GetEntity();
}

uint32 CActorServer::GetEntityID() const
{
	Ast(GetHandler());
	return GetHandler()->GetEntityID();
}

CCharacterDictator* CActorServer::GetCharacter() const
{
	Ast(GetHandler()); 
	return GetHandler()->GetCharacter();
}

void CActorServer::OnMoveBegan()
{
	CCharacterDictator* pCharacter = GetCharacter();
	ChangeMoveState(eMS_Move);
	if (pCharacter->CppGetCtrlState(eFCS_InWalkState))
	{
		if (pCharacter->CppGetCtrlState(eFCS_InDrawWeaponMode))
			ChangeActionState(eAS_Walk_HoldWpn);
		else
			ChangeActionState(eAS_Walk_BackWpn);
	}
	else
	{
		if (pCharacter->CppGetCtrlState(eFCS_InBattle))
		{
			ChangeActionState(eAS_Run_Battle);	
		}
		else
		{
			if (pCharacter->CppGetCtrlState(eFCS_InDrawWeaponMode))
				ChangeActionState(eAS_Run_HoldWpn);
			else
				ChangeActionState(eAS_Run_BackWpn);
		}	
	}
}

void CActorServer::OnMoveEnded()
{
	CCharacterDictator* pCharacter = GetCharacter();
	if (pCharacter->CppIsLive())
	{
		ChangeMoveState(eMS_Stop);
		if (pCharacter->CppGetCtrlState(eFCS_InBattle))
		{
			ChangeActionState(eAS_Idle_Battle);
		}
		else
		{
			if (pCharacter->CppGetCtrlState(eFCS_InDrawWeaponMode))
				ChangeActionState(eAS_Idle_HoldWpn);
			else
				ChangeActionState(eAS_Idle_BackWpn);
		}
	}
}

void CActorServer::OnChangeNAAniKeyFrame(const TCHAR* szAniFileName)
{
	m_pKeyFrameIniFile = CAniKeyFrameCfg::GetAniKeyFrameCfg(szAniFileName);
	if (!m_pKeyFrameIniFile)
	{
		stringstream str;
		str << szAniFileName << endl;
		GenErr("初始动作关键帧失败,请检查配置表与关键帧文件名是否正确.", str.str());
	}
}

void CActorServer::OnChangeWFAniKeyFrame(const TCHAR* szAniFileName)
{
	m_pWFKeyFrameIniFile = CAniKeyFrameCfg::GetAniKeyFrameCfg(szAniFileName);
	if (!m_pWFKeyFrameIniFile)
	{
		stringstream str;
		str << szAniFileName << endl;
		GenErr("初始玩法动作关键帧失败,请检查配置表与关键帧文件名是否正确.", str.str());
	}
}

uint8 CActorServer::GetHitFrameByActionState(EActionState eActionState)
{
	uint8 uKeyFrame(0);
	switch (eActionState)
	{		
	case eAS_Attack_Right:	uKeyFrame = m_pKeyFrameIniFile->GetValue("attack01", "k", 0);	break;
	case eAS_Attack_Left:	uKeyFrame = m_pKeyFrameIniFile->GetValue("attack02", "k", 0);	break;
	case eAS_Attack_All:	uKeyFrame = m_pKeyFrameIniFile->GetValue("attack03", "k", 0);	break;
	case eAS_Strike:		uKeyFrame = m_pKeyFrameIniFile->GetValue("attack04", "k", 0);	break;
	case eAS_Attack_Assist:	uKeyFrame = m_pKeyFrameIniFile->GetValue("attack05", "k", 0);	break;
	case eAS_Attack_Fist:	uKeyFrame = m_pKeyFrameIniFile->GetValue("attack07", "k", 0);	break;
	case eAS_Take:			uKeyFrame = m_pWFKeyFrameIniFile->GetValue("take", "k", 0);		break;
	case eAS_Fire:			uKeyFrame = m_pWFKeyFrameIniFile->GetValue("fire", "k", 0);		break;
	default:				uKeyFrame = 16;					break;
	}

	if (eActionState >= eAS_Attack_Right && eActionState <= eAS_Strike && uKeyFrame == 0)
		uKeyFrame = m_pKeyFrameIniFile->GetValue("attack01", "k", 0);

	return uKeyFrame;
}

uint8 CActorServer::GetEndFrameByActionState(EActionState eActionState)
{
	switch (eActionState)
	{
	case eAS_Attack_Right:	return m_pKeyFrameIniFile->GetValue("attack01", "e", 0);	break;
	case eAS_Attack_Left:	return m_pKeyFrameIniFile->GetValue("attack02", "e", 0);	break;
	case eAS_Attack_All:	return m_pKeyFrameIniFile->GetValue("attack03", "e", 0);	break;
	case eAS_Strike:		return m_pKeyFrameIniFile->GetValue("attack04", "e", 0);	break;
	case eAS_Attack_Assist: return m_pKeyFrameIniFile->GetValue("attack05", "e", 0);	break;
	case eAS_Fire:			return m_pWFKeyFrameIniFile->GetValue("fire", "e", 0);		break;
	default:				return 35;				break;
	}
}

uint8 CActorServer::GetCastKeyFrameByAniName(const string& strAniName)
{
	if (!strAniName.empty())
		return m_pKeyFrameIniFile->GetValue(strAniName.c_str(), "k", 0);
	
	return 0;
}

CSyncVariantServer* CActorServer::GetVariantStateForce()
{
	return m_pVariantStateForce;	
}

CSyncVariantServer* CActorServer::GetVariantMoveState()
{
	return m_pVariantMoveState;
}

CSyncVariantServer* CActorServer::GetVariantMoveDir()
{
	return m_pVariantMoveDir;
}

CSyncVariantServer* CActorServer::GetVariantActionRhythm()
{
	return m_pVariantActionRhythm;
}

CSyncVariantServer* CActorServer::GetVariantActionIndex()
{
	return m_pVariantActionIndex;	
}

CSyncVariantServer* CActorServer::GetVariantHurtResult()
{
	return m_pVariantHurtResult;
}

CSyncVariantServer* CActorServer::GetVariantActionState()
{
	return m_pVariantActionState;
}

CSyncVariantServer* CActorServer::GetVariantDirForce()
{
	return m_pVariantDirForce;
}

CSyncVariantServer* CActorServer::GetVariantDirGac2Gas()
{
	return m_pVariantDirGac2Gas;
}

CSyncVariantServer* CActorServer::GetVariantActionDir()
{
	return m_pVariantActionDir;
}

CSyncVariantServer* CActorServer::GetVariantWalkBackRadius()
{
	return m_pVariantWalkBackRadius;
}

CSyncVariantServer* CActorServer::GetVariantNAActionRhythm()
{
	return m_pVariantNAActionRhythm;
}
