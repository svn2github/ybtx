#include "stdafx.h"
#include "CSyncCoreObjectServer.h"
#include "CSyncCoreSceneServer.h"
#include "CoreObjectServerCommon.h"
#include "CSyncMetaSceneServer.h"
#include "CSyncCoreObjectMediator.h"
#include "CObjMoveStateServer.h"
#include "TCoreObject.inl"
#include "CSyncCoreSceneMgrServer.h"
#include "CCoreMsgMgr.h"
#include "CObjTransferState.h"
#include "CAOICmd.h"
#include "AoiDef.h"
#include "CAsyAoiViewObj.h"
#include "CoreObjectServerPosRelation.h"
#include "CCoreObjDistObserver.h"
#include "CCoreObjTracer.h"
#include "CSyncWatchHandler.h"
#include "CCoreSceneServer.h"
#include "CSyncCorePos.h"
#include "CSyncCoreQuadObject.h"
#include "CCoreTrMirrorBufferMgr.h"
#include "TSqrAllocator.inl"
#include "CCoreServerObjResult.h"
#include "CCoreServerResult.inl"
#include "SyncCoreObjEvent.h"
#include "CSyncDistortedTimeObjServer.h"
#include "CSyncDistortedTimeSceneServer.h"
#include "CObjDestoryState.h"

namespace sqr
{

	template<>
	void TCoreObject<CSyncTraitsServer>::SetHandler(ISyncCoreObjectServerHandler* pHandler)
	{
	}

	template<>
	ISyncCoreObjectServerHandler* TCoreObject<CSyncTraitsServer>::GetHandler()const
	{
		return NULL;
	}
}

template class TCoreObject<CSyncTraitsServer>;

CSyncCoreObjectServer::CSyncCoreObjectServer(uint32 uObjID, CSyncCoreSceneServer* pScene, const CFPos& PixelPos)
:ParentObject_t(NULL, PixelPos, pScene)
,CSyncPtCORSubject(eSCOE_Count)
,m_pScene( pScene )
,m_uGPCNFlagRefCount(0)
,m_eFlag( eSOF_None )
,m_pTransferState( NULL )
,m_pDestoryState(NULL)
,m_eLifeState(eSOLS_Running)
,m_bIsTracing(false)
,m_uLastMoveID(0)
,m_uTraceSessionID(0)
,m_pPosData(NULL)
,m_pQuadObject(NULL)
{
	Ast(m_pScene);

	m_RefsByAoiObjHandler.SetHolder(this);
	m_AoiObj.SetHolder(this);

	m_pTracer = new CCoreObjTracer(this);
	m_pPosObservee = new CObjPosObservee(this);
	m_RefsByTracer.SetHolder(this);

	m_uID = uObjID;
	const uint32 uIndex = TIDPtrMap<CSyncCoreObjectServer*>::GetIndexByID(m_uID);
	IDVecSynCoreObjectServer& vecSynCoreObjServer = GetSynCoreObjServerVec();
	if( vecSynCoreObjServer.size() <= uIndex )
		vecSynCoreObjServer.resize( uIndex+1 );
	vecSynCoreObjServer[uIndex] = this;
}

CSyncCoreObjectServer::~CSyncCoreObjectServer(void)
{
	const uint32 uIndex = TIDPtrMap<CSyncCoreObjectServer*>::GetIndexByID(m_uID);
	IDVecSynCoreObjectServer& vecSynCoreObjServer = GetSynCoreObjServerVec();
	vecSynCoreObjServer[uIndex] = NULL;

	delete m_pPosObservee;
	m_pPosObservee = NULL;

	delete m_pTracer;
	m_pTracer = NULL;

	m_AoiObj.Detach();

	if(!m_mapPosObserver.empty())
	{
		MapPosObserver::iterator it = m_mapPosObserver.begin();

		for(; it != m_mapPosObserver.end(); ++it)
		{
			it->second->StopObserve();
			delete it->second;
			delete it->first;
		}

		m_mapPosObserver.clear();
	}
	
	ReleasePosData();
	ReleaseQuadObject();
}

