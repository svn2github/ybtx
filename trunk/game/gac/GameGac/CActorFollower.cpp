#include "stdafx.h"
#include "CActorFollower.h"
#include "IActorFollowerHandler.h"
#include "CCharacterDirector.h"
#include "FXPlayer.h"
#include "CFacingMgr.h"
#include "CActorCfgClient.h"
#include "CFighterFollower.h"
#include "ICharacterFollowerCallbackHandler.h"
#include "CFacialAniCfgMgr.h"
#include "CRenderObject.h"
#include "CSoundPlayer.h"
#include "CFxSoundPlayer.h"
#include "CActorVariantCallback.h"
#include "CRenderLinkMgr.h"
#include "CDirectionMgr.h"

#define TURN_INTERVAL	10		//每次间隔时间(毫秒)

class CMoveStateNode
{
public:
	CMoveStateNode(MoveFunc);

	MoveFunc	m_MoveFunc;
};

CMoveStateNode::CMoveStateNode(MoveFunc func) : m_MoveFunc(func)
{
}

CActorFollower::CActorFollower()
: m_pActionFX(NULL)
, m_bIsAllBodyAni(false)
, m_bNeedMoveUpBodyAni(false)
, m_eCurOnceActionLevel(eAPP_NULL)
, m_bComboSing(false)
, m_fNAActionRhythm(1.0f)
, m_fNATempRhythm(1.0f)
, m_fCurAniRhythm(1.0f)
, m_nUpperBodyID(-1)
, m_uWalkBackRadius(3)
, m_uMoveDir(0)
, m_uActionDir(160)
, m_uActionIndex(0)
, m_strArmorString("")
, m_eHurtResult(eHR_Hit)
{
}

CActorFollower::~CActorFollower()
{
	SetCurOnceActionLevel(eAPP_NULL);
}

void CActorFollower::RegVariantCallbackFunc()
{
	CCharacterFollower* pCharacter = GetCharacter();
	Ast(pCharacter)
	m_pMoveStateCallback	= new CMoveStateCallback(this);
	m_pMoveDirCallback		= new CMoveDirCallback(this);
	m_pActionStateCallback	= new CActionStateCallback(this);
	m_pActionDirCallback	= new CActionDirCallback(this);
	m_pWalkBackRadiusCallback = new CWalkBackRadiusCallback(this);
	m_pNAActionRhythmCallback = new CNAActionRhythmCallback(this);
	m_pActionFX=new CPlayerFX(pCharacter);

	pCharacter->RegSyncVarChangedCallback("MoveState", m_pMoveStateCallback, eCTI_Agile);
	pCharacter->RegSyncVarChangedCallback("MoveDir", m_pMoveDirCallback, eCTI_Agile);
	pCharacter->RegSyncVarChangedCallback("ActionState", m_pActionStateCallback, eCTI_Agile);
	pCharacter->RegSyncVarChangedCallback("ActionDir", m_pActionDirCallback, eCTI_Agile);
	pCharacter->RegSyncVarChangedCallback("NAActionRhythm", m_pNAActionRhythmCallback, eCTI_Agile);
	pCharacter->RegSyncVarChangedCallback("WalkBackRadius", m_pWalkBackRadiusCallback, eCTI_Agile);

	pCharacter->GetDirectionMgr()->InitDirection();
	SetNeedMoveUpBodyAni(false);
	SetCurOnceActionLevel(eAPP_NULL);
}

void CActorFollower::ClearVariantCallbackFunc()
{
	CCharacterFollower* pCharacter = GetCharacter();
	Ast(pCharacter)
	pCharacter->UnRegSyncVarChangedCallback("MoveState", m_pMoveStateCallback, eCTI_Agile);
	pCharacter->UnRegSyncVarChangedCallback("MoveDir", m_pMoveDirCallback, eCTI_Agile);
	pCharacter->UnRegSyncVarChangedCallback("ActionState", m_pActionStateCallback, eCTI_Agile);
	pCharacter->UnRegSyncVarChangedCallback("ActionDir", m_pActionDirCallback, eCTI_Agile);
	pCharacter->UnRegSyncVarChangedCallback("NAActionRhythm", m_pNAActionRhythmCallback, eCTI_Agile);
	pCharacter->UnRegSyncVarChangedCallback("WalkBackRadius", m_pWalkBackRadiusCallback, eCTI_Agile);

	m_pActionFX->CancelFx();
	m_pActionFX = NULL;
}

IActorFollowerHandler* CActorFollower::GetHandler()const
{
	return static_cast<IActorFollowerHandler*>(CActorClient::GetHandler());
}

uint32 CActorFollower::GetEntityID() const
{
	Ast(GetCharacter());
	return GetCharacter()->GetEntityID();
}

CCharacterFollower* CActorFollower::GetCharacter() const
{
	Ast(GetHandler());
	return GetHandler()->GetCharacter();
}

void CActorFollower::DoNowAni(bool bIsMoveAction)
{
	if (bIsMoveAction)
	{
		if (!GetCharacter()->IsMoving())
			return;
		DoActionState(GetMoveActState(), GetActionDir(), false, false, GetActionIndex());
	}
	else
	{
		// 涉及到动作包切换,防止重复动作
		if (GetActionState() == eAS_Take || GetActionState() == eAS_Fire)
			return;
		DoActionState(GetActionState(), GetActionDir(), false, true, GetActionIndex());
	}
}

