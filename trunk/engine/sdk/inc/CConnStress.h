#pragma once

/************************************************************************/
/*
该类主要是进行压力测试时候使用，对于以前的CConnClient，由于只能有一个
连接，所以我们得进行一些相关修改，使其同时能有多个连接连上服务器。
我们还要处理所有的网络协议，但是考虑到性能问题，一些网络协议我们只会
接受不会处理。
*/
/************************************************************************/

#include "TMsgDispatcher.h"
#include "TConnection.h"
#include "CTick.h"
#include "CTraitsStress.h"
#include "TClientMsgRegister.h"

namespace sqr
{
	class CGas2GacGC_Ping_Client;
	class CGas2GacGC_Small_Shell_Message;
	class CGas2GacGC_Big_Shell_Message;
	class CGas2GacOC_Set_Main_Scene;
	class CGas2GacGC_Middle_Shell_Message;
	class CGas2GacGC_Tell_Server_Time;

	class CGas2GacOC_Create_Still_Object;
	class CGas2GacOC_Create_Moving_Object;
	class CGas2GacOC_Destroy_Object;
	class CGas2GacOC_Set_Time_Distorted_Ratio;

	class CGas2GacOC_Set_Director_Active;
	class CGas2GacOC_Set_Dir_Max_Speed_Zero;


	class CGas2GacOC_Set_Object_Pos;
	class CGas2GacOC_Sync_Object_Pos;
	class CGas2GacOC_Enable_Follower_Delay;

	class CGas2GacOC_Move_Follower_To;
	class CGas2GacOC_Change_Follower_Move_Speed;

	class CGas2GacOC_Change_Follower_Move_Target;
	class CGas2GacOC_Tell_Follower_Stepped_BigDist;
	class CGas2GacOC_Tell_Follower_Stopped;
	class CGas2GacOC_Tell_Follower_Reached;
	class CGas2GacOC_Validate_Follower_Pos;
	class CGas2GacGC_Tell_Error;
	class CGas2GacGC_Tell_Current_Version;
	class CGas2GacGC_Tell_OwnerInfo;
	class CGas2GacOC_Set_Main_Scene;
	class CGas2GacGC_Tell_Guids;

	class CSlowSigner;
	class CLogOwner;

	class CConnStress
		: public TConnection<CTraitsStress>
		,public TClientMsgRegister<CTraitsStress>
		,public CTick
	{
		friend class TClientMsgRegister<CTraitsStress>;
		friend class CConnMgrStress;
		template<typename Traits> friend class TConnectionMgr;
	public:
		CConnStress();
		~CConnStress(void);

		bool Send(const void* pData,uint32 uSize);

		//static void RegisterMsgHandler();

		uint32 GetLatency()const;

		//return false表示连接没有建立
		bool IsConnected()const;

		bool GetLocalAddress(CAddress& Address)const;
		bool GetRemoteAddress(CAddress& Address)const;
		void ShutDown();
		void ShutDown(const char* sMsg);

		template<typename Cmd>
		void SendStressCmd(Cmd *cmd);

		uint64 GetServerProcessTime()const;

		void SendErrMsg(const char* szError);
		CLogOwner* GetLogOwner();

	private:
		uint64 GetLastSyncedServerFrameTime()const;
		void SendShellMessage(void);

		CSlowSigner*	m_pSlowSigner;

		void		OnTick(void);
		uint64		m_uLastPingTime;
		uint32		m_uCurrentDelay;
		bool		m_bGlobalTimeSynced;	//标记已经收到全局时间同步
		bool		m_bWaitingDisconnect;
		uint64		m_uLastSyncedServerFrameTime;

		void OnConnected();
		void OnConnectFailed(EPipeConnFailedReason eReason);
		void DoShutDownAndCallbackDerived();

		typedef TConnection<CTraitsStress>	ParentConn_t;
		friend class TConnection<CTraitsStress>;
		void LogOnOffLineMsg(const char*){}

		//连接自己处理的消息
		template<typename CmdClass>static void RegisterServerCommand();
		template<typename CmdClass>void OnServerCommand(const CmdClass* pCmd,void* pParam);


		//压力测试创建director
		template<typename CmdClass>void OnSceneCommand(const CmdClass* pCmd,void* pParam);
		template<typename CmdClass>static void RegisterSceneCommand();

		//对象处理的消息
		template<typename CmdClass,typename ObjectClass,void (ObjectClass::*pFun)(const CmdClass*)>
		void OnObjectCommand(CmdClass* pCmd,void* pParam);
		template<typename CmdClass,typename ObjectClass,void (ObjectClass::*pFun)(const CmdClass*)>
		static void RegisterObjectCommand();



		// ping值计算相关
		template < class CmdType >
		void OnServerCommand(const CmdType* ) {}
		void OnServerCommand(const CGas2GacGC_Ping_Client* pCmd);
		void OnServerCommand(const CGas2GacGC_Small_Shell_Message* pCmd);
		void OnServerCommand(const CGas2GacGC_Middle_Shell_Message* pCmd);
		void OnServerCommand(const CGas2GacGC_Big_Shell_Message* pCmd);
		void OnServerCommand(const CGas2GacGC_Tell_Server_Time* pCmd);
		void OnServerCommand(const CGas2GacGC_Tell_Error* pCmd);
		void OnServerCommand(const CGas2GacOC_Set_Main_Scene* pCmd);
		void OnServerCommand(const CGas2GacGC_Tell_Guids* pCmd);
	};
}
