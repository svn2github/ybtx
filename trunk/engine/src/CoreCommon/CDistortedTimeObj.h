#pragma once
#include "IDistortedTime.h"
#include "CDistortedTimeAllocator.h"
#include "TIDPtrMap.h"

namespace sqr
{
	class CDistortedTick;
	class CDistortedTimeScene;

	class CDistortedTimeObj 
		: public IDistortedTime
		, public CDistortedTimeMallocObject
	{
		friend class CDistortedTimeScene;
	public:
		virtual uint64 GetDistortedProcessTime()const;

		void SetTimeGroup(uint32 uTimeGroup);
		uint32 GetTimeGroup()const;

		float GetTimeRatio()const;

		//将DistortedTick的时间间隔调整为正常间隔
		virtual void RefreshTickInterval(CDistortedTick* pTick)=0;
		template<typename TTimeSystem>
		void RefreshTickInterval(CDistortedTick* pTick);

		//void SetPosition(CPos& pos);	//这个函数在全场景唯一时间系数的情况下是没用的,暂时为以后功能扩展做准备

		void TransferTo(CDistortedTimeScene* pNewScene);

	protected:
		CDistortedTimeObj(CDistortedTimeScene* pScene);
		~CDistortedTimeObj();

		template<typename TTimeSystem>
		void RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc);
		template<typename TTimeSystem>
		void UnregistDistortedTick(CDistortedTick* pTick);
		template<typename TTimeSystem>
		bool IntRegistDistortedTick(CDistortedTick* pTick, uint32 uCyc);
		template<typename TTimeSystem>
		bool IntUnregistDistortedTick(CDistortedTick* pTick);

		virtual void OnTimeRatioChanged(float fOldTimeRatio);
		template<typename TTimeSystem>
		void OnTimeRatioChanged(float fOldTimeRatio);

		bool TimeNeedToBeDistorted()const;
		uint64 CalDistortedTime(uint64 uCurTime, uint64 uLastRealTimeWhenTimeRatioChanged, uint64 uLastDistortedTimeWhenTimeRatioChanged, float fTimeRatio)const;

		uint64 GetDistortedProcessTime(float fTimeRatio)const;
		template<typename TTimeSystem>
		uint64 GetDistortedFrameTime(float fTimeRatio)const;

		typedef TIDPtrMap<CDistortedTick>	TickContainer;
		TickContainer						m_TickContainer;


		//分别为ProcessTime和FrameTime记录了真实时间和扭曲时间
		uint64 m_uLastDistortedProcessTimeWhenTimeRatioChanged;	//记录上次时间系数改变时的扭曲时间
		uint64 m_uLastRealProcessTimeWhenTimeRatioChanged;			//记录上次时间系数改变时的真实时间
		uint64 m_uLastDistortedFrameTimeWhenTimeRatioChanged;	
		uint64 m_uLastRealFrameTimeWhenTimeRatioChanged;			

	private:
		uint32 m_uTimeGroup;

		CDistortedTimeScene* m_pDistortedTimeScene;
		typedef list<CDistortedTimeObj*, CDistortedTimeAllocator<CDistortedTimeObj*> >	ListDistortedTimeObj;
		ListDistortedTimeObj::iterator		m_itDistortedTimeObjList;


	};
}
