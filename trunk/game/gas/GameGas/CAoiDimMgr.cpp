#include "stdafx.h"
#include "CAoiDimMgr.h"
#include <deque>
#include "CCoreSceneServer.h"
#include "CCoreObjectServer.h"
#include "CCoreObjectDictator.h"
#include "CCoreObjectMediator.h"
#include "CoreCommon.h"
#include "BaseHelper.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"
#include "CDimensionInfo.h"
#include "CoreObjectCommon.h"
#include "CGameConfigServer.h"

using namespace std;



//这个define存在表示全部对象的AOI属性都设在第0层，否则分层设
//#define SET_ALL_IN_SAME_DIM

#ifdef SET_ALL_IN_SAME_DIM
	#define AOI_DIM_COUNT 1
#else
	#define AOI_DIM_COUNT eADT_Count
#endif

VecAoiDimFilter CAoiDimFilter::m_VecEyeSightFilter;
VecAoiDimFilter CAoiDimFilter::m_VecSizeFilter;
CDimensionInfo* CAoiDimFilter::m_DimensionInfo = NULL;
bool CAoiDimFilter::__init = CAoiDimFilter::BuildAoiDimFilter();
uint32 CAoiDimFilter::m_uNpcSleepDim = 100;

void CAoiDimFilter::Unload()
{
	ClearVector(m_VecEyeSightFilter);
	ClearVector(m_VecSizeFilter);

	if (m_DimensionInfo)
	{
		delete m_DimensionInfo;
		m_DimensionInfo = NULL;
	}
}

CAoiDimFilter::CAoiDimFilter()
{
	m_bDim.assign(eADT_Count, false);
}

bool CAoiDimFilter::BuildAoiDimFilter()
{
	//建立各维EyeSight规则
	m_VecEyeSightFilter.assign(eAIET_Count, NULL);
	for(uint32 i = 0; i < eAIET_Count; ++i)
	{
		CreateEyeSightFilter(EAoiItemEyeSightType(i));
	}

	//建立各维Size规则
	m_VecSizeFilter.assign(eAIST_Count, NULL);
	for(uint32 i = 0; i < eAIST_Count; ++i)
	{
		CreateSizeFilter(EAoiItemSizeType(i));
	}

	return true;
}

void CAoiDimFilter::CreateEyeSightFilter(EAoiItemEyeSightType eType)
{
	CAoiDimFilter* pFilter = new CAoiDimFilter();
	switch(eType)
	{
	case eAIET_Player:
		pFilter->Set(eADT_Player);
		break;
	case eAIET_Magic:
		pFilter->Set(eADT_Magic);
		break;
	case eAIET_RefMagic:
		pFilter->Set(eADT_RefMagic);
		break;
	case eAIET_Servant:
		pFilter->Set(eADT_Servant);
		break;
	case eAIET_Trap:
		pFilter->Set(eADT_Trap);
		break;
	case eAIET_TrapSeePlayer:
		pFilter->Set(eADT_TrapSeePlayer);
		break;
	case eAIET_TrapSeeNpc:
		pFilter->Set(eADT_TrapSeeNpc);
		break;
	case eAIET_InteractObj:
		pFilter->Set(eADT_InteractObj);
	case eAIET_VisiPlayerObj:
		pFilter->Set(eADT_VisiPlayerObj);
		break;
	case eAIET_DoubleEyeSightVisiPlayerObj:
		pFilter->Set(eADT_VisiPlayerObj);
		pFilter->Set(eADT_VisiPlayerObjInterested);
		break;
	case eAIET_InvisiPlayerObj:
		pFilter->Set(eADT_InvisiPlayerObj);
		break;
	case eAIET_DoubleEyeSightInvisiPlayerObj:
		pFilter->Set(eADT_InvisiPlayerObj);
		pFilter->Set(eADT_InvisiPlayerObjInterested);
		break;
	case eAIET_InterestedObj:
		pFilter->Set(eADT_InvisiPlayerObjInterested);
		break;
	case eAIET_BlindObj:
		break;
	default:
		delete pFilter;
		{
			stringstream sExp;
			sExp << "不存在的EyeSight类型[" << eType << "]\n";
			GenErr(sExp.str());
			return;
		}
	}

	m_VecEyeSightFilter[eType] = pFilter;
}