EWeaponPosAndBattleState CActorFollower::GetWeaponPosAndBattleState()
{
	CCharacterFollower* pCharFollower = GetCharacter();
	CCharacterDirector* pCharDirector = pCharFollower->CastToCharacterDirector();
	if (pCharDirector)
	{
		if (pCharDirector->CppGetCtrlState(eFCS_InNormalHorse))
			return eWPBS_BackWeapon;

		if (pCharDirector->IsNormalAttacking())
			return eWPBS_BattleState;
	}
	else
	{
		if (pCharFollower->GetIsPlayer())
		{
			if (pCharFollower->CppGetCtrlState(eFCS_InNormalHorse))
				return eWPBS_BackWeapon;

			if (pCharFollower->CppGetCtrlState(eFCS_InNormalAttacking))
				return eWPBS_BattleState;	
		}
	}

	if (pCharFollower->CppGetCtrlState(eFCS_InBattle) || pCharFollower->GetActor()->GetCurOnceActionLevel() == eAPP_AttackAction
		|| pCharFollower->GetActor()->GetCurOnceActionLevel() == eAPP_SkillAction)
	{
		return eWPBS_BattleState;
	}
	else
	{
		if (pCharFollower->CppGetCtrlState(eFCS_InDrawWeaponMode))
			return eWPBS_HoldWeapon;
		else
			return eWPBS_BackWeapon;
	}
}

//--------------------------------------------------------
//					MoveState相关
//--------------------------------------------------------
bool CActorFollower::DoMoveState(EMoveState eMoveState, bool bForce)
{
	bool bAllow = ChangeMoveState(eMoveState, bForce);
	if (bAllow == false)
		return bAllow;

	switch (eMoveState)
	{
	case eMS_Stop:
		{
			MoveFunc func = &CActorFollower::DoStop;
			RegisterMoveAnimation(func);
		}
		break;
	case eMS_Move:
		{
			MoveFunc func = &CActorFollower::DoMove;
			RegisterMoveAnimation(func);
		}
		break;
	default:
		break;
	}

	return bAllow;
}

void CActorFollower::DoMoveAnimation()
{
	list<CMoveStateNode*>::iterator iter = m_lMoveList.begin();
	for (; iter != m_lMoveList.end(); ++iter)
	{
		(this->*((*iter)->m_MoveFunc))();
	}

	iter  = m_lMoveList.begin();
	MOVE_LIST::iterator eiter = m_lMoveList.end();
	for ( iter; iter != eiter; ++iter )
		SafeDelete(*iter);

	m_lMoveList.clear();
}

void CActorFollower::RegisterMoveAnimation(MoveFunc func)
{
	CMoveStateNode* moveNode = new CMoveStateNode(func);
	m_lMoveList.push_back(moveNode);
}

void CActorFollower::DoStop()
{
	DoActionState(GetStopActState());
}

void CActorFollower::DoMove()
{
	CDir dirCur;
	CFPos posCur, posDes;
	posCur = GetCharacter()->GetPixelPos();
	if (GetCharacter()->GetCurPixelWayPoint(posDes))
	{
		CVector2f vecDir;
		vecDir.x = posDes.x - posCur.x;
		vecDir.y = posDes.y - posCur.y;
		dirCur.Set(vecDir);
		SetMoveDir(dirCur.uDir);
	}
	else
	{
		SetMoveDir(GetActionDir());
	}
	DoActionState(GetMoveActState());
}

EActionState CActorFollower::GetStopActState()
{
	EActionState eActionState;
	EWeaponPosAndBattleState eWPBS = GetWeaponPosAndBattleState();
	switch (eWPBS)
	{
	case eWPBS_BattleState:
		eActionState = eAS_Idle_Battle;
		break;
	case eWPBS_HoldWeapon:
		eActionState = eAS_Idle_HoldWpn;
		break;
	case eWPBS_BackWeapon:
		eActionState = eAS_Idle_BackWpn;
		break;
	}
	return eActionState;
}

EActionState CActorFollower::GetMoveActState()
{
	EActionState eActionState;
	EMoveMode eMoveMode = GetCharacter()->GetMoveMode(GetWalkBackRadius());
	switch (eMoveMode)
	{
	case eMM_Run:
		{
			EWeaponPosAndBattleState eWPBS = GetWeaponPosAndBattleState();
			if (!GetCharacter()->CppGetCtrlState(eFCS_ForbitTurnAround))
				SetActionDir(GetMoveDir());

			if (!GetCharacter()->CppGetCtrlState(eFCS_InWalkState))
			{
				switch (eWPBS)
				{
				case eWPBS_BattleState:
					eActionState = eAS_Run_Battle;
					break;
				case eWPBS_HoldWeapon:
					eActionState = eAS_Run_HoldWpn;
					break;
				case eWPBS_BackWeapon:
					eActionState = eAS_Run_BackWpn;
					break;
				}
			}
			else
			{
				switch (eWPBS)
				{
				case eWPBS_BattleState:
					eActionState = eAS_Run_Battle;
					break;
				case eWPBS_HoldWeapon:
					eActionState = eAS_Walk_HoldWpn;
					break;
				case eWPBS_BackWeapon:
					eActionState = eAS_Walk_BackWpn;
					break;
				}
			}
		}
		break;
	case eMM_WalkBack:
		eActionState = eAS_Walk_Battle;
		break;
	}

	return eActionState;
}

