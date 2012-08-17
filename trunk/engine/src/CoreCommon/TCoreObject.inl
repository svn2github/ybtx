#pragma once
#include "IPathFinding.h"
#include "TObjMoveState.h"
#include "TCoreObject.h"
#include "TMetaScene.h"
#include "CPixelPath.h"
#include "BaseHelper.h"
#include "CoreCommon.h"
#include "MoveCallArgs.h"
#include "PatternRef.inl"
#include "ErrLogHelper.h"
#include "TBaseCOEvent.h"
#include "TIDPtrMap.inl"
#include "CDistortedTimeObj.h"
#include "CLogCoreObject.h"
#include <iomanip>

template<typename Traits>
TCoreObject<Traits>::TCoreObject(ImpHandler_t* pHandler,const CFPos& PixelPos,ImpCoreScene_t* pScene)
:m_pHandler(NULL)
,m_pMovState(NULL)
,m_Pos(PixelPos)
,m_pDistortedTimeObj(NULL)
{
	m_pDistortedTimeObj = pScene->CreateDistortedTimeObj();
	m_pRefByEvent=new RefeeByEvent_t( static_cast<ImpCoreObject_t*>( this ) );

	SetHandler(pHandler);

	m_uID = static_cast<ImpCoreObject_t*>(this)->CreateID();

	pScene->AddObject( static_cast< ImpCoreObject_t* >(this) );
}

template<typename Traits>
TCoreObject<Traits>::~TCoreObject()
{
	SetHandler(NULL);

	IntStopDoingMove(); //如果没有这句，会内存泄漏

	GetScene()->DelObject( static_cast< ImpCoreObject_t* >(this) );
	Ver( static_cast< ImpCoreObject_t* >(this)->DestroyID() );

	delete m_pRefByEvent;
	GetScene()->DestroyDistortedTimeObj(m_pDistortedTimeObj);
}

template<typename Traits>
void TCoreObject<Traits>::GetGridPos(CPos& GridPos)const
{
	PixelPos2GridPos(GridPos,GetPixelPos());
}

template<typename Traits>
const CFPos& TCoreObject<Traits>::GetPixelPos()const
{
	return m_Pos;
}

template<typename Traits>
void TCoreObject<Traits>::GetPixelPos(CFPos& pos)
{
	pos = GetPixelPos();
}

template<typename Traits>
uint32 TCoreObject<Traits>::GetLocalID()const
{
	return m_uID;
}

template<typename Traits>
typename TCoreObject<Traits>::ImpHandler_t*
TCoreObject<Traits>::GetHandler()const
{
	return m_pHandler;
}

template<typename Traits>
void TCoreObject<Traits>::SetHandler(ImpHandler_t* pHandler)
{
	AdvDelRef(m_pHandler);
	m_pHandler=pHandler;
	AdvAddRef(m_pHandler);
}

template<typename Traits>
typename TCoreObject<Traits>::ImpCoreScene_t*
TCoreObject<Traits>::GetScene()const
{
	return static_cast<const ImpCoreObject_t*>(this)->IntGetScene();
}


template<typename Traits>
typename TCoreObject<Traits>::ImpObjMoveState_t*
TCoreObject<Traits>::GetMovState()const
{
	return m_pMovState;
}

template<typename Traits>
bool TCoreObject<Traits>::IntSetPositionSingle(const CFPos& PixelPos)
{
	ImpMetaScene_t* pMetaScene=GetScene()->GetMetaScene();

	const int32 nRatio = eGridSpanForObj / eGridSpan;

	if( ( float( pMetaScene->GetWidthInPixel() ) * nRatio > PixelPos.x )
		&&
		( float( pMetaScene->GetHeightInPixel() ) * nRatio > PixelPos.y ))
	{
		static_cast<ImpCoreObject_t*>(this)->IntSetPositionDerived(PixelPos);
		return true;
	}

	return false;
}

