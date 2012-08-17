#include "stdafx.h"
#include "ExpHelper.h"
#include "CMetaSceneClient.h"
#include "CCoreSceneClient.h"
#include "CCoreObjectDirector.h"
#include "CCoreObjectFollower.h"
#include "CCoreObjectPerformer.h"
#include "CCoreSceneMgrClient.h"
#include "CRenderSystemClient.h"
#include "ICoreObjectFollowerHandler.h"
#include "TCoreScene.inl"
#include "TPart.inl"
#include "CAppClient.h"
#include "PrtlGas2GacObjectDef.h"
#include "CConnClient.h"
#include "TSqrAllocator.inl"
#include "ICoreSceneClientHandler.h"
#include "CDistortedTimeSceneClient.h"
#include "CSyncVariantClientHolderPool.h"

template class TCoreScene<CTraitsClient>;
template class TPart<CCoreSceneClient,CMetaSceneClient>;
template class TPart<CCoreSceneClient,CCoreSceneMgrClient>;

CCoreSceneClient::CCoreSceneClient(CMetaSceneClient* pMetaScene,CCoreSceneMgrClient* pMgr,bool bUseByLogic)
:ParentScene_t(pMetaScene,pMgr)
,m_pDistortedTimeScene(new CDistortedTimeSceneClient)
,m_bDestroyDirector(false)
{
	m_pRenderScene		= NULL;
	m_pHandler			= NULL;
	m_bBeingUsedByLogic	= bUseByLogic;
	m_bBeingUsedByEngine= !bUseByLogic;

	SQR_TRY
	{
		m_pRenderScene=CRenderSystemClient::Inst()->CreateRenderScene(&(pMetaScene->m_SceneManager));
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
		ShowErrorMsgAndExit("CRenderScene create fail, the client will be shutdown.");

	}
	SQR_TRY_END;
	MainScene()=this;
}

CCoreSceneClient::~CCoreSceneClient(void)
{
	DestroyAllObject();
	MainScene()=NULL;

	SQR_TRY
	{
		CRenderSystemClient::Inst()->DestroyRenderScene( m_pRenderScene );
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
		ShowErrorMsgAndExit("CRenderScene delete fail, the client will be shut");
	}
	SQR_TRY_END;

	if(m_pHandler)
		AdvDelRef(m_pHandler);

	SafeRelease(m_pDistortedTimeScene);
}

void CCoreSceneClient::SetHandler(ICoreSceneClientHandler* pHandler)
{
	if(m_pHandler)
		AdvDelRef(m_pHandler);
	m_pHandler=pHandler;
	AdvAddRef(m_pHandler);
}

ICoreSceneClientHandler* CCoreSceneClient::GetHandler()const
{
	return m_pHandler;
}

