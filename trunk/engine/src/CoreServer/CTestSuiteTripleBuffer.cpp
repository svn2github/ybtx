#include "stdafx.h"
#include "CTestSuiteTripleBuffer.h"
#include "CTrMirrorPosBuffer.h"
#include "CMirrorBuffer.h"
#include "CTrMirrorQuadBuffer.h"
#include "CMirrorBuffer.h"

#include "QuadTreeFun.h"

#include "CAsynCorePos.h"
#include "CSyncCorePos.h"

#include "CAsynCoreQuadObject.h"
#include "CAsynCoreQuadScene.h"
#include "CSyncCoreQuadObject.h"
#include "CSyncCoreQuadScene.h"

namespace sqr
{
	const char* RegTestSuiteTripleBuffer()
	{
		static TestSuiteFactory<CTestSuiteTripleBuffer> m_factory;
		const char* szSuiteName="CTestSuiteTripleBuffer";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}
} 


void CTestSuiteTripleBuffer::setUp()
{
	m_uGridSpan = 64;
	m_uMinSplite = 16 * m_uGridSpan;
	m_uPrecision = m_uGridSpan;
}

void CTestSuiteTripleBuffer::tearDown()
{

}

CTrMirrorBuffer* CTestSuiteTripleBuffer::CreatePosBufferMgr(uint32 uDataNum)
{
	return CTrMirrorPosBuffer::CreateBufferMgr(uDataNum);
}

CTrMirrorBuffer* CTestSuiteTripleBuffer::CreateQuadBufferMgr(uint32 uDataNum)
{
	return CTrMirrorQuadBuffer::CreateBufferMgr(uDataNum);
}

void CTestSuiteTripleBuffer::TestCreateAndDelete()
{
	CTrMirrorBuffer* pMgr = CreatePosBufferMgr(10);

	pMgr->Release();

	pMgr = CreateQuadBufferMgr(10);

	pMgr->Release();
}


void CTestSuiteTripleBuffer::TestIdleSlot()
{
	CTrMirrorBuffer* pMgr = CreatePosBufferMgr(1);

	CMirrorBuffer* pReadBuffer = pMgr->GetReadBuffer();

	uint32 uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT_EQUAL((uint32)1, uSlotNum);

	uint32 uSlot = pReadBuffer->WithdrawIdleSlot();
	CPPUNIT_ASSERT_EQUAL((uint32)0, uSlot);

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT_EQUAL((uint32)0, uSlotNum);

	uSlot = pReadBuffer->WithdrawIdleSlot();
	CPPUNIT_ASSERT_EQUAL((uint32)1, uSlot);

	pReadBuffer->RestoreIdleSlot(0);
	pReadBuffer->RestoreIdleSlot(1);

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT_EQUAL((uint32)2, uSlotNum);

	bool bIdle = pReadBuffer->IsIdleSlot(1);
	CPPUNIT_ASSERT_EQUAL(true, bIdle);

	bIdle = pReadBuffer->IsIdleSlot(3);
	CPPUNIT_ASSERT_EQUAL(false, bIdle);

	//-----------------------------------------------------

	CMirrorBuffer* pWriteBuffer = pMgr->GetWriteBuffer();
	uSlotNum = pWriteBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT_EQUAL((uint32)1, uSlotNum);

	bIdle = pWriteBuffer->IsIdleSlot(0);
	CPPUNIT_ASSERT_EQUAL(true, bIdle);

	bIdle = pWriteBuffer->IsIdleSlot(1);
	CPPUNIT_ASSERT_EQUAL(false, bIdle);

	pWriteBuffer->RemoveIdleSlot(0);
	pWriteBuffer->RemoveIdleSlot(1);


	pMgr->SwapWIBuffer();


	pMgr->Release();
}

