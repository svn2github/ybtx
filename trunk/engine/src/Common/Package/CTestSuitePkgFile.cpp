//$Id: CTestSuitePkgFile.cpp 59050 2007-06-26 03:54:48Z shhuang $
#include "stdafx.h"
#include "CTestSuitePkgFile.h"
#include "CPkgFile.h"
#include "CommonTest.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include <iostream>


namespace sqr
{
	const wstring PATH_ALIAS_TEST = L"res";

#ifdef _WIN32
	class DMALoad
	{
		int32 m_count;
	public:
		DMALoad(){m_count = 0;}
		void Load()
		{
			m_count++;
		}

		static void FuncAsynCall( void * pContext, CRefBufFile& BufFile )
		{
			//	cout<<"str="<<str<<"size"<<size<<endl;
			static_cast<DMALoad *>(pContext)->Load();
		}
	};
#endif


	const char* RegTestSuitePkgFile()
	{
		static TestSuiteFactory<CTestSuitePkgFile> m_factory;
		const char* szSuiteName="CTestSuitePkgFile";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}

	void CTestSuitePkgFile::setUp()
	{

	}


	void CTestSuitePkgFile::tearDown()
	{

	}

	void CTestSuitePkgFile::TestAsynRead()
	{
#ifdef _WIN32
		DMALoad dma;
		CPkgFile::ReadWholeFile( PATH_ALIAS_TEST.c_str(), "test/CTestSuitePkgFile/test.txt",&dma, DMALoad::FuncAsynCall );
		SleepEx(10,true);
		CPkgFile::Flush(100);
#endif
	}

	void CTestSuitePkgFile::OpenFile()
	{

		CPkgFile PkgFile;
		int32 nRet = PkgFile.Open(PATH_ALIAS_TEST.c_str(), "test/CTestSuitePkgFile/test.txt",false);
		PkgFile.Close();
		CPPUNIT_ASSERT_EQUAL((int32)eFE_SUCCESS, nRet);

	}

	void CTestSuitePkgFile::ReadFile()
	{
		char  buff[2048];
		CPkgFile PkgFile;
		PkgFile.Open(PATH_ALIAS_TEST.c_str(), "test/CTestSuitePkgFile/test.txt",false);
		int32 nRet = PkgFile.read(buff,5);
		PkgFile.Close();
		CPPUNIT_ASSERT_EQUAL((int32)5,nRet);
		string szResult(buff,5);
		CPPUNIT_ASSERT_MESSAGE( "last script file name error.\n",szResult == "12345" );
		//	CPPUNIT_ASSERT_EQUAL(buff,"12345");
	}

	void CTestSuitePkgFile::UseError()
	{
		char  buff[2048];
		CPkgFile PkgFile;
		CPPUNIT_ASSERT_EQUAL((int32)-1,PkgFile.read(buff,5));
		PkgFile.Close();
		CPPUNIT_ASSERT_EQUAL((int32)-1,PkgFile.Seek(1,SEEK_SET));
		CPPUNIT_ASSERT_EQUAL((int32)0,PkgFile.Tell());
	}

	void CTestSuitePkgFile::SeekError()
	{
		CPkgFile PkgFile;
		PkgFile.Open(PATH_ALIAS_TEST.c_str(), "test/CTestSuitePkgFile/test.txt",false);
		CPPUNIT_ASSERT_EQUAL((int32)-1,PkgFile.Seek(-1,SEEK_SET));
		CPPUNIT_ASSERT_EQUAL((int32)-1,PkgFile.Seek(12,SEEK_SET));

		CPPUNIT_ASSERT_EQUAL((int32)-1,PkgFile.Seek(-1,SEEK_CUR));
		CPPUNIT_ASSERT_EQUAL((int32)-1,PkgFile.Seek(12,SEEK_CUR));

		CPPUNIT_ASSERT_EQUAL((int32)-1,PkgFile.Seek(1,SEEK_END));
	}

	void CTestSuitePkgFile::DoMore()
	{
		char  buff[2048];
		CPkgFile PkgFile;
		PkgFile.Open(PATH_ALIAS_TEST.c_str(), "test/CTestSuitePkgFile/test.txt",false);
		int32 nRet = PkgFile.read(buff,5);

		PkgFile.Open(PATH_ALIAS_TEST.c_str(), "test/CTestSuitePkgFile/test.txt",false);
		nRet = PkgFile.read(buff,5);

		CPPUNIT_ASSERT_EQUAL((int32)5,nRet);
		string szResult(buff,5);
		CPPUNIT_ASSERT_MESSAGE( "last script file name error.\n",szResult == "12345" );

		PkgFile.Close();
	}

	void CTestSuitePkgFile::SeekReadMore()
	{
		char  buff[2048];
		CPkgFile PkgFile;
		PkgFile.Open(PATH_ALIAS_TEST.c_str(), "test/CTestSuitePkgFile/test.txt",false);
		PkgFile.read(buff,1);
		PkgFile.Seek(1,SEEK_CUR);
		PkgFile.read(buff + 1,2);

		PkgFile.Seek(-1,SEEK_CUR);
		PkgFile.read(buff + 3,2);
		string szResult(buff,5);
		CPPUNIT_ASSERT_MESSAGE( "last script file name error.\n",szResult == "13445" );

		PkgFile.Close();
	}

	void CTestSuitePkgFile::TellMore()
	{
		char  buff[2048];
		CPkgFile PkgFile;
		PkgFile.Open(PATH_ALIAS_TEST.c_str(), "test/CTestSuitePkgFile/test.txt",false);
		PkgFile.read(buff,1);
		PkgFile.Seek(1,SEEK_CUR);
		PkgFile.read(buff + 1,2);
		CPPUNIT_ASSERT_EQUAL((int32)4,PkgFile.Tell());
		PkgFile.Seek(-1,SEEK_CUR);
		PkgFile.read(buff + 3,2);
		CPPUNIT_ASSERT_EQUAL((int32)5,PkgFile.Tell());
		PkgFile.Close();
	}

	void CTestSuitePkgFile::GetSize()
	{
		char  buff[2048];
		CPkgFile PkgFile;
		PkgFile.Open(PATH_ALIAS_TEST.c_str(), "test/CTestSuitePkgFile/test.txt",false);
		PkgFile.read(buff,1);
		CPPUNIT_ASSERT_EQUAL((int32)11,PkgFile.Size());
		PkgFile.Close();
	}

}

