#pragma once 

#include "ThreadTypes.h"
#include "CPos.h"
#include "AoiDef.h"
#include "TTrThreadMsgBuffer.h"
#include "TQuadAllocator.h"

namespace sqr
{
	class CAoiJob;
	class CAoiResult;
	class IAoiViewObjHandler;

	class CAoiThread;
	class CAoiThreadMgr;

	class CError;

	class CAoiThreadTraits
	{
	public:
		typedef CAoiJob		LeftMsgType;
		typedef CAoiResult	RightMsgType;
	};

	class CAoiThreadMgr
		: public TTrThreadMsgBuffer<CAoiThreadTraits>
	{
		typedef TTrThreadMsgBuffer<CAoiThreadTraits> Parent_t;
		friend class TTrThreadMsgBuffer<CAoiThreadTraits>;

		friend class CAoiJob;
		friend class CAoiResult;
		friend class CAoiThread;
		friend class CAsyAoiScene;
		friend class CTestSuiteAoi;
		friend class CAoiEndThreadsJob;

	public:
		static void Init(uint32 num);
		static void Unit();
		static CAoiThreadMgr*& Inst();
	
	private:
		void OnWakeRight();
		void OnLeftException(CError& error);
		void OnRightException(CError& error);	

	private:

		void	CreateAoiThreads(uint32 num);
		void	MgrInit(void);
		void	ReleaseThreads(void);

		CAoiThreadMgr(uint32 num);
		~CAoiThreadMgr();
	
		
	private:
		bool					m_bEndThreads;

		HSEMAPHORE	m_smJobQueue;

		std::vector<CAoiThread*, TQuadAllocator<CAoiThread*> >			m_vecThreads;	
	};

}
