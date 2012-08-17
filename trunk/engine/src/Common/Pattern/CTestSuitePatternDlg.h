#pragma once
#include "ExpHelper.h"
#include "PatternDlg.h"
#include "cppunit/extensions/HelperMacros.h" 

namespace sqr
{
	class CTestSuitePatternDlg
		:public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuitePatternDlg);

		CPPUNIT_TEST(TestAdd);
		CPPUNIT_TEST(TestRemove);

		CPPUNIT_TEST_SUITE_END();

		void TestAdd();
		void TestRemove();
		void TestDestroy();
	};

}
