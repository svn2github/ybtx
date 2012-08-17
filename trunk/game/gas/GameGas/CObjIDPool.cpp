#include "stdafx.h"
#include "CObjIDPool.h"

CGameEntityIDPool::CGameEntityIDPool()
:m_uCurID(0)
{
}

CGameEntityIDPool::~CGameEntityIDPool()
{
}

uint32 CGameEntityIDPool::GenerateID()
{
	return ++m_uCurID;
}
