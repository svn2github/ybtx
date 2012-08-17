#pragma once
//$id$
//对象的移动状态类，负责保存移动状态的数据，以及执行移动操作

#include "CDistortedTick.h"
#include "CRenderSlot.h"
#include "TimeHelper.h"
#include "CCoreObjMallocObject.h"

#ifdef GetObject
#undef GetObject
#endif

namespace sqr
{	
	template<class T> struct TPos;
	typedef TPos<int32> CPos;
	typedef TPos<float> CFPos;

	class CPixelPath;

	template<typename Traits>
	class TCoreObject;

	template<typename Traits>
	class TObjMoveState
		:public Traits::Tick_t
		,public CCoreObjMallocObject
	{
		typedef typename Traits::CoreObject_t		ImpCoreObject_t;
		typedef typename Traits::ObjMoveState_t		ImpObjMoveState_t;
		typedef typename Traits::TimeSystem_t		TimeSystem_t;

		friend class TCoreObject<Traits>;
	public:
		ImpCoreObject_t* GetObject()const;

		const CPixelPath* GetPath()const;
		
		void GetCurWayPoint(CFPos& PixelPos)const;
		float GetPathLength()const;

		//return false表示MovedDist已经大于EndDist
		bool SetEndDist(float fEndDist);
		bool SetReachDist(float fReachDist);

		float GetMovedDist()const;
		float GetRealMovedDist()const;
		//真实系统时间进行坐标运算,得出对象目前精准的坐标
		void GetRealCurPos(CFPos& PixelPos);

		uint32 GetMoveCyc()const;
		void SetMoveCyc(uint32 uMoveCyc);

		bool SetSpeed(float fSpeed);
		float GetSpeed()const;
	protected:
		TObjMoveState(ImpCoreObject_t* pObj,CPixelPath* pPath,float fSpeed,float fBeginDist);
		virtual ~TObjMoveState();

		bool NeedSkipFrameDerived()const;
		void CreateTickingDerived(uint32 uCyc);
		void DeleteTickingDerived();
		void OnTick();

		void IntSetSpeedDerived(float fSpeed);

		float IntGetNewMoveDistDerived()const;

		void IntSetNewPath(CPixelPath* pPath,float fBeginDist);
		void SetNewPathDerived(CPixelPath* pPath,float fBeginDist);

	private:		
		bool CheckLastStep();

		void ResetUpdatePosTcp();

		bool Reached()const;

		float GetNewMoveDist()const;

		//此函数使用的是真实系统时间进行坐标运算，与Tick的逻辑时间是完全不同的，
		//他应该只被用于结束某次移动的最终坐标计算，不应该在连续移动的过程中使用。
		void UpdateCurPosByRealTime();
		//服务端逻辑线程发过来的MoveTo进行ChangeMovePath之前
		//需要根据逻辑线程对象的MovedDist来更新引擎线程对象的坐标
		void UpdateCurPosByMovedDist(float fMovedDist);

		float IntGetRealMovedDist()const;

		ImpCoreObject_t*	m_pObject;
		CPixelPath*			m_pPath;
		float				m_fSpeed;
		float			m_fMovedDist;
		float			m_fEndDist;
		//uint32			m_uMoveCyc;
		float			m_fLastUpdateDist;
		uint16			m_uCurWayPoint;

		//CTimeCheckPoint		m_tcpThisPath;		//移动的起始时间
		uint64			m_uThisPathExpiredTime;		//移动的起始时间

	};
}


