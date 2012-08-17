#include "stdafx.h"
#include "CTeamServerManager.h"
#include "ITeamServerManagerHandler.h"
#include "CAppServer.h"
#include "CCharacterMediator.h"
#include "CFighterDictator.h"
#include "BaseHelper.h"
#include "CConnServer.h"
#include "PtrlGas2GacCPPDef.h"
#include "PtrlGac2GasCPPDef.h"
#include "CSyncVariantServer.h"
#include "FighterProperty.inl"
#include "CTick.h"
#include "TSqrAllocator.inl"

CConnServer* GetConn(uint32 uEntityID)
{
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	CCharacterMediator* pPlayer= pCharacter?pCharacter->CastToCharacterMediator():NULL;
	return pPlayer?pPlayer->GetConnection():NULL;
}

CCharacterMediator* GetCharacterMediator(uint32 uEntityID)
{
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	return pCharacter?pCharacter->CastToCharacterMediator():NULL;
}

struct TempVariantCache
{
	TempVariantCache(CCharacterMediator* pOwner)
	{
		CSyncVariantServer* pVariant = pOwner->GetSyncVariant(eCTI_Static)->GetMember("Res");
		m_varHairResID=pVariant->GetMember("HairResID");
		m_varFaceResID=pVariant->GetMember("FaceResID");
		m_varHeadResID=pVariant->GetMember("HeadIndexID");
		m_varBodyResID=pVariant->GetMember("BodyIndexID");
	}
	~TempVariantCache()
	{
	}
	CSyncVariantServer* m_varHairResID;
	CSyncVariantServer* m_varFaceResID;
	CSyncVariantServer* m_varHeadResID;
	CSyncVariantServer* m_varBodyResID;
};

class CTeamInfoServer : public CTick
{
	typedef vector< pair<CCharacterMediator*,CGas2GacCPP_TeamSyncInfo*>,TFighterAllocator<pair<CCharacterMediator*,CGas2GacCPP_TeamSyncInfo*> >  > VecSyncInfo;
	typedef set<uint32,less<uint32>,TFighterAllocator<uint32> >		SetEntityID;	
	typedef map<uint32,TempVariantCache*,less<uint32>,TFighterAllocator<pair<uint32,TempVariantCache* > > >	MapEntityID;	
public:
	CTeamInfoServer(uint32 uEntityID)
		:m_uTeamMasterEntityID(uEntityID)
		,m_uCount(0)
	{
		AddMember(uEntityID);
		CAppServer::Inst()->RegisterTick(this,1000);
	}
	~CTeamInfoServer()
	{
		CAppServer::Inst()->UnRegisterTick(this);
		for (MapEntityID::iterator it = m_mapEntityID.begin(); it != m_mapEntityID.end(); ++it)
		{
			SafeDelete(it->second);
		}
	}
	void AddMember(uint32 uEntityID)
	{
		CCharacterMediator* pOwner=GetCharacterMediator(uEntityID);
		if (pOwner==NULL)
		{
			return;
		}
		if(m_setEntityID.insert(uEntityID).second)
		{
			TempVariantCache* pCache = new TempVariantCache(pOwner);
			m_mapEntityID.insert(make_pair(uEntityID,pCache));
		}
	}

	void DelMember(uint32 uEntityID)
	{
		MapEntityID::iterator it  = m_mapEntityID.find(uEntityID);
		if(it!=m_mapEntityID.end())
		{
			SafeDelete( it->second);
			m_mapEntityID.erase(it);
		}
		m_setEntityID.erase(uEntityID);
	}

	void ChangeTeamMaster(uint32 uEntityID)
	{
		m_uTeamMasterEntityID = uEntityID;
	}

	const SetEntityID& GetTeam()
	{
		return m_setEntityID;
	}

