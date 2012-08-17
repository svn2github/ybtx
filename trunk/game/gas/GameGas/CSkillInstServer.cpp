#include "stdafx.h"
#include "CSkillInstServer.h"
#include "ErrLogHelper.h"
#include <sstream>
#include "CFighterDictator.h"
#include "CCharacterDictator.h"
#include "CGameConfigServer.h"
#include "CMagicStateCfg.h"
#include "BaseHelper.h"
#include "CSkillServer.h"

#ifdef TRACE_SKILLINST_COUNTER
uint32 CSkillInstServer::m_uMaxAllInstID = 0;
#endif

CSkillGlobalInstServer::CSkillGlobalInstServer(uint32 uCreatorID,const string& sSkillName, const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, EAttackType eAttackType)
: m_uCreatorID(uCreatorID)
, m_uSkillLevel(uSkillLevel)
, m_strSkillName(sSkillName)
, m_eAttackType(eAttackType)
, m_uDir(0)
, m_fValueScale(0)
, m_bProcessLocked(false)						 
, m_bForbitSetupState(false)
, m_uCounter(0)
, m_pSkill(new CNormalSkillServerSharedPtr(pSkill))
{
#ifdef TRACE_SKILLINST_COUNTER
	m_uID = CSkillInstServer::CreateInstID();
	cout << "CSkillGlobalInstServer(IscE)[" << m_uID << "]构造 = " << m_uCounter << "\n";
#endif
}

//给普攻用
CSkillGlobalInstServer::CSkillGlobalInstServer()
: m_uCreatorID(0)
, m_uSkillLevel(0)
, m_eAttackType(eATT_None)
, m_uDir(0)
, m_fValueScale(0)
, m_bForbitSetupState(false)
, m_uCounter(0)
, m_pSkill(NULL)
{
#ifdef TRACE_SKILLINST_COUNTER
	m_uID = CSkillInstServer::CreateInstID();
	cout << "CSkillGlobalInstServer()[" << m_uID << "]构造 = " << m_uCounter << "\n";
#endif
}

CSkillGlobalInstServer::~CSkillGlobalInstServer()
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillGlobalInstServer[" << m_uID << "]析构\n";
#endif
	delete m_pSkill;
	m_pSkill = NULL;
}

void CSkillGlobalInstServer::AddRef()
{
	++m_uCounter;
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillGlobalInstServer[" << m_uID << "] AddRef = " << m_uCounter << "\n";
#endif
}

void CSkillGlobalInstServer::DelRef()
{
	--m_uCounter;
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillGlobalInstServer[" << m_uID << "] DelRef = " << m_uCounter << "\n";
#endif
	if(m_uCounter == 0)
		delete this;
}

ESkillType CSkillGlobalInstServer::GetSkillType()
{
	//if (GetInterval())			//如果DOT伤害与物理魔法伤害分离的话就需要这句
	//	return eST_DOT;

	if (eATT_Puncture == m_eAttackType || eATT_Chop == m_eAttackType || eATT_BluntTrauma == m_eAttackType)
		return eST_Physical;

	if (eATT_Nature == m_eAttackType || eATT_Destroy == m_eAttackType || eATT_Evil == m_eAttackType)
		return eST_Magic;

	return eST_None;
};









CSkillDupliInstServer::CSkillDupliInstServer()
: m_bTargetAliveCheckIsIgnored(false)
, m_bIsTrigger(false)
, m_bIsScopeTargetMop(false)
, m_uCounter(0)
{
#ifdef TRACE_SKILLINST_COUNTER
	m_uID = CSkillInstServer::CreateInstID();
	cout << "CSkillDupliInstServer()[" << m_uID << "]构造 = " << m_uCounter << "\n";
#endif
}

