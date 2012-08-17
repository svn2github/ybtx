#pragma once

#include "CPos.h"

#define MAXDIS 2048

namespace sqr
{
typedef TPos<int16> CPathPos;

namespace FindPath
{
enum EOptimizeLevel
{
	eOL_Null		= 0,
	eOL_Normal		= 1,
	eOL_Ultra		= 2
};

template< class T, int BUFSIZE = 100 >
class CSearcher
{
	T*			m_MapMask;

	CPathPos    m_EndPos;
	CPathPos    m_PathBuf[BUFSIZE];
	CPathPos	m_LeftBuf[BUFSIZE/2 + 2];
	CPathPos	m_RightBuf[BUFSIZE/2 + 2];
	int32		m_nNearest;
	int32		m_nNrstJugde;
	int32		m_nBufSize;

	bool IsSeeThePoint( CPathPos& SrcPos, CPathPos& DestPos, CPathPos& Barrier, CPathPos* pPath = NULL, int* pStep = NULL )
	{
		int16 xRange = (int16)abs( DestPos.x - SrcPos.x );
		int16 yRange = (int16)abs( DestPos.y - SrcPos.y );

		int16 xDelta = DestPos.x < SrcPos.x ? -1 : 1;
		int16 yDelta = DestPos.y < SrcPos.y ? -1 : 1;

		CPathPos CurPos = SrcPos;
		if ( pPath )
			pPath[(*pStep)++] = CurPos;

		int nCurDx = 0;
		int nCurDy = 0;
		int nJudge = 0;

		if ( yRange > xRange )
		{
			nCurDy = 1;
			nJudge = -( max<int>( yRange, 2 )>>1 );
		}
		else
		{
			nCurDx = 1;
			nJudge = ( xRange>>1 );
		}

		while ( CurPos != DestPos )
		{
			if ( nJudge < 0 )
			{
				CurPos.y = CurPos.y + yDelta;
				nCurDy++;
				nJudge += xRange;
			}
			else
			{
				CurPos.x = CurPos.x + xDelta;
				nCurDx++;
				nJudge -= yRange;
			}

			if ( m_MapMask->IsBarrier( CurPos.x, CurPos.y ) )
			{
				Barrier = CurPos;
				return false;
			}
			else if ( pPath )
			{
				pPath[(*pStep)++] = CurPos;
			}
		}
		return true;
	}

	bool FindNextPos( CPathPos& CurPos, CPathPos& CrossPos, CPathPos& Barrier, bool bRight )
	{
		const CPathPos Direct[8] =
		{
			CPathPos(  1,  1 ),CPathPos(  1,  0 ),CPathPos(  1, -1 ),CPathPos(  0, -1 ),
			CPathPos( -1, -1 ),CPathPos( -1,  0 ),CPathPos( -1,  1 ),CPathPos(  0,  1 )
		};
		char Inver[3][3] = { { 4, 3, 2 }, { 5, 8, 1 }, { 6, 7, 0 } };

		CPathPos Temp = Barrier - CurPos;

		int nDelta = bRight ? -2 : 2;
		int n = 0;
		for ( int j = Inver[ Temp.y + 1 ][ Temp.x + 1 ] + 8; n < 8 ; j += nDelta, n++ )
		{
			CPathPos MovePos = CurPos + Direct[j&7];
			if ( !m_MapMask->IsBarrier( MovePos.x, MovePos.y ) )
			{
				CurPos = CurPos + Direct[ ( j + ( bRight ? 1 : 7 ) )&7 ];
				if ( m_MapMask->IsBarrier( CurPos.x, CurPos.y ) )
				{
					Barrier = CurPos;
					CrossPos = CurPos = MovePos;
				}
				else
					CrossPos = MovePos;
				return true;
			}
			Barrier = MovePos;
		}

		return false;
	}

	int Judge( CPathPos Pos1, CPathPos Pos2 )
	{
		CPathPos Delta = Pos2 - Pos1;
		int x = abs( Delta.x );
		int y = abs( Delta.y );
		return ( x > y ) ? x + (y>>2): y + (x>>2);
	}

