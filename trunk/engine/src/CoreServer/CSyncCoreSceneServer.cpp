#include "stdafx.h"
#include "CSyncCoreSceneServer.h"
#include "CSyncCoreObjectMediator.h"
#include "CSyncCoreObjectCalculator.h"
#include "CSyncCoreSceneMgrServer.h"
#include "CSyncMetaSceneServer.h"
#include "TCoreScene.inl"
#include "TPart.inl"
#include "CoreObjectServerCommon.h"
#include "CRectangle.h"
#include "CTimeCheckPoint.h"
#include "BaseHelper.h"
#include "CSyncAppServer.h"
#include "IMulticast.h"
#include "PrtlGas2GacGenericDef.h"
#include "PrtlGas2GacObjectDef.h"
#include "TConnection_inl.inl"
#include "CAsyAoiScene.h"
#include "CAsyAoiViewObj.h"
#include "CAsyAoiSyncerObj.h"
#include "CAsyAoiSynceeObj.h"
#include "CAoiViewObjHandler.h"
#include "CAoiSyncerObjHandler.h"
#include "CAoiSynceeObjHandler.h"
#include "QuadTreeFun.h"
#include "CoreCommon.inl"
#include "ErrLogHelper.h"
#include "CSyncCoreObjectServer.h"
#include "CCoreObjectDictator.h"
#include "CCoreObjectMediator.h"
#include "CSynConnServer.h"
#include "CSynConnMgrServer.h"
#include "CSyncCoreMsgMgr.h"
#include "TIDPtrMap.inl"
#include "DebugHelper.h"
#include "CSyncCoreQuadScene.h"
#include "CCoreTrMirrorBufferMgr.h"
#include "CAoiSceneHandler.h"
#include "TSqrAllocator.inl"
#include "CCoreServerSceneResult.h"
#include "CCoreServerResult.inl"
#include "CSyncDistortedTimeSceneServer.h"

template class TCoreScene<CSyncTraitsServer>;
template class TPart<CSyncCoreSceneServer,CSyncCoreSceneMgrServer>;

CSyncCoreSceneServer* CSyncCoreSceneServer::GetSynCoreSceneServer(uint32 uSceneID)
{
	const uint32 uIndex = TIDPtrMap<CSyncCoreSceneServer*>::GetIndexByID(uSceneID);
	IDVecSynCoreSceneServer& vecSynCoreScene = GetIDVecCCoreSCeneServer();
	if( vecSynCoreScene.size() <= uIndex )
		return NULL;
	return vecSynCoreScene[ uIndex ];
}

CSyncCoreSceneServer::CSyncCoreSceneServer(uint32 uID, CSyncMetaSceneServer* pMetaServer, CSyncCoreSceneMgrServer* pMgr ,const char* szSyncAoiFileName, CDimensionInfo* DimInfo)
   :ParentScene_t( pMetaServer,pMgr )
   ,m_pMulticast( NULL )
   ,m_uID(uID)
   ,m_bDestroying(false)
   ,m_pDistortedTimeScene(new CSyncDistortedTimeSceneServer)
{
	const uint32 uIndex = TIDPtrMap<CSyncCoreSceneServer*>::GetIndexByID(uID);
	IDVecSynCoreSceneServer& vecSynCoreScene = GetIDVecCCoreSCeneServer();
	if( vecSynCoreScene.size() <= uIndex )
		vecSynCoreScene.resize( uIndex+1 );
	vecSynCoreScene[uIndex] = this;

	CAoiSceneHandler* pAoiHandler = new CAoiSceneHandler(this);
	m_pAoiScene = new CAsyAoiScene(szSyncAoiFileName, eGridSpan, GetMetaScene()->GetWidthInPixel(),
		GetMetaScene()->GetHeightInPixel(), pAoiHandler, DimInfo);

	m_pMulticast = CSyncAppServer::Inst()->GetIMulticast();

	(new CCoreEndCreateSyncCoreSceneResult(m_uID))->Add();
}

