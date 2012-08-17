#include "stdafx.h"
#include "CTestSuiteVariant.h"
#include "CommonTest.h"
#include "CVariant.h"
#include "CCypherVariant.h"
#include "CCypherVariantHolder.h"
#include "CCypherVariantCodeTable.h"
#include "CCypherVariantStream.h"
#include "TVariantStream.inl"


namespace sqr
{
	const char* RegTestSuiteVariant()
	{
		static TestSuiteFactory<CTestSuiteVariant> m_factory;
		const char* szSuiteName="CTestSuiteVariant";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}
}


void CTestSuiteVariant::setUp()
{

}


void CTestSuiteVariant::tearDown()
{

}


void CTestSuiteVariant::TestCreation()
{
	CVariant Variant;	
}


void CTestSuiteVariant::TestInteger()
{
	CVariant Integer;
	Integer.SetNumber<int64>(1);
	CPPUNIT_ASSERT_EQUAL( int64(1),Integer.GetNumber<int64>() );

	Integer.SetNumber<int64>(2);
	CPPUNIT_ASSERT_EQUAL( int64(2),Integer.GetNumber<int64>() );

	CVariant Integer2;
	Integer2.SetNumber<int64>(4);
	CPPUNIT_ASSERT_EQUAL( int64(4),Integer2.GetNumber<int64>() );

	CVariant Integer3;
	Integer3.SetString("hello");
	Integer3.SetNumber<int64>(4);
	CPPUNIT_ASSERT_EQUAL( int64(4),Integer3.GetNumber<int64>() );
}

void CTestSuiteVariant::TestIntegerException()
{
	CVariant Integer;
	Integer.SetNumber<int32>(1);
	Integer.GetNumber<double>();
}


void CTestSuiteVariant::TestDouble()
{
	CVariant Double;
	Double.SetNumber<double>(1.0);
	CPPUNIT_ASSERT_EQUAL( 1.0,Double.GetNumber<double>() );

	Double.SetNumber<double>(2.0);
	CPPUNIT_ASSERT_EQUAL( 2.0,Double.GetNumber<double>() );

	CVariant Double2;
	Double2.SetNumber<double>(2.98);
	CPPUNIT_ASSERT_EQUAL( 2.98,Double2.GetNumber<double>() );

	CVariant Double3;
	Double.SetString("hello");
	Double3.SetNumber<double>(2.98);
	CPPUNIT_ASSERT_EQUAL( 2.98,Double3.GetNumber<double>() );
}


void CTestSuiteVariant::TestDoubleException()
{
	CVariant Double;
	Double.SetNumber<double>(1.0);
	Double.GetString();
}


void CTestSuiteVariant::TestFloat()
{
	CVariant Float;
	Float.SetNumber<float>(1.0f);
	CPPUNIT_ASSERT_EQUAL( 1.0f,Float.GetNumber<float>() );

	Float.SetNumber<float>(2.0f);
	CPPUNIT_ASSERT_EQUAL( 2.0f,Float.GetNumber<float>() );

	CVariant Float2;
	Float2.SetNumber<float>(2.98f);
	CPPUNIT_ASSERT_EQUAL( 2.98f,Float2.GetNumber<float>() );

	CVariant Float3;
	Float3.SetString("hello");
	Float3.SetNumber<float>(2.98f);
	CPPUNIT_ASSERT_EQUAL( 2.98f,Float3.GetNumber<float>() );
}


void CTestSuiteVariant::TestFloatException()
{
	CVariant Float;
	Float.SetNumber<float>(1.0f);
	Float.GetString();
}

void CTestSuiteVariant::TestString()
{
	CVariant String;
	String.SetString("hello");
	CPPUNIT_ASSERT_EQUAL( 0,strcmp("hello",String.GetString()) );

	String.SetString("olleh");
	CPPUNIT_ASSERT_EQUAL( 0,strcmp("olleh",String.GetString()) );

	CVariant String2;
	String2.SetString("olleh");
	CPPUNIT_ASSERT_EQUAL( 0,strcmp("olleh",String2.GetString()) );

	CVariant String3;
	String3.SetNumber<int32>(1);
	String3.SetString("olleh");
	CPPUNIT_ASSERT_EQUAL( 0,strcmp("olleh",String3.GetString()) );
}


void CTestSuiteVariant::TestStringException()
{
	CVariant String;
	String.SetString("hello");
	String.GetMember("hello");
}

