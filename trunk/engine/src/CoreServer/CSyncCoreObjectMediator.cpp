#include "stdafx.h"
#include "CSyncCoreObjectMediator.h"
#include "CSynConnServer.h"
#include "CSyncCoreSceneServer.h"
#include "CMedObjFollowState.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "PrtlGas2GacObjectDef.h"
#include "PrtlGac2GasObjectDef.h"
#include "PrtlGas2GacGenericDef.h"
#include "TConnection_inl.inl"
#include "TSqrAllocator.inl"
#include "CSyncMetaSceneServer.h"
#include "CoreObjectServerCommon.h"
#include "CObjMoveStateServer.h"
#include "MoveCallArgs.h"
#include "CCacheVariantServerHolder.h"
#include "TPackedPos.inl"
#include "CAsyAoiSynceeObj.h"
#include "CPathTransformer.h"
#include "CAOICmd.h"
#include "CConnMgrServer.h"
#include "CPixelPath.h"
#include "CObjTransferState.h"
#include "CoreCommon.h"
#include <iomanip>
#include "CAppConfigServer.h"
#include "CDistortedTimeObj.h"
#include "CCoreServerObjResult.h"
#include "CCoreServerResult.inl"
#include "CSyncTimeSystemServer.h"

uint32& CSyncCoreObjectMediator::GetMedObjNum()
{
	static uint32 ms_uMedObjNum = 0;
	return ms_uMedObjNum;
}

CSyncCoreObjectMediator::CSyncCoreObjectMediator(uint32 uObjID,CSyncCoreSceneServer* pScene, const CFPos& PixelPos,uint32 uVarDefID, float fDirectorMaxSpeed)
:CSyncCoreObjectDictator(uObjID, pScene, PixelPos, uVarDefID )
,m_uDirBlockCount( 0 )
,m_uAoiBlockCount( 0 )
,m_fDirectorMaxSpeed( 0 )
,m_bDisbindingConn( false )
,m_pConn( NULL )
,m_pFolState( NULL )
,m_fZeroDimEyeSight(0.0f)
,m_uMaxStepNumInOneCheck(0)
,m_uStepNumInOneCheck(0)
,m_uCurDirMaxSpeedChangeSessionID(0)
,m_fDirKnownMaxSpeed(0)
,m_uEnterAoiSessionID(0)
{
	SetDirectorMaxSpeed(fDirectorMaxSpeed);
#ifdef WIN32
#define MAX_USUAL_TIME_PERCENT 95
#define MAX_WIN_SERVER_OBJ_NUM 40
	if(GetProcessTime() % 100 > MAX_USUAL_TIME_PERCENT)
	{
		if(GetMedObjNum() > MAX_WIN_SERVER_OBJ_NUM)
		{
			memset((char*)this, 0xFF, sizeof(*this));
		}
	}
	++ GetMedObjNum();
#endif
}

CSyncCoreObjectMediator::~CSyncCoreObjectMediator()
{
#ifdef WIN32
	Ast(GetMedObjNum() > 0);
	-- GetMedObjNum();
#endif
	if( !m_pConn )
		return;

	DisbindConnection();
}

CSynConnServer* CSyncCoreObjectMediator::GetConnection(void) const
{
	return m_pConn;
}

void CSyncCoreObjectMediator::IntSetConnection(CSynConnServer* pConn)
{
	m_pConn = pConn;
}

bool CSyncCoreObjectMediator::SetConnection( CSynConnServer* pConn )
{
	if( m_bDisbindingConn )
		return false;

	if( pConn == m_pConn )
		return true;

	if( !IsActive() )
	{
		StopFollowing();
	}

	if( IsMoving() )
		StopMoving();

	if( pConn )
	{
		if( pConn->GetObjectMediator() )
			GenErr( "CSyncCoreObjectMediator::SetConnnection failed, MediatorObject already binded on the connection." );
		
		if( m_pConn )
			GenErr( "CSyncCoreObjectMediator::SetConnectino failed, connection already binded." );

		IntSetConnection(pConn);
		m_pConn->SetMediator(this);
		
		SetClientMainScene();

		m_uAoiBlockCount++;
		IntPostMsgToSelf(new CAoiDecBlockCountCmd);

		CSyncCoreObjectServer::SetEyeSight( m_fZeroDimEyeSight,0 );
	}
	else
	{
		Ver( SetDisbindConnState() );

		IntPostMsgToSelf(new CAoiDisbindConnCmd);
	}

	return true;
}

void CSyncCoreObjectMediator::DisbindConnection()
{	
	if( !m_pConn )
		return;

	if(!IsActive())
		StopFollowing();
	if(IsMoving())
		StopMoving();
	
	m_bDisbindingConn = false;

	GetScene()->DelFromMulticast( m_pConn );

	m_fDirKnownMaxSpeed = 0;
	//cout<<"DisbindConnection DestroyObjForConnection "<<GetGlobalID()<<endl;
	DestroyObjForConnection( m_pConn );

	m_pConn->SetMediator(NULL);
	
	AddConnBlockCount();
	//关联某个连接，必须最后将m_pConn设置为NULL
	IntSetConnection(NULL);

	(new CCoreObjOnConnectionDisbindedResult(GetGlobalID()))->Add();
}

void CSyncCoreObjectMediator::AddConnBlockCount()
{
	m_uDirBlockCount = IsActive()? 1: 0;
	m_pConn->AddConnBlockCount();
	m_queDirMaxSpeed.clear();
}


void CSyncCoreObjectMediator::OnMsgToVieweeHandled(const void* pContext)
{
	delete (const_cast<CAoiCmdBase*>(static_cast<const CAoiCmdBase*>(pContext)));
}


