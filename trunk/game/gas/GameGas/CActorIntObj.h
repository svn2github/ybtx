#pragma once
#include "CActorServer.h"

class IActorIntObjHandler;

class CActorIntObj : public CActorServer
{
public:
	CActorIntObj();
	virtual ~CActorIntObj(); 
	IActorIntObjHandler* GetHandler() const;
	void InitActorVariant();
	CSyncVariantServer* GetVariantActionDir();

private:
	CSyncVariantServer* m_pVariantActionDir;
};