//--------------------------------------------------------
//					ActionState相关
//--------------------------------------------------------
bool CActorFollower::DoFunActionState(EActionState eActionState, uint8 uActionIndex)
{
	return DoActionState(eActionState, -1, false, false, uActionIndex);
}

void CActorFollower::EndFunActionState()
{
	if(GetActionState() == eAS_Fun_Start || GetActionState() == eAS_Fun_Keep)
		DoActionState(eAS_Idle_BackWpn);
}

bool CActorFollower::DoActionState(EActionState eActionState, int16 nActionDir, bool bImmed, bool bForce, uint8 uActionIndex)
{
 //  	if (GetCharacter()->CastToCharacterDirector())
 //  	{
 //  		printf("^^^ [Wanna DoAni] EntityID:(%d) DoActionState:(%s) MoveState(%s)\n",
 //   		GetEntityID(), GetActionStateStr(eActionState), GetMoveStateStr(GetMoveState()));
	//}

	CRenderObject* RenderObj = GetCharacter()->GetRenderObject();
	Ast(RenderObj);

	if (eAS_HoldStill == eActionState)
	{
		GetCharacter()->GetFacingMgr()->RemoveFacingPair();
		RenderObj->HoldAniStill(1024);
		return true;
	}

	if (eAS_ResumeAni == eActionState)
	{
		RenderObj->ResumeAni(1024);
		return true;
	}

	bool bIsMoving = GetCharacter()->IsMoving();

	// 设置朝向
	if (!SetActionDirection(eActionState, nActionDir, bImmed, bForce, bIsMoving))
		return false;

	CActorCfgClient* pActorAniCfg = CActorCfgClient::GetActorCfgByActionState(eActionState);
	EActionPlayPriority ePriorityLevel = pActorAniCfg->GetPriorityLevel();

	// 执行动作规则
	if (!PlayActionRule(RenderObj, eActionState, bIsMoving, ePriorityLevel))
		return false;

	ExecuteSpecialRuleForAllBodyAni(eActionState);

	if (m_bIsAllBodyAni)
		DoMoveState(eMS_LockMove);

	EActionState eOldActionState = eActionState;
	eActionState = AdjustActionRhythm(eActionState);
	if (eOldActionState != eActionState)
		pActorAniCfg = CActorCfgClient::GetActorCfgByActionState(eActionState);

	EActionPlayMode eActorPlayMode = pActorAniCfg->GetActorPlayMode();
	if (eActionState == GetActionState() && bIsMoving && !bForce && eAPM_AllBodyLoop == eActorPlayMode)
	{
		RenderObj->SetRenderVelocity(m_fCurAniRhythm, -1);
		CCharacterFollower* pCharacter = GetCharacter();
		if (pCharacter->CppGetCtrlState(eFCS_InNormalHorse) || pCharacter->CppGetCtrlState(eFCS_InBattleHorse))
		{
			CRenderObject* pRenderHorse = GetCharacter()->GetRenderLinkMgr()->GetHorseRenderObj();
			if (pRenderHorse)
				pRenderHorse->SetRenderVelocity(m_fCurAniRhythm, -1);
		}
		return true;
	}

   	SetComboSing(false);
	SetActionIndex(uActionIndex);
	

	// 播放部位 全身 or 上半身
	bool bPlayAllBody = true;
	if (bIsMoving)
	{
		if (eAPM_UpBodyOnce == eActorPlayMode)
			bPlayAllBody = false;

		if (m_bIsAllBodyAni)
			bPlayAllBody = true;
	}

	// 播放方式 循环 or 一次
	bool bLoop = true;
	if (eAPM_AllBodyLoop != eActorPlayMode)
		bLoop = false;

	// 动作播放
	string strAniName;
	if (!GetCharacter()->GetIsPlayer() && !GetCharacter()->GetIsShadow())		// NPC
	{
		//printf("^^^ [NPC DoAni] EntityID:(%d) ActionState:(%s) AniRhythm(%.2f) PlayAllBody(%d) Loop(%d) IsMoving(%d) \n",
		//	GetEntityID(), GetActionStateStr(eActionState), m_fCurAniRhythm, bPlayAllBody, bLoop, bIsMoving);

		strAniName = pActorAniCfg->GetNPCAniName();
		bool bResult = RenderObj->DoAni( strAniName.c_str(), bLoop, m_fCurAniRhythm);
		RenderObj->Update();
		//通过ActionCommon表新添加的替换缺失动作列，来播放缺失动作
		if (!bResult)
		{
			eActionState = pActorAniCfg->GetNPCReplaceState();
			if (eAS_Error == eActionState)
			{
				SetNeedMoveUpBodyAni(false);
				SetCurOnceActionLevel(eAPP_NULL);
				return false;
			}
			pActorAniCfg = CActorCfgClient::GetActorCfgByActionState(eActionState);
			strAniName = pActorAniCfg->GetNPCAniName();
			eActorPlayMode = pActorAniCfg->GetActorPlayMode();
			if (eActorPlayMode == eAPM_AllBodyLoop)
			{
				if (bLoop == false)
					OnAnimationEnd("");
				bLoop = true;
			}
			bResult = RenderObj->DoAni( strAniName.c_str(), bLoop, m_fCurAniRhythm);
			RenderObj->Update();

			if (!bResult)
			{
				if (!bLoop)
					OnAnimationEnd(strAniName.c_str());
				return false;
			}
		}

	}
	else		// 玩家
	{
		if (m_nUpperBodyID == -1)
		{
			m_nUpperBodyID = RenderObj->GetSkeletalIdByName("Bip SpinePro");
			if(-1 == m_nUpperBodyID)
				m_nUpperBodyID = RenderObj->GetSkeletalIdByName("Bip01 Spine1");
			else
				m_nUpperBodyID = RenderObj->GetSkeletalIdByName("Bip01 Spine");
		}
		
		if (eAS_Fun_Start == eActionState || eAS_Fun_Keep == eActionState)
		{
			CFacialAniCfgMgr* facialCfg = CFacialAniCfgMgr::GetFicialAniCfgByIndex(GetActionIndex());
			strAniName = facialCfg->GetFacialAniName(eActionState);
		}
		else	
			strAniName = pActorAniCfg->GetPlayerAniName();

		if (m_bNeedMoveUpBodyAni && (eAS_Run_Battle >= eActionState && eAS_Idle_BackWpn <= eActionState))
		{
			RenderObj->DoDefaultAni(m_nUpperBodyID, 0);
			m_bNeedMoveUpBodyAni = false;
		}

		//if (GetCharacter()->CastToCharacterDirector())
		//	printf("^^^ [DoAni] EntityID:(%d) ActionState:(%s) ActionDir(%d) AniRhythm(%.2f) PlayAllBody(%d) Loop(%d) IsMoving(%d) \n",
		//	GetEntityID(), GetActionStateStr(eActionState), GetActionDir(), m_fCurAniRhythm, bPlayAllBody, bLoop, bIsMoving);
		bool bPlayerDoAni(false), bHorseDoAni(false);
		// 主角
		if (bPlayAllBody)
			bPlayerDoAni = RenderObj->DoAni( strAniName.c_str(), bLoop, m_fCurAniRhythm);
		else
			bPlayerDoAni = RenderObj->DoAni( strAniName.c_str(), m_nUpperBodyID, bLoop, m_fCurAniRhythm);

		// 坐骑
		bool bHorseDoAniResult = true;
		if (!(bIsMoving && eAPM_UpBodyOnce == eActorPlayMode))
		{
			bool bRiding = GetCharacter()->CppGetCtrlState(eFCS_InNormalHorse) || GetCharacter()->CppGetCtrlState(eFCS_InBattleHorse);
			if (bRiding)
			{
				CRenderObject* pRenderHorse = GetCharacter()->GetRenderLinkMgr()->GetHorseRenderObj();
				if (pRenderHorse)
				{
					bHorseDoAni = pRenderHorse->DoAni( strAniName.c_str(), bLoop, m_fCurAniRhythm);
				}
			}
		}

		if (!bPlayerDoAni)
		{
			SetNeedMoveUpBodyAni(false);
			SetCurOnceActionLevel(eAPP_NULL);
			if (GetMoveState() == eMS_LockMove)
			{
				if (bIsMoving)
					ChangeMoveState(eMS_Move, true);
				else
					ChangeMoveState(eMS_Stop, true);	
			}
			return false;
		}

		if (!bIsMoving)
		{
			if (eAPM_UpBodyOnce == eActorPlayMode)
				m_bNeedMoveUpBodyAni = true;

			if (m_bIsAllBodyAni || eActionState == eAS_Attack_Ride)
				m_bNeedMoveUpBodyAni = false;
		}
	}

	// 特效播放
	PlayFxByActionState(eActionState, strAniName);
	ChangeActionState(eActionState);
	return true;
}