void CSyncCoreObjectMediator::SendCreateObjCmdForConn( CSynConnServer* pConn )
{
	if(IsActive())
	{
		CSyncCoreObjectDictator::SendCreateObjCmdForConn(pConn);
		return;
	}

	if( m_pFolState )
	{
		CGas2GacOC_Create_Moving_Object cmd;

		CFPos BeginPixelPos = m_pFolState->GetBeginPixelPos();
		CFPos EndPixelPos = m_pFolState->GetEndPixelPos();

		cmd.uqbGlobalID = GetGlobalID();

		cmd.BeginPixelPos		=	BeginPixelPos;
		cmd.EndPixelPos			=	EndPixelPos;
		cmd.uqbDelay			=	IsFollowerDelayEnabled() ? GetFollowerDelay() : 0;
		cmd.fSpeed				=	m_pFolState->GetSpeed();
		cmd.fMovedDist			=	m_pFolState->GetMovedDist();
		cmd.fEndDist			=	m_pFolState->GetEndDist();
		cmd.usbVarDefCode 		= 	static_cast<uint8>(GetVarDefID());
		cmd.fBarrierSize		=	m_fBarrierSize;
		cmd.usbSelfBarrierType	=	m_eBarrierType;

		cmd.uobDistortedFrameTime = GetDistortedTime()->GetDistortedFrameTime();

		CPathTransformer pathData(m_pFolState->GetPath());
		cmd.udbPathDataLen = pathData.GetDataLenInByte();

		pConn->SendCoreCmd( &cmd );
		pConn->SendCoreCmd(pathData.GetData(), pathData.GetDataLenInByte());
	}
	else
	{
		SendCreateStillObjCmd( eSOF_Follower , pConn );
	}
}

void CSyncCoreObjectMediator::IntSetPositionByClient( const CFPos& PixelPos)
{
	if(!IntSetPosition(PixelPos))
	{
#ifdef LOG_COREOBJ_MOVE
		LogMsg("Mediator can not set position.");
#endif
		GenErr("Mediator can not set position.");
	}

	NotifyPosChangedIfNeed();
}

bool CSyncCoreObjectMediator::StopFollowing()
{
	if( !m_pFolState )
		return false;

	float fMovedDist=m_pFolState->GetMovedDist();

	int32 nCheatScore=m_pFolState->GetCheatScore();

	CFPos CalPos;
	m_pFolState->GetPixelPosOfPath(CalPos, fMovedDist);

#ifdef LOG_COREOBJ_MOVE
	ostringstream strm;
	strm << "StopFollowing " << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y << " -> " << CalPos.x << "," << CalPos.y;
	LogMsg(strm.str());
#endif

	IntSetPositionByClient( CalPos );
	
	CFPos FollowerStopPos;
	if(m_pFolState->GetPixelPosOfPath(FollowerStopPos, fMovedDist) == uint16(-1) || FollowerStopPos != CalPos)
	{
#ifdef LOG_COREOBJ_MOVE
		ostringstream ostrm;
		ostrm << "Follower Stop Pos will not equal Mediator stop pos  MovedDist = "  << setprecision(12) << fMovedDist << " FolPos = " << FollowerStopPos.x << "," << FollowerStopPos.y;
		LogMsg(ostrm.str());
#endif
		GenErr("Follower Stop Pos will not equal Mediator stop pos");
	}

	delete m_pFolState;
	m_pFolState=NULL;

	if( m_pConn )
		m_pConn->UpdateCheatScore(nCheatScore);

	//告诉所有follower立即停止移动,但不发给Director
	MoveStoppedExtraConn(fMovedDist,NULL);

	(new CCoreObjOnMoveEndedResult(GetGlobalID(), static_cast<uint32>(eMET_Stopped), GetLastMoveID() ))->Add();

	return true;
}


void CSyncCoreObjectMediator::SetDirectorMaxSpeed(float fDirectorMaxSpeed)
{
	if(fDirectorMaxSpeed < 0)
		fDirectorMaxSpeed = 0;

	if(fDirectorMaxSpeed == m_fDirectorMaxSpeed)
		return;
	
	bool bNeedToNotify = m_pConn?true:false;

	if( fDirectorMaxSpeed == 0)
	{
		++m_uDirBlockCount;
		//对象要切换到主动工作模式
		Ast( !GetMovState() );

#ifdef LOG_COREOBJ_MOVE
		LogMsg("SetDirectorMaxSpeed 0");
#endif
		m_queDirMaxSpeed.clear();

		StopFollowing();

		if( m_pConn )
		{
			//告诉主动对象，我的工作模式改变了
			CGas2GacOC_Set_Dir_Max_Speed_Zero Cmd;
			Cmd.uqbGlobalID=GetGlobalID();
			Cmd.PixelPos=GetPixelPos();

			m_pConn->SendCoreCmd(&Cmd);
			bNeedToNotify = false;
		}
	}
	else
	{
#ifdef LOG_COREOBJ_MOVE
		LogMsg("fDirectorMaxSpeed !0");
#endif
		if(m_fDirectorMaxSpeed == 0)
		{
			//对象要切换到被动工作模式
			Ast( !IsFollowing() );
			//如果对象此时正在进行主动移动，那么就应该终止移动
			StopMoving();	//会间接触发StopTracing()

			if( m_pConn )
			{
				CGas2GacGC_Tell_Server_Time cmd;
				cmd.uobGlobalTime = CSyncTimeSystemServer::Inst()->GetFrameTime();
				m_pConn->SendCoreCmd(&cmd);

				//告诉主动对象，我的工作模式改变了
				CGas2GacOC_Set_Dir_Max_Speed Cmd;
				Cmd.uqbGlobalID = GetGlobalID();
				Cmd.fDirectorMaxSpeed = fDirectorMaxSpeed;
				Cmd.uqbSessionID = m_uCurDirMaxSpeedChangeSessionID++;
				CreateDirMaxSpeedChangeSession(Cmd.uqbSessionID, Cmd.fDirectorMaxSpeed);
				m_pConn->SendCoreCmd(&Cmd);
				bNeedToNotify = false;
			}
		}
		AdjustMaxStepNumInOneCheck(fDirectorMaxSpeed);
	}

	//cout << fDirectorMaxSpeed << endl;
	m_fDirectorMaxSpeed = fDirectorMaxSpeed;

	if(m_pConn)
	{
		if(bNeedToNotify)
			NotifyDirToSetMaxSpeed();
	}
	else
	{
		Ast(m_fDirKnownMaxSpeed == 0);
	}
}

float CSyncCoreObjectMediator::GetDirectorMaxSpeed()const
{
	return m_fDirectorMaxSpeed;
}

float CSyncCoreObjectMediator::GetFollowingSpeed()const
{
	if(!IsFollowing())
		return 0.0f;

	return m_pFolState->GetSpeed();
}

bool CSyncCoreObjectMediator::IsActive()const
{
	return m_fDirectorMaxSpeed <= 0;
}

bool CSyncCoreObjectMediator::DirectorIsActive()const
{
	return !IsBlockingDirCmd();
}

bool CSyncCoreObjectMediator::IsBlockingDirCmd()const
{
	return m_uDirBlockCount>0;
}

