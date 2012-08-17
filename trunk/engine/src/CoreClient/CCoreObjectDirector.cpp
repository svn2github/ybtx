#include "stdafx.h"
#include "CCoreObjectDirector.h"
#include "ICoreObjectDirectorHandler.h"
#include "CObjMoveStateClient.h"
#include "PrtlGas2GacObjectDef.h"
#include "PrtlGac2GasObjectDef.h"
#include "CDirObjActiveState.h"
#include "ExpHelper.h"
#include "CConnClient.inl"
#include "TConnection_inl.inl"
#include "CRenderSystemClient.h"
#include "CCoreSceneClient.h"
#include "CoreCommon.h"
#include "CoreObjectCommon.h"
#include "COEventClient.h"
#include "MoveCallArgs.h"
#include "TPackedPos.inl"
#include "BaseHelper.h"
#include "CPathTransformer.h"
#include "CPixelPath.h"
#include "CCoreMsgMgr.h"
#include "ICoreSceneClientHandler.h"
#include "CDistortedTimeObjClient.h"
#include "CTimeSystemClient.h"
#include "CoreObjectCommon.h"
#include <iomanip>

CCoreObjectDirector::CCoreObjectDirector(const CGas2GacOC_Create_Still_Object* pCmd)
:CCoreObjectFollower(pCmd)
{
	if(GetLastDistortedProcessTime() != 0 && GetLastProcessTime() != 0)
	{
		static_cast<CDistortedTimeObjClient*>(GetDistortedTimeObj())->
			SetLastDistortedProcessTime(GetLastDistortedProcessTime(), GetLastProcessTime());
	}

	if(GetLastDistortedFrameTime() != 0 && GetLastFrameTime() != 0)
	{
		static_cast<CDistortedTimeObjClient*>(GetDistortedTimeObj())->
			SetLastDistortedFrameTime(GetLastDistortedFrameTime(), GetLastFrameTime());
	}

	m_pActState				= NULL;
	m_fDirectorMaxSpeed		= 0;
	m_bWaitingToBeActive	= false;
	m_bActive				= pCmd->usbTypeFlag==eSOF_Active_Director;

	ControlCamera(true);
	SetViewVariantInitEnd();
	SetSyncVariantInitEnd();
}

CCoreObjectDirector::~CCoreObjectDirector(void)
{
	GetLastDistortedProcessTime() = GetDistortedTimeObj()->GetDistortedProcessTime();
	GetLastProcessTime() = GetProcessTime();
	GetLastDistortedFrameTime() = static_cast<CDistortedTimeObjClient*>(GetDistortedTimeObj())->GetDistortedFrameTime();
	GetLastFrameTime() = CTimeSystemClient::Inst()->GetFrameTime();

	ControlCamera(false);

	if( !m_pActState )
		return;

	if( IntIsDoingMove() )
		IntStopMoving();

	delete m_pActState;
	m_pActState=NULL;
}

void CCoreObjectDirector::TellMediatorActivated()
{
	CGac2GasOC_Director_Is_Activated cmd;
	CConnClient::Inst()->SendCoreCmd(&cmd);
}

void CCoreObjectDirector::RefreshCameraDest(const CFPos& pos)
{
	if (CClientRenderScene::HasInst())
	{
		float fRatio = static_cast<float>(eGridSpanForObj / eGridSpan);
		CClientRenderScene::GetInst()->SetCameraDest(pos.x  / fRatio,pos.y  / fRatio);
	}
}

EClientObjectType CCoreObjectDirector::GetType()const
{
	return eCOT_Director;
}

ICoreObjectDirectorHandler* CCoreObjectDirector::GetHandler()const
{
	ICoreObjectFollowerHandler* pHandler=CCoreObjectFollower::GetHandler();
	return pHandler?pHandler->CastToObjDirHandler():NULL;
}

bool CCoreObjectDirector::IsPassive()const
{
	return !m_bActive;
}

