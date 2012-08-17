#include "stdafx.h"
#include "CTestSuiteConnServer.h"



namespace sqr
{
	const char* GetTestSuiteEnDecoderName()
	{
		static TestSuiteFactory<CTestSuiteConnServer> m_factory;
		const char* szSuiteName="CTestSuiteConnServer";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}

	void CTestSuiteConnServer::setUp()
	{

	}

	void CTestSuiteConnServer::tearDown()
	{

	}
}