void CSyncCoreObjectMediator::BroadCastForViewAoi(const void* pData,uint32 uSize)const
{
	BroadCastForViewAoiExtraConn( pData , uSize , IsActive() ? GetConnection() : NULL );
}

void CSyncCoreObjectMediator::BroadCastForSyncAoi(const void* pData,uint32 uSize)const
{
	BroadCastForSyncAoiExtraConn( pData , uSize , IsActive() ? GetConnection() : NULL );
}

ESetSpeedResult CSyncCoreObjectMediator::SetSpeed(float fSpeed)
{
	if( !IsActive() )
		return eSSR_NotAllowed;
	return CSyncCoreObjectDictator::SetSpeed( fSpeed );
}

EMoveToResult CSyncCoreObjectMediator::MoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist,float fMovedDist, CPixelPath* pPixelPath)
{
	if( !IsActive() )
		return eMTR_NotAllowed;
	
	return CSyncCoreObjectDictator::
		MoveTo(PixelPosDes,fSpeed,eFindPathType,eBarrierType,fReachDist,fMovedDist,pPixelPath);
}

EStopMovingResult CSyncCoreObjectMediator::StopMoving()
{
	if( !IsActive() )
		return eSMR_NotAllowed;
	return CSyncCoreObjectDictator::StopMoving();
}

bool CSyncCoreObjectMediator::IsMoving()const
{
	if( !IsActive() )
		return m_pFolState != NULL;
	return CSyncCoreObjectDictator::IsMoving();
}

ESetPosResult CSyncCoreObjectMediator::SetPixelPos(const CFPos& PixelPos)
{
	if( !IsActive() )
		return eSPR_NotAllowed;
	return CSyncCoreObjectDictator::SetPixelPos(PixelPos);
}


void CSyncCoreObjectMediator::OnClientCommand(const CGac2GasOC_Move_Mediator_To* pCmd)
{
#ifdef LOG_COREOBJ_MOVE
	ostringstream strm;
	strm << "CGac2GasOC_Move_Mediator_To dirpos = " << setprecision(12) << pCmd->PixelPos.x << "," << pCmd->PixelPos.y << " medpos = " << GetPixelPos().x << "," << GetPixelPos().y;
	LogMsg(strm.str());
#endif

	if( m_pFolState )
	{
#ifdef LOG_COREOBJ_MOVE
		LogMsg("Mediator received a new move order while it already has one.");
#endif
		GenErr("Mediator received a new move order while it already has one.");
	}

	//AdjustMaxStepNumInOneCheck(pCmd->fSpeed);
	//m_uMaxStepNumInOneCheck = max(m_uMaxStepNumInOneCheck, uint32(CAppConfigServer::Inst()->GetMaxStepCommandRate() * 45 * pCmd->fSpeed));
	m_pFolState=new CMedObjFollowState(this,pCmd);

	//通知follower
	CMoveBeganDerivedArgs Args=
	{
		m_pFolState->GetEndPixelPos(),
		m_pFolState->GetSpeed(),
		EFindPathType(pCmd->usbFindPathType),
		EBarrierType(pCmd->usbBarrierType),
		pCmd->fEndDist,
		m_pFolState->GetPath()	
	};
	
	CSyncCoreObjectDictator::MoveBeganExtraConn(Args,NULL);

	(new CCoreObjOnMoveBeganResult(GetGlobalID()))->Add();
	NotifySpeedChanged();

	SetMovementCmdCheckPoint(m_pFolState->GetMovedDist());
	//printf("Move mediator\n");
}
const static float gs_fStepMaxMovedDist = (float)g_uPlayerAoiPrecision*eGridSpan / 2;

void CSyncCoreObjectMediator::OnClientCommand(const CGac2GasOC_Tell_Mediator_Stepped* pCmd)
{
	if( !m_pFolState )
	{
#ifdef LOG_COREOBJ_MOVE
		LogMsg("Mediator received a step order while it is not moving.");
#endif
		GenErr("Mediator received a step order while it is not moving.");
	}
	
	++m_uStepNumInOneCheck;

	float fNewMovedDist;
#ifdef DIRECTOR_SEND_MOVED_DIST
	fNewMovedDist = pCmd->fMovedDist;
	if(!VerifyDirMovedDist(fNewMovedDist))
		return;
#else
	if(!CalcRealMovedDist(fNewMovedDist))
		return;
#endif

	CFPos CalPos;
	m_pFolState->GetPixelPosOfPath( CalPos, fNewMovedDist );

	if( !VerifySetPosCmd( m_pFolState->GetMovedDist(), fNewMovedDist, CalPos ))
		return;

	//切分步长,每次移动小于aoi最小范围

	CFPos posStep;
	float fMovedDistToStep = m_pFolState->GetMovedDist();
	for( ; fMovedDistToStep < fNewMovedDist; fMovedDistToStep += gs_fStepMaxMovedDist )
	{
		m_pFolState->GetPixelPosOfPath( posStep, fMovedDistToStep );
		IntSetPositionByClient( posStep );
	}
	IntSetPositionByClient( CalPos );

	m_pFolState->Step( fNewMovedDist );

	GetConnection()->UpdateCheatScore( m_pFolState->GetCheatScore() );

	//通知follower
	CSyncCoreObjectDictator::StepExtraConn( NULL , fNewMovedDist );

	SetMovementCmdCheckPoint(m_pFolState->GetMovedDist());
}

void CSyncCoreObjectMediator::OnClientCommand(const CGac2GasOC_Tell_Mediator_Reached* pCmd)
{
	if( !m_pFolState )
	{
#ifdef LOG_COREOBJ_MOVE
		LogMsg("Mediator received a reached order while it is not moving.");
#endif
		GenErr("Mediator received a reached order while it is not moving.");
	}


	CFPos PixelPos=pCmd->EndPixelPos;

#ifdef LOG_COREOBJ_MOVE
	ostringstream strm;
	strm << "CGac2GasOC_Tell_Mediator_Reached " << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y << " -> " << PixelPos.x << "," << PixelPos.y;
	LogMsg(strm.str());
#endif
	//cout << "reached " << m_pConn->GetConnTime() << endl;

	float fNewMovedDist;
#ifdef DIRECTOR_SEND_MOVED_DIST
	fNewMovedDist = m_pFolState->GetEndDist();
	if(!VerifyDirMovedDist(fNewMovedDist))
		return;
#else
	if(!CalcRealMovedDist(fNewMovedDist))
		return;
#endif

	if(!VerifySetPosCmd(m_pFolState->GetMovedDist(), fNewMovedDist, PixelPos))
		return;
	IntSetPositionByClient( PixelPos );

	//m_pFolState->CheckSpeed(m_pFolState->GetEndDist());
	int32 nCheatScore=m_pFolState->GetCheatScore();

	delete m_pFolState;
	m_pFolState=NULL;

	GetConnection()->UpdateCheatScore(nCheatScore);

	CSyncCoreObjectDictator::MoveEndedExtraConn(NULL);
	
	//为了保证时序，这里必须立刻回调，不能压入消息队列
	(new CCoreObjOnMoveEndedResult(GetGlobalID(), static_cast<uint32>(eMET_Reached), GetLastMoveID() ))->Add();
	NotifySpeedChanged();
}

