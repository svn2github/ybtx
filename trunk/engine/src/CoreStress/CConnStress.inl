#pragma once
//$id$
//对象系统使用的连接对象,仅负责对象系统相关的网络消息的派发

#include "CConnStress.h"
#include "CCoreObjectStress.h"
#include "CTimeSystemStress.h"

template<typename CmdClass>
void CConnStress::OnServerCommand(const CmdClass* pCmd,void* pParam)
{
	OnServerCommand(pCmd);
	//	return ls_Inst;
}

//template<typename CmdClass>
//void CConnStress::OnServerCommand(const CmdClass* pCmd,void* pParam)
//{
//	OnServerCommand(pCmd);
//}

template<typename CmdClass>
void CConnStress::RegisterServerCommand()
{
	CConnStress::SetHandler<CmdClass>(&CConnStress::OnServerCommand);
}


template<typename CmdClass,typename ObjectClass,void (ObjectClass::*pFun)(const CmdClass*)>
void CConnStress::OnObjectCommand(CmdClass* pCmd,void* pParam)
{
	//(static_cast<ObjectClass*>(CCoreObjectFollower::GetObjectFollower(pCmd->uqbGlobalID))->*pFun)(pCmd);
}

template<typename CmdClass,typename ObjectClass,void (ObjectClass::*pFun)(const CmdClass*)>
void CConnStress::RegisterObjectCommand()
{
	CConnStress::SetHandler<CmdClass>(&CConnStress::OnObjectCommand<const CmdClass,ObjectClass,pFun>);
}

template<typename CmdClass>
void CConnStress::OnSceneCommand(const CmdClass* pCmd,void* pParam)
{
	(CCoreObjectStress::MainObject()->OnServerCommand)(this, pCmd);
}

template<typename CmdClass>
void CConnStress::RegisterSceneCommand()
{
	CConnStress::SetHandler<CmdClass>(&CConnStress::OnSceneCommand<CmdClass>);
}

template<typename Cmd>
void CConnStress::SendStressCmd(Cmd *cmd)
{
	cmd->uobClientKnownServerFrameTime = GetLastSyncedServerFrameTime();
	cmd->uobClientTime = CTimeSystemStress::Inst()->GetFrameTime();
	//cout << "send " << typeid(Cmd).name() << GetLastSyncedServerFrameTime() << endl;
	TConnection<CTraitsStress>::SendCoreCmd(reinterpret_cast<const void*>(cmd),sizeof(Cmd));
}
