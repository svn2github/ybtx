#pragma once

#include "CoreCommonTest.h"

namespace sqr
{
	class CMetaSceneMgrServer;

	class CTestSuiteMetaSceneServer
		:public TestFixture
	{
	public:
		void setUp(void);
		void tearDown(void);

		CPPUNIT_TEST_SUITE(CTestSuiteMetaSceneServer);
		CPPUNIT_TEST( TestReadBlock );
		CPPUNIT_TEST( TestGetBarrier );
		CPPUNIT_TEST(TestCreateMap);
		CPPUNIT_TEST(TestFindPathNoLoad);
		CPPUNIT_TEST(TestSearchLine);
		CPPUNIT_TEST(TestAStarNoBarrier);
		CPPUNIT_TEST(TestAStarNoBarrier2);
		CPPUNIT_TEST(TestLineCanPassLowBarrier);
		CPPUNIT_TEST(TestAStarCanPassLowBarrier);
		CPPUNIT_TEST(TestUseLineNoPassLowBarrier);
		CPPUNIT_TEST(TestUseLinePassLowBarrier);
		CPPUNIT_TEST(TestAStarCanPassAllBarrier);
		CPPUNIT_TEST(TestReFlush);
		CPPUNIT_TEST(TestLineReFlush);
		CPPUNIT_TEST(TestHypoLine);
		CPPUNIT_TEST(TestUseTime);
		CPPUNIT_TEST(TestLastIsBarrier);
		CPPUNIT_TEST(TestLineFirstIsBarrier);		//直线第一个是障碍的
		CPPUNIT_TEST(TestAFirstIsBarrier);			//a*第一个是障碍的
		CPPUNIT_TEST(TestHypoLineFirstIsBarrier);	//HypoLine第一个是障碍的
		CPPUNIT_TEST(TestAIgnoreEnd);				//a*忽略目标点为障碍的
		CPPUNIT_TEST(TestLineIgnoreEnd);			//直线忽略目标点为障碍的
		CPPUNIT_TEST(TestLineIgnoreEndNoPassBarrier); //直线忽略目标点为障碍的,可是不能找到路径的
		CPPUNIT_TEST_SUITE_END();

	public:
		void TestReadBlock();
		void TestGetBarrier();
		void TestCreateMap();
		void TestFindPathNoLoad();
		void TestSearchLine();
		void TestAStarNoBarrier();
		void TestAStarNoBarrier2();
		void TestLineCanPassLowBarrier();
		void TestAStarCanPassLowBarrier();
		void TestUseLineNoPassLowBarrier();
		void TestUseLinePassLowBarrier();
		void TestAStarCanPassAllBarrier();
		void TestHypoLine();
		void TestLineReFlush();
		void TestReFlush();
		void TestLastIsBarrier();
		void TestUseTime();
		void TestLineFirstIsBarrier();
		void TestAFirstIsBarrier();
		void TestHypoLineFirstIsBarrier();
		void TestLineIgnoreEnd();
		void TestLineIgnoreEndNoPassBarrier();
		void TestAIgnoreEnd();
		
	private:
		CMetaSceneMgrServer*	m_pMetaSceneMgr;
	};

}