	uint32 GetTeamMaster()
	{
		return m_uTeamMasterEntityID;
	}
private:
	void OnTick()
	{
		VecSyncInfo vecSyncInfo;
		MakeVecInfo(vecSyncInfo);
		SetEntityID::const_iterator it_owner = m_setEntityID.begin();
		for (;it_owner!=m_setEntityID.end();++it_owner)
		{
			CCharacterMediator* pSyncer=GetCharacterMediator(*it_owner);
			if (pSyncer==NULL)
			{
				continue;
			}
			if (m_uCount==5)
			{
				SendToGac(vecSyncInfo,pSyncer,false);
			}
			else
			{
				SendToGac(vecSyncInfo,pSyncer,true);
			}
		}
		for (VecSyncInfo::iterator it = vecSyncInfo.begin(); it != vecSyncInfo.end(); ++it)
		{
			SafeDelete((*it).second);
		}
		vecSyncInfo.clear();
		if (m_uCount==5)
		{
			m_uCount=0;
		} 
		else
		{
			m_uCount++;
		}
	}
	void MakeVecInfo(VecSyncInfo& vecSyncInfo)
	{
		MapEntityID::const_iterator it_owner = m_mapEntityID.begin();
		for (;it_owner!=m_mapEntityID.end();++it_owner)
		{
			CCharacterMediator* pOwner=GetCharacterMediator(it_owner->first);
			if (pOwner==NULL)
			{
				continue;
			}
			TempVariantCache* pTempVariantCache = it_owner->second;
			CGas2GacCPP_TeamSyncInfo* pInfo =new CGas2GacCPP_TeamSyncInfo;
			pInfo->uEntityID = pOwner->GetEntityID();
			CFighterDictator* pFighter = pOwner->GetFighter();
			pInfo->uHP=pFighter->m_HealthPoint.LimitGet();
			pInfo->uMP=pFighter->m_ManaPoint.LimitGet();
			pInfo->uEP=pFighter->m_EnergyPoint.LimitGet();
			pInfo->uRP=pFighter->m_RagePoint.LimitGet();
			pInfo->uBaseHP=pFighter->m_HealthPoint.Get(pFighter);
			pInfo->uBaseMP=pFighter->m_ManaPoint.Get(pFighter);
			pInfo->uBaseEP=pFighter->m_EnergyPoint.Get(pFighter);
			pInfo->uBaseRP=pFighter->m_RagePoint.Get(pFighter);
			pInfo->uLevel = pOwner->CppGetLevel();
			pInfo->uHairResID=pTempVariantCache->m_varHairResID->GetNumber<uint8>();
			pInfo->uFaceResID=pTempVariantCache->m_varFaceResID->GetNumber<uint8>();
			pInfo->uHeadIndexID=pTempVariantCache->m_varHeadResID->GetNumber<uint32>();
			pInfo->uBodyIndexID=pTempVariantCache->m_varBodyResID->GetNumber<uint32>();
			pInfo->fPos = pOwner->GetPixelPos();
			pInfo->bInBattle = pFighter->GetCtrlState(eFCS_InBattle);
			vecSyncInfo.push_back(make_pair(pOwner,pInfo));
		}
	}
	void SendToGac(VecSyncInfo& vecSyncInfo,CCharacterMediator* pSyncer,bool bIsSameArea)
	{
		VecSyncInfo::const_iterator it = vecSyncInfo.begin();
		for (;it!=vecSyncInfo.end();++it)
		{
			CCharacterMediator* pOwner = (*it).first;
			if (pSyncer==pOwner||pSyncer->IsInPlayerSet(pOwner)||(bIsSameArea&&pSyncer->GetScene()!=pOwner->GetScene()))
			{
				continue;
			}
			CConnServer* pConn = pSyncer->GetConnection();
			if (pConn)
			{
				pConn->Send(((*it).second),sizeof(CGas2GacCPP_TeamSyncInfo));
			}
		}
	}
	MapEntityID m_mapEntityID;
	SetEntityID m_setEntityID;
	uint32 m_uTeamMasterEntityID;
	uint32 m_uCount;
};

CTeamServerManager::~CTeamServerManager()
{
	for (MapTeamMgr::iterator it = m_mapTeamManager.begin(); it != m_mapTeamManager.end(); ++it)
	{
		SafeDelete(it->second);
	}
}

void CTeamServerManager::AddTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
{
	MapTeamMgr::iterator it = m_mapTeamManager.find(make_pair(uTeamID,eTeamType));
	if (it!=m_mapTeamManager.end())
	{
		CTeamInfoServer* pTeamInfo=it->second;
		pTeamInfo->AddMember(uEntityID);
		SetHandler::iterator it=m_setHandler.begin();
		for (;it!=m_setHandler.end();++it)
		{
			(*it)->OnAddTeamMember(uTeamID,eTeamType,uEntityID);
		}
	}
	else
	{
		 CreateTeam(uTeamID,eTeamType,uEntityID);
	}
}

