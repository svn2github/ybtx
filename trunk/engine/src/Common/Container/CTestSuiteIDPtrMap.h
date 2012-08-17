#pragma once

namespace sqr
{

	class CTestSuiteIDPtrMap
		:public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteIDPtrMap);
		CPPUNIT_TEST(TestIteration);
		//CPPUNIT_TEST(TestPerformance);
		CPPUNIT_TEST_SUITE_END();

		void TestIteration();
		void TestPerformance();
	};
}
