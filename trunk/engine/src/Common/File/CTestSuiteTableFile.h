#pragma once
#include <string>
#include "CTxtTableFile.h"
#include "cppunit/extensions/HelperMacros.h"

namespace sqr
{
	class CTestSuiteTableFile : public TestFixture
	{
	public:
		CTestSuiteTableFile(void);
		~CTestSuiteTableFile(void);
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteTableFile);

		CPPUNIT_TEST(TestLoadFile);

		CPPUNIT_TEST(TestGetHeight);
		CPPUNIT_TEST(TestGetWidth);

		CPPUNIT_TEST(TestGetString);
		CPPUNIT_TEST(TestGetInteger);
		CPPUNIT_TEST(TestGetFloat);

		CPPUNIT_TEST_SUITE_END();

		// 载入制表符分隔文件
		void TestLoadFile();

		// 得到行数
		void TestGetHeight();
		// 得到列数
		void TestGetWidth();

		// 根据列号得到某行某列
		void TestGetString();
		// 根据列号得到某行某列
		void TestGetInteger();
		// 根据列号得到某行某列
		void TestGetFloat();

	};
}

