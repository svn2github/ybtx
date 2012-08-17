#include "stdafx.h"
#include "CTestSuiteTableFile.h"
#include "CommonTest.h"

namespace sqr	
{
	//注册到测试引擎
	const char* RegTestSuiteTableFile()
	{
		static TestSuiteFactory<CTestSuiteTableFile> m_factory;
		const char* szSuiteName="CTestSuiteTableFile";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}

	CTestSuiteTableFile::CTestSuiteTableFile(void)
	{
	}

	CTestSuiteTableFile::~CTestSuiteTableFile(void)
	{
	}

	void CTestSuiteTableFile::setUp()
	{
	}

	void CTestSuiteTableFile::tearDown() 
	{
	}
	//测试加载函数
	void CTestSuiteTableFile::TestLoadFile()
	{
		//CTxtTableFile testfile;
		//CPPUNIT_ASSERT(testfile.Load(L"", "test/CTestSuiteTableFile/test.txt"));

	}
	//测试得到行数函数
	void CTestSuiteTableFile::TestGetHeight()
	{
		CTxtTableFile testfile;
		int value = 0;
		testfile.Load(L"", "test/CTestSuiteTableFile/test.txt");
		value = testfile.GetHeight();
		CPPUNIT_ASSERT(28==value);
	}
	//测试得到列数函数
	void CTestSuiteTableFile::TestGetWidth()
	{
		CTxtTableFile testfile;
		int value = 0;
		testfile.Load(L"", "test/CTestSuiteTableFile/test.txt");
		value = testfile.GetWidth();
		CPPUNIT_ASSERT(3==value);

	}
	//根据列号得到某行某列
	void CTestSuiteTableFile::TestGetString()
	{
		CTxtTableFile testfile;
		const char* szValue;
		testfile.Load(L"", "test/CTestSuiteTableFile/test.txt");
		szValue = testfile.GetString(20,1);
		CPPUNIT_ASSERT(strcmp("424",szValue)==0);
		szValue = testfile.GetString(20,"co2");
		CPPUNIT_ASSERT(strcmp("424",szValue)==0);
	}
	//根据列号得到某行某列
	void CTestSuiteTableFile::TestGetInteger()
	{
		CTxtTableFile testfile;
		int value = 0;
		testfile.Load(L"", "test/CTestSuiteTableFile/test.txt");
		value = testfile.GetInteger(16,0,10);
		CPPUNIT_ASSERT(17==value);

		value = testfile.GetInteger(16,"co2",10);
		CPPUNIT_ASSERT(22==value);

	}
	//根据某行得到某行某列
	void CTestSuiteTableFile::TestGetFloat() 
	{
		CTxtTableFile testfile;
		float value = 0;
		testfile.Load(L"", "test/CTestSuiteTableFile/test.txt");
		value =  testfile.GetFloat(15,2,10);
		CPPUNIT_ASSERT(13==value);

		float val = 0;
		val = testfile.GetFloat(27,"co3",10);
		CPPUNIT_ASSERT(1==val);
	}

}