void CActorFollower::DoActionSimplify(EActionState eActionState)
{
	CActorCfgClient* pActorAniCfg = CActorCfgClient::GetActorCfgByActionState(eActionState);
	string strAniName = pActorAniCfg->GetNPCAniName();
	bool bLoop = eAPM_AllBodyLoop == pActorAniCfg->GetActorPlayMode() ? true : false;
	GetCharacter()->GetRenderObject()->DoAni( strAniName.c_str(), bLoop, 1.0f);
}

void CActorFollower::ExecuteSpecialRuleForAllBodyAni(EActionState eActionState)
{
	// 规则1，特定职业暴击动作全身播放
	if (eAS_Strike == eActionState)
	{
		EClass eClass = GetCharacter()->CppGetClass();
		switch (eClass)
		{
		case eCL_Warrior:
		case eCL_MagicWarrior:
		case eCL_OrcWarrior:
			m_bIsAllBodyAni = true;
			return;
			break;
		default:
			break;
		}
	}

	// 规则2，顺发出手动作如填表标示需全身播放
	if (eAS_Cast == eActionState && m_bIsAllBodyAni)
		return;
	
	m_bIsAllBodyAni = false;
}

bool CActorFollower::SetActionDirection(EActionState eActionState, int16 nActionDir, bool bImmed, bool bForce, bool bIsMoving)
{
	uint8 uPreActionDir = GetActionDir();
	uint8 uActionDir = (uint8)nActionDir;

	if (!GetCharacter()->CppGetCtrlState(eFCS_ForbitTurnAround) || bForce)
	{
		if (nActionDir > -1)
		{
			if (!((eAS_Suffer_Back <= eActionState) && (eActionState <= eAS_Suffer_Right)  && bIsMoving) && uActionDir != GetActionDir())
			{
				SetActionDir(uActionDir);
				CDir dir;
				dir.uDir = GetActionDir();
				GetCharacter()->GetDirectionMgr()->SetDirection(uActionDir, bImmed);
			}
		}
		else
		{
			CDir dir;
			dir.uDir = GetActionDir();
			GetCharacter()->GetDirectionMgr()->SetDirection(GetActionDir(), bImmed);
		}
	}

	return true;
}