void CTestSuiteTripleBuffer::TestPos()
{
	CTrMirrorBuffer* pMgr = CreatePosBufferMgr(1);
	CMirrorBuffer* pReadBuffer = pMgr->GetReadBuffer();
	CMirrorBuffer* pWriteBuffer = pMgr->GetWriteBuffer();

	CFPos pos(10, 10);

	CAsynCorePos* pRBData = new CAsynCorePos(pos, pMgr);
	CFPos RBPos = pRBData->GetPos();
	CPPUNIT_ASSERT(pos == RBPos);
	
	uint32 uSlot = pRBData->GetSlot();

	uint32 uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT_EQUAL((uint32)0, uSlotNum);

	//------------------------------------------------

	pWriteBuffer->IncVersion();
	CSyncCorePos* pWBData = new CSyncCorePos(pos, uSlot, pMgr);
	CFPos WBPos = pWBData->GetPos();
	CPPUNIT_ASSERT(pos == WBPos);

	pMgr->SwapWIBuffer();
	WBPos = pWBData->GetPos();
	CPPUNIT_ASSERT(pos == WBPos);

	pMgr->SwapRIBuffer();

	//--------------------------------------------------

	CFPos NewPos(100, 100);

	pWriteBuffer->IncVersion();
	pWBData->SetPos(NewPos);
	WBPos = pWBData->GetPos();
	CPPUNIT_ASSERT(NewPos == WBPos);
	pMgr->SwapWIBuffer();

	pWriteBuffer->IncVersion();
	WBPos = pWBData->GetPos();
	CPPUNIT_ASSERT(NewPos == WBPos);

	NewPos = CFPos(111, 111);
	pWBData->SetPos(CFPos(111, 111));
	pMgr->SwapWIBuffer();

	//---------------------------------------------------
	pMgr->SwapRIBuffer();

	RBPos = pRBData->GetPos();
	CPPUNIT_ASSERT(NewPos == RBPos);

	pWriteBuffer->IncVersion();
	WBPos = pWBData->GetPos();
	CPPUNIT_ASSERT(NewPos == WBPos);

	pRBData->Release();
	pWBData->Release();

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 0);

	uSlotNum = pWriteBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 1);

	pMgr->SwapWIBuffer();
	pMgr->SwapRIBuffer();

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 1);

	pMgr->Release();
}


CFPos TestTriple_GetRandomPos(uint32 uWidth, uint32 uHeight)
{
	int x = (uint32)rand() % uWidth;
	int y = (uint32)rand() % uHeight;

	return CFPos((float)x, (float)y);
}