void CCoreSceneClient::DestroyAllObject()
{
	if(!MainScene())
		GenErr("MainScene doesnt exist!");

	if (m_bDestroyDirector)
	{
		return;
	}
	m_bDestroyDirector = true;

	uint32 uCoreObjID = 0;
	CCoreObjectClient* pFollowerObj = NULL;
	ListObject_t listCoreObj = GetAllObject();
	
	while( !listCoreObj.empty())
	{
		if (listCoreObj.front()->GetType()==eCOT_Director)
		{
			uCoreObjID = listCoreObj.front()->GetLocalID();
			listCoreObj.pop_front();
			continue;
		}
		pFollowerObj = listCoreObj.front();
		if( listCoreObj.front()->GetHandler() )
			listCoreObj.front()->GetHandler()->OnDestroy();

		listCoreObj = GetAllObject();
		if (listCoreObj.empty())
		{
			break;
		}
		SQR_TRY
		{
			if (pFollowerObj==listCoreObj.front())
			{
				delete listCoreObj.front();
				listCoreObj.pop_front();
			}
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	}
	CCoreObjectClient* pDirectorObj = CCoreObjectClient::GetObject(uCoreObjID);
	if(pDirectorObj)
	{
		if( pDirectorObj->GetHandler() )
			pDirectorObj->GetHandler()->OnDestroy();
		SQR_TRY
		{
			pDirectorObj = CCoreObjectClient::GetObject(uCoreObjID);
			delete pDirectorObj;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
		CSyncVariantClientHolderPool::GetInst()->Clear();
	}
}

template<typename ObjectClass,typename CmdClass>
void CCoreSceneClient::CreateObject(CmdClass* pCmd)
{
	ObjectClass* pObj = new ObjectClass(pCmd);
}

template<typename CmdClass>
void CCoreSceneClient::CreateObject(CmdClass* pCmd)
{

	if( pCmd->usbTypeFlag != eSOF_Follower )
	{

		SQR_TRY
		{
			CreateObject<CCoreObjectDirector>(pCmd);
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
			ShowErrorMsgAndExit("CCoreObjectDirector create fail, the client will be shut");
		}
		SQR_TRY_END;
	}
	else
	{
		SQR_TRY
		{
			CreateObject<CCoreObjectFollower>(pCmd);
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	}
}

void CCoreSceneClient::OnServerCommand(const CGas2GacOC_Create_Moving_Object* pCmd)
{
	if(!CConnClient::Inst())
		GenErr("CoreObj would not be created as the client is disconnected.");

	CreateObject<CCoreObjectFollower>(pCmd);
}

void CCoreSceneClient::OnServerCommand(const CGas2GacOC_Create_Still_Object* pCmd)
{
	if(!CConnClient::Inst())
		GenErr("CoreObj would not be created as the client is disconnected.");

	CreateObject(pCmd);
}

void CCoreSceneClient::OnServerCommand(const CGas2GacOC_Destroy_Object* pCmd)
{
	//cout << "OnServerCommand::CGas2GacOC_Destroy_Object\t" << pCmd->uqbGlobalID << endl; 
	CCoreObjectFollower* pObj=CCoreObjectFollower::GetObjectFollower(pCmd->uqbGlobalID);
	if( ! pObj )
		GenErr(" Follower not found while trying to destroy it.");

	if (pObj->GetType() == eCOT_Director)
	{
		if(!CAppClient::Inst())
			return;	//运行TestEngine的时候这个东西有可能是null

		CCoreSceneMgrClient::Inst()->DestroyMainSceneIfNotBeingUsedByLogic();
	}
	else
	{
		//加上此try和catch是为了保证引擎对象能顺利删除,以后若能保证OnDestroy不会再抛错可以考虑将此try catch代码删除
		SQR_TRY
		{
			if(pObj->GetHandler())
				pObj->GetHandler()->OnDestroy();
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;

		SQR_TRY
		{
			pObj=CCoreObjectFollower::GetObjectFollower(pCmd->uqbGlobalID);
			delete pObj;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
		pObj = NULL;
	}
	
}

void CCoreSceneClient::OnServerCommand(const CGas2GacOC_Set_Time_Distorted_Ratio* pCmd)
{
	m_pDistortedTimeScene->SetTimeDistortedRatio(pCmd->fRatio);
}

CCoreObjectPerformer* CCoreSceneClient::CreateObjectPerformer(const CFPos& Pos,ICoreObjectPerformerHandler* 
pHandler)
{
	return new CCoreObjectPerformer(pHandler,Pos);
}

void CCoreSceneClient::DestroyObjectPerformer(CCoreObjectPerformer* pObject)
{
	delete pObject;
}

CRenderScene* CCoreSceneClient::GetRenderScene()const
{
	return m_pRenderScene;
}

CCoreSceneClient*& CCoreSceneClient::MainScene()
{
	static CCoreSceneClient* ls_pInst=NULL;
	return ls_pInst;
}

CCoreSceneClient*  CCoreSceneClient::Inst()
{
	return MainScene();
}

void CCoreSceneClient::SetDynamicBarrierInfo(uint32 uOwnerID, const DynamicBarrierArea& dynamicBarrier, EBarrierType eBarrierType)
{
	ParentScene_t::SetDynamicBarrierInfo(uOwnerID, dynamicBarrier, eBarrierType);
}

CDistortedTimeObjClient* CCoreSceneClient::CreateDistortedTimeObj()
{
	return m_pDistortedTimeScene->CreateDistortedTimeObj();
}

void CCoreSceneClient::DestroyDistortedTimeObj(CDistortedTimeObj* pObj)
{
	m_pDistortedTimeScene->DestroyDistortedTimeObj(pObj);
}

void CCoreSceneClient::SetTimeDistortedRatio(const CGas2GacOC_Set_Main_Scene* pCmd)
{
	m_pDistortedTimeScene->SetTimeDistortedRatio(pCmd->fTimeDistortedRatio);
}
