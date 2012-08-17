#pragma once
namespace sqr
{
	class CScript;
}

namespace sqr
{
	class CTestSuiteRpcCall
		:public TestFixture
	{
	public:
		static sqr::CScript* GetScript();
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteRpcCall);

		CPPUNIT_TEST(TestReg);
		CPPUNIT_TEST(TestCall);
		CPPUNIT_TEST(TestWordCall);
		CPPUNIT_TEST(TestDWordCall);
		CPPUNIT_TEST(TestDDWordCall);
		CPPUNIT_TEST(TestStringCall);
		CPPUNIT_TEST(TestBStringCall);
		CPPUNIT_TEST(TestMoreStrCall);
		CPPUNIT_TEST(TestMoreNumCall);
		CPPUNIT_TEST(TestMixCall);


		CPPUNIT_TEST(TestErrorIdCall);
		CPPUNIT_TEST(TestErrorXXCall);
		CPPUNIT_TEST(TestNoFoundFun);
		CPPUNIT_TEST(TestNotAllCall);
		CPPUNIT_TEST(TestNotAllCall2);
		CPPUNIT_TEST(TestClosePipe);
		CPPUNIT_TEST(TestCallError);

		CPPUNIT_TEST(TestNoArgCall);

		CPPUNIT_TEST_SUITE_END();

		void TestReg();
		void TestCall();
		void TestWordCall();
		void TestDWordCall();
		void TestDDWordCall();
		void TestStringCall();
		void TestBStringCall();
		void TestMoreStrCall();
		void TestMoreNumCall();
		void TestMixCall();

		void TestErrorIdCall();
		void TestErrorXXCall();
		void TestNoFoundFun();
		void TestNotAllCall();
		void TestNotAllCall2();
		void TestClosePipe();
		void TestCallError();
		void TestNoArgCall();
	};

}