template<typename Traits>
void TCoreObject<Traits>::IntSetPositionDerived(const CFPos& PixelPos)
{
	m_Pos = PixelPos;
}
 
// 鼠标操作寻路
template<typename Traits>
EMoveToResult TCoreObject<Traits>::IntMoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,
											 EBarrierType eBarrierType,float fReachDist, float fMovedDist, CPixelPath* pPixelPath, int32 nMaxStep)
{
	Ast(fSpeed > 0.0f);
	ImpObjMoveState_t* pMoveState=m_pMovState;

	EMoveToResult eResult;

	fReachDist*=eGridSpanForObj;

	if( pMoveState )
	{
#ifdef LOG_COREOBJ_MOVE
		ostringstream strm;
		strm << "IntMoveTo move_path_change " << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y << " fReachDist = " << fReachDist;
		static_cast<ImpCoreObject_t*>(this)->LogMsg(strm.str());
#endif
		if(pPixelPath)
		{
			m_pMovState->UpdateCurPosByMovedDist(fMovedDist);
			//CFPos beginPos;
			//pPixelPath->GetBeginPixelPos(beginPos);
			//if(GetPixelPos() != beginPos)
			//{
			//	cout << GetPixelPos().x << "." << GetPixelPos().y << "!=" << beginPos.x << "," << beginPos.y << " " << fMovedDist << endl;
			//}
		}
		else
		{
			m_pMovState->UpdateCurPosByRealTime();
		}

#ifdef LOG_COREOBJ_MOVE
		ostringstream stream;
		CFPos beginPathPos;
		m_pMovState->GetPath()->GetBeginPixelPos(beginPathPos);
		stream << "old path: len = " << m_pMovState->GetPath()->GetPathPixelLength() << " beginpos " << beginPathPos.x << "," << beginPathPos.y;
		stream << "new path: begin pos " << GetPixelPos().x << "," << GetPixelPos().y;
		static_cast<ImpCoreObject_t*>(this)->LogMsg(stream.str());
#endif

		const float fChangeDist=pMoveState->GetMovedDist();

		eResult = DoMove(GetPixelPos(), PixelPosDes, fSpeed, eFindPathType, eBarrierType, 0, pPixelPath, nMaxStep);
		if( eResult == eMTR_Success )
		{
			CFPos EndPos;
			m_pMovState->GetPath()->GetEndPixelPos(EndPos);
			m_pMovState->SetReachDist( fReachDist );
#ifdef LOG_COREOBJ_MOVE
			ostringstream strm;
			strm << "path original_len " << m_pMovState->GetPath()->GetPathPixelLength() << " cut_len " << m_pMovState->GetPathLength() << " fReachDist " << fReachDist;
			CFPos BeginPos;
			m_pMovState->GetPath()->GetBeginPixelPos(BeginPos);
			if(pPixelPath)
			{
				CFPos pixelPathBeginPos;
				pPixelPath->GetBeginPixelPos(pixelPathBeginPos);
				strm << " pixelpathbeginpos " << pixelPathBeginPos.x << "," << pixelPathBeginPos.y;
			}
			strm << " beginpos" << BeginPos.x << "," << BeginPos.y << " endpos" << EndPos.x << "," << EndPos.y;
			const PathList& pathList = m_pMovState->GetPath()->GetGridPath()->GetPath();
			for(uint32 u = 0; u < pathList.size(); ++u)
				strm << " " << pathList[u].x << "," << pathList[u].y;
			static_cast<ImpCoreObject_t*>(this)->LogMsg(strm.str());
#endif

			CMovePathChangedDerivedArgs Args=
			{
				EndPos,
				m_pMovState->GetSpeed(),
				eFindPathType,
				eBarrierType,
				m_pMovState->GetPathLength(),
				fChangeDist,
				m_pMovState->GetPath()
			};

			//cout << "cmt speed " << m_pMovState->GetSpeed() << endl;
			static_cast<ImpCoreObject_t*>(this)->OnMovePathChangedDerived(Args);
		}
	}
	else
	{
#ifdef LOG_COREOBJ_MOVE
		ostringstream strm;
		strm << "IntMoveTo begin_move " << setprecision(12) << GetPixelPos().x << "," << GetPixelPos().y << " fReachDist = " << fReachDist;
		//CFPos beginPathPos;
		//m_pMovState->GetPath()->GetBeginPixelPos(beginPathPos);
		//strm << "new path: len = " << m_pMovState->GetPath()->GetPathPixelLength() << " beginpos " << beginPathPos.x << "," << beginPathPos.y;
		static_cast<ImpCoreObject_t*>(this)->LogMsg(strm.str());
#endif

		CFPos BeginPixelPos;
		if(pPixelPath)
		{
			pPixelPath->GetBeginPixelPos(BeginPixelPos);
			if(BeginPixelPos != GetPixelPos())
			{
				//cout << "SyncObj has sent OnMoveEnded msg, AsynObj did not receive it yet" << endl; 
				IntSetPixelPos(BeginPixelPos);
			}
		}
		else
		{
			BeginPixelPos = GetPixelPos();
		}
		eResult = DoMove(BeginPixelPos,PixelPosDes,fSpeed,eFindPathType,eBarrierType,0, pPixelPath, nMaxStep);
		if(eResult==eMTR_Success)
		{
			CFPos EndPos;
			m_pMovState->GetPath()->GetEndPixelPos(EndPos);
			m_pMovState->SetReachDist( fReachDist );
#ifdef LOG_COREOBJ_MOVE
			ostringstream strm;
			CFPos BeginPos;
			m_pMovState->GetPath()->GetBeginPixelPos(BeginPos);
			strm << "path original_len " << m_pMovState->GetPath()->GetPathPixelLength() << " cut_len " << m_pMovState->GetPathLength() << " fReachDist " << fReachDist << " beginpos (" << BeginPos.x << "," << BeginPos.y << ") curpos (" << GetPixelPos().x << "," << GetPixelPos().y << ")";
			static_cast<ImpCoreObject_t*>(this)->LogMsg(strm.str());
#endif

			CMoveBeganDerivedArgs Args=
			{
				EndPos,
				m_pMovState->GetSpeed(),
				eFindPathType,
				eBarrierType,
				m_pMovState->GetPathLength(),
				m_pMovState->GetPath()
			};

			static_cast<ImpCoreObject_t*>(this)->OnMoveBeganDerived(Args);
		}
	}
	if(pPixelPath && eResult != eMTR_Success)
	{
		pPixelPath->Release();
	}
		
	return eResult;
}

