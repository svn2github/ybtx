#include "stdafx.h"
#include "CCoreServerObjResult.h"
#include "CCoreObjectMediator.h"
#include "ICoreObjectMediatorHandler.h"
#include "IObjPosObserverHandler.h"
#include "CCoreSceneServer.h"
#include "TimeHelper.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include <algorithm>
#include "CSyncCoreObjectServer.h"

CCoreServerObjResult::CCoreServerObjResult(uint32 uObjID)
:m_uObjID(uObjID)
{
}

CCoreServerObjResult::~CCoreServerObjResult(void)
{
}

CCoreObjectServer* CCoreServerObjResult::GetCoreObj()const
{
	return CCoreObjectServer::GetObjectServer(m_uObjID);
}

CCoreObjOnMoveBeganResult::CCoreObjOnMoveBeganResult(uint32 uObjID)
:CCoreServerObjResult(uObjID)
{
}

CCoreObjOnMoveBeganResult::~CCoreObjOnMoveBeganResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;

	//如果是处于非active状态下的Med对象则需要将移动标志位置为true
	CCoreObjectMediator* pMedObj = pCoreObj->CastToObjMediator();
	if(pMedObj && !pMedObj->IsActive())
		pCoreObj->m_bIsMoving = true;

	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;

	pHandler->OnMoveBegan();
}

CCoreObjOnMovePathChangedResult::CCoreObjOnMovePathChangedResult(uint32 uObjID)
:CCoreServerObjResult(uObjID)
{
}


CCoreObjOnMovePathChangedResult::~CCoreObjOnMovePathChangedResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;

	pHandler->OnMovePathChanged();	
}

CCoreObjOnMoveEndedResult::CCoreObjOnMoveEndedResult(uint32 uObjID, uint32 uMoveEndedType, uint32 uSessionID)
:CCoreServerObjResult(uObjID)
,m_uMoveEndedType(uMoveEndedType)
,m_uSessionID(uSessionID)
{
	CSyncCoreObjectServer* pSyncCoreObj = CSyncCoreObjectServer::GetObjectServer(uObjID);
	pSyncCoreObj->ForceUpDateQuadTreePos();
}

CCoreObjOnMoveEndedResult::~CCoreObjOnMoveEndedResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;

	CCoreObjectMediator* pMedObj = pCoreObj->CastToObjMediator();
	//Dic对象以及无论处不处于active状态的Med对象则需要将移动标志位置为false
	if(m_uSessionID == pCoreObj->m_uLastMoveID && 
		(m_uMoveEndedType == eMET_Reached || (pMedObj && !pMedObj->IsActive() && m_uMoveEndedType == eMET_Stopped)))
	{
		//cout << "move reached" << endl;
		pCoreObj->ResetMovedDistArg(NULL, 0);
		pCoreObj->m_bIsMoving = false;
		//pCoreObj->m_bIsTracing = false;
	}

	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;

	pHandler->OnMoveEnded(static_cast<EMoveEndedType>(m_uMoveEndedType), m_uSessionID);	
}

CCoreObjOnTraceEndedResult::CCoreObjOnTraceEndedResult(uint32 uObjID, uint32 uTraceEndedType, uint32 uSessionID)
:CCoreServerObjResult(uObjID)
,m_uTraceEndedType(uTraceEndedType)
,m_uSessionID(uSessionID)
{
}

CCoreObjOnTraceEndedResult::~CCoreObjOnTraceEndedResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;

	//Dic对象以及无论处不处于active状态的Med对象则需要将移动标志位置为false
	if(m_uSessionID == pCoreObj->m_uTraceSessionID)
	{
		//cout << "EndTracing" << endl;
		//pCoreObj->ResetTracedDistArg(NULL);
		//pCoreObj->m_bIsMoving = false;
		pCoreObj->m_bIsTracing = false;
	}

	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;

	pHandler->OnTraceEnded(static_cast<ETraceEndedType>(m_uTraceEndedType));	
}


CCoreObjOnTransferedResult::CCoreObjOnTransferedResult(uint32 uObjID, uint32 uNewSceneID, const CFPos& OldPos)
:CCoreServerObjResult(uObjID)
,m_uNewSceneID(uNewSceneID)
,m_OldPos(OldPos)
{
}