CSyncCoreObjectServer::IDVecSynCoreObjectServer& CSyncCoreObjectServer::GetSynCoreObjServerVec()
{
	static IDVecSynCoreObjectServer ls_vecSynCoreObjServer;
	return ls_vecSynCoreObjServer;
}

uint32 CSyncCoreObjectServer::CreateID()
{
	return 0;
}

bool CSyncCoreObjectServer::DestroyID()
{
	return true;
}

void CSyncCoreObjectServer::InsertToScene()
{
	if(m_pQuadObject)
	{	
		m_pQuadObject->InsertToScene(m_pScene->m_pQuadScene);
	}
}

void CSyncCoreObjectServer::RemoveFrScene()
{
	if(m_pQuadObject)
	{
		m_pQuadObject->RemoveFrScene(m_pScene->m_pQuadScene);
	}
}

ESetPosResult CSyncCoreObjectServer::SetGridPos(const CPos& GridPos)
{
	return SetPixelPos( CFPos(GetPixelOfGrid(GridPos.x),GetPixelOfGrid(GridPos.y)) );
}

ESetSpeedResult CSyncCoreObjectServer::SetSpeed(float fSpeed)
{
	return ParentObject_t::IntSetSpeed(fSpeed);
}

EMoveToResult CSyncCoreObjectServer::MoveTo(const CFPos& PixelPosDes,float fSpeed
										,EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist,float fMovedDist, CPixelPath* pPixelPath)
{
	if(ParentObject_t::IsBindingObj())
		return eMTR_NotAllowed;
	//cout<<GetGlobalID()<<"\tCSynCoreObjectServer::MoveTo"<<endl;
	return ParentObject_t::IntMoveTo(PixelPosDes,fSpeed,eFindPathType,eBarrierType,fReachDist,fMovedDist,pPixelPath);
}

EMoveToResult CSyncCoreObjectServer::MoveToLimit(const CFPos& PixelPosDes, float fSpeed, 
						  EFindPathType eFindPathType, EBarrierType eBarrierType,float fReachDist, float fMovedDist, CPixelPath* pPixelPath, int32 nMaxStep)
{
	if(ParentObject_t::IsBindingObj())
		return eMTR_NotAllowed;
	return ParentObject_t::IntMoveTo(PixelPosDes, fSpeed, eFindPathType, eBarrierType, fReachDist, fMovedDist, pPixelPath, nMaxStep);
}

EStopMovingResult CSyncCoreObjectServer::StopMoving()
{
	return ParentObject_t::IntStopMoving();
}

void CSyncCoreObjectServer::OnMoveSteppedDerived(const CFPos&)
{
	NotifyPosChangedIfNeed();
}

void CSyncCoreObjectServer::OnMoveBeganDerived(const CMoveBeganDerivedArgs& Args)
{
	(new CCoreObjOnMoveBeganResult(GetGlobalID()))->Add();
	NotifySpeedChanged();
}

void CSyncCoreObjectServer::EndTracing()
{
	if(m_bIsTracing)
	{
		(new CCoreObjOnTraceEndedResult(GetGlobalID(), static_cast<uint32>(eTET_Traced), m_uTraceSessionID))->Add();
		IntStopTracing();
	}
}

void CSyncCoreObjectServer::OnEndMoveDerived()
{
	(new CCoreObjOnMoveEndedResult(GetGlobalID(), static_cast<uint32>(eMET_Reached), m_uLastMoveID))->Add();
	EndTracing();
	NotifySpeedChanged();
}

void CSyncCoreObjectServer::OnMoveStoppedDerived(float fStopDist)
{
	(new CCoreObjOnMoveEndedResult(GetGlobalID(), static_cast<uint32>(eMET_Stopped), m_uLastMoveID))->Add();
	StopTracing();
	NotifySpeedChanged();
}