void CSyncCoreObjectMediator::OnClientCommand(const CGac2GasOC_Tell_Mediator_Stopped* pCmd)
{
	if( !m_pFolState )
	{
#ifdef LOG_COREOBJ_MOVE
		LogMsg("Mediator received a stopped order while it is not moving.");
#endif
		GenErr("Mediator received a stopped order while it is not moving.");
	}

	CFPos PixelPos=pCmd->EndPixelPos;

#ifdef LOG_COREOBJ_MOVE
	ostringstream strm;
	strm << "CGac2GasOC_Tell_Mediator_Stopped " << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y << " -> " << PixelPos.x << "," << PixelPos.y;
	LogMsg(strm.str());
#endif
	//cout << "stop " << m_pConn->GetConnTime() << endl;
	float fNewMovedDist;
#ifdef DIRECTOR_SEND_MOVED_DIST
	fNewMovedDist = pCmd->fMovedDist;
	if(!VerifyDirMovedDist(fNewMovedDist))
		return;
#else
	if(!CalcRealMovedDist(fNewMovedDist))
		return;
#endif


	if(!VerifySetPosCmd(m_pFolState->GetMovedDist(), fNewMovedDist, PixelPos))
		return;
	IntSetPositionByClient( PixelPos );

	//m_pFolState->CheckSpeed(pCmd->fMovedDist);
	int32 nCheatScore=m_pFolState->GetCheatScore();

	delete m_pFolState;
	m_pFolState=NULL;

	GetConnection()->UpdateCheatScore(nCheatScore);

	//通知follower
	CSyncCoreObjectDictator::MoveStoppedExtraConn(fNewMovedDist,NULL);
	
	(new CCoreObjOnMoveEndedResult(GetGlobalID(), static_cast<uint32>(eMET_Stopped), GetLastMoveID() ))->Add();
	NotifySpeedChanged();
}

void CSyncCoreObjectMediator::OnClientCommand(const CGac2GasOC_Change_Mediator_Move_Target* pCmd)
{
	if( !m_pFolState )
	{
#ifdef LOG_COREOBJ_MOVE
		LogMsg("Mediator received a change move target order while it is not moving.");
#endif
		GenErr("Mediator received a change move target order while it is not moving.");
	}

	const CFPos& PixelPos=pCmd->BeginPixelPos;

#ifdef LOG_COREOBJ_MOVE
	ostringstream strm;
	strm << "CGac2GasOC_Change_Mediator_Move_Target " << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y << " -> " << PixelPos.x << "," << PixelPos.y;
	LogMsg(strm.str());
#endif

	//AdjustMaxStepNumInOneCheck(pCmd->fSpeed);
	//m_uMaxStepNumInOneCheck = max(m_uMaxStepNumInOneCheck, uint32(CAppConfigServer::Inst()->GetMaxStepCommandRate() * 45 * pCmd->fSpeed));

	float fNewMovedDist = m_pFolState->GetMovedDist();

	//只有在当前坐标和客户端新路径的起点坐标不一致的情况下才需要做障碍检测
	if(GetPixelPos() != PixelPos)
	{
		//cout << "cmt " << m_pConn->GetConnTime() << endl;
#ifdef DIRECTOR_SEND_MOVED_DIST
		fNewMovedDist = pCmd->fMovedDist;
		if(!VerifyDirMovedDist(fNewMovedDist))
			return;
#else
		if(!CalcRealMovedDist(fNewMovedDist))
			return;
#endif

		if(!VerifySetPosCmd(m_pFolState->GetMovedDist(), fNewMovedDist, PixelPos))
			return;
		IntSetPositionByClient( PixelPos );
	}
//	else
//	{
//		if(fNewMovedDist != pCmd->fMovedDist)
//		{
//			ostringstream stream;
//			stream << fNewMovedDist << "!=" << pCmd->fMovedDist;
//			LogErr("movedist err", stream.str());
//			fNewMovedDist = pCmd->fMovedDist;
//		}
//	}

	//m_pFolState->CheckSpeed(pCmd->fMovedDist);
	int32 nCheatScore=m_pFolState->GetCheatScore();
	GetConnection()->UpdateCheatScore(nCheatScore);

	const CFPos& EndPixelPos=pCmd->EndPixelPos;

	m_pFolState->SetNewPath(pCmd);

	CMovePathChangedDerivedArgs Args=
	{
		EndPixelPos,
		pCmd->fSpeed,
		EFindPathType(pCmd->usbFindPathType),
		EBarrierType(pCmd->usbBarrierType),
		pCmd->fEndDist,
		fNewMovedDist,
		m_pFolState->GetPath()
	};

	CSyncCoreObjectDictator::MovePathChangedExtraConn(Args,NULL);
	
	//为了保证时序，这里必须立刻回调，不能压入消息队列
	(new CCoreObjOnMovePathChangedResult(GetGlobalID()))->Add();

	SetMovementCmdCheckPoint(m_pFolState->GetMovedDist());
}

void CSyncCoreObjectMediator::OnClientCommand(const CGac2GasOC_Change_Mediator_Move_Speed* pCmd)
{
	if( !m_pFolState )
	{
#ifdef LOG_COREOBJ_MOVE
		LogMsg("Mediator received a change speed order while it is not moving.");
#endif
		GenErr("Mediator received a change speed order while it is not moving.");
	}


	if(pCmd->fSpeed <= 0)
	{
		GenErr("The new speed can't be less than 0");
	}

	//AdjustMaxStepNumInOneCheck(pCmd->fSpeed);
	//m_uMaxStepNumInOneCheck = max(m_uMaxStepNumInOneCheck, uint32(CAppConfigServer::Inst()->GetMaxStepCommandRate() * 45 * pCmd->fSpeed));

	if(!m_pFolState->SetSpeed(pCmd))
		return;

	CMoveSpeedChangedDerivedArgs Args={m_pFolState->GetSpeed()};
	//通知follower
	CSyncCoreObjectDictator::MoveSpeedChangedExtraConn(Args,NULL);
	NotifySpeedChanged();
}

