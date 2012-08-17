#pragma once
#include "CSyncCoreObjectDictator.h"
#include "TCoreObjAllocator.h"

namespace sqr
{
	class CPixelPath;
	class CCoreObjectMediator;

	class CSynConnServer;
	class ISyncCoreObjectMediatorHandler;
	class CMedObjFollowState;
	class CCoreObjTracer;
	
	class CGac2GasOC_Move_Mediator_To;
	class CGac2GasOC_Tell_Mediator_Reached;
	class CGac2GasOC_Tell_Mediator_Stepped;
	class CGac2GasOC_Tell_Mediator_Stopped;
	class CGac2GasOC_Director_Is_Activated;
	class CGac2GasOC_Change_Mediator_Move_Target;
	class CGac2GasOC_Change_Mediator_Move_Speed;
	class CGac2GasOC_Tell_Mediator_SetPosition;
	class CGac2GasOC_Tell_Mediator_Max_Speed_Set;

	class CSyncCoreObjectMediator
		:public CSyncCoreObjectDictator
	{
	public:
		typedef ISyncCoreObjectMediatorHandler	Handler_t;
		typedef map<uint32, uint32, less<uint32>, TCoreObjAllocator<pair<uint32, uint32> > > MapSyncObj_t;

		//这个函数是异步调用，返回false表示上一个异步请求还未完成
		bool SetConnection(CSynConnServer* pConn);
		CSynConnServer* GetConnection(void) const;

		//fSpeed: grid per second
		void SetDirectorMaxSpeed(float fDirectorMaxSpeed);
		float GetDirectorMaxSpeed()const;
		float GetFollowingSpeed()const;

		void IntEnableFollowerDelay(bool bEnable);

		ISyncCoreObjectMediatorHandler* GetHandler()const;

		EServerObjectType GetType()const;

		bool IsMoving()const;
		EMoveToResult MoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist,float fMovedDist, CPixelPath* pPixelPath);
		ESetSpeedResult SetSpeed(float fSpeed);
		EStopMovingResult StopMoving();
		ESetPosResult SetPixelPos(const CFPos& PixelPos);

		static void SetMoveSpeedCheckArg(uint32 uClientTimeStampErrorThreshold, float fMaxStepCommandRate);
		uint32 GetEnterAOISessionID();

	protected:
		friend class CSyncCoreSceneServer;
		CSyncCoreObjectMediator(uint32 uObjID,CSyncCoreSceneServer* pScene,const CFPos& PixelPos,uint32 uVarDefID, float fDirectorMaxSpeed);
		~CSyncCoreObjectMediator(void);

		//--------以下内容为了实现CSyncCoreObjectServer的虚函数----------
		void OnMoveSteppedDerived(const CFPos&);
		void OnMoveBeganDerived(const CMoveBeganDerivedArgs& Args);
		void OnEndMoveDerived();
		void OnMoveStoppedDerived(float fStopDist);
		void OnMovePathChangedDerived(const CMovePathChangedDerivedArgs& Args);
		void OnMoveSpeedChangedDerived(const CMoveSpeedChangedDerivedArgs& Args);
		void OnPixelPosSetDerived(bool bSuperPosition);
		void OnMsgToVieweeHandled(const void* pContext);
		//********以上内容为了实现CSyncCoreObjectServer的虚函数

		void PostMsgToSyncer(const void* pContext);
		void OnMsgFromSyncer(const void* pContext);
		void OnMsgToSyncerHandled(const void* pContext);

		virtual void GetRealCurPos(CFPos& PixelPos);

		static uint32& GetMedObjNum();
		
		void OnSyncBindingRelation(uint32 uParentID, bool bRet);

		void OnObjectEnterViewAoi(uint32 uCoreObjID);
		void OnObjectLeaveViewAoi(uint32 uCoreObjID);
		void OnObjectEnterSyncAoi(uint32 uCoreObjID);
		void OnObjectLeaveSyncAoi(uint32 uCoreObjID);

	private:
		friend class CMedObjFollowState;
		friend class CCoreObjTracer;
		friend class CSynConnServer;
		friend class CCoreObjectMediator;
		friend class CSyncCoreObjectDictator;
		template<typename ObjectType,typename HandlerType>
		friend class TAoiSyncerHandler;
		
		friend class CAoiViewObjHandler;
		friend class CAoiSyncerObjHandler;
		friend class CAoiSynceeObjHandler;

		friend class CAoiDisbindConnCmd;
		friend class CAoiDecBlockCountCmd;

		//检测客户端发送Step的频率
		bool CheckStepCommandFrequence(uint32& uStepNumInOneCheck);
		void AdjustMaxStepNumInOneCheck(float fSpeed);
		uint32 CalcMaxStepNumInOneCheck(float fSpeed);

