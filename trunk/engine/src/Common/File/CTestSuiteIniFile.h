#pragma once
#include <string>
#include "CIniFile.h"
#include "cppunit/extensions/HelperMacros.h"

namespace sqr
{
	class CTestSuiteIniFile : public TestFixture
	{
	public:
		CTestSuiteIniFile(void);
		~CTestSuiteIniFile(void);
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteIniFile);

		CPPUNIT_TEST(TestOpenFile);

		CPPUNIT_TEST(TestGetKeyValue);
		CPPUNIT_TEST(TestGetString);
		CPPUNIT_TEST(TestGetInteger);
		CPPUNIT_TEST(TestGetChar);

		CPPUNIT_TEST(TestWriteString);
		CPPUNIT_TEST(TestWriteInteger);
		CPPUNIT_TEST(TestGetKeyValue);

		CPPUNIT_TEST_SUITE_END();

		void TestOpenFile();//测试文件是否打开
		//根据打开的文件，测试从中取得指是否正确
		void TestGetKeyValue();
		void TestGetString();
		void TestGetInteger();
		void TestGetChar();
		////向文件中写入信息，测试信息是否写入
		void TestWriteString();
		void TestWriteInteger();
	};
}