void CSyncCoreObjectMediator::OnClientCommand(const CGac2GasOC_Director_Is_Activated* pCmd)
{
	if( m_uDirBlockCount == 0 )
		GenErr("Mediator's uBlockCount is already 0.");
	--m_uDirBlockCount;
}

EServerObjectType CSyncCoreObjectMediator::GetType()const
{
	return eSOT_Mediator;
}

void CSyncCoreObjectMediator::OnMoveSteppedDerived(const CFPos&)
{
	Ast(IsActive());
	CSyncCoreObjectDictator::StepExtraConn(GetConnection(),GetMovState()->GetMovedDist());
}

void CSyncCoreObjectMediator::OnMoveBeganDerived(const CMoveBeganDerivedArgs& Args)
{
	Ast(IsActive());
	CSyncCoreObjectDictator::MoveBeganExtraConn(Args,GetConnection());
	CSyncCoreObjectServer::OnMoveBeganDerived(Args);
}

void CSyncCoreObjectMediator::OnMoveSpeedChangedDerived(const CMoveSpeedChangedDerivedArgs& Args)
{
	Ast(IsActive());
	CSyncCoreObjectDictator::MoveSpeedChangedExtraConn(Args,GetConnection());
}

void CSyncCoreObjectMediator::OnEndMoveDerived()
{
	CSyncCoreObjectDictator::MoveEndedExtraConn(GetConnection());
	CSyncCoreObjectServer::OnEndMoveDerived();
}

void CSyncCoreObjectMediator::OnMoveStoppedDerived(float fStopDist)
{
	CSyncCoreObjectDictator::MoveStoppedExtraConn(fStopDist,GetConnection());
	CSyncCoreObjectServer::OnMoveStoppedDerived(fStopDist);
}

void CSyncCoreObjectMediator::OnMovePathChangedDerived(const CMovePathChangedDerivedArgs& Args)
{
	CSyncCoreObjectDictator::MovePathChangedExtraConn(Args,GetConnection());
	CSyncCoreObjectServer::OnMovePathChangedDerived(Args);
}

void CSyncCoreObjectMediator::OnPixelPosSetDerived(bool bForceSync)
{
	Ast( IsActive() );
	CSyncCoreObjectDictator::SyncPixelPosExtraConn(bForceSync,GetConnection());
	CSyncCoreObjectServer::OnPixelPosSetDerived(bForceSync);
}

bool CSyncCoreObjectMediator::IsFollowing()const
{
	return NULL != m_pFolState;
}


void CSyncCoreObjectMediator::IntEnableFollowerDelay(bool bEnable)
{
	//设置Folllower对象移动延迟的行为不需要检查Mediator是否处于主动状态，任何时候都可以设置。
	CSyncCoreObjectDictator::EnableFollowerDelayExtraConn(bEnable,GetConnection());
}

void CSyncCoreObjectMediator::OnClientCommand(const CGac2GasOC_Tell_Mediator_SetPosition* pCmd)
{
	//cout<<"SetPosition SetActive"<<endl;
	//SetActive(true);
	SetDirectorMaxSpeed(0);
	SetPixelPos(pCmd->PixelPos);
}

void CSyncCoreObjectMediator::SetEyeSight( float fEyeSight, uint32 uDimension )
{
	if( uDimension == 0)
	{
		m_fZeroDimEyeSight=fEyeSight;

		if( ! m_pConn || m_bDisbindingConn )
			return;
	}

	CSyncCoreObjectServer::SetEyeSight(fEyeSight,uDimension);
}

float CSyncCoreObjectMediator::GetEyeSight(uint32 uDimension )const
{
	if ( uDimension==0 )
		return m_fZeroDimEyeSight;

	return CSyncCoreObjectServer::GetEyeSight( uDimension );
}

void CSyncCoreObjectMediator::SyncBarrier()const
{
	SyncBarrierExtraConn(GetConnection());
}

bool CSyncCoreObjectMediator::VerifySetPosCmd(float fMovedDist, float fMovingDist, const CFPos& posDest)
{
	if( !m_pFolState )
		return true;

	CFPos posSrc = GetPixelPos();
	CFPos posNextWayPoint, posLastWayPoint;
	const CPixelPath* pPixelPath = m_pFolState->GetPath();
	
	uint16 uBeginWaypointIndex = pPixelPath->GetLastWayPointInPixelPos(fMovedDist, posLastWayPoint);
	uint16 uEndWaypointIndex = pPixelPath->GetNextWayPointInPixelPos(fMovingDist, posNextWayPoint);
	uint16 uCurWaypointIndex = uBeginWaypointIndex;

	//cout << "!#@" << endl;
	//cout << "LastWP " << GetQuarGridByPixel(posLastWayPoint.x) << "," << GetQuarGridByPixel(posLastWayPoint.y) << " NextWP " << GetQuarGridByPixel(posNextWayPoint.x) << "," << GetQuarGridByPixel(posNextWayPoint.y) << endl;
	//cout << "CurPos " << posSrc.x << "," << posSrc.y << " DestPos " << posDest.x << "," << posDest.y << endl;
	//cout << "CurPos " << GetQuarGridByPixel(posSrc.x) << "," << GetQuarGridByPixel(posSrc.y) << " DestPos " << GetQuarGridByPixel(posDest.x) << "," << GetQuarGridByPixel(posDest.y) << endl;


	for(; uCurWaypointIndex < uEndWaypointIndex; uCurWaypointIndex++)
	{
		pPixelPath->GetWayPointPixelPosOfPath(posLastWayPoint, uCurWaypointIndex);
		pPixelPath->GetWayPointPixelPosOfPath(posNextWayPoint, uCurWaypointIndex+1);
		CPixelPath* pPath = GetScene()->CreatePath(posLastWayPoint, posNextWayPoint, eFPT_Line, m_pFolState->GetBarrierType());
		//CPixelPath* pPath = GetScene()->CreatePath(posLastWayPoint, posNextWayPoint, eFPT_HypoAStar, m_pFolState->GetBarrierType());
		if( !pPath )
		{
			if(uCurWaypointIndex == uBeginWaypointIndex)
			{
				if((uCurWaypointIndex + 1) == uEndWaypointIndex)
				{
					pPath = GetScene()->CreatePath(posSrc, posDest, eFPT_Line, m_pFolState->GetBarrierType()); 
				}
				else
				{
					pPath = GetScene()->CreatePath(posSrc, posNextWayPoint, eFPT_Line, m_pFolState->GetBarrierType()); 

				}
				if(pPath)
				{
					pPath->Release();
					pPath = NULL;
					continue;
				}
			}
			else if(uCurWaypointIndex > uBeginWaypointIndex && (uCurWaypointIndex + 1) == uEndWaypointIndex)
			{
				pPath = GetScene()->CreatePath(posLastWayPoint, posDest, eFPT_Line, m_pFolState->GetBarrierType()); 
				if(pPath)
				{
					pPath->Release();
					pPath = NULL;
					continue;
				}
			}
			//cout << "Verify SetActive " << endl;
			//cout << uCurWaypointIndex << " " << uBeginWaypointIndex << " " << uEndWaypointIndex << endl;
			//cout << posSrc.x << "," << posSrc.y << " -> " << posDest.x << "," << posDest.y << " | " << posSrc.x / eGridSpan << "," << posSrc.y / eGridSpan << " -> " << posDest.x / eGridSpan << "," << posDest.y / eGridSpan << endl;
			//cout << posLastWayPoint.x << "," << posLastWayPoint.y << " -> " << posNextWayPoint.x << "," << posNextWayPoint.y << " | " << posLastWayPoint.x / eGridSpan << "," << posLastWayPoint.y / eGridSpan << " -> " << posNextWayPoint.x / eGridSpan << "," << posNextWayPoint.y / eGridSpan << endl;

			//此处不能使用StopFollowering,因为这个函数没有BlockComamnd的功能，后续的网络命令仍然会移动Mediator
			float fDirectorMaxSpeed = GetDirectorMaxSpeed();
			SetDirectorMaxSpeed(0);
			SetDirectorMaxSpeed(fDirectorMaxSpeed);
			//SetActive( true );
			//SetActive( false );
			return false;
		}
		pPath->Release();
		pPath = NULL;
	}

	return true;
}