void CTestSuiteVariant::TestAssign()
{
	CVariant Result;
	CVariant Var;
	Result=Var;
	CPPUNIT_ASSERT_EQUAL( Result.GetType(),eVVT_Table );
	Var.SetMember("abc")->SetNumber<int8>(123);
	Var.SetMember("cde");
	Var.SetMember("f")->SetString("shit");
	Result=Var;
	CPPUNIT_ASSERT_EQUAL( Result.GetMember("abc")->GetNumber<int8>(),(int8)123 );
	CPPUNIT_ASSERT_EQUAL( Result.GetMember("cde")->GetType(),eVVT_Table );
	CPPUNIT_ASSERT_EQUAL( string(Result.GetMember("f")->GetString()),string("shit") );
}


void CTestSuiteVariant::TestTable()
{
	CVariant Table;
	CPPUNIT_ASSERT( !Table.GetMember("hello") );

	Table.SetMember( "hello" )->SetNumber( 1.0 );
	CPPUNIT_ASSERT_EQUAL( 1.0, Table.GetMember("hello")->GetNumber<double>() );
	CPPUNIT_ASSERT_EQUAL( 1.0, Table.GetMember("hello")->GetNumber<double>() );

	Table.SetMember( "hello1" )->SetString( "hello" );
	CPPUNIT_ASSERT_EQUAL( 0, strcmp("hello",Table.GetMember("hello1")->GetString()) );

	Table.SetMember( "hello" )->SetString( "hello" );
	CPPUNIT_ASSERT_EQUAL( 0, strcmp("hello",Table.GetMember("hello")->GetString()) );

	Table.SetMember( "hello" );
	Table.GetMember("hello")->SetMember("hello")->SetNumber(1.0);
	CPPUNIT_ASSERT_EQUAL( 1.0, Table.GetMember("hello")->GetMember("hello")->GetNumber<double>() );

	Table.DelMember("hello1");
	CPPUNIT_ASSERT( !Table.GetMember("hello1") );

	CPPUNIT_ASSERT_EQUAL( 1.0, Table.GetMember("hello")->GetMember("hello")->GetNumber<double>() );
}


void CTestSuiteVariant::TestTableException()
{
	CVariant Table;
	Table.GetNumber<int64>();
}


void CTestSuiteVariant::TestEnDecodeInt8()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetNumber<int8>(7);
	
	Variant.Encode(strm.GetInputStream());

	RootVariant.Decode(strm.GetOutputStream());

	CPPUNIT_ASSERT_EQUAL( Variant.GetNumber<int8>(), RootVariant.GetRootVariant()->GetNumber<int8>() );
}

void CTestSuiteVariant::TestEnDecodeInt64()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetNumber<int64>(645454545);
	
	Variant.Encode(strm.GetInputStream());	
	RootVariant.Decode(strm.GetOutputStream());

	CPPUNIT_ASSERT_EQUAL( Variant.GetNumber<int64>(), RootVariant.GetRootVariant()->GetNumber<int64>() );
}

void CTestSuiteVariant::TestEnDecodeDouble()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetNumber<double>(0.0001);
	
	Variant.Encode(strm.GetInputStream());	
	RootVariant.Decode(strm.GetOutputStream());

	CPPUNIT_ASSERT_EQUAL( Variant.GetNumber<double>(), RootVariant.GetRootVariant()->GetNumber<double>() );
}


void CTestSuiteVariant::TestEnDecodeFloat()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetNumber<float>(0.0001f);
	
	Variant.Encode(strm.GetInputStream());	
	RootVariant.Decode(strm.GetOutputStream());

	CPPUNIT_ASSERT_EQUAL( Variant.GetNumber<float>(), RootVariant.GetRootVariant()->GetNumber<float>() );
}

void CTestSuiteVariant::TestEnDecodeString()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetString("hello");
	
	Variant.Encode(strm.GetInputStream());	
	RootVariant.Decode(strm.GetOutputStream());

	CPPUNIT_ASSERT_EQUAL( 0,strcmp(Variant.GetString(), RootVariant.GetRootVariant()->GetString()) );
}

void CTestSuiteVariant::TestEnDecodeTable()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetMember("a")->SetString("hello");
	Variant.SetMember("b")->SetNumber<double>(0.001);
	Variant.SetMember("c")->SetMember("d")->SetNumber<int64>(1232323);
	
	Variant.Encode(strm.GetInputStream());	
	RootVariant.Decode(strm.GetOutputStream());

	CPPUNIT_ASSERT_EQUAL( 0,strcmp( Variant.GetMember("a")->GetString(),RootVariant.GetRootVariant()->GetMember("a")->GetString()) );
	CPPUNIT_ASSERT_EQUAL( Variant.GetMember("b")->GetNumber<double>(),RootVariant.GetRootVariant()->GetMember("b")->GetNumber<double>() );
	CPPUNIT_ASSERT_EQUAL( Variant.GetMember("c")->GetMember("d")->GetNumber<int64>(),RootVariant.GetRootVariant()->GetMember("c")->GetMember("d")->GetNumber<int64>() );
	CPPUNIT_ASSERT( ! RootVariant.GetRootVariant()->GetMember("d") );
}


