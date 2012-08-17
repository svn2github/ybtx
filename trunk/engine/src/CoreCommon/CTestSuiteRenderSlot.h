#pragma once

namespace sqr
{
	class CTestSuiteRenderSlot : public TestFixture
	{
	public:
		void setUp();
		void tearDown();
		
		CPPUNIT_TEST_SUITE(CTestSuiteRenderSlot);

		CPPUNIT_TEST(TestUpdateWithDeleteOther);
		CPPUNIT_TEST(TestInsertAndDelete);

		CPPUNIT_TEST_SUITE_END();

		void TestUpdateWithDeleteOther();
		void TestInsertAndDelete();
	};
}