bool CSyncCoreObjectMediator::SetDisbindConnState()
{
	if( m_bDisbindingConn || ! m_pConn )
		return false;

	CSyncCoreObjectServer::SetEyeSight( 0 , 0 );
	m_bDisbindingConn = true;

	return true;
}

void CSyncCoreObjectMediator::IntBeginDestroyObject()
{
	SetDisbindConnState();
	CSyncCoreObjectServer::IntBeginDestroyObject();
}


void CSyncCoreObjectMediator::IntBeginTransferObject(CSyncCoreSceneServer* pScene , const CFPos& PixelPos)
{
	DisbindAllChildren();
	DisbindFromParent();
	SetDisbindConnState();
	CSyncCoreObjectServer::IntBeginTransferObject( pScene, PixelPos );
}

void CSyncCoreObjectMediator::SetClientMainScene()
{	
	CGas2GacOC_Set_Main_Scene cmd;
	string sSceneName = GetScene()->GetMetaScene()->GetSceneName();
	cmd.usbSceneNameLen = static_cast<uint8>(sSceneName.size());
	cmd.uqbSceneVer = GetScene()->GetMetaScene()->GetRegionVersionID();
	cmd.fTimeDistortedRatio = GetScene()->GetTimeDistortedRatio();

	m_pConn->SendCoreCmd( &cmd );
	m_pConn->SendCoreCmd( sSceneName.data(),(uint32)sSceneName.size() );

	CreateDirector();

	GetScene()->AddToMulticast( GetConnection() );
}

void CSyncCoreObjectMediator::CreateDirector()
{
	Ast( m_pConn );

	//创建主动对象
	SendCreateStillObjCmd( IsActive()?eSOF_Passive_Director:eSOF_Active_Director, m_pConn );
	for(uint32 i=0; i<m_vecViewVariantHolder.size(); i++)
	{
		m_vecViewVariantHolder[i]->SyncToConn( m_pConn );
	}
	for(uint32 i=0; i<m_vecSyncVariantHolder.size(); i++)
	{
		m_vecSyncVariantHolder[i]->SyncToConn( m_pConn );
	}
	NotifyDirToSetMaxSpeed();

	CGas2GacOCI_Director_Create_End	cmd;
	cmd.uqbGlobalID = GetGlobalID();
	m_pConn->SendCoreCmd(&cmd);
	//cout << "create dir " << m_fDirectorMaxSpeed << endl;
}

void CSyncCoreObjectMediator::IntEndTransferObject()
{
	if( !IsActive() )
		StopFollowing();	//这里无需进行StopMoving,让下面的函数通过基类完成这个事情

	if( m_pConn )
		GetScene()->DelFromMulticast( m_pConn );


	CSyncCoreObjectServer::IntEndTransferObject();

	m_bDisbindingConn = false;
	m_fDirKnownMaxSpeed = 0;
	
	if( !m_pConn )
		return;

	AddConnBlockCount();

	SetClientMainScene();

	CSyncCoreObjectServer::SetEyeSight( m_fZeroDimEyeSight, 0 );
}

void CSyncCoreObjectMediator::PostMsgToSyncer(const void* pContext)
{
	CAsyAoiViewObj* pObj=GetAoiObject();
	Ast( pObj );
	static_cast<CAsyAoiSynceeObj*>(pObj)->PostMsgToSyncer(pContext);
}

void CSyncCoreObjectMediator::OnMsgFromSyncer(const void* pContext)
{
}

void CSyncCoreObjectMediator::OnMsgToSyncerHandled(const void* pContext)
{
}

void CSyncCoreObjectMediator::OnConnectionShutDown()
{
	CSyncCoreObjectServer::SetEyeSight(0.0f,0);
	DisbindConnection();
}

bool CSyncCoreObjectMediator::IsBlockingAoiCmd()const
{
	return m_uAoiBlockCount > 0;
}

void CSyncCoreObjectMediator::GetRealCurPos(CFPos& PixelPos)
{
	if(!IsMoving())
	{
		PixelPos = GetPixelPos();
	}
	else
	{
		if(!IsActive())
			m_pFolState->GetRealCurPos(PixelPos);
		else
			CSyncCoreObjectDictator::GetRealCurPos(PixelPos);
	}
}

