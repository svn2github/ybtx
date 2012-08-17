#include "stdafx.h"
#include "CAllStateMgrClient.h"
#include "CFighterFollower.h"
#include "FXPlayer.h"
#include "ICharacterFollowerCallbackHandler.h"
#include "CCharacterFollower.h"
#include "CRenderObject.h"
#include "CFightCallbackData.h"

CBaseStateClient::CBaseStateClient(uint32 uDynamicId, int32 iTime, int32 iRemainTime, uint32 uSkillLevel,
								   uint32 uInstallerID, CAllStateMgrClient* pAllStateMgr)
: m_uDynamicId(uDynamicId),
m_iTime(iTime),
m_iRemainTime(iRemainTime),
m_uSkillLevel(uSkillLevel),
m_uInstallerID(uInstallerID),
m_pAllStateMgr(pAllStateMgr)
{
	m_uSetupOrder = ++pAllStateMgr->m_uMaxSetupOrder;
	if(iTime < -1)
	{
		stringstream str;
		//str << "状态客户端的构造函数的总时间参数不能为" << iRemainTime << endl;
		str << iRemainTime;
		GenErr("状态客户端的构造函数的总时间参数不能为小于-1的值", str.str());
	}
	if(iRemainTime < -1)
	{
		stringstream str;
		//str << "状态客户端的构造函数的剩余时间参数不能为" << iRemainTime << endl;
		str << iRemainTime;
		GenErr("状态客户端的构造函数的总时间参数不能为小于-1的值", str.str());
		//m_iRemainTime = 0;
	}
	//if(iRemainTime >= 0)
	//{
	//	CAppClient::Inst()->RegisterTick(this, 1000);
	//}
	//m_uUpdateTime = GetProcessTime();
}

CBaseStateClient::~CBaseStateClient()
{
	m_pAllStateMgr->m_mapSetupOrder.erase(m_uSetupOrder);
	//if(m_iRemainTime >= 0) CAppClient::Inst()->UnRegisterTick(this);
}

void CBaseStateClient::UpdateTime(int32 iTime)
{
	m_iTime = iTime;
}

void CBaseStateClient::UpdateRemainTime(int32 iRemainTime)
{
	if(iRemainTime >= 0)				//有限时间更新
	{
		//CAppClient::Inst()->UnRegisterTick(this);
		//if(iRemainTime >= 0) CAppClient::Inst()->RegisterTick(this, 1000);
		m_iRemainTime = iRemainTime;
		m_uUpdateTime = GetOwner()->GetDistortedProcessTime();
	}
	else if(iRemainTime == -1)		//无限时间更新
	{
		//if(m_iRemainTime >= 0) CAppClient::Inst()->UnRegisterTick(this);
		m_iRemainTime = iRemainTime;
		m_uUpdateTime = GetOwner()->GetDistortedProcessTime();
	}
}

float CBaseStateClient::GetRemainTime()
{
	if(m_iRemainTime < 0)
		return float(m_iRemainTime);
	
	uint64 uProcessTime = GetOwner()->GetDistortedProcessTime();
	int64 iDurationTime = int64(uProcessTime - m_uUpdateTime);
	if (iDurationTime < 0)
	{
		//stringstream str;
		//str << GetCfg()->GetName() << "：" << uProcessTime << " - " << m_uUpdateTime << " = " << iDurationTime;
		//GenErr("状态客户端的GetRemainTime()计算经过的时间不能为负数", str.str());
		iDurationTime = 0;
	}
	float fReturnTime = float(m_iRemainTime) - float(iDurationTime) / 1000.0f;
	if(fReturnTime < 0.0f)
	{
		return 0.0f;
	}
	return fReturnTime;
}