void CAoiDimFilter::CreateSizeFilter(EAoiItemSizeType eType)
{
	CAoiDimFilter* pFilter = new CAoiDimFilter();
	switch(eType)
	{
	case eAIST_PlayerOrServant:					//玩家、召唤兽
		pFilter->Set(eADT_Player);
		pFilter->Set(eADT_Magic);
		pFilter->Set(eADT_Servant);
		pFilter->Set(eADT_Trap);
		pFilter->Set(eADT_TrapSeeNpc);
		pFilter->Set(eADT_VisiPlayerObj);
		pFilter->Set(eADT_TrapSeePlayer);
		break;
	case eAIST_Magic:							//魔法
		pFilter->Set(eADT_Player);
		pFilter->Set(eADT_RefMagic);			//为反弹魔法而设的
		break;
	case eAIST_TaskNPC:							//普通任务NPC
		pFilter->Set(eADT_Player);
		pFilter->Set(eADT_Trap);
		pFilter->Set(eADT_TrapSeeNpc);
		break;
	case eAIST_IntObj:							//普通IntObj
		pFilter->Set(eADT_Player);
		break;
	case eAIST_Monster:							//普通怪（只能被玩家、召唤兽、魔法看见）
		pFilter->Set(eADT_Player);
		pFilter->Set(eADT_Magic);
		pFilter->Set(eADT_Servant);
		pFilter->Set(eADT_Trap);
		pFilter->Set(eADT_TrapSeeNpc);
		break;
	case eAIST_InteractObj:						//交互NPC/IntObj
		pFilter->Set(eADT_Player);
		pFilter->Set(eADT_Magic);
		pFilter->Set(eADT_Servant);
		pFilter->Set(eADT_Trap);
		pFilter->Set(eADT_InteractObj);
		pFilter->Set(eADT_TrapSeeNpc);
		break;


	case eAIST_InterestingMonster:				//被个性关注的普通怪
		pFilter->Set(eADT_Player);
		pFilter->Set(eADT_Magic);
		pFilter->Set(eADT_Servant);
		pFilter->Set(eADT_Trap);
		pFilter->Set(eADT_VisiPlayerObjInterested);			//这个在兴趣需求充分后还要重新考虑
		pFilter->Set(eADT_InvisiPlayerObjInterested);		//这个在兴趣需求充分后还要重新考虑
		pFilter->Set(eADT_TrapSeeNpc);
		break;
	case eAIST_InterestingNPC:					//被个性关注的任务NPC（比如走近会被打招呼的任务NPC）
		pFilter->Set(eADT_Player);
		pFilter->Set(eADT_Trap);
		pFilter->Set(eADT_VisiPlayerObjInterested);			//这个在兴趣需求充分后还要重新考虑
		pFilter->Set(eADT_InvisiPlayerObjInterested);		//这个在兴趣需求充分后还要重新考虑
		pFilter->Set(eADT_TrapSeeNpc);
		break;
	case eAIST_InterestingIntObj:				//被个性关注的IntObj（比如嗜肉个性里的肉）
		pFilter->Set(eADT_Player);
		pFilter->Set(eADT_VisiPlayerObjInterested);			//这个在兴趣需求充分后还要重新考虑
		pFilter->Set(eADT_InvisiPlayerObjInterested);		//这个在兴趣需求充分后还要重新考虑
		break;
	case eAIST_DeadPlayer:						//死亡后的玩家
		pFilter->Set(eADT_Player);
		break;
	case eAIST_SleepNPC:
		break;
	default:
		delete pFilter;
		{
			stringstream sExp;
			sExp << "不存在的Size类型[" << eType << "]\n";
			GenErr(sExp.str());
			return;
		}
	}

	m_VecSizeFilter[eType] = pFilter;
}


CDimensionInfo* CAoiDimFilter::DimensionInfo()
{
	if(!m_DimensionInfo)
	{
		m_DimensionInfo = new CDimensionInfo;

		m_DimensionInfo->SetDimensionInfo(0, g_uPlayerAoiPrecision*eGridSpanForObj);

		for(uint32 i = 1; i < AOI_DIM_COUNT; ++i)
		{
			if(i == eADT_Magic || i == eADT_RefMagic || i == eADT_Trap)
			{
				m_DimensionInfo->SetDimensionInfo(i, eGridSpanForObj/4);
				continue;
			}
			m_DimensionInfo->SetDimensionInfo(i, eGridSpanForObj);
		}
		m_DimensionInfo->SetDimensionInfo(CAoiDimFilter::m_uNpcSleepDim, 5*eGridSpanForObj);
	}

	return m_DimensionInfo;
}

