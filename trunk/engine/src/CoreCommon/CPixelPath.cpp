#include "stdafx.h"
#include "CPixelPath.h"
#include "CPos.h"
#include "IPathFinding.h"
#include "CoreCommon.h"
#include "ExpHelper.h"
#include "CoreObjectCommon.h"
#include "TSqrAllocator.inl"


CPixelPath::CPixelPath(IPath* pPath,
					   const CFPos& BeginPixelPos,
					   const CFPos& EndPixelPos)
					   : m_BeginPixelPos( BeginPixelPos )
					   , m_EndPixelPos( EndPixelPos )
					   , m_pGridPath(pPath)
{
	m_fPathLength = CalPathPixelLength();

	CFPos PixelPos;
	if ( uint16(-1) == GetPixelPosOfPath( PixelPos,m_fPathLength) )
		GenErr("CPixelPath length error.");

	CFPos begin; // 起点像素坐标
	CFPos end;   // 终点像素坐标

	GetBeginPixelPos(begin);
	GetEndPixelPos(end);

	if( begin == end )
		GenErr("PixelPath's length is zero");
}

CPixelPath::~CPixelPath(void)
{
	m_pGridPath->Release();
}

void CPixelPath::Release()
{
	delete this;
}

void CPixelPath::GetBeginPixelPos(CFPos& Pos)const
{
	Pos = m_BeginPixelPos;
}

void CPixelPath::GetEndPixelPos(CFPos& Pos)const
{
	Pos = m_EndPixelPos;
}

uint16 CPixelPath::GetPixelPosOfPath(CFPos& PixelPos,float fDist)const
{
	uint16 uCurWayPoint=0;
	if( !GetPixelPosOfPath( PixelPos, uCurWayPoint, 0, fDist ) )
		return uint16(-1);
	return uCurWayPoint;
}

bool CPixelPath::GetPixelPosOfPath( CFPos& PixelPos, uint16& uCurWayPoint, float fCurDist,float fDist )const
{
	CFPos BaseWayPoint,NextWayPoint;
	if( !GetWayPointPixelPosOfPath( BaseWayPoint, uCurWayPoint ) )
		return false;

	if( !GetWayPointPixelPosOfPath( NextWayPoint, uCurWayPoint+1 ) )
		return false;

	//获得距离向量
	CFPos DistVec=NextWayPoint-BaseWayPoint;

	//获得当前点到当前waypoint的距离
	const float xMul = DistVec.x*DistVec.x;
	const float yMul = DistVec.y*DistVec.y;
	const float xMul_sum_yMul = xMul + yMul;
	if(xMul_sum_yMul < 0)
	{
		ostringstream strm;
		strm << DistVec.x << "," << DistVec.y << " " << xMul << "," << yMul << " " << xMul_sum_yMul << " " << m_BeginPixelPos.x << "," << m_BeginPixelPos.y << " " << m_EndPixelPos.x << "," << m_EndPixelPos.y << " " << uCurWayPoint;
		GenErr("sqrt negative value", strm.str());
	}
	const float fPartDist=sqrt(xMul_sum_yMul);
	//float fPartDist=sqrt( DistVec.x*DistVec.x+DistVec.y*DistVec.y );
	const float fNextDist=fCurDist+fPartDist;

	if( fDist > fNextDist )
		return GetPixelPosOfPath( PixelPos, ++uCurWayPoint, fNextDist, fDist );

	const float fDiff = fDist-fCurDist;
	const float fRate= fDiff/fPartDist;
	DistVec.x = DistVec.x * fRate;
	DistVec.y = DistVec.y * fRate;
	PixelPos = BaseWayPoint + DistVec;
	return true;
}

uint16 CPixelPath::GetLastWayPointInPixelPos(float fMovedDist, CFPos& posLastWayPoint)const
{
	CFPos curPos;
	CPos pos;
	uint16 uCurWayPoint = 0;
	uCurWayPoint = GetPixelPosOfPath(curPos, fMovedDist);
	if(uCurWayPoint != uint16(-1))
	{
		m_pGridPath->GetPos(pos, uCurWayPoint);
		posLastWayPoint.x = GetPixelOfQuarGrid(pos.x);
		posLastWayPoint.y = GetPixelOfQuarGrid(pos.y);
	}

	return uCurWayPoint;
}

