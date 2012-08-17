#include "stdafx.h"
#include "CoreCommon.h"
#include "PrtlGas2GacObjectDef.h"
#include "PrtlGas2GacGenericDef.h"
#include "ErrLogHelper.h"
#include "CSyncCoreObjectMediator.h"
#include "CObjMoveStateServer.h"
#include "IPipe.h"
#include "IMulticast.h"
#include "CPixelPath.h"
#include "CMetaSceneServer.h"
#include "CSynConnServer.h"
#include "CSyncCoreSceneServer.h"
#include "TConnection_inl.inl"
#include "CoreObjectServerCommon.h"
#include "TSendAdapter.h"
#include "MoveCallArgs.h"
#include "CObjVarDefCfg.h"
#include "CObjVarDef.h"
#include "CCacheVariantServerHolder.h"
#include "CAppConfigServer.h"
#include "CPathTransformer.h"
#include "CAppServer.h"
#include "CAOICmd.h"
#include "CSynConnMgrServer.h"
#include "CAsyAoiSyncerObj.h"
#include "CLogCoreObject.h"
#include "CCoreObjectDictator.h"
#include "CSyncVariantServerHolder.h"
#include "CSyncAppServer.h"
#include "CSyncMetaSceneServer.h"
#include "CCoreServerObjResult.h"
#include "CCoreServerResult.inl"
#include "CDistortedTimeObj.h"
#include "TBindObject.inl"
#include "CVariantHolderDef.h"
#include "BaseHelper.h"
#ifdef LOG_COREOBJ_MOVE
#include "IPathFinding.h"
#endif

CSyncCoreObjectDictator::CSyncCoreObjectDictator(uint32 uObjID, CSyncCoreSceneServer* pScene,const CFPos& PixelPos,uint32 uVarDefID)
:CSyncCoreObjectServer(uObjID,pScene,PixelPos)
,m_pSyncMulticast(NULL),m_pViewMulticast(NULL)
,m_eBarrierType(eBT_NoBarrier)
,m_fBarrierSize(0.0f)
,m_bEnableFollowerDelay(true)
,m_pBindObj(NULL)
{
	//CCoreObjectDictator* pAsynObj = CCoreObjectServer::GetObjectServer(uObjID)->CastToObjDictator();

	m_pBindObj = new TBindObject<CSyncCoreObjectDictator>(this);

	m_pSyncMulticast=CSyncAppServer::Inst()->GetIMulticast();
	m_pViewMulticast=CSyncAppServer::Inst()->GetIMulticast();
		
	m_uVarDefID = uVarDefID;

	CObjVarDef* pDef=GetVarDefCfg()->Get(uVarDefID);
	CCypherVariantCodeTable* pCodeTable;
	
	for (uint32 i=0;i<pDef->GetSyncCodeTableCount();i++)
	{
		pCodeTable=pDef?pDef->GetSyncCodeTable((ECodeTableIndex)i):NULL;
		m_vecSyncVariantHolder.push_back(new CCacheVariantServerHolder(uVarDefID, pCodeTable, this, true, (ECodeTableIndex)i));
	}
	//pSynCoreObj->GetSyncVariantHolder()->SetSyncVariantHolderImp(m_pSyncVariantHolder);

	for (uint32 i=0;i<pDef->GetViewCodeTableCount();i++)
	{
		pCodeTable=pDef?pDef->GetViewCodeTable((ECodeTableIndex)i):NULL;
		m_vecViewVariantHolder.push_back(new CCacheVariantServerHolder(uVarDefID, pCodeTable, this, false, (ECodeTableIndex)i));
	}
	//pSynCoreObj->GetViewVariantHolder()->SetSyncVariantHolderImp(m_pViewVariantHolder);
#ifdef LOG_COREOBJ_MOVE
	//m_uChangeFolMoveTargetCount = 0;
#endif
}

CSyncCoreObjectDictator::~CSyncCoreObjectDictator(void)
{
	//CCoreObjectDictator* pAsynObj = CCoreObjectServer::GetObjectServer(GetGlobalID())->CastToObjDictator();
#ifdef LOG_COREOBJ_MOVE
	//m_uChangeFolMoveTargetCount = 0;
#endif

	ClearVector(m_vecSyncVariantHolder);
	ClearVector(m_vecViewVariantHolder);
	ClearBarrier();
	m_pViewMulticast->Release();
	m_pViewMulticast = NULL;
	m_pSyncMulticast->Release();
	m_pSyncMulticast = NULL;

	delete m_pBindObj;
	m_pBindObj = NULL;
}

CCacheVariantServer* CSyncCoreObjectDictator::GetSyncVariant(ECodeTableIndex eIndex)const
{
	if (m_vecSyncVariantHolder.size() <= (uint32)eIndex)
		return NULL;
	return m_vecSyncVariantHolder[eIndex]->GetRootVariant();
}

CCacheVariantServer* CSyncCoreObjectDictator::GetViewVariant(ECodeTableIndex eIndex)const
{
	if (m_vecViewVariantHolder.size() <= (uint32)eIndex)
		return NULL;
	return m_vecViewVariantHolder[eIndex]->GetRootVariant();
}

