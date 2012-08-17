#include "stdafx.h"
#include "CCoreSceneServer.h"
#include "CSyncCoreSceneServer.h"
#include "CCoreObjectServer.h"
#include "CoreCommon.h"
#include "CSyncCoreObjectServer.h"
#include "CMetaSceneServer.h"
#include "QuadTreeFun.h"
#include "CSyncCoreSceneServer.h"
#include "TIDPtrMap.inl"
#include "ErrLogHelper.h"
#include "CCoreObjectMediator.h"
#include "CoreCommon.inl"
#include "CDynamicBarrierInfo.h"
#include "TPart.inl"
#include "CDynamicBarrierInfo.h"
#include "CCoreObjectCalculator.h"
#include "CAsynCoreQuadScene.h"
#include "CCoreTrMirrorBufferMgr.h"
#include "BaseHelper.h"
#include "ICoreSceneServerHandler.h"
#include "CAsyncContext.h"
#include "TSqrAllocator.inl"
#include "CCoreServerSceneJob.h"
#include "CCoreServerJob.inl"
#include "CAsynDistortedTimeSceneServer.h"
#include "CPathTransformer.h"

template class TPart<CCoreSceneServer, CMetaSceneServer>;

CCoreSceneServer* CCoreSceneServer::GetCoreSceneServer(uint32 uSceneID)
{
	CCoreSceneServer* pCoreSceneSever;
	return GetMapCoreScene().Get(pCoreSceneSever, uSceneID)? pCoreSceneSever : NULL;
}

CCoreSceneServer::CCoreSceneServer(CMetaSceneServer* pMetaScene)
:ParentMetaPart_t(pMetaScene)
,m_eCoreSceneState(ECoreSceneState_Construct)
,m_pHandler(NULL)
,m_pDestroyContext( NULL )
,m_pDistortedTimeScene(new CAsynDistortedTimeSceneServer)
{
	m_uID = GetMapCoreScene().Add(this);
	SQR_TRY
	{
		m_pBarrierInfo = new CDynamicBarrierInfo(pMetaScene->GetWidthInGrid(), pMetaScene->GetHeightInGrid());
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;
		strm << " scene name = " << pMetaScene->GetSceneName() << " width = " << pMetaScene->GetWidthInGrid() << " height = " << pMetaScene->GetHeightInGrid();
		exp.AppendMsg(strm.str().c_str());
		LogExp(exp);
	}
	SQR_TRY_END;
}

CCoreSceneServer::~CCoreSceneServer(void)
{
	if(m_pHandler)
		AdvDelRef(m_pHandler);
	delete m_pBarrierInfo;
	ReleaseQuadScene();
	Ver (GetMapCoreScene().Del(m_uID));

	SafeRelease(m_pDistortedTimeScene);
}

void CCoreSceneServer::SetHandler(ICoreSceneServerHandler* pHandler)
{
	if(m_pHandler)
		AdvDelRef(m_pHandler);
	m_pHandler=pHandler;
	AdvAddRef(m_pHandler);
}

ICoreSceneServerHandler* CCoreSceneServer::GetHandler()const
{
	return m_pHandler;
}

CCoreSceneServer::MapCoreScene& CCoreSceneServer::GetMapCoreScene()
{
	static MapCoreScene ms_mapCoreScene;
	return ms_mapCoreScene;
}

CMetaSceneServer* CCoreSceneServer::GetMetaScene()const
{
	return static_cast<CMetaSceneServer*>(ParentMetaPart_t::GetWhole());
}

CPixelPath* CCoreSceneServer::CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType, 
										 EBarrierType eBarrierType, int32 nMaxStep)
{
	return GetMetaScene()->CreatePath( PixelPosBegin, PixelPosEnd,
		eFindPathType, eBarrierType, nMaxStep, m_pBarrierInfo );
}

CPixelPath* CCoreSceneServer::CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType, 
										 EBarrierType eBarrierType)
{
	return GetMetaScene()->CreatePath( PixelPosBegin, PixelPosEnd,
		eFindPathType, eBarrierType, m_pBarrierInfo );
}

CPixelPath* CCoreSceneServer::CreatePath(const int16* pPathData, uint16 uPathDataLen, const CFPos& PixelPosBegin, 
										 const CFPos& PixelPosEnd)
{
	return GetMetaScene()->CreatePath(pPathData, uPathDataLen, PixelPosBegin, PixelPosEnd, m_pBarrierInfo);
}

CPixelPath* CCoreSceneServer::CreatePath(const vector<CFPos>& vecPos)
{
	CPathTransformer pathTransformer(vecPos);
	const CFPos& posBegin = *vecPos.begin();
	const CFPos& posEnd = *(vecPos.end()-1);

	return GetMetaScene()->
		CreatePath(pathTransformer.GetData(), pathTransformer.GetDataLenInByte(), posBegin, posEnd, m_pBarrierInfo);
}