void CCoreObjectDirector::OnMoveBeganDerived(const CMoveBeganDerivedArgs& Args)
{
	Ast( !IsPassive() );

	Ast( !m_pActState );
	m_pActState=new CDirObjActiveState;
	//m_pActState->m_fLastSentMovedDist = GetPixelPos();
	
#ifdef LOG_COREOBJ_MOVE
	ostringstream strm;
	strm << "Move_Mediator_To\t" << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y;
	LogMsg(strm.str());
#endif

	CGac2GasOC_Move_Mediator_To cmd;
	cmd.fSpeed = Args.fSpeed;
	cmd.EndPixelPos=Args.EndPixelPos;
	cmd.fEndDist = Args.fEndDist;
	cmd.usbFindPathType = Args.eFindPathType;
	cmd.usbBarrierType = Args.eBarrierType;
	cmd.PixelPos = GetPixelPos();
	
	CPathTransformer pathData(Args.pPixelPath);
	cmd.udbPathLen = pathData.GetDataLenInByte();

	SendCmdToMediator(&cmd);
	CConnClient::Inst()->TConnection<CTraitsClient>::SendCoreCmd(pathData.GetData(), pathData.GetDataLenInByte());

	CCoreObjectClient::OnMoveBeganDerived(Args);
	Ast( Args.pPixelPath->GetPathPixelLength() >= Args.fEndDist );
}


void CCoreObjectDirector::OnEndMoveDerived()
{
	if(IsPassive())
	{
		Ast(!m_pActState);
		CCoreObjectFollower::OnEndMoveDerived();
		if( !HasUnfinishedPassiveMovement() && m_bWaitingToBeActive )
			SwitchActiveMode();
	}
	else
	{
#ifdef LOG_COREOBJ_MOVE
		ostringstream strm;
		strm << "Tell_Mediator_Reached\t" << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y;
		LogMsg(strm.str());
#endif
		CGac2GasOC_Tell_Mediator_Reached cmd;
		cmd.uqbTimeStamp = (uint32)m_pActState->m_tcpBegin.GetElapse();
		cmd.EndPixelPos = GetPixelPos();

		SendCmdToMediator(&cmd);

		Ast(m_pActState);
		delete m_pActState;
		m_pActState=NULL;

		CCoreObjectClient::OnEndMoveDerived();
		CClientRenderScene::GetInst()->SetCameraMaxSpeed(0.0f);
	}
}


void CCoreObjectDirector::OnMoveStoppedDerived(float fStopDist)
{
#ifdef LOG_COREOBJ_MOVE
	ostringstream strm;
	strm << "Tell_Mediator_Stopped\t" << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y;
	LogMsg(strm.str());
#endif
	CGac2GasOC_Tell_Mediator_Stopped cmd;
#ifdef DIRECTOR_SEND_MOVED_DIST
	cmd.fMovedDist = fStopDist;
#endif
	cmd.EndPixelPos = GetPixelPos();
	cmd.uqbTimeStamp = (uint32)m_pActState->m_tcpBegin.GetElapse();

	delete m_pActState;
	m_pActState=NULL;

	SendCmdToMediator(&cmd);

	CClientRenderScene::GetInst()->SetCameraMaxSpeed(0.0f);
	CCoreObjectClient::OnMoveStoppedDerived(fStopDist);
}


const static float gs_fMaxSentMovedDist = (float)g_uPlayerAoiPrecision*eGridSpan / 2;

void CCoreObjectDirector::OnMoveSteppedDerived( const CFPos& OldPixelPos )
{
	if( IsPassive() )
	{
		CCoreObjectClient::OnMoveSteppedDerived( OldPixelPos );
	}
	else
	{
		const float fMovedDist = GetMovState()->GetMovedDist();
		
		Ast(fMovedDist >= m_pActState->m_fLastSentMovedDist);
		
		bool bNeedToTell = ( fMovedDist - m_pActState->m_fLastSentMovedDist > gs_fMaxSentMovedDist );

		if( !bNeedToTell )
			bNeedToTell = m_pActState->m_tcpLastTellStep.GetElapse() > 1000;

		if( bNeedToTell )
		{
			//float fMovedDistToTell = m_pActState->m_fLastSentMovedDist + gs_fMaxSentMovedDist;

			CGac2GasOC_Tell_Mediator_Stepped cmd;

			//for( ; fMovedDistToTell < fMovedDist; fMovedDistToTell += gs_fMaxSentMovedDist )
			//{
			//	cmd.fMovedDist = fMovedDistToTell;
	
			//	SendCmdToMediator(&cmd);
			//}
#ifdef DIRECTOR_SEND_MOVED_DIST
			cmd.fMovedDist = fMovedDist;
#endif

			SendCmdToMediator(&cmd);

			m_pActState->m_tcpLastTellStep.SetCheckPoint();
			m_pActState->m_fLastSentMovedDist = fMovedDist;
		}
		
		Ast( GetMovState()->GetMovedDist() <= GetMovState()->GetPathLength() );

		/*CGac2GasOC_Report_Invalid_Step temp;
		for( int i=0;i<1000;++i)
			CConnClient::Inst()->SendCoreCmd(&temp);*/
	}
}