// 鼠标操作寻路
template<typename Traits>
EMoveToResult TCoreObject<Traits>::DoMove(const CFPos& PixelPosBegin,const CFPos& PixelPosEnd ,float fSpeed,
										  EFindPathType eFindPathType,EBarrierType eBarrierType,float fBeginDist,
										  CPixelPath* pPath, int32 nMaxStep)
{
	if( PixelPosBegin.x==PixelPosEnd.x &&
		PixelPosBegin.y==PixelPosEnd.y )
		return eMTR_SuperPosition;

	if(!pPath)
		pPath=GetScene()->CreatePath(PixelPosBegin,PixelPosEnd,eFindPathType,eBarrierType, nMaxStep); 
	//if(pPath)
	//	pPath->Release();
	//pPath=GetScene()->CreatePath(PixelPosBegin,PixelPosEnd,eFindPathType,eBarrierType, nMaxStep);

	if(!pPath)
		return eMTR_FindPathFailed;

	DoMove(pPath,fSpeed,fBeginDist);

	return eMTR_Success;
}

// 寻路结束，键盘/鼠标操作寻路汇合
template<typename Traits>
void TCoreObject<Traits>::DoMove(CPixelPath* pPath,float fSpeed,float fBeginDist)
{
	//cout<<m_uID<<"\tTCoreObject<Traits>::DoMove Speed is\t"<<fSpeed<<endl;
	Ast(pPath);

	if( m_pMovState ) // 目前走的是else分支
	{
		//cout << "newpath " << endl;
		m_pMovState->SetNewPathDerived(pPath,fBeginDist);
		m_pMovState->IntSetSpeedDerived(fSpeed);
	}
	else
	{
#ifdef LOG_COREOBJ_MOVE
		static_cast<ImpCoreObject_t*>(this)->LogMsg("Create ObjMoveState");
#endif
		//cout << "cte move" << endl;
		m_pMovState=new ImpObjMoveState_t(static_cast<ImpCoreObject_t*>(this),pPath,fSpeed,fBeginDist);
		m_pMovState->IntSetSpeedDerived( fSpeed );
	}
}


