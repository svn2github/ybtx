#pragma once

#include "ExpHelper.h"

namespace sqr
{
	class IDatabase;
}



namespace sqr
{
	class  IDbThreadMgr;
	class IDbConnectionMgr;
	class CDbThreadHandler;

	class CTestSuiteDbThreadsMgr : public TestFixture
	{
	public:

		void setUp();
		void tearDown();
		CPPUNIT_TEST_SUITE(CTestSuiteDbThreadsMgr);
		CPPUNIT_TEST(TestInsertAndSelectCount);
		CPPUNIT_TEST_SUITE_END();
		
		void TestInsertAndSelectCount();
		void TestUpdate();



	private:
		IDatabase*	m_pDataBase;
		IDbThreadMgr* m_pMgr;
		IDbConnectionMgr* m_pConnMgr;
		CDbThreadHandler*	m_semWait;
	};

}