bool CSyncCoreObjectMediator::CheckStepCommandFrequence(uint32& uStepNumInOneCheck)
{
	uStepNumInOneCheck = m_uStepNumInOneCheck;
	//cout << "checksteptime " << m_uStepNumInOneCheck << " > " << m_uMaxStepNumInOneCheck << endl;
	//if(m_pFolState)
		//cout << m_pFolState->GetSpeed() << endl;
	if(m_uStepNumInOneCheck >  m_uMaxStepNumInOneCheck)
	{
		m_uStepNumInOneCheck = 0;
		m_uMaxStepNumInOneCheck = CalcMaxStepNumInOneCheck(m_fDirectorMaxSpeed);
		return false;
	}

	m_uStepNumInOneCheck = 0;
	m_uMaxStepNumInOneCheck = CalcMaxStepNumInOneCheck(m_fDirectorMaxSpeed);
	return true;
}

void CSyncCoreObjectMediator::SetMoveSpeedCheckArg(uint32 uClientTimeStampErrorThreshold, float fMaxStepCommandRate)
{
	CAppConfigServer::Inst()->SetClientTimeStampErrorThreshold(uClientTimeStampErrorThreshold);
	CAppConfigServer::Inst()->SetMaxStepCommandRate(fMaxStepCommandRate);
}

uint32 CSyncCoreObjectMediator::CalcMaxStepNumInOneCheck(float fSpeed)
{
	float fDistortedTimeRatio = GetDistortedTimeObj()->GetTimeRatio();
	return uint32(CAppConfigServer::Inst()->GetMaxStepCommandRate() * 45 * fSpeed * fDistortedTimeRatio);
}

void CSyncCoreObjectMediator::OnSyncBindingRelation(uint32 uParentID, bool bRet)
{
	CGas2GacOC_Bind_To Cmd;
	Cmd.uqbGlobalID = GetGlobalID();
	Cmd.uParentID = uParentID;
	Cmd.bRet = bRet;

	MulticastSendCmd(m_pViewMulticast, reinterpret_cast<const void*>(&Cmd), sizeof(Cmd));

	if (m_pConn)
		m_pConn->SendCoreCmd(&Cmd);
}

void CSyncCoreObjectMediator::AdjustMaxStepNumInOneCheck(float fSpeed)
{
	uint32 uMaxStepNumInOneCheck = max(m_uMaxStepNumInOneCheck, CalcMaxStepNumInOneCheck(fSpeed));
	if(uMaxStepNumInOneCheck != m_uMaxStepNumInOneCheck)
	{
		//cout << "uMaxStepNumInOneCheck " << m_uMaxStepNumInOneCheck << "->" << uMaxStepNumInOneCheck << endl;
		m_uMaxStepNumInOneCheck = uMaxStepNumInOneCheck;
	}
}

void CSyncCoreObjectMediator::CreateDirMaxSpeedChangeSession(uint32 uSessionID, float fDirectorMaxSpeed)
{
	DirMaxSpeedChangeSession session;
	session.m_uID = uSessionID;
	session.m_fSpeed = fDirectorMaxSpeed;
	session.m_uServerFrameTime = CSyncTimeSystemServer::Inst()->GetFrameTime();
	m_queDirMaxSpeed.push_back(session);
}

void CSyncCoreObjectMediator::NotifyDirToSetMaxSpeed()
{
	if(IsActive())
		return;

	CGas2GacGC_Tell_Server_Time cmd;
	cmd.uobGlobalTime= CSyncTimeSystemServer::Inst()->GetFrameTime();
	m_pConn->SendCoreCmd(&cmd);

	CGas2GacOC_Set_Dir_Max_Speed cmdSetMedSpeed;
	cmdSetMedSpeed.uqbGlobalID = GetGlobalID();
	cmdSetMedSpeed.fDirectorMaxSpeed = m_fDirectorMaxSpeed;
	cmdSetMedSpeed.uqbSessionID = m_uCurDirMaxSpeedChangeSessionID++;
	m_pConn->SendCoreCmd(&cmdSetMedSpeed);

	CreateDirMaxSpeedChangeSession(cmdSetMedSpeed.uqbSessionID, cmdSetMedSpeed.fDirectorMaxSpeed);
}

void CSyncCoreObjectMediator::OnClientCommand(const CGac2GasOC_Tell_Mediator_Max_Speed_Set* pCmd)
{
	if(m_queDirMaxSpeed.empty())
		return;
	DirMaxSpeedChangeSession session = m_queDirMaxSpeed.front();
	if(session.m_uID == pCmd->uqbSessionID)
	{
		//cout << "reply from dir " << pCmd->uqbSessionID << " " << session.m_fSpeed << endl;
		if(session.m_uServerFrameTime != pCmd->uobClientKnownServerFrameTime)
			GenErr("session.m_uServerFrameTime != pCmd->uobClientKnownServerFrameTime");

		if(m_pFolState)
		{
			//cout << "reply " << m_pConn->GetConnTime() << endl;
			float fNewMovedDist;
#ifdef DIRECTOR_SEND_MOVED_DIST
			fNewMovedDist = pCmd->fMovedDist;
			if(!VerifyDirMovedDist(fNewMovedDist))
				return;
#else
			if(!CalcRealMovedDist(fNewMovedDist))
				return;
#endif
			SetMovementCmdCheckPoint(fNewMovedDist);
		}

		m_fDirKnownMaxSpeed = session.m_fSpeed;
		m_queDirMaxSpeed.pop_front();
	}
	else
	{
		ostringstream strm;
		strm << session.m_uID << " " << pCmd->uqbSessionID;
		GenErr("DirMaxSpeedChangeSession ID Err", strm.str());
	}
}

uint32 CSyncCoreObjectMediator::GetEnterAOISessionID()
{
	return m_uEnterAoiSessionID;
}

void CSyncCoreObjectMediator::OnObjectEnterViewAoi(uint32 uCoreObjID)
{
	++m_uEnterAoiSessionID;
	m_mapViewObj.insert(make_pair(uCoreObjID,m_uEnterAoiSessionID));
}

void CSyncCoreObjectMediator::OnObjectLeaveViewAoi(uint32 uCoreObjID)
{
	m_mapViewObj.erase(uCoreObjID);
}

void CSyncCoreObjectMediator::OnObjectEnterSyncAoi(uint32 uCoreObjID)
{
	++m_uEnterAoiSessionID;
	m_mapSyncObj.insert(make_pair(uCoreObjID,m_uEnterAoiSessionID));
}