template<typename Traits>
bool TCoreObject<Traits>::IntStopDoingMove()
{
	if(!m_pMovState)
		return false;

	delete m_pMovState;
	m_pMovState=NULL;
	return true;
}

template<typename Traits>
void TCoreObject<Traits>::IntOnMoveEnded()
{
	CPos pos;
	GetGridPos(pos);
	

#define ENDMOVE
#ifdef ENDMOVE

	const CPixelPath* pPath=m_pMovState->GetPath();

#else
#ifdef LOG_COREOBJ_MOVE
		ostringstream strm;
		strm << "Del ObjMS" << " PathLen " << m_pMovState->GetPathLength() << " origin path " << m_pMovState->GetPath()->GetPathPixelLength();
		static_cast<ImpCoreObject_t*>(this)->LogMsg(strm.str());
#endif
	delete m_pMovState;
	m_pMovState = NULL;

#endif
	
	//如果你在这个回调里面调用了IntMoveTo，肯定会挂得很惨，正确的办法是调用DoMove。
	static_cast<ImpCoreObject_t*>(this)->OnEndMoveDerived();

#ifdef ENDMOVE

	if( pPath == m_pMovState->GetPath() )
	{
#ifdef LOG_COREOBJ_MOVE
		ostringstream strm;
		strm << "Del ObjMS" << " PathLen " << m_pMovState->GetPathLength() << " origin path " << m_pMovState->GetPath()->GetPathPixelLength();
		static_cast<ImpCoreObject_t*>(this)->LogMsg(strm.str());
#endif
		//OnEndMoveDerived没有调用DoMove
		delete m_pMovState;
		m_pMovState = NULL;
	}
#endif
}

template<typename Traits>
ESetSpeedResult TCoreObject<Traits>::IntSetSpeed(float fSpeed)
{
	if( fSpeed==0 )
		return static_cast<ESetSpeedResult>(IntStopMoving());

	if( fSpeed<0 )
		return eSSR_InvalidSpeed;

	if( !m_pMovState )
		return eSSR_IsNotMoving;

	if(!m_pMovState->SetSpeed( fSpeed ))
		return eSSR_Success;

	CMoveSpeedChangedDerivedArgs Args={fSpeed};

	static_cast<ImpCoreObject_t*>(this)->OnMoveSpeedChangedDerived(Args);
	return eSSR_Success;
}


template<typename Traits>
bool TCoreObject<Traits>::IntIsDoingMove()const
{
	return GetMovState() != NULL;
}

