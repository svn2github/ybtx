#pragma once
#include "FindPathDefs.h"
#include "CoreObjectDefs.h"
#include "CDistortedTick.h"
#include "PatternRef.inl"
#include "CPos.h"
#include "TCoreObjAllocator.h"
#include "SyncPatternCOR.h"

namespace sqr
{
	class CSyncCoreObjectServer;
	class CObjPosObservee;

	//Tracer不再继承CObjPosObserver,改用tick来实现. CObjPosObserver不能改成用tick来实现,因为对象都静止时tick就白费了
	//Tracer的Tick会自动根据当前距离的远近调整Tick间隔,距离越近间隔越小.间隔最小为100ms,最大为500ms
	class CCoreObjTracer
		:public CDistortedTick
		,public CCoreObjMallocObject
		,public CPtCORHandler
	{
		friend class CSyncCoreObjectServer;
	public:
		CCoreObjTracer(CSyncCoreObjectServer* pCoreObj);

		ETraceResult Trace(CSyncCoreObjectServer* pTarget, float fSpeed, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist);
		void Stop();
		
	private:
		~CCoreObjTracer();

		virtual void OnTick();
		virtual void OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg);

		EMoveToResult Move(float fSpeed = 0);
		CSyncCoreObjectServer* GetTarget()const;
		void AdjustTickFreq();
		float GetSelfSpeed()const;
		float GetTargetSpeed()const;

		CSyncCoreObjectServer*	m_pCoreObj;
		float					m_fSpeed;
		EFindPathType			m_eFindPathType;
		EBarrierType			m_eBarrierType;
		float					m_fReachDist;
		TPtRefer<CCoreObjTracer, CSyncCoreObjectServer> m_Target;
		uint32					m_uCurTickFreq;
		CFPos					m_fTargetCurRealPos;
		uint32					m_uNextTickFreq;		//用来保存下一个tick周期
	};
}