CCacheVariantServerHolder* CSyncCoreObjectDictator::GetSyncVariantHolder(ECodeTableIndex eIndex)const
{
	if (m_vecSyncVariantHolder.size() <= (uint32)eIndex)
		return NULL;
	return m_vecSyncVariantHolder[eIndex];
}

CCacheVariantServerHolder* CSyncCoreObjectDictator::GetViewVariantHolder(ECodeTableIndex eIndex)const
{
	if (m_vecViewVariantHolder.size() <= (uint32)eIndex)
		return NULL;
	return m_vecViewVariantHolder[eIndex];
}

CObjVarDefCfg* CSyncCoreObjectDictator::GetVarDefCfg()
{
	static CObjVarDefCfg ls_DefCfg;
	return &ls_DefCfg;
}

void CSyncCoreObjectDictator::MulticastSendCmd(IMulticast*pMulticast, const void* pData, uint32 uSize)const
{
	pMulticast->SendCmd(pData, uSize);
}

void CSyncCoreObjectDictator::StepExtraConn(sqr::CSynConnServer *pExtraConn, float fMovedDist)
{
	uint32 uMovedDist = (uint32)(fMovedDist * 1000);
	if (uMovedDist < 256)
	{
		CGas2GacOC_Tell_Follower_Stepped_SmallDist cmd;

		cmd.uqbGlobalID=GetGlobalID();
		cmd.uMovedDist=uMovedDist;

		MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));

		if(pExtraConn)
			pExtraConn->SendCoreCmd(&cmd);
	}
	else if (uMovedDist < 65536)
	{
		CGas2GacOC_Tell_Follower_Stepped_MiddleDist cmd;

		cmd.uqbGlobalID=GetGlobalID();
		cmd.uMovedDist=uMovedDist;

		MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));

		if(pExtraConn)
			pExtraConn->SendCoreCmd(&cmd);
	}
	else
	{
		CGas2GacOC_Tell_Follower_Stepped_BigDist cmd;

		cmd.uqbGlobalID=GetGlobalID();
		cmd.fMovedDist=fMovedDist;

		MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));

		if(pExtraConn)
			pExtraConn->SendCoreCmd(&cmd);
	}
}

void CSyncCoreObjectDictator::OnMoveSteppedDerived(const CFPos& OldPixelPos)
{
	CSyncCoreObjectServer::OnMoveSteppedDerived(OldPixelPos);
	StepExtraConn(NULL, GetMovState()->GetMovedDist());
}

void CSyncCoreObjectDictator::MoveBeganExtraConn(const CMoveBeganDerivedArgs& Args,CSynConnServer* pExtraConn)
{
	CGas2GacOC_Move_Follower_To cmd;

#ifdef LOG_COREOBJ_MOVE
	cmd.BeginPixelPos		=	GetPixelPos();
#endif
	cmd.uqbGlobalID			=	GetGlobalID();
	cmd.EndPixelPos			=	Args.EndPixelPos;
	cmd.fEndDist			=	Args.fEndDist;
	cmd.fSpeed				=	Args.fSpeed;

	CPathTransformer pathData(Args.pPixelPath);
	cmd.udbPathDataLen		=	pathData.GetDataLenInByte();
	//cout << cmd.udbPathLen << " CGas2GacOC_Move_Follower_To" << endl;
	//cout << pArg->pThis->GetGlobalID() << " fEndDist " << cmd.fEndDist << endl;

#ifdef LOG_COREOBJ_MOVE
	LogMsg("Move_Fol_To");
#endif

	MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	m_pViewMulticast->SendCmd(pathData.GetData(), pathData.GetDataLenInByte());

	if (pExtraConn)
	{
		pExtraConn->SendCoreCmd(&cmd);
		pExtraConn->SendCoreCmd(pathData.GetData(), pathData.GetDataLenInByte());
	}
}

void CSyncCoreObjectDictator::OnMoveBeganDerived(const CMoveBeganDerivedArgs& Args)
{
	//printf("goodman move begin at(%u,%u)\n",GetPixelPos().x,GetPixelPos().y);
	//cout<<GetGlobalID()<<"\tCCoreObjectDictator::OnMoveBeganDerived"<<endl;
	MoveBeganExtraConn(Args, NULL);
	CSyncCoreObjectServer::OnMoveBeganDerived(Args);
}



void CSyncCoreObjectDictator::MoveSpeedChangedExtraConn(const CMoveSpeedChangedDerivedArgs& Args,CSynConnServer* pExtraConn)
{
	CGas2GacOC_Change_Follower_Move_Speed cmd;

	cmd.uqbGlobalID=GetGlobalID();
	cmd.fSpeed=Args.fSpeed;

	MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	
	if (pExtraConn)
	{
		pExtraConn->SendCoreCmd(&cmd);
	}
}

void CSyncCoreObjectDictator::OnMoveSpeedChangedDerived(const CMoveSpeedChangedDerivedArgs& Args)
{
	MoveSpeedChangedExtraConn(Args, NULL);
	CSyncCoreObjectServer::OnMoveSpeedChangedDerived(Args);
}


