#pragma once
#include "CPos.h"
#include "CQuadMallocObject.h"

namespace sqr
{
	struct CAoiShape
		: public CQuadMallocObject
	{
		bool*	m_aryShape;

		uint32	m_uWidth;
		uint32	m_uHeight;

		CPos	m_posCenter;

		inline bool In(const CPos& GridPos)const
		{
			CPos OffsetGridPos=GridPos+m_posCenter;

			if( OffsetGridPos.x>=int32(m_uWidth) || OffsetGridPos.x<0 ||
				OffsetGridPos.y>=int32(m_uHeight) || OffsetGridPos.y<0 )
				return false;

			return m_aryShape[OffsetGridPos.y*m_uHeight+OffsetGridPos.x];
		}
	};
}