bool CCoreSceneServer::IsGridValid(const CPos& GridPos) const
{
	return (uint32)GridPos.x < GetMetaScene()->GetWidthInGrid() && (uint32)GridPos.y < GetMetaScene()->GetHeightInGrid();
}

EBarrierType CCoreSceneServer::GetStaticBarrier( const CPos& GridPos )const
{
	return GetMetaScene()->GetBarrier(GridPos);
}

EBarrierType CCoreSceneServer::GetDynamicBarrier( const CPos& GridPos )const
{
	return m_pBarrierInfo->GetBarrier(GridPos);
}

EBarrierType CCoreSceneServer::GetBarrier( const CPos& GridPos )const
{
	return GetMetaScene()->GetBarrier(GridPos, m_pBarrierInfo);
}

CCoreObjectMediator* CCoreSceneServer::CreateObjectMediator(const CFPos& PixelPos,ICoreObjectMediatorHandler* pHandler,uint32 uVarDefID, float fDirectorMaxSpeed)
{
	ConfirmObjectCreateCondition( PixelPos );
	CCoreObjectMediator* pCoreObj = new CCoreObjectMediator(pHandler, this, uVarDefID, fDirectorMaxSpeed);
	uint32 uSlot = pCoreObj->CreatePosData(PixelPos);

	(new CCoreCreateMedObjJob(GetGlobalID(), pCoreObj->GetGlobalID(), PixelPos, uVarDefID, uSlot, fDirectorMaxSpeed))->Add();
	return pCoreObj;
}

CCoreObjectDictator* CCoreSceneServer::CreateObjectDictator(const CFPos& PixelPos,ICoreObjectDictatorHandler* pHandler,uint32 uVarDefID)
{
	ConfirmObjectCreateCondition( PixelPos );
	CCoreObjectDictator* pCoreObj = new CCoreObjectDictator(pHandler, this, uVarDefID);
	uint32 uSlot = pCoreObj->CreatePosData(PixelPos);

	(new CCoreCreateDicObjJob(GetGlobalID(), pCoreObj->GetGlobalID(), PixelPos, uVarDefID, uSlot))->Add();
	return pCoreObj;
}

CCoreObjectCalculator* CCoreSceneServer::CreateObjectCalculator(const CFPos& PixelPos,ICoreObjectCalculatorHandler* pHandler)
{
	ConfirmObjectCreateCondition( PixelPos );
	CCoreObjectCalculator* pCoreObj = new CCoreObjectCalculator(pHandler, this);
	uint32 uSlot = pCoreObj->CreatePosData(PixelPos);
	(new CCoreCreateCalcObjJob(GetGlobalID(), pCoreObj->GetGlobalID(), PixelPos, uSlot))->Add();
	return pCoreObj;
}

bool CCoreSceneServer::DestroyObject(CCoreObjectServer* pObject)
{
	return pObject?pObject->BeginDestroyObject():true;
}

void CCoreSceneServer::QueryFunc(void* pVecObject, uint32 uId)
{
	typedef vector<CCoreObjectServer*>	VecObject_t;
	VecObject_t* pVec = static_cast<VecObject_t*>(pVecObject);
	CCoreObjectServer* pObject = CCoreObjectServer::GetObjectServer(uId);
	if(pObject)
		pVec->push_back(pObject);
}

void CCoreSceneServer::QueryObject(vector<CCoreObjectServer*>& vecObject,const CFPos& Pos,float fGridRadius)const
{
	m_pQuadScene->QueryObject(QueryFunc, &vecObject, Pos, fGridRadius * eGridSpan);
}

void CCoreSceneServer::QueryObject(vector<CCoreObjectServer*>& vecObject, CCoreObjectServer* pObj, 
								   const CFPos& OffsetPos, float fRadius) const
{
	m_pQuadScene->QueryObject(QueryFunc, &vecObject, pObj->m_pQuadObject, OffsetPos, fRadius * eGridSpan);
}

bool CCoreSceneServer::TransferObject( CCoreObjectServer* pObj, CCoreSceneServer* pScene, const CFPos& PixelPos )
{
	if( pObj->GetScene() != this )
		GenErr("TransferObject failed with error:Object is not in the source scene!");

	if( pScene == this )
		GenErr("TransferObject failed with error:Target scene is the source scene!");

	return pObj->BeginTransferObject( pScene,PixelPos );
}

bool CCoreSceneServer::Destroying()const
{
	return (m_eCoreSceneState == ECoreSceneState_Desconstruct) ? true : false;
}

