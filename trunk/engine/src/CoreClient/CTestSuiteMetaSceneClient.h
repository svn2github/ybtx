#pragma once

namespace sqr
{
	class CMetaSceneMgrClient;
	class CMetaSceneClient;

	class CTestSuiteMetaSceneClient
		:public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteMetaSceneClient);
		CPPUNIT_TEST( TestReadRegionFileSimple);
		CPPUNIT_TEST_SUITE_END();

	private:
		CMetaSceneMgrClient*	m_pMetaSceneMgr;
		CMetaSceneClient*		m_pMetaScene;

		void TestReadRegionFileSimple();
	};

}