#pragma once

#include "CDistortedTick.h"
#include "CNpcAIMallocObject.h"

class CEnmityMgr;
class CNpcEnmityMember;

class CEnmityMemberCheckTick
	: public CDistortedTick
	, public CNpcAIMallocObject
{
public:
	CEnmityMemberCheckTick(CEnmityMgr* pEnmityMgr, CNpcEnmityMember* pMember);
	~CEnmityMemberCheckTick();
	void OnTick();
private:
	CEnmityMgr*		m_pEnmityMgr;
	CNpcEnmityMember* m_pMember;
};
