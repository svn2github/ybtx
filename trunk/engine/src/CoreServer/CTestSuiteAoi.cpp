#include "stdafx.h" 
#include "CTestSuiteAoi.h" 
#include "CAoiSynceeObjHandler.h"
#include "CAoiSceneHandler.h"
#include "CAsyAoiSynceeObj.h"
#include "CAsyAoiScene.h"
#include "ExpHelper.h"
#include "CTimeCheckPoint.h"

#ifndef _WIN32
#include <unistd.h>
#endif
 
#include <time.h>

#include "ThreadHelper.h"
#include "ThreadTypes.h"
#include "CAoiThreadMgr.h"

#include "CSyncTimeSystemServer.h"

#include "CAoiQuadTree.h"

#include "CTrMsgBufferSwapper.h"

#include "CDimensionInfo.h"

namespace sqr
{
	const char* RegTestSuiteAoi()
	{
		static TestSuiteFactory<CTestSuiteAoi> m_factory;
		const char* szSuiteName="CTestSuiteAoi";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}
} 
 
namespace sqr 
{ 
	class CAoiCallBack : public CAoiSynceeObjHandler 
	{ 
	public:
		CAoiCallBack(CSyncCoreObjectMediator* pObject = NULL) : CAoiSynceeObjHandler(pObject){}
		virtual void OnViewRelationChanged(CAsyAoiViewObj* pObjServer, bool bSee, uint32 uDimension) {}
		virtual void OnSyncRelationChanged(CAsyAoiSynceeObj* pObjMediator,bool bSee, uint32 uDimension) {}
		virtual void OnMsgFromViewer(const void* pContext){}
		virtual void OnMsgFromViewee(const void* pContext){}
		virtual void OnMsgToVieweeHandled(const void* pContext){}
		virtual void OnMsgToViewerHandled(const void* pContext){}
		virtual void OnMsgFromSelf(const void* pContext){}
		virtual void OnHalted(){}
		virtual void OnMsgFromSyncer(const void* pContext){}
		virtual void OnMsgFromSyncee(const void* pContext){}
		virtual void OnMsgToSyncerHandled(const void* pContext){}
		virtual void OnMsgToSynceeHandled(const void* pContext){}
	}; 
	
	class CTestAoiSceneHandler : public CAoiSceneHandler
	{
	public:
		CTestAoiSceneHandler(CSyncCoreSceneServer* pScene = NULL) : CAoiSceneHandler(NULL){}
		virtual void OnHalted(){}
	};
} 


const int32 MAX_GRID_X = 512 * 64; 
const int32 MAX_GRID_Y = 448 * 64; 
 
const int32 MIN_X_POS = 0; 
const int32 MIN_Y_POS = 0; 
const int32 MAX_X_POS = (MAX_GRID_X - 1); 
const int32 MAX_Y_POS = (MAX_GRID_Y - 1); 
const int32 MoveStep = 64; 
const uint32 AoiObjectNum = 2000;

const char szAoiName[] = "test/CTestSuiteCoreObjectServer/aoi.bmp"; 
 
void CTestSuiteAoi::setUp() 
{ 
	CSyncTimeSystemServer::Init(33);
    CAoiThreadMgr::Init(1);
	
	CDimensionInfo info;
	info.SetDimensionInfo(0, 64);

	m_pAoiScene = new CAsyAoiScene(szAoiName,64, MAX_GRID_X, MAX_GRID_Y, new CTestAoiSceneHandler, &info);
} 
 
 
 
void CTestSuiteAoi::tearDown() 
{
	CTestAoiSceneHandler* pHandler = (CTestAoiSceneHandler*)m_pAoiScene->GetHandler();
	m_pAoiScene->Release();

	delete pHandler;

	CAoiThreadMgr::Unit();

	CSyncTimeSystemServer::Unit();
} 

