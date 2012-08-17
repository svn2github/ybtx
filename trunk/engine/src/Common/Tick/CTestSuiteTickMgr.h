//$Id: CTestSuiteTickMgr.h 54851 2007-04-11 13:08:43Z shhuang $
#pragma once
#include "CError.h"


namespace sqr
{

	class CTestSuiteTickMgr
		:public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteTickMgr);

		CPPUNIT_TEST(TestCreation);
		CPPUNIT_TEST(TestOnTickNormal);
		CPPUNIT_TEST(TestOnTickRU);
		CPPUNIT_TEST(TestRegisterMore);
		CPPUNIT_TEST(TestDoubleRegister);
		CPPUNIT_TEST(TestOnTickRight);
		CPPUNIT_TEST(TestFree);
		CPPUNIT_TEST(TestMid);
		CPPUNIT_TEST(TestFreeThis);
		CPPUNIT_TEST(TestRegisterMyself);
		CPPUNIT_TEST(TestClearOther);
		CPPUNIT_TEST(TestRegOther);
		CPPUNIT_TEST(TestTwoFrequency);
		CPPUNIT_TEST(TestNum);
		CPPUNIT_TEST(TestAddRemain);
		CPPUNIT_TEST(TestRegTickMgr);
		CPPUNIT_TEST(TestUpperLimit);
		CPPUNIT_TEST(TestRegisterMyselfRing);
		CPPUNIT_TEST(TestRegisterMix);
		CPPUNIT_TEST(TestRegisterLittle);
		CPPUNIT_TEST(TestJustOneSlot);
		CPPUNIT_TEST(TestRegisterZero);
		CPPUNIT_TEST_EXCEPTION(TestCreationError,CError);
		CPPUNIT_TEST_EXCEPTION(TestCreationError1,CError);
		
		//	CPPUNIT_TEST_EXCEPTION(TestRegisterError1,CError);


		CPPUNIT_TEST_SUITE_END();

		void TestCreation();
		void TestCreationError();
		void TestCreationError1();
		void TestOnTickNormal();
		void TestOnTickRU();
		void TestRegisterZero();
		void TestRegisterLittle();
		void TestRegisterMore();
		void TestDoubleRegister();
		void TestOnTickRight();
		void TestFree();
		void TestMid();
		void TestFreeThis();
		void TestRegisterMyself();
		void TestClearOther();
		void TestRegOther();
		void TestTwoFrequency();
		void TestNum();
		void TestAddRemain();
		void TestRegTickMgr();
		void TestUpperLimit();
		void TestRegisterMyselfRing();
		void TestRegisterMix();
		void TestJustOneSlot();
	};

}

