#include "stdafx.h"
#include "LoadSkillCommon.h"
#include "CTxtTableFile.h"
#include "CMoveMagicClient.h"
#include "CFighterFollower.h"
#include "CCharacterFollower.h"
#include "CoreCommon.h"
#include "CCoreSceneClient.h"
#include "CRenderSystemClient.h"
#include "CSceneManagerClient.h"
#include "CPixelPath.h"
#include "FXPlayer.h"
#include "StringHelper.h"
#include "CMoveMagicCfg.h"

#define UPDATE_TRANK_TIME	20
#define GRAVITY_OF_JUMP		1000

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CMoveMagicCfgClient::MapMoveMagicCfgClientByID CMoveMagicCfgClient::m_mapCfgByID;
CMoveMagicCfg::MapActionType	CMoveMagicCfg::ms_mapActionType;

bool CMoveMagicCfgClient::LoadConfig(const TCHAR* cfgFile)
{
	CTxtTableFile TabFile;
	CMoveMagicCfgClient*	pCfgNode;

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;

	ClearMap(m_mapCfgByID);
	CMoveMagicCfg::InitMapActionType();

	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		pCfgNode = new CMoveMagicCfgClient;
		pCfgNode->m_uID = i;
		pCfgNode->m_fSpeed = TabFile.GetFloat(i, szMagic_Speed, 0) * eGridSpanForObj;
		pCfgNode->m_eActionType = CMoveMagicCfg::ms_mapActionType[TabFile.GetString(i, szMagic_ActionType)];
		pCfgNode->m_strStartAct = TabFile.GetString(i, szMagic_StartActionName);
		pCfgNode->m_strStartFx = TabFile.GetString(i, szMagic_StartFxName);
		pCfgNode->m_strProcessAct = TabFile.GetString(i, szMagic_ProcessActionName);
		pCfgNode->m_strProcessFx = TabFile.GetString(i, szMagic_ProcessFxName);
		pCfgNode->m_strEndAct =TabFile.GetString(i, szMagic_EndActionName);
		pCfgNode->m_strEndFx = TabFile.GetString(i, szMagic_EndFxName);
		trimend(pCfgNode->m_strStartFx);
		trimend(pCfgNode->m_strStartAct);
		trimend(pCfgNode->m_strProcessFx);
		trimend(pCfgNode->m_strProcessAct);
		trimend(pCfgNode->m_strEndFx);
		trimend(pCfgNode->m_strEndAct);

		CMoveMagicCfgClientSharedPtr* pCfgNodeSharedPtr = new CMoveMagicCfgClientSharedPtr(pCfgNode);
		m_mapCfgByID.insert(make_pair(pCfgNode->m_uID, pCfgNodeSharedPtr));
	}
	return true;
}

void CMoveMagicCfgClient::UnloadConfig()
{
	ClearMap(m_mapCfgByID);
}

CMoveMagicCfgClientSharedPtr& CMoveMagicCfgClient::GetByID(uint32 uID)
{
	stringstream ExpStr;
	CMoveMagicCfgClient::MapMoveMagicCfgClientByID::iterator mapCfgItr;
	mapCfgItr = m_mapCfgByID.find(uID);
	if (mapCfgItr == m_mapCfgByID.end())
	{
		ExpStr << "位移魔法" << uID << "不存在";
		GenErr(ExpStr.str());
		//return NULL;
	}
	return *(mapCfgItr->second);
}

CMoveMagicClient::CMoveMagicClient(CFighterFollower* pFighter, const CMoveMagicCfgClientSharedPtr& pCfg)
	: m_pFighter(pFighter)
	, m_pCfg(pCfg)
	, m_vSrcPosition(0.0f, 0.0f, 0.0f)
	, m_vCurPosition(0.0f, 0.0f, 0.0f)
	, m_vDstPosition(0.0f, 0.0f, 0.0f)
	, m_uPassTime(UPDATE_TRANK_TIME)
	, m_fAllTime(0.0f)
	, m_fHeightSrcSpeed(0.0f)
	, m_bIsMove(false)
	, m_bPlayEndAni(false)
{
	if (!m_pFighter->GetCharacter()->CoreObjisExist())
		delete this;

	m_pFighter->Attach(this, eCE_Offline);
	m_pFighter->Attach(this, eCE_ChangeMapEnd);
	m_pFighter->Attach(this, eCE_Transport);
	m_pFighter->Attach(this, eCE_Die);
	m_pFighter->Attach(this, eCE_MoveSuccess);
	m_pFighter->Attach(this, eCE_MoveStop);
	m_pFighter->Attach(this, eCE_MoveBegan);
	m_pFighter->Attach(this, eCE_MovePathChange);
	m_pFighter->Attach(this, eCE_OnAniEnd);

}

CMoveMagicClient::~CMoveMagicClient()
{
	CRenderObject* pRenderObj = m_pFighter->GetCharacter()->GetHorseRenderObj();

	if (pRenderObj)
	{
		m_pFighter->GetCharacter()->UnRegistDistortedTick(this);
		CFPos PixelPos = m_pFighter->GetCharacter()->GetPixelPos();
		float fRatio = eGridSpanForObj / eGridSpan;
		Ast( CClientRenderScene::HasInst());
		CRenderScene* pRenderScene = CClientRenderScene::GetInst();
		pRenderScene->SetRenderObjectPosition(*pRenderObj,CVector3f(PixelPos.x / fRatio, 0, PixelPos.y / fRatio),m_pFighter->GetCharacter()->CanBeOnTheTerrain());
	}
	m_pFighter->GetCharacter()->SetUpdateRenderObjPos(true);
	m_pFighter->GetCharacter()->GetActor()->SetCurOnceActionLevel(eAPP_NULL);
	m_pFighter->SetTargetPos(m_pFighter->GetCharacter()->GetPixelPos());
}