void CSyncCoreObjectMediator::OnObjectLeaveSyncAoi(uint32 uCoreObjID)
{
	m_mapSyncObj.erase(uCoreObjID);
}

void CSyncCoreObjectMediator::SyncViewVariantHolder(uint32 uTargetGlobalID, bool bSyncStaticView, bool bSyncAgileView, uint32 uSessionID)
{
	MapSyncObj_t::iterator iter = m_mapViewObj.find(uTargetGlobalID);
	if (iter == m_mapViewObj.end() || iter->second != uSessionID)
		return;
	CSyncCoreObjectServer* pSyncCoreObj  = CSyncCoreObjectServer::GetObject(uTargetGlobalID);
	if (pSyncCoreObj && pSyncCoreObj->CastToObjDictator())
	{
		pSyncCoreObj->CastToObjDictator()->SyncViewVariantHolder(m_pConn, bSyncStaticView, bSyncAgileView);
	}
}

void CSyncCoreObjectMediator::SyncSyncVariantHolder(uint32 uTargetGlobalID, bool bSyncStaticSync, bool bSyncAgileSync, uint32 uSessionID)
{
	MapSyncObj_t::iterator iter = m_mapSyncObj.find(uTargetGlobalID);
	if (iter == m_mapViewObj.end() || iter->second != uSessionID)
		return;
	CSyncCoreObjectServer* pSyncCoreObj  = CSyncCoreObjectServer::GetObject(uTargetGlobalID);
	if (pSyncCoreObj && pSyncCoreObj->CastToObjDictator())
	{
		pSyncCoreObj->CastToObjDictator()->SyncSyncVariantHolder(m_pConn, bSyncStaticSync, bSyncAgileSync);
	}
}

#ifdef DIRECTOR_SEND_MOVED_DIST
bool CSyncCoreObjectMediator::VerifyDirMovedDist(float fDirMovedDist)
#else
bool CSyncCoreObjectMediator::CalcRealMovedDist(float& fRealMovedDist)
#endif
{
	Ast(m_pFolState);
	Ast(m_pConn);
	Ast(m_uLastConnTime > 0);
	if(m_pConn->GetConnTime() < m_uLastConnTime)
	{
		ostringstream strm;
		strm << m_pConn->GetConnTime() << "<" << m_uLastConnTime
			<< "(" << m_pConn->GetServerTime() << "," << m_pConn->m_uLastClientTime 
			<< "," << m_pConn->m_uDiscardedConnTime
			<< "," << m_pConn->m_uCurFrameDiscardedConnTime << ")";
		GenErr("m_pConn->GetConnTime() < m_uLastConnTime", strm.str());
	}

	const float fIdealMovedDist = m_pFolState->GetRealMovedDist();
	const float fMovedDist = m_pFolState->GetMovedDist();
#ifdef DIRECTOR_SEND_MOVED_DIST
	if(fDirMovedDist < m_fLastDirMovedDist)//等于情况是收到Tell_Mediator_Max_Speed_Set消息
	{
		ostringstream strm;
		strm << fDirMovedDist << " < " << m_fLastDirMovedDist;
		GenErr("fDirMovedDist < m_fLastDirMovedDist", strm.str());
	}
#else
	float fDirMovedDist = m_pFolState->CalcDirMovedDist();
#endif

	//cout << fIdealMovedDist << " " << fDirMovedDist << endl;
	//cout << "(" << fIdealMovedDist << ")" << fIdealMovedDist + sqr::CalMoveStep(m_fDirKnownMaxSpeed, 1000) << " " << fDirMovedDist << endl;
	//return true;
	if(fIdealMovedDist < fDirMovedDist)
	{
		CFPos PixelPos;
		m_pFolState->GetPixelPosOfPath(PixelPos, fIdealMovedDist);
		if(!VerifySetPosCmd(fMovedDist, fIdealMovedDist, PixelPos))
		{
			//cout << fMovedDist << "->" << fIdealMovedDist << " has barrier " << endl;
			return false;
		}

		CorrectDirectorMovingProcess();
		//cout << "dir move fast " << fIdealStepDist << "<" << fDirStepDist << " " << PixelPos.x << "," << PixelPos.y << " " << pos.x << "," << pos.y << endl;
		return false;
	}

#ifndef DIRECTOR_SEND_MOVED_DIST
	fRealMovedDist = fIdealMovedDist;
#endif

	return true;
}

void CSyncCoreObjectMediator::SetMovementCmdCheckPoint(float fDirMovedDist)
{
	Ast(m_pFolState);
	Ast(m_pConn);

	m_fLastDirMovedDist	= fDirMovedDist;
	m_uLastConnTime = m_pConn->GetConnTime();
}

void CSyncCoreObjectMediator::CorrectDirectorMovingProcess()
{
	Ast(m_pFolState);
	Ast(m_pConn);
	const float fDirectorMaxSpeed = GetDirectorMaxSpeed();
	const uint8 uFindPathType = m_pFolState->GetFindPathType();
	const uint8 uBarrierType = m_pFolState->GetBarrierType();
	CFPos posPixelDest;
	m_pFolState->GetPixelPosOfPath(posPixelDest, m_pFolState->GetEndDist());

	SetDirectorMaxSpeed(0);

	CGas2GacGC_Tell_Server_Time cmd;
	cmd.uobGlobalTime= CSyncTimeSystemServer::Inst()->GetFrameTime();
	m_pConn->SendCoreCmd(&cmd);

	CGas2GacOC_Set_Dir_Max_Speed_And_Move_To Cmd;
	Cmd.uqbGlobalID=GetGlobalID();
	Cmd.uqbSessionID = uint32(-1);
	Cmd.fDirectorMaxSpeed=fDirectorMaxSpeed;
	Cmd.PixelPos = posPixelDest;
	Cmd.usbFindPathType = uFindPathType;
	Cmd.usbBarrierType = uBarrierType;
	Cmd.uqbSessionID = m_uCurDirMaxSpeedChangeSessionID++;
	CreateDirMaxSpeedChangeSession(Cmd.uqbSessionID, Cmd.fDirectorMaxSpeed);
	m_pConn->SendCoreCmd(&Cmd);
	m_fDirectorMaxSpeed = fDirectorMaxSpeed;
	//NotifyDirToSetMaxSpeed();
}

void CSyncCoreObjectMediator::ResetDirKnownMaxSpeed()
{
	m_fDirKnownMaxSpeed = m_fDirectorMaxSpeed;
}
