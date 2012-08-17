#pragma once

namespace sqr
{
	class CTestSuiteScript
		:public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteScript);
		CPPUNIT_TEST(TestMain);
		CPPUNIT_TEST_SUITE_END();

		void TestMain();
	};

}
