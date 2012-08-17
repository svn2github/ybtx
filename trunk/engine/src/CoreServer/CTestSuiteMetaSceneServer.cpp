#include "stdafx.h"
#include "CTestSuiteMetaSceneServer.h"
#include "CMetaSceneServer.h"
#include "CMetaSceneMgrServer.h"
#include "CPathFinding.h"
#include <ctime>
#include "CDirSearch.h"
#include <iostream>

using namespace sqr::FindPath;
using namespace sqr;

namespace sqr
{
	const char* RegTestSuiteMetaSceneServer()
	{
		static TestSuiteFactory<CTestSuiteMetaSceneServer> m_factory;
		const char* szSuiteName="CTestSuiteMetaSceneServer";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}
}


void CTestSuiteMetaSceneServer::setUp(void)
{
	m_pMetaSceneMgr=new CMetaSceneMgrServer;
}

void CTestSuiteMetaSceneServer::tearDown(void)
{
	delete m_pMetaSceneMgr;
}

// blocktest为2*2个region，其中障碍信息如下：
//（0,0）高 ，（31,0）中，（31,31）为高，（0,31）低
//（8,5）~（24,5）高 ，（12,10）~（12,26）低
void CTestSuiteMetaSceneServer::TestReadBlock()
{
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->
		CreateMetaScene( "map/newmapa" );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	//高障
	CPPUNIT_ASSERT_EQUAL((int32)3,int32(pFindPath->m_BarrierMgr.GetQuarBarrierType(0, 0)));
	//中障
	CPPUNIT_ASSERT_EQUAL((int32)2,int32(pFindPath->m_BarrierMgr.GetQuarBarrierType(31, 0)));
	//高障
	CPPUNIT_ASSERT_EQUAL((int32)3,int32(pFindPath->m_BarrierMgr.GetQuarBarrierType(31, 31)));
	//低障
	CPPUNIT_ASSERT_EQUAL((int32)1,int32(pFindPath->m_BarrierMgr.GetQuarBarrierType(0, 31)));
	for(uint32 i = 0; i <= 16; i++)
	{
		CPPUNIT_ASSERT_EQUAL((int32)3,int32(pFindPath->m_BarrierMgr.GetQuarBarrierType(8 + i, 5)));
	}
	for(uint32 i = 0; i <= 16; i++)
	{
		CPPUNIT_ASSERT_EQUAL((int32)1,int32(pFindPath->m_BarrierMgr.GetQuarBarrierType(12, 10 + i)));
	}
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}
void CTestSuiteMetaSceneServer::TestGetBarrier()
{
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->
		CreateMetaScene( "map/newmapa" );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	CPPUNIT_ASSERT_EQUAL(pFindPath->GetBarrierType(0,0),eBT_HighBarrier);
	CPPUNIT_ASSERT_EQUAL(pFindPath->GetBarrierType(-1,-1),eBT_OutRange);
	CPPUNIT_ASSERT_EQUAL(pFindPath->GetBarrierType(0,31),eBT_LowBarrier);
	CPPUNIT_ASSERT_EQUAL(pFindPath->GetBarrierType(31,0),eBT_MidBarrier);
	CPPUNIT_ASSERT_EQUAL(pFindPath->GetBarrierType(13,13),eBT_NoBarrier);
}
void CTestSuiteMetaSceneServer::TestCreateMap()
{
	IMap* pMap = CreateMap(10,10);
	pMap->Release();
}
//不load地图直接使用imap类
void CTestSuiteMetaSceneServer::TestFindPathNoLoad()
{
	IMap* pMap = CreateMap(10,10);
	IPath* pPath = pMap->CreatePath(CPos(0, 0), CPos(10, 10), eFPT_AStar, eBT_NoBarrier);
	CPPUNIT_ASSERT(pPath == NULL);
	pMap->Release();
}
//中间没有障碍的，看是否直接用画线的办法判断
void CTestSuiteMetaSceneServer::TestSearchLine()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(0, 1),CPos(0, 10), eFPT_AStar , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(0,1));

	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(0,10));
	pPath->Release();
	//这里不能调用pFindPath->Release();因为在CMetaSceneServer中会调用，这个接口也没有被导出去
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}