		void IntSetPositionByClient( const CFPos& PixelPos);
		void IntSetConnection(CSynConnServer* pConn);
		//网络断开
		void OnConnectionShutDown();
		void DisbindConnection();
		void AddConnBlockCount();
		
		void CreateDirector();

		//对象进行主被动状态切换的时候需要用到的变量。
		bool IsBlockingDirCmd()const;
		bool IsBlockingAoiCmd()const;
		
		//void SetActive(bool bTrue);
		bool IsActive()const;
		bool DirectorIsActive()const;

		bool SetDisbindConnState();
		bool StopFollowing();

		uint32					m_uDirBlockCount;
		uint32					m_uAoiBlockCount;

		float					m_fDirectorMaxSpeed;

		bool					m_bDisbindingConn:1;


		//与这个Mediator绑定的连接对象
		CSynConnServer*			m_pConn;
		CMedObjFollowState*		m_pFolState;

		
		void UpdateCheatScore(int32 uCheatScoreModifier);

		void SetClientMainScene();

		bool IsFollowing()const;

		void BroadCastForViewAoi(const void* pData,uint32 uSize)const;
		void BroadCastForSyncAoi(const void* pData,uint32 uSize)const;

		void SendCreateObjCmdForConn(CSynConnServer* pConn);

		//下列函数专供连接对象调用，都是客户端发送上来的命令
		void OnClientCommand(const CGac2GasOC_Move_Mediator_To* pCmd);
		void OnClientCommand(const CGac2GasOC_Tell_Mediator_Reached* pCmd);
		void OnClientCommand(const CGac2GasOC_Tell_Mediator_Stepped* pCmd);
		void OnClientCommand(const CGac2GasOC_Tell_Mediator_Stopped* pCmd);
		void OnClientCommand(const CGac2GasOC_Change_Mediator_Move_Target* pCmd);
		void OnClientCommand(const CGac2GasOC_Change_Mediator_Move_Speed* pCmd);
		void OnClientCommand(const CGac2GasOC_Director_Is_Activated* pCmd);
		void OnClientCommand(const CGac2GasOC_Tell_Mediator_SetPosition* pCmd);
		void OnClientCommand(const CGac2GasOC_Tell_Mediator_Max_Speed_Set* pCmd);

		void SyncViewVariantHolder(uint32 uTargetGlobalID, bool bSyncStaticView, bool bSyncAgileView, uint32 uSessionID);
		void SyncSyncVariantHolder(uint32 uTargetGlobalID, bool bSyncStaticSync, bool bSyncAgileSync, uint32 uSessionID);

		template <typename CmdType>
		void OnClientCommandWithTimeStamp(const CmdType* pCmd);
		//上面的函数专供连接对象调用，都是客户端发送上来的命令

		template<typename CmdType>
		void ViewMulticastSendCmd(const CmdType* pCmd)const;

		void SetEyeSight( float fEyeSight, uint32 uDimension = 0 );
		float GetEyeSight(uint32 uDimension = 0)const;

		void SyncBarrier()const;
		//拿当前点的上个waypoint和目标点的下个waypoint做直线寻路进行检测
		bool VerifySetPosCmd(float fMovedDist, float fMovingDist, const CFPos& posDest);

		void IntBeginDestroyObject();
		void IntBeginTransferObject(CSyncCoreSceneServer* pScene , const CFPos& PixelPos);
		void IntEndTransferObject();

		float m_fZeroDimEyeSight;

		uint32	m_uMaxStepNumInOneCheck;//每两次check之间最大允许的step次数
		uint32	m_uStepNumInOneCheck;	//每两次check之间累计的Step次数
		
		void CorrectDirectorMovingProcess();
		void NotifyDirToSetMaxSpeed();
		void ResetDirKnownMaxSpeed();
		void CreateDirMaxSpeedChangeSession(uint32 uSessionID, float fDirectorMaxSpeed);

		struct DirMaxSpeedChangeSession
		{
			uint32	m_uID;
			uint64	m_uServerFrameTime;
			float	m_fSpeed;
		};

		typedef deque<DirMaxSpeedChangeSession, TCoreObjAllocator<DirMaxSpeedChangeSession> > QueDirMaxSpeedChangeSession_t;
		QueDirMaxSpeedChangeSession_t	m_queDirMaxSpeed;
		uint32						m_uCurDirMaxSpeedChangeSessionID;
		float						m_fDirKnownMaxSpeed;

		
#ifdef DIRECTOR_SEND_MOVED_DIST
		bool VerifyDirMovedDist(float fDirMovedDist);
#else
		bool CalcRealMovedDist(float& fRealMovedDist);
#endif

		void	SetMovementCmdCheckPoint(float fDirMovedDist);
		float						m_fLastDirMovedDist;
		uint64						m_uLastConnTime;

		MapSyncObj_t				m_mapViewObj;
		MapSyncObj_t				m_mapSyncObj;
		uint32						m_uEnterAoiSessionID;
	};
}