bool CActorFollower::PlayActionRule(CRenderObject* RenderObj, EActionState eActionState, bool bIsMoving, EActionPlayPriority ePriorityLevel)
{
	if ((GetMoveState() == eMS_LockMove || GetMoveState() == eMS_LockStop) && ePriorityLevel != eAPP_DeathAction)
	{
		if (!(GetMoveState() == eMS_LockMove && (ePriorityLevel == eAPP_SkillAction || ePriorityLevel == eAPP_CoerceAction || ePriorityLevel == eAPP_AttackAction)))
			return false;
	}

	switch (ePriorityLevel)
	{
	case eAPP_DeathAction:
		{
			GetCharacter()->GetFacingMgr()->RemoveFacingPair();
			m_bNeedMoveUpBodyAni = false;
			if (eActionState == eAS_Dead)
			{
				GetCharacter()->GetRenderLinkMgr()->DoWeaponAnimation(0);
				SetCurOnceActionLevel(eAPP_NULL);
			}
		}
		break;
	case eAPP_CoerceAction:
		{
			if (eAS_Whirl == eActionState)
				SetCurOnceActionLevel(eAPP_CoerceAction);
			else
				SetCurOnceActionLevel(eAPP_NULL);
			m_bNeedMoveUpBodyAni = false;
			DoMoveState(eMS_LockMove);

			if (GetCharacter()->GetIsPlayer() || GetCharacter()->GetIsShadow())
				GetCharacter()->GetRenderObject()->StopAni(m_nUpperBodyID, 0);
		}
		break;
	case eAPP_SkillAction:
		{
			if (eAPP_AttackAction == GetCurOnceActionLevel() || eAPP_SufferAction == GetCurOnceActionLevel())
			{
				if (GetMoveState() == eMS_LockMove)
					DoMoveState(GetPreMoveState(), true);

				m_bNeedMoveUpBodyAni = false;
				IModelHandler* pHandler = (IModelHandler*)RenderObj->GetModelHandler();
				if ( pHandler )
				{
					SetCurOnceActionLevel(eAPP_NULL);
					if (GetMoveState() == eMS_LockMove)
					{
						if (GetCharacter()->IsMoving())
							ChangeMoveState(eMS_Move, true);
						else
							ChangeMoveState(eMS_Stop, true);	
					}
				}
			}

			if (eActionState == eAS_Channel || eActionState == eAS_Sing)
			{
				m_bNeedMoveUpBodyAni = false;
				DoMoveState(eMS_LockMove);
			}

			SetCurOnceActionLevel(eAPP_SkillAction);
		}
		break;
	case eAPP_AttackAction:
		{
			if (eAPP_SkillAction == GetCurOnceActionLevel() || eAPP_CoerceAction == GetCurOnceActionLevel())
				return false;

			if (eAPP_SufferAction == GetCurOnceActionLevel() || eAS_Attack_Assist == GetActionState())
			{
				m_bNeedMoveUpBodyAni = false;
				IModelHandler* pHandler = (IModelHandler*)RenderObj->GetModelHandler();
				if ( pHandler )
				{
					SetCurOnceActionLevel(eAPP_NULL);
					if (GetMoveState() == eMS_LockMove)
					{
						if (GetCharacter()->IsMoving())
							ChangeMoveState(eMS_Move, true);
						else
							ChangeMoveState(eMS_Stop, true);	
					}
				}
			}

			if (eActionState == eAS_Attack_Ride)
			{
				if (eAPP_AttackAction == GetCurOnceActionLevel())
					return false;
				else
					DoMoveState(eMS_LockMove);
			}

			SetCurOnceActionLevel(eAPP_AttackAction);
			GetCharacter()->SetDirLockedTarget(true);
		}
		break;
	case eAPP_SufferAction:
		{
			if ((GetCharacter()->GetIsPlayer() || GetCharacter()->GetIsBoss()) && eAPP_AttackAction == GetCurOnceActionLevel())
				return false;

			if (eAPP_SkillAction == GetCurOnceActionLevel() || eAPP_SpecialAction == GetCurOnceActionLevel())
				return false;

			SetCurOnceActionLevel(eAPP_SufferAction);
		}
		break;
	case eAPP_MoveStopAction:
		{
			if (eAPP_AttackAction == GetCurOnceActionLevel() || eAPP_SkillAction == GetCurOnceActionLevel())
			{
				if (eAS_Idle_Battle != eActionState && eAS_Walk_Battle != eActionState && eAS_Run_Battle != eActionState)
					return false;

				if (eAS_Idle_Battle == eActionState)
				{
					bool bSucc = RenderObj->DoAniFromPart(m_nUpperBodyID);
					if (bSucc)
					{
						ChangeActionState(eAS_Idle_Battle);
						m_bNeedMoveUpBodyAni = true;
						return false;
					}
				}
			}

			if (bIsMoving)
			{
				if (eAS_Idle_BackWpn <= eActionState && eAS_Idle_Battle >= eActionState)
					return false;
			}
			else
			{
				if (eAS_Walk_BackWpn <= eActionState && eAS_Run_Battle >= eActionState)
					return false;
				
				if(eAS_Idle_Battle != eActionState)
					RenderObj->Update(); //更新头顶文字
			}

			//NPC StopMoving 后 Attack 客户端 Follower 移动延迟调用 OnMoveEnded 会切掉攻击动作
			if(!GetCharacter()->GetIsPlayer())
			{
				if (eAPP_AttackAction == GetCurOnceActionLevel() || eAPP_SkillAction == GetCurOnceActionLevel())
				{
					if (GetCharacter()->CppGetCtrlState(eFCS_InBattle) && (eActionState == eAS_Idle_Battle || eActionState == eAS_Run_Battle))
						return false;

					SetCurOnceActionLevel(eAPP_NULL);
				}
			}
		}
		break;
	case eAPP_SwitchAction:
		{
			SetCurOnceActionLevel(eAPP_SwitchAction);
		}
		break;
	case eAPP_SpecialAction:
		{
			if ((eAPP_AttackAction == GetCurOnceActionLevel() || eAPP_SkillAction == GetCurOnceActionLevel()))
				return false;
			m_bNeedMoveUpBodyAni = false;
			SetCurOnceActionLevel(eAPP_SpecialAction);
		}
		break;
	}

	return true;
}