void CSyncCoreObjectServer::OnMovePathChangedDerived(const CMovePathChangedDerivedArgs& Args)
{
	(new CCoreObjOnMovePathChangedResult(GetGlobalID()))->Add();
}

void CSyncCoreObjectServer::OnMoveSpeedChangedDerived(const CMoveSpeedChangedDerivedArgs& Args)
{
	NotifySpeedChanged();
}

void CSyncCoreObjectServer::OnPixelPosSetDerived(bool /*bSuperPosition*/)
{
	ForceUpDateQuadTreePos();
	NotifyPosChangedIfNeed();
}

uint32 CSyncCoreObjectServer::GetGlobalID()const
{
	return GetLocalID();
}

CSyncCoreObjectServer* CSyncCoreObjectServer::GetObjectServer(uint32 uObjGlobalID)
{
	const uint32 uIndex = TIDPtrMap<CSyncCoreObjectServer*>::GetIndexByID(uObjGlobalID);
	IDVecSynCoreObjectServer& vecSynCoreObjServer = GetSynCoreObjServerVec();
	if( vecSynCoreObjServer.size() <= uIndex )
		return NULL;
	return vecSynCoreObjServer[ uIndex ];
}

bool CSyncCoreObjectServer::IsMoving()const
{
	return IntIsDoingMove();
}

ESetPosResult CSyncCoreObjectServer::SetPixelPos(const CFPos& PixelPos)
{
	return ParentObject_t::IntSetPixelPos(PixelPos);
}

void CSyncCoreObjectServer::IntSetPositionDerived(const CFPos& PixelPos)
{
	m_pPosData->SetPos(PixelPos);

	ParentObject_t::IntSetPositionDerived(PixelPos);
	if(m_pQuadObject)
		m_pQuadObject->SetPos(PixelPos, m_pScene->m_pQuadScene);

	//考虑去掉aoi的pixelpos，换成gridpos
	CAsyAoiViewObj* pAoiObj = GetAoiObject();
	if(pAoiObj)
		pAoiObj->SetPos(PixelPos);
}

void CSyncCoreObjectServer::NotifyPosChangedIfNeed()
{
	NotifyPosChangedIfNeedSingle();
	CSyncCoreObjectDictator* pDic = this->CastToObjDictator();
	if(pDic)
		pDic->BindObjNotifyPosChangedIfNeed();
}

void CSyncCoreObjectServer::NotifyPosChangedIfNeedSingle()
{
	if ( !( m_eFlag & eSOF_GridPosChangedNotifycation ) )
		return;
	OnObserverPosChanged();
	OnObserveePosChanged();
}

void CSyncCoreObjectServer::NotifySpeedChanged()
{
	//cout << GetGlobalID() << " speed " << endl;
	Notify(eSCOE_SpeedChanged);
}

void CSyncCoreObjectServer::SetSize(float fSize, uint32 uDimension)
{
	CAsyAoiViewObj* pAoiObj = GetAoiObject();
	if(pAoiObj)
		pAoiObj->SetSize(fSize * eGridSpan, uDimension);
}

float CSyncCoreObjectServer::GetSize(uint32 uDimension)const
{
	return GetAoiObject()->GetSize(uDimension) / eGridSpan;
}

void CSyncCoreObjectServer::SetEyeSight(float fEyeSight, uint32 uDimension)
{
	CAsyAoiViewObj* pAoiObj = GetAoiObject();
	if(pAoiObj)
		pAoiObj->SetEyeSight(fEyeSight * eGridSpan, uDimension);
}

float CSyncCoreObjectServer::GetEyeSight(uint32 uDimension)const
{
	return GetAoiObject()->GetEyeSight(uDimension) / eGridSpan;
}

void CSyncCoreObjectServer::SetStealth(float fStealth, uint32 uDimension)
{
	CAsyAoiViewObj* pAoiObj = GetAoiObject();
	if(pAoiObj)
		pAoiObj->SetStealth(fStealth * eGridSpan, uDimension);
}

