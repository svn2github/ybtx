#include "stdafx.h"
#include "CTalentEffServer.h"
#include "CMagicEffServer.h"
#include "CSkillInstServer.h"
#include "CFighterMediator.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"
#include "CMagicEffDataMgrServer.h"

void	CTalentHolderServer::InsertTalent(const string& szName, uint32 uPoint)
{
	const CTalentEffCfgServerSharedPtr& pCfg = CTalentEffCfgServer::GetTalentEff(szName);
	if (!pCfg||eTT_Skill==pCfg->GetTalentType())
	{
		m_setSkillTalent.insert(szName);
	}
	CTalentHolder::InsertTalent(szName,uPoint);
}

void	CTalentHolderServer::RemoveTalent(const string& szName)
{
	CTalentHolder::RemoveTalent(szName);
	m_setSkillTalent.erase(szName);
}

void	CTalentHolderServer::InsertEquipTalent(const string& szName, uint32 uPoint)
{
	const CTalentEffCfgServerSharedPtr& pCfg = CTalentEffCfgServer::GetTalentEff(szName);
	if (!pCfg)
	{
		CTalentHolder::ChangeTalent(szName,uPoint);
		return;
	}
	Ast(pCfg&&(eTT_Equip==pCfg->GetTalentType()));
	switch (pCfg->GetRuleType())
	{
	case eTRT_Stackup:
		CTalentHolder::ChangeTalent(szName,uPoint);
		break;
	case eTRT_Decentral:
		InsertDecentralTalent(szName,uPoint);
		break;
	case eTRT_Unique:
		InsertUniqueTalent(szName,uPoint);
		break;
	}
}

bool	CTalentHolderServer::RemoveEquipTalent(const string& szName,uint32 uPoint)
{
	bool bResult =true;
	const CTalentEffCfgServerSharedPtr& pCfg = CTalentEffCfgServer::GetTalentEff(szName);
	if (!pCfg)
	{
		CTalentHolder::ChangeTalent(szName,-(int32)uPoint);
		return bResult;
	}
	Ast(pCfg&&(eTT_Equip==pCfg->GetTalentType()));
	switch (pCfg->GetRuleType())
	{
	case eTRT_Stackup:
		CTalentHolder::ChangeTalent(szName,-(int32)uPoint);
		break;
	case eTRT_Decentral:
		bResult = RemoveDecentralTalent(szName,uPoint);
		break;
	case eTRT_Unique:
		bResult = RemoveUniqueTalent(szName,uPoint);
	}
	return bResult;
}

void	CTalentHolderServer::ClearAllSkillTalent()
{
	while (!m_setSkillTalent.empty())
	{
		RemoveTalent(*m_setSkillTalent.begin());
	}
}

void CTalentHolderServer::InsertDecentralTalent(const string& szName, uint32 uPoint)
{	
	uint32 uTalentPoint = GetPoint(szName);
	MapLessRuleTalent::iterator it = m_mapRuleTalent.find(szName);
	if (uPoint<=uTalentPoint)
	{
		if (it!=m_mapRuleTalent.end())
		{
			MultisetTalent& setTalent= it->second;
			setTalent.insert(uPoint);
		}
		else
		{
			MultisetTalent setTalent;
			setTalent.insert(uPoint);
			m_mapRuleTalent.insert(make_pair(szName, setTalent));
		}
		if (GetHandler()) GetHandler()->OnTalentAdded(szName,uPoint);
	}
	else
	{
		if (it!=m_mapRuleTalent.end())
		{
			MultisetTalent& setTalent= it->second;
			setTalent.insert(uTalentPoint);
		}
		else  if(uTalentPoint!=0)
		{
			MultisetTalent setTalent;
			setTalent.insert(uTalentPoint);
			m_mapRuleTalent.insert(make_pair(szName, setTalent));
		}
		InsertTalent(szName,uPoint);
	}
}

void CTalentHolderServer::InsertUniqueTalent(const string& szName, uint32 uPoint)
{
	uint32 uTalentPoint = GetPoint(szName);
	MapLessRuleTalent::iterator it = m_mapRuleTalent.find(szName);
	if (it!=m_mapRuleTalent.end())
	{
		MultisetTalent& setTalent= it->second;
		if (uPoint<=uTalentPoint)
		{
			setTalent.insert(uPoint);
		}
		else
		{
			RemoveTalent(szName);
			setTalent.insert(uTalentPoint);
			InsertTalent(szName,uPoint);
		}
	}	
	else
	{
		if (uPoint<=uTalentPoint)
		{
			MultisetTalent setTalent;
			setTalent.insert(uPoint);
			m_mapRuleTalent.insert(make_pair(szName, setTalent));
		}
		else
		{		
			if (uTalentPoint!=0)
			{
				RemoveTalent(szName);
				MultisetTalent setTalent;
				setTalent.insert(uTalentPoint);
				m_mapRuleTalent.insert(make_pair(szName, setTalent));
			}
			InsertTalent(szName,uPoint);
		}
	}
}

