//$Id: CTestSuitePkgFile.h 50627 2007-01-23 10:42:26Z shhuang $
#pragma once
#include "ExpHelper.h"
#include "CTestSuitePkgFile.h"
#include "CPkgFile.h"


namespace sqr
{

	class CTestSuitePkgFile
		:public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuitePkgFile);

		CPPUNIT_TEST(TestAsynRead);
		CPPUNIT_TEST(OpenFile);
		CPPUNIT_TEST(ReadFile);
		CPPUNIT_TEST(UseError);
		CPPUNIT_TEST(SeekError);
		CPPUNIT_TEST(DoMore);
		CPPUNIT_TEST(TellMore);
		CPPUNIT_TEST(GetSize);

		CPPUNIT_TEST(SeekReadMore);

		CPPUNIT_TEST_SUITE_END();

		void GetSize();
		void DoMore();
		void UseError();
		void SeekError();
		void ReadFile();
		void OpenFile();
		void TellMore();
		void SeekReadMore();
		void TestAsynRead();

	};

}