uint16 CPixelPath::GetNextWayPointInPixelPos(float fMovedDist, CFPos& posNextWayPoint)const
{
	CFPos curPos;
	CPos pos;

	uint16 uCurWayPoint = GetPixelPosOfPath(curPos, fMovedDist);

	if(uCurWayPoint == uint16(-1))
	{
		return uCurWayPoint;
	}

	if(uCurWayPoint == m_pGridPath->GetPathSize() - 1)
	{
		m_pGridPath->GetPos(pos, uCurWayPoint);
		posNextWayPoint.x = GetPixelOfQuarGrid(pos.x);
		posNextWayPoint.y = GetPixelOfQuarGrid(pos.y);
		return uCurWayPoint;
	}
	else
	{
		Ver(m_pGridPath->GetPos(pos, uCurWayPoint + 1));
		posNextWayPoint.x = GetPixelOfQuarGrid(pos.x);
		posNextWayPoint.y = GetPixelOfQuarGrid(pos.y);
		return uCurWayPoint + 1;
	}
}

bool CPixelPath::GetWayPointPixelPosOfPath(CFPos& PixelPos,uint16 uIndex)const
{
	if( uIndex == 0 )
	{
		GetBeginPixelPos(PixelPos);
		return true;
	}

	if( uIndex == m_pGridPath->GetPathSize()-1 )
	{
		GetEndPixelPos(PixelPos);
		return true;
	}

	CPos GridPos;
	if( !m_pGridPath->GetPos( GridPos,uIndex ) )
	{
		GetEndPixelPos(PixelPos);
		return false;
	}

	PixelPos.x = GetPixelOfQuarGrid( GridPos.x );
	PixelPos.y = GetPixelOfQuarGrid( GridPos.y );
	return true;
}

float CPixelPath::CalPathPixelLength()const
{
	float fLength=0;

	CFPos BeginPixelPos;
	CFPos BeginPos,EndPos;

	GetBeginPixelPos( BeginPixelPos );

	const uint16 uSegmentCount = static_cast<uint16>(m_pGridPath->GetPathSize() - 1);

	BeginPos = BeginPixelPos;

	for( uint16 i=0 ; ; )
	{
		GetWayPointPixelPosOfPath(EndPos,i+1);
		float xDiff = EndPos.x - BeginPos.x;
		float yDiff = EndPos.y - BeginPos.y;
		float xMul = xDiff*xDiff;
		float yMul = yDiff*yDiff;
		float xMul_sum_yMul = xMul + yMul;
		fLength += sqrt(xMul_sum_yMul);

		if( ++i >= uSegmentCount )
			break;;

		GetWayPointPixelPosOfPath(BeginPos,i);
	}

	return fLength;
}


float CPixelPath::GetPathPixelLength()const
{
	return m_fPathLength;
}

size_t CPixelPath::GetPathSize()const
{
	return m_pGridPath->GetPathSize();
}

const IPath* CPixelPath::GetGridPath()const
{
	return m_pGridPath;
}

CPixelPath* CPixelPath::Clone()const
{
	return new CPixelPath(m_pGridPath->Clone(),m_BeginPixelPos,m_EndPixelPos);
}

void CPixelPath::PrintPath()const
{
	const PathList& deqPath = m_pGridPath->GetPath();
	CFPos BeginPixelPos, EndPixelPos;
	GetBeginPixelPos(BeginPixelPos);
	GetEndPixelPos(EndPixelPos);
	cout << BeginPixelPos.x << "," << BeginPixelPos.y << "->";
	PathList::const_iterator it = deqPath.begin();
	for(; it != deqPath.end(); ++it)
	{
		cout << it->x << "," << it->y << "->";
	}
	cout << EndPixelPos.x << "," << EndPixelPos.y << endl;
	cout << "+++++++++++++++++++ " << endl;
}
