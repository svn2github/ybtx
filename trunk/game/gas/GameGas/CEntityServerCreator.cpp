#include "stdafx.h"
#include "CEntityServerCreator.h"
#include "CCharacterDictator.h"
#include "CCharacterMediator.h"
#include "CCoreSceneServer.h"
#include "CMetaSceneServer.h"
#include "CGlobalObjVarDefCfg.h"
#include "TCharacterAdapterServer.h" 
#include "CCoreObjectDictator.h"
#include "CFighterMediator.h"
#include "ICharacterDictatorHandler.h"
#include "IActorServerHandler.h"
#include "ICharacterMediatorHandler.h"
#include "IFighterServerHandler.h"
#include "CEntityServer.h"
#include "CIntObjServer.h"
#include "CActorServer.h"
#include "CActorIntObj.h"
#include "CEntityServerManager.h"
#include "CoreCommon.h"
#include "ErrLogHelper.h"
#include "CSyncVariantServerHolder.h"
#include "CCoreObjectMediator.h"
#include "ErrLogHelper.h"
#include "BaseHelper.h"

CCharacterDictator* CEntityServerCreator::CreateCharacterDictator(CCoreSceneServer *pScene, const CFPos* pPixelPos, const TCHAR* szVarCfgName, const char* szName, bool IsIgnoreClog)
{
	CCharacterDictator* pCharacter = NULL;
	CFPos fNoBarrierPos;
	CCoreObjectDictator* pCoreObj;
	SQR_TRY
	{
		Ast(pScene);
		if (!IsIgnoreClog)
		{
			FindNoBarrierPos(fNoBarrierPos, pScene, pPixelPos);
			pCoreObj = pScene->CreateObjectDictator(fNoBarrierPos, NULL, CGlobalObjVarDefCfg::GetInst()->GetIDByName(szVarCfgName));
		}
		else
		{
			pCoreObj = pScene->CreateObjectDictator(*pPixelPos, NULL, CGlobalObjVarDefCfg::GetInst()->GetIDByName(szVarCfgName));
		}
		pCharacter = new CCharacterDictator(pCoreObj);
		InitNpcVariant(pCharacter);
	}
	SQR_CATCH(exp)
	{
		string strErrType = "Create CharacterDictator Failed ！";
		ostringstream strm;
		strm<<" SceneName: "<<pScene->GetSceneName()<<" Name: "<<szName<<
			"坐标：（"<<pPixelPos->x<<","<<pPixelPos->y<<")  无障碍坐标:(" << fNoBarrierPos.x << "," << fNoBarrierPos.y << ") 场景大小：("<<pScene->GetWidthInPixel()<<","<<pScene->GetHeightInPixel()<<")"<<endl;
		exp.AppendMsg(strm.str());
		GenErr(strErrType, strm.str());
	}
	SQR_TRY_END;
	
	return pCharacter;
}

CCharacterDictator* CEntityServerCreator::CreateCharacterDictatorWithMaster(CCharacterDictator* pMaster, const TCHAR* szVarCfgName, const char* szName)
{
	CCharacterDictator* pCharacter = NULL;
	SQR_TRY
	{
		Ast(pMaster->GetCoreObj());
		CCoreObjectDictator* pMasterCoreObj = pMaster->GetCoreObj();
		Ast(pMasterCoreObj);
		CFPos fPos;
		pMasterCoreObj->GetPixelPos(fPos);
		pCharacter = CreateCharacterDictator(pMasterCoreObj->GetScene(), &fPos, szVarCfgName, szName, false);
		if(pCharacter)
			pCharacter->BindTo(pMaster);
	}
	SQR_CATCH(exp)
	{
		string strErrType = "Create CharacterDictatorWithMaster Failed ！";
		ostringstream strm;
		strm<<" Name: "<<szName<<endl;
		exp.AppendMsg(strm.str());
		GenErr(strErrType, strm.str());
	}
	SQR_TRY_END;

	return pCharacter;
}

CCharacterMediator* CEntityServerCreator::CreateCharacterMediator(CCoreSceneServer *pScene, const CFPos* pPixelPos, const TCHAR* szVarCfgName)
{
	CCharacterMediator* pCharacter = NULL;
	SQR_TRY
	{	
		Ast(pScene);
		CCoreObjectMediator* pCoreObj = pScene->CreateObjectMediator(*pPixelPos, NULL, CGlobalObjVarDefCfg::GetInst()->GetIDByName(szVarCfgName), CCoreObjectMediator::GetDefaultDirectorMaxSpeed());
		pCharacter = new CCharacterMediator(pCoreObj);
		InitPlayerVariant(pCharacter);
	}
	SQR_CATCH(exp)
	{
		string strErrType = "Create CharacterMediator Failed ！";
		ostringstream strm;
		strm<<" SceneName: "<<pScene->GetSceneName()<<"坐标：（"<<pPixelPos->x<<","<<pPixelPos->y<<") 场景大小：("<<pScene->GetWidthInPixel()<<","<<pScene->GetHeightInPixel()<<")"<<endl;
		exp.AppendMsg(strm.str());
		GenErr(strErrType, strm.str());
	}
	SQR_TRY_END;

	return pCharacter;
}

