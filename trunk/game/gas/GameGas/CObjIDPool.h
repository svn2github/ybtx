#pragma once
#include "CEntityMallocObject.h"

class IObjIDPool : public CEntityMallocObject
{
public:
	virtual ~IObjIDPool() {}
	virtual uint32 GenerateID() = 0;
};

class CGameEntityIDPool : public IObjIDPool
{
public:
	CGameEntityIDPool();
	virtual ~CGameEntityIDPool();

	virtual uint32 GenerateID();

private:
	uint32 m_uCurID;
};