CCoreObjOnTransferedResult::~CCoreObjOnTransferedResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;

	pCoreObj->m_eLifeState = CCoreObjectServer::eAOLS_Running;
	CCoreSceneServer* pOldScene = pCoreObj->GetScene();
	Ast (pOldScene);
	CCoreSceneServer* pNewScene = CCoreSceneServer::GetCoreSceneServer(m_uNewSceneID);
	Ast (pNewScene);
	pOldScene->DelObject(pCoreObj);
	pCoreObj->ReSetCoreScene(pNewScene);
	pNewScene->AddObject(pCoreObj);
	pCoreObj->TransferDistortedTimeObjTo(pNewScene);

	CCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(pDicObj)
	{
		EBarrierType eBarrierType;
		pDicObj->SetBarrier(pDicObj->GetBarrierSize(), pDicObj->GetBarrierType(eBarrierType)?eBarrierType:eBT_NoBarrier);
	}

	CCoreObjectMediator* pMedObj = pCoreObj->CastToObjMediator();
	if(pMedObj)
		pMedObj->SetSyncEyeSize( pMedObj->m_fZeroDimEyeSight);

	//这句一定要放到最后,外部逻辑收到这个回调表明切换场景的所有工作都已完成
	pHandler->OnTransfered(pOldScene, m_OldPos);
}

CCoreObjOnAoiMsgFromSelfResult::CCoreObjOnAoiMsgFromSelfResult(uint32 uObjID, const void* pContext)
:CCoreServerObjResult(uObjID)
,m_pContext(pContext)
{
}


CCoreObjOnAoiMsgFromSelfResult::~CCoreObjOnAoiMsgFromSelfResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;

	pHandler->OnAoiMsgFromSelf(m_pContext);	
}


CCoreObjOnEventResult::CCoreObjOnEventResult(uint32 uObjID, const void* pData)
:CCoreServerObjResult(uObjID)
,m_pData(pData)
{
}

CCoreObjOnEventResult::~CCoreObjOnEventResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;

	pHandler->OnEvent(m_pData);	
}


CCoreObjOnCaughtViewSightOfResult::CCoreObjOnCaughtViewSightOfResult(uint32 uObjID, uint32 uObjGlobalID, 
uint32 uDimension)
:CCoreServerObjResult(uObjID)
,m_uObjGlobalID(uObjGlobalID)
,m_uDimension(uDimension)
{
}


CCoreObjOnCaughtViewSightOfResult::~CCoreObjOnCaughtViewSightOfResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;

	pHandler->OnCaughtViewSightOf(m_uObjGlobalID, m_uDimension);	
}

CCoreObjOnLostViewSightOfResult::CCoreObjOnLostViewSightOfResult(uint32 uObjID, uint32 uObjGlobalID, 
uint32 uDimension)
:CCoreServerObjResult(uObjID)
,m_uObjGlobalID(uObjGlobalID)
,m_uDimension(uDimension)
{
}


CCoreObjOnLostViewSightOfResult::~CCoreObjOnLostViewSightOfResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;

	pHandler->OnLostViewSightOf(m_uObjGlobalID, m_uDimension);	
}

CCoreObjOnDestroyResult::CCoreObjOnDestroyResult(uint32 uObjID)
:CCoreServerObjResult(uObjID)
{
}


CCoreObjOnDestroyResult::~CCoreObjOnDestroyResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(pHandler)
	{
		pHandler->OnDestroy();
	}
	delete pCoreObj;
}

CCoreObjOnConnectionDisbindedResult::CCoreObjOnConnectionDisbindedResult(uint32 uObjID)
:CCoreServerObjResult(uObjID)
{
}

CCoreObjOnConnectionDisbindedResult::~CCoreObjOnConnectionDisbindedResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CCoreObjectMediator* pCoreObjMed = pCoreObj->CastToObjMediator();
	if(!pCoreObjMed)
		return;
	ICoreObjectMediatorHandler* pHandler = pCoreObjMed->GetHandler();
	if(!pHandler)
		return;

	pHandler->OnConnectionDisbinded();
}

CCoreObjOnObserveeEnteredResult::CCoreObjOnObserveeEnteredResult(uint32 uObjID, uint32 uHandlerID)
:CCoreServerObjResult(uObjID)
,m_uHandlerID(uHandlerID)
{
}


CCoreObjOnObserveeEnteredResult::~CCoreObjOnObserveeEnteredResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;

	CCoreObjectServer::MapPosObserverHandler::iterator itHandler = pCoreObj->m_mapPosObserverHandler.find(m_uHandlerID);
	if(itHandler == pCoreObj->m_mapPosObserverHandler.end())
		return;

	IWatchHandler* pHandler = itHandler->second->CastToWatchHandler();
	if(!pHandler)
		return;
	pHandler->OnObserveeEntered();
}

CCoreObjOnObserveeLeftResult::CCoreObjOnObserveeLeftResult(uint32 uObjID, uint32 uHandlerID)
:CCoreServerObjResult(uObjID)
,m_uHandlerID(uHandlerID)
{
}


CCoreObjOnObserveeLeftResult::~CCoreObjOnObserveeLeftResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;

	CCoreObjectServer::MapPosObserverHandler::iterator itHandler = pCoreObj->m_mapPosObserverHandler.find(m_uHandlerID);
	if(itHandler == pCoreObj->m_mapPosObserverHandler.end())
		return;

	IWatchHandler* pHandler = itHandler->second->CastToWatchHandler();
	if(!pHandler)
		return;
	pHandler->OnObserveeLeft();
}

