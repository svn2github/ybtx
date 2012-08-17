#pragma once
#include "CDistortedTick.h"
#include "CPos.h"
#include "CEntityMallocObject.h"
#include "CStaticObject.h"

class CEntityClient;
class IAutoTrackerHandler;

class CAutoTracker : public CDistortedTick, public CEntityMallocObject
{
public:
	CAutoTracker();
	virtual ~CAutoTracker();
	static uint32 GetTickTime();
	static float GetDistAdjustment();

	bool Init(uint32 uAttackerID, uint32 uTargetID, float fReachGridDist, IAutoTrackerHandler* pHandler, bool bIsNormalAttackTracker);
	void Cancel();
	bool IsNormalAttackTracker();
	virtual void OnTick();

	void SetAttackerID(uint32 uID){ m_uAttackerID = uID; }
	void SetTargetID(uint32 uID){ m_uTargetID = uID; }

	void SetHandler(IAutoTrackerHandler* pHandler);
	IAutoTrackerHandler* GetHandler()const;

	bool IsAutoTracking()const { return m_bIsAutoTracking; }
private:
	static uint32 GetMaxStepInAStar();
	void Track(CEntityClient* pAttacker, CEntityClient* pTarget, bool bBypassingBarrier);
	bool IsInScope(CEntityClient* pAttacker, CEntityClient* pTarget);
	bool HasBarrierInLine(CEntityClient* pAttacker, CEntityClient* pTarget);

private:
	uint32 m_uAttackerID;
	uint32 m_uTargetID;
	IAutoTrackerHandler* m_pHandler;
	float m_fReachGridDist;
	bool m_bIsAutoTracking;
	bool m_bIsNormalAttackTracker;
	CFPos m_posDest;
};

class IAutoTrackerHandler : public virtual CStaticObject, public CEntityMallocObject
{
public:
	virtual ~IAutoTrackerHandler(){}

	virtual bool CreatedInCPP(){return false;}	// 是否用于C++代码，CAutoTracker的SetHandler里面需要区分
	virtual void OnReached(CEntityClient* pAttacker, CEntityClient* pTarget);
	virtual void OnCancel(CEntityClient* pAttacker, CEntityClient* pTarget);
	virtual bool OnCheckStopCond(CEntityClient* pAttacker, CEntityClient* pTarget);
	
};
