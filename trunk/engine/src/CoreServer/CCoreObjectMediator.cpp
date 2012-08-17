#include "stdafx.h"
#include "CCoreObjectMediator.h"
#include "CSyncCoreObjectMediator.h"
#include "CConnServer.h"
#include "CConnServer.inl"
#include "CSynConnServer.h"
#include "ICoreObjectMediatorHandler.h"
#include "CCoreServerObjJob.h"
#include "CCoreServerJob.inl"

CCoreObjectMediator::CCoreObjectMediator(ICoreObjectMediatorHandler* pHandler, CCoreSceneServer* pScene, uint32 uVarDefID, float fDirectorMaxSpeed)
:CCoreObjectDictator(pHandler, pScene, uVarDefID)
,m_pConn(NULL)
,m_fDirectorMaxSpeed(0)
,m_fZeroDimEyeSight(0.0f)
{
	SetDirectorMaxSpeed(fDirectorMaxSpeed);
}

CCoreObjectMediator::~CCoreObjectMediator(void)
{
	if (GetConnection())
	{
		GetConnection()->SetMediator(NULL);
	}
}

//CSyncCoreObjectMediator* CCoreObjectMediator::GetSynCoreObjMed()const
//{
//	return static_cast<CSyncCoreObjectMediator*>(GetSynCoreObj());
//}

//void CCoreObjectMediator::IntSetConnection(CConnServer* pConn)
//{
//	m_pConn = pConn;
//	pConn->SetMediator(this);
//	CSynConnServer* pSynConn = CSynConnServer::GetSynConn(pConn->GetGlobalID());
//	GetSynCoreObjMed()->IntSetConnection(pSynConn);
//}

//void CCoreObjectMediator::OnConnectionShutDown()
//{
//	m_pConn = NULL;
//	GetConnection()->SetMediator(NULL);
//	GetSynCoreObjMed()->OnConnectionShutDown();
//}

bool CCoreObjectMediator::SetConnection(CConnServer* pConn)
{
	(new CCoreObjSetConnectionJob(GetGlobalID(), pConn?pConn->GetGlobalID():0))->Add();

	if(m_pConn)
		m_pConn->SetMediator(NULL);
	m_pConn = pConn;
	if(pConn)
		pConn->SetMediator(this);

	SetSyncEyeSize( m_fZeroDimEyeSight);
	return true;	//和引擎线程中MedObj中的SetConnection的条件检测不一致,如果外部逻辑代码错误使用SetConnection会造成潜在的bug
	//CSynConnServer* pSynConn = CSynConnServer::GetSynConn(pConn->GetGlobalID());
	//return GetSynCoreObjMed()->SetConnection(pSynConn);
}

CConnServer* CCoreObjectMediator::GetConnection(void) const
{
	//CSynConnServer* pSynConn = GetSynCoreObjMed()->GetConnection();
	//if (!pSynConn)
	//	return NULL;
	//return CConnServer::GetConnServer(pSynConn->GetGlobalID());
	return m_pConn;
}

float CCoreObjectMediator::GetDefaultDirectorMaxSpeed()
{
	return 3.0f;
}

void CCoreObjectMediator::SetDirectorMaxSpeed(float fDirectorMaxSpeed)
{
	if(fDirectorMaxSpeed < 0)
		fDirectorMaxSpeed = 0;

	if(m_fDirectorMaxSpeed == fDirectorMaxSpeed)
		return;

	if(fDirectorMaxSpeed == 0)
	{
		ResetMovedDistArg(NULL, 0);
		m_bIsMoving = false;
		m_bIsTracing = false;
	}

	(new CCoreObjSetDirectorMaxSpeedJob(GetGlobalID(), fDirectorMaxSpeed))->Add();
	m_fDirectorMaxSpeed = fDirectorMaxSpeed;
}

float CCoreObjectMediator::GetDirectorMaxSpeed()const
{
	return m_fDirectorMaxSpeed;
}

//void CCoreObjectMediator::SetActive(bool bActive)
//{
//	if(IsActive() == bActive)
//		return;
//
//	m_fDirectorMaxSpeed = bActive;
//	if(m_fDirectorMaxSpeed)
//		m_bIsMoving = false;
//	(new CCoreObjSetDirectorMaxSpeedJob(GetGlobalID(), m_fDirectorMaxSpeed))->Add();
//	ResetMovedDistArg(NULL);
//	m_bIsMoving = false;
//	m_bIsTracing = false;
//
//	//GetSynCoreObjMed()->SetActive(bTrue);
//}

