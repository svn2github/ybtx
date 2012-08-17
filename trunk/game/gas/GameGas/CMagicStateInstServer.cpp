#include "stdafx.h"
#include "CMagicStateInstServer.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "DebugHelper.h"
#include "TSqrAllocator.inl"
#include "CMagicStateCfg.h"
#include "CFighterDictator.h"

uint32 CMagicEffDataMgrServer::m_uOrder = 0;

CMagicStateCascadeDataMgr::CMagicStateCascadeDataMgr()
{
	//m_eInstType = eIT_MagicStateInst;
	m_itrCascade = m_listEffDataMgr.end();
}

CMagicStateCascadeDataMgr::~CMagicStateCascadeDataMgr()
{
	//for(m_itrCascade = m_listEffDataMgr.begin(); m_itrCascade != m_listEffDataMgr.end(); m_itrCascade++)
	//{
	//	ClearMap(*m_itrCascade);
	//}

	ClearCascade();
	//SafeDelete(m_pCfg);
}

//CMagicStateCfgServerSharedPtr& CMagicStateCascadeDataMgr::GetCfg()
//{
//	return *m_pCfg;
//}

//CMagicEffDataMgrServer*	CMagicStateCascadeDataMgr::GetCurEffDataMgr(uint32 uId)
//{
//	if(m_itrCascade == m_listMapEffDataMgr.end())
//	{
//		stringstream str;
//		str << "叠加层数迭代器未赋初结点或已到末结点\n";
//		GenErr(str.str());
//		return NULL;
//	}
//	MapMEffDataMgrServer& MapDataMgr = *m_itrCascade;
//	MapMEffDataMgrServer::iterator itr = MapDataMgr.find(uId);
//	if(itr == MapDataMgr.end())
//	{
//		stringstream str;
//		str << "ID为" << uId << "的魔法效果未加入魔法状态内的存储对象\n";
//		GenErr(str.str());
//		return NULL;
//	}
//	return itr->second;
//	return NULL;
//}
//
//CMagicEffDataMgrServer* CMagicStateCascadeDataMgr::AddDataMgr(uint32 uId)
//{
//	if(m_listMapEffDataMgr.empty())
//	{
//		stringstream str;
//		str << "未插入魔法状态的叠加维\n";
//		GenErr(str.str());
//		return NULL;
//	}
//	MapMEffDataMgrServer& MapDataMgr = m_listMapEffDataMgr.back();
//	CMagicEffDataMgrServer* pDataMgr = new CMagicEffDataMgrServer;
//	MapDataMgr.insert(make_pair(uId, pDataMgr));
//	return pDataMgr;
//}

CMagicEffDataMgrServer::CMagicEffDataMgrServer(CFighterDictator* pFrom)
:m_uFromID(0)
//:m_pArrayMopData(NULL), m_uArrayLen(0)
{
	++m_uOrder;
	if(pFrom)
	{
		m_uFromID = pFrom->GetEntityID();
	}
}

CMagicEffDataMgrServer::~CMagicEffDataMgrServer()
{
	Clear();
}

void CMagicEffDataMgrServer::Clear()
{
	//if(m_pArrayMopData != NULL)
	//{
	//	delete[] m_pArrayMopData;
	//	m_pArrayMopData = NULL;
	//}
	m_vecMopData.clear();
}

void CMagicEffDataMgrServer::CreateArrayMopData(uint32 uCount)
{
	Clear();
	//m_pArrayMopData = new CMagicOpStoredObj[uCount];
	//m_uArrayLen = uCount;
	m_vecMopData.resize(uCount);
}

const CMagicOpStoredObj& CMagicEffDataMgrServer::GetArrayMopDataForMagicState(uint32 uIndex, CSkillInstServer* pSkillInst)const
{
	if(uIndex > GetArrayLen())
	{
		stringstream str;
		str << "GetArrayMopDataForMagicState() uIndex = " << uIndex << ", GetArrayLen() = " << GetArrayLen();
		if(pSkillInst)
		{
			str << ", pSkillInst->GetType() = " << pSkillInst->GetType() << ", pSkillInst->GetRef() = "
				<< pSkillInst->GetRef();
			CSkillLocalInstServer* pLocalInst = pSkillInst->GetLocalInst();
			if(pLocalInst)
			{
				str << ", pLocalInst->GetRef() = " << pLocalInst->GetRef();
			}
			else
			{
				str << ", pLocalInst = NULL";
			}
		}
		else
		{
			str << ", pSkillInst == NULL";
		}
		GenErr("没有该操作的参数数据", str.str());
		//return NULL;
	}

	//return m_pArrayMopData[uIndex - 1];
	return m_vecMopData[uIndex - 1];
}

