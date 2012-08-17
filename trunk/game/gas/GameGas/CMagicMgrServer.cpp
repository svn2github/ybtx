#include "stdafx.h"
#include "CMagicMgrServer.h"
#include "CFighterDictator.h"
#include "CAureMagicServer.h"
#include "CBattleArrayMagicCfgServer.h"
#include "PtrlGas2GacCPPDef.h"
#include "ErrLogHelper.h"
#include "TSqrAllocator.inl"
#include "CShockWaveMagicServer.h"
#include "CShockWaveMagicCfgServer.h"
#include "CPositionMagicServer.h"
#include "CPositionMagicCfgServer.h"

CMagicMgrServer::CMagicMgrServer(CFighterDictator* pFighter)
:m_pMove(NULL)
,m_pTransfer(NULL)
,m_pPosition(NULL)
,m_pTotem(NULL)
,m_pFighter(pFighter)
,m_pArrayStation(NULL)
,m_uCheckID(0)
{
}

CMagicMgrServer::~CMagicMgrServer(void)
{
	CAureMagicServer::CancelAllAureMagic(GetFighter());
	CAureMagicServer::CancelAllInvisibleAureMagic(GetFighter());
	/*if (!m_mapMagic.empty())
	{
		MapMagicServer_t::iterator it = m_mapMagic.begin();
		for(;it!=m_mapMagic.end();++it)
		{
			ostringstream strm;		
			CMagicServer* pMagic = it->second;
			strm << " Magic " << pMagic->GetCfg()->GetName()<<" 没有正常删除"<<endl;
			LogErr(strm.str().c_str());
		}
	}*/
}


uint32 CMagicMgrServer::AddMagic(CMagicServer* pMagic)
{
	m_mapMagic.insert(make_pair(++m_uCheckID,pMagic));
	return m_uCheckID;
}

void	CMagicMgrServer::RemoveMagic(uint32 uCheckID)
{
	MapMagicServer_t::iterator it = m_mapMagic.find(uCheckID);
	if (it!=m_mapMagic.end())
	{
		m_mapMagic.erase(it);
	}
}

bool	CMagicMgrServer::IsValid(uint32 uCheckID)
{
	MapMagicServer_t::iterator it = m_mapMagic.find(uCheckID);
	if (it!=m_mapMagic.end())
	{
		return true;
	}
	return false;
}

void CMagicMgrServer::OnCastMagicSkill(uint32 uMagicID,EMagicGlobalType eType,bool bLoop)
{
	CGas2GacCPP_OnCastMagicSkill Cmd;
	Cmd.uObjID	= GetFighter()->GetEntityID();
	Cmd.uType	= uint8(eType);
	Cmd.uMagicID = uMagicID;
	Cmd.bLoop = bLoop;
	GetFighter()->SendCmdToGac(&Cmd);
}

void CMagicMgrServer::OnCancelMagicSkill()
{
	CGas2GacCPP_OnCancelMagicSkill Cmd;
	Cmd.uObjID	= GetFighter()->GetEntityID();
	GetFighter()->SendCmdToGac(&Cmd);
}

CShockWaveMagicHandlerServer* CMagicMgrServer::GetShockwaveMagic(const string& strName)const
{
	if(!m_mapMagic.empty())
	{
		MapMagicServer_t::const_iterator it = m_mapMagic.begin();
		for(;it!=m_mapMagic.end();++it)
		{
			CMagicServer* pMagic = it->second;
			if(pMagic->GetMagicType() == eMGT_ShockWave)
			{
				CShockWaveMagicHandlerServer* pShockwaveMagic = static_cast<CShockWaveMagicHandlerServer*>(pMagic);
				if(pShockwaveMagic->GetCfgSharedPtr()->GetName() == strName)
					return pShockwaveMagic;
			}
		}
	}
	return NULL;
}

CPositionMagicServer* CMagicMgrServer::GetPositionMagicByName(const string& strName)const
{
	if (!m_mapMagic.empty())
	{
		MapMagicServer_t::const_iterator it = m_mapMagic.begin();
		for (;it!=m_mapMagic.end();++it)
		{
			CMagicServer* pMagic = it->second;
			if (pMagic->GetMagicType() == eMGT_Position)
			{
				CPositionMagicServer* pPostionMagic = class_cast<CPositionMagicServer*>(pMagic);
				if (pPostionMagic->GetCfgSharedPtr()->GetName() == strName)
					return pPostionMagic;
			}
		}
	}
	return NULL;
}

void CMagicMgrServer::SetArrayName(const string& strArrayName)
{
	CBattleArrayMagicCfgServerSharedPtr& pCfg = CBattleArrayMagicCfgServer::GetBattleArrayMagic(strArrayName);
	if (!pCfg)
	{
		GenErr("在阵法配置表里不存在", strArrayName+" 在阵法配置表里不存在");
	}
	m_strArrayName=strArrayName;
}