uint32 CCoreSceneServer::Send(const void* pData,uint32 uSize)
{
	(new CCoreSendSceneCmdJob(GetGlobalID(), pData, uSize))->Add();
	return uSize;
}

bool CCoreSceneServer::IsPixelValid(const CFPos& PixelPos)const
{
	CPos pos; 
	InlPixelPos2GridPos(pos, PixelPos);

	return IsGridValid(pos);
}

uint32 CCoreSceneServer::CreateQuadScene(CMetaSceneServer* pMetaScene)
{
	uint32 uWidth = pMetaScene->GetWidthInPixel();
	uint32 uHeight = pMetaScene->GetHeightInPixel();

	m_pQuadScene = new CAsynCoreQuadScene(uWidth, uHeight, eGridSpan, 
		CCoreTrMirrorBufferMgr::Inst()->GetQuadBufferMgr());

	return m_pQuadScene->GetSlot();
}

void CCoreSceneServer::ReleaseQuadScene()
{
	m_pQuadScene->Release();
	m_pQuadScene = NULL;
}

void CCoreSceneServer::ConfirmObjectCreateCondition(const CFPos& PixelPos)
{
	if( !IsPixelValid(PixelPos) )
	{
		ostringstream oss;
		oss<<"Invalid object PixelPos("<<PixelPos.x<<","<<PixelPos.y<<") while creating object!";
		GenErr( "Invalid object PixelPos", oss.str() );
	}
}

void CCoreSceneServer::SetDynamicBarrierInfo(uint32 uOwnerID, const DynamicBarrierArea& dynamicBarrier, EBarrierType eBarrierType)
{
	m_pBarrierInfo->SetBarrier(uOwnerID, dynamicBarrier, eBarrierType);
}

bool CCoreSceneServer::HasObject()const
{
	return !m_lstObject.empty();
}

void CCoreSceneServer::AddObject(CCoreObjectServer* pObj)
{
	m_lstObject.push_front( pObj );
	pObj->m_itObjectListInScene = m_lstObject.begin();
}

void CCoreSceneServer::DelObject(CCoreObjectServer* pObj)
{
	Ast( pObj->GetScene() == this );
	m_lstObject.erase( pObj->m_itObjectListInScene );
}

void CCoreSceneServer::SetCoreSceneState(ECoreSceneState eSceneState)
{
	m_eCoreSceneState = eSceneState;
}

void CCoreSceneServer::DestroyAllObj()
{
	ListObject::iterator itCur = m_lstObject.begin();
	ListObject::iterator itEnd = m_lstObject.end();

	for(; itCur != itEnd; ++itCur)
	{
		(*itCur)->BeginDestroyObject();
	}
}

bool CCoreSceneServer::Destroy()
{
	if( m_pDestroyContext )
		return false;	

	m_pDestroyContext= new CAsyncContext;

	if( GetHandler() )
		GetHandler()->OnDestroy( m_pDestroyContext );

	m_eCoreSceneState = ECoreSceneState_Desconstruct;

	//在删除场景时服务端逻辑线程将确保在开始场景删除流程之前所有对象开始进行删除流程,这样也就确保了对象的删除流程都是从逻辑线程发起
	DestroyAllObj();

	return true;
}

void CCoreSceneServer::EndDestroy()
{
	//当删场景由引擎线程触发时m_pDestroyContext为NULL
	//Ast( m_pDestroyContext );

	CAsyncContext* pContext = m_pDestroyContext;

	ICoreSceneServerHandler* pHandler = GetHandler();
	if( pHandler )
		AdvAddRef( pHandler );

	delete this;

	if( pHandler )
		pHandler->OnDestroyed( pContext );

	delete pContext;
}

CAsynDistortedTimeObjServer* CCoreSceneServer::CreateDistortedTimeObj()
{
	return m_pDistortedTimeScene->CreateDistortedTimeObj();
}

void CCoreSceneServer::DestroyDistortedTimeObj(CDistortedTimeObj* pObj)
{
	m_pDistortedTimeScene->DestroyDistortedTimeObj(pObj);
}

void CCoreSceneServer::SetTimeDistortedRatio(float fTimeDistortedRatio)
{
	if(m_pDistortedTimeScene->GetTimeDistortedRatio() == fTimeDistortedRatio)
		return;

	(new CCoreSetTimeDistortedRatioJob(GetGlobalID(), fTimeDistortedRatio))->Add();
	m_pDistortedTimeScene->SetTimeDistortedRatio(fTimeDistortedRatio);
}

uint32 CCoreSceneServer::GetWidthInPixel() const
{
	return GetMetaScene()->GetWidthInPixel();
}

uint32 CCoreSceneServer::GetHeightInPixel() const
{
	return GetMetaScene()->GetHeightInPixel();
}