float CSyncCoreObjectServer::GetStealth(uint32 uDimension)const
{
	return GetAoiObject()->GetStealth(uDimension) / eGridSpan;
}

void CSyncCoreObjectServer::SetKeenness(float fKeenness, uint32 uDimension)
{
	CAsyAoiViewObj* pAoiObj = GetAoiObject();
	if(pAoiObj)
		pAoiObj->SetKeenness(fKeenness * eGridSpan, uDimension);
}

float CSyncCoreObjectServer::GetKeenness(uint32 uDimension)const
{
	return GetAoiObject()->GetKeenness(uDimension) / eGridSpan;
}

uint32 CSyncCoreObjectServer::GetViewGroup(uint32 uDimension)const
{
	return GetAoiObject()->GetViewGroup(uDimension);
}

void CSyncCoreObjectServer::SetViewGroup(uint32 uViewGroup, uint32 uDimension)
{
	CAsyAoiViewObj* pAoiObj = GetAoiObject();
	if(pAoiObj)
		pAoiObj->SetViewGroup(uViewGroup, uDimension);
}

bool CSyncCoreObjectServer::HaltAoiObject()
{
	//这个函数是可以多次调用的
	return GetAoiObject()->Halt();
}

void CSyncCoreObjectServer::SetFlag(EServerObjectFlag eFlag)
{
	m_eFlag = eFlag;
}

EServerObjectFlag CSyncCoreObjectServer::GetFlag()const
{
	return m_eFlag;
}

void CSyncCoreObjectServer::AddGPCNFlagRef()
{
	//if(this->CastToObjMediator())
	//	cout << GetGlobalID() << " AddGPCNFlagRef " << m_uGPCNFlagRefCount << " + 1" << endl;
	m_uGPCNFlagRefCount++;
	if(m_uGPCNFlagRefCount == 1)
	{
		SetGPCNFlag(true);
	}
}

void CSyncCoreObjectServer::DelGPCNFlagRef()
{
	//if(this->CastToObjMediator())
	//	cout << GetGlobalID() << " DelGPCNFlagRef " << m_uGPCNFlagRefCount << endl;
	if (m_uGPCNFlagRefCount > 0)
	{
		--m_uGPCNFlagRefCount;
		//cout << " - 1\n";
	}
	if(m_uGPCNFlagRefCount == 0)
	{
		SetGPCNFlag(false);
	}
}

void CSyncCoreObjectServer::SetGPCNFlag(bool bSet)
{
	uint32 uFlag = GetFlag();

	if ( bSet )
	{
		uFlag = uFlag | eSOF_GridPosChangedNotifycation;
	}
	else
	{
		uFlag = uFlag & (~eSOF_GridPosChangedNotifycation);
	}

	SetFlag( static_cast<EServerObjectFlag>( uFlag ) );
}

bool CSyncCoreObjectServer::GetGPCNFlag()const
{
	return GetFlag() & eSOF_GridPosChangedNotifycation;
}


void CSyncCoreObjectServer::PostEvent(const void* pContext)
{
	(new CCoreObjOnEventResult(GetGlobalID(), pContext))->Add();
}

CSyncCoreObjectMediator* CSyncCoreObjectServer::CastToObjMediator()
{
	return const_cast<CSyncCoreObjectMediator*>
		(const_cast<const CSyncCoreObjectServer*>(this)->CastToObjMediator());
}

CSyncCoreObjectDictator* CSyncCoreObjectServer::CastToObjDictator()
{
	return const_cast<CSyncCoreObjectDictator*>
		(const_cast<const CSyncCoreObjectServer*>(this)->CastToObjDictator());
}


const CSyncCoreObjectMediator* CSyncCoreObjectServer::CastToObjMediator()const
{
	return GetType() == eSOT_Mediator ?
		static_cast<const CSyncCoreObjectMediator*>( this ) : NULL;
}