EActionState CActorFollower::AdjustActionRhythm(EActionState eActState)
{
	CActorCfgClient* pActorAniCfg = CActorCfgClient::GetActorCfgByActionState(eActState);
	EActionPlayPriority ePriorityLevel = pActorAniCfg->GetPriorityLevel();
	CCharacterFollower* pCharacter = GetCharacter();
	CRenderObject* pRenderObject = GetCharacter()->GetRenderObject();

	if (eActState == eAS_Walk_BackWpn  || eActState == eAS_Walk_HoldWpn)
	{
		float fWalkGroundSpeed(1.5f);
		if( pCharacter->GetIsPlayer())
		{
			CRenderable * pRenderObj = pCharacter->GetRenderObject();
			if( pRenderObj && pRenderObj->GetType() == ROT_MODEL )
			{
				CAnimationGroup* pAni;
				pAni = class_cast<CModel*>( pRenderObj )->GetAnimateGroup();
				if( pAni )
				{
					CActorCfgClient* pActorAniCfg = CActorCfgClient::GetActorCfgByActionState(eActState);
					fWalkGroundSpeed = CMath::Abs(pAni->GetGroundSpeed(pAni->GetAnimationNum(pActorAniCfg->GetPlayerAniName().c_str())));
					if(fWalkGroundSpeed==0.0f)
						fWalkGroundSpeed = 1.5f;
				}
			}
		}
		else if (pCharacter->GetNpcType() != ENpcType_Shadow)
		{
			CRenderable * pRenderObj = pCharacter->GetRenderObject();
			if( pRenderObj && pRenderObj->GetType() == ROT_MODEL )
			{
				CAnimationGroup* pAni = NULL;
				pAni = class_cast<CModel*>( pRenderObj )->GetAnimateGroup();
				if( !pAni )
				{
					m_fCurAniRhythm = 1.0f;
					return eActState;
				}
				fWalkGroundSpeed = pAni->GetGroundSpeed(pAni->GetAnimationNum( "walk01" ));
				if(fWalkGroundSpeed==0.0f)
					fWalkGroundSpeed = 1.5f;
			}
		}


		float fWalkSpeed = pCharacter->GetCoreSpeed();
		float fWalkRate = fWalkSpeed / fWalkGroundSpeed;
		m_fCurAniRhythm = fWalkRate;
		if (pRenderObject && pRenderObject->GetScale() > 0)
			m_fCurAniRhythm = m_fCurAniRhythm / pRenderObject->GetScale();
	}
	else if (eActState == eAS_Run_BackWpn || eActState == eAS_Run_HoldWpn || eActState == eAS_Run_Battle || eActState == eAS_Walk_Battle)
	{
		float fRunGroundSpeed(3.0f);
		
		if( pCharacter->GetIsPlayer() )
		{
			CRenderable * pRenderObj = pCharacter->GetRenderObject();
			if( pRenderObj && pRenderObj->GetType() == ROT_MODEL )
			{
				CAnimationGroup* pAni;
				pAni = class_cast<CModel*>( pRenderObj )->GetAnimateGroup();
				if( pAni )
				{
					CActorCfgClient* pActorAniCfg = CActorCfgClient::GetActorCfgByActionState(eActState);
					fRunGroundSpeed = CMath::Abs(pAni->GetGroundSpeed(pAni->GetAnimationNum(pActorAniCfg->GetPlayerAniName().c_str())));
					if(fRunGroundSpeed==0.0f)
						fRunGroundSpeed = 3.6f;
				}
			}	
		}
		else if ( pCharacter->GetNpcType() != ENpcType_Shadow)
		{
			CRenderable * pRenderObj = pCharacter->GetRenderObject();
			if( pRenderObj && pRenderObj->GetType() == ROT_MODEL )
			{
				CAnimationGroup* pAni;
				pAni = class_cast<CModel*>( pRenderObj )->GetAnimateGroup();
				if( !pAni )
				{
					m_fCurAniRhythm = 1.0f;
					return eActState;
				}
				fRunGroundSpeed = pAni->GetGroundSpeed(pAni->GetAnimationNum("run01"));
				if(fRunGroundSpeed==0.0f)
					fRunGroundSpeed = 3.0f;
			}
		}
		float fRunSpeed = pCharacter->GetCoreSpeed();
		float fRunRate = fRunSpeed/fRunGroundSpeed;			
		m_fCurAniRhythm = fRunRate;

		if (m_fCurAniRhythm <= 0.5f && eAS_Walk_Battle != eActState)
		{
			if (eAS_Run_BackWpn == eActState)
			{
				if (!GetCharacter()->CppGetCtrlState(eFCS_InNormalHorse))
					eActState = eAS_Walk_BackWpn;
			}
			else
				eActState = eAS_Walk_HoldWpn;

			m_fCurAniRhythm *= 2;
		}
		if (pRenderObject && pRenderObject->GetScale() > 0)
			m_fCurAniRhythm = m_fCurAniRhythm / pRenderObject->GetScale();
	}
	else if (ePriorityLevel == eAPP_AttackAction)
		m_fCurAniRhythm = GetNAActionRhythm() * GetNATempRhythm();
	else
		m_fCurAniRhythm = 1.0f;

	return eActState;
}