CSyncCoreSceneServer::~CSyncCoreSceneServer()
{
	//cout << "删除场景";

	CTimeCheckPoint tcp;

	if( HasObject() )
		GenErr("It is not allowed to destroy CoreScene before all CoreObject in which is destroyed!.");

	ReleaseQuadScene();

	CAoiSceneHandler* pAoiHandler = m_pAoiScene->GetHandler();
	m_pAoiScene->Release();
	delete pAoiHandler;

	m_pMulticast->Release();

	const uint32 uIndex = TIDPtrMap<CSyncCoreSceneServer*>::GetIndexByID(m_uID);
	IDVecSynCoreSceneServer& vecSynCoreScene = GetIDVecCCoreSCeneServer();
	vecSynCoreScene[uIndex] = NULL;

	m_bDestroying = false;

	SafeRelease(m_pDistortedTimeScene);
	//cout << "花费时间" << tcp.GetElapse() << endl;	
}

CSyncCoreSceneServer::IDVecSynCoreSceneServer& CSyncCoreSceneServer::GetIDVecCCoreSCeneServer()
{
	static IDVecSynCoreSceneServer ms_vecSynCoreScene;
	return ms_vecSynCoreScene;
}


void CSyncCoreSceneServer::AddToMulticast( CSynConnServer* pConn )
{
	m_pMulticast->AddPipe( pConn->GetPipe() );
}

void CSyncCoreSceneServer::DelFromMulticast( CSynConnServer* pConn )
{
	m_pMulticast->DelPipe( pConn->GetPipe() );
}

CAsyAoiScene* CSyncCoreSceneServer::GetAoiScene() const
{
	return m_pAoiScene;
}

bool CSyncCoreSceneServer::HaltAoiScene()
{
	return m_pAoiScene->Halt();
}

bool CSyncCoreSceneServer::IsPixelValid(const CFPos& PixelPos)const
{
	CPos pos; 
	InlPixelPos2GridPos(pos, PixelPos);

	return IsGridValid(pos);
}

void CSyncCoreSceneServer::ConfirmObjectCreateCondition(const CFPos& PixelPos)
{
	if( !IsPixelValid(PixelPos) )
	{
		ostringstream oss;
		oss<<"Invalid object PixelPos("<<PixelPos.x<<","<<PixelPos.y<<") while creating object!";
		GenErr( "Invalid object PixelPos", oss.str() );
	}
}

void CSyncCoreSceneServer::ConfirmObjectCreateSceneState(CSyncCoreObjectServer* pObj)
{
	if( Destroying() )
	{
		pObj->SetAoiObjectHalt();
		m_listObjectInDestroyingScene.push_back(pObj);
	}
}

void CSyncCoreSceneServer::CreateObjectMediator(uint32 uObjID, const CFPos& PixelPos,uint32 uVarDefID, uint32 uSlot, float fDirectorMaxSpeed)
{
	CSyncCoreObjectMediator* pObj = new CSyncCoreObjectMediator(uObjID, this, PixelPos, uVarDefID, fDirectorMaxSpeed);
	pObj->CreatePosData(uSlot, PixelPos);

	pObj->InsertToScene();
	CreateAoiObj(pObj);

	ConfirmObjectCreateSceneState(pObj);
}

void CSyncCoreSceneServer::CreateObjectDictator(uint32 uObjID, const CFPos& PixelPos, uint32 uVarDefID, uint32 uSlot)
{
	CSyncCoreObjectDictator* pObj = new CSyncCoreObjectDictator(uObjID, this, PixelPos,uVarDefID);
	pObj->CreatePosData(uSlot, PixelPos);

	pObj->InsertToScene();
	CreateAoiObj(pObj);
	ConfirmObjectCreateSceneState(pObj);
}

void CSyncCoreSceneServer::CreateObjectCalculator(uint32 uObjID, const CFPos& PixelPos, uint32 uSlot)
{
	CSyncCoreObjectCalculator* pObj = new CSyncCoreObjectCalculator(uObjID, this, PixelPos);
	pObj->CreatePosData(uSlot, PixelPos);

	pObj->InsertToScene();
	CreateAoiObj(pObj);
	ConfirmObjectCreateSceneState(pObj);
}

