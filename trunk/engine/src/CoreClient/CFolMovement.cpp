#include "stdafx.h"
#include "CFolMovement.h"
#include "CCoreSceneClient.h"
#include "CoreCommon.h"
#include "CPos.h"
#include "ExpHelper.h"
#include "CPixelPath.h"
#include "IPathFinding.h"
#include "CoreObjectCommon.h"
#include "CLogCoreObject.h"
#include <iomanip>
#include "CConnClient.h"
#include "CServerTimeGuesser.h"


CFolMovement::CFolMovement( CCoreSceneClient* pScene, const CFPos& PixelPosBegin ,const CFPos& PixelPosEnd
						   ,EFolMovementType eType, float fSpeed, EFindPathType eFindPathType, EBarrierType eBarrierType,float fEndDist,uint64 uServerBaseTime)
						   :m_bPathIsCut(true)	//由于加入了ReachDist的原因，Path总是被Cut过的
						   ,m_fPathLength(fEndDist)
						   ,m_fSpeed(fSpeed)
						   ,m_EstimatedServerTime(new CServerTimeGuesser)
						   ,m_fMovedDistWhenRemoteSuspended(-1)
						   ,m_fMovedDistWhenRemoteReached(-1)
						   ,m_uTotalSuspendedTime(0)
{
	m_eType = eType;

	m_pPath=pScene->CreatePath( PixelPosBegin,PixelPosEnd,eFindPathType,eBarrierType );

	if( !m_pPath )
		return;

	m_EstimatedServerTime->SetBaseServerTime(uServerBaseTime);

	//Ast( m_pPath->GetPathPixelLength() >= m_fPathLength );
	if( m_pPath->GetPathPixelLength() < m_fPathLength )
	{
		ostringstream strm;
		strm << "Client path len = " << setprecision(12) << m_pPath->GetPathPixelLength() << " Server path len = " << m_fPathLength;
#ifdef LOG_COREOBJ_MOVE
		CLogCoreObject::Inst().LogMsg(strm.str());
#endif
		GenErr("Client Path < Server Path. ", strm.str());
	}
}

CFolMovement::CFolMovement(CCoreSceneClient* pScene,const int16* pPathData, uint16 uPathDataLen, const CFPos& PixelPosBegin ,const CFPos& PixelPosEnd
				,EFolMovementType eType, float fSpeed, float fEndDist, uint64 uServerBaseTime)
				:m_bPathIsCut(false)	//由于加入了ReachDist的原因，Path总是被Cut过的
				,m_fPathLength(fEndDist)
				,m_eType(eType)
				,m_fSpeed(fSpeed)
				,m_EstimatedServerTime(new CServerTimeGuesser)
				,m_fMovedDistWhenRemoteSuspended(-1)
				,m_fMovedDistWhenRemoteReached(-1)
				,m_uTotalSuspendedTime(0)
{
	m_pPath=pScene->CreatePath(pPathData, uPathDataLen, PixelPosBegin, PixelPosEnd);

	if( !m_pPath )
		return;

	m_EstimatedServerTime->SetBaseServerTime(uServerBaseTime);

	//Ast( m_pPath->GetPathPixelLength() >= m_fPathLength );
	if( m_pPath->GetPathPixelLength() < m_fPathLength )
	{
		ostringstream strm;
		strm << "Client path len = " << setprecision(12) << m_pPath->GetPathPixelLength() << " Server path len = " << m_fPathLength;
#ifdef LOG_COREOBJ_MOVE
		CLogCoreObject::Inst().LogMsg(strm.str());
#endif
		GenErr("Client Path < Server Path. ", strm.str());
	}
}

CFolMovement::~CFolMovement()
{
	delete m_EstimatedServerTime;
	m_EstimatedServerTime = NULL;

	if( m_pPath )
	{
		m_pPath->Release();
		m_pPath=NULL;
	}
}

CFolMovement* CFolMovement::Create(CCoreSceneClient* pScene,const CFPos& PixelPosBegin,const CFPos& PixelPosEnd
								   ,EFolMovementType eType, float fSpeed, EFindPathType eFindPathType, EBarrierType eBarrierType, float fEndDist, uint64 uServerBaseTime)
{
	CFolMovement* pMm=new CFolMovement(pScene,PixelPosBegin,PixelPosEnd,eType,fSpeed,eFindPathType,eBarrierType,fEndDist, uServerBaseTime);
	if(!pMm->m_pPath)
	{
		delete pMm;
		pMm=NULL;
	}
#ifndef LOG_COREOBJ_MOVE
	else
	{
		float fOrgLength=pMm->GetOriginPath()->GetPathPixelLength();
		float fCutLength=pMm->GetPathLength();
		if( fOrgLength < fCutLength )
		{
			ostringstream strm;
			strm<<"OrgLength("<<fOrgLength<<") < CutLength("<<fCutLength<<")";
			GenErr("fOrgLength < fCutLength ", strm.str());
		}
	}
#endif
	return pMm;
}


