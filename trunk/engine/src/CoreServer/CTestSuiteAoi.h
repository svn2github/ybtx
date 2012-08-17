#pragma once 
#include "CPos.h"

namespace sqr
{
	class CSynAoiScene;
	class CSynAoiViewObj;
	class CMetaSceneMgrServer;
	class CMetaSceneServer;
	class CTestAoiMoveObj;
	class CSynAoiSynceeObj;
	class CAsyAoiSynceeObj;
	class CAsyAoiScene;
	class CAsyAoiViewObj;
	class CSynAoiSynceeObj;

	class CAsyAoiSynceeObj;
	class CAsyAoiScene;

	class CTrMsgBufferSwapper;

	class CTestSuiteAoi : public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteAoi);

		CPPUNIT_TEST(TestAoi_SetPos);
		//CPPUNIT_TEST(TestAoi_Move);
		CPPUNIT_TEST_SUITE_END();
	private:
		void AddAoiObject(vector<CAsyAoiSynceeObj*> &vec, uint32 num);
		void DelAoiObject(vector<CAsyAoiSynceeObj*> &vec);

		void DestroyAoiObj(CAsyAoiSynceeObj* pAoiObj);
		CAsyAoiSynceeObj* CreateAoiObj(const CFPos& pos);

		CAsyAoiScene *m_pAoiScene;

		CTrMsgBufferSwapper*	m_pTrBufferMgr;
		
		void TestAoi_SetPos();
		void TestAoi_Move();
	};	
}
