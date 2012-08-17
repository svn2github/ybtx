#pragma once
#include "CDistortedTimeAllocator.h"

namespace sqr
{
	class CDistortedTimeObj;

	class CDistortedTimeScene
		: public CDistortedTimeMallocObject
	{
	public:
		CDistortedTimeScene(void);
		virtual ~CDistortedTimeScene();

		void Release();
		void DestroyDistortedTimeObj(CDistortedTimeObj* pObj);

		void SetTimeGroup(uint32 uTimeGroup);
		uint32 GetTimeGroup()const;
		void SetTimeDistortedRatio(float fTimeDistortedRatio);
		float GetTimeDistortedRatio()const;

		void AddDistortedTimeObj(CDistortedTimeObj* pObj);
		void DelDistortedTimeObj(CDistortedTimeObj* pObj);

		void OnDistortedTimeObjDestroyed();

	private:
		typedef list<CDistortedTimeObj*, CDistortedTimeAllocator<CDistortedTimeObj*> >	DistortedTimeObjContainer;
		DistortedTimeObjContainer			m_DistortedTimeObjContainer;

		float	m_fTimeDistortedRatio;	//时间系数
		uint32	m_uTimeGroup;			//时间组,只有组号不相同的DistortedTimeObj才会受时间系数影响
		bool	m_bInDestroying;
	};
}