const CSyncCoreObjectDictator* CSyncCoreObjectServer::CastToObjDictator()const
{
	return GetType() != eSOT_Calculator ?
		static_cast<const CSyncCoreObjectDictator*>( this ) : NULL;
}

const CAsyAoiViewObj* CSyncCoreObjectServer::GetAoiObject() const
{
	return m_AoiObj.Get();
}

CAsyAoiViewObj* CSyncCoreObjectServer::GetAoiObject()
{
	return const_cast<CAsyAoiViewObj*>
		(const_cast<const CSyncCoreObjectServer*>(this)->GetAoiObject());
}

void CSyncCoreObjectServer::ForceUpDateQuadTreePos()
{
	if (GetAoiObject())
		GetAoiObject()->ForceSetPos(GetPixelPos());
	if(m_pQuadObject)
		m_pQuadObject->ForceSetPos(GetPixelPos(), m_pScene->m_pQuadScene);
}

void CSyncCoreObjectServer::OnViewRelationChanged(CSyncCoreObjectServer* pObjServer,
											  bool bSee, uint32 uDimension)
{
}

bool CSyncCoreObjectServer::BeginDestroyObject()
{
	if( eSOLS_Transfering==m_eLifeState )
	{
		delete m_pTransferState;
		m_pTransferState = NULL;
		m_eLifeState =eSOLS_Destroying;
	}

	if( eSOLS_Running !=m_eLifeState)
		return false;

	IntBeginDestroyObject();

	return true;
}


void CSyncCoreObjectServer::EndDestroyObject()
{
	//如果对象在删除场景过程中创建m_pDestoryState此时为空
	if (!m_pDestoryState || (m_pDestoryState && m_pDestoryState->DelRef()))
	{
		if(GetType() == eSOT_Dictator || GetType() == eSOT_Mediator)
		{
			CSyncCoreObjectDictator* pThisDictator = class_cast<CSyncCoreObjectDictator*>(this);
			pThisDictator->DisbindAllChildren();
			pThisDictator->DisbindFromParent();
		}

		(new CCoreObjOnDestroyResult(GetGlobalID()))->Add();

		CSyncCoreSceneServer* pScene = GetScene();
		pScene->DestroyAoiObj( GetAoiObject() );
		RemoveFrScene();

		if (m_pDestoryState)
		{
			delete m_pDestoryState;
			m_pDestoryState = NULL;
		}
		delete this;

		pScene->OnObjectDestroyed();
	}
}

void CSyncCoreObjectServer::IntBeginDestroyObject()
{
	Ver( HaltAoiObject() );
	m_eLifeState = eSOLS_Destroying;
	m_pDestoryState = new CObjDestoryState();
}


bool CSyncCoreObjectServer::BeginTransferObject(CSyncCoreSceneServer* pScene,const CFPos& PixelPos)
{
	if( eSOLS_Running != m_eLifeState  )
		return false;

	IntBeginTransferObject(pScene,PixelPos);

	return true;
}

void CSyncCoreObjectServer::IntBeginTransferObject(CSyncCoreSceneServer* pScene,const CFPos& PixelPos)
{
	Ver( HaltAoiObject() );
	m_pTransferState = new CObjTransferState(pScene, PixelPos);
	m_eLifeState = eSOLS_Transfering;
}


void CSyncCoreObjectServer::EndTransferObject()
{
	if (m_pTransferState && m_pTransferState->DelRef())
	{
		m_eLifeState = eSOLS_Running;
		CFPos OldPos= GetPixelPos();
		IntEndTransferObject();

		(new CCoreObjOnTransferedResult(GetGlobalID(), GetScene()->GetGlobalID(), OldPos))->Add();

		delete m_pTransferState;
		m_pTransferState = NULL;
	}
}

namespace sqr
{
	struct AoiInfo
	{
		float			fSize;	
		float			fEyeSight;	
		float			fStealth;		
		float			fKeenness;	
		uint32		uViewGroup;	
	};
}


