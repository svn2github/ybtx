#pragma once
#include "CPos.h"
#include "CFindPathMallocObject.h"

namespace sqr
{

	class CPathDirectNode : public CFindPathMallocObject
	{
	public:
		CPathDirectNode();
		CPathDirectNode(int x, int y, float flength);
		
		inline float GetRemainderLength()
		{
			return m_fRemainderLength;
		}

		inline const CPos& GetPos()
		{
			return m_pos;
		}

		inline void SetPos(const CPos& pos)
		{
			m_pos = pos;
		}
	private:
		CPos	m_pos;
		float	m_fRemainderLength;
	};
}