void CSyncCoreObjectDictator::MoveStoppedExtraConn(float fStopDist,CSynConnServer* pExtraConn)
{
	CGas2GacOC_Tell_Follower_Stopped cmd;
	cmd.uqbGlobalID=GetGlobalID();
	cmd.fMovedDist=fStopDist;

#ifdef LOG_COREOBJ_MOVE
	LogMsg("Tell_Fol_Stop");
#endif
	MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));

	if (pExtraConn)
	{
		pExtraConn->SendCoreCmd(&cmd);
	}
}

void CSyncCoreObjectDictator::OnMoveStoppedDerived(float fStopDist)
{
	//printf("Move stopped\n");
	MoveStoppedExtraConn(fStopDist, NULL);
	CSyncCoreObjectServer::OnMoveStoppedDerived(fStopDist);
}


void CSyncCoreObjectDictator::MoveEndedExtraConn(CSynConnServer* pExtraConn)
{
	CGas2GacOC_Tell_Follower_Reached cmd;
	cmd.uqbGlobalID=GetGlobalID();

#ifdef LOG_COREOBJ_MOVE
	cmd.ReachedPixelPos = GetPixelPos();
	//cmd.EndDist = GetMovState()->GetMovedDist();
#endif

	MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	if (pExtraConn)
	{
		pExtraConn->SendCoreCmd(&cmd);
	}
}

void CSyncCoreObjectDictator::OnEndMoveDerived()
{
	//cout<<GetGlobalID()<<"\tCCoreObjectDictator::OnEndMoveDerived"<<endl;
	MoveEndedExtraConn(NULL);
	CSyncCoreObjectServer::OnEndMoveDerived();

	//printf("goodman move end at(%u,%u)\n",GetPixelPos().x,GetPixelPos().y);
}
#ifdef LOG_COREOBJ_MOVE
uint32& CSyncCoreObjectDictator::GetChangeFolMoveTargetSessionID()
{
	static uint32 ms_uChangeFolMoveTargetSessionID;
	return ms_uChangeFolMoveTargetSessionID;
}
#endif
void CSyncCoreObjectDictator::MovePathChangedExtraConn(const CMovePathChangedDerivedArgs& Args,CSynConnServer* pExtraConn)
{
	CGas2GacOC_Change_Follower_Move_Target cmd;

	cmd.uqbGlobalID			=	GetGlobalID();
	cmd.fMovedDist			=	Args.fMovedDist;
	cmd.fEndDist			=	Args.fEndDist;
	cmd.fSpeed				=	Args.fSpeed;
	cmd.EndPixelPos			=	Args.EndPixelPos;
#ifdef LOG_COREOBJ_MOVE
	cmd.BeginPixelPos	=	GetPixelPos();
	cmd.uqbSessionID		=	GetChangeFolMoveTargetSessionID();

	ostringstream strm;
	strm << "SendFol " << GetChangeFolMoveTargetSessionID() << " CurPos " << GetPixelPos().x << "," << GetPixelPos().y << " pathlen " << Args.pPixelPath->GetPathPixelLength();
	CFPos BeginPos;
	CFPos EndPos;
	Args.pPixelPath->GetBeginPixelPos(BeginPos);
	Args.pPixelPath->GetEndPixelPos(EndPos);
	strm << " beginpos" << BeginPos.x << "," << BeginPos.y << " endpos" << EndPos.x << "," << EndPos.y;
	const PathList& pathList = Args.pPixelPath->GetGridPath()->GetPath();
	for(uint32 u = 0; u < pathList.size(); ++u)
		strm << " " << pathList[u].x << "," << pathList[u].y;
#endif

	CPathTransformer pathData(Args.pPixelPath);
	cmd.udbPathDataLen			=	pathData.GetDataLenInByte();

#ifdef LOG_COREOBJ_MOVE
	strm << " datalen " << pathData.GetDataLenInByte();
	LogMsg(strm.str());
#endif
	MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	m_pViewMulticast->SendCmd(pathData.GetData(), pathData.GetDataLenInByte());
	if (pExtraConn)
	{
		pExtraConn->SendCoreCmd(&cmd);
		pExtraConn->SendCoreCmd(pathData.GetData(), pathData.GetDataLenInByte());
	}
#ifdef LOG_COREOBJ_MOVE
	++GetChangeFolMoveTargetSessionID();
#endif
}

void CSyncCoreObjectDictator::OnMovePathChangedDerived(const CMovePathChangedDerivedArgs& Args)
{
	MovePathChangedExtraConn(Args, NULL);
	CSyncCoreObjectServer::OnMovePathChangedDerived( Args );
}


void CSyncCoreObjectDictator::SyncPixelPosExtraConn(bool bForceSync,CSynConnServer* pExtraConn)
{
	if( bForceSync )
	{
		CGas2GacOC_Sync_Object_Pos cmd;
		cmd.uqbGlobalID=GetGlobalID();
		
		MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));
		
		if (pExtraConn)
			pExtraConn->SendCoreCmd(&cmd);
	}
	else
	{
		CGas2GacOC_Set_Object_Pos cmd;
		cmd.uqbGlobalID=GetGlobalID();
		cmd.PixelPos=GetPixelPos();

		MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));
		
		if (pExtraConn)
			pExtraConn->SendCoreCmd(&cmd);		
	}
}