void CMoveMagicClient::OnTick()
{
	CRenderObject* pRenderObj = m_pFighter->GetCharacter()->GetHorseRenderObj();
	if (!m_pFighter->GetCharacter() || (float)m_uPassTime >= m_fAllTime*1000)
	{
		delete this;
		return;
	}

	CFPos posPixel = m_pFighter->GetCharacter()->GetPixelPos();
	m_vCurPosition.x = posPixel.x;
	m_vCurPosition.z = posPixel.y;
	m_vCurPosition.y = m_vSrcPosition.y + m_fHeightSrcSpeed * (float(m_uPassTime)/1000) - GRAVITY_OF_JUMP * (float(m_uPassTime)/1000) * (float(m_uPassTime)/1000)/2;
	pRenderObj->SetPosition(m_vCurPosition);
	m_uPassTime += UPDATE_TRANK_TIME;

	if (!m_bPlayEndAni && (m_fAllTime*1000 - m_uPassTime) < 200)
	{
		string strEndAction = m_pCfg->GetEndAct();
		string strEndEffect = m_pCfg->GetEndFx();

		CCharacterFollower* pCharacter = m_pFighter->GetCharacter();
		if(!strEndAction.empty())
		{
			pCharacter->GetActor()->DoActionState(eAS_MoveEnd);
		}

		if(!strEndEffect.empty())
		{
			string strFXFile;
			string strFXName;
			pCharacter->GetActor()->SplitSkillFxFile(strEndEffect,strFXFile,strFXName,true);
			if (!strFXFile.empty())
			{
				CPlayerFX::PlayOnceFX(pCharacter,strFXFile, strFXName);
			}
		}

		m_bPlayEndAni = true;
	}
}

void CMoveMagicClient::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	if (pSubject == m_pFighter)
	{
		switch (uEvent)
		{
		case eCE_MoveBegan:
		case eCE_MovePathChange:
			{
				if (!m_pFighter->GetCharacter()->IsPassive())
				{
					delete this;
					break;
				}

				CCharacterFollower* pCharacter = m_pFighter->GetCharacter();
				CPixelPath* pPath = pCharacter->GetClonedMovePath();
				CCoreSceneClient* pCoreScene = CCoreSceneClient::MainScene();

				if (pCharacter)
				{
					string strStartAction = m_pCfg->GetStartAct();
					string strStartEffect = m_pCfg->GetStartFx();

					if(!strStartAction.empty())
					{
						pCharacter->GetActor()->DoActionState(eAS_MoveStart);
					}

					if(!strStartEffect.empty())
					{
						string strFXFile,strFXName;
						pCharacter->GetActor()->SplitSkillFxFile(strStartEffect,strFXFile,strFXName,true);
						if (!strFXFile.empty())
						{
							CPlayerFX::PlayOnceFX(pCharacter,strFXFile, strFXName);
						}
					}
				}

				if (!pPath)
				{
					break;
				}

				CFPos posPixel = pCharacter->GetPixelPos();
				CFPos DesPos; 
				pPath->GetEndPixelPos(DesPos);
				pPath->Release();
				m_vCurPosition.x = m_vSrcPosition.x = posPixel.x;
				m_vCurPosition.z = m_vSrcPosition.z = posPixel.y;	
				m_vCurPosition.y = m_vSrcPosition.y = pCoreScene->GetRenderScene()->GetDataScene()->GetOffsetHeightInPixel(m_vSrcPosition.x, m_vSrcPosition.z);

				m_vDstPosition.x = DesPos.x;
				m_vDstPosition.z = DesPos.y;
				m_vDstPosition.y = pCoreScene->GetRenderScene()->GetDataScene()->GetOffsetHeightInPixel(m_vDstPosition.x, m_vDstPosition.z);
				m_fAllTime = posPixel.Dist(DesPos) / m_pCfg->GetSpeed();
				m_fHeightSrcSpeed = (m_vDstPosition.y - m_vCurPosition.y + GRAVITY_OF_JUMP * m_fAllTime * m_fAllTime/2) / m_fAllTime;

				pCharacter->RegistDistortedTick(this, UPDATE_TRANK_TIME);
				pCharacter->SetUpdateRenderObjPos(false);
				m_bIsMove = true;
			}
			break;
		case eCE_OnAniEnd:
			{
				string strProcessAction = m_pCfg->GetProcessAct();
				string strProcessEffect = m_pCfg->GetProcessFx();

				CCharacterFollower* pCharacter = m_pFighter->GetCharacter();
				if(!strProcessAction.empty())
				{
					pCharacter->GetActor()->DoActionState(eAS_MoveProcess);
				}

				if(!strProcessEffect.empty())
				{
					string strFXFile,strFXName;
					pCharacter->GetActor()->SplitSkillFxFile(strProcessEffect,strFXFile,strFXName,true);
					if (!strFXFile.empty())
					{
						CPlayerFX::PlayOnceFX(pCharacter,strFXFile, strFXName);
					}
				}
			}
			break;
		case eCE_MoveSuccess:
		case eCE_Offline:
		case eCE_ChangeMapEnd:
		case eCE_Transport:
		case eCE_Die:
		case eCE_MoveStop:
			if(m_bIsMove)delete this;
			break;
		}
	}
}