bool CAoiDimFilter::NeedStealthDim(uint32 eDim)
{
	switch(eDim)
	{
	case eADT_Player:
	case eADT_Servant:
	case eADT_InteractObj:
	case eADT_VisiPlayerObj:
	case eADT_VisiPlayerObjInterested:
		return true;
	default:
		return false;
	}
}















CAoiDimMgr::CAoiDimMgr()
:m_eEyeSightType(eAIET_Undefined),
m_eSizeType(eAIST_Undefined),
m_eObjectAoiType(EAoi_Null),
m_fEyeSight(0.0f),
m_fInterestedEyeSight(0.0f),
m_fSize(0.0f),
m_fStealth(0.0f),
m_fKeenness(0.0f),
m_uViewGroup(0)
{
	m_bEyeSightFlush.assign(AOI_DIM_COUNT, false);
	m_bSizeFlush.assign(AOI_DIM_COUNT, false);
}

void CAoiDimMgr::SetEyeSight(float fEyeSight,CCoreObjectServer* pCoreObj)
{
	if(m_fEyeSight != fEyeSight)
	{
		m_fEyeSight = fEyeSight;
		FlushEyeSight(pCoreObj);
	}
}

void CAoiDimMgr::SetSecondEyeSight(float fInterestedEyeSight,CCoreObjectServer* pCoreObj)
{
#ifndef SET_ALL_IN_SAME_DIM
	if(m_fInterestedEyeSight != fInterestedEyeSight)
	{
		m_fInterestedEyeSight = fInterestedEyeSight;
		FlushEyeSight(pCoreObj);
	}
#endif
}

void CAoiDimMgr::SetDoubleEyeSight(float fEyeSight, float fInterestedEyeSight, CCoreObjectServer* pCoreObj)
{
	if(m_fEyeSight != fEyeSight || m_fInterestedEyeSight != fInterestedEyeSight )
	{
		m_fEyeSight = fEyeSight;
#ifndef SET_ALL_IN_SAME_DIM
		m_fInterestedEyeSight = fInterestedEyeSight;
#endif
		FlushEyeSight(pCoreObj);
	}
}

void CAoiDimMgr::SetSize(float fSize, CCoreObjectServer* pCoreObj)
{
	if(m_fSize != fSize)
	{
		m_fSize = fSize;
		FlushSize(pCoreObj);
	}
}

void CAoiDimMgr::SetStealth(float fStealth, CCoreObjectServer* pCoreObj)
{
	if(m_fStealth != fStealth)
	{
		m_fStealth = fStealth;
		FlushStealth(pCoreObj);
	}
}

void CAoiDimMgr::SetKeenness(float fKeenness, CCoreObjectServer* pCoreObj)
{
	if(m_fKeenness != fKeenness)
	{
		m_fKeenness = fKeenness;
		FlushKeenness(pCoreObj);
	}
}

void CAoiDimMgr::SetViewGroup(uint32 uViewGroup, CCoreObjectServer* pCoreObj)
{
	if(m_uViewGroup != uViewGroup)
	{
		m_uViewGroup = uViewGroup;
		FlushViewGroup(pCoreObj);
	}
}










void CAoiDimMgr::FlushEyeSight(CCoreObjectServer* pCoreObj)
{
	if(m_eEyeSightType != eAIET_Undefined)
	{
		if (pCoreObj->CastToObjMediator())	//玩家第0层单独设置
			pCoreObj->CastToObjMediator()->SetSyncEyeSize(m_fEyeSight);	

		for(uint32 i = 1; i < AOI_DIM_COUNT; ++i)		
		{
			if(CAoiDimFilter::GetEyeSightFilter(m_eEyeSightType, i))
			{
				if(!CAoiDimFilter::IsInterestedDim(i))
				{
					pCoreObj->SetViewEyeSight(m_fEyeSight, i);
				}
				else
				{
					pCoreObj->SetViewEyeSight(m_fInterestedEyeSight, i);
				}
				m_bEyeSightFlush[i] = true;
			}
			else
			{
				if(m_bEyeSightFlush[i])
					pCoreObj->SetViewEyeSight(0.0f, i);
			}
		}
	}
}

