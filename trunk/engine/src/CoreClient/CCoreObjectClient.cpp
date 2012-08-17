#include "stdafx.h"
#include "CCoreObjectClient.h"
#include "CCoreSceneClient.h"
#include "ICoreObjectClientHandler.h"
#include "CTimeSystemClient.h"
#include "CMetaSceneClient.h"
#include "CAppClient.h"
#include "CCoreSceneMgrClient.h"
#include "COEventClient.h"
#include "CCOEventWayPointChanged.h"
#include "CObjMoveStateClient.h"
#include "TCoreObject.inl"
#include "CCoreMsgMgr.h"
#include "MoveCallArgs.h"
#include "ErrLogHelper.h"
#include "TIDPtrMap.inl"
#include "TSqrAllocator.inl"
#include "CDistortedTimeObjClient.h"
#include "CRenderTime.h"
template class TCoreObject<CTraitsClient>;

CCoreObjectClient::CCoreObjectClient(ICoreObjectClientHandler* pHandler,const CFPos& PixelPos)
:ParentObject_t(pHandler,PixelPos,CCoreSceneClient::MainScene())
{
}

CCoreObjectClient::~CCoreObjectClient()
{
}

uint32 CCoreObjectClient::CreateID()
{
	return GetObjectMap().Add(this);
}

bool CCoreObjectClient::DestroyID()
{
	return GetObjectMap().Del(GetLocalID());
}

CCoreSceneClient* CCoreObjectClient::IntGetScene()const
{
	return CCoreSceneClient::MainScene();
}

void CCoreObjectClient::OnMoveSteppedDerived(const CFPos&)
{
}

bool CCoreObjectClient::GetCurPixelWayPoint(CFPos& PixelPos)const
{
	return IntGetCurPixelWayPoint(PixelPos);
}

bool CCoreObjectClient::GetCurGridWayPoint(CPos& GridPos)const
{
	CFPos PixelPos;
	if( !GetCurPixelWayPoint(PixelPos) )
		return false;

	GridPos.x = GetGridByPixel( PixelPos.x );
	GridPos.y = GetGridByPixel( PixelPos.y );
	return true;
}

void CCoreObjectClient::OnWayPointChangedDerived()
{	
	CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())
		CCOEventWayPointChanged(this));
}

void CCoreObjectClient::OnMoveBeganDerived(const CMoveBeganDerivedArgs& Args)
{
	//cout<<GetLocalID()<<"\tCCoreObjectClient::OnMoveBeganDerived"<<endl;
	CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())
		CCOEventMoveBeganClient(this));
}

void CCoreObjectClient::OnEndMoveDerived()
{
	//cout<<GetLocalID()<<"\tCCoreObjectClient::OnEndMoveDerived"<<endl;
	CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())
		CCOEventMoveEndedClient(this,eMET_Reached));
}

void CCoreObjectClient::OnMoveStoppedDerived(float)
{
	//cout<<GetLocalID()<<"\tCCoreObjectClient::OnMoveStoppedDerived"<<endl;
	CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())
		CCOEventMoveEndedClient(this,eMET_Stopped));
}

void CCoreObjectClient::OnMovePathChangedDerived(const CMovePathChangedDerivedArgs& Args)
{
	CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())
		CCOEventMovePathChangedClient(this));
}

void CCoreObjectClient::OnMoveSpeedChangedDerived(const CMoveSpeedChangedDerivedArgs& Args)
{
}

void CCoreObjectClient::OnPixelPosSetDerived(bool bSuperPosition)
{
}

bool CCoreObjectClient::IsMoving()const
{
	return IntIsDoingMove();
}

void CCoreObjectClient::IntSetPositionDerived(const CFPos& PixelPos)
{
	ParentObject_t::IntSetPositionDerived( PixelPos );
	ICoreObjectClientHandler* pHandler = GetHandler();
	if(pHandler)
		pHandler->OnRefreshRenderObjPos(PixelPos);
	CCoreMsgMgr::Inst().PostMsg(new (CCoreMsgMgr::Inst())CCOEventPosChangedClient(this));
}

void CCoreObjectClient::LogMsg(const string& log)
{
}

#ifdef USE_RENDER_SLOT
void CCoreObjectClient::OnSpeedAdjust(uint32 uMoveInterval)
{
}
#endif

float CCoreObjectClient::GetRealMovedDistDerived()const
{
	return ParentObject_t::GetRealMovedDistDerived();
}