void CSyncCoreSceneServer::CreateAoiObj(CSyncCoreObjectServer* pObj)
{
	CAsyAoiViewObj* pAoiObj = NULL;

	switch( pObj->GetType() )
	{
	case eSOT_Calculator:
		{
			CAoiViewObjHandler* pAoiHandler = new CAoiViewObjHandler( pObj );
			pAoiObj=m_pAoiScene->CreateViewObj(pObj->GetPixelPos(), pAoiHandler);
		}
		break;
	case eSOT_Dictator:
		{
			CAoiSyncerObjHandler* pAoiHandler = new CAoiSyncerObjHandler( static_cast<CSyncCoreObjectDictator*>(pObj) );
			pAoiObj = m_pAoiScene->CreateSyncerObj(pObj->GetPixelPos(), pAoiHandler);
		}
		break;
	case eSOT_Mediator:
		{
			CAoiSynceeObjHandler* pAoiHandler = new CAoiSynceeObjHandler( static_cast<CSyncCoreObjectMediator*>(pObj) );
			pAoiObj = m_pAoiScene->CreateSynceeObj(pObj->GetPixelPos(), pAoiHandler);
		}
		break;
	default:
		GenErr("Invalid Server object type.");
	}

	if(pObj->GetAoiObject())
		LogErr("m_pAoiObj != NULL");
	pObj->m_AoiObj.Attach(pAoiObj->m_RefsByCoreObj);
}

void CSyncCoreSceneServer::DestroyAoiObj(CAsyAoiViewObj* pAoiObj)
{
	CAoiViewObjHandler* pHandler=pAoiObj->GetHandler();

	EAoiObjType eType=pAoiObj->GetType(); 

	m_pAoiScene->DestroyObj(pAoiObj);

	switch( eType )
	{
	case eAOT_ViewObj:
		delete static_cast<CAoiViewObjHandler*>(pHandler);
		break;
	case eAOT_SyncerObj:
		delete static_cast<CAoiSyncerObjHandler*>(pHandler);
		break;
	case eAOT_SynceeObj:
		delete static_cast<CAoiSynceeObjHandler*>(pHandler);
		break;
	default:
		{
			ostringstream oss;
			oss<<"Invalid aoi obj type:"<<eType;
			GenErr("CSyncCoreSceneServer::DestroyAoiObj Error", oss.str());
		}
	}

}

CSyncDistortedTimeObjServer* CSyncCoreSceneServer::CreateDistortedTimeObj()
{
	return m_pDistortedTimeScene->CreateDistortedTimeObj();
}

void CSyncCoreSceneServer::DestroyDistortedTimeObj(CDistortedTimeObj* pObj)
{
	m_pDistortedTimeScene->DestroyDistortedTimeObj(pObj);
}

void CSyncCoreSceneServer::SetTimeDistortedRatio(float fTimeDistortedRatio)
{
	m_pDistortedTimeScene->SetTimeDistortedRatio(fTimeDistortedRatio);

	CGas2GacOC_Set_Time_Distorted_Ratio cmd;
	cmd.fRatio = fTimeDistortedRatio;

	ListObject_t listCoreObj = GetAllObject();
	ListObject_t::iterator it = listCoreObj.begin();
	ListObject_t::iterator it_end = listCoreObj.end(); 
	for(; it != it_end; ++it)
	{
		CSyncCoreObjectMediator* pMediator = (*it)->CastToObjMediator();
		if (pMediator)
		{
			CSynConnServer* pConn = pMediator->GetConnection();
			if(pConn)
				pConn->SendCoreCmd(&cmd);
		}
	}
}

float CSyncCoreSceneServer::GetTimeDistortedRatio()const
{
	return m_pDistortedTimeScene->GetTimeDistortedRatio();
}

CSyncMetaSceneServer* CSyncCoreSceneServer::GetMetaScene()const
{
	return static_cast<CSyncMetaSceneServer*>(ParentScene_t::GetMetaScene());
}

void CSyncCoreSceneServer::MulticastSendCmd(IMulticast*pMulticast, const void* pData, uint32 uSize)
{
	pMulticast->SendCmd(pData, uSize);
}