void CAoiDimMgr::FlushSize(CCoreObjectServer* pCoreObj)
{
	if(m_eSizeType != eAIST_Undefined)
	{
		pCoreObj->CastToObjDictator()->SetSyncSize(m_fSize);
		for(uint32 i = 1; i < AOI_DIM_COUNT; ++i)
		{
			if(CAoiDimFilter::GetSizeFilter(m_eSizeType, i))
			{
				pCoreObj->SetViewSize(m_fSize, i);
				m_bSizeFlush[i] = true;
			}
			else
			{
				if(m_bSizeFlush[i])
					pCoreObj->SetViewSize(0.0f, i);
			}
		}
	}
}

void CAoiDimMgr::FlushStealth(CCoreObjectServer* pCoreObj)
{
	if((m_eEyeSightType != eAIET_Undefined || m_eSizeType != eAIST_Undefined) &&
		(m_fEyeSight > 0.0f || m_fSize > 0.0f || m_fInterestedEyeSight > 0.0f))
	{
		pCoreObj->CastToObjDictator()->SetSyncStealth(m_fStealth); // 第0层单独设置
		for(uint32 i = 1; i < AOI_DIM_COUNT; ++i)
		{
			if(ExistInDim(EAoiDimType(i)))
			{
				if(CAoiDimFilter::NeedStealthDim(i))
				{
					pCoreObj->SetViewStealth(m_fStealth, i);
					//cout << "SetStealth(" << m_fStealth << ", " << i << ")\n";
				}
				else
				{
					pCoreObj->SetViewStealth(0.0f, i);
					//cout << "SetStealth(0.0f, " << i << ")\n";
				}
			}
		}
	}
}

void CAoiDimMgr::FlushKeenness(CCoreObjectServer* pCoreObj)
{
	if((m_eEyeSightType != eAIET_Undefined || m_eSizeType != eAIST_Undefined) &&
		(m_fEyeSight > 0.0f || m_fSize > 0.0f || m_fInterestedEyeSight > 0.0f))
	{
		if (pCoreObj->CastToObjMediator())
			pCoreObj->CastToObjMediator()->SetSyncKeenness(m_fKeenness);	//第0层单独设置

		for(uint32 i = 1; i < AOI_DIM_COUNT; ++i)
		{
			if(ExistInDim(EAoiDimType(i)))
			{
				if(CAoiDimFilter::NeedStealthDim(i))
				{
					pCoreObj->SetViewKeenness(m_fKeenness, i);
					//cout << "SetKeenness(" << m_fKeenness << ", " << i << ")\n";
				}
				else
				{
					pCoreObj->SetViewKeenness(0.0f, i);
					//cout << "SetKeenness(0.0f, " << i << ")\n";
				}
			}
		}
	}
}

void CAoiDimMgr::FlushViewGroup(CCoreObjectServer* pCoreObj)
{
	if((m_eEyeSightType != eAIET_Undefined || m_eSizeType != eAIST_Undefined) &&
		(m_fEyeSight > 0.0f || m_fSize > 0.0f || m_fInterestedEyeSight > 0.0f))
	{
		pCoreObj->CastToObjDictator()->SetSyncGroup(m_uViewGroup);
		for(uint32 i = 1; i < AOI_DIM_COUNT; ++i)
		{
			if(ExistInDim(EAoiDimType(i)))
			{
				if(CAoiDimFilter::NeedStealthDim(i))
				{
					pCoreObj->SetViewGroup(m_uViewGroup, i);
					//cout << "SetViewGroup(" << m_uViewGroup << ", " << i << ")\n";
				}
				else
				{
					pCoreObj->SetViewGroup(0, i);
					//cout << "SetViewGroup(0, " << i << ")\n";
				}
			}
		}
	}
}

