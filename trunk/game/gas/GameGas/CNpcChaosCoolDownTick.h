#pragma once

class CNpcEnmityMember;

#include "CDistortedTick.h"
#include "CNpcAIMallocObject.h"

class CNpcChaosCoolDownTick
	: public CDistortedTick
	, public CNpcAIMallocObject
{
public:
	CNpcChaosCoolDownTick(CNpcEnmityMember* pMember);
	~CNpcChaosCoolDownTick();

	void OnTick();
private:
	CNpcEnmityMember* m_pMember;
};
