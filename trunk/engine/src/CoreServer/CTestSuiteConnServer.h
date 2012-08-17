#pragma once

namespace sqr
{
	class CTestSuiteConnServer : public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteConnServer);
		CPPUNIT_TEST_SUITE_END();

	};
}