//RenderObject Callback Function
void CActorFollower::OnSepAnimationEnd(const TCHAR* szName)
{
	//cout << "^^^ [OnSepAnimationEnd] AniName is " << szName << endl;
	if (eAS_Die == GetActionState())
		return;
	if (!GetNeedMoveUpBodyAni())
		SetCurOnceActionLevel(eAPP_NULL);
	if (GetCharacter()->CppGetCtrlState(eFCS_InBattle))
		GetCharacter()->SetDirLockedTarget(true);

	if (GetComboSing() && GetActionState() == eAS_Cast)
		DoActionState(eAS_Sing);
	if (GetCharacter()->IsMoving())
	{
		if (eAS_Channel != GetActionState())
		{
			GetCharacter()->ResumeMovingActionDir(GetWalkBackRadius());
			DoMove();
		}
		else
		{
			ChangeActionState(GetMoveActState());
		}
	}
	else
	{
		ChangeActionState(GetStopActState());
	}
}

void CActorFollower::OnAnimationEnd(const TCHAR* szName)
{
	//cout << "^^^ [OnAnimationEnd] AniName is " << szName << endl;	
	SetNeedMoveUpBodyAni(false);
	SetCurOnceActionLevel(eAPP_NULL);
	if (GetCharacter()->CppGetCtrlState(eFCS_InBattle))
		GetCharacter()->SetDirLockedTarget(true);

	switch(GetActionState())
	{
	case eAS_Birth:
		DoActionState(eAS_Idle_BackWpn);
		break;
	case eAS_Die:
		DoActionState(eAS_Dead);
		break;
	case eAS_Die_Special:
		DoActionState(eAS_Dead_Special);
		break;
	case eAS_Fun_Start:
		DoFunActionState(eAS_Fun_Keep, GetActionIndex());
	    break;
	case eAS_Respond_Enter:
		DoActionState(eAS_Respond_Keep);
		break;
	case eAS_MoveStart:
		GetCharacter()->GetFighter()->Notify(eCE_OnAniEnd);
		break;
	case eAS_SitDown:
		DoActionState(eAS_TakeFood);
		break;
	case eAS_Cast:
		{
			if (GetComboSing())
			{
				DoActionState(eAS_Sing);
				break;
			}
		}
	default:
		{
			if (GetCharacter()->IsMoving())
			{
				if (DoMoveState(eMS_Move, true))
					DoMoveAnimation();
			}
			else
			{
				if (DoMoveState(eMS_Stop, true))
					DoMoveAnimation();
			}
		}
	    break;
	}
}

void CActorFollower::OnResLoaded()
{
	GetCharacter()->GetCallbackHandler()->OnResLoaded(GetCharacter());
}

