#include "stdafx.h"
#include "CFighterVariantCallback.h"
#include "CSyncVariantClient.h"
#include "CCharacterFollower.h"
#include "CFighterFollower.h"
#include "ICharacterFollowerCallbackHandler.h"

int CFighterHealthPointVariantCallback::exec(CCypherVariant *pVariant)
{
	string strVariantName = pVariant->GetName();
	CCypherVariant* pRootVariant = pVariant->GetRoot();
	CCypherVariant* pAgileValueVariant = pRootVariant->GetMember("AgileValue");
	if ( strVariantName == "HealthPointAdder" )
	{
		CCypherVariant* pAdderVariant = pAgileValueVariant->GetMember("HealthPointAdder");
		float fAdderValue = (float)pAdderVariant->GetNumber<uint32>();
		m_pFighter->CppSetPropertyValue(ePID_HealthPoint*ePFT_Count+ePFT_Adder, fAdderValue);
	}
	else if ( strVariantName == "HealthPointMutiplier" )
	{
		CCypherVariant* pMutiplierVariant = pAgileValueVariant->GetMember("HealthPointMutiplier");
		float fMutiplierValue = pMutiplierVariant->GetNumber<float>();
		m_pFighter->CppSetPropertyValue(ePID_HealthPoint*ePFT_Count+ePFT_Multiplier, fMutiplierValue);
	}
	else if ( strVariantName == "HealthPointAgile" )
	{
		CCypherVariant* pAgileVariant = pAgileValueVariant->GetMember("HealthPointAgile");
		float fAgileValue = (float)pAgileVariant->GetNumber<uint32>();
		m_pFighter->CppSetPropertyValue(ePID_HealthPoint*ePFT_Count+ePFT_Agile, fAgileValue);
	}
	CCharacterFollower* pCharacter = m_pFighter->GetCharacter();
	pCharacter->UpdateHeadBloodRendler();
	if (m_pFighter->GetCallBackHandler() && pCharacter && pCharacter->GetAgileValueBeCare())
	{
		m_pFighter->GetCallBackHandler()->OnAgileValueChanged(m_pFighter->GetEntityID());
	}
	return 0;
}

int CFighterConsumePointVariantCallback::exec(CCypherVariant *pVariant)
{
	string strVariantName = pVariant->GetName();
	CCypherVariant* pRootVariant = pVariant->GetRoot();
	CCypherVariant* pAgileValueVariant = pRootVariant->GetMember("AgileValue");
	if ( strVariantName == "ConsumePointAdder" )
	{
		CCypherVariant* pAdderVariant = pAgileValueVariant->GetMember("ConsumePointAdder");
		float fAdderValue = (float)pAdderVariant->GetNumber<uint32>();
		if (m_pFighter->CppGetClass() == eCL_Warrior || m_pFighter->CppGetClass() == eCL_OrcWarrior)
		{
			m_pFighter->CppSetPropertyValue(ePID_RagePoint*ePFT_Count+ePFT_Adder, fAdderValue);
		}
		else if (m_pFighter->CppGetClass() == eCL_DwarfPaladin)
		{
			m_pFighter->CppSetPropertyValue(ePID_EnergyPoint*ePFT_Count+ePFT_Adder, fAdderValue);
		}
		else
		{
			m_pFighter->CppSetPropertyValue(ePID_ManaPoint*ePFT_Count+ePFT_Adder, fAdderValue);
		}
	}
	else if ( strVariantName == "ConsumePointMutiplier" )
	{
		CCypherVariant* pMutiplierVariant = pAgileValueVariant->GetMember("ConsumePointMutiplier");
		float fMutiplierValue = pMutiplierVariant->GetNumber<float>();
		if (m_pFighter->CppGetClass() == eCL_Warrior || m_pFighter->CppGetClass() == eCL_OrcWarrior)
		{
			m_pFighter->CppSetPropertyValue(ePID_RagePoint*ePFT_Count+ePFT_Multiplier, fMutiplierValue);
		}
		else if (m_pFighter->CppGetClass() == eCL_DwarfPaladin)
		{
			m_pFighter->CppSetPropertyValue(ePID_EnergyPoint*ePFT_Count+ePFT_Multiplier, fMutiplierValue);
		}
		else
		{
			m_pFighter->CppSetPropertyValue(ePID_ManaPoint*ePFT_Count+ePFT_Multiplier, fMutiplierValue);
		}
	}
	else if ( strVariantName == "ConsumePointAgile" )
	{
		CCypherVariant* pAgileVariant = pAgileValueVariant->GetMember("ConsumePointAgile");
		float fAgileValue = (float)pAgileVariant->GetNumber<uint32>();
		if (m_pFighter->CppGetClass() == eCL_Warrior || m_pFighter->CppGetClass() == eCL_OrcWarrior)
		{
			m_pFighter->CppSetPropertyValue(ePID_RagePoint*ePFT_Count+ePFT_Agile, fAgileValue);
		}
		else if (m_pFighter->CppGetClass() == eCL_DwarfPaladin)
		{
			m_pFighter->CppSetPropertyValue(ePID_EnergyPoint*ePFT_Count+ePFT_Agile, fAgileValue);
		}
		else
		{
			m_pFighter->CppSetPropertyValue(ePID_ManaPoint*ePFT_Count+ePFT_Agile, fAgileValue);
		}
	}
	CCharacterFollower* pCharacter = m_pFighter->GetCharacter();
	pCharacter->UpdateHeadBloodRendler();
	if (m_pFighter->GetCallBackHandler() && pCharacter && pCharacter->GetAgileValueBeCare())
	{
		m_pFighter->GetCallBackHandler()->OnAgileValueChanged(m_pFighter->GetEntityID());
	}
	return 0;
}

