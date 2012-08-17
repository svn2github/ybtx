#pragma once
#include "TSingleton.h"
#include <queue>
#include "CCoder.h"
#include "CTimeCheckPoint.h"
#include "CRefObject.h"
#include "IProcessCallBack.h"
#include "CGraphicMallocObject.h"
namespace sqr
{
	class SyncCoder;
	class IProcessCallBack;

	enum SyncPRI
	{
		SPRI_High	= 0,
		SPRI_Middle	= 1,
		SPRI_Low	= 2,
		SPRI_COUNT,
		SPRI_NONE,
	};

	class CSyncSystem : public CGraphicMallocObject
	{
	public:
		typedef GList<SyncCoder*> SyncList;
		typedef SyncList::iterator	SyncIterator;

		static void Create(void);
		static void Destroy(void);
		//static void Reset(void);
		static void PushCoder(SyncCoder* pCoder);
		static void SafeDelCoder(SyncCoder* pCoder);
		
		static bool	BeginSyncFun(uint64 time);
		static bool	StopSynFun(void);
		static bool	TryToStop(void);
		static void	BeginSyncFun(IProcessCallBack* pSpro = NULL);
		static void DelCoder(SyncCoder* pCoder);
		static void RefreshCoderPRI(SyncCoder* pCoder);
	protected:
		//以后我们的fiber切换要调用这个函数，别直接调用系统的SwtichToFiber
		static void ResumeFiber(void* pFiber);
		

	protected:
		static void	SyncFun(void);
		static void CoderStop(void);
		static SyncList			st_listSyncCoders[SPRI_COUNT];
		static uint64			st_SyncTime;
		static PVOID			st_pSyncFiber;
		static PVOID			st_pCallThread;
		static SyncCoder*		st_pActiveCoder;
		//static DWORD			st_ShowTime;
		static CTimeCheckPoint	st_CheckPoint;
		static bool				st_isStopImme;
		//static bool			st_isBusy;
		static bool				st_isIdle;

	private:
		static GVector<void*> st_mapContext2Stack;
	};

#define SyncCoder_BreakPoint() \
	if(BreakPoint()) return true;

	struct SyncListIterator
	{
		SyncListIterator(SyncPRI pri,CSyncSystem::SyncIterator& Iter)
			: m_eSpri(pri), m_SyncIt(Iter) {};
		SyncListIterator() : m_eSpri(SPRI_COUNT) {}
		SyncPRI	m_eSpri;
		CSyncSystem::SyncIterator m_SyncIt;
	};

	class SyncCoder 
		: public CCoder		
	{
		friend class CSyncSystem;
	public:
		virtual bool AutoDeCode(void) = 0;
		SyncCoder():m_eSpri(SPRI_Low){}
		virtual ~SyncCoder() {};
		SyncPRI	GetPRI(void); 
		void	SetPRI(SyncPRI pri);
	protected:
		bool BreakPoint(void);
		SyncPRI	m_eSpri;
	protected:
		inline	bool IsFree(void);
		void PopIterator( SyncListIterator& Iter );
		void PushIterator( SyncListIterator& Iter );
		queue< SyncListIterator > m_SyncIters;
	};

	inline SyncPRI SyncCoder::GetPRI(void)
	{
		return m_eSpri;
	}

	inline bool SyncCoder::IsFree()
	{
		return m_SyncIters.empty();
	}

	inline bool 
		SyncCoder::BreakPoint(void)
	{
		return CSyncSystem::TryToStop();
	}
}