const CMagicOpStoredObj& CMagicEffDataMgrServer::GetArrayMopDataForOther(uint32 uIndex, CSkillInstServer* pSkillInst)const
{
	if(uIndex > GetArrayLen())
	{
		stringstream str;
		str << "GetArrayMopDataForOther() uIndex = " << uIndex << ", GetArrayLen() = " << GetArrayLen();
		if(pSkillInst)
		{
			str << ", pSkillInst->GetType() = " << pSkillInst->GetType() << ", pSkillInst->GetRef() = "
				<< pSkillInst->GetRef();
			CSkillLocalInstServer* pLocalInst = pSkillInst->GetLocalInst();
			if(pLocalInst)
			{
				str << ", pLocalInst->GetRef() = " << pLocalInst->GetRef();
			}
			else
			{
				str << ", pLocalInst = NULL";
			}
		}
		else
		{
			str << ", pSkillInst == NULL";
		}
		GenErr("没有该操作的参数数据", str.str());
		//return NULL;
	}

	//return m_pArrayMopData[uIndex - 1];
	return m_vecMopData[uIndex - 1];
}

const CMagicOpStoredObj& CMagicEffDataMgrServer::GetArrayMopData(uint32 uIndex)const
{
	if(uIndex > GetArrayLen())
	{
		stringstream str;
		str << "uIndex = " << uIndex << ", GetArrayLen() = " << GetArrayLen();
		GenErr("没有该操作的参数数据", str.str());
		//return NULL;
	}

	//return m_pArrayMopData[uIndex - 1];
	return m_vecMopData[uIndex - 1];
}

void CMagicEffDataMgrServer::SetArrayMopDataForMagicState(uint32 uIndex, const CMagicOpStoredObj& pMopData)
{
	if(uIndex > GetArrayLen())
	{
		stringstream str;
		str << "uIndex = " << uIndex << ", GetArrayLen() = " << m_vecMopData.size();
		GenErr("设置操作的参数数据失败\n", str.str());
		return ;
	}

	//m_pArrayMopData[uIndex - 1] = pMopData;
	m_vecMopData[uIndex - 1] = pMopData;
}

void CMagicEffDataMgrServer::SetArrayMopDataForOther(uint32 uIndex, const CMagicOpStoredObj& pMopData)
{
	if(uIndex > GetArrayLen())
	{
		stringstream str;
		str << "uIndex = " << uIndex << ", GetArrayLen() = " << m_vecMopData.size();
		GenErr("设置操作的参数数据失败\n", str.str());
		return ;
	}

	//m_pArrayMopData[uIndex - 1] = pMopData;
	m_vecMopData[uIndex - 1] = pMopData;
}

CMagicEffDataMgrServer*	CMagicStateCascadeDataMgr::GetCurEffDataMgr()const
{
	if(m_itrCascade == m_listEffDataMgr.end())
	{
		stringstream str;
		str << "叠加层数迭代器未赋初结点或已到末结点\n";
		GenErr(str.str());
		//return NULL;
	}
	return &(*m_itrCascade);
}

CMagicEffDataMgrServer* CMagicStateCascadeDataMgr::GetLastEffDataMgr()
{
	if(m_listEffDataMgr.empty())
	{
		stringstream str;
		str << "未插入魔法状态的叠加维\n";
		GenErr(str.str());
		//return NULL;
	}
	return &m_listEffDataMgr.back();
}

const ListEffDataMgrServer& CMagicStateCascadeDataMgr::GetEffDataMgrList()const
{
	return m_listEffDataMgr;
}