CSkillDupliInstServer::CSkillDupliInstServer(CSkillDupliInstServer* pSkillInst)
: m_bTargetAliveCheckIsIgnored(pSkillInst->m_bTargetAliveCheckIsIgnored)
, m_bIsTrigger(pSkillInst->m_bIsTrigger)
, m_bIsScopeTargetMop(false)
, m_uCounter(0)
{
#ifdef TRACE_SKILLINST_COUNTER
	m_uID = CSkillInstServer::CreateInstID();
	cout << "CSkillDupliInstServer(Inst)[" << m_uID << "]构造 = " << m_uCounter << "\n";
#endif
}

//void CSkillDupliInstServer::Release(CSkillInstServer* pSkillInst)
//{
//	RevertPropertyMap(m_mapPropertyAdderChange, pSkillInst);
//	RevertPropertyMap(m_mapPropertyMultiplierChange, pSkillInst);
//	delete this;
//}

CSkillDupliInstServer::~CSkillDupliInstServer()
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillDupliInstServer[" << m_uID << "]析构\n";
#endif
	//ClearMap(m_mapPropertyAdderChange);
	//ClearMap(m_mapPropertyMultiplierChange);
}

CSkillDupliInstServer* CSkillDupliInstServer::CreateBranch(CSkillDupliInstServer* pSkillDupliInst)
{
	return new CSkillDupliInstServer(pSkillDupliInst);
}

void CSkillDupliInstServer::AddRef()
{
	++m_uCounter;
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillDupliInstServer[" << m_uID << "] AddRef = " << m_uCounter << "\n";
#endif
}

void CSkillDupliInstServer::DelRef()
{
	--m_uCounter;
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillDupliInstServer[" << m_uID << "] DelRef = " << m_uCounter << "\n";
#endif
	if(m_uCounter == 0)
		delete this;
}










CSkillLocalInstServer::CSkillLocalInstServer()
: m_uCounter(0)
, m_bContainerExist(false)
{
#ifdef TRACE_SKILLINST_COUNTER
	m_uID = CSkillInstServer::CreateInstID();
	cout << "CSkillLocalInstServer()[" << m_uID << "]构造\n";
#endif
}

CSkillLocalInstServer::~CSkillLocalInstServer()
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillLocalInstServer[" << m_uID << "]析构\n";
#endif
}

void CSkillLocalInstServer::SetStateId(uint32 uStateId)
{
	stringstream str;
	str << "StateId = " << uStateId;
	GenErr("非状态不能调用SetStateId()", str.str());
}

void CSkillLocalInstServer::SetInterval(bool bIsDOT)
{
	stringstream str;
	str << "bIsDOT = " << bIsDOT;
	GenErr("非状态不能调用SetInterval()", str.str());
}

void CSkillLocalInstServer::SetIsCalc(bool bIsCalc)
{
	stringstream str;
	str << "bIsCalc = " << bIsCalc;
	GenErr("非状态不能调用SetIsCalc()", str.str());
}

void CSkillLocalInstServer::AddRef()
{
	++m_uCounter;
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillLocalInstServer[" << m_uID << "] AddRef = " << m_uCounter << "\n";
#endif
}

bool CSkillLocalInstServer::DelRef()
{
	--m_uCounter;
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillLocalInstServer[" << m_uID << "] DelRef = " << m_uCounter << "\n";
#endif
	if(m_uCounter == 0)
	{
		delete this;
		return true;
	}
	return false;
}






map<string, EMagicStateEffType>	CSkillMagicStateInstServer::m_mapMagicStateEffType;
bool CSkillMagicStateInstServer::m_bInit = CSkillMagicStateInstServer::InitMap();

CMagicStateCfgServerSharedPtr& CSkillMagicStateInstServer::GetCfg()
{
	return *m_pCfg;
}

CSkillMagicStateInstServer::CSkillMagicStateInstServer(CMagicStateCfgServerSharedPtr& pCfg, EMagicStateEffType eMagicStateEffType)
: m_eMagicStateEffType(eMagicStateEffType)
, m_pCfg(new CMagicStateCfgServerSharedPtr(pCfg))
, m_bIsInterval(false)
, m_bIsCalc(false)
{
}

