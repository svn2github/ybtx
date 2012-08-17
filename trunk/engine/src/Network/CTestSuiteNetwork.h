#pragma once

//$Id$
//这个文件包含了Network的所有的单元测试的用例的声明

#include "ExpHelper.h"


namespace sqr
{

	class CTestSuiteNetwork
		:public TestFixture
	{
	public:
		void setUp();
		void tearDown();


		CPPUNIT_TEST_SUITE(CTestSuiteNetwork);

		CPPUNIT_TEST(TestCreation);
		CPPUNIT_TEST(TestOutBuffer);
		CPPUNIT_TEST(TestListen);

		CPPUNIT_TEST(TestConnect);
		CPPUNIT_TEST(TestConnectAndListen);
		CPPUNIT_TEST(TestMultiConnectionToInternet);

		CPPUNIT_TEST(TestConnectFailedAndReconnect);

		CPPUNIT_TEST(TestMultiConnectionToSelf);
		CPPUNIT_TEST(TestSendRecvData);
		CPPUNIT_TEST(TestConnectFailed);

		CPPUNIT_TEST(TestListenFailed);
		CPPUNIT_TEST(TestGracefullyDisconnect);
		CPPUNIT_TEST(TestRegister);

		CPPUNIT_TEST(TestListenAfterConnectError);
		CPPUNIT_TEST(TestConnectAfterListenError);

		CPPUNIT_TEST(TestReactorWaitTime);
		CPPUNIT_TEST(TestCancelBlock);

		CPPUNIT_TEST_SUITE_END();

		void TestCreation();
		void TestOutBuffer();
		void TestListen();

		void TestConnect();
		void TestConnectAndListen();
		void TestMultiConnectionToInternet();

		void TestMultiConnectionToSelf();
		void TestConnectFailed();
		void TestConnectFailedAndReconnect();
		void TestListenFailed();

		void TestSendRecvData();
		void TestGracefullyDisconnect();

		void TestRegister();
		void TestDupRegisterException();
		void TestListenAfterConnectError();

		void TestConnectAfterListenError();
		void TestReactorWaitTime();
		void TestCancelBlock();
	};

}