void CTestSuiteTripleBuffer::TestQuad()
{
	const uint32 uSceneWidth = 512 * m_uGridSpan;
	const uint32 uSceneHeight = 448 * m_uGridSpan;

	CFRect rect = GetQuadRect(uSceneWidth, uSceneHeight);

	CTrMirrorBuffer* pMgr = CreateQuadBufferMgr(3);
	CMirrorBuffer* pReadBuffer = pMgr->GetReadBuffer();
	CMirrorBuffer* pWriteBuffer = pMgr->GetWriteBuffer();

	CAsynCoreQuadScene* pRBScene = new CAsynCoreQuadScene(uSceneWidth, uSceneHeight, m_uPrecision, pMgr);

	uint32 uRBSceneSlot = pRBScene->GetSlot();
	CPPUNIT_ASSERT(uRBSceneSlot == 0);

	CFPos pos = TestTriple_GetRandomPos(uSceneWidth, uSceneHeight);
	CAsynCoreQuadObject* pRBObject = new CAsynCoreQuadObject(0, pos, pMgr);
	uint32 uRBObjectSlot = pRBObject->GetSlot();
	CPPUNIT_ASSERT(uRBObjectSlot == 1);

	pRBObject->SetSize((float)(16 * m_uGridSpan));

	pMgr->SwapRIBuffer();

	//-------------------------------------------------------
	pWriteBuffer->IncVersion();

	CSyncCoreQuadScene* pWBScene = new CSyncCoreQuadScene(uSceneWidth, uSceneHeight, m_uPrecision, 
											  uRBSceneSlot, pMgr);
	uint32 uWBSceneSlot = pWBScene->GetSlot();
	CPPUNIT_ASSERT(uWBSceneSlot == 0);
	
	CSyncCoreQuadObject* pWBObject = new CSyncCoreQuadObject(0, pos, uRBObjectSlot, pMgr);
	uint32 uWBObjectSlot = pWBObject->GetSlot();
	CPPUNIT_ASSERT(uWBObjectSlot == 1);

	uint32 uSlotNum = pWriteBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 1);

	pWBObject->SetSize((float)(16 * m_uGridSpan), pWBScene);
	pWBObject->SetPos(CFPos(100, 100), pWBScene);

	pMgr->SwapWIBuffer();

	//-----------------------------------------------------------
	CFPos pos1 = TestTriple_GetRandomPos(uSceneWidth, uSceneHeight);
	CAsynCoreQuadObject* pRBObject1 = new CAsynCoreQuadObject(1, pos1, pMgr);
	uRBObjectSlot = pRBObject1->GetSlot();
	CPPUNIT_ASSERT(uRBObjectSlot == 2);

	pRBObject1->SetSize((float)(16 * m_uGridSpan));

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 0);

	pMgr->SwapRIBuffer();

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 0);
	
	uRBObjectSlot = pRBObject1->GetSlot();
	CPPUNIT_ASSERT(uRBObjectSlot == 2);

	pos = pRBObject->GetPosition();
	CPPUNIT_ASSERT(pos == CFPos(100, 100));

	//----------------------------------------------------
	
	pWriteBuffer->IncVersion();
	
	CSyncCoreQuadObject* pWBObject1 = new CSyncCoreQuadObject(1, pos1, 2, pMgr);
	uWBObjectSlot = pWBObject1->GetSlot();
	CPPUNIT_ASSERT(uWBObjectSlot == 2);

	pWBObject1->SetSize((float)(16 * m_uGridSpan), pWBScene);

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 0);

	pMgr->SwapWIBuffer();

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 0);

	//---------------------------------------

	pRBObject->Release();
	pRBObject1->Release();
	pRBScene->Release();

	pMgr->SwapRIBuffer();

	pWriteBuffer->IncVersion();

	pWBObject->RemoveFrScene(pWBScene);
	pWBObject->Release();

	pWBObject1->RemoveFrScene(pWBScene);
	pWBObject1->Release();

	pWBScene->Release();
	
	uSlotNum = pWriteBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 3);

	pMgr->SwapWIBuffer();

	pMgr->SwapRIBuffer();

	pWriteBuffer->IncVersion();
	pMgr->SwapWIBuffer();

	uSlotNum = pWriteBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 3);

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 3);

	pMgr->Release();
}

namespace sqr
{
	
	class CTestTripleQueryObject
	{
	public:
		CTestTripleQueryObject(uint32 uId) : m_uId(uId){
			ms_mapId2Object[uId] = this;
		}

		uint32 GetId() { return m_uId; }
		
		static CTestTripleQueryObject* GetObject(uint32 uId)
		{
			return ms_mapId2Object[uId];
		};

	private:
		uint32 m_uId;
		static map<uint32, CTestTripleQueryObject*> ms_mapId2Object;
	};

	map<uint32, CTestTripleQueryObject*> CTestTripleQueryObject::ms_mapId2Object;

	void TestTripleQueryFunc(void* pVecObject, uint32 uId)
	{
		typedef vector<CTestTripleQueryObject*>	VecObject_t;
		VecObject_t* pVec = static_cast<VecObject_t*>(pVecObject);

		CTestTripleQueryObject* pObject = CTestTripleQueryObject::GetObject(uId);
		if(pObject)
			pVec->push_back(pObject);
	}
}



