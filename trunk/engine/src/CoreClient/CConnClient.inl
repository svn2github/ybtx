#pragma once
//$id$
//对象系统使用的连接对象,仅负责对象系统相关的网络消息的派发
#include "CCoreSceneClient.h"
#include "CoreCommon.h"
#include "CCoreObjectFollower.h"
#include "CCoreObjectDirector.h"
#include "CConnClient.h"
#include "CQuickRand.h"
#include "TMsgDispatcher_inl.inl"
#include "ExpHelper.h"
#include "CSlowSigner.inl"
#include "CTimeSystemClient.h"

template<typename CmdClass>
void CConnClient::OnServerCommand(const CmdClass* pCmd,void* pParam)
{
	OnServerCommand(pCmd);
}

template<typename CmdClass>
void CConnClient::RegisterServerCommand()
{
	CConnClient::SetHandler<CmdClass>(&CConnClient::OnServerCommand);
}

template<typename CmdClass>
void CConnClient::OnSceneCommand(const CmdClass* pCmd,void* pParam)
{
	(CCoreSceneClient::MainScene()->OnServerCommand)(pCmd);
}

template<typename CmdClass>
void CConnClient::RegisterSceneCommand()
{
	CConnClient::SetHandler<CmdClass>(&CConnClient::OnSceneCommand<CmdClass>);
}


template<typename CmdClass>
void CConnClient::OnSceneMgrCommand(const CmdClass* pCmd,void* pParam)
{
	(CCoreSceneMgrClient::Inst()->OnServerCommand)(pCmd);
}

template<typename CmdClass>
void CConnClient::RegisterSceneMgrCommand()
{
	CConnClient::SetHandler<CmdClass>(&CConnClient::OnSceneMgrCommand<CmdClass>);
}

template<typename CmdClass,typename ObjectClass,void (ObjectClass::*pFun)(const CmdClass*)>
void CConnClient::OnObjectCommand(CmdClass* pCmd,void* pParam)
{
	if(!CCoreSceneClient::Inst())
	{
		GenErr("场景都没,哪来的对象");
		return;
	}
	typename ObjectClass* pObj = static_cast<ObjectClass*>(CCoreObjectFollower::GetObjectFollower(pCmd->uqbGlobalID));
	if(pObj == NULL)
	{
		ostringstream strm;
		strm << pCmd->uqbGlobalID; 
		GenErr("引擎对像不存在, 之前很可能在创建该对象时发生了异常 ", strm.str());
		return;
	}
	(pObj->*pFun)(pCmd);
}

template<typename CmdClass,typename ObjectClass,void (ObjectClass::*pFun)(const CmdClass*)>
void CConnClient::RegisterObjectCommand()
{
	CConnClient::SetHandler<CmdClass>(&CConnClient::OnObjectCommand<const CmdClass,ObjectClass,pFun>);
}

template<typename CmdClass>
void CConnClient::RegisterFollowerFastDelayCommand()
{
	RegisterObjectCommand<CmdClass,CCoreObjectFollower,&CCoreObjectFollower::OnFastServerCommand<const CmdClass> >();
}

template<typename CmdClass>
void CConnClient::RegisterFollowerSlowDelayCommand()
{
	RegisterObjectCommand<CmdClass,CCoreObjectFollower,&CCoreObjectFollower::OnSlowServerCommand<const CmdClass> >();
}

template<typename CmdClass>
void CConnClient::RegisterFollowerCommand()
{
	RegisterObjectCommand<CmdClass,CCoreObjectFollower,&CCoreObjectFollower::OnServerCommand>();
}


template<typename CmdClass>
void CConnClient::RegisterDirectorCommand()
{
	RegisterObjectCommand<CmdClass,CCoreObjectDirector,&CCoreObjectDirector::OnServerCommand>();
}



template<typename CmdType>
void CConnClient::SlowSignOnData(const void* pData,size_t stDataSize,CmdType* pCmd)
{
	return m_pSlowSigner->SlowSignOnData( pData,stDataSize,pCmd );
}

template<typename CmdType>
void CConnClient::SlowSignOnMediatorCmd(CmdType* pCmd)
{
	return m_pSlowSigner->SlowSignOnMediatorCmd( pCmd );
}

template<typename CmdClass>
void CConnClient::SendCoreCmd(CmdClass* pCmd)
{
	pCmd->uobClientKnownServerFrameTime = GetLastSyncedServerFrameTime();
	pCmd->uobClientTime = CTimeSystemClient::Inst()->GetFrameTime();
	TConnection<CTraitsClient>::SendCoreCmd(reinterpret_cast<const void*>(pCmd),sizeof(CmdClass));

	if(pCmd->uobClientKnownServerFrameTime > 100000000000000000)
	{
		ostringstream strm;
		strm << typeid(CmdClass).name() << pCmd->uobClientKnownServerFrameTime;
		LogErr("ClientKnownServerFrameTime Err", strm.str());
	}
}