template<typename CmdType>
void CCoreObjectDirector::SendCmdToMediator(CmdType* pCmd)const
{
	pCmd->uobClientKnownServerFrameTime = CConnClient::Inst()->GetLastSyncedServerFrameTime();
	pCmd->uobClientTime = CTimeSystemClient::Inst()->GetFrameTime();
	CConnClient::Inst()->SlowSignOnMediatorCmd(pCmd);
	CConnClient::Inst()->TConnection<CTraitsClient>::SendCoreCmd(pCmd);

	if(pCmd->uobClientKnownServerFrameTime > 100000000000000000)
	{
		ostringstream strm;
		strm << typeid(CmdType).name() << pCmd->uobClientKnownServerFrameTime;
		LogErr("ClientKnownServerFrameTime Err", strm.str());
	}
}


ESetPosResult CCoreObjectDirector::SetPixelPos(const CFPos& PixelPos)
{
	if(IsPassive())
		return eSPR_NotAllowed;
	return IntSetPixelPos(PixelPos);
}

ESetPosResult CCoreObjectDirector::SetGridPos(const CPos& GridPos)
{
	return SetPixelPos( CFPos(GetPixelOfGrid(GridPos.x),GetPixelOfGrid(GridPos.y)) );
}

void CCoreObjectDirector::GetServerPixelPos(CFPos& PixelPos)const
{
	if(m_pActState)
	{
		if(!GetMovState())
			GenErr("MoveState is NULL");
		GetMovState()->GetPath()->GetPixelPosOfPath(PixelPos, m_pActState->m_fLastSentMovedDist);
	}
	else
	{
		CCoreObjectFollower::GetServerPixelPos(PixelPos);
	}
}

float CCoreObjectDirector::GetMaxSpeed()const
{
	return m_fDirectorMaxSpeed;
}

bool CCoreObjectDirector::CheckSpeed(float fSpeed)const
{
	if( fSpeed > m_fDirectorMaxSpeed )
	{
		ostringstream strm;
		strm << "speed " << fSpeed << " mediation_speed " << m_fDirectorMaxSpeed;
		GenErr("Directior 移动速度不可超过仲裁对象允许的速度", strm.str());
		return false;
	}
	return true;
}

ESetSpeedResult CCoreObjectDirector::SetSpeed(float fSpeed)
{
	if(IsPassive())
		return eSSR_NotAllowed;

	if(!CheckSpeed(fSpeed))
		return eSSR_NotAllowed;

	return IntSetSpeed(fSpeed);
}

EMoveToResult CCoreObjectDirector::MoveToLimit(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,
											   EBarrierType eBarrierType,float fReachDist, int32 nMaxStep)
{
	if(IsPassive())
		return eMTR_NotAllowed;

	if(!CheckSpeed(fSpeed))
		return eMTR_NotAllowed;

	return IntMoveTo(PixelPosDes,fSpeed,eFindPathType,eBarrierType,fReachDist, NULL, 0, nMaxStep);
}

EMoveToResult CCoreObjectDirector::MoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType
										  ,EBarrierType eBarrierType,float fReachDist)
{
	if(IsPassive())
		return eMTR_NotAllowed;

	if(!CheckSpeed(fSpeed))
		return eMTR_NotAllowed;

	return IntMoveTo(PixelPosDes,fSpeed,eFindPathType,eBarrierType,fReachDist,NULL,0);
}

EStopMovingResult CCoreObjectDirector::StopMoving()
{
	if(IsPassive())
		return eSMR_NotAllowed;

	return IntStopMoving();
}