void CAoiDimMgr::FlushOthers(CCoreObjectServer* pCoreObj)
{
	if((m_eEyeSightType != eAIET_Undefined || m_eSizeType != eAIST_Undefined) &&
		(m_fEyeSight > 0.0f || m_fSize > 0.0f || m_fInterestedEyeSight > 0.0f))
	{
		CCoreObjectMediator* pMedObj = pCoreObj->CastToObjMediator();
		if ( pMedObj )
			pMedObj->SetSyncKeenness(m_fKeenness);

		CCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
		pDicObj->SetSyncStealth(m_fStealth);
		pDicObj->CastToObjDictator()->SetSyncGroup(m_uViewGroup);

		for(uint32 i = 1; i < AOI_DIM_COUNT; ++i)
		{
			if(ExistInDim(EAoiDimType(i)))
			{
				if(CAoiDimFilter::NeedStealthDim(i))
				{
					pCoreObj->SetViewStealth(m_fStealth, i);
					pCoreObj->SetViewKeenness(m_fKeenness, i);
					pCoreObj->SetViewGroup(m_uViewGroup, i);
					//cout << "SetStealth(" << m_fStealth << ", " << i << ")\n";
					//cout << "SetKeenness(" << m_fKeenness << ", " << i << ")\n";
					//cout << "SetViewGroup(" << m_uViewGroup << ", " << i << ")\n";
				}
				else
				{
					pCoreObj->SetViewStealth(0.0f, i);
					pCoreObj->SetViewKeenness(0.0f, i);
					pCoreObj->SetViewGroup(0, i);
					//cout << "SetStealth(0.0f, " << i << ")\n";
					//cout << "SetKeenness(0.0f, " << i << ")\n";
					//cout << "SetViewGroup(0, " << i << ")\n";
				}
			}
		}
	}
}









void CAoiDimMgr::SetEyeSightType(EAoiItemEyeSightType eType, CCoreObjectServer* pCoreObj)
{
	if(m_eEyeSightType != eType)
	{
#ifdef SET_ALL_IN_SAME_DIM
		m_eEyeSightType = eAIET_Player;
#else
		m_eEyeSightType = eType;
#endif
		FlushEyeSight(pCoreObj);
		FlushOthers(pCoreObj);
	}
}

void CAoiDimMgr::SetSizeType(EAoiItemSizeType eType, CCoreObjectServer* pCoreObj)
{
	if(m_eSizeType != eType)
	{
#ifdef SET_ALL_IN_SAME_DIM
		m_eSizeType = eAIST_PlayerOrServant;
#else
		m_eSizeType = eType;
#endif
		FlushSize(pCoreObj);
		FlushOthers(pCoreObj);
	}
}

void CAoiDimMgr::SetAoiType(EAoiItemEyeSightType eEyeSightType, EAoiItemSizeType eSizeType, CCoreObjectServer* pCoreObj)
{
	bool bEyeSightSet = false, bSizeSet = false;
	if(m_eEyeSightType != eEyeSightType)
	{
#ifdef SET_ALL_IN_SAME_DIM
		m_eEyeSightType = eAIET_Player;
#else
		m_eEyeSightType = eEyeSightType;
#endif
		FlushEyeSight(pCoreObj);
		bEyeSightSet = true;
	}
	if(m_eSizeType != eSizeType)
	{
#ifdef SET_ALL_IN_SAME_DIM
		m_eSizeType = eAIST_PlayerOrServant;
#else
		m_eSizeType = eSizeType;
#endif
		FlushSize(pCoreObj);
		bSizeSet = true;
	}
	if(bEyeSightSet || bSizeSet)
	{
		FlushOthers(pCoreObj);
	}
}

bool CAoiDimMgr::ExistInDim(EAoiDimType eAoiDim)
{
	Ast(m_eEyeSightType != eAIET_Undefined && m_eSizeType != eAIST_Undefined);
	if(CAoiDimFilter::GetSizeFilter(m_eSizeType, eAoiDim) && m_fSize > 0.0f)
		return true;
	else if(CAoiDimFilter::GetEyeSightFilter(m_eEyeSightType, eAoiDim))
	{
		if(CAoiDimFilter::IsInterestedDim(eAoiDim))
		{
			if(m_fInterestedEyeSight > 0.0f)
				return true;
		}
		else
		{
			if(m_fEyeSight > 0.0f)
				return true;
		}
	}
	return false;
}

void CAoiDimMgr::InitAoiType(EAoiItemEyeSightType eEyeSightType, EAoiItemSizeType eSizeType)
{
#ifdef SET_ALL_IN_SAME_DIM
	m_eEyeSightType = eAIET_Player;
	m_eSizeType = eAIST_PlayerOrServant;
#else
	m_eEyeSightType = eEyeSightType;
	m_eSizeType = eSizeType;
#endif
}

