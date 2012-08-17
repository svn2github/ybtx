#include "stdafx.h"
#include "CBattleArrayMagicServer.h"
#include "CBattleArrayMagicCfgServer.h"
#include "CMagicServer.inl"
#include "CFighterDictator.h"
#include "CCoreSceneServer.h"
#include "CCharacterDictator.h"
#include "CMagicEffServer.h"
#include "CoreCommon.h"
#include "CFighterMediator.h"
#include "CMagicMgrServer.h"
#include "PtrlGas2GacCPPDef.h"
#include "CCoreObjectDictator.h"
#include "FighterProperty.inl"
#include "BaseHelper.h"
#include "CSyncVariantServerHolder.h"
#include "DebugHelper.h"
#include "TSqrAllocator.inl"
#include "StringHelper.h"

#define MAX_BATTLEARRAY		5
#define MAKEARRAYSPEED_1	45
#define MAKEARRAYSPEED_2	50
#define MAKEARRAYSPEED_3	57
#define MAKEARRAYSPEED_4	63
#define MAKEARRAYSPEED_5	71

CBattleArrayMagicServer::VecMakeArray_t CBattleArrayMagicServer::ms_vecMakeArraySpeed;

CBattleArrayStationServer::CBattleArrayStationServer(CBattleArrayMagicServer* pArrayMgr,uint32 uID, uint32 uEntityID,const CFPos& pos)
:CMagicServer(pArrayMgr->m_pInst,pArrayMgr->m_pFrom,0)
,m_pArrayMgr(pArrayMgr)
,m_uID(uID)
,m_uEntityID(uEntityID)
,m_Pos(pos)
,m_bInStation(false)
,m_eBattleArrayState(eBAS_OutArray)
{
	CreateCoreObj<CBattleArrayStationServer>(pos);
	SetEyeSight(1);
	CSyncVariantServerHolder* pViewRootVariant = GetCoreObj()->GetViewVariantHolder(eCTI_Static);
	pViewRootVariant->SetNumber<uint8>("Attribute/ArrayStationID", uID);
	CCharacterDictator* pCharacter=CCharacterDictator::GetCharacterByID(m_uEntityID);
	m_pArrayMgr->SetOutArrayStateCount(++m_pArrayMgr->m_uOutArrayStateCount);
	if (NULL==pCharacter||pCharacter->GetGameEntityType()!=eGET_CharacterMediator)
	{
		return;
	}
	CFighterMediator* pFighter = pCharacter->GetFighter()->CastToFighterMediator();
	CMagicMgrServer* pMagicMgr = pFighter->GetMagicMgr();
	pMagicMgr->SetArrayStation(this);
	pFighter->Attach(this,eCE_Die);
	pFighter->Attach(this,eCE_Offline);
	pFighter->Attach(this,eCE_Reborn);
	if (pCharacter->GetScene()==m_pArrayMgr->m_pScene)
	{
		ChangeArrayState(eBAS_ReturnArray);
	}
	else
	{
		ChangeArrayState(eBAS_OutArray);
	}
}

CBattleArrayStationServer::~CBattleArrayStationServer()
{
	CCoreObjectDictator* pObj = GetCoreObj();
	if (pObj)
	{
		pObj->SetHandler(NULL);
		m_pArrayMgr->m_pScene->DestroyObject(pObj);
	}
	CCharacterDictator* pCharacter=CCharacterDictator::GetCharacterByID(m_uEntityID);
	if (pCharacter)
	{
		if (pCharacter->IsMoving())
		{
			pCharacter->StopMoving();
		}
		if (NULL==pCharacter&&pCharacter->GetGameEntityType()!=eGET_CharacterMediator)
		{
			return;
		}
		CFighterMediator* pFighter = pCharacter->GetFighter()->CastToFighterMediator();
		CMagicMgrServer* pMagicMgr = pFighter->GetMagicMgr();
		pMagicMgr->SetArrayStation(NULL);
	}
}

void	CBattleArrayStationServer::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if (!pCoreObj||!pCoreObj->GetHandler()) return;
	size_t uType = (size_t)(pCoreObj->GetHandler()->GetTag());
	if(uType == (uint32)(eCOHT_Character))
	{
		CCharacterDictator* pCharacter =CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
		if (!pCharacter) return;
		if (m_uEntityID!=0&&pCharacter->GetEntityID()==m_uEntityID)
		{
			m_bInStation=true;
			ChangeArrayState(eBAS_InArray);
		}
	}
}