bool CCoreObjectMediator::IsActive()const
{
	return m_fDirectorMaxSpeed <= 0;
}

ICoreObjectMediatorHandler* CCoreObjectMediator::GetHandler()const
{
	return CCoreObjectDictator::GetHandler()->CastToObjMedHandler();
}

EServerObjectType CCoreObjectMediator::GetType()const
{
	return eSOT_Mediator;
}

CCoreObjectMediator* CCoreObjectMediator::CastToObjMediator()
{
	return this;
}

CCoreObjectDictator* CCoreObjectMediator::CastToObjDictator()
{
	return this;
}

EMoveToResult CCoreObjectMediator::MoveTo(const CFPos& PixelPosDes,uint16 uPixelSpeed,
	EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist)
{
	if( !IsActive() )
		return eMTR_NotAllowed;
	
	return CCoreObjectDictator::
		MoveTo(PixelPosDes,uPixelSpeed,eFindPathType,eBarrierType,fReachDist);
	//return GetSynCoreObjMed()->MoveTo(PixelPosDes,uPixelSpeed,eFindPathType,eBarrierType,fReachDist);
}

ESetSpeedResult CCoreObjectMediator::SetSpeed(float fSpeed)
{
	if( !IsActive() )
		return eSSR_NotAllowed;
	return CCoreObjectDictator::SetSpeed( fSpeed );
	//return GetSynCoreObjMed()->SetSpeed(fSpeed);
}

EStopMovingResult CCoreObjectMediator::StopMoving()
{
	if( !IsActive() )
		return eSMR_NotAllowed;
	return CCoreObjectDictator::StopMoving();
	//return GetSynCoreObjMed()->StopMoving();
}

void CCoreObjectMediator::Trace(CCoreObjectServer* pTarget, float fSpeed
	, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist)
{
	if( !IsActive() )
		return ;
	return CCoreObjectDictator::Trace(pTarget, fSpeed, eFindPathType, eBarrierType, fReachDist);
}

void CCoreObjectMediator::StopTracing()
{
	if( !IsActive() )
		return ;
	return CCoreObjectDictator::StopTracing();
}

ESetPosResult CCoreObjectMediator::SetPixelPos(const CFPos& PixelPos)
{
	if( !IsActive() )
		return eSPR_NotAllowed;
	return CCoreObjectDictator::SetPixelPos(PixelPos);
	//return GetSynCoreObjMed()->SetPixelPos(PixelPos);
}


void CCoreObjectMediator::SetSyncEyeSize(float fEyeSight)
{
	m_fZeroDimEyeSight=fEyeSight;
	if( !m_pConn )
		return;
	m_mapDim2AoiData[0].fEyeSight = fEyeSight;
	(new CCoreObjSetEyeSightJob(GetGlobalID(), fEyeSight, 0))->Add();
}

float CCoreObjectMediator::GetSyncEyeSize()const
{
	return m_fZeroDimEyeSight;
}

void CCoreObjectMediator::SetSyncKeenness(float Keenness)
{
	m_mapDim2AoiData[0].fKeenness = Keenness;
	(new CCoreObjSetKeennessJob(GetGlobalID(), Keenness, 0))->Add();
}

float CCoreObjectMediator::GetSyncKeenness()const
{
	return GetAoiData(0).fKeenness;
}

void CCoreObjectMediator::BroadCastForViewAoi(const void* pData,uint32 uSize)const
{
	BroadCastForViewAoiExtraConn( pData , uSize , IsActive() ? GetConnection() : NULL );
}

void CCoreObjectMediator::BroadCastForSyncAoi(const void* pData,uint32 uSize)const
{
	BroadCastForSyncAoiExtraConn( pData , uSize , IsActive() ? GetConnection() : NULL );
}

void CCoreObjectMediator::SyncBarrier()const
{
	SyncBarrierExtraConn(GetConnection());
}

void CCoreObjectMediator::SetMoveSpeedCheckArg(uint32 uClientTimeStampErrorThreshold, float fMaxStepCommandRate)
{
	//cout << uClientTimeStampErrorThreshold << "  " << fMaxStepCommandRate << endl;
	(new CCoreSetMoveSpeedCheckArgJob(uClientTimeStampErrorThreshold, fMaxStepCommandRate))->Add();
}