CCoreObjWatchResult::CCoreObjWatchResult(uint32 uObjID, uint32 uHandlerID, uint32 uWatchResult)
:CCoreServerObjResult(uObjID)
,m_uHandlerID(uHandlerID)
,m_uWatchResult(uWatchResult)
{
}

CCoreObjWatchResult::~CCoreObjWatchResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;

	CCoreObjectServer::MapPosObserverHandler::iterator itHandler = pCoreObj->m_mapPosObserverHandler.find(m_uHandlerID);
	if(itHandler == pCoreObj->m_mapPosObserverHandler.end())
		return;

	IWatchHandler* pHandler = itHandler->second->CastToWatchHandler();
	if(!pHandler)
		return;

	pHandler->OnWatched(static_cast<EWatchResult>(m_uWatchResult));
}

CCoreReSetCoreSceneResult::CCoreReSetCoreSceneResult(uint32 uObjID, uint32 uSceneID)
:CCoreServerObjResult(uObjID)
,m_uSceneID(uSceneID)
{
	
}

CCoreReSetCoreSceneResult::~CCoreReSetCoreSceneResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();
	if(!pCoreObj)
		return;
	CCoreSceneServer* pCoreScene = CCoreSceneServer::GetCoreSceneServer(m_uSceneID);
	if (!pCoreScene)
		return;
	pCoreObj->ReSetCoreScene(pCoreScene);

}

CCorePixelPosSetResult::CCorePixelPosSetResult(uint32 uObjID)
: CCoreServerObjResult(uObjID)
{

}

CCorePixelPosSetResult::~CCorePixelPosSetResult()
{
	CCoreObjectServer* pCoreObj  = GetCoreObj();

	if(!pCoreObj)
		return;

	ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
	if(!pHandler)
		return;
	pCoreObj->m_bPosSet = false;
}