bool CTalentHolderServer::RemoveDecentralTalent(const string& szName, uint32 uPoint)
{
	bool bResult =true;
	uint32 uTalentPoint = GetPoint(szName);
	MapLessRuleTalent::iterator it = m_mapRuleTalent.find(szName);
	if (it!=m_mapRuleTalent.end())
	{
		MultisetTalent& setTalent= it->second;
		MultisetTalent::iterator set_it = setTalent.find(uPoint);
		if (set_it!=setTalent.end())
		{
			setTalent.erase(set_it);
			if (GetHandler()) GetHandler()->OnTalentRemove(szName,uPoint);
		}
		else
		{
			RemoveTalent(szName);
			if (uPoint!=uTalentPoint)
			{
				bResult = false;
				ostringstream strErr; 
				strErr << szName <<":  "<<uPoint<<"  "<<uTalentPoint<<endl;
				LogErr("要删除天赋点与当前天赋点数不符",strErr.str());
			}
			if (GetHandler()) GetHandler()->OnTalentRemove(szName,(*setTalent.begin()));
			InsertTalent(szName,(*setTalent.begin()));
			setTalent.erase(setTalent.begin());
		}
		if (setTalent.empty()) m_mapRuleTalent.erase(it);
	}
	else
	{
		RemoveTalent(szName);
		if (uPoint!=uTalentPoint)
		{
			bResult = false;
			ostringstream strErr; 
			strErr << szName <<":  "<<uPoint<<"  "<<uTalentPoint<<endl;
			LogErr("要删除天赋点与当前天赋点数不符",strErr.str());
		}
	}
	return bResult;
}
	
bool CTalentHolderServer::RemoveUniqueTalent(const string& szName, uint32 uPoint)
{
	bool bResult =true;
	uint32 uTalentPoint = GetPoint(szName);
	MapLessRuleTalent::iterator it = m_mapRuleTalent.find(szName);
	if (it!=m_mapRuleTalent.end())
	{
		MultisetTalent& setTalent= it->second;
		MultisetTalent::iterator set_it = setTalent.find(uPoint);
		if (set_it!=setTalent.end())
		{
			setTalent.erase(set_it);
		}
		else
		{
			RemoveTalent(szName);
			if (uPoint!=uTalentPoint)
			{
				bResult = false;
				ostringstream strErr; 
				strErr << szName <<":  "<<uPoint<<"  "<<uTalentPoint<<endl;
				LogErr("要删除天赋点与当前天赋点数不符",strErr.str());
			}
			InsertTalent(szName,(*setTalent.begin()));
			setTalent.erase(setTalent.begin());
		}
		if (setTalent.empty()) m_mapRuleTalent.erase(it);
	}
	else
	{
		RemoveTalent(szName);
		if (uPoint!=uTalentPoint)
		{
			bResult = false;
			ostringstream strErr; 
			strErr << szName <<":  "<<uPoint<<"  "<<uTalentPoint<<endl;
			LogErr("要删除天赋点与当前天赋点数不符",strErr.str());
		}
	}
	return bResult;
}
TalentEffData::TalentEffData(CSkillInstServer* pInst)
:m_pInst(pInst)
{
	m_pInst->SetTargetAliveCheckIsIgnored(true);	//天赋放开对目标的死亡检测
	m_pCancelEffData=new CEffDataByGlobalIDMgr;
}

TalentEffData::~TalentEffData()
{
	delete m_pCancelEffData;
	m_pInst->DestroyOrigin();
}

CTalentEffServer::CTalentEffServer(const CFighterMediator* pFighter)
:m_pFighter(const_cast<CFighterMediator*>(pFighter))
{
}
	
CTalentEffServer::~CTalentEffServer()
{
	MapTalentEffData::iterator it_begin = m_mapTalentaEffData.begin();
	MapTalentEffData::iterator it_end = m_mapTalentaEffData.end();
	for (;it_begin!=it_end;++it_begin)
	{
		const string& szName = it_begin->first.first;
		const CTalentEffCfgServerSharedPtr& pCfg = CTalentEffCfgServer::GetTalentEff(szName);
		pCfg->GetMagicEff()->Cancel(it_begin->second->m_pInst, m_pFighter, it_begin->second->m_pCancelEffData->GetEffData(m_pFighter->GetEntityID()));
		delete it_begin->second;
	}
}

