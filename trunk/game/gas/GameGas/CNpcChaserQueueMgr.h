#pragma once

#include "CPos.h"
#include "CEntityMallocObject.h"
#include "TEntityAllocator.h"

class CCharacterDictator;

class CNpcChaserQueueMgr
	:public CEntityMallocObject
{
	typedef map<uint32, uint32, less<uint32>, TEntityAllocator<pair<uint32, uint32> > > MapNpcChaser2Angle;
public:
	CNpcChaserQueueMgr(CCharacterDictator* pOwner);
	~CNpcChaserQueueMgr();

	void GetRegulateNpcPlace(uint32 uEntityID, CFPos& Pos);
	MapNpcChaser2Angle& GetChaserMap(float fAttackDis);
	void DelChaser(uint32 uEntityID);
private:
	CCharacterDictator*		m_pOwner;
	MapNpcChaser2Angle		m_mapNearChase;
	MapNpcChaser2Angle		m_mapMiddleChase;
	MapNpcChaser2Angle		m_mapFarChase;
};