void	CBattleArrayStationServer::OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if (!pCoreObj||!pCoreObj->GetHandler()) return;
	size_t uType = (size_t)(pCoreObj->GetHandler()->GetTag());
	if(uType == (uint32)(eCOHT_Character))
	{
		CCharacterDictator* pCharacter =CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
		if (!pCharacter) return;
		if (m_uEntityID!=0&&pCharacter->GetEntityID()==m_uEntityID)
		{
			m_bInStation=false;
			ChangeArrayState(eBAS_OutArray);
		}
	}
}


void CBattleArrayStationServer::ChangeArrayState(EBattleArrayState eState)
{
	CCharacterDictator* pCharacter=CCharacterDictator::GetCharacterByID(m_uEntityID);
	if (NULL==pCharacter||m_eBattleArrayState==eState)
	{
		return;
	}
	EBattleArrayState eArrayState=m_eBattleArrayState;
	m_eBattleArrayState=eState;
	switch (eState)
	{
	case eBAS_InArray:
		if (m_pArrayMgr->m_uOutArrayStateCount>0&&eArrayState==eBAS_OutArray)
		{
			m_pArrayMgr->SetOutArrayStateCount(--m_pArrayMgr->m_uOutArrayStateCount);
		}
		m_pArrayMgr->SetInStationCount(++m_pArrayMgr->m_uInStationCount);
		CMagicEffServer::GetMagicEff("压阵")->Do(m_pSkillInst,pCharacter->GetFighter(),pCharacter->GetFighter());
		break;
	case eBAS_OutArray:
		{
			DetachAll();
			CMagicEffServer::GetMagicEff("离阵")->Do(m_pSkillInst,pCharacter->GetFighter(),pCharacter->GetFighter());
			uint8 uOutArrayStateCount=++m_pArrayMgr->m_uOutArrayStateCount;
			m_pArrayMgr->SetOutArrayStateCount(uOutArrayStateCount);
			if (uOutArrayStateCount==MAX_BATTLEARRAY)
			{
				return;
			}
			if (m_pArrayMgr->m_uInStationCount>0)
			{
				m_pArrayMgr->SetInStationCount(--m_pArrayMgr->m_uInStationCount);
			}	
		}
		break;
	case eBAS_ReturnArray:
		if (pCharacter->GetScene()==m_pArrayMgr->m_pScene)
		{
			if (m_pArrayMgr->m_uOutArrayStateCount>0&&eArrayState==eBAS_OutArray)
			{
				m_pArrayMgr->SetOutArrayStateCount(--m_pArrayMgr->m_uOutArrayStateCount);
			}
			pCharacter->GetFighter()->Attach(this,eCE_ChangeMapEnd);
			pCharacter->GetFighter()->Attach(this,eCE_CancelCoerceMove);
			CMagicEffServer::GetMagicEff("归阵")->Do(m_pSkillInst,pCharacter->GetFighter(),pCharacter->GetFighter());
			CFPos CurPos = pCharacter->GetPixelPos();
			int32 iMaxStep =(int32) (abs(CurPos.x - m_Pos.x) + abs(CurPos.y - m_Pos.y))/eGridSpanForObj;
			 iMaxStep = iMaxStep > 5 ? iMaxStep : 5 ;
			EMoveToResult eResult=pCharacter->MoveToInAStarPath(m_Pos,pCharacter->GetFighter()->m_RunSpeed.Get(pCharacter->GetFighter()),eFPT_AStar,eBT_LowBarrier,0.0f,iMaxStep);
			if (eResult!=eMTR_Success)
			{
				ChangeArrayState(eBAS_OutArray);
			}
		}
		break;
	case eBAS_FinishArray:
		FinishArray();
		break;
	default:
		break;
	}
	NotifyClientArrayChange();
}

void CBattleArrayStationServer::FinishArray()
{
	CCharacterDictator* pCharacter=CCharacterDictator::GetCharacterByID(m_uEntityID);
	if (pCharacter&&pCharacter->CastToCharacterMediator())
	{
		string strSkillName=GetCfgSharedPtr()->GetPosSkill(m_uID-1);
		if (NULL==pCharacter&&pCharacter->GetGameEntityType()!=eGET_CharacterMediator)
		{
			return;
		}
		CFighterMediator* pFighter = pCharacter->GetFighter()->CastToFighterMediator();
		if (!strSkillName.empty())
		{
			pFighter->AddTempSkill(strSkillName,m_pArrayMgr->CalcArrayLevel());
		}
	}
}

void CBattleArrayStationServer::NotifyClientArrayChange()
{
	CSyncVariantServerHolder* pViewRootVariant = GetCoreObj()->GetViewVariantHolder(eCTI_Static);
	pViewRootVariant->SetNumber<uint8>("Attribute/ArrayState", m_eBattleArrayState);
}

void CBattleArrayStationServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CCharacterDictator* pCharacter=CCharacterDictator::GetCharacterByID(m_uEntityID);
	if(pCharacter&&pSubject == pCharacter->GetFighter())
	{
		switch(uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
		case eCE_ChangeMapEnd:
			ChangeArrayState(eBAS_OutArray);
			break;
		case eCE_Reborn:
			if (m_bInStation)
			{
				ChangeArrayState(eBAS_InArray);
			}
			else
			{
				ChangeArrayState(eBAS_OutArray);
			}
			break;
		case eCE_CancelCoerceMove:
			{
				if (m_eBattleArrayState==eBAS_ReturnArray)
				{
					ChangeArrayState(eBAS_OutArray);
				}
				if (pCharacter->IsMoving())
				{
					pCharacter->StopMoving();
				}
			}
			break;
		default:
			break;
		}
	}
}

CBattleArrayMagicCfgServerSharedPtr& CBattleArrayStationServer::GetCfgSharedPtr()const
{
	return m_pArrayMgr->GetCfgSharedPtr();
}

CBattleArrayMagicServer::CBattleArrayMagicServer(CSkillInstServer* pInst, const CBattleArrayMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom,  const VecBattleArray_t& vecBattleArray)
:m_pInst(pInst)
,m_pCfg(new CBattleArrayMagicCfgServerSharedPtr(pCfg))
,m_pFrom(pFrom)
,m_pScene(pFrom->GetScene())
,m_uInStationCount(0)
,m_uOutArrayStateCount(0)
,m_iValue(GetCfgSharedPtr()->GetMaxAddUpValue())
,m_uValue(GetCfgSharedPtr()->GetMaxAddUpValue())
,m_vecBattleArray(vecBattleArray)
{
	uint8 uCharacterCount=0;
	VecBattleArray_t::const_iterator it=vecBattleArray.begin();
	for (;it!=vecBattleArray.end();++it)
	{
		if ((*it).first==0)
		{
			++uCharacterCount;
		}
	}
	if (uCharacterCount==MAX_BATTLEARRAY)
	{
		delete this;
		return;
	}
	for(uint8 i=1;i<=MAX_BATTLEARRAY;++i)
	{
		m_vecBAStation.push_back(new CBattleArrayStationServer(this,i,m_vecBattleArray[i-1].first,m_vecBattleArray[i-1].second));
	}
	ms_vecMakeArraySpeed.resize(MAX_BATTLEARRAY+1);
	ms_vecMakeArraySpeed[0]=0;
	ms_vecMakeArraySpeed[1]=MAKEARRAYSPEED_1;
	ms_vecMakeArraySpeed[2]=MAKEARRAYSPEED_2;
	ms_vecMakeArraySpeed[3]=MAKEARRAYSPEED_3;
	ms_vecMakeArraySpeed[4]=MAKEARRAYSPEED_4;
	ms_vecMakeArraySpeed[5]=MAKEARRAYSPEED_5;
}

void CBattleArrayMagicServer::SetInStationCount(sqr::uint8 uCount)
{
	m_uInStationCount=uCount;

	if (m_uInStationCount==1)
	{
		m_pFrom->RegistDistortedTick(this, 1000);
	}
	else if(m_uInStationCount==0)
	{
		m_pFrom->UnRegistDistortedTick(this);
	}
}

void CBattleArrayMagicServer::SetOutArrayStateCount(uint8 uCount)
{
	m_uOutArrayStateCount=uCount;
	if (m_uOutArrayStateCount==MAX_BATTLEARRAY)
	{
		SetInStationCount(0);
		m_iValue=-1000;
		m_uValue=0;
		NotifyClientArrayValueChange();
		delete this;
	}
}

void CBattleArrayMagicServer::OnTick()
{
	m_iValue-=ms_vecMakeArraySpeed[m_uInStationCount];
	NotifyClientArrayValueChange();
	if (m_iValue<=0&&m_iValue>-1000)
	{
		m_pFrom->UnRegistDistortedTick(this);
		VecBAStation_t::iterator it=m_vecBAStation.begin();
		for (;it!=m_vecBAStation.end();++it)
		{
			(*it)->ChangeArrayState(eBAS_FinishArray);
		}
		delete this;
	}
}

void CBattleArrayMagicServer::OnRemoveTeamMember(sqr::uint32 uTeamID, ETeamType eTeamType, uint32 uEntityID)
{
	VecBAStation_t::iterator it=m_vecBAStation.begin();
	for (;it!=m_vecBAStation.end();++it)
	{
		if ((*it)->m_uEntityID==uEntityID)
		{
			(*it)->NotifyClientArrayChange();
			(*it)->m_uEntityID=0;
			return;
		}
	}
}