CSkillMagicStateInstServer::~CSkillMagicStateInstServer()
{
	delete m_pCfg;
	m_pCfg = NULL;
}

EMagicStateEffType CSkillMagicStateInstServer::GetMagicStateEffTypeByTitle(const string& sTitle)
{
	map<string, EMagicStateEffType>::iterator itr = m_mapMagicStateEffType.find(sTitle);
	if(itr != m_mapMagicStateEffType.end())
	{
		return itr->second;
	}
	else
	{
		stringstream str;
		str << "名字为[" << sTitle << "]";
		GenErr("CMagicStateCascadeDataMgr::GetMagicStateEffTypeByTitle 找不到的魔法效果类型", str.str());
		return eMSET_Undefined;
	}
}








CSkillInstServer::CSkillInstServer()
: m_pGlobal(NULL)
, m_pDupli(NULL)
, m_pLocal(NULL)
, m_uCounter(0)
, m_uShockWaveCounter(0)
, m_bIsMagic(false)
, m_bIsTalent(false)
, m_bIsArea(false)
, m_bLastEffect(true)
, m_uOneEffectHPChangeValue(0)
, m_bPassive(false)
{
#ifdef TRACE_SKILLINST_COUNTER
	m_uID = CSkillInstServer::CreateInstID();
	cout << "CSkillInstServer()[" << m_uID << "]构造 = " << m_uCounter << "\n";
#endif
}

CSkillInstServer* CSkillInstServer::CreateOrigin(uint32 uCreatorID, const string& sSkillName, uint8 uSkillLevel, EAttackType eAttackType)
{
	return CreateOrigin(uCreatorID, sSkillName, CNormalSkillServer::GetSkillByName(sSkillName), uSkillLevel, eAttackType);
}

CSkillInstServer* CSkillInstServer::CreateOrigin(uint32 uCreatorID, const string& sSkillName, const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, EAttackType eAttackType)
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nCreateOrigin()\n";
#endif
	CSkillInstServer* pNewSkillInst = new CSkillInstServer;
	pNewSkillInst->m_pGlobal = new CSkillGlobalInstServer(uCreatorID, sSkillName, pSkill, uSkillLevel, eAttackType);
	pNewSkillInst->m_pDupli = new CSkillDupliInstServer;
	pNewSkillInst->AddRef();
	return pNewSkillInst;
}

void CSkillInstServer::DestroyOrigin()
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nDestroyOrigin()\n";
#endif
	DelRef();
}

CSkillInstServer* CSkillInstServer::CreateNAOrigin()
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nCreateNAOrigin()\n";
#endif
	CSkillInstServer* pNewSkillInst = new CSkillInstServer;
	pNewSkillInst->m_pGlobal = new CSkillGlobalInstServer;
	pNewSkillInst->m_pDupli = new CSkillDupliInstServer;
	pNewSkillInst->m_pLocal = new CNAInstServer;
	pNewSkillInst->m_pLocal->SetContainerExist(true);
	pNewSkillInst->AddRef();
	return pNewSkillInst;
}

void CSkillInstServer::DestroyNAOrigin()
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nDestroyNAOrigin()\n";
#endif
	m_pLocal->SetContainerExist(false);
	DelRef();
}

CSkillInstServer* CSkillInstServer::CreateTransfer(CSkillInstServer* pSkillInst)
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nCreateTransfer()\n";
#endif
	CSkillInstServer* pNewSkillInst = new CSkillInstServer;
	pNewSkillInst->m_pGlobal = pSkillInst->m_pGlobal;
	pNewSkillInst->m_pDupli = pSkillInst->m_pDupli;
	pNewSkillInst->AddRef();
	return pNewSkillInst;
}

void CSkillInstServer::DestroyTransfer()
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nDestroyTransfer()\n";
#endif
	DelRef();
}

