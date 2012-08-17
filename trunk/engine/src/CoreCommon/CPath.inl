
#include "CPath.h"



inline uint32 CPath::GetPathSize()const
{
	return (uint32)m_Path.size();
}

inline void CPath::AddPath(const CPos& Pos)
{
	m_Path.push_front(Pos);
}

inline void CPath::PushBackPath(const CPos& Pos)
{
	m_Path.push_back(Pos);
}

