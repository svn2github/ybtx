#pragma once
#include "CoreObjectDefs.h"
#include "CPos.h"
#include "CDir.h"
#include "FindPathDefs.h"
#include "PatternCOR.h"
#include "PatternRef.h"
#include "CMagicMallocObject.h"

class CEntityServer;

class IMoveManager
	: public CPtCORHandler
	, public CMagicMallocObject
{
private:
	TPtBiRef< IMoveManager, CEntityServer >	m_Entity;
protected:
	IMoveManager(CEntityServer* pEntity);
	virtual ~IMoveManager(){}
	CEntityServer* GetEntity()const {return m_Entity.Get();}
	EMoveToResult MoveTo(const CFPos& PixelPosDes,float fPixelSpeed,EBarrierType eBarrierType);
	EMoveToResult MoveTo(const CFPos& PixelPosDes,float fPixelSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType);
	EMoveToResult MoveTo(const CDir& Dir,float fPixelDistance,float fPixelSpeed,EBarrierType eBarrierType);
	EMoveToResult RandomMoveTo(float fDistance,float fPixelSpeed,EBarrierType eBarrierType);
	uint32			m_uMoveID;
public:
	virtual void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)=0;
};

class CMoveServerManager:public IMoveManager
{
public:
	EMoveToResult MoveTo(const CFPos& PixelPosDes,float fPixelSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType);
private:
	friend class CEntityServer;
	CMoveServerManager(CEntityServer* pEntity);
	~CMoveServerManager();
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
};