void CSyncCoreObjectDictator::OnPixelPosSetDerived(bool bSuperPosition)
{
	SyncPixelPosExtraConn(bSuperPosition, NULL);
	CSyncCoreObjectServer::OnPixelPosSetDerived(bSuperPosition);
}

void CSyncCoreObjectDictator::MulticastCmdForAoi(const void* pData,uint32 uSize, IMulticast* pMulticast)
{
	pMulticast->SendCmd(pData, uSize);
}


void CSyncCoreObjectDictator::BroadCastForAoiExtraConn(const void* pData,uint32 uSize,CSynConnServer* pExtraConn, IMulticast* pMulticast)const
{
	if( uSize <=0xff )
	{
		CGas2GacGC_Small_Shell_Message cmd;
		cmd.usbDataLength = static_cast<uint8>(uSize);

		MulticastSendCmd(pMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));
		if (pExtraConn)
			pExtraConn->SendCoreCmd(&cmd);
	}
	else if(uSize <= 0xffff )
	{
		CGas2GacGC_Middle_Shell_Message cmd;
		cmd.udbDataLength = static_cast<uint16>(uSize);
		MulticastSendCmd(pMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));
		if (pExtraConn)
			pExtraConn->SendCoreCmd(&cmd);
	}
	else
	{
		CGas2GacGC_Big_Shell_Message cmd;
		cmd.uqbDataLength = uSize;
		MulticastSendCmd(pMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));
		if (pExtraConn)
			pExtraConn->SendCoreCmd(&cmd);
	}

	pMulticast->SendCmd(pData, uSize);
	if (pExtraConn)
		pExtraConn->SendCoreCmd(pData, uSize);
}

void CSyncCoreObjectDictator::BroadCastForViewAoiExtraConn(const void* pData,uint32 uSize,CSynConnServer* pExtraConn)const
{
	BroadCastForAoiExtraConn(pData, uSize, pExtraConn, m_pViewMulticast );
}

void CSyncCoreObjectDictator::BroadCastForSyncAoiExtraConn(const void* pData,uint32 uSize,CSynConnServer* pExtraConn)const
{
	BroadCastForAoiExtraConn(pData, uSize, pExtraConn, m_pSyncMulticast );
}

void CSyncCoreObjectDictator::BroadCastForViewAoi(const void* pData,uint32 uSize)const
{
	BroadCastForViewAoiExtraConn(pData,uSize,NULL);
}
void CSyncCoreObjectDictator::BroadCastForSyncAoi(const void* pData,uint32 uSize)const
{
	BroadCastForSyncAoiExtraConn(pData, uSize, NULL);
}

void CSyncCoreObjectDictator::SendCreateObjCmdForConn(CSynConnServer* pConn)
{
	if( GetMovState() )
	{
		CGas2GacOC_Create_Moving_Object cmd;

		const CPixelPath* pPath=GetMovState()->GetPath();

		CFPos BeginPixelPos;
		pPath->GetBeginPixelPos(BeginPixelPos);

		CFPos EndPixelPos;
		pPath->GetEndPixelPos(EndPixelPos);

		cmd.uqbGlobalID			=	GetGlobalID();
		cmd.BeginPixelPos		=	BeginPixelPos;
		cmd.EndPixelPos		=	EndPixelPos;
		cmd.uqbDelay			=	IsFollowerDelayEnabled() ? GetFollowerDelay() : 0;
		cmd.fSpeed				=	GetMovState()->GetSpeed();
		cmd.fMovedDist			=	GetMovState()->GetMovedDist();
		cmd.fEndDist			=	GetMovState()->GetPathLength();
		cmd.usbVarDefCode 		= 	static_cast<uint8>(m_uVarDefID);
		cmd.fBarrierSize		=	m_fBarrierSize;
		cmd.usbSelfBarrierType	=	m_eBarrierType;

		cmd.uobDistortedFrameTime = GetDistortedTime()->GetDistortedFrameTime();

		CPathTransformer pathData(GetMovState()->GetPath());
		cmd.udbPathDataLen = pathData.GetDataLenInByte();

		pConn->SendCoreCmd(&cmd);
		pConn->SendCoreCmd(pathData.GetData(), pathData.GetDataLenInByte());
	}
	else
	{
		SendCreateStillObjCmd(eSOF_Follower,pConn);
	}
}

void CSyncCoreObjectDictator::CreateFollowerForConnection(CSynConnServer* pConn, uint32 uSessionID)
{
	this->SendCreateObjCmdForConn(pConn);
	CGas2GacOC_Check_Object_ViewVariant cmd;
	cmd.uqbGlobalID = GetGlobalID();
	cmd.uSessionID	= uSessionID;
	cmd.uBindParentID = m_pBindObj->GetBindParentID();
	cmd.uStaticVariantEncodeTimes = m_vecViewVariantHolder[eCTI_Static]->GetRealEncodeTimes();
	cmd.uAgileVariantEncodeTimes = m_vecViewVariantHolder[eCTI_Agile]->GetRealEncodeTimes();
	pConn->SendCoreCmd(&cmd);
}