void CSyncCoreObjectServer::IntEndTransferObject()
{
	if( IsMoving() )
		StopMoving();

	vector<uint32> vecDim;
	deque<AoiInfo> vecAoiInfo;
	GetAoiObject()->GetDimensions(vecDim);
	for (vector<uint32>::iterator it = vecDim.begin();it!=vecDim.end();++it)
	{
		AoiInfo AoiInfo;
		AoiInfo.fSize = GetAoiObject()->GetSize(*it);
		AoiInfo.fEyeSight = GetAoiObject()->GetEyeSight(*it);
		AoiInfo.fKeenness = GetAoiObject()->GetKeenness(*it);
		AoiInfo.fStealth = GetAoiObject()->GetStealth(*it);
		AoiInfo.uViewGroup = GetAoiObject()->GetViewGroup(*it);
		vecAoiInfo.push_back(AoiInfo);
	}

	GetScene()->DestroyAoiObj( GetAoiObject() );
	GetScene()->DelObject( this );
	RemoveFrScene();

	ReSetCoreScene(m_pTransferState->m_pTargetScene);
	RawSetPixelPos( m_pTransferState->m_PixelPos );
	
	m_pPosData->SetPos(m_pTransferState->m_PixelPos);
	if (m_pQuadObject)m_pQuadObject->RawSetPos(m_pTransferState->m_PixelPos);
	
	GetScene()->AddObject( this );
	InsertToScene();

	GetScene()->CreateAoiObj(this);

	for (vector<uint32>::iterator it = vecDim.begin();it!=vecDim.end();++it)
	{
		GetAoiObject()->SetSize(vecAoiInfo.front().fSize,*it);
		GetAoiObject()->SetEyeSight(vecAoiInfo.front().fEyeSight,*it);
		GetAoiObject()->SetKeenness(vecAoiInfo.front().fKeenness,*it);
		GetAoiObject()->SetStealth(vecAoiInfo.front().fStealth,*it);
		GetAoiObject()->SetViewGroup(vecAoiInfo.front().uViewGroup,*it);
		vecAoiInfo.pop_front();
	}
	
	GetDistortedTimeObj()->TransferTo(GetScene()->m_pDistortedTimeScene);
}


void CSyncCoreObjectServer::OnAoiObjectHalted()
{
	switch (m_eLifeState)
	{
	case eSOLS_Transfering:
		EndTransferObject();
		break;
	case eSOLS_Destroying:
		EndDestroyObject();
		break;
	default:
		break;
	}
}

void CSyncCoreObjectServer::IntPostMsgToViewee(const void* pContext, uint32 uDimension)
{
	GetAoiObject()->PostMsgToViewee(pContext, uDimension);
}

void CSyncCoreObjectServer::IntPostMsgToViewer(const void* pContext, uint32 uDimension)
{
	GetAoiObject()->PostMsgToViewer(pContext, uDimension);
}

void CSyncCoreObjectServer::PostAoiMsgToSelf(const void* pContext)
{
	CAoiPostShellMsgCmd* pCmd = new CAoiPostShellMsgCmd(pContext);
	IntPostMsgToSelf(pCmd);
}

void CSyncCoreObjectServer::IntPostMsgToSelf(const void* pContext)
{
	GetAoiObject()->PostMsgToSelf(pContext);
}

void CSyncCoreObjectServer::OnMsgFromViewer(const void* pContext)
{
}

void CSyncCoreObjectServer::OnMsgFromViewee(const void* pContext)
{
}

void CSyncCoreObjectServer::OnMsgToVieweeHandled(const void* pContext)
{
}

void CSyncCoreObjectServer::OnMsgToViewerHandled(const void* pContext)
{
}

void CSyncCoreObjectServer::OnMsgFromSelf(const void* pContext)
{
	CAoiCmdBase* pCmd=const_cast<CAoiCmdBase*>(static_cast<const CAoiCmdBase*>(pContext));
	pCmd->HandleCmd(this);
	delete pCmd;
}

