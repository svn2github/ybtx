#pragma once

namespace sqr
{
	class CTestSuiteSafeContainer : public TestFixture
	{
	public:
		void setUp();
		void tearDown();
		
		CPPUNIT_TEST_SUITE(CTestSuiteSafeContainer);

		CPPUNIT_TEST(TestListEmpty);
		CPPUNIT_TEST(TestListModify);
		CPPUNIT_TEST(TestListComplexModify);

		CPPUNIT_TEST(TestSTLMapIteratorStable);

		CPPUNIT_TEST(TestMapEmpty);
		CPPUNIT_TEST(TestMapModify);
		CPPUNIT_TEST(TestMapComplexModify);

		CPPUNIT_TEST(TestSetEmpty);
		CPPUNIT_TEST(TestSetModify);
		CPPUNIT_TEST(TestSetComplexModify);

		CPPUNIT_TEST_SUITE_END();

		void TestListEmpty();
		void TestListModify();
		void TestListComplexModify();

		void TestSTLMapIteratorStable();

		void TestMapEmpty();
		void TestMapModify();
		void TestMapComplexModify();

		void TestSetEmpty();
		void TestSetModify();
		void TestSetComplexModify();
	};
}