void CSyncCoreObjectDictator::SyncViewVariantHolder(CSynConnServer* pConn, bool bSyncStaticView, bool bSyncAgileView)
{
#ifdef CACHE_VARIANT
	CGas2GacOC_Init_View_Variant	InitCmd;
	InitCmd.uqbGlobalID = GetGlobalID();
	pConn->SendCoreCmd(&InitCmd);
#endif
	if (bSyncStaticView)
	{
		m_vecViewVariantHolder[eCTI_Static]->SyncToConn( pConn );
	}
	if (bSyncAgileView)
	{
		m_vecViewVariantHolder[eCTI_Agile]->SyncToConn( pConn );
	}	
	CGas2GacOC_Sync_View_Variant_End	cmd;
	cmd.uqbGlobalID = GetGlobalID();
	cmd.uBindParentID = m_pBindObj->GetBindParentID();
	cmd.uStaticVariantEncodeTimes = m_vecViewVariantHolder[eCTI_Static]->GetRealEncodeTimes();
	cmd.uAgileVariantEncodeTimes = m_vecViewVariantHolder[eCTI_Agile]->GetRealEncodeTimes();
	pConn->SendCoreCmd(&cmd);
}

void CSyncCoreObjectDictator::SyncSyncVariantHolder(CSynConnServer* pConn, bool bSyncStaticSync, bool bSyncAgileSync)
{
#ifdef CACHE_VARIANT
	CGas2GacOC_Init_Sync_Variant	InitCmd;
	InitCmd.uqbGlobalID = GetGlobalID();
	pConn->SendCoreCmd(&InitCmd);
#endif
	if (bSyncStaticSync)
	{
		m_vecSyncVariantHolder[eCTI_Static]->SyncToConn( pConn );
	}
	if (bSyncAgileSync)
	{
		m_vecSyncVariantHolder[eCTI_Agile]->SyncToConn( pConn );
	}
	CGas2GacOC_Entered_Sync_Aoi cmd;
	cmd.uqbGlobalID = GetGlobalID();
	cmd.uStaticVariantEncodeTimes = m_vecSyncVariantHolder[eCTI_Static]->GetRealEncodeTimes();
	cmd.uAgileVariantEncodeTimes = m_vecSyncVariantHolder[eCTI_Agile]->GetRealEncodeTimes();
	pConn->SendCoreCmd(&cmd);
}

void CSyncCoreObjectDictator::SendCreateStillObjCmd(EStillObjFlag eStillObjFlag,CSynConnServer* pConn)
{
	CGas2GacOC_Create_Still_Object cmd;

	cmd.uqbGlobalID		=	GetGlobalID();
	cmd.PixelPos		=	GetPixelPos();
	cmd.uqbDelay		=	IsFollowerDelayEnabled() ? GetFollowerDelay() : 0;
	cmd.usbTypeFlag		=	eStillObjFlag;
	cmd.usbVarDefCode 	= 	static_cast<uint8>(m_uVarDefID);
	cmd.fBarrierSize	=	m_fBarrierSize;
	cmd.usbSelfBarrierType	=	m_eBarrierType;
	cmd.uobDistortedFrameTime = GetDistortedTime()->GetDistortedFrameTime();
	pConn->SendCoreCmd(&cmd);
}


void CSyncCoreObjectDictator::DestroyObjForConnection(CSynConnServer* pConn)
{
	//cout<<"DestroyObjForConnection "<<GetGlobalID()<<endl;
	CGas2GacOC_Destroy_Object cmd;
	cmd.uqbGlobalID	=	GetGlobalID();

	pConn->SendCoreCmd(&cmd);
}

void CSyncCoreObjectDictator::OnSyncRelationChanged(CSyncCoreObjectMediator* pObjMediator,bool bSee, uint32 uDimension)
{
	Ast (uDimension == 0);
	TellClientSyncRelationChanged(pObjMediator, bSee, uDimension);
}


EServerObjectType CSyncCoreObjectDictator::GetType()const
{
	return eSOT_Dictator;
}

void CSyncCoreObjectDictator::OnViewRelationChanged(CSyncCoreObjectServer* pObjServer,bool bSee, uint32 uDimension)
{
	Ast (uDimension == 0);
	CSyncCoreObjectMediator* pObjMed = pObjServer->CastToObjMediator();
	Ast (pObjMed);
	TellClientViewRelationChanged(pObjMed, bSee, uDimension);
}