bool CSyncCoreObjectServer::Running()const
{
	return GetAoiObject()->Running();
}

bool CSyncCoreObjectServer::Destroying()const
{
	return eSOLS_Destroying==m_eLifeState&&!Running();
}

bool CSyncCoreObjectServer::Transfering()const
{
	return m_pTransferState != NULL;
}

void CSyncCoreObjectServer::LogMsg(const string& log)
{
}

void CSyncCoreObjectServer::SetQuerySize(float fSize)
{
	if(m_pQuadObject)m_pQuadObject->SetSize(fSize, m_pScene->m_pQuadScene);
}

float CSyncCoreObjectServer::GetQuerySize() const
{
	if(m_pQuadObject)
		return m_pQuadObject->GetSize();

	return 0;
}

void CSyncCoreObjectServer::GetRealCurPos(CFPos& PixelPos)
{
	if(!IsMoving())
	{
		PixelPos = GetPixelPos();
	}
	else
	{
		GetMovState()->GetRealCurPos(PixelPos);
	}
}

float CSyncCoreObjectServer::Distance(const CSyncCoreObjectServer* pCoreObj)const
{
	Ast(pCoreObj);
	CFPos posSelf, posTarget;
	posSelf = GetPixelPos();
	posTarget = pCoreObj->GetPixelPos();
	float fDist = posSelf.Dist(posTarget);
	return fDist;
	//if(pCoreObj->CastToObjDictator())
	//{
	//	//cout << "CSyncCoreObjectServer::Distance fOrgDist = " << fDist << " TargetBS = " << pCoreObj->GetBarrierSize() << endl;
	//	float fOffSet = pCoreObj->CastToObjDictator()->GetBarrierSize() * eGridSpanForObj;
	//	fDist -= fOffSet;
	//}
	//return max(0.0f, fDist);
}

void CSyncCoreObjectServer::SetAoiObjectHalt()
{
	if( eSOLS_Running != m_eLifeState  )
		return;

	Ver(HaltAoiObject());
	m_eLifeState = eSOLS_AoiDestroying;
}


CObjPosObservee* CSyncCoreObjectServer::GetPosObservee()const
{
	return m_pPosObservee;
}

void CSyncCoreObjectServer::OnObserveePosChanged()
{
	if(!m_pPosObservee)
		return;

	m_pPosObservee->OnPosChanged();
}

void CSyncCoreObjectServer::OnObserverPosChanged()
{
	if(m_mapPosObserver.empty())
		return;

	MapPosObserver::iterator it = m_mapPosObserver.begin();
	for(; it != m_mapPosObserver.end(); ++it)
		it->second->OnObserverPosChanged();
}

void CSyncCoreObjectServer::Watch(uint32 uHandlerID, CSyncCoreObjectServer* pTarget, float fPixelDistThreshold)
{
	if(!pTarget)
		return;

	CSyncWatchHandler* pHandler = new CSyncWatchHandler;
	pHandler->m_uID = uHandlerID;
	if(!m_mapPosObserverHandler.insert(make_pair(uHandlerID, pHandler)).second)
	{
		stringstream str;
		str << " handlerID " << uHandlerID;
		LogErr("Watch failure", str.str());
		delete pHandler;
		pHandler = NULL;
		return;
	}

	CCoreObjDistObserver* pDistObserver = new CCoreObjDistObserver(this->CastToObjDictator(), pHandler);
	(new CCoreObjWatchResult(GetGlobalID(), pHandler->m_uID, static_cast<uint32>(pDistObserver->Observe(pTarget->GetPosObservee(), fPixelDistThreshold))))->Add();
}