CFolMovement* CFolMovement::Create(CCoreSceneClient* pScene,const int16* pPathData, uint16 uPathDataLen,const CFPos& PixelPosBegin,const CFPos& PixelPosEnd
			,EFolMovementType eType, float fSpeed, float fEndDist, uint64 uServerBaseTime)
{
	CFolMovement* pMm=new CFolMovement(pScene,pPathData, uPathDataLen,PixelPosBegin,PixelPosEnd,eType,fSpeed,fEndDist, uServerBaseTime);
	if(!pMm->m_pPath)
	{
		delete pMm;
		pMm=NULL;
	}
#ifndef LOG_COREOBJ_MOVE
	else
	{
		float fOrgLength=pMm->GetOriginPath()->GetPathPixelLength();
		float fCutLength=pMm->GetPathLength();
		if( fOrgLength < fCutLength )
		{
			ostringstream strm;
			strm<<"OrgLength("<<fOrgLength<<") < CutLength("<<fCutLength<<")";
			GenErr("fOrgLength < fCutLength ", strm.str());
		}
	}
#endif
	return pMm;

}

CFPos CFolMovement::GetMovementBeginPixelPos()const
{
	CFPos Pos;
	m_pPath->GetBeginPixelPos(Pos);
	return Pos;
}

CFPos CFolMovement::GetMovementEndPixelPos()const
{
	CFPos PixelPos;		
	if(m_fPathLength<0)
	{
		m_pPath->GetEndPixelPos(PixelPos);
		return PixelPos;
	}
	Ver(GetPixelPos(PixelPos,m_fPathLength));
	return PixelPos;
}

CFPos CFolMovement::GetPathBeginPixelPos()const
{
	CFPos PixelPos;
	m_pPath->GetBeginPixelPos(PixelPos);
	return PixelPos;
}

CFPos CFolMovement::GetPathEndPixelPos()const
{
	CFPos PixelPos;
	m_pPath->GetEndPixelPos(PixelPos);
	return PixelPos;
}

bool CFolMovement::GetPixelPos(CFPos& PixelPos,float fDist)const
{
	return uint16(-1) != m_pPath->GetPixelPosOfPath(PixelPos,fDist);
}

float CFolMovement::GetPathLength()const
{
	return m_fPathLength;
}

void CFolMovement::CutPath(float fChangeDist)
{
	if( fChangeDist > m_fPathLength )
	{
		ostringstream strm;
		strm<<"fChangeDist("<<fChangeDist<<") > m_fPathLength("<<m_fPathLength<<")";
		GenErr("fChangeDist > m_fPathLength ", strm.str());
	}

	m_fPathLength = fChangeDist;
	m_bPathIsCut = true;
}

bool CFolMovement::PathIsCut()const
{
	return m_bPathIsCut;
}

CPixelPath* CFolMovement::ClonePixelPath()const
{
	return m_pPath->Clone();
}

const CPixelPath* CFolMovement::GetOriginPath()const
{
	return m_pPath;
}

uint64 CFolMovement::GetEstimatedServerTime()const
{
	return m_EstimatedServerTime->Guess();
}

uint64 CFolMovement::GetElapsedTime()const
{
	return m_EstimatedServerTime->GetElapse();
}

float CFolMovement::GetMovedDistWhenRemoteReached()const
{
	return m_fMovedDistWhenRemoteReached;
}

void CFolMovement::SetMovedDistWhenRemoteReached(float fMovedDistWhenRemoteReached)
{
	m_fMovedDistWhenRemoteReached = fMovedDistWhenRemoteReached;
	SetMovedDistWhenRemoteSuspended(fMovedDistWhenRemoteReached);
}

float CFolMovement::GetMovedDistWhenRemoteSuspended()const
{
	return m_fMovedDistWhenRemoteSuspended;
}

void CFolMovement::SetMovedDistWhenRemoteSuspended(float fMovedDistWhenRemoteSuspended)
{
	m_fMovedDistWhenRemoteSuspended = fMovedDistWhenRemoteSuspended;
}

bool CFolMovement::RemoteReached()const
{
	return m_fMovedDistWhenRemoteReached >= 0;
	//return GetPathLength() == GetMovedDistWhenRemoteSuspended();
}

float CFolMovement::GetRemoteMovedDist()const
{
	if(RemoteReached())
		return m_fMovedDistWhenRemoteReached;

	if(m_fMovedDistWhenRemoteSuspended > 0)
		return m_fMovedDistWhenRemoteSuspended;

	return CalIdealDistByTimeInMovement(int32(GetElapsedTime()) + GetTimeErr() - int32(m_uTotalSuspendedTime));
}

int32 CFolMovement::GetTimeErr()const
{
	return int32(CConnClient::Inst()->GetEstimatedServerTime() - GetEstimatedServerTime());
}

float CFolMovement::CalIdealDistByTimeInMovement(int32 nTime)const
{
	if(!this)
		return 0;
	return nTime*GetSpeed()*eGridSpanForObj/1000;
}

void CFolMovement::AccumulateSuspendedTime(uint32 uSuspendedTime)
{
	m_uTotalSuspendedTime += uSuspendedTime;
}