	int SearchFromCur( CPathPos& CurPos, int CurStep )
	{
		CPathPos PreBar, Barrier;
		CPathPos DeltaStep = m_EndPos - CurPos;

		if ( IsSeeThePoint( CurPos, m_EndPos, Barrier, m_PathBuf, &CurStep ) )
		{
			m_PathBuf[CurStep++] = m_EndPos;
			return CurStep;
		}

		CPathPos posLeft  = m_PathBuf[ CurStep - 1 ];
		CPathPos posRight = m_PathBuf[ CurStep - 1 ];
		CPathPos barLeft  = Barrier;
		CPathPos barRight = Barrier;

		int nLeft = 0;
		int nRight = 0;
		int nMinJudge = 0x7fffffff;
		int nPos = 0;
		bool bLeft = true;

		for (;;)
		{
			CPathPos CrossPos;
			CPathPos posBar;
			int nJugeLeft	= Judge( posLeft, m_EndPos );
			int nJugeRight	= Judge( posRight, m_EndPos );
			if ( nJugeLeft < nJugeRight )
			{
				if ( nLeft + CurStep + 1 > m_nBufSize )
					return -1;
				if ( nJugeLeft < nMinJudge )
				{
					nMinJudge	= nJugeLeft;
					nPos		= nLeft;
					bLeft		= true;
				}
				if ( !FindNextPos( posLeft, CrossPos, barLeft, false ) )
					return -1;
				if ( posLeft != CrossPos )
					m_LeftBuf [nLeft ++]	= CrossPos;
				m_LeftBuf [nLeft ++]	= posLeft;

				//·ÀÖ¹ÈÆ±ß
				if ( nLeft > m_nBufSize/2 )
					break;
			}
			else
			{
				if ( nRight + CurStep + 1 > m_nBufSize )
					return -1;
				if ( nJugeRight < nMinJudge )
				{
					nMinJudge	= nJugeRight;
					nPos		= nRight;
					bLeft		= false;
				}
				if ( !FindNextPos( posRight, CrossPos, barRight, true ) )
					return -1;
				if ( posRight != CrossPos )
					m_RightBuf[nRight++]	= CrossPos;
				m_RightBuf[nRight++]	= posRight;

				//·ÀÖ¹ÈÆ±ß
				if ( nRight > m_nBufSize/2 )
					break;
			}

			//if( nLeft + nRight > 1 && abs( posLeft.x - posRight.x ) + abs( posLeft.y - posRight.y ) < 2 )
			//	break;

			if ( posLeft == posRight )
			{
				if ( nRight > 1 )
				{
					CPathPos pos = posLeft;
					CPathPos bar = barLeft;
					FindNextPos( pos, CrossPos, bar, false );
					if ( m_RightBuf[ nRight - 2 ] == CrossPos )
						break;
				}
				else if ( nLeft > 1 )
				{
					CPathPos pos = posRight;
					CPathPos bar = barRight;
					FindNextPos( pos, CrossPos, bar, true );
					if ( m_LeftBuf[ nLeft - 2 ] == CrossPos )
						break;
				}
			}
		}

		if ( nMinJudge < m_nNrstJugde )
		{
			m_nNearest = CurStep + nPos;
			m_nNrstJugde = nMinJudge;
			//if( m_nNearest >= BufSize )
			//	Sleep(0);
		}

		if ( bLeft )
		{
			memcpy( m_PathBuf + CurStep, m_LeftBuf, ( nPos + 1 )*sizeof( CPathPos ) );
			return SearchFromCur( m_LeftBuf[nPos], CurStep + nPos + 1 );
		}
		else
		{
			memcpy( m_PathBuf + CurStep, m_RightBuf, ( nPos + 1 )*sizeof( CPathPos ) );
			return SearchFromCur( m_RightBuf[nPos], CurStep + nPos + 1 );
		}
	}

	int Optimize( int Step )
	{
		int i,j,k,n;
		CPathPos posBar;
		CPathPos PathBuf[BUFSIZE];

		j = 1;
		PathBuf[0] = m_PathBuf[Step - 1];
		for ( i = Step - 1; i > 0; i = n, j++ )
		{
			for ( n = 0; n < i; n++ )
			{
				if ( IsSeeThePoint( m_PathBuf[n], m_PathBuf[i], posBar ) )
					break;
			}

			PathBuf[j] = m_PathBuf[n];
		}

		for ( k = 0; k < j; k++ )
			m_PathBuf[k] = PathBuf[ j - 1 - k ];

		return j;
	}

	int UltraOptimize( int Step )
	{
		int i,j,k,n;
		CPathPos posBar;
		CPathPos PathBuf[BUFSIZE];
		CPathPos TempBuf[BUFSIZE];

		j = 0;
		for ( i = Step - 1; i > 0; i = n )
		{
			int nTempStep=0;
			for ( n = 0; n < i; n++ )
			{
				nTempStep=0;
				if ( IsSeeThePoint( m_PathBuf[n], m_PathBuf[i], posBar, TempBuf, &nTempStep ) )
					break;
			}

			for ( k = nTempStep - 1; k > 0; k--, j++ )
				PathBuf[j] = TempBuf[k];
		}

		for ( k = 0; k < j; k++ )
			m_PathBuf[k] = PathBuf[ j - 1 - k ];
		Step = j;

		for ( i = 0, j = 1; i< Step - 1; i += n, j++  )
		{
			for ( n = Step - i - 1; n > 1; n-- )
			{
				if ( IsSeeThePoint( m_PathBuf[i], m_PathBuf[ i + n ], posBar ) )
					break;
			}
			m_PathBuf[j] = m_PathBuf[ i + n ];
		}

		return j;
	}


public:
	CSearcher() : m_nBufSize(BUFSIZE) {};

	void SetMaxBufSize( int32 nBufSize )
	{
		m_nBufSize = limit2( nBufSize, 0, BUFSIZE );
	}

	const CPathPos* SearchRoad( T* pMapMask, CPathPos StartPos, CPathPos EndPos,
	                            int& StepNum, EOptimizeLevel eOptimize, int* pNearestToEnd )
	{
		m_MapMask	  = pMapMask;
		m_EndPos      = EndPos;
		m_nNearest	  = 0;
		m_nNrstJugde  = 0x7fffffff;

		m_PathBuf[0]  = StartPos;
		StepNum = SearchFromCur( StartPos, 1 );

		if ( pNearestToEnd )
			*pNearestToEnd = m_nNearest;

		if ( eOptimize == eOL_Normal )
		{
			if ( StepNum > 0 )
				StepNum = Optimize( StepNum );
			else if ( pNearestToEnd )
				*pNearestToEnd = Optimize( *pNearestToEnd );
		}
		if ( eOptimize == eOL_Ultra )
		{
			if ( StepNum > 0 )
				StepNum = UltraOptimize( StepNum );
			else if ( pNearestToEnd )
				*pNearestToEnd = UltraOptimize( *pNearestToEnd );
		}

		return m_PathBuf;
	}
};
}
}