CSkillInstServer* CSkillInstServer::CreatePartition(CSkillInstServer* pSkillInst)
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nCreatePartition()\n";
#endif
	CSkillInstServer* pNewSkillInst = new CSkillInstServer;
	pNewSkillInst->m_pGlobal = pSkillInst->m_pGlobal;
	pNewSkillInst->m_pDupli = CSkillDupliInstServer::CreateBranch(pSkillInst->m_pDupli);
	pNewSkillInst->AddRef();
	return pNewSkillInst;
}

void CSkillInstServer::DestroyPartition()
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nDestroyPartition()\n";
#endif
	DelRef();
}

//CSkillInstServer* CSkillInstServer::CreateLocal(CSkillInstServer* pSkillInst, EInstType eInstType)
//{
//	CSkillInstServer* pNewSkillInst = new CSkillInstServer;
//	pNewSkillInst->m_pGlobal = pSkillInst->m_pGlobal;
//	pNewSkillInst->m_pDupli = CSkillDupliInstServer::CreateBranch(pSkillInst->m_pDupli);
//	switch(eInstType)
//	{
//	case eIT_None:
//		break;
//	case eIT_MagicStateInst:
//		pNewSkillInst->m_pLocal = new CSkillMagicStateInstServer;
//		break;
//	case eIT_NormalAttackInst:
//		pNewSkillInst->m_pLocal = new CNAInstServer;
//		break;
//	default:
//		{
//			stringstream str;
//			str << eInstType;
//			GenErr("不可创建的CSkillLocalInstServer类型", str.str());
//		}
//	}
//	pNewSkillInst->AddRef();
//	return pNewSkillInst;
//}

CSkillInstServer* CSkillInstServer::CreateLocal(CSkillInstServer* pSkillInst, CSkillLocalInstServer* pLocalInst)
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nCreateLocal()\n";
#endif
	CSkillInstServer* pNewSkillInst = new CSkillInstServer;
	pNewSkillInst->m_pGlobal = pSkillInst->m_pGlobal;
	pNewSkillInst->m_pDupli = CSkillDupliInstServer::CreateBranch(pSkillInst->m_pDupli);
	pNewSkillInst->m_pLocal = pLocalInst;
	if(pLocalInst)
	{
		pLocalInst->SetContainerExist(true);
	}
	pNewSkillInst->AddRef();
	return pNewSkillInst;
}

void CSkillInstServer::DestroyLocal()
{
#ifdef TRACE_SKILLINST_COUNTER
	cout << "\nDestroyLocal()\n";
#endif
	if(m_pLocal)
	{
	//	delete m_pLocal;
	//	m_pLocal = NULL;
		m_pLocal->SetContainerExist(false);
	}
	DelRef();
}

CSkillInstServer::~CSkillInstServer()
{
	RevertPropertyMap(m_mapPropertyAdderChange);
	RevertPropertyMap(m_mapPropertyMultiplierChange);
	if(m_pLocal)
	{
		delete m_pLocal;
		m_pLocal = NULL;
	}
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillInstServer[" << m_uID << "]析构\n\n";
#endif
}

void CSkillInstServer::AddRef()
{
	m_pGlobal->AddRef();
	m_pDupli->AddRef();
	if(m_pLocal)
		m_pLocal->AddRef();
	++m_uCounter;
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillInstServer[" << m_uID << "] AddRef = " << m_uCounter << "\n";
#endif
}

void CSkillInstServer::DelRef()
{
	--m_uCounter;
#ifdef TRACE_SKILLINST_COUNTER
	cout << "CSkillInstServer[" << m_uID << "] DelRef = " << m_uCounter << "\n";
#endif
	if(m_pLocal)
	{
		if(m_pLocal->DelRef())
			m_pLocal = NULL;
	}
	m_pDupli->DelRef();
	m_pGlobal->DelRef();
	if(m_uCounter == 0)
		delete this;
}

const CNormalSkillServerSharedPtr& CSkillInstServer::GetSkillCfg()const
{
	if(m_pGlobal->m_pSkill == NULL)
		return CNormalSkillServer::ms_NULL;
	return *(m_pGlobal->m_pSkill);
}