void CSyncCoreObjectDictator::TellClientSyncRelationChanged(CSyncCoreObjectMediator* pObjMediator,bool bSee, uint32 uDimension)
{
	CSynConnServer* pConn=pObjMediator->GetConnection();

	if( bSee )
	{
		if( pConn && !pObjMediator->IsBlockingAoiCmd() )
		{
			if(!m_pSyncMulticast->AddPipe( pConn->GetPipe() ))
			{
				CAddress addr;
				pConn->GetRemoteAddress(addr);
				const char* szAddrIP = addr.GetAddress();

				ostringstream strm;
				strm << GetGlobalID() << " see " << pObjMediator->GetGlobalID() << " Dim=" << uDimension << " ip" << szAddrIP;
				LogErr("SyncMulticast->AddPipe err ", strm.str(), pConn);
			}
			pObjMediator->OnObjectEnterSyncAoi(GetGlobalID());
#ifdef CACHE_VARIANT
			CGas2GacOC_Check_Object_SyncVariant cmd;
			cmd.uqbGlobalID = GetGlobalID();
			cmd.uSessionID = pObjMediator->GetEnterAOISessionID();
			cmd.uStaticVariantEncodeTimes = m_vecSyncVariantHolder[eCTI_Static]->GetRealEncodeTimes();
			cmd.uAgileVariantEncodeTimes = m_vecSyncVariantHolder[eCTI_Agile]->GetRealEncodeTimes();
			pConn->SendCoreCmd(&cmd);
#else
			SyncSyncVariantHolder(pConn, true, true);
#endif
		}
	}
	else
	{
		if( pConn && !pObjMediator->IsBlockingAoiCmd() )
		{
			IPipe* pPipe = pConn->GetPipe();
			if(pPipe && !m_pSyncMulticast->DelPipe(pPipe) && pPipe->IsConnected())
			{
				CAddress addr;
				pConn->GetRemoteAddress(addr);
				const char* szAddrIP = addr.GetAddress();

				ostringstream strm;
				strm << GetGlobalID() << " see " << pObjMediator->GetGlobalID() << " Dim=" << uDimension << " ip" << szAddrIP;
				LogErr("SyncMulticast->DelPipe err ", strm.str(), pConn);
			}

			CGas2GacOC_Left_Sync_Aoi cmd;
			cmd.uqbGlobalID = GetGlobalID();
			pConn->SendCoreCmd(&cmd);
			pObjMediator->OnObjectLeaveSyncAoi(GetGlobalID());
		}
	}
}

void CSyncCoreObjectDictator::TellClientViewRelationChanged(CSyncCoreObjectMediator* pObjMediator,bool bSee, uint32 uDimension)
{
	if( uDimension == 0 )
	{
		CSynConnServer* pConn;

		if ( !pObjMediator->IsBlockingAoiCmd() && (pConn=pObjMediator->GetConnection()) )
		{
			if(bSee)
			{
				if(!m_pViewMulticast->AddPipe( pConn->GetPipe() ))
				{
					CAddress addr;
					pConn->GetRemoteAddress(addr);
					const char* szAddrIP = addr.GetAddress();

					ostringstream strm;
					strm << GetGlobalID() << " see " << pObjMediator->GetGlobalID() << " ip" << szAddrIP;
					LogErr("ViewMulticast->AddPipe err ", strm.str(), pConn);
				}
				pObjMediator->OnObjectEnterViewAoi(GetGlobalID());
#ifdef CACHE_VARIANT
				CreateFollowerForConnection( pConn, pObjMediator->GetEnterAOISessionID() );
#else
				this->SendCreateObjCmdForConn(pConn);
				SyncViewVariantHolder(pConn, true, true);
#endif
			}
			else
			{
				IPipe* pPipe = pConn->GetPipe();
				if(pPipe && !m_pViewMulticast->DelPipe(pPipe) && pPipe->IsConnected())
				{
					CAddress addr;
					pConn->GetRemoteAddress(addr);
					const char* szAddrIP = addr.GetAddress();

					ostringstream strm;
					strm << GetGlobalID() << " see " << pObjMediator->GetGlobalID() << " ip" << szAddrIP;
					LogErr("ViewMulticast->DelPipe err ", strm.str(), pConn);
				}
				DestroyObjForConnection( pConn );
				pObjMediator->OnObjectLeaveViewAoi(GetGlobalID());
			}
		}
	}
}

void CSyncCoreObjectDictator::OnMsgFromViewer(const void* pContext)
{
	static_cast<CAoiCmdBase*>(const_cast<void*>(pContext))->HandleCmd(this);
}

ISend* CSyncCoreObjectDictator::GetIS(uint32 uRange)const
{
	static TSendAdapter<CSyncCoreObjectDictator,&CSyncCoreObjectDictator::BroadCastForSyncAoi>	gs_SBC;
	static TSendAdapter<CSyncCoreObjectDictator,&CSyncCoreObjectDictator::BroadCastForViewAoi>	gs_VBC;

	if( uRange == 0 )
	{
		gs_SBC.SetTarget(this);
		return &gs_SBC;
	}
	gs_VBC.SetTarget(this);
	return &gs_VBC;
}