//寻路不能过低障
void CTestSuiteMetaSceneServer::TestAStarNoBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(11, 10),CPos(13, 10), eFPT_AStar , eBT_NoBarrier);

	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(11,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(13,10));
	pPath->GetPos(Pos, 2);
	CPPUNIT_ASSERT(Pos == CPos(13,10));
	pPath->Release();
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}

void CTestSuiteMetaSceneServer::TestAStarNoBarrier2()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa" );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	CPath * pPath = (CPath*)pFindPath->CreatePath(CPos(11, 10),CPos(24, 4), eFPT_AStar , eBT_NoBarrier);

	//for(uint32 i = 0; i < pPath->GetPathSize(); i++)
	//{
	//	pPath->GetPos(Pos,i);
	//	cout<< "x = " << Pos.x << " y = " << Pos.y<<endl; 
	//}
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(11,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(24,4));
	pPath->GetPos(Pos, 2);
	CPPUNIT_ASSERT(Pos == CPos(24,4));
	pPath->GetPos(Pos, 3);
	CPPUNIT_ASSERT(Pos == CPos(24,4));
	pPath->GetPos(Pos, 4);
	CPPUNIT_ASSERT(Pos == CPos(24,4));
	pPath->Release();
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}

//寻路能过低障
void CTestSuiteMetaSceneServer::TestLineCanPassLowBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(11, 10),CPos(13, 10), eFPT_AStar , eBT_LowBarrier);

	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(11,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(13,10));
	pPath->Release();
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}


void CTestSuiteMetaSceneServer::TestAStarCanPassLowBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene( "map/newmapa" );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(12, 10),CPos(24, 4), eFPT_AStar , eBT_LowBarrier);

	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(12,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(24,6));
	pPath->GetPos(Pos, 2);
	CPPUNIT_ASSERT(Pos == CPos(25,5));
	pPath->GetPos(Pos, 3);
	CPPUNIT_ASSERT(Pos == CPos(24,4));
	pPath->Release();

	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}

//用直线方式寻路，用于处理跳跃过程,能过低障
void CTestSuiteMetaSceneServer::TestUseLinePassLowBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(10, 10),CPos(15, 10), eFPT_Line , eBT_LowBarrier);

	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(10,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(15,10));
	pPath->Release();

	pPath = pFindPath->CreatePath(CPos(22, 4),CPos(22, 8), eFPT_Line , eBT_LowBarrier);
	CPPUNIT_ASSERT(pPath == NULL);

	m_pMetaSceneMgr->DestroyMetaScene(pLoader);

}

//用直线方式寻路，用于处理跳跃过程,不能过低障
void CTestSuiteMetaSceneServer::TestUseLineNoPassLowBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(10, 10),CPos(15, 10), eFPT_Line , eBT_NoBarrier);

	CPPUNIT_ASSERT(pPath == NULL);

	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}

//寻路，用于处理跳跃过程,能过全部的障碍
void CTestSuiteMetaSceneServer::TestAStarCanPassAllBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(8, 5),CPos(24, 5), eFPT_AStar , eBT_HighBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(8,5));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(24,5));
	pPath->Release();
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}


//生成的ipath用于reflush测试
void CTestSuiteMetaSceneServer::TestReFlush()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(10, 10),CPos(15, 10), eFPT_AStar , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(10,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(12,9));
	pPath->GetPos(Pos, 2);
	CPPUNIT_ASSERT(Pos == CPos(15,10));

	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}
//次优直线寻路的reflush
void CTestSuiteMetaSceneServer::TestLineReFlush()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(10, 10),CPos(15, 10), eFPT_HypoLine , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(10,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(11,10));

	m_pMetaSceneMgr->DestroyMetaScene(pLoader);

}
//次优直线寻路，碰到障碍后返回前一个坐标位置
void CTestSuiteMetaSceneServer::TestHypoLine()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa" );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(10, 10),CPos(15, 10), eFPT_HypoLine , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(10,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(11,10));
	pPath->Release();

	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}