void CBattleArrayMagicServer::NotifyClientArrayValueChange()
{
	VecBAStation_t::iterator it=m_vecBAStation.begin();
	for (;it!=m_vecBAStation.end();++it)
	{
		if (((*it)->m_uEntityID)!=0)
		{
			CGas2GacCPP_OnArrayValueChange Cmd;
			Cmd.uObjID=(*it)->m_uEntityID;
			Cmd.uTotalValue=m_uValue;
			Cmd.uCurrentValue=max(m_iValue,0);
			Cmd.uSpeed=ms_vecMakeArraySpeed[m_uInStationCount];
			Cmd.udbNameLen = GetCfgSharedPtr()->GetName().size();
			CCharacterDictator* pCharacter=CCharacterDictator::GetCharacterByID((*it)->m_uEntityID);
			if (pCharacter)
			{
				CConnServer* pConn= pCharacter->GetConnection();
				if( pConn)
				{
					pConn->Send(&Cmd,sizeof(Cmd));
					pConn->Send(GetCfgSharedPtr()->GetName().c_str(), Cmd.udbNameLen);
				}
			}
		}
	}
}

CBattleArrayMagicServer::~CBattleArrayMagicServer()
{	
	m_pFrom->UnRegistDistortedTick(this);
	VecBAStation_t::iterator it=m_vecBAStation.begin();
	for (;it!=m_vecBAStation.end();++it)
	{
		delete *it;
	}
	SafeDelete(m_pCfg);
}

void CBattleArrayMagicServer::Transfer(const string& strPos,const string& strEntityID,VecBattleArray_t& vecBattleArray)
{
	vector<string> vecstrPos,vecstrEntityID;
	string split=",";
	sqr::split(strPos,split,vecstrPos);
	sqr::split(strEntityID,split,vecstrEntityID);
	if (vecstrPos.size()!=vecstrEntityID.size())
	{
		cout << "阵法位置数据与EntityID数据个数不匹配\n";
		return;
	}
	vector<string>::iterator it_pos=vecstrPos.begin();
	vector<string>::iterator it_ID=vecstrEntityID.begin();
	for (;it_pos!=vecstrPos.end()&&it_ID!=vecstrEntityID.end();++it_pos,++it_ID)
	{
		vector<string> vecsplitstrPos;
		sqr::split_fix_size(*it_pos,1,vecsplitstrPos);
		CFPos pos;
		if (vecsplitstrPos.size()==1)
		{
			pos.x=0.0f;
			pos.y=GetPixelOfGrid(sqr::integer(vecsplitstrPos[0]));
		}
		else if (vecsplitstrPos.size()==2)
		{
			pos.x=GetPixelOfGrid(sqr::integer(vecsplitstrPos[0]));
			pos.y=GetPixelOfGrid(sqr::integer(vecsplitstrPos[1]));
		}
		vecBattleArray.push_back(make_pair(sqr::integer(*it_ID),pos));
	}
}

void CBattleArrayMagicServer::PositionTransfer(VecBattleArray_t& vecBattleArray,const CFPos& pos)
{
	CFPos PixelPos;
	PixelPos.x=pos.x;
	PixelPos.y=pos.y+5*eGridSpanForObj;
	VecBattleArray_t::iterator it= vecBattleArray.begin();
	for (;it!=vecBattleArray.end();++it)
	{
		float fSin=0,fCos=0,fSinFirst=0,fCosFirst=0,fDistance=0,fPosX=0,fPosY=0;
		fPosX=it->second.x;
		fPosY=it->second.y;
		fDistance =sqrt(fPosX*fPosX+fPosY*fPosY);
		if (fDistance!=0.0f)
		{
			fSinFirst=fPosY/fDistance;
			fCosFirst=fPosX/fDistance;
		}
		fSin = fSinFirst*(0.707f) +fCosFirst*(0.707f);
		fCos = fCosFirst*(0.707f) - fSinFirst*(0.707f);
		(*it).second.x=PixelPos.x - fDistance*fCos;
		(*it).second.y=PixelPos.y - fDistance*fSin;
	}
}

uint32 CBattleArrayMagicServer::CalcArrayLevel()
{
	uint32 uLevel=0;
	VecBAStation_t::iterator it=m_vecBAStation.begin();
	for (;it!=m_vecBAStation.end();++it)
	{
		CCharacterDictator* pCharacter=CCharacterDictator::GetCharacterByID((*it)->m_uEntityID);
		if (pCharacter)
		{
			uLevel+=pCharacter->CppGetLevel();
		}
	}
	return uLevel;
}

CBattleArrayMagicCfgServerSharedPtr& CBattleArrayMagicServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