void CSyncCoreObjectDictator::IntSetPositionDerived(const CFPos& PixelPos)
{
	const bool bNeedToUpdateBarrier = this->HasBarrier();
	if(bNeedToUpdateBarrier)
	{
		CPos MyOldGridPos;
		GetGridPos(MyOldGridPos);

		CPos MyNewGridPos;
		MyNewGridPos.x = GetGridByPixel(PixelPos.x);
		MyNewGridPos.y = GetGridByPixel(PixelPos.y);

		if( MyNewGridPos != MyOldGridPos )
		{
			UpdateSceneBarrier(eBT_NoBarrier);
			CSyncCoreObjectServer::IntSetPositionDerived(PixelPos);
			UpdateSceneBarrier(m_eBarrierType);
			return;
		}
	}
	CSyncCoreObjectServer::IntSetPositionDerived(PixelPos);
}

void CSyncCoreObjectDictator::EnableFollowerDelayExtraConn(bool bEnable,CSynConnServer* pExtraConn)
{
	CGas2GacOC_Enable_Follower_Delay Cmd;
	Cmd.uqbGlobalID=GetGlobalID();
	Cmd.uqbDelay=bEnable ? GetFollowerDelay() : 0;	

	MulticastSendCmd(m_pViewMulticast, reinterpret_cast<const void*>(&Cmd), sizeof(Cmd));

	if (pExtraConn)
		pExtraConn->SendCoreCmd(&Cmd);
}

void CSyncCoreObjectDictator::IntEnableFollowerDelay(bool bEnable)
{
	EnableFollowerDelayExtraConn(bEnable, NULL);
}


void CSyncCoreObjectDictator::EnableFollowerDelay(bool bEnable)
{
	if ( m_bEnableFollowerDelay == bEnable )
		return;

	m_bEnableFollowerDelay = bEnable;

	IntEnableFollowerDelay( bEnable );
}

bool CSyncCoreObjectDictator::IsFollowerDelayEnabled()const
{
	return m_bEnableFollowerDelay;
}

uint32 CSyncCoreObjectDictator::GetFollowerDelay()const
{
	return CAppConfigServer::Inst()->GetFollowerDelay();
}

bool CSyncCoreObjectDictator::UpdateSceneBarrier(EBarrierType eBarrierType)
{
	uint8 uDBRadius = GetBarrierSizeInGrid();
	if(uDBRadius <= 0)
		return false;

	BarrierArea barrierArea;
	CSyncMetaSceneServer* pMetaScene = IntGetScene()->GetMetaScene();
	CalcBarrierArea(barrierArea, GetPixelPos(), uDBRadius, pMetaScene->GetWidthInGrid(), pMetaScene->GetHeightInGrid());

	IntGetScene()->SetDynamicBarrierInfo(GetGlobalID(), barrierArea, eBarrierType);

	return true;
}

void CSyncCoreObjectDictator::SetBarrier(float fBarrierSize, EBarrierType eBarrierType)
{
	if(fBarrierSize < 0)
		GenErr("error::BarrierSize is minus");
	if(fBarrierSize > 0 && eBarrierType <= eBT_NoBarrier)
		GenErr("error::BarrierType is illegal");

	UpdateSceneBarrier(eBT_NoBarrier);
	m_fBarrierSize = fBarrierSize;
	UpdateSceneBarrier(m_eBarrierType);
	SyncBarrier();

	if(eBarrierType == m_eBarrierType)
		return;

	if(UpdateSceneBarrier(eBarrierType))
	{
		m_eBarrierType = eBarrierType;
		SyncBarrier();
	}
}

void CSyncCoreObjectDictator::SyncBarrier()const
{
	SyncBarrierExtraConn(NULL);
}

void CSyncCoreObjectDictator::SyncBarrierExtraConn(CSynConnServer* pExtraConn)const
{
	CGas2GacOC_Sync_Dynamic_Barrier cmd;
	cmd.uqbGlobalID = GetGlobalID();
	cmd.fBarrierSize = m_fBarrierSize;
	cmd.usbBarrierType = m_eBarrierType;

	MulticastSendCmd(m_pViewMulticast,reinterpret_cast<const void*>(&cmd), sizeof(cmd));

	if(pExtraConn)
		pExtraConn->SendCoreCmd(&cmd);
}

bool CSyncCoreObjectDictator::HasBarrier()const
{
	return m_fBarrierSize > 0;
}

void CSyncCoreObjectDictator::ClearBarrier()
{
	SetBarrier(0, eBT_NoBarrier);
}

bool CSyncCoreObjectDictator::GetBarrierType(EBarrierType& eBarrierType)const
{
	if(m_fBarrierSize == 0)
		return false;
	eBarrierType = m_eBarrierType;
	return true;
}

uint8 CSyncCoreObjectDictator::GetBarrierSizeInGrid()const
{
	//return m_fBarrierSize == 0.0f ? 0 : uint8(m_fBarrierSize) + 1;
	return uint8(m_fBarrierSize * 2);
}

float CSyncCoreObjectDictator::GetBarrierSize()const
{
	return m_fBarrierSize;
}

void CSyncCoreObjectDictator::PostMsgToSyncee(const void* pContext)
{
	CAsyAoiViewObj* pObj=GetAoiObject();
	if( !pObj )
		return ;
	static_cast<CAsyAoiSyncerObj*>(pObj)->PostMsgToSyncee(pContext);
}

void CSyncCoreObjectDictator::OnMsgFromSyncee(const void* pContext)
{
}