int CFighterCastingBeginVariantCallback::exec(CCypherVariant *pVariant)
{
	CCypherVariant* pRootVariant = pVariant->GetRoot();
	CCypherVariant* pCastingProcessVariant = pRootVariant->GetMember("CastingProcess");
	CCypherVariant* pCastIDVariant = pCastingProcessVariant->GetMember("CastID");
	CCypherVariant* pCastTimeVariant = pCastingProcessVariant->GetMember("CastTime");
	CCypherVariant* pGuessStartTimeVariant = pCastingProcessVariant->GetMember("GuessStartTime");
	CCypherVariant* pTargetIDVariant = pCastingProcessVariant->GetMember("TargetIDOfCast");

	if (pCastIDVariant && pCastTimeVariant && pGuessStartTimeVariant && pTargetIDVariant)
	{
		uint32 uCastID = pCastIDVariant->GetNumber<uint16>();
		if (uCastID == 0)
			return 0;
		float fCastTime = pCastTimeVariant->GetNumber<float>();
		int64 uGuessStartTime = pGuessStartTimeVariant->GetNumber<int64>();
		uint64 uFrameTime = m_pFighter->GetDistortedServerFrameTime();
		//int64 uExpiredTime = int64(fCastTime*1000) - uFrameTime + uGuessStartTime;
		int64 uExpiredTime = uFrameTime - uGuessStartTime;
		float fExpiredTime = (float)uExpiredTime/1000;
		if (fExpiredTime < 0.0f)
			fExpiredTime = 0.0f;
		uint32 uTargetID = pTargetIDVariant->GetNumber<uint32>();
		m_pFighter->OnCastBegin(m_pFighter->GetEntityID(), uCastID, uTargetID, fCastTime, fExpiredTime, false);
	}
	return 0;
}

int CFighterCastingEndVariantCallback::exec(CCypherVariant *pVariant)
{
	CCypherVariant* pRootVariant = pVariant->GetRoot();
	CCypherVariant* pCastingProcessVariant = pRootVariant->GetMember("CastingProcess");
	CCypherVariant* pCastIDVariant = pCastingProcessVariant->GetMember("CastID");
	CCypherVariant* pSkillIDVariant = pCastingProcessVariant->GetMember("uSkillID");
	CCypherVariant* pIsFinishVariant = pCastingProcessVariant->GetMember("bIsFinish");
	if (pCastIDVariant && pSkillIDVariant && pIsFinishVariant)
	{
		uint32 uCastID = pCastIDVariant->GetNumber<uint16>();
		uint32 uSkillID = pSkillIDVariant->GetNumber<uint16>();
		if (uCastID == 0 || uSkillID == 0)
			return 0;
		bool bIsFinish = pIsFinishVariant->GetNumber<bool>();
		//uint64 uFrameTime = m_pFighter->GetDistortedServerFrameTime();
		//cout << "CastingEnd " << uFrameTime << endl;
		m_pFighter->OnCastEnd(uSkillID, uCastID, bIsFinish);
	}
	return 0;
}