void CTeamServerManager::RemoveTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
{
	MapTeamMgr::iterator it = m_mapTeamManager.find(make_pair(uTeamID,eTeamType));
	if (it!=m_mapTeamManager.end())
	{
		CTeamInfoServer* pTeamInfo=it->second;
		pTeamInfo->DelMember(uEntityID);
	
		const SetEntityID setTeam= CppGetTeamMember(uTeamID,eTeamType);
		SetEntityID::const_iterator it_owner = setTeam.begin();
		for (;it_owner!=setTeam.end();++it_owner)
		{
			CGas2GacCPP_RemoveTeamMember Cmd;
			Cmd.uEntityID=uEntityID;
			CConnServer* pConn = GetConn(*it_owner);
			if (pConn)
			{
				pConn->Send(&Cmd,sizeof(Cmd));
			}
		}
		if (pTeamInfo->GetTeam().empty())
		{
			DestroyTeam(uTeamID,eTeamType);
		}
	}
}

void CTeamServerManager::CreateTeam(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
{
	MapTeamMgr::iterator it = m_mapTeamManager.find(make_pair(uTeamID,eTeamType));
	if (it!=m_mapTeamManager.end())
	{
		return;
	}
	CTeamInfoServer* pTeamInfo=new CTeamInfoServer(uEntityID);
	m_mapTeamManager.insert(make_pair(make_pair(uTeamID,eTeamType),pTeamInfo));
	if (!m_setHandler.empty())
	{
		SetHandler::iterator it=m_setHandler.begin();
		for (;it!=m_setHandler.end();++it)
		{
			(*it)->OnCreateTeam(uTeamID,eTeamType,uEntityID);
		}
	}
	return;
}

void CTeamServerManager::DestroyTeam(uint32 uTeamID,ETeamType eTeamType)
{
	MapTeamMgr::iterator it = m_mapTeamManager.find(make_pair(uTeamID,eTeamType));
	if (it!=m_mapTeamManager.end())
	{
		CTeamInfoServer* pTeamInfo=it->second;
		delete pTeamInfo;
		m_mapTeamManager.erase(it);
		if (!m_setHandler.empty())
		{
			SetHandler::iterator it=m_setHandler.begin();
			for (;it!=m_setHandler.end();++it)
			{
				(*it)->OnDestroyTeam(uTeamID,eTeamType);
			}
		}


	}
}

void CTeamServerManager::ChangeTeamMaster(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
{
	MapTeamMgr::iterator it = m_mapTeamManager.find(make_pair(uTeamID,eTeamType));
	if (it!=m_mapTeamManager.end())
	{
		CTeamInfoServer* pTeamInfo=it->second;
		pTeamInfo->ChangeTeamMaster(uEntityID);
		if (!m_setHandler.empty())
		{
			SetHandler::iterator it=m_setHandler.begin();
			for (;it!=m_setHandler.end();++it)
			{
				(*it)->OnChangeTeamMaster(uTeamID,eTeamType,uEntityID);
			}
		}
	}
}

uint32 CTeamServerManager::GetTeamMaster(uint32 uTeamID,ETeamType eTeamType)const
{
	MapTeamMgr::const_iterator it = m_mapTeamManager.find(make_pair(uTeamID,eTeamType));
	if (it!=m_mapTeamManager.end())
	{
		return it->second->GetTeamMaster();
	}
	return 0;
}

const CTeamServerManager::SetEntityID& CTeamServerManager::GetTeamMember(uint32 uTeamID,ETeamType eTeamType)const
{
	static SetEntityID setEmpty;
	MapTeamMgr::const_iterator it = m_mapTeamManager.find(make_pair(uTeamID,eTeamType));
	if (it!=m_mapTeamManager.end())
	{
		return it->second->GetTeam();
	}
	return setEmpty;
}

void CTeamServerManager::AddHandler(ITeamServerManagerHandler* pHandler)
{
	m_setHandler.insert(pHandler);
}

void CTeamServerManager::RemoveHander(ITeamServerManagerHandler* pHandler)
{
	m_setHandler.erase(pHandler);
}

