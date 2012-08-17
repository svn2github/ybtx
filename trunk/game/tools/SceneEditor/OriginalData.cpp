#include "stdafx.h"
#include "OriginalData.h"
#include "DataType.h"


using namespace std;


COriginalData::COriginalData(void)
{
	
}

COriginalData::~COriginalData(void)
{
	DelAll();
}


PNPC COriginalData::Exp_AddNpc(const char* _名字,
					   const char*		_类型,
					   const char*  _AI类型,
					   uint8		_Npc是否唯一,
					   uint8		_NPC阵营,
					   uint8		_等级下限,
					   uint8		_等级上限,
					   uint32		_刷新时间,
					   uint8		_职业
					   )
{

	PNPC		pNewNpc = new NPC;
	pNewNpc->_名字 =  _名字;
	pNewNpc->_类型 =  _类型;
	pNewNpc->_AI类型 = _AI类型;
	pNewNpc->_Npc是否唯一 = _Npc是否唯一;
	pNewNpc->_等级下限    = _等级下限;
	pNewNpc->_等级上限    = _等级上限;
	pNewNpc->_刷新时间    = _刷新时间;
	pNewNpc->_NPC职业	= _职业;

	m_mapNpcData.insert(std::make_pair(pNewNpc->_名字,pNewNpc));
	return pNewNpc;
}

void COriginalData::Exp_AddIntObj(const char* name, uint8 IsSingle, uint8 IsBarrier)
{
	POBJ pNewObj = new OBJ;
	pNewObj->name = name;
	pNewObj->isSingle = IsSingle;
	pNewObj->isBarrier = IsBarrier;
	m_mapObjData.insert(std::make_pair(pNewObj->name,pNewObj));
	if (IsBarrier != 0)
	{
		m_vecBarrierObj.push_back(name);
	}
}



void COriginalData::Exp_AddTrap(const char* name, uint8 IsSingle)
{
	PTRAP pNewTrap = new TRAP;
	pNewTrap->name = name;
	pNewTrap->isSingle = IsSingle;
	m_mapTrapData.insert(std::make_pair(pNewTrap->name,pNewTrap));
}

bool COriginalData::Exp_AddSceneCfg(const char* sceneName, const char* mapFile, const char* sceneFile, const char* areaFile)
{
	if (!sceneName || !mapFile)
	{
		return false;
	}
	SceneCfg* pCfg = new SceneCfg(mapFile, sceneFile, areaFile);
	return m_mapSceneCfg.insert(make_pair(sceneName, pCfg)).second;
}


void COriginalData::DelAll()
{
	NpcData_Iter npcIt = m_mapNpcData.begin();
	while (npcIt != m_mapNpcData.end())
	{
		if(npcIt->second)
		{
			delete npcIt->second;
		}
		npcIt = m_mapNpcData.erase(npcIt);
	}
	m_mapNpcData.clear();

	ObjData_Iter objIt = m_mapObjData.begin();
	while (objIt != m_mapObjData.end())
	{
		if(objIt->second)
		{
			delete objIt->second;
		}
		objIt = m_mapObjData.erase(objIt);
	}
	m_mapObjData.clear();


	TrapData_Iter trapIt = m_mapTrapData.begin();
	while (trapIt != m_mapTrapData.end())
	{
		if(trapIt->second)
		{
			delete trapIt->second;
		}
		trapIt = m_mapTrapData.erase(trapIt);
	}
	m_mapTrapData.clear();

	SceneCfg_Iter sceneIt = m_mapSceneCfg.begin();
	while (sceneIt != m_mapSceneCfg.end())
	{
		if(sceneIt->second)
		{
			delete sceneIt->second;
		}
		sceneIt = m_mapSceneCfg.erase(sceneIt);
	}
	m_mapSceneCfg.clear();

}

PNPC COriginalData::GetNpc(const CString& name)
{

	NpcData_Iter iter = m_mapNpcData.find(name);
	if (iter == m_mapNpcData.end())
	{
		return NULL;
	}
	return iter->second;
}

bool COriginalData::IsSingleNpc(const CString& name)
{
	NpcData_Iter iter = m_mapNpcData.find(name);
	if (iter == m_mapNpcData.end())
	{
		cout << name << " 不在 COriginalData 管理中"<< endl;
		return false;
	}
	if (iter->second->_Npc是否唯一 == 1)
	{
		return true;
	}
	return false;
}

bool COriginalData::IsSingleObj(const CString& name)
{
	ObjData_Iter iter = m_mapObjData.find(name);
	if (iter == m_mapObjData.end())
	{
		cout << name << " 不在 COriginalData 管理中"<< endl;
		return false;
	}
	if (iter->second->isSingle == 1)
	{
		return true;
	}
	return false;
}

bool COriginalData::IsSingleTrap(const CString& name)
{
	TrapData_Iter iter = m_mapTrapData.find(name);
	if (iter == m_mapTrapData.end())
	{
		cout << name << " 不在 COriginalData 管理中"<< endl;
		return false;
	}
	if (iter->second->isSingle == 1)
	{
		return true;
	}
	return false;
}

bool COriginalData::IsNpcInCommonCfg(const CString& name)
{
	return m_mapNpcData.find(name) != m_mapNpcData.end();
}

bool COriginalData::IsObjInCommonCfg(const CString& name)
{
	return m_mapObjData.find(name) != m_mapObjData.end();
}

bool COriginalData::IsTrapInCommonCfg(const CString& name)
{
	return m_mapTrapData.find(name) != m_mapTrapData.end();
}

bool COriginalData::IsBarrierObj(const CString& name)
{
	ObjData_Iter iter = m_mapObjData.find(name);
	if (iter == m_mapObjData.end())
	{
		cout << name << " 不在 COriginalData 管理中"<< endl;
		return false;
	}
	if (iter->second->isBarrier != 0)
	{
		return true;
	}
	return false;
}

CString COriginalData::GetMapFile(const CString& sceneName)
{
	SceneCfg_Map::iterator iter = m_mapSceneCfg.find(sceneName);
	if (iter != m_mapSceneCfg.end())
	{
		return (iter->second)->mapFile;
	}
	else
		return "";
}

CString COriginalData::GetSceneFile(const CString& sceneName)
{
	SceneCfg_Map::iterator iter = m_mapSceneCfg.find(sceneName);
	if (iter != m_mapSceneCfg.end())
	{
		return (iter->second)->sceneFile;
	}
	else
		return "";
}

CString COriginalData::GetAreaFile(const CString& sceneName)
{
	SceneCfg_Map::iterator iter = m_mapSceneCfg.find(sceneName);
	if (iter != m_mapSceneCfg.end())
	{
		return (iter->second)->areaFile;
	}
	else
		return "";
}