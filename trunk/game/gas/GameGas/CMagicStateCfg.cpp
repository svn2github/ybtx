#include "stdafx.h"
#include "CMagicStateCfg.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.inl"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "TStringRef.inl"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}


//初始化静态成员
CMagicStateCfgServer::MapMagicStateCfg CMagicStateCfgServer::m_mapCfg;
CMagicStateCfgServer::MapMagicStateCfgById CMagicStateCfgServer::m_mapCfgById;
CMagicStateCfgServer::MapCascadeType CMagicStateCfgServer::m_mapCascadeType;
CMagicStateCfgServer::MapReplaceRuler CMagicStateCfgServer::m_mapReplaceRuler;
CMagicStateCfgServerSharedPtr CMagicStateCfgServer::ms_NULL;

CMagicStateCfgServer::CMagicStateCfgServer()
: CBaseStateCfgServer(eSGT_MagicState)
{
	m_pCancelableMagicEff = new MagicEffServerStringRefer(this);
	m_pDotMagicEff = new MagicEffServerStringRefer(this);
	m_pFinalMagicEff = new MagicEffServerStringRefer(this);
}

CMagicStateCfgServer::CMagicStateCfgServer(const CMagicStateCfgServer& cfg)
: CBaseStateCfgServer(cfg)
,m_eReplaceType(cfg.m_eReplaceType)
,m_eCascadeType(cfg.m_eCascadeType)
,m_bCascadeGradation(cfg.m_bCascadeGradation)
,m_calcCascadeMax(new CCfgCalc(*(cfg.m_calcCascadeMax)))
,m_fDotInterval(cfg.m_fDotInterval)
{
	string str;
	m_pCancelableMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pCancelableMagicEff->GetStringKey(str);
	m_pCancelableMagicEff->SetStringKey(str);
	m_pDotMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pDotMagicEff->GetStringKey(str);
	m_pDotMagicEff->SetStringKey(str);
	m_pFinalMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pFinalMagicEff->GetStringKey(str);
	m_pFinalMagicEff->SetStringKey(str);
}

CMagicStateCfgServer::~CMagicStateCfgServer()
{
	SafeDelete(m_calcCascadeMax);
	SafeDelete(m_calcTime);
	//SafeDelete(m_pFMAssociateDotM);
	SafeDelete(m_pCancelableMagicEff);
	SafeDelete(m_pDotMagicEff);
	SafeDelete(m_pFinalMagicEff);
}

bool CMagicStateCfgServer::LoadConfig(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	InitMapCascadeType();
	CreateMap(m_mapReplaceRuler, 2, ("否"), ("是"));

	CTxtTableFile TabFile;
	CMagicStateCfgServer*	pCfgNode;

	SetTabFile(TabFile, "魔法状态");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	UnloadConfig();

	for(int32 i=1; i<TabFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		pCfgNode = new CMagicStateCfgServer;
		pCfgNode->m_eGlobalType = eSGT_MagicState;
		pCfgNode->m_uId = eSIC_MagicState + i;

		ReadItem(pCfgNode->m_sName,					szMagicState_Name);
		ReadItem(pCfgNode->m_eDecreaseType,			szTplState_DecreateType,		ms_mapDecreaseType);
		ReadItem(pCfgNode->m_bDispellable,			szTplState_Dispellable,			CANEMPTY,	YES);
		ReadItem(pCfgNode->m_eIconCancelCond,		szBaseState_CancelCond,			CANEMPTY,	eICC_None,		ms_mapIconCancelCond);
		ReadItem(pCfgNode->m_eReplaceType,			szMagicState_ReplaceRuler,		CANEMPTY,	eRR_KeepCurState,	m_mapReplaceRuler);
		ReadItem(pCfgNode->m_eCascadeType,			szMagicState_CascadeType,		m_mapCascadeType);
		ReadItem(pCfgNode->m_bCascadeGradation,		szMagicState_CascadeGradation,	CANEMPTY,	YES);
		ReadItem(pCfgNode->m_calcCascadeMax,		szMagicState_CascadeMax,		GE,			-1);
		ReadItem(pCfgNode->m_calcTime,				szMagicState_Time,				GE,			-1);
		ReadItem(pCfgNode, *(pCfgNode->m_pCancelableMagicEff),	szMagicState_CancelableMagicEff,CANEMPTY);


		pCfgNode->m_fDotInterval = 1.0f;
		string tempstring;
		ReadItem(tempstring,						szMagicState_DotMagicEff,		CANEMPTY);
		if(tempstring.empty())
		{
			//pCfgNode->m_pDotMagicEff = NULL;
		}
		else
		{
			size_t comma = tempstring.find(',');
			SetItemTitle(szMagicState_DotMagicEff);
			if(comma != string::npos)
			{
				pCfgNode->m_pDotMagicEff->SetStringKey(tempstring.substr(0, comma));
				SetValue(pCfgNode->m_fDotInterval, tempstring.substr(comma + 1));
			}
			else
			{
				pCfgNode->m_pDotMagicEff->SetStringKey(tempstring);
			}
		}

		ReadItem(pCfgNode, *(pCfgNode->m_pFinalMagicEff),		szMagicState_FinalMagicEff,	CANEMPTY);
		ReadItem(pCfgNode->m_sModel,				szTplState_Model,			CANEMPTY);
		//ReadItem(pCfgNode->m_fScale,				szTplState_Scale,			CANEMPTY,		1.0f,		GT,		0.0f);

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
			if(iValueCount > 2)
			{
				GenExpInfo("表达式个数大于2", pCalc->GetTestString());
				pCfgNode->m_fScaleTime = (float)pCalc->GetDblValue(1);
			}
			else if(iValueCount > 1)
			{
				pCfgNode->m_fScaleTime = (float)pCalc->GetDblValue(1);
			}
			else
			{
				pCfgNode->m_fScaleTime = 0.0f;
			}
		}
		//ReadItem(pCfgNode->m_pFMAssociateDotM,		szMagicState_FMAssociateDotM,CANEMPTY);

		pCfgNode->m_bDecrease = pCfgNode->m_eDecreaseType <= eDST_IncreaseEnd ? false : true;

		if (pCfgNode->m_calcTime->IsSingleNumber())
		{
			pCfgNode->SetNeedSaveToDB(pCfgNode->m_calcTime->GetDblValue());
		}

		//pCfgNode->m_bPersistent = false;

		ReadItem(pCfgNode->m_bPersistent,			szState_Persistent,				CANEMPTY,		NO);

		CCfgCalc* calcFX;
		ReadMixedItem(calcFX,		szTplState_FXID,			CANEMPTY,	"");
		calcFX->SetStringSplitter(';');
		if(!pCfgNode->m_sModel.empty() || !pCfgNode->m_sModel.empty() || !pCalc->GetTestString().empty()
			|| !calcFX->GetTestString().empty())
		{
			pCfgNode->SetNeedSync();
		}

		CMagicStateCfgServerSharedPtr* pCfgNodeSharedPtr = new CMagicStateCfgServerSharedPtr(pCfgNode);
		InsertStringSharedPtrMap(m_mapCfg, pCfgNodeSharedPtr);
		InsertUint32SharedPtrMap(m_mapCfgById, pCfgNodeSharedPtr);
		delete pCalc;
		delete calcFX;
	}

	return true;
}

