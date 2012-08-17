#include "stdafx.h"
#include "CSyncCoreSceneServer.h"
#include "CSyncCoreSceneMgrServer.h"
#include "TCoreSceneMgr.inl"
#include "TWhole.inl"
#include "CAoiThreadMgr.h"
#include "CTimeCheckPoint.h"
#include "CSyncAppServer.h"
#include "CCoreSceneServer.h"
#include "ExpHelper.h"
#include "DebugHelper.h"

template class TCoreSceneMgr<CSyncTraitsServer>;
template class TWhole<CSyncCoreSceneMgrServer,CSyncCoreSceneServer>;

CSyncCoreSceneMgrServer::CSyncCoreSceneMgrServer()
:m_bHalting( false )
{
	CAoiThreadMgr::Init(1);
}

CSyncCoreSceneMgrServer::~CSyncCoreSceneMgrServer()
{
	ClearParts();

	CTimeCheckPoint tcp;
	cout << "关闭aoi线程";
	CAoiThreadMgr::Unit();
	cout << "花费时间" << tcp.GetElapse() << endl;
}

CSyncCoreSceneServer* CSyncCoreSceneMgrServer::
CreateCoreScene(uint32 uScenID, CSyncMetaSceneServer* pMetaScene, const char* szSyncAoiFileName, CDimensionInfo* DimInfo, uint32 uSlot)
{
	if( Halting() )
		GenErr("Can't create CoreScene  while application is doing quit!");
	if(!pMetaScene || !szSyncAoiFileName)
		return NULL;

	CSyncCoreSceneServer* pSynCoreScene  = new CSyncCoreSceneServer(uScenID, pMetaScene, this, szSyncAoiFileName, DimInfo);
	pSynCoreScene->CreateQuadScene(uSlot);



	return pSynCoreScene;
}

bool CSyncCoreSceneMgrServer::DestroyCoreScene(CSyncCoreSceneServer* pScene)
{
	return pScene->BeginDestroyScene();
}

uint32 CSyncCoreSceneMgrServer::Send(const void* pData,uint32 uSize)
{
	for(list<CSyncCoreSceneServer*>::const_iterator it=GetParts().begin();it!=GetParts().end();++it)
		static_cast<ISend*>(*it)->Send(pData,uSize);

	return uSize;
}

bool CSyncCoreSceneMgrServer::Halting()const
{
	return m_bHalting;
}

bool CSyncCoreSceneMgrServer::Halt()
{
	if( Halting() )
		return false;

	m_bHalting = true;	

	if( HasPart() )
	{
		for(list<CSyncCoreSceneServer*>::const_iterator it=GetParts().begin();it!=GetParts().end();++it)
			(*it)->BeginDestroyScene();
	}
	else
	{
		//所有的CoreScene都已经被删除，立即出发完成的通知。
		CSyncAppServer::Inst()->OnSceneMgrHalted();
	}

	CSyncAppServer::Inst()->Log("Halt SyncSceneMgr.");

	return true;
}

void CSyncCoreSceneMgrServer::OnACoreSceneDestroyed()
{
	//服务器并不是处于关闭状态
	if( !Halting() )
		return;

	//场景仍然没有被删除完毕
	if( HasPart() )
	{
		ostringstream strm;
		
		strm<<"剩余"<<GetParts().size()<<"个场景未删除";
		CSyncAppServer::Inst()->Log(strm.str().c_str());
	
		return;
	}

	CSyncAppServer::Inst()->Log("SyncSceneMgr halted.");

	CSyncAppServer::Inst()->OnSceneMgrHalted();
}