CIntObjServer* CEntityServerCreator::CreateServerIntObject(CCoreSceneServer* pScene, const CFPos* pPixelPos, const TCHAR* szVarCfgName, const char* szName)
{
	CIntObjServer* pObj = NULL;
	SQR_TRY
	{
		CCoreObjectDictator* pCoreObj = pScene->CreateObjectDictator(* pPixelPos, NULL, CGlobalObjVarDefCfg::GetInst()->GetIDByName(szVarCfgName));
		pObj = new CIntObjServer(pCoreObj);
		InitObjVariant(pObj);
		pObj->PostAoiMsgToSelf(NULL);
	}
	SQR_CATCH(exp)
	{
		string strErrType = "Create ServerIntObject Failed ！";
		ostringstream strm;
		strm<<" SceneName: "<<pScene->GetSceneName()<<" Name: "<<szName<<"坐标：（"<<pPixelPos->x<<","<<pPixelPos->y<<") 场景大小：("<<pScene->GetWidthInPixel()<<","<<pScene->GetHeightInPixel()<<")"<<endl;
		exp.AppendMsg(strm.str());
		GenErr(strErrType, strm.str());
	}
	SQR_TRY_END;

	return pObj;
}

void CEntityServerCreator::DestroyEntity(CEntityServer* pEntity)
{
	Ast (pEntity);
	pEntity->SetObjInValidByDestory();
	CCoreObjectServer* pCoreObj = pEntity?pEntity->GetCoreObj():NULL;
	if(pEntity&&pEntity->GetGameEntityType()==eGET_CharacterDictator) //召唤兽型坐骑
	{
		CCharacterDictator* pCharacter = class_cast<CCharacterDictator*>(pEntity);
		if (pCharacter && pCharacter->GetNpcAIHandler())
			pCharacter->SendNpcDestoryMsg();
	}
	CCoreSceneServer *pScene=pCoreObj?pCoreObj->GetScene():NULL;
	if(pScene)
		pScene->DestroyObject(pCoreObj);
}

//调节坐标位置,使创建对象不在障碍里
void CEntityServerCreator::FindNoBarrierPos(CFPos& resultPos, CCoreSceneServer *pScene, const CFPos* pPixelPos)
{
	CPos Pos;
	PixelPos2GridPos(Pos, *pPixelPos);
	EBarrierType  BarrierType = pScene->GetBarrier(Pos);
	if(BarrierType == eBT_NoBarrier || BarrierType == eBT_LowBarrier)
	{
		resultPos = *pPixelPos;
		return;
	}
	int Length = 1; //边长,每转两次增加1
	int Dir = 1;	//方向, 1:上(y++)  2:右(x++) 3:下(y--) 4:左(x--)
	int OffsetX = 0, OffsetY = 0;
	int SearchCount =  0;
	while (SearchCount < 30)  //最多30次
	{
		++SearchCount;
		switch(Dir)
		{
		case 1:
			OffsetX = 0;
			OffsetY = 1;
			break;
		case 2:
			OffsetX = 1;
			OffsetY = 0;
			break;
		case 3:
			OffsetX = 0;
			OffsetY = -1;
		    break;
		case 4:
			OffsetX = -1;
			OffsetY = 0;
		    break;
		}
		for(int i = 0; i < Length; ++i) 
		{
			Pos.x += OffsetX;
			Pos.y += OffsetY;
			BarrierType = pScene->GetBarrier(Pos);
			if(BarrierType == eBT_NoBarrier || BarrierType == eBT_LowBarrier)
			{
				GridPos2PixelPos(resultPos, Pos);
				return;
			}
		}
		if(SearchCount % 2 == 0)
		{
			++Length;
		}
		Dir = (Dir == 4) ? 1 : Dir + 1;
	}
	resultPos = *pPixelPos;
}

void CEntityServerCreator::InitPlayerVariant(CCharacterMediator* pPlayer)
{
	pPlayer->m_pViewVariantAttribute_EntityID->SetNumber((uint32)pPlayer->GetEntityID());
	pPlayer->m_pViewVariantAttribute_Type->SetNumber((uint8)eCT_Player);
	
	pPlayer->m_pSyncVariantAttribute1_TargetID->SetNumber((uint32)0);
}

void CEntityServerCreator::InitNpcVariant(CCharacterDictator* pNpc)
{
	pNpc->m_pViewVariantAttribute_EntityID->SetNumber((uint32)pNpc->GetEntityID());
	pNpc->m_pViewVariantAttribute_Type->SetNumber((uint8)eCT_Npc);

	pNpc->m_pSyncVariantAttribute1_TargetID->SetNumber((uint32)0);
	pNpc->m_pViewVariantMaster_MasterID->SetNumber((uint32)0);
}

void CEntityServerCreator::InitObjVariant(CIntObjServer* pObj)
{
	pObj->m_pViewVariantAttribute_EntityID->SetNumber((uint32)pObj->GetEntityID());
	pObj->m_pViewVariantAttribute_Type->SetNumber((uint8)eCT_IntObj);

	pObj->GetActor()->InitActorVariant();
}