void CTestSuiteVariant::TestEnDecodeSetMember()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetMember("hello",&(strm.GetInputStream()));
	
	RootVariant.Decode(strm.GetOutputStream());

	CPPUNIT_ASSERT( RootVariant.GetRootVariant()->GetMember("hello") );
}

void CTestSuiteVariant::TestEnDecodeDelMember()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetMember("hello");
	
	Variant.Encode(strm.GetInputStream());
	RootVariant.Decode(strm.GetOutputStream());

	strm.Clear();

	strm.Str("");

	Variant.DelMember("hello",&strm);
	RootVariant.Decode(strm);

	CPPUNIT_ASSERT( !RootVariant.GetRootVariant()->GetMember("hello") );
}

void CTestSuiteVariant::TestEnDeCodeSetEmptyTable()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetMember("hello");
	
	Variant.Encode(strm.GetInputStream());
	RootVariant.Decode(strm.GetOutputStream());

	strm.Clear();

	strm.Str("");

	Variant.SetEmptyTable(&strm);
	RootVariant.Decode(strm);

	CPPUNIT_ASSERT( !RootVariant.GetRootVariant()->GetMember("hello") );
}

void CTestSuiteVariant::TestEnDeCodeSetString()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetMember("hello");
	
	Variant.Encode(strm.GetInputStream());
	RootVariant.Decode(strm.GetOutputStream());

	strm.Clear();

	strm.Str("");

	Variant.GetMember("hello")->SetString("damn",&strm);
	RootVariant.Decode(strm);

	CPPUNIT_ASSERT_EQUAL( 0, strcmp( "damn",RootVariant.GetRootVariant()->GetMember("hello")->GetString()) );

	strm.Clear();
	strm.Str("");
	Variant.SetString("damn",&strm);
	RootVariant.Decode(strm.GetOutputStream());

	CPPUNIT_ASSERT_EQUAL( 0, strcmp( "damn",RootVariant.GetRootVariant()->GetString()) );
}

void CTestSuiteVariant::TestEnDeCodeSetNumber()
{
	CCypherVariant	Variant;
	CCypherVariantHolderForDecode	RootVariant(NULL);
	CCypherVariantStream strm;
	
	Variant.SetMember("hello");
	
	Variant.Encode(strm.GetInputStream());
	RootVariant.Decode(strm.GetOutputStream());

	strm.Clear();

	strm.Str("");

	Variant.GetMember("hello")->SetNumber<int64>(12345,&strm);
	RootVariant.Decode(strm);

	CPPUNIT_ASSERT_EQUAL( Variant.GetMember("hello")->GetNumber<int64>(), RootVariant.GetRootVariant()->GetMember("hello")->GetNumber<int64>() );

	strm.Clear();
	strm.Str("");
	Variant.SetNumber<float>(3.0f,&strm);
	RootVariant.Decode(strm.GetOutputStream());

	CPPUNIT_ASSERT_EQUAL( Variant.GetNumber<float>(), RootVariant.GetRootVariant()->GetNumber<float>() );
}

//<root>
//	<Node1>
//		<Node1-1/>
//		<Node1-2/>
//	</Node1>
//	<Node2/>
//</root>
void CTestSuiteVariant::TestCodeTable()
{
	CCypherVariantCodeTable Table;
	ifstream ifile("test/CTestSuiteVariant/TestCodeTable.xml",ios::binary);
	ifstream empty;
	CPPUNIT_ASSERT( ifile );
	Table.LoadConfig(ifile, empty, "root");
	ifile.close();

	CPPUNIT_ASSERT(Table.GetRoot() != NULL);
	CPPUNIT_ASSERT(strcmp(Table.GetEntity(1)->GetName(), "Node1")==0);
	CPPUNIT_ASSERT(strcmp(Table.GetEntity(2)->GetName(), "Node1-1")==0);
	CPPUNIT_ASSERT(strcmp(Table.GetEntity(2)->GetParent()->GetName(), "Node1")==0);
	CPPUNIT_ASSERT(Table.GetEntity(2)->GetParent()->GetID() == 1);
	CPPUNIT_ASSERT(Table.GetEntity(1)->GetParent()->GetID() == 0);

}

