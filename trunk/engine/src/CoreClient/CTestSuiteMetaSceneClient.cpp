#include "stdafx.h"
#include "CTestSuiteMetaSceneClient.h"
#include "CMetaSceneClient.h"
#include "CMetaSceneMgrClient.h"
#include "CPkgFile.h"
#include <fstream>


const char* RegTestSuiteMetaSceneClient()
{
	static TestSuiteFactory<CTestSuiteMetaSceneClient> m_factory;
	const char* szSuiteName="CTestSuiteMetaSceneClient";
	TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
	return szSuiteName;
}

namespace sqr
{

	void CTestSuiteMetaSceneClient::setUp(void)
	{
		m_pMetaSceneMgr=new CMetaSceneMgrClient;
	}

	void CTestSuiteMetaSceneClient::tearDown(void)
	{
		delete m_pMetaSceneMgr;
	}


	void CTestSuiteMetaSceneClient::TestReadRegionFileSimple()
	{
		m_pMetaScene = m_pMetaSceneMgr->CreateMetaScene( "test/CTestSuiteMetaScene/traptest" );
		m_pMetaSceneMgr->DestroyMetaScene(m_pMetaScene);
	}

}