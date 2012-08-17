#pragma once
#include "LoadSkillCommon.h"
#include "CTplStateCfgClient.h"
#include "CTxtTableFile.h"
#include "FightDef.h"
#include "CCfgColChecker.inl"
#include "CCfgCalc.inl"
#include "BaseHelper.h"
#include "StringHelper.h"

template <class StateType>
typename CTplStateCfgClient<StateType>::MapTplStateCfgById CTplStateCfgClient<StateType>::m_mapCfgById;

template <class StateType>
typename CTplStateCfgClient<StateType>::MapTplStateCfgByName CTplStateCfgClient<StateType>::m_mapCfgByName;



template <class StateType>
const TCHAR* CTplStateCfgClient<StateType>::GetFX()
{
	return m_calcFX->GetTestString().c_str();
}

template <class StateType>
const bool CTplStateCfgClient<StateType>::IsMultiCascadeFX()
{
	return m_calcFX->GetStringCount() > 1;
}

template <class StateType>
const TCHAR* CTplStateCfgClient<StateType>::GetFX(uint32 i)
{
	if(i == 0)
	{
		GenErr("CTplStateCfgClient<StateType>::GetFX(uCascade)层数参数为0\n");
		return "";
	}
	if(i >= m_calcFX->GetStringCount())
		i = m_calcFX->GetStringCount();
	return m_calcFX->GetString(i - 1).c_str();
}




template <class StateType>
bool CTplStateCfgClient<StateType>::LoadConfig(const TCHAR* cfgFile)
{
	using namespace CfgChk;
	
	CTxtTableFile TabFile;
	CTplStateCfgClient<StateType>*	pCfgNode;

	if (!TabFile.Load(L"cfg", cfgFile)) return false;
	SetTabFile(TabFile, "状态");

	ClearMap(m_mapCfgById);
	m_mapCfgByName.clear();

	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		SetLineNo(i);
		pCfgNode = new CTplStateCfgClient<StateType>;
		pCfgNode->m_eGlobalType = GetStateType();
		pCfgNode->m_uId = GetStateTypeFloor() + i;
		pCfgNode->m_sName = TabFile.GetString(i, szGlobalState_Name);
		trimend(pCfgNode->m_sName);
		ReadItem(pCfgNode->m_eDecreaseType,		 szTplState_DecreateType,				CANEMPTY,	eDST_Increase,	ms_mapDecreaseType);
		pCfgNode->m_bDecrease = pCfgNode->m_eDecreaseType <= eDST_Increase ? false : true;
		pCfgNode->m_sIcon = TabFile.GetString(i, szTplState_IconID);
		trimend(pCfgNode->m_sIcon);
		pCfgNode->m_sModel = TabFile.GetString(i, szTplState_Model);
		trimend(pCfgNode->m_sModel);
		//pCfgNode->m_sFX = TabFile.GetString(i, szTplState_FXID);
		ReadMixedItem(pCfgNode->m_calcFX,		szTplState_FXID,		CANEMPTY,	"");
		pCfgNode->m_calcFX->SetStringSplitter(';');
		pCfgNode->m_calcFX->SetTypeMultiString();
		
		//ReadItem(pCfgNode->m_fScale,			szTplState_Scale,				CANEMPTY,	1.0f,			GT,		0.0f);
		CCfgCalc* pCalc = NULL;
		ReadMixedItem(pCalc,			szTplState_Scale,				CANEMPTY,	"");
		if(pCalc->GetTestString().empty())
		{
			pCfgNode->m_fScale = 1.0f;
			pCfgNode->m_fScaleTime = 0.0f;
		}
		else
		{
			int32 iValueCount = pCalc->GetValueCount();
			pCfgNode->m_fScale = (float)pCalc->GetDblValue(0);
			if(iValueCount > 1)
			{
				pCfgNode->m_fScaleTime = (float)pCalc->GetDblValue(1);
			}
			else
			{
				pCfgNode->m_fScaleTime = 0.0f;
			}
		}
		delete pCalc;

		ReadItem(pCfgNode->m_calcTime,			szMagicState_Time,				GE,			-1);
		//ReadItem(pCfgNode->m_bCancelable, szTplState_Cancelable,		CANEMPTY,	YES);
		ReadItem(pCfgNode->m_eIconCancelCond,	szBaseState_CancelCond,			CANEMPTY,	eICC_None,		ms_mapIconCancelCond);

		pCfgNode->m_fDotInterval = 1.0f;
		if (pCfgNode->m_eGlobalType == eSGT_MagicState)
		{
			string tempstring;
			ReadItem(tempstring,						szMagicState_DotMagicEff,		CANEMPTY);
			if(!tempstring.empty())
			{
				size_t comma = tempstring.find(',');
				//SetItemTitle(szMagicState_DotMagicEff);
				if(comma != string::npos)
				{
					SetValue(pCfgNode->m_fDotInterval, tempstring.substr(comma + 1));
				}
			}
		}

		SharedPtrType* pCfgNodeSharedPtr = new SharedPtrType(pCfgNode);
		m_mapCfgById.insert(make_pair(pCfgNode->m_uId, pCfgNodeSharedPtr));
		m_mapCfgByName.insert(make_pair(pCfgNode->m_sName, pCfgNodeSharedPtr));
	}
	return true;
}

template <class StateType>
void CTplStateCfgClient<StateType>::UnloadConfig()
{
	ClearMap(m_mapCfgById);
	m_mapCfgByName.clear();
}

template <class StateType>
CTplStateCfgClient<StateType>::~CTplStateCfgClient()
{
	SafeDelete(m_calcFX);
	SafeDelete(m_calcTime);
}

template <class StateType>
typename CTplStateCfgClient<StateType>::SharedPtrType& CTplStateCfgClient<StateType>::GetById(uint32 uId)
{
	stringstream ExpStr;
	CTplStateCfgClient<StateType>::MapTplStateCfgById::iterator mapCfgItr = m_mapCfgById.find(uId);
	if (mapCfgItr == m_mapCfgById.end())
	{
		ExpStr << "魔法状态" << uId << "不存在";
		GenErr(ExpStr.str());
		//return NULL;
	}
	return *(mapCfgItr->second);
}

template <class StateType>
typename CTplStateCfgClient<StateType>::SharedPtrType& CTplStateCfgClient<StateType>::GetByName(const TCHAR* name)
{
	stringstream ExpStr;
	CTplStateCfgClient<StateType>::MapTplStateCfgByName::iterator mapCfgItr = m_mapCfgByName.find(name);
	if (mapCfgItr == m_mapCfgByName.end())
	{
		ExpStr << "魔法状态" << name << "不存在";
		GenErr(ExpStr.str());
		//return NULL;
	}
	return *(mapCfgItr->second);
}

