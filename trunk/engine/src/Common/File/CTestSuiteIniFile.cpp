#include "stdafx.h"
#include "CTestSuiteIniFile.h"
#include "CommonTest.h"

namespace sqr
{
	const char* RegTestSuiteIniFile()
	{
		static TestSuiteFactory<CTestSuiteIniFile> m_factory;
		const char* szSuiteName="CTestSuiteIniFile";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}

	CTestSuiteIniFile::CTestSuiteIniFile(void)
	{
	}

	CTestSuiteIniFile::~CTestSuiteIniFile(void)
	{
	}

	void CTestSuiteIniFile::setUp()
	{
	}

	void CTestSuiteIniFile::tearDown()
	{
	}

	void CTestSuiteIniFile::TestOpenFile()
	{
		CIniFile inifile;
		CPPUNIT_ASSERT(inifile.Open(L"", L"test/CTestSuiteIniFile/test.ini"));
	}

	void CTestSuiteIniFile::TestGetKeyValue()
	{
		CIniFile testfile;

		testfile.Open(L"", L"test/CTestSuiteIniFile/test.ini");
		string value = testfile.GetValue("Book", "DataBase", string("default"));

		CPPUNIT_ASSERT(value == "35$");
	}

	void CTestSuiteIniFile::TestGetString()
	{
		CIniFile testfile;

		testfile.Open(L"", L"test/CTestSuiteIniFile/test.ini");
		
		string value;
		value = testfile.GetValue("Book", "Data", string("default"));//没有找到对应的key值时，写入给定参数值
		CPPUNIT_ASSERT(value == "default");

		value = testfile.GetValue("Book", "DataBase", string("default"));//找到对应的key值时，将key对应的value传给lpRString
		CPPUNIT_ASSERT(value == "35$");
	}

	void CTestSuiteIniFile::TestGetInteger()
	{
		CIniFile testfile;

		testfile.Open(L"", L"test/CTestSuiteIniFile/test.ini");

		CPPUNIT_ASSERT(25 == testfile.GetValue("Ball", "FootBool", 100));
		CPPUNIT_ASSERT(100 == testfile.GetValue("Ball", "Tennis", 100));
	}

	void CTestSuiteIniFile::TestGetChar()
	{
		CIniFile testfile;

		testfile.Open(L"", L"test/CTestSuiteIniFile/test.ini");
		
		CPPUNIT_ASSERT('2' == testfile.GetValue("Ball", "FootBool", 'D'));
	}

	void CTestSuiteIniFile::TestWriteString()
	{
		CIniFile testfile;

		testfile.Open(L"", L"test/CTestSuiteIniFile/test.ini");
		testfile.SetValue("Ball", "BasketBool", "25");

		string value = testfile.GetValue("Ball", "FootBool", string("default"));//参数分别为[]括号中值，key值以及value值

		CPPUNIT_ASSERT(value == "25");
	}

	void CTestSuiteIniFile::TestWriteInteger()
	{
		CIniFile testfile;

		testfile.Open(L"", L"test/CTestSuiteIniFile/test.ini");
		testfile.SetValue("Animal", "Cat", 60);

		string value = testfile.GetValue("Animal", "Cat", string("default"));//参数分别为[]括号中值，key值以及value值
		CPPUNIT_ASSERT(value == "60");

		CPPUNIT_ASSERT(60 == testfile.GetValue("Animal", "Cat", 0));
	}
}

