#pragma once
#include "CError.h"


namespace sqr
{

	class CCypherVariantStream;

	class CTestSuiteVariant
		:public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteVariant);

		CPPUNIT_TEST(TestCreation);
		CPPUNIT_TEST(TestInteger);
		CPPUNIT_TEST_EXCEPTION(TestIntegerException,CError);
		CPPUNIT_TEST(TestFloat);
		CPPUNIT_TEST_EXCEPTION(TestFloatException,CError);
		CPPUNIT_TEST(TestDouble);
		CPPUNIT_TEST_EXCEPTION(TestDoubleException,CError);
		CPPUNIT_TEST(TestString);
		CPPUNIT_TEST_EXCEPTION(TestStringException,CError);
		CPPUNIT_TEST(TestTable);
		CPPUNIT_TEST_EXCEPTION(TestTableException,CError);
		CPPUNIT_TEST(TestAssign);

		CPPUNIT_TEST(TestEnDecodeInt8);
		CPPUNIT_TEST(TestEnDecodeInt64);
		CPPUNIT_TEST(TestEnDecodeFloat);
		CPPUNIT_TEST(TestEnDecodeDouble);
		CPPUNIT_TEST(TestEnDecodeString);
		CPPUNIT_TEST(TestEnDecodeTable);
		CPPUNIT_TEST(TestEnDecodeSetMember);
		CPPUNIT_TEST(TestEnDecodeDelMember);
		CPPUNIT_TEST(TestEnDeCodeSetEmptyTable);
		CPPUNIT_TEST(TestEnDeCodeSetString);
		CPPUNIT_TEST(TestEnDeCodeSetNumber);
		CPPUNIT_TEST(TestCodeTable);

		CPPUNIT_TEST_SUITE_END();

		void TestCreation();
		void TestInteger();
		void TestIntegerException();
		void TestDouble();
		void TestDoubleException();
		void TestFloat();
		void TestFloatException();
		void TestString();
		void TestStringException();
		void TestTable();
		void TestTableException();
		void TestAssign();

		void TestEnDecodeInt8();
		void TestEnDecodeInt64();
		void TestEnDecodeDouble();
		void TestEnDecodeFloat();
		void TestEnDecodeString();
		void TestEnDecodeTable();
		void TestEnDecodeSetMember();
		void TestEnDecodeDelMember();
		void TestEnDeCodeSetEmptyTable();
		void TestEnDeCodeSetString();
		void TestEnDeCodeSetNumber();

		void TestCodeTable();
	};

}