bool CCoreObjectDirector::IsMoving()const
{
	if(!IsPassive())
	{
		Ast( (m_pActState != NULL) == CCoreObjectClient::IsMoving() );
		return CCoreObjectClient::IsMoving();
	}
	return CCoreObjectFollower::IsMoving();
}

CPixelPath* CCoreObjectDirector::GetClonedMovePath()const
{
	if(!IsPassive() && IsMoving())
	{
		return GetMovState()->GetPath()->Clone();
	}
	return NULL;

}

void CCoreObjectDirector::SwitchActiveMode()
{
	m_bWaitingToBeActive=false;
	m_bActive=true;
}


void CCoreObjectDirector::StopMovingWhenPassThrough()
{
	if( m_bWaitingToBeActive )
		SwitchActiveMode();

	CCoreObjectFollower::StopMovingWhenPassThrough();
}

void CCoreObjectDirector::TellMedMaxSpeedSet(uint32 uSessionID)const
{
	const float fMovedDist = GetMovState() ? GetMovState()->GetMovedDist() : 0;

	CGac2GasOC_Tell_Mediator_Max_Speed_Set cmd;
	cmd.uqbSessionID = uSessionID;
#ifdef DIRECTOR_SEND_MOVED_DIST
	cmd.fMovedDist = fMovedDist;
#endif

	SendCmdToMediator(&cmd);
}

void CCoreObjectDirector::OnServerCommand(const CGas2GacOCI_Director_Create_End* pCmd)
{
	GetScene()->GetHandler()->OnDirectorCreated(this);
}

void CCoreObjectDirector::OnServerCommand(const CGas2GacOC_Set_Dir_Max_Speed* pCmd)
{
	if(m_fDirectorMaxSpeed == 0 && pCmd->fDirectorMaxSpeed > 0)
	{
		if( IsPassive() )
		{
			TellMediatorActivated();

			if( HasUnfinishedPassiveMovement() )
			{
				m_bWaitingToBeActive=true;
			}
			else
			{
				SwitchActiveMode();
			}
		}
	}

	m_fDirectorMaxSpeed = pCmd->fDirectorMaxSpeed;
	TellMedMaxSpeedSet(pCmd->uqbSessionID);
}

void CCoreObjectDirector::OnServerCommand(const CGas2GacOC_Set_Dir_Max_Speed_And_Move_To* pCmd)
{
	Ast(pCmd->fDirectorMaxSpeed > 0);

	m_fDirectorMaxSpeed = pCmd->fDirectorMaxSpeed;

	Ast( IsPassive() );

	TellMediatorActivated();

	Ast(!HasUnfinishedPassiveMovement());
	SwitchActiveMode();

	TellMedMaxSpeedSet(pCmd->uqbSessionID);

	IntMoveTo(pCmd->PixelPos,m_fDirectorMaxSpeed,EFindPathType(pCmd->usbFindPathType),EBarrierType(pCmd->usbBarrierType),0,NULL,0);

	//MoveTo
}

void CCoreObjectDirector::OnServerCommand(const CGas2GacOC_Set_Director_Active* pCmd)
{
	m_fDirectorMaxSpeed = pCmd->fDirectorMaxSpeed;

	Ast( IsPassive() );

	TellMediatorActivated();

	if( HasUnfinishedPassiveMovement() )
	{
		m_bWaitingToBeActive=true;
	}
	else
	{
		SwitchActiveMode();
	}
}

void CCoreObjectDirector::OnServerCommand( const CGas2GacOC_Set_Dir_Max_Speed_Zero* pCmd )
{
	m_fDirectorMaxSpeed = 0;

	if( IsPassive() )
	{
		Ast( m_bWaitingToBeActive );
		m_bWaitingToBeActive=false;
		return;
	}

	Ast( !m_bWaitingToBeActive );
	m_bActive=false;

#ifdef LOG_COREOBJ_MOVE
	ostringstream strm;
	strm << "CGas2GacOC_Set_Dir_Max_Speed_Zero " << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y << " -> "<< pCmd->PixelPos.x << "," << pCmd->PixelPos.y;
	LogMsg(strm.str());
#endif

	IntSetPosition( pCmd->PixelPos );

	if( GetMovState() )
	{
#ifdef LOG_COREOBJ_MOVE
		LogMsg("IntStopDoingMove");
#endif
		IntStopDoingMove();
		Ast( m_pActState );
		delete m_pActState;
		m_pActState=NULL;

		CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())
			CCOEventMoveEndedClient(this,eMET_Stopped));
	}
}