void CAllStateMgrClient::OnStateCategoryBegin(uint32 uCategoryId, uint32 uDynamicId, uint32 uCascade)
{
	uint32 uEntityID = m_pFighter->GetEntityID();
	//cout << "OnStateCategoryBegin(" << uEntityID << ", " << uCascade << ")\n";
	CCharacterFollower* pCharacter = m_pFighter->GetCharacter();	//CCharacterFollower::GetCharacterByID(uEntityID);
	if (!pCharacter)
	{
		//cout << "CCharacterFollower[" << uEntityID << "]不存在\n";
		return;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalId(uCategoryId);
	//string sStateName = pCfg->GetName();

	//模型
	if(!pCfg->GetModelStr().empty())
	{
		//改变模型部分
		if(m_pFighter->GetCallBackHandler()) 
			m_pFighter->GetCallBackHandler()->OnStateBegin(uEntityID, uCategoryId, uDynamicId);

		m_pFighter->GetCharacter()->DoNowAni();
	}

	//缩放
	float fScale = pCfg->GetScale();
	if(fScale != 1.0f)
	{
		//m_dScale *= fScale;
		//m_dScale *= pow(fScale, int32(uCascade));
		double dScale = pCharacter->GetRenderObject()->GetFinalScale() * pow(fScale, int32(uCascade));
		m_dScale = dScale;
		pCharacter->GetRenderObject()->SetFinalScale(float(dScale), m_pFighter->GetDistortedProcessTime() + pCfg->GetScaleTime());
		//cout << "模型缩放为" << fScale << "^" << uCascade << "倍，相当于最初的" << dScale << "倍\n";
	}

	//特效
	string strFXFile,strFXName;
	uint32 uDelayTime = 0;
	pCharacter->GetActor()->SplitSkillFxFile(pCfg->GetFX(uCascade),strFXFile,strFXName,uDelayTime,false);

	if (!strFXFile.empty())
	{
		//目前特效是以名字（Category）为单位，重复名字的分散状态特效不分散，否则还要改OnStateBegin的调用位置
		if(m_mapFxPlayer.find(uCategoryId) == m_mapFxPlayer.end())
		{
			CPlayerFX* pFxPlayer = new CPlayerFX(pCharacter);
			m_mapFxPlayer.insert(make_pair(uCategoryId, pFxPlayer));
			pFxPlayer->PlayLoopFX(strFXFile, strFXName,uDelayTime);
		}
	}

}

void CAllStateMgrClient::OnStateCascadeChange(uint32 uCategoryId, uint32 uDynamicId, uint32 uCascade, uint32 uOldCascade, bool bFromAdd)
{
	uint32 uEntityID = m_pFighter->GetEntityID();
	//cout << "OnState" << (bFromAdd ? "EntityAdd" : "CascadeChange") << "(" << uEntityID << "," << uCascade << "," << uOldCascade << ")\n";
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalId(uCategoryId);
	CCharacterFollower* pCharacter = m_pFighter->GetCharacter();	//CCharacterFollower::GetCharacterByID(uEntityID);
	if (!pCharacter)
	{
		//cout << "CCharacterFollower[" << uEntityID << "]不存在\n";
		return;
	}


	//缩放
	float fScale = pCfg->GetScale();
	if(fScale != 1.0f)
	{
		//m_dScale *= fScale;
		int32 uCascadeSub = int32(uCascade) - int32(uOldCascade);
		if(uCascadeSub != 0)
		{
			//m_dScale *= pow(fScale, uCascadeSub);
			double dScale = pCharacter->GetRenderObject()->GetFinalScale() * pow(fScale, uCascadeSub);
			m_dScale = dScale;
			pCharacter->GetRenderObject()->SetFinalScale(float(dScale), m_pFighter->GetDistortedProcessTime() + pCfg->GetScaleTime());
			//cout << "模型缩放为" << fScale << "^" << uCascadeSub << "倍，相当于最初的" << dScale << "倍\n";
		}
		else
		{
			double dScale = pCharacter->GetRenderObject()->GetFinalScale();
			if(dScale > 0.0f)
			{
				double dScaleChangeRate = m_dScale / dScale;
				if(dScaleChangeRate > 1.0002 || dScaleChangeRate < 0.9998)
				{
					pCharacter->GetRenderObject()->SetScale(float(m_dScale));
					//cout << "模型现在大小 == " << pCharacter->GetRenderObject()->GetFinalScale() <<"\n";
				}
			}
		}
	}

	string strFXFile,strFXName;
	uint32 uDelayTime = 0;
	pCharacter->GetActor()->SplitSkillFxFile(pCfg->GetFX(uCascade),strFXFile,strFXName,uDelayTime,false);
	//特效
	if(pCfg->IsMultiCascadeFX()&&!strFXFile.empty())
	{

		//目前特效是以名字（Category）为单位，重复名字的分散状态特效不分散，否则还要改OnStateBegin的调用位置
		MapFxPlayer::iterator itr = m_mapFxPlayer.find(uCategoryId);
		if(itr != m_mapFxPlayer.end())
		{
			itr->second->CancelFx();
			m_mapFxPlayer.erase(itr);
		}


		//目前特效是以名字（Category）为单位，重复名字的分散状态特效不分散，否则还要改OnStateEnd的调用位置
		if(m_mapFxPlayer.find(uCategoryId) == m_mapFxPlayer.end())
		{
			CPlayerFX* pFxPlayer = new CPlayerFX(pCharacter);
			m_mapFxPlayer.insert(make_pair(uCategoryId, pFxPlayer));
			pFxPlayer->PlayLoopFX(strFXFile, strFXName,uDelayTime);
		}

	}
}


void CAllStateMgrClient::OnStateCategoryEnd(uint32 uCategoryId, uint32 uDynamicId, uint32 uOldCascade)
{
	uint32 uEntityID = m_pFighter->GetEntityID();
	//cout << "OnStateCategoryEnd(" << uEntityID << ", " << uOldCascade << ")\n";
	CCharacterFollower* pTargetFolObj = m_pFighter->GetCharacter();		//CCharacterFollower::GetCharacterByID(uEntityID);
	if (!pTargetFolObj)
	{
		//cout << "CCharacterFollower[" << uEntityID << "]不存在\n";
		return;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalId(uCategoryId);
	//string sStateName = pCfg->GetName();


	//特效
	//vector<string> sFXTable = CFxPlayer::Split(pCfg->GetFX(uCascade), ",");
	//string sFXFile	= sFXTable[0].c_str();
	//string sFXName	= sFXTable[1].c_str();
	//目前特效是以名字（Category）为单位，重复名字的分散状态特效不分散，否则还要改OnStateEnd的调用位置
	MapFxPlayer::iterator itr = m_mapFxPlayer.find(uCategoryId);
	if(itr != m_mapFxPlayer.end())
	{
		itr->second->CancelFx();
		m_mapFxPlayer.erase(itr);
	}

	//缩放
	float fScale = pCfg->GetScale();
	if(fScale != 1.0f)
	{
		//m_dScale /= fScale;
		//m_dScale *= pow(fScale, -int32(uOldCascade));
		double dScale = pTargetFolObj->GetRenderObject()->GetFinalScale() * pow(fScale, -int32(uOldCascade));
		m_dScale = dScale;
		pTargetFolObj->GetRenderObject()->SetFinalScale(float(dScale), m_pFighter->GetDistortedProcessTime() + pCfg->GetScaleTime());
		//cout << "模型缩放为" << fScale << "^" << (-int32(uOldCascade)) << "倍，相当于最初的" << dScale << "倍\n";
	}

	//模型
	if(!pCfg->GetModelStr().empty())
	{
		//改变模型部分
		if(m_pFighter->GetCallBackHandler())
			m_pFighter->GetCallBackHandler()->OnStateEnd(uEntityID, uCategoryId, uDynamicId);

		m_pFighter->GetCharacter()->DoNowAni();
	}
}

void CAllStateMgrClient::OnStateEntityAdd(uint32 uCategoryId, uint32 uDynamicId, uint32 uCascade)
{
	OnStateCascadeChange(uCategoryId, uDynamicId, uCascade, 0);
}

void CAllStateMgrClient::OnStateEntityDelete(uint32 uCategoryId, uint32 uDynamicId, uint32 uOldCascade, uint32 uLeftDynamicId, uint32 uLeftCascade)
{
	uint32 uEntityID = m_pFighter->GetEntityID();
	//cout << "OnStateEntityDelete(" << uEntityID << ", " << uOldCascade << ")\n";
	CCharacterFollower* pCharacter = m_pFighter->GetCharacter();		//CCharacterFollower::GetCharacterByID(uEntityID);
	if (!pCharacter)
	{
		//cout << "CCharacterFollower[" << uEntityID << "]不存在\n";
		return;
	}

	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalId(uCategoryId);
	//string sStateName = pCfg->GetName();

	string strFXFile,strFXName;
	uint32 uDelayTime = 0;
	pCharacter->GetActor()->SplitSkillFxFile(pCfg->GetFX(uLeftCascade),strFXFile,strFXName,uDelayTime,false);
	//特效
	if(pCfg->IsMultiCascadeFX()&&!strFXFile.empty())
	{
		//目前特效是以名字（Category）为单位，重复名字的分散状态特效不分散，否则还要改OnStateBegin的调用位置
		MapFxPlayer::iterator itr = m_mapFxPlayer.find(uCategoryId);
		if(itr != m_mapFxPlayer.end())
		{
			itr->second->CancelFx();
			m_mapFxPlayer.erase(itr);
		}


		//目前特效是以名字（Category）为单位，重复名字的分散状态特效不分散，否则还要改OnStateEnd的调用位置
		if(m_mapFxPlayer.find(uCategoryId) == m_mapFxPlayer.end())
		{
			CPlayerFX* pFxPlayer = new CPlayerFX(pCharacter);
			m_mapFxPlayer.insert(make_pair(uCategoryId, pFxPlayer));
			pFxPlayer->PlayLoopFX(strFXFile, strFXName,uDelayTime);
		}

	}

	//缩放
	float fScale = pCfg->GetScale();
	if(fScale != 1.0f)
	{
		//m_dScale /= fScale;
		//m_dScale *= pow(fScale, -int32(uOldCascade));
		double dScale = pCharacter->GetRenderObject()->GetFinalScale() * pow(fScale, -int32(uOldCascade));
		m_dScale = dScale;
		pCharacter->GetRenderObject()->SetFinalScale(float(dScale), m_pFighter->GetDistortedProcessTime() + pCfg->GetScaleTime());
		//cout << "模型缩放为" << fScale << "^" << (-int32(uOldCascade)) << "倍，相当于最初的" << dScale << "倍\n";
	}
}

CAllStateMgrClient::CAllStateMgrClient(CFighterFollower* pFighter)
: m_pFighter(pFighter)
, m_dScale(1.0f)
, m_uMaxSetupOrder(0)
{
	m_mapFxPlayer.clear();
	m_mapSetupOrder.clear();
}

CAllStateMgrClient::~CAllStateMgrClient()
{
	for(MapFxPlayer::iterator itr = m_mapFxPlayer.begin(); itr != m_mapFxPlayer.end(); itr++)
	{
		(itr->second)->CancelFx();
	}
	m_mapFxPlayer.clear();
	m_mapSetupOrder.clear();
}

void CAllStateMgrClient::ReplayFx()
{
	for(MapFxPlayer::iterator itr = m_mapFxPlayer.begin(); itr != m_mapFxPlayer.end(); itr++)
	{
		itr->second->ReplayFX();
	}
}

void CAllStateMgrClient::OnBuffUpdate(uint32 uDynamicId, const TCHAR* szName, uint32 uCount, int32 iTime, float fRemainTime, bool bDecrease, uint32 uSkillLevel, float fDotInterval, uint32 uInstallerID, int32 iCalcValue)
{
	if (m_pFighter->GetCharacter()->GetAgileValueBeCare() && m_pFighter->GetCallBackHandler())
	//if (m_pFighter->GetCallBackHandler())
	{
		CBuffData pBuffData;
		pBuffData.SetObjID(m_pFighter->GetEntityID());
		pBuffData.SetDynamicId(uDynamicId);
		pBuffData.SetName(szName);
		pBuffData.SetCount(uCount);
		pBuffData.SetTime(iTime);
		pBuffData.SetRemainTime(fRemainTime);
		pBuffData.SetDecrease(bDecrease);
		pBuffData.SetSkillLevel(uSkillLevel);
		pBuffData.SetDotInterval(fDotInterval);
		pBuffData.SetInstallerID(uInstallerID);
		pBuffData.SetCalcValue(iCalcValue);
		m_pFighter->GetCallBackHandler()->OnBuffIconUpdate(&pBuffData);
	}
}

void CAllStateMgrClient::SetTargetBuff()
{
	//添加所有状态的特效和图标
	for(MapSetupOrder::iterator itr = m_mapSetupOrder.begin(); itr != m_mapSetupOrder.end(); itr++)
	{
		CBaseStateCfgClient* pCfg = class_cast<CBaseStateCfgClient*>(itr->second->GetCfg());
		m_pFighter->GetAllStateMgr()->OnBuffUpdate(
			itr->second->GetDynamicID(), pCfg->GetName(), itr->second->GetCount(), 
			itr->second->GetTime() ,itr->second->GetRemainTime(), 
			pCfg->GetDecrease(), itr->second->GetSkillLevel(), pCfg->GetDotInterval(), itr->second->GetInstallerID());
	}
}
