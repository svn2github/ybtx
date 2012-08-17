#include "stdafx.h"
#include "CommonTest.h"
#include "CTestSuiteRenderSlot.h"

#include "CRenderSlot.h"

namespace sqr
{
	const char* RegTestSuiteRenderSlot()
	{
		static TestSuiteFactory<CTestSuiteRenderSlot> m_factory;
		const char* szSuiteName="CTestSuiteRenderSlot";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}

	class CTestSlot
		:public CRenderSlot
	{
	public:
		CTestSlot():m_bStopped(false),m_uCount(0){}
		virtual ~CTestSlot(){Unregist();}

		virtual void OnUpdate(uint64 uLastFrameUsedTime)
		{
			if(!m_bStopped)
			{
				//++m_uCount;
				//uint32 uRand = uint32(rand()) % 100;
				//if(m_uCount == 13)
				//{
					//delete this;
					Unregist();
				//}

			}
		}
		virtual void Regist()
		{
			CRenderSlotOwner::Inst().Insert(this);
			m_bStopped = false;
		}

		virtual void Unregist()
		{
			if(CRenderSlotOwner::Inst().Delete(this))
				m_bStopped = true;
		}

		bool	m_bStopped;
		uint32	m_uCount;
	};

	class CTestStandardSlot
		:public CTestSlot
	{
	public:
		CTestStandardSlot(){}
	};

	class CTestLimitedSlot
		:public CTestSlot
	{
	public:
		CTestLimitedSlot(){}
	};

	class CTestSlotTwins
		:public CTestSlot
	{
	public:
		CTestSlotTwins(CTestSlotTwins* pOther):m_pOther(pOther) {}
		
		virtual void OnUpdate(uint64 uLastFrameUsedTime)
		{
			if(m_pOther)
				m_pOther->Unregist();
			CTestSlot::OnUpdate(uLastFrameUsedTime);
			ms_uUpdateCount++;
		}

		CTestSlotTwins* m_pOther;
		static uint32 ms_uUpdateCount;
	};

	uint32 CTestSlotTwins::ms_uUpdateCount = 0;
}


void CTestSuiteRenderSlot::setUp()
{
}

void CTestSuiteRenderSlot::tearDown()
{
}

void CTestSuiteRenderSlot::TestUpdateWithDeleteOther()
{
	vector<CTestSlotTwins*> vec;
	int32 i = 0;
	for(; i < 1000; ++i)
	{
		CTestSlotTwins* pSlot = new CTestSlotTwins(NULL);
		pSlot->Regist();
		vec.push_back(pSlot);
	}
	for(i = 0; i < 999; ++i)
	{
		vec[i]->m_pOther = vec[i+1];
	}

	CTestSlotTwins::ms_uUpdateCount = 0;
	CRenderSlotOwner::Inst().Update(0);
	//CPPUNIT_ASSERT_EQUAL(uint32(2),CTestSlotTwins::ms_uUpdateCount);

	//CRenderSlotOwner::Inst().PrintSlot();
	CRenderSlotOwner::Inst().Defrag();
	//CRenderSlotOwner::Inst().PrintSlot();
	CPPUNIT_ASSERT_EQUAL(uint32(0),uint32(CRenderSlotOwner::Inst().Size()));

	for(i = 0; i < 1000; ++i)
	{
		vec[i]->Regist();
	}
	for(i = 500; i < 1000; ++i)
	{
		vec[i]->m_pOther = vec[i-500];
	}

	//CTestSlotTwins::ms_uUpdateCount = 0;
	CRenderSlotOwner::Inst().UpdateFromSlot(0, 500);
	//CPPUNIT_ASSERT_EQUAL(uint32(1000),CTestSlotTwins::ms_uUpdateCount);

	CRenderSlotOwner::Inst().Defrag();
	CPPUNIT_ASSERT_EQUAL(uint32(0),uint32(CRenderSlotOwner::Inst().Size()));
}

void CTestSuiteRenderSlot::TestInsertAndDelete()
{
	vector<CTestSlot*> vec;
	uint32 u = 0;
	for(; u < 1000; ++u)
	{
		CTestSlot* pSlot = new CTestStandardSlot;
		pSlot->Regist();
		vec.push_back(pSlot);
		pSlot = new CTestLimitedSlot;
		pSlot->Regist();
		vec.push_back(pSlot);
	}
	
	for(uint32 u = 0; u < 1; ++u)
	{
		CTestSlot* pSlot;
		pSlot = new CTestStandardSlot;
		pSlot->Regist();
		vec.push_back(pSlot);
		pSlot = new CTestLimitedSlot;
		pSlot->Regist();
		vec.push_back(pSlot);
		CRenderSlotOwner::Inst().Update(0);
	}
	vector<CTestSlot*>::iterator it = vec.begin();
	for(; it != vec.end(); ++it)
	{
		delete *it;
	}
	CRenderSlotOwner::Inst().Defrag();
	CPPUNIT_ASSERT_EQUAL(uint32(0),uint32(CRenderSlotOwner::Inst().Size()));
}