void CSyncCoreObjectDictator::OnMsgToSynceeHandled(const void* pContext)
{
}

void CSyncCoreObjectDictator::LogMsg(const string& log)
{
	ostringstream strm;
	strm << GetGlobalID() << "\t" << log;
	CLogCoreObject::Inst().LogMsg(strm.str());
}

float CSyncCoreObjectDictator::Distance(const CSyncCoreObjectServer* pCoreObj)const
{
	Ast(pCoreObj);
	CFPos posSelf, posTarget;
	posSelf = GetPixelPos();
	posTarget = pCoreObj->GetPixelPos();
	float fDist = posSelf.Dist(posTarget);
	return fDist;
	//if (pCoreObj->CastToObjDictator())
	//{
	//	//cout << "CSyncCoreObjectDictator::Distance fOrgDist = " << fDist << " selfBS = " << GetBarrierSize() <<  " TargetBS = " << pCoreObj->GetBarrierSize() << endl;
	//	float fOffSet = (GetBarrierSize() + pCoreObj->CastToObjDictator()->GetBarrierSize()) * eGridSpanForObj;
	//	fDist -= fOffSet;
	//}
	//return max(0.0f, fDist);
}

void CSyncCoreObjectDictator::OnSyncBindingRelation(uint32 uParentID, bool bRet)
{
	CGas2GacOC_Bind_To Cmd;
	Cmd.uqbGlobalID = GetGlobalID();
	Cmd.uParentID = uParentID;
	Cmd.bRet = bRet;

	MulticastSendCmd(m_pViewMulticast, reinterpret_cast<const void*>(&Cmd), sizeof(Cmd));
}

void CSyncCoreObjectDictator::OnSyncDisbindingParentRelation()
{
	CGas2GacOC_Disbind_From_Parent Cmd;
	Cmd.uqbGlobalID = GetGlobalID();

	MulticastSendCmd(m_pViewMulticast, reinterpret_cast<const void*>(&Cmd), sizeof(Cmd));
}

void CSyncCoreObjectDictator::OnSyncDisbindingAllChildRelation()
{
	CGas2GacOC_Disbind_All_Children Cmd;
	Cmd.uqbGlobalID = GetGlobalID();

	MulticastSendCmd(m_pViewMulticast, reinterpret_cast<const void*>(&Cmd), sizeof(Cmd));
}

void CSyncCoreObjectDictator::IntBeginTransferObject(CSyncCoreSceneServer* pScene , const CFPos& PixelPos)
{
	DisbindAllChildren();
	DisbindFromParent();
	CSyncCoreObjectServer::IntBeginTransferObject( pScene, PixelPos );
}

bool CSyncCoreObjectDictator::BindTo(uint32 uParentID)
{
	return m_pBindObj->BindTo(uParentID);
}

bool CSyncCoreObjectDictator::DisbindFromParent()
{
	return m_pBindObj->DisbindFromParent();
}

bool CSyncCoreObjectDictator::DisbindChild(uint32 uChildID)
{
	return m_pBindObj->DisbindChild(uChildID);
}

bool CSyncCoreObjectDictator::DisbindAllChildren()
{
	return m_pBindObj->DisbindAllChildren();
}

bool CSyncCoreObjectDictator::IsBindingObj() const
{
	return m_pBindObj->IsBindingObj();
}







void CSyncCoreObjectDictator::OnBindTo(uint32 uParentID, bool bRet)
{
	CAoiBindToCmd* pCmd = new CAoiBindToCmd(uParentID, bRet);
	IntPostMsgToSelf(pCmd);	
}

void CSyncCoreObjectDictator::OnBindToAoiDone(uint32 uParentID, bool bRet)
{
	if(IsBindingObj())		//注意在逻辑线程同步语义的前提下，必须判断已绑定才能向客户端发消息，因为在AOI消息回来前可能先收到解绑的Job
	{
		OnSyncBindingRelation(uParentID, bRet);
	}
	//(new CCoreObjOnBindToResult(GetGlobalID(), uParentID, bRet))->Add();
}

void CSyncCoreObjectDictator::OnDisbindFromParent(bool bRet)
{
	OnSyncDisbindingParentRelation();
	//(new CCoreObjOnDisbindFromParentResult(GetGlobalID(), bRet))->Add();
}

void CSyncCoreObjectDictator::OnDisbindToChild(uint32 uChildID, bool bRet)
{
	//(new CCoreObjOnDisbindToChildResult(GetGlobalID(), uChildID, bRet))->Add();
}

void CSyncCoreObjectDictator::OnDisbindToAllChild(bool bRet)
{
	OnSyncDisbindingAllChildRelation();
	//(new CCoreObjOnDisbindToAllChildResult(GetGlobalID(), bRet))->Add();
}


bool CSyncCoreObjectDictator::BindObjIntSetPosition(const CFPos& PixelPos)
{
	return m_pBindObj->BindObjIntSetPosition(PixelPos);
}

void CSyncCoreObjectDictator::BindObjNotifyPosChangedIfNeed()
{
	m_pBindObj->BindObjNotifyPosChangedIfNeed();
}

