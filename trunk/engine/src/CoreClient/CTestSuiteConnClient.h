#pragma once

namespace sqr
{
	class CTestSuiteConnClient : public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteConnClient);
		CPPUNIT_TEST_SUITE_END();

	};
}