CAsyAoiSynceeObj* CTestSuiteAoi::CreateAoiObj(const CFPos& pos)
{
	CAsyAoiSynceeObj *pObj = m_pAoiScene->CreateSynceeObj(pos, new CAoiCallBack());
	pObj->SetEyeSight(16);
	pObj->SetSize(20);
	return pObj;
}

void CTestSuiteAoi::DestroyAoiObj(CAsyAoiSynceeObj* pAoiObj)
{
	CAoiViewObjHandler* pHandler=pAoiObj->GetHandler();

	pAoiObj->Halt();

	m_pAoiScene->DestroyObj(pAoiObj);
	delete static_cast<CAoiCallBack*>(pHandler);
}

static map<CAsyAoiSynceeObj*, int32> ms_mapObjToDir;

void CTestSuiteAoi::AddAoiObject(vector<CAsyAoiSynceeObj*> &vec, uint32 num)
{ 
	for (uint32 i = 0; i < num; i++)
	{
		float x = static_cast<float>((abs(rand())) % MAX_X_POS);
		float y = static_cast<float>((abs(rand())) % MAX_Y_POS);

		CAsyAoiSynceeObj *pObj = CreateAoiObj(CFPos(x, y));

		Ver( ms_mapObjToDir.insert(make_pair(pObj, abs(rand())%4) ).second );

		vec.push_back(pObj);
	}
}
 
void CTestSuiteAoi::DelAoiObject(std::vector<CAsyAoiSynceeObj*> &vec)
{
	vector<CAsyAoiSynceeObj*>::iterator iter, end;
	iter = vec.begin();
	end = vec.end();

	while (iter != end)
	{
		CAsyAoiSynceeObj* pAoiObj=(*iter);

		ms_mapObjToDir.erase(pAoiObj);

		DestroyAoiObj(pAoiObj);

		iter++;
	}
	
	vec.clear();
}

/* 
	Direction 
	0 right 
	1 left 
	2 down 
	3 up 
*/ 
 
static void MoveRight(CAsyAoiSynceeObj *pObj) 
{ 
	CFPos pos = pObj->GetPos(); 
 
	if (pos.x + MoveStep > MAX_X_POS) 
	{ 
		ms_mapObjToDir[pObj] = 1; 
		pos.x = pos.x - MoveStep; 
	} 
	else 
	{ 
		pos.x = pos.x + MoveStep; 
	} 
 
	pObj->SetPos(pos); 
} 
 
static void MoveLeft(CAsyAoiSynceeObj *pObj) 
{ 
	CFPos pos = pObj->GetPos(); 
 
	if (pos.x - MoveStep < MIN_X_POS) 
	{ 
		ms_mapObjToDir[pObj] = 0; 
		pos.x = pos.x + MoveStep; 
	} 
	else 
	{ 
		pos.x = pos.x - MoveStep; 
	} 
 
	pObj->SetPos(pos); 
} 
 
static void MoveDown(CAsyAoiSynceeObj *pObj) 
{ 
	CFPos pos = pObj->GetPos(); 
 
	if (pos.y + MoveStep > MAX_Y_POS) 
	{ 
		ms_mapObjToDir[pObj] = 3; 
		pos.y = pos.y - MoveStep; 
	} 
	else 
	{ 
		pos.y = pos.y + MoveStep; 
	} 
 
	pObj->SetPos(pos); 
} 
 
static void MoveUp(CAsyAoiSynceeObj *pObj) 
{ 
	CFPos pos = pObj->GetPos(); 
 
	if (pos.y - MoveStep < MIN_Y_POS) 
	{ 
		ms_mapObjToDir[pObj] = 2; 
		pos.y = pos.y + MoveStep; 
	} 
	else 
	{ 
		pos.y = pos.y - MoveStep; 
	} 
 
	pObj->SetPos(pos); 
} 
 