void CActorFollower::OnSetVisable(bool isVisable)
{
	ICharacterFollowerCallbackHandler* pHandler = GetCharacter()->GetCallbackHandler();
	if (pHandler)
	{
		if (isVisable)
			pHandler->OnIntoScreen(GetCharacter());
		else
			pHandler->OnLeaveScreen(GetCharacter());
	}
}

// -----------------------------------------------------------------------------------------------
// 动作特效
// -----------------------------------------------------------------------------------------------
void CActorFollower::PlayFxByActionState(EActionState eActionState, const string& strAniName)
{
	CActorCfgClient* pActorAniCfg = CActorCfgClient::GetActorCfgByActionState(eActionState);
	if (!pActorAniCfg->IsFxFollowAni())
		return;
		
	m_pActionFX->StopFX();

	switch (eActionState)
	{
	case eAS_Attack_All:
	case eAS_Attack_Right:
	case eAS_Attack_Left:
	case eAS_Attack_Assist:
	case eAS_Attack_Fist:
	case eAS_Strike:
		{
			m_strAttackFxName=strAniName;
			CRenderObject* pRenderObj= GetCharacter()->GetRenderObject();
			uint32 unitIndex = pRenderObj->AddEffect((m_strAttackFxFile+m_strAttackFxName+"/create.efx").c_str(), (m_strAttackFxName+"/create").c_str(),0,NULL);

			if (GetCharacter()->GetIsPlayer())
			{
				CWhiskSoundPlayer* pWhiskSoundPlayer = new CWhiskSoundPlayer(GetEntityID());
				pWhiskSoundPlayer->SetActionState(eActionState);
				pWhiskSoundPlayer->Reset();
				pRenderObj->SetEffectUnitHandler( unitIndex, pWhiskSoundPlayer, pWhiskSoundPlayer->GetUnitName() );
				SafeRelease(pWhiskSoundPlayer);

				CHitSoundPlayer* pHitSoundPlayer = new CHitSoundPlayer(GetEntityID());
				pHitSoundPlayer->SetActionState(eActionState);
				pHitSoundPlayer->Reset();
				pRenderObj->SetEffectUnitHandler( unitIndex, pHitSoundPlayer, pHitSoundPlayer->GetUnitName() );
				SafeRelease(pHitSoundPlayer);
			}
		}
		break;
	case eAS_Suffer_Back:
	case eAS_Suffer_Left:
	case eAS_Suffer_Right:
		CSoundPlayer::PlayYelpByType(GetCharacter(), eSTN_Suffer);
		break;
	case eAS_Die:
		{
			if (GetCharacter()->GetIsPlayer())
				CSoundPlayer::PlayYelpByType(GetCharacter(), eSTN_Die);
		}
	default:	
		string strFxPartPath = pActorAniCfg->GetFxPartPath()+"/";
		if (pActorAniCfg->GetActorPlayMode() != eAPM_AllBodyLoop)
		{
			CPlayerFX::PlayOnceFX(GetCharacter(), m_strActionFxFile+strFxPartPath+strAniName+"/create.efx", strAniName+"/create");
		}
		else
		{
			m_pActionFX->PlayLoopFX(m_strActionFxFile+strFxPartPath+strAniName+"/create.efx", strAniName+"/create");
		}
		break;
	}
}

//------------------------------------------------------------------------------
void	CActorFollower::OnKeyFrame( const TCHAR * szName )
{
	string strFrameName = szName;
	if ( strFrameName.find("f") != -1 )
	{
		string strCueNameTail = strFrameName.substr(1);
		CSoundPlayer::PlayFootstep(GetCharacter(), strCueNameTail);
	}
}

void CActorFollower::SplitAttackFxFile(string &FxFile, string &FxName)
{
	FxFile	= m_strAttackFxFile+m_strAttackFxName+"/explode.efx";
	FxName	= m_strAttackFxName+"/explode";
}

void CActorFollower::SplitSkillFxFile(const string &InFx, string &FxFile, string &FxName, bool bNeedSpliceName)
{
	uint32 uDeleyTime;
	SplitSkillFxFile(InFx,FxFile,FxName,uDeleyTime,bNeedSpliceName);
}

void CActorFollower::SplitSkillFxFile(const string &InFx, string &FxFile, string &FxName, uint32& uDeleyTime,bool bNeedSpliceName)
{
	if (InFx.empty())
	{
		return;
	}
	vector<string> sFXTable = CBaseFX::Split(InFx, ",");
	string strFXFile; 
	if (sFXTable[0].empty() && sFXTable.size() > 1)
	{
		strFXFile = m_strSkillFxFile;
	}
	else
	{
	
		if (bNeedSpliceName)
		{
			strFXFile = sFXTable[0]+m_strSpliceName;
		}
		else
		{
			strFXFile = sFXTable[0]; 
		}
	}

	if (sFXTable.size() > 2)
	{
		uDeleyTime = atoi(sFXTable[2].c_str());
	}

	int32 index = std::string::npos, last_search_position = 0;  

	if ((index=sFXTable[0].find(".efx", last_search_position)) != std::string::npos)
	{
		FxFile	=  sFXTable[0];
	}
	else
	{
		FxFile	= strFXFile+sFXTable[1]+".efx";
	}
	FxName	= sFXTable[1];
}