void CAoiDimMgr::InitPlaySizeForNpcSleepDim(CCoreObjectServer* pCoreObj)
{
	Ast (pCoreObj);
	pCoreObj->SetViewEyeSight(0, CAoiDimFilter::m_uNpcSleepDim);
	pCoreObj->SetViewSize(CGameConfigServer::Inst()->GetPlayerSleepDimSize(), CAoiDimFilter::m_uNpcSleepDim);
	pCoreObj->SetViewStealth(0, CAoiDimFilter::m_uNpcSleepDim);
	pCoreObj->SetViewKeenness(0, CAoiDimFilter::m_uNpcSleepDim);
}

void CAoiDimMgr::InitNpcEyeSizeForNpcSleepDim(bool bSleep,CCoreObjectServer* pCoreObj)
{
	Ast (pCoreObj);
	if (bSleep)
	{
		pCoreObj->SetViewEyeSight(CGameConfigServer::Inst()->GetNpcSleepEyeSigh(), CAoiDimFilter::m_uNpcSleepDim);
		pCoreObj->SetViewSize(0, CAoiDimFilter::m_uNpcSleepDim);
		pCoreObj->SetViewStealth(0, CAoiDimFilter::m_uNpcSleepDim);
		pCoreObj->SetViewKeenness(0, CAoiDimFilter::m_uNpcSleepDim);
	}
}

void CAoiDimMgr::UniteObjectAoiType(EObjectAoiType eObjectAoiType, CCoreObjectServer* pCoreObj)
{
	if(m_eObjectAoiType <= EAoi_PassiveNpc && EAoi_NatureNpc == eObjectAoiType)
		SetObjectAoiType(EObjectAoiType(EAoi_NatureNpc | m_eObjectAoiType), pCoreObj);
}

void CAoiDimMgr::SetObjectAoiType(EObjectAoiType eObjectAoiType, CCoreObjectServer* pCoreObj)
{
	m_eObjectAoiType = eObjectAoiType;
	switch(eObjectAoiType)
	{
	case EAoi_TaskNpc:
		SetAoiType(eAIET_BlindObj, eAIST_TaskNPC, pCoreObj);
		break;
	case EAoi_IntObjSeeNone:
		SetAoiType(eAIET_BlindObj, eAIST_IntObj, pCoreObj);
		break;
	case EAoi_ActiveNpc:
		SetAoiType(eAIET_VisiPlayerObj, eAIST_Monster, pCoreObj);
		break;
	case EAoi_PassiveNpc:
		SetAoiType(eAIET_BlindObj, eAIST_Monster, pCoreObj);
		break;
	case EAoi_NatureNpc:
		SetAoiType(eAIET_InterestedObj, eAIST_TaskNPC, pCoreObj);
		break;
	case EAoi_NatureActiveNpc:
		SetAoiType(eAIET_DoubleEyeSightVisiPlayerObj, eAIST_Monster, pCoreObj);
		break;
	case EAoi_NaturePassiveNpc:
		SetAoiType(eAIET_InterestedObj, eAIST_Monster, pCoreObj);		//暂时先用eAIET_InterestedObj，若需要看其它NPC则用eAIET_DoubleEyeSightInvisiPlayerObj
		break;
	case EAoi_ActiveServant:
		SetAoiType(eAIET_Servant, eAIST_PlayerOrServant, pCoreObj);
		break;
	case EAoi_PassiveServant:
		SetAoiType(eAIET_BlindObj, eAIST_PlayerOrServant, pCoreObj);
		break;
	case EAoi_BattleHorse:
		SetAoiType(eAIET_BlindObj, eAIST_PlayerOrServant, pCoreObj);
		break;
	case EAoi_IntObjSeeAll:
		SetAoiType(eAIET_VisiPlayerObj, eAIST_IntObj, pCoreObj);
		break;
	case EAoi_TrapSeeAll:
		SetAoiType(eAIET_Trap, eAIST_IntObj, pCoreObj);
		break;
	case EAoi_TrapSeePlayer:
		SetAoiType(eAIET_TrapSeePlayer, eAIST_IntObj, pCoreObj);
		break;
	case EAoi_TrapSeeNpc:
		SetAoiType(eAIET_TrapSeeNpc, eAIST_IntObj, pCoreObj);
		break;
	case EAoi_SleepNPC:
		SetAoiType(eAIET_BlindObj, eAIST_SleepNPC, pCoreObj);
	default:
		break;
	}
}

#undef AOI_DIM_COUNT
#ifdef SET_ALL_IN_SAME_DIM
	#undef SET_ALL_IN_SAME_DIM
#endif