//测试最后一个是障碍的各种情况
void CTestSuiteMetaSceneServer::TestLastIsBarrier()
{

	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(10, 10),CPos(12, 10), eFPT_HypoLine , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(10,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(11,10));
	pPath->Release();

	pPath = pFindPath->CreatePath(CPos(10, 10),CPos(12, 10), eFPT_Line , eBT_NoBarrier);
	CPPUNIT_ASSERT(pPath == NULL);

	pPath = pFindPath->CreatePath(CPos(10, 10),CPos(12, 10), eFPT_AStar , eBT_NoBarrier);
	CPPUNIT_ASSERT(pPath == NULL);
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}
//直线忽略第一个障碍
void CTestSuiteMetaSceneServer::TestLineFirstIsBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(12, 10),CPos(13, 10), eFPT_Line , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(12,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(13,10));
	pPath->Release();
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}
//a*忽略第一个障碍
void CTestSuiteMetaSceneServer::TestAFirstIsBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(12, 10),CPos(12, 27), eFPT_AStar , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(12,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(11,11));
	pPath->GetPos(Pos, 2);
	CPPUNIT_ASSERT(Pos == CPos(11,26));
	pPath->GetPos(Pos, 3);
	CPPUNIT_ASSERT(Pos == CPos(12,27));
	pPath->Release();
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}
//次优寻路忽略第一个障碍
void CTestSuiteMetaSceneServer::TestHypoLineFirstIsBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(12, 10),CPos(13, 10), eFPT_HypoLine , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(12,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(13,10));
	pPath->Release();
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}

void CTestSuiteMetaSceneServer::TestLineIgnoreEnd()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(10, 10),CPos(12,10 ), eFPT_LineIgnoreEnd , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(10,10));
	pPath->GetPos(Pos, 1);
	CPPUNIT_ASSERT(Pos == CPos(12,10));
	pPath->Release();
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}
//直线忽略目标点为障碍的,可是不能找到路径的
void CTestSuiteMetaSceneServer::TestLineIgnoreEndNoPassBarrier()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(10, 10),CPos(13,10 ), eFPT_LineIgnoreEnd , eBT_NoBarrier);
	CPPUNIT_ASSERT(pPath == NULL);
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}
//a*忽略目标点是障碍的
void CTestSuiteMetaSceneServer::TestAIgnoreEnd()
{
	CPos Pos;
	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene(  "map/newmapa"  );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(12, 10),CPos(12,26), eFPT_AStarIgnoreEnd , eBT_NoBarrier);
	pPath->GetPos(Pos, 0);
	CPPUNIT_ASSERT(Pos == CPos(12,10));
	pPath->GetPos(Pos, 3);
	CPPUNIT_ASSERT(Pos == CPos(12,26));
	pPath->Release();
	m_pMetaSceneMgr->DestroyMetaScene(pLoader);
}
//测试a星和贴边的时间比较
void CTestSuiteMetaSceneServer::TestUseTime()
{
	/*int32 nPathNum,nXB,nYB,nXE,nYE,nNum = 32;	
	CSearcher<CNodeState,10000> search;

	time_t t;
	srand(std::time(&t));
	CTimeCheckPoint timego;

	CMetaSceneServer* pLoader=m_pMetaSceneMgr->CreateMetaScene( "test/CTestSuiteMetaScene/blocktest" );
	CMap* pFindPath = static_cast<CMap*>(pLoader->m_pMapPath);
	IPath * pPath = pFindPath->CreatePath(CPos(10, 10),CPos(15, 10), eFPT_AStar , eBT_NoBarrier);
	timego.SetCheckPoint();
	for(int i = 0;i < 2000;)
	{
	nXB = rand()%nNum;
	nYB = rand()%nNum;
	nXE = rand()%nNum;
	nYE = rand()%nNum;
	IPath * pPath = pFindPath->CreatePath(CPos(nXB, nYB),CPos(nXE, nYE), eFPT_AStar , eBT_NoBarrier);
	if(pPath != NULL)
	{
	i++;
	pPath->Release();
	}

	}
	cout <<"Line:"<<pFindPath->m_uCountInLine <<" A* time go =" << timego.GetElapse() << endl;


	timego.SetCheckPoint();
	for(int i = 0;i < 2000;)
	{
	nXB = rand()%nNum;
	nYB = rand()%nNum;
	nXE = rand()%nNum;
	nYE = rand()%nNum;
	if(pFindPath->m_NodeState.IsBarrier(nXB,nYB) || pFindPath->m_NodeState.IsBarrier(nXE,nYE))
	continue;

	search.SearchRoad( &(pFindPath->m_NodeState),sqr::CPathPos(nXB,nYB),sqr::CPathPos(nXE,nYE),nPathNum,eOL_Normal,NULL);
	if(nPathNum >= 0 )
	i++;	
	}
	cout<< "foword time go =" << timego.GetElapse()<<endl;*/
}