uint32 CSyncCoreSceneServer::Send(const void* pData,uint32 uSize)
{	
	if( uSize <=0xff )
	{
		CGas2GacGC_Small_Shell_Message cmd;
		cmd.usbDataLength = static_cast<uint8>(uSize);

		MulticastSendCmd(m_pMulticast, reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	}
	else if(uSize <= 0xffff )
	{
		CGas2GacGC_Middle_Shell_Message cmd;
		cmd.udbDataLength = static_cast<uint16>(uSize);
		MulticastSendCmd(m_pMulticast, reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	}
	else
	{
		CGas2GacGC_Big_Shell_Message cmd;
		cmd.uqbDataLength = uSize;
		MulticastSendCmd(m_pMulticast, reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	}

	m_pMulticast->SendCmd(pData, uSize);
	return uSize;
}

ISend* CSyncCoreSceneServer::GetIS()const
{
	return const_cast<CSyncCoreSceneServer*>(this);
}


bool CSyncCoreSceneServer::DestroyObject(CSyncCoreObjectServer* pObject)
{
	return pObject?pObject->BeginDestroyObject():true;
}


bool CSyncCoreSceneServer::TransferObject( CSyncCoreObjectServer* pObj, CSyncCoreSceneServer* pScene, const CFPos& PixelPos )
{
	if( pObj->GetScene() != this )
		GenErr("TransferObject failed with error:Object is not in the source scene!");

	if( pScene == this )
		GenErr("TransferObject failed with error:Target scene is the source scene!");

	return pObj->BeginTransferObject( pScene,PixelPos );
}

bool CSyncCoreSceneServer::BeginDestroyScene()
{
	const ListObject_t& lstObject=GetAllObject();

	m_bDestroying = true;

	if( lstObject.empty() )
	{
		HaltAoiScene();
	}
	else
	{
		for( ListObject_t::const_iterator it=lstObject.begin();it != lstObject.end() ;++it )
			(*it)->BeginDestroyObject();
	}

	return true;
}

void CSyncCoreSceneServer::EndDestroyScene()
{
	//这个函数会被aoi scene onhalted消息所调用
	ClearObjectInDestroyingSceneList();

	CSyncCoreSceneMgrServer* pMgr=
		static_cast<CSyncCoreSceneMgrServer*>( ParentMgrPart_t::GetWhole() );

	(new CCoreEndDestroySceneResult(m_uID))->Add();
	delete this;

	pMgr->OnACoreSceneDestroyed();
}

bool CSyncCoreSceneServer::Destroying()const
{
	return m_bDestroying;
}

void CSyncCoreSceneServer::OnObjectDestroyed()
{
	if( !Destroying() )
	{
		//场景并非处于正在删除的状态，某个对象删除对场景没有任何影响。
		return;
	}

	++CSyncAppServer::GetCounter();

	if( HasObject() )
	{
		const ListObject_t& listObject = GetAllObject();
		if (listObject.size()>m_listObjectInDestroyingScene.size())
		{
			//对象还没有删除完毕
			return;
		}
	}

	HaltAoiScene();
}

void CSyncCoreSceneServer::SetDynamicBarrierInfo(uint32 uOwnerID, const DynamicBarrierArea& dynamicBarrier, EBarrierType eBarrierType)
{
	ParentScene_t::SetDynamicBarrierInfo(uOwnerID, dynamicBarrier, eBarrierType);
}

void CSyncCoreSceneServer::ClearObjectInDestroyingSceneList()
{
	while(!m_listObjectInDestroyingScene.empty())
	{
		CSyncCoreObjectServer* pObj = m_listObjectInDestroyingScene.front();
		m_listObjectInDestroyingScene.pop_front();
		pObj->EndDestroyObject();
	}
}

void CSyncCoreSceneServer::CreateQuadScene(uint32 uSlot)

{
	uint32 uWidth = GetMetaScene()->GetWidthInPixel();
	uint32 uHeight = GetMetaScene()->GetHeightInPixel();
	m_pQuadScene = new CSyncCoreQuadScene(uWidth, uHeight, eGridSpan, 
		uSlot, CCoreTrMirrorBufferMgr::Inst()->GetQuadBufferMgr());
}


void CSyncCoreSceneServer::ReleaseQuadScene()
{
	m_pQuadScene->Release();
	m_pQuadScene = NULL;
}