int CFighterSetStateVariantCallback::exec(CCypherVariant *pVariant)
{
	CCypherVariant* pStateVariant = pVariant->GetParent();
	CCypherVariant* pCategoryIdVariant = pStateVariant->GetMember("CategoryId");
	CCypherVariant* pDynamicIdVariant = pStateVariant->GetMember("DynamicId");
	CCypherVariant* pCountVariant = pStateVariant->GetMember("Count");
	CCypherVariant* pTimeVariant = pStateVariant->GetMember("Time");
	CCypherVariant* pGuessStartVariant = pStateVariant->GetMember("GuessStartTime");
	CCypherVariant* pSkillLevel = pStateVariant->GetMember("SkillLevel");
	CCypherVariant* pInstallerID = pStateVariant->GetMember("InstallerID");
	CCypherVariant* pCalcValue = pStateVariant->GetMember("CalcValue");

	//cout << "CFighterSetStateVariantCallback::exec(" << pCategoryIdVariant << ", " << pDynamicIdVariant
	//	<< ", " << pCountVariant << ", " << pTimeVariant << ", " << pGuessStartVariant << endl;
	if (pCategoryIdVariant && pDynamicIdVariant && pCountVariant && pTimeVariant && pGuessStartVariant && pInstallerID && pCalcValue)
	{
		uint32 uCategoryId = pCategoryIdVariant->GetNumber<uint16>();
		uint32 uDynamicId = pDynamicIdVariant->GetNumber<uint32>();
		uint32 uCount = pCountVariant->GetNumber<uint8>();
		int64 iTime = pTimeVariant->GetNumber<int64>();
		int64 iGuessStartTime = pGuessStartVariant->GetNumber<int64>();
		uint32 uSkillLevel = pSkillLevel->GetNumber<uint8>();
		uint32 uInstallerID = pInstallerID->GetNumber<uint32>();
		uint32 iCalcValue = pCalcValue->GetNumber<int32>();

		m_pFighter->OnSetState(uCategoryId, uDynamicId, uCount, iTime, iGuessStartTime, uSkillLevel,uInstallerID,iCalcValue);
	}
	return 0;
}

int CFighterDelStateVariantCallback::exec(CCypherVariant *pVariant)
{

	CCypherVariant* pStateVariant = pVariant->GetParent();
	CCypherVariant* pCategoryIdVariant = pStateVariant->GetMember("CategoryId");
	CCypherVariant* pDynamicIdVariant = pStateVariant->GetMember("DynamicId");

	//cout << "CFighterDelStateVariantCallback::exec(" << pCategoryIdVariant << ", " << pDynamicIdVariant << endl;
	if(!pVariant->GetNumber<bool>())
		return 0;

	if (pCategoryIdVariant && pDynamicIdVariant)
	{
		uint32 uCategoryId = pCategoryIdVariant->GetNumber<uint16>();
		uint32 uDynamicId = pDynamicIdVariant->GetNumber<uint32>();

		m_pFighter->OnDeleteState(uCategoryId, uDynamicId);

	}
	return 0;
}

int CFighterClearAllStateVariantCallback::exec(CCypherVariant *pVariant)
{
#ifdef SEND_STATE_BY_VAR
	//cout << "CFighterClearAllStateVariantCallback::exec() m_pFighter = " << m_pFighter << endl;
	m_pFighter->OnClearAllState();
#endif

	return 0;
}

int CFighterCtrlValueVariantCallback::exec(CCypherVariant *pVariant)
{
	CCypherVariant* pCtrlValueVariant = pVariant->GetParent();
	CCypherVariant* pValueVariant = pCtrlValueVariant->GetMember("Value");
	CCypherVariant* pValueTypeVariant = pCtrlValueVariant->GetMember("ValueTypeID");
	CCypherVariant* pSetVariant = pCtrlValueVariant->GetMember("Set");
	CCypherVariant* pIsAliveVariant = pCtrlValueVariant->GetMember("IsAlive");
	if (pValueVariant && pValueTypeVariant && pSetVariant && pIsAliveVariant)
	{
		uint32 uValue = pValueVariant->GetNumber<uint32>();
		uint32 uValueTypeID = pValueTypeVariant->GetNumber<uint32>();
		bool bSet = pSetVariant->GetNumber<bool>();
		bool bIsAlive = pIsAliveVariant->GetNumber<bool>();
		m_pFighter->OnCtrlVariantChanged(uValue, uValueTypeID, bSet, bIsAlive);
	}
	return 0;
}