void CTestSuiteTripleBuffer::TestQueryObject()
{
	const uint32 uSceneWidth = 512 * m_uGridSpan;
	const uint32 uSceneHeight = 448 * m_uGridSpan;
	
	CTrMirrorBuffer* pMgr = CreateQuadBufferMgr(3);
	CMirrorBuffer* pWriteBuffer = pMgr->GetWriteBuffer();
	CMirrorBuffer* pReadBuffer = pMgr->GetReadBuffer();

	CFRect rect = GetQuadRect(uSceneWidth, uSceneHeight);

	CTestTripleQueryObject Object0(0);
	CTestTripleQueryObject Object1(1);

	CAsynCoreQuadScene* pRBScene = new CAsynCoreQuadScene(uSceneWidth, uSceneHeight, m_uPrecision, pMgr);
	
	CAsynCoreQuadObject* pRBObject0 = new CAsynCoreQuadObject(0, CFPos(10, 10), pMgr);
	CAsynCoreQuadObject* pRBObject1 = new CAsynCoreQuadObject(1, CFPos(11, 11), pMgr);
	pRBObject0->SetSize(10);
	pRBObject1->SetSize(11);

	vector<CTestTripleQueryObject*> vecObject;
	pRBScene->QueryObject(TestTripleQueryFunc, &vecObject, CFPos(10, 10), 16);

	CPPUNIT_ASSERT(vecObject.size() == 0);
	
	pMgr->SwapRIBuffer();

	//-----------------------------------------
	pWriteBuffer->IncVersion();
	
	CSyncCoreQuadScene* pWBScene = new CSyncCoreQuadScene(uSceneWidth, uSceneHeight, m_uPrecision, 0, pMgr);
	CSyncCoreQuadObject* pWBObject0 = new CSyncCoreQuadObject(0, CFPos(10, 10), 1, pMgr);
	CSyncCoreQuadObject* pWBObject1 = new CSyncCoreQuadObject(1, CFPos(11, 11), 2, pMgr);
	pWBObject0->SetSize(10, pWBScene);
	pWBObject1->SetSize(11, pWBScene);
	
	pMgr->SwapWIBuffer();

	//----------------------------------------
	pMgr->SwapRIBuffer();

	vecObject.clear();
	pRBScene->QueryObject(TestTripleQueryFunc, &vecObject, CFPos(10, 10), 16);

	CPPUNIT_ASSERT(vecObject.size() == 2);
	for(uint32 i = 0; i < (uint32)vecObject.size(); i++)
	{
		CTestTripleQueryObject* pObject = vecObject[i];
		uint32 uId = pObject->GetId();
		CPPUNIT_ASSERT(uId == 0 || uId == 1);
	}

	//----------------------------------------------------------------------
	pWriteBuffer->IncVersion();
	pWBObject1->SetPos(CFPos(400, 400), pWBScene);

	pMgr->SwapWIBuffer();

	pMgr->SwapRIBuffer();

	vecObject.clear();

	pRBScene->QueryObject(TestTripleQueryFunc, &vecObject, CFPos(10, 10), 16);

	CPPUNIT_ASSERT(vecObject.size() == 1);
	CPPUNIT_ASSERT(vecObject[0]->GetId() == 0);

	vecObject.clear();
	pRBScene->QueryObject(TestTripleQueryFunc, &vecObject, CFPos(400, 400), 16);

	CPPUNIT_ASSERT(vecObject.size() == 1);
	CPPUNIT_ASSERT(vecObject[0]->GetId() == 1);

	pRBObject0->Release();
	pRBObject1->Release();
	pRBScene->Release();

	//--------------------------------------------------
	pWriteBuffer->IncVersion();

	pWBObject0->RemoveFrScene(pWBScene);	
	pWBObject0->Release();
	
	pWBObject1->RemoveFrScene(pWBScene);
	pWBObject1->Release();
	
	pWBScene->Release();

	pMgr->SwapWIBuffer();
	pMgr->SwapRIBuffer();

	pWriteBuffer->IncVersion();
	pMgr->SwapWIBuffer();

	uint32 uSlotNum = pWriteBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 3);

	uSlotNum = pReadBuffer->GetIdleSlotNum();
	CPPUNIT_ASSERT(uSlotNum == 3);

	pMgr->Release();
}
