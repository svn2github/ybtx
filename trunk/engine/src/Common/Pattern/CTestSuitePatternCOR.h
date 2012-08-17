#pragma once
#include "ExpHelper.h"


namespace sqr
{

	class CTestSuitePatternCOR
		:public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuitePatternCOR);

		CPPUNIT_TEST(TestCreation);
		CPPUNIT_TEST(TestDetachMore);
		CPPUNIT_TEST(TestDetachAll);
		CPPUNIT_TEST(TestDetachDiff);
		CPPUNIT_TEST(TestDetachAllInDiff);
		CPPUNIT_TEST(TestAttachDetach);
		CPPUNIT_TEST(TestAttachDel);
		CPPUNIT_TEST(TestDetachHandler);
		CPPUNIT_TEST(TestAttachMore);
		CPPUNIT_TEST(TestAttach2Join);
		CPPUNIT_TEST(TestAttached);
		CPPUNIT_TEST(TestOnEventDetachSelf);
		CPPUNIT_TEST(TestOnEventDetachAll);
		CPPUNIT_TEST(TestOnEventDetachOther);
		CPPUNIT_TEST(TestOnEventDelSubject);
		CPPUNIT_TEST_SUITE_END();

		void TestCreation();
		void TestDetachMore();
		void TestDetachAll();
		void TestDetachDiff();
		void TestDetachAllInDiff();
		void TestAttachDetach();
		void TestAttachDel();
		void TestDetachHandler();
		void TestAttachMore();
		void TestAttach2Join();
		void TestAttached();
		void TestOnEventDetachSelf();
		void TestOnEventDetachAll();
		void TestOnEventDelSubject();
		void TestOnEventDetachOther();
	};

}

