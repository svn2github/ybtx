#pragma once

namespace sqr
{
	class CTestSuiteEnDecoder : public TestFixture
	{
	public:
		void setUp();
		void tearDown();
		CPPUNIT_TEST_SUITE(CTestSuiteEnDecoder);
		CPPUNIT_TEST(TestEnCodeAndDecode);
		CPPUNIT_TEST_SUITE_END();

		void TestEnCodeAndDecode();
	};
}