void CMagicEffDataMgrServer::CopyDataWithRatioFrom(const CMagicEffDataMgrServer* pSrc, float fRatio)
{
	CreateArrayMopData(pSrc->GetArrayLen());
	
	for(uint32 i = 0; i < pSrc->GetArrayLen(); i++)
	{
		const CMagicOpStoredObj& StoredObj = pSrc->GetArrayMopData(i + 1);
		//m_pArrayMopData[i].m_iMOPArg = int32(StoredObj.m_iMOPArg * fRatio);
		//m_pArrayMopData[i].m_uMOPRet = StoredObj.m_uMOPRet;
		m_vecMopData[i].m_MOPArg = int32(StoredObj.m_MOPArg.GetInt32() * fRatio);
		m_vecMopData[i].m_uMOPRet = StoredObj.m_uMOPRet;
	}
}

void CMagicStateCascadeDataMgr::CopyDataWithRatioFrom(CMagicStateCascadeDataMgr* pSrc, float fRatio)
{
	//m_listEffDataMgr.clear();
	
	GetLastEffDataMgr()->CopyDataWithRatioFrom(pSrc->GetLastEffDataMgr(), fRatio);

	////因为下面的for循环有可能破坏pSrc中保存的迭代器
	//ListEffDataMgrServer::iterator oldItrOfSrc = pSrc->GetItr();

	//for(pSrc->SetItrBegin(); pSrc->GetItrEnd(); pSrc->DoItrStep())
	//{
	//	CMagicEffDataMgrServer magicEffDataMgrServer;
	//	magicEffDataMgrServer.CopyDataWithRatioFrom(pSrc->GetCurEffDataMgr(), fRatio);
	//	m_listEffDataMgr.push_back(magicEffDataMgrServer);
	//}

	//pSrc->SetItr(oldItrOfSrc);	//还原pSrc中的迭代器
}

void CMagicStateCascadeDataMgr::SerializeToDB(CStoredObjDBDataVec* pStoredRet, ICharacterDictatorCallbackHandler* pHandler, uint32 uFighterGlobalID, uint32 uInstID)
{
	size_t uCascadeIndex = 1;
	for(ListEffDataMgrServer::iterator itr = m_listEffDataMgr.begin();
		itr != m_listEffDataMgr.end(); itr++, uCascadeIndex++)
	{	//遍历叠加层数
		for(size_t uMopIndex = 1; uMopIndex <= itr->GetArrayLen(); uMopIndex++)
		{	//遍历魔法操作
			const CMagicOpStoredObj& StoredObj = itr->GetArrayMopData(uMopIndex);
			CStoredObjDBData* pStored = new CStoredObjDBData(uInstID, uCascadeIndex, uMopIndex, StoredObj.m_MOPArg.GetInt32(), StoredObj.m_uMOPRet);
			//pHandler->AddStoredObjToDB(uFighterGlobalID, uInstID, &aState);
			pStoredRet->PushBack(pStored);
#ifdef COUT_STATEDB_INFO
			cout << "第" << uCascadeIndex << "层状态第" << uMopIndex << "个魔法操作的存储对象存入数据库\n";
#endif
		}
	}
}

//EMagicStateEffType CMagicStateCascadeDataMgr::GetMagicStateEffTypeByTitle(const string& sTitle)
//{
//	map<string, EMagicStateEffType>::iterator itr = m_mapMagicStateEffType.find(sTitle);
//	if(itr != m_mapMagicStateEffType.end())
//	{
//		return itr->second;
//	}
//	else
//	{
//		stringstream str;
//		str << "名字为[" << sTitle << "]";
//		GenErr("CMagicStateCascadeDataMgr::GetMagicStateEffTypeByTitle 找不到的魔法效果类型", str.str());
//		return eMSET_Undefined;
//	}
//}


void CMagicStateCascadeDataMgr::AssertItrEnd()
{
	if(m_itrCascade == m_listEffDataMgr.end())
		GenErr("魔法状态里的存储对象超出读取范围");
}

int32 CMagicStateCascadeDataMgr::CalcAllEffData()
{
	int32 iValue = 0;
	ListEffDataMgrServer::iterator itr = m_listEffDataMgr.begin();
	for(;itr != m_listEffDataMgr.end(); itr++)
	{	//遍历叠加层数
		for(size_t uMopIndex = 1; uMopIndex <= itr->GetArrayLen(); uMopIndex++)
		{	//遍历魔法操作
			const CMagicOpStoredObj& StoredObj = itr->GetArrayMopData(uMopIndex);
			const CValueData& Value = StoredObj.m_MOPArg;
			iValue += Value.GetInt32();
		}
	}
	return iValue;
}
