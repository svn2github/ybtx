#include "stdafx.h"
#include "CTestSuiteConnClient.h"

namespace sqr
{
	const char* GetTestSuiteEnDecoderName()
	{
		static TestSuiteFactory<CTestSuiteConnClient> m_factory;
		const char* szSuiteName="CTestSuiteConnClient";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}

	void CTestSuiteConnClient::setUp()
	{

	}

	void CTestSuiteConnClient::tearDown()
	{

	}
}