//BindObj同步语义不需要发给逻辑线程，有回调发送时序逻辑，暂时先别删，以后如果改异步要参考
//CCoreObjOnBindToResult::CCoreObjOnBindToResult(uint32 uObjID, uint32 uParentObjID, bool bRet)
//:CCoreServerObjResult(uObjID)
//,m_uParentObjID(uParentObjID)
//,m_bRet(bRet)
//{
//}
//
//CCoreObjOnBindToResult::~CCoreObjOnBindToResult()
//{
//	CCoreObjectServer* pParentCoreObj = CCoreObjectServer::GetObjectServer(m_uParentObjID);
//	if(pParentCoreObj)
//	{
//		pair<CCoreObjectServer::SetUInt32::iterator, bool> pr
//			= pParentCoreObj->m_setBindChildID.insert(m_uObjID);
//		if(pr.second)
//		{
//			ICoreObjectServerHandler* pHandler = pParentCoreObj->GetHandler();
//			if(pHandler)
//			{
//				SQR_TRY
//				{
//					pHandler->OnBindToChild(m_uObjID, m_bRet);
//				}
//				SQR_CATCH(exp)
//				{
//					LogExp(exp);
//				}
//				SQR_TRY_END;
//			}
//		}
//	}
//
//	CCoreObjectServer* pCoreObj  = GetCoreObj();
//	if(pCoreObj)
//	{
//		pCoreObj->m_uBindParentID = m_uParentObjID;
//		ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
//		if(pHandler)
//		{
//			SQR_TRY
//			{
//				pHandler->OnBindToParent(m_uParentObjID, m_bRet);
//			}
//			SQR_CATCH(exp)
//			{
//				LogExp(exp);
//			}
//			SQR_TRY_END;
//		}
//	}
//}
//
////为避免绑定关系重叠，解绑和回调顺序暂时先按先解绑后回调，如果有具体需求需要范例再考虑是否可调换
//CCoreObjOnDisbindFromParentResult::CCoreObjOnDisbindFromParentResult(uint32 uObjID, bool bRet)
//:CCoreServerObjResult(uObjID)
//,m_bRet(bRet)
//{
//}
//
//CCoreObjOnDisbindFromParentResult::~CCoreObjOnDisbindFromParentResult()
//{
//	CCoreObjectServer* pCoreObj  = GetCoreObj();
//	if(pCoreObj)
//	{
//		uint32 uParentObjID = pCoreObj->m_uBindParentID;
//		CCoreObjectServer* pParentCoreObj = CCoreObjectServer::GetObjectServer(uParentObjID);
//		if(pParentCoreObj)
//		{
//			if(pParentCoreObj->m_setBindChildID.erase(m_uObjID))
//			{
//				ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
//				if(pHandler)
//				{
//					SQR_TRY
//					{
//						pHandler->OnDisbindToChild(m_uObjID, m_bRet);
//					}
//					SQR_CATCH(exp)
//					{
//						LogExp(exp);
//					}
//					SQR_TRY_END;
//				}
//			}
//		}
//
//		//如果CCoreObjectServer对象是异步析构，则不需要再GetCoreObj()
//		CCoreObjectServer* pCoreObj = GetCoreObj();
//		if(pCoreObj)
//		{
//			ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
//			pCoreObj->m_uBindParentID = 0;
//			if(pHandler)
//			{
//				SQR_TRY
//				{
//					pHandler->OnDisbindFromParent(uParentObjID, m_bRet);
//				}
//				SQR_CATCH(exp)
//				{
//					LogExp(exp);
//				}
//				SQR_TRY_END;
//			}
//		}
//	}
//}
//
//CCoreObjOnDisbindToChildResult::CCoreObjOnDisbindToChildResult(uint32 uObjID, uint32 uChildObjID, bool bRet)
//:CCoreServerObjResult(uObjID)
//,m_uChildObjID(uChildObjID)
//,m_bRet(bRet)
//{
//}
//
//CCoreObjOnDisbindToChildResult::~CCoreObjOnDisbindToChildResult()
//{
//	CCoreObjectServer* pCoreObj  = GetCoreObj();
//	if(pCoreObj)
//	{
//		if(pCoreObj->m_setBindChildID.erase(m_uChildObjID))
//		{
//			ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
//			if(pHandler)
//			{
//				SQR_TRY
//				{
//					pHandler->OnDisbindToChild(m_uChildObjID, m_bRet);
//				}
//				SQR_CATCH(exp)
//				{
//					LogExp(exp);
//				}
//				SQR_TRY_END;
//			}
//		}
//	}
//
//	CCoreObjectServer* pChildCoreObj = CCoreObjectServer::GetObjectServer(m_uChildObjID);
//	if(pChildCoreObj)
//	{
//		ICoreObjectServerHandler* pHandler = pChildCoreObj->GetHandler();
//		pChildCoreObj->m_uBindParentID = 0;
//		if(pHandler)
//		{
//			SQR_TRY
//			{
//				pHandler->OnDisbindFromParent(m_uObjID, m_bRet);
//			}
//			SQR_CATCH(exp)
//			{
//				LogExp(exp);
//			}
//			SQR_TRY_END;
//		}
//
//	}
//}
//
//CCoreObjOnDisbindToAllChildResult::CCoreObjOnDisbindToAllChildResult(uint32 uObjID, bool bRet)
//:CCoreServerObjResult(uObjID)
//,m_bRet(bRet)
//{
//}
//
//CCoreObjOnDisbindToAllChildResult::~CCoreObjOnDisbindToAllChildResult()
//{
//	CCoreObjectServer* pCoreObj  = GetCoreObj();
//	if(pCoreObj)
//	{
//		CCoreObjectServer::SetUInt32& setRealBindChildID = pCoreObj->m_setBindChildID;
//
//		//避免回调导致pCoreObj消失，如果CoreObjectServer是异步析构，则可以去掉这句
//		CCoreObjectServer::SetUInt32 setImageBindChildID = setRealBindChildID;
//		//copy(setRealBindChildID.begin(), setRealBindChildID.end(), setImageBindChildID.begin());
//
//		setRealBindChildID.clear();		//注意这句提前删除可能会导致回调函数中效果不准确，如果确实如此则需要根据具体需求调整调用位置
//
//		for(CCoreObjectServer::SetUInt32::iterator itr = setImageBindChildID.begin();
//			itr != setImageBindChildID.end(); ++itr)
//		{
//			uint32 uChildID = *itr;
//			//setImageBindChildID.erase(uChildID);		这句无意义，不需要
//
//			ICoreObjectServerHandler* pHandler = pCoreObj->GetHandler();
//			if(pHandler)
//			{
//				SQR_TRY
//				{
//					pHandler->OnDisbindToChild(uChildID, m_bRet);
//				}
//				SQR_CATCH(exp)
//				{
//					LogExp(exp);
//				}
//				SQR_TRY_END;
//			}
//
//			CCoreObjectServer* pChildCoreObj = CCoreObjectServer::GetObjectServer(uChildID);
//			if(pChildCoreObj)
//			{
//				pChildCoreObj->m_uBindParentID = 0;
//				ICoreObjectServerHandler* pHandler = pChildCoreObj->GetHandler();
//				if(pHandler)
//				{
//					SQR_TRY
//					{
//						pHandler->OnDisbindFromParent(m_uObjID, m_bRet);
//					}
//					SQR_CATCH(exp)
//					{
//						LogExp(exp);
//					}
//					SQR_TRY_END;
//				}
//			}
//		}
//	}
//}