bool CMagicStateCfgServer::InitMapCascadeType()					//建立群体叠加类型字符串到对应枚举类型的映射
{
	m_mapCascadeType["集中"] = eCT_Central;
	m_mapCascadeType["分散"] = eCT_Decentral;
	m_mapCascadeType["唯一"] = eCT_Unique;
	m_mapCascadeType["共享"] = eCT_Share;

	return true;
}

void CMagicStateCfgServer::UnloadConfig()
{
	ClearMap(m_mapCfg);
	m_mapCfgById.clear();
}

void CMagicStateCfgServer::UpdateCfg(const string& strName)
{
	MapMagicStateCfg::iterator it_name = m_mapCfg.find(strName);
	if(it_name != m_mapCfg.end())
	{
		CMagicStateCfgServerSharedPtr* pOldCfg = it_name->second;
		CMagicStateCfgServerSharedPtr* pNewCfg = new CMagicStateCfgServerSharedPtr(new CMagicStateCfgServer(*(pOldCfg->get())));
		m_mapCfg.erase(it_name);
		CfgChk::InsertStringSharedPtrMap(m_mapCfg, pNewCfg);
		MapMagicStateCfgById::iterator it_id2 = m_mapCfgById.find(pOldCfg->get()->GetId());
		if(it_id2 != m_mapCfgById.end())
		{
			m_mapCfgById.erase(it_id2);
			CfgChk::InsertUint32SharedPtrMap(m_mapCfgById, pNewCfg);
		}

		delete pOldCfg;
	}
}

CMagicStateCfgServerSharedPtr& CMagicStateCfgServer::Get(const string& magicStateName)
{
	CMagicStateCfgServer::MapMagicStateCfg::iterator mapCfgItr;
	mapCfgItr = m_mapCfg.find(magicStateName);
	if (mapCfgItr == m_mapCfg.end())
	{
		stringstream ExpStr;
		ExpStr << "魔法状态名不存在";
		CfgChk::GenExpInfo(ExpStr.str(), magicStateName);
		return ms_NULL;
	}
	return *(mapCfgItr->second);
}

CMagicStateCfgServerSharedPtr& CMagicStateCfgServer::GetById(uint32 uId)
{
	CMagicStateCfgServer::MapMagicStateCfgById::iterator mapCfgItr;
	mapCfgItr = m_mapCfgById.find(uId);
	if (mapCfgItr == m_mapCfgById.end())
	{
		stringstream ExpStr;
		ExpStr << "魔法状态ID不存在";
		CfgChk::GenExpInfo(ExpStr.str(), uId);
		return ms_NULL;
	}
	return *(mapCfgItr->second);
}

bool CMagicStateCfgServer::InstallerOughtToBeNull()
{
	return (m_eCascadeType == eCT_Central || m_eCascadeType == eCT_Share) && (!m_calcCascadeMax->IsSingleNumber() || m_calcCascadeMax->GetIntValue() > 1);
}

const CMagicEffServerSharedPtr& CMagicStateCfgServer::GetCancelableMagicEff()const
{
	return m_pCancelableMagicEff->Get();
}

const CMagicEffServerSharedPtr& CMagicStateCfgServer::GetDotMagicEff()const
{
	return m_pDotMagicEff->Get();
}

const CMagicEffServerSharedPtr& CMagicStateCfgServer::GetFinalMagicEff()const
{
	return m_pFinalMagicEff->Get();
}