template<typename Traits>
EStopMovingResult TCoreObject<Traits>::IntStopMoving()
{
	if( !m_pMovState )
		return eSMR_IsNotMoving;

	m_pMovState->UpdateCurPosByRealTime();

	float fMovedDist=m_pMovState->GetMovedDist();

	//cout<< m_uID <<"\tTCoreObject<Traits>::IntStopMoving" << endl;
#ifdef LOG_COREOBJ_MOVE
	static_cast<ImpCoreObject_t*>(this)->LogMsg("IntStopMoving");
#endif
	CFPos PixelPos, CalcPos;
	PixelPos = GetPixelPos();
#ifdef LOG_COREOBJ_MOVE
	if(m_pMovState->GetPath()->GetPixelPosOfPath(CalcPos, fMovedDist) == uint16(-1) || PixelPos != CalcPos)
	{
		ostringstream strm;
		strm << "PixelPos doesnt match movedDist movedDist = " << fMovedDist << " PixelPos != CalcPos " << PixelPos.x << "," << PixelPos.y << " != " << CalcPos.x << "," << CalcPos.y;
		static_cast<ImpCoreObject_t*>(this)->LogMsg(strm.str());
		//GenErr(strm.str());
	}
#endif

	Ver( IntStopDoingMove() );

	static_cast<ImpCoreObject_t*>(this)->OnMoveStoppedDerived( fMovedDist );

	return eSMR_Success;
}


template<typename Traits>
ESetPosResult TCoreObject<Traits>::IntSetPixelPos(const CFPos& PixelPos )
{
	if(m_pMovState)
	{
		return eSPR_IsMoving;
	}

	bool bSuperPosition=GetPixelPos()==PixelPos;

	ESetPosResult eResult=IntSetPosition(PixelPos)?eSPR_Success:eSPR_OutOfBound;

	static_cast<ImpCoreObject_t*>(this)->OnPixelPosSetDerived( bSuperPosition );

	return eResult;
}

template<typename Traits>
bool TCoreObject<Traits>::IntGetCurPixelWayPoint( CFPos& PixelPos )const
{
	

	if( !m_pMovState )
		return false;

	GetMovState()->GetCurWayPoint( PixelPos );
	return true;
}

template<typename Traits>
typename TCoreObject<Traits>::MapObject_t& TCoreObject<Traits>::GetObjectMap()
{
	static MapObject_t ls_Inst;
	return ls_Inst;
}

template<typename Traits>
typename TCoreObject<Traits>::ImpCoreObject_t*
TCoreObject<Traits>::GetObject(uint32 uLocalID)
{
	ImpCoreObject_t* pObj;
	return GetObjectMap().Get( pObj, uLocalID ) ? pObj : NULL;	
}

template<typename Traits>
void TCoreObject<Traits>::OnWayPointChangedDerived()
{
}


template<typename Traits>
void TCoreObject<Traits>::RawSetPixelPos(const CFPos& PixelPos)
{
	m_Pos = PixelPos;
}

template<typename Traits>
float TCoreObject<Traits>::GetSpeed()const
{
	if( !m_pMovState )
		return 0.0f;

	return GetMovState()->GetSpeed();
}

template<typename Traits>
CDistortedTimeObj* TCoreObject<Traits>::GetDistortedTimeObj()const
{
	return m_pDistortedTimeObj;
}


template<typename Traits>
bool TCoreObject<Traits>::IntSetPosition(const CFPos& PixelPos)
{
	bool bRet = IntSetPositionSingle(PixelPos);
	if(!bRet)
	{
		return bRet;
	}
	return static_cast<ImpCoreObject_t*>(this)->BindObjIntSetPosition(PixelPos);
}

//template<typename Traits>
//ESetPosResult TCoreObject<Traits>::IntSetPixelPos( const CFPos& PixelPos )
//{
//	ESetPosResult eRet = IntSetPixelPosSingle(PixelPos);
//	if(eRet != eSPR_Success)
//	{
//		return eRet;
//	}
//	return static_cast<ImpCoreObject_t*>(this)->BindObjIntSetPixelPos(PixelPos, eRet);
//}



template<typename Traits>
IDistortedTime* TCoreObject<Traits>::GetDistortedTime()const
{
	return m_pDistortedTimeObj;
}

template<typename Traits>
float TCoreObject<Traits>::GetRealMovedDistDerived()const
{
	return GetMovState()->IntGetRealMovedDist();
}