void CTestSuiteAoi::TestAoi_Move()
{
    vector<CAsyAoiSynceeObj*> vecObj; 

	//创建aoi对象，并随机设置初始位置和移动方向 
	uint32 num = AoiObjectNum;
	AddAoiObject(vecObj, num);

	vector<CAsyAoiSynceeObj*> vecNpc;
	AddAoiObject(vecNpc, num);

	bool bFlag = true;

	CTimeCheckPoint tcp;

	vector<CAsyAoiSynceeObj*>::iterator iter, end; 
	vector<CAsyAoiSynceeObj*>::iterator iter1, end1; 
	//开始移动 
	for (int i = 0; i < 1000000; i++) 
	{ 
	    tcp.SetCheckPoint();
		
		iter = vecObj.begin(); 
		end = vecObj.end(); 
		while (iter != end) 
		{ 
			if (i % 100 == 0)
			{
				int32 dir = (abs(rand())) % 4; 
				ms_mapObjToDir[*iter] = dir; 
			}
			
			float fSize = (float)(rand() % 20) * 64;
			float fEyeSight = (float)(rand() % 30) * 64;

			(*iter)->SetSize(fSize);
			(*iter)->SetEyeSight(fEyeSight);

			switch(ms_mapObjToDir[(*iter)]) 
			{ 
			case 0: 
				MoveRight((*iter)); 
				break; 
			case 1: 
				MoveLeft((*iter)); 
				break; 
			case 2: 
				MoveDown((*iter)); 
				break; 
			case 3: 
				MoveUp((*iter)); 
				break; 
			default: 
				GenErr("Error"); 
				break; 
			} 
			iter++; 
		} 

		bFlag ^= true;
		if (bFlag)
		{
			iter1 = vecNpc.begin();
			end1 = vecNpc.end();
			while (iter1 != end1) 
			{ 
				if (i % 100 == 0)
				{
					int32 dir = (abs(rand())) % 4; 
					ms_mapObjToDir[*iter1] = dir; 
				}

				switch(ms_mapObjToDir[(*iter1)]) 
				{ 
				case 0: 
					MoveRight((*iter1)); 
					break; 
				case 1: 
					MoveLeft((*iter1)); 
					break; 
				case 2: 
					MoveDown((*iter1)); 
					break; 
				case 3: 
					MoveUp((*iter1)); 
					break; 
				default: 
					GenErr("Error"); 
					break; 
				} 
				iter1++; 
			} 
		}
		
		/*CAoiThreadMgr::Inst()->SwapJobBuffer();
		
		CAoiThreadMgr::Inst()->HandleAllJobs();*/

		uint32 time = static_cast<uint32>(tcp.GetElapse());
		cout << time << endl;
		
		if (time < 333)
		{
			sqr::Sleep(333 - time);
		}
		
	} 


	//删除 
	DelAoiObject(vecObj);
	DelAoiObject(vecNpc);
}

void CTestSuiteAoi::TestAoi_SetPos()
{
	vector<CAsyAoiSynceeObj*> vecObj;
	AddAoiObject(vecObj, AoiObjectNum);

	CTimeCheckPoint tcp;

	for (int i = 0; i < 1000000; i++)
	{
		tcp.SetCheckPoint();

		for(int j = 0; j < AoiObjectNum; j++)
		{
			float x = (float)(rand() % MAX_X_POS);
			float y = (float)(rand() % MAX_Y_POS);
			
			float fSize = (float)(rand() % 20) * 64;
			float fEyeSight = (float)(rand() % 30) * 64;
			
			vecObj[j]->SetSize(fSize);
			vecObj[j]->SetPos(CFPos(x, y));	
			vecObj[j]->SetEyeSight(fEyeSight);
		}
		
		CAoiThreadMgr::Inst()->SwapLeftBuffer();
		
		CAoiThreadMgr::Inst()->HandleAllLeftMsg();

		uint32 time = static_cast<uint32>(tcp.GetElapse());
		if (time < 333)
		{
			sqr::Sleep(333 - time);
		}
	}

	DelAoiObject(vecObj);


}