void CSyncCoreObjectServer::StopWatching(uint32 uHandlerID)
{
	MapPosObserverHandler::iterator itHandler = m_mapPosObserverHandler.find(uHandlerID); 
	if(itHandler == m_mapPosObserverHandler.end())
		return;
	CSyncWatchHandler* pHandler = itHandler->second->CastToWatchHandler();
	if(!pHandler)
		return;

	MapPosObserver::iterator it = m_mapPosObserver.find(pHandler);
	if(m_mapPosObserver.end() != it)
	{
		CCoreObjDistObserver* pDistObserver = it->second->CastToDistObserver();
		pDistObserver->Destroy();
		delete pHandler;
		m_mapPosObserverHandler.erase(itHandler);
	}
}

void CSyncCoreObjectServer::Trace(CSyncCoreObjectServer* pTarget, float fSpeed
	, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist)
{
	if(!pTarget)
	{
		(new CCoreObjOnTraceEndedResult(GetGlobalID(), static_cast<uint32>(eTET_Failed), m_uTraceSessionID))->Add();
		return;
	}

	if(m_pTracer->Trace(pTarget, fSpeed, eFindPathType, eBarrierType, fReachDist) == eTR_Success)
	{
		//如果失败需要发OnTraceEnded的result,参数为failed
		//(new CCoreObjOnTraceEndedResult(GetGlobalID(), static_cast<uint32>(eTET_Failed), m_uTraceSessionID))->Add();
		//return;
		m_bIsTracing = true;
	}
	else
	{
		if(m_bIsTracing)
		{
			StopMoving();
			IntStopTracing();
		}
		(new CCoreObjOnTraceEndedResult(GetGlobalID(), static_cast<uint32>(eTET_Failed), m_uTraceSessionID))->Add();
	}
}

void CSyncCoreObjectServer::IntStopTracing()
{
	m_pTracer->Stop();
	m_bIsTracing = false;
}

void CSyncCoreObjectServer::StopTracing()
{
	if(!m_bIsTracing)
		return;
	(new CCoreObjOnTraceEndedResult(GetGlobalID(), static_cast<uint32>(eTET_Stopped), m_uTraceSessionID))->Add();
	IntStopTracing();
}

void CSyncCoreObjectServer::CreatePosData(uint32 uSlot, const CFPos& pos)
{
	m_pPosData = new CSyncCorePos(pos, uSlot, 
		CCoreTrMirrorBufferMgr::Inst()->GetPosBufferMgr());
}

void CSyncCoreObjectServer::ReleasePosData()
{
	if(m_pPosData)
	{
		m_pPosData->Release();
		m_pPosData = NULL;
	}
	
}

void CSyncCoreObjectServer::CreateQuadObject(uint32 uSlot, const CFPos& pos)
{
	m_pQuadObject = new CSyncCoreQuadObject(GetGlobalID(), pos, uSlot, 
		CCoreTrMirrorBufferMgr::Inst()->GetQuadBufferMgr());
}

void CSyncCoreObjectServer::ReleaseQuadObject()
{
	if(m_pQuadObject)
	{
		m_pQuadObject->Release();
		m_pQuadObject = NULL;
	}
}

void CSyncCoreObjectServer::ReSetCoreScene(CSyncCoreSceneServer* pSynCoreScene)
{
	m_pScene = pSynCoreScene;
	//(new CCoreReSetCoreSceneResult(m_uID, pSynCoreScene->GetGlobalID()))->Add();
}

void CSyncCoreObjectServer::IncreaseMoveToSessionID()
{
	++m_uLastMoveID;
}

void CSyncCoreObjectServer::IncreaseTraceSessionID()
{
	++m_uTraceSessionID;
}


uint32 CSyncCoreObjectServer::GetLastMoveID()const
{
	return m_uLastMoveID;
}

uint32 CSyncCoreObjectServer::GetTraceSessionID()const
{
	return m_uTraceSessionID;
}


CSyncCoreObjectServer* CSyncCoreObjectServer::GetObject(uint32 uLocalID)
{
	return GetObjectServer(uLocalID);	
}








