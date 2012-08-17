#pragma once
#include "CActor.h"

class IActorClientHandler;

class CActorClient 
	: public CActor
{
public:
	CActorClient() {}
	virtual ~CActorClient() {}

	inline void SetHandler(IActorClientHandler* pHandler) { m_pHandler = pHandler; }
	inline IActorClientHandler* GetHandler() const { return m_pHandler; }

private:
	IActorClientHandler* m_pHandler;
};