EInstType CSkillInstServer::GetType()
{
	return m_pLocal ? m_pLocal->GetType() : eIT_SkillInst;
}

uint32 CSkillInstServer::GetStateId()
{
	return m_pLocal ? m_pLocal->GetStateId() : 0;
}

void CSkillInstServer::SetStateId(uint32 uStateId)
{
	if(m_pLocal)
		m_pLocal->SetStateId(uStateId);
}

bool CSkillInstServer::GetInterval()
{
	return m_pLocal ? m_pLocal->GetInterval() : false;
}

void CSkillInstServer::SetInterval(bool bIsDOT)	
{
	if(m_pLocal)
		m_pLocal->SetInterval(bIsDOT);
}

bool CSkillInstServer::GetIsCalc()
{
	return m_pLocal ? m_pLocal->GetIsCalc() : false;
}

void CSkillInstServer::SetIsCalc(bool bIsCalc)	
{
	if(m_pLocal)
		m_pLocal->SetIsCalc(bIsCalc);
}

CSkillMagicStateInstServer* CSkillInstServer::GetMagicStateInst()
{
	if(!m_pLocal)
	{
		GenErr("CSkillInstServer未创建Local");
		return NULL;
	}
	else
	{
		CSkillMagicStateInstServer* pMagicStateInst = NULL;
		pMagicStateInst = class_cast<CSkillMagicStateInstServer*>(m_pLocal);
		if(!pMagicStateInst)
		{
			GenErr("CSkillInstServer的Local不是CSkillMagicStateInstServer");
			return NULL;
		}
		return pMagicStateInst;
	}
}

CNAInstServer* CSkillInstServer::GetNAInstServer()
{
	if(!m_pLocal)
	{
		GenErr("CSkillInstServer未创建Local");
		return NULL;
	}
	else
	{
		CNAInstServer* pNAInst = NULL;
		pNAInst = class_cast<CNAInstServer*>(m_pLocal);
		if(!pNAInst)
		{
			GenErr("CSkillInstServer的Local不是CNAInstServer");
			return NULL;
		}
		return pNAInst;
	}
}

void CSkillInstServer::RevertPropertyMap(MapPropertyChange& mapProperty)
{
	if (mapProperty.empty())
	{
		return;
	}
	MapPropertyChange::iterator it_Begin = mapProperty.begin();
	MapPropertyChange::iterator it_End = mapProperty.end();
	for(;it_Begin!=it_End;++it_Begin)
	{
		if (CGameConfigServer::Inst()->GetCheckSkillValueSafeState())
		{
			CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(it_Begin->first.second);
			ostringstream strErr; 
			strErr << this->GetSkillName() << " 修改未撤销的数值: " << it_Begin->first.first <<" = ";
			it_Begin->second->GetValueToStr(strErr);
			if (pCharacter->GetNpcAIHandler())
				strErr << " NpcName:  "<<pCharacter->m_sNpcName<<endl;
			else
				strErr << " 玩家EntityID 为： "<<it_Begin->first.second<<endl;
			LogErr("技能修改数值未撤销",strErr.str());
			if (pCharacter)
			{
				CFighterDictator* pFighter = pCharacter->GetFighter();
				it_Begin->second-> RevertPropertyValue(pFighter);
			}
		}
		delete it_Begin->second;
	}
}

void CSkillInstServer::InitSkillLevel(uint8 uSkillLevel)
{
	//if(m_pGlobal->m_uSkillLevel != 0)
	//{
	//	stringstream str;
	//	str << "待设等级：" << uSkillLevel << "；已设等级：" << uSkillLevel;
	//	LogErr("CSkillInstServer::InitSkillLevel(uSkillLevel)不能修改已设置的技能等级", str.str());
	//	return;
	//}
	m_pGlobal->m_uSkillLevel = uSkillLevel;
}
