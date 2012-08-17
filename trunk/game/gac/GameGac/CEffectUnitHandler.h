#pragma once
#include "IEffectUnit.h"

class CEffectUnitHandler : public IEffectUnitHandler
{
public:
	CEffectUnitHandler() { AddRef(); }
	virtual ~CEffectUnitHandler(){}
	bool	IsValid()	{ return true; }
};