void CTalentEffServer::InitClassTalentEff(EClass eClass)
{
	CTalentEffCfgServer::VecTalentEffCfgServer vecTalentEff;
	CTalentEffCfgServer::GetClassTalentEffVec(eClass,vecTalentEff);
	CTalentEffCfgServer::VecTalentEffCfgServer::iterator it = vecTalentEff.begin();
	for(; it != vecTalentEff.end(); ++it)
	{		
		OnTalentAdded((*it)->get()->GetName(),m_pFighter->CppGetLevel());
	}
}

void CTalentEffServer::Update()
{
	SetUpdateTalentEff::iterator it_begin = m_setUpdate.begin();
	SetUpdateTalentEff::iterator it_end = m_setUpdate.end();
	for (;it_begin!=it_end;++it_begin)
	{
		pair<MapTalentEffData::iterator, MapTalentEffData::iterator> it = m_mapTalentaEffData.equal_range(make_pair(it_begin->first,it_begin->second));
		for (; it.first != it.second; ++it.first)
		{
			const CTalentEffCfgServerSharedPtr& pCfg = CTalentEffCfgServer::GetTalentEff(it_begin->first);
			pCfg->GetMagicEff()->Cancel(it.first->second->m_pInst, m_pFighter, it.first->second->m_pCancelEffData->GetEffData(m_pFighter->GetEntityID()));
			delete it.first->second;
			CSkillInstServer* pInst = CSkillInstServer::CreateOrigin(m_pFighter->GetEntityID(),it_begin->first,it_begin->second,eATT_None);
			pInst->SetIsTalent(true);
			TalentEffData* pData = new TalentEffData(pInst);
			pCfg->GetMagicEff()->Do(pInst, m_pFighter, m_pFighter, pData->m_pCancelEffData->AddEffData(m_pFighter->GetEntityID(), m_pFighter));
			it.first->second = pData;
		}
	}
}

void CTalentEffServer::OnTalentChanged(const string& szName, int32 iPoint)
{
	const CTalentEffCfgServerSharedPtr& pCfg = CTalentEffCfgServer::GetTalentEff(szName);
	if(!pCfg) return;
	uint32 uPoint = m_pFighter->GetTalentHolder()->GetPoint(szName);
	OnTalentRemove(szName,uPoint+iPoint);
	OnTalentAdded(szName,uPoint);
}

void CTalentEffServer::OnTalentAdded(const string& szName, uint32 uPoint)
{
	m_pFighter->SyncToClientTalentChange(szName.c_str(),(int32)uPoint);
	const CTalentEffCfgServerSharedPtr& pCfg = CTalentEffCfgServer::GetTalentEff(szName);
	if(!pCfg) return;
	CSkillInstServer* pInst = CSkillInstServer::CreateOrigin(m_pFighter->GetEntityID(),szName,uPoint,eATT_None);
	pInst->SetIsTalent(true);
	TalentEffData* pData = new TalentEffData(pInst);
	pCfg->GetMagicEff()->Do(pInst, m_pFighter, m_pFighter, pData->m_pCancelEffData->AddEffData(m_pFighter->GetEntityID(), m_pFighter));
	m_mapTalentaEffData.insert(make_pair(make_pair(szName,uPoint),pData));
	if (pCfg->IsUpdateWithLevel())
	{
		m_setUpdate.insert(make_pair(szName,uPoint));
	}
}

void CTalentEffServer::OnTalentRemove(const string& szName,uint32 uPoint)
{
	m_pFighter->SyncToClientTalentRemove(szName.c_str());
	MapTalentEffData::iterator it_map = m_mapTalentaEffData.find(make_pair(szName,uPoint));
	if (it_map!=m_mapTalentaEffData.end())
	{
		const CTalentEffCfgServerSharedPtr& pCfg = CTalentEffCfgServer::GetTalentEff(szName);
		pCfg->GetMagicEff()->Cancel(it_map->second->m_pInst, m_pFighter, it_map->second->m_pCancelEffData->GetEffData(m_pFighter->GetEntityID()));
		delete it_map->second;
		m_mapTalentaEffData.erase(it_map);
	}
	SetUpdateTalentEff::iterator it_set = m_setUpdate.find(make_pair(szName,uPoint));
	if (it_set!=m_setUpdate.end())
	{
		m_setUpdate.erase(it_set);
	}
}
