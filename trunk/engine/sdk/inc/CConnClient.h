#pragma once

#include "TMsgDispatcher.h"
#include "TConnection.h"
#include "CTick.h"
#include "CTraitsClient.h"
#include "TClientMsgRegister.h"
#include "IConnMgrClientHandler.h"
#include "GuidTypes.h"


namespace sqr
{
	class CCoreObjectFollower;
	class CCoreSceneClient;
	class CConnMgrClient;
	class CDistortedTimeObjClient;
	class CFolMovement;
	class CFolObjFollowState;

	class CServerTimeGuesser;

	class CGas2GacGC_Ping_Client;
	class CGas2GacGC_Small_Shell_Message;
	class CGas2GacGC_Big_Shell_Message;
	class CGas2GacOC_Set_Main_Scene;
	class CGas2GacGC_Middle_Shell_Message;
	class CGas2GacGC_Tell_Server_Time;
	class CGas2GacGC_Tell_Error;
	class CGas2GacGC_Tell_Current_Version;
	class CGas2GacGC_Tell_OwnerInfo;
	class CGas2GacGC_Tell_Guids;
	class CLogOwner;

	class CSlowSigner;

	class CConnClient
		:public TConnection<CTraitsClient>
		,public TClientMsgRegister<CTraitsClient>
		,public CTick
	{
		friend class TClientMsgRegister<CTraitsClient>;
		friend class CConnMgrClient;
		friend class CCoreSceneMgrClient;
		friend class CDistortedTimeObjClient;
		friend class CCoreObjectFollower;
		friend class CCoreObjectDirector;
		friend class CFolMovement;
		friend class CFolObjFollowState;

		template<typename Traits> friend class TConnectionMgr;
	public:
		CConnClient();
		~CConnClient(void);

		void SetConnValue(const char* szKey, const char* szValue);
		bool Send(const void* pData,uint32 uSize);

		//static void RegisterMsgHandler();
		static CConnClient*& Inst();

		uint32 GetLatency() const;

		//return false表示连接没有建立
		bool IsConnected()const;
		void ShutDown(const char* szMsg);
		void ShutDown();

		uint64 GetServerFrameTime()const;

		void SendErrMsg(const char* szError);

		void StartRecvTrafficStat();
		void LogMsgRecvTrafficForCpp(const char* szProtlName, uint16 uMsgID, uint32 uMsgLen);
		void LogMsgRecvTrafficForScript(uint16 uMsgID, uint32 uMsgLen);
		void LogMsgRecvTrafficForVariantDecode(const char* sName, uint32 uMsgLen);
		void CloseRecvTrafficStat();

		uint64 GetEstimatedServerTime()const;
		uint64 GetEstimatedServerFrameTime()const;

		const char* GetLocalPhysicalAddress();

		CLogOwner* GetLogOwner();

	private:
		uint64 GetLastSyncedServerFrameTime()const;

		uint64		m_uLastSyncedServerFrameTime;
		bool		m_bGlobalTimeSynced;	//标记已经收到全局时间同步
		int64		m_nClientServerTimeDiff;	//client和server的绝对时间的秒数差异
		
		template<typename CmdType>
		void SendCoreCmd(CmdType* pCmd);

		typedef TConnection<CTraitsClient>	ParentConn_t;

		template<typename CmdType>
		void SlowSignOnData(const void* pData,size_t stDataSize,CmdType* pCmd);

		template<typename CmdType>
		void SlowSignOnMediatorCmd(CmdType* pCmd);		
		void LogOnOffLineMsg(const char*);

		CSlowSigner*	m_pSlowSigner;

		//通知服务端是否为测试客户端
		void TellServerBeTesting();

		//通知服务端客户端的guid信息
		void TellServerClientGuid();

		void UploadErrLog(const char* szErrComm, const char* szErrInfo);
		void OnMainSceneSet();

		void		OnTick(void);
		uint64		m_uLastPingTime;
		uint32		m_uCurrentDelay;
		bool		m_bWaitingDisconnect;
		bool		m_bShutting;
		bool		m_bRecvTrafficStatStarted;

		struct RecvTrafficData { RecvTrafficData():m_uCount(0),m_uTraffic(0){} string m_strProtlName; uint32 m_uCount; uint32 m_uTraffic; };
		typedef vector<RecvTrafficData> VecRecvTrafficData;
		VecRecvTrafficData	m_vecRecvTrafficDataForCpp;
		VecRecvTrafficData	m_vecRecvTrafficDataForScript;
		typedef map<string, RecvTrafficData, less<string>, TConnAllocator<pair<string, RecvTrafficData> > > MapVariantDecodeTimes;
		MapVariantDecodeTimes m_mapRecvTrafficVariantData;


		CServerTimeGuesser*	m_EstimatedServerTime;


		static bool MoreTraffic(const RecvTrafficData& left, const RecvTrafficData& right);
		void GenRecvTrafficStatFile();

		void OnConnected();
		void OnConnectFailed(EPipeConnFailedReason eReason);

		void DoShutDownAndCallbackDerived();
		
		typedef TConnection<CTraitsClient>	ParentConn_t;
		friend class TConnection<CTraitsClient>;

		void RegDoShutDownTickDerived();

		//连接自己处理的消息
		template<typename CmdClass>static void RegisterServerCommand();
		template<typename CmdClass>void OnServerCommand(const CmdClass* pCmd,void* pParam);


		//场景处理的消息
		template<typename CmdClass>void OnSceneCommand(const CmdClass* pCmd,void* pParam);
		template<typename CmdClass>static void RegisterSceneCommand();



		//场景管理器处理的消息
		template<typename CmdClass>void OnSceneMgrCommand(const CmdClass* pCmd,void* pParam);
		template<typename CmdClass>static void RegisterSceneMgrCommand();



		//对象处理的消息
		template<typename CmdClass,typename ObjectClass,void (ObjectClass::*pFun)(const CmdClass*)>
		void OnObjectCommand(CmdClass* pCmd,void* pParam);
		template<typename CmdClass,typename ObjectClass,void (ObjectClass::*pFun)(const CmdClass*)>
		static void RegisterObjectCommand();
		template<typename CmdClass>static void RegisterFollowerFastDelayCommand();
		template<typename CmdClass>static void RegisterFollowerSlowDelayCommand();
		template<typename CmdClass>static void RegisterFollowerCommand();
		template<typename CmdClass>static void RegisterDirectorCommand();



		// ping值计算相关
		void OnServerCommand(const CGas2GacGC_Ping_Client* pCmd);
		void OnServerCommand(const CGas2GacGC_Small_Shell_Message* pCmd);
		void OnServerCommand(const CGas2GacGC_Middle_Shell_Message* pCmd);
		void OnServerCommand(const CGas2GacGC_Big_Shell_Message* pCmd);
		void OnServerCommand(const CGas2GacGC_Tell_Guids* pCmd);
		void OnServerCommand(const CGas2GacGC_Tell_Server_Time* pCmd);
		void OnServerCommand(const CGas2GacOC_Set_Main_Scene* pCmd);
		void OnServerCommand(const CGas2GacGC_Tell_Error* pCmd);
		void OnServerCommand(const CGas2GacGC_Tell_Current_Version* pCmd);
		void OnServerCommand(const CGas2GacGC_Tell_OwnerInfo* pCmd);
	};

}