//这个函数被触发的时候，Director对象肯定处于主动状态,是由客户端游戏逻辑调用MoveTo触发的
void CCoreObjectDirector::OnMovePathChangedDerived(const CMovePathChangedDerivedArgs& Args)
{
	Ast(!IsPassive());

	Ast(m_pActState);

	CGac2GasOC_Change_Mediator_Move_Target	cmd;		

#ifdef DIRECTOR_SEND_MOVED_DIST	
	cmd.fMovedDist			=	Args.fMovedDist;
#endif
	cmd.fSpeed = Args.fSpeed;
	cmd.fEndDist			=	Args.fEndDist;
	cmd.usbFindPathType = Args.eFindPathType;
	cmd.usbBarrierType = Args.eBarrierType;
	cmd.BeginPixelPos		=	GetPixelPos();
	cmd.EndPixelPos		=	Args.EndPixelPos;
	cmd.uqbTimeStamp		=	(uint32)m_pActState->m_tcpBegin.GetElapse();

	m_pActState->m_uTellStepCounter=0;
	m_pActState->m_fLastSentMovedDist = GetMovState()->GetMovedDist();
	m_pActState->m_fUnsentMovedDist = 0;
	//cout << "path change " << m_pActState->m_fLastSentMovedDist << endl;
	m_pActState->m_tcpLastTellStep.SetCheckPoint();

	//Args.pPixelPath->PrintPath();
	CPathTransformer pathData(Args.pPixelPath);
	cmd.udbPathLen = pathData.GetDataLenInByte();
	CFPos endPos;
	Args.pPixelPath->GetEndPixelPos(endPos);
	//cout << "end pos " << endPos.x << "," << endPos.y << " " << Args.EndPixelPos.x << "," << Args.EndPixelPos.y << endl;

	//pathData.PrintPath();
	SendCmdToMediator(&cmd);
	CConnClient::Inst()->TConnection<CTraitsClient>::SendCoreCmd(pathData.GetData(), pathData.GetDataLenInByte());

	CCoreObjectClient::OnMovePathChangedDerived(Args);
	Ast( Args.pPixelPath->GetPathPixelLength() >= Args.fEndDist );
}


void CCoreObjectDirector::OnMoveSpeedChangedDerived(const CMoveSpeedChangedDerivedArgs& Args)
{
	Ast(!IsPassive());

	CGac2GasOC_Change_Mediator_Move_Speed cmd;
	cmd.fSpeed=Args.fSpeed;

	SendCmdToMediator(&cmd);
}


void CCoreObjectDirector::IntSetPositionDerived(const CFPos& PixelPos)
{
	//cout << "setPos " << GetPixelPos().x << "->" << PixelPos.x << " " << GetPixelPos().y << "->" << PixelPos.y << endl;
	RefreshCameraDest(PixelPos);
	CCoreObjectClient::IntSetPositionDerived(PixelPos);
}

bool CCoreObjectDirector::ControlCamera(bool bControl)
{
	//CRenderScene* pRenderScene=CClientRenderScene::GetInst();

	//bool bCameraControlledByDirector = pRenderScene->m_bCameraControlledByDirector;
	//pRenderScene->m_bCameraControlledByDirector = bControl;

	RefreshCameraDest(GetPixelPos());
	return true;//bCameraControlledByDirector;
}

uint64& CCoreObjectDirector::GetLastDistortedProcessTime()
{
	static uint64 ms_uLastdistortedProcessTime = 0;
	return ms_uLastdistortedProcessTime;
}

uint64& CCoreObjectDirector::GetLastProcessTime()
{
	static uint64 ms_uLastProcessTime = 0;
	return ms_uLastProcessTime;
}

uint64& CCoreObjectDirector::GetLastDistortedFrameTime()
{
	static uint64 ms_uLastdistortedFrameTime = 0;
	return ms_uLastdistortedFrameTime;
}

uint64& CCoreObjectDirector::GetLastFrameTime()
{
	static uint64 ms_uLastFrameTime = 0;
	return ms_uLastFrameTime;
}
