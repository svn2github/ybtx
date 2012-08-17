#pragma once
#include "FindPathDefs.h"
#include "CPos.h"
#include "TObjMoveState.h"
#include "CTraitsClient.h"
#include "TimeHelper.h"
#include "CLogCoreObject.h"

#ifdef USE_RENDER_SLOT
#include "CRenderSlot.h"
#endif


namespace sqr
{
	/*
	CFolMovement类负责存放所有移动请求。

	对象如果正处在路径上，则使用路径进度平滑办法。
	对象的高度不是立即修改的，寻路行为直接用对象的最新高度来进行。

	去除服务端对象向客户端发送的step命令,换成以下两种方法解决相应问题. 
	1.每个客户端的所有fol对象自己管理和服务端时间差的波动,从而消除因此带来的和服务端之间的位移误差,服务端向客户端同步时间的频率由原来的5秒提高到1秒. 
	2.服务端Med对象增加对Dir对象发送step消息的检测,如果发现dir超时发送step,Med则通知fol停止移动,直到dir恢复发送step消息.
	*/
	class CFolMovement;
	class CCoreObjectFollower;
	class CGas2GacOC_Suspend_Follower_Moving;

#ifdef LOG_COREOBJ_MOVE
	class CPrintMovedDist;
#endif

	enum EObjectHeight;

	class CFolObjFollowState
#ifdef USE_RENDER_SLOT
		:public CRenderSlot
#else
		:public CTick
#endif
	{
		friend class CCoreObjectFollower;
	public:
		CFolObjFollowState(CFolMovement* pMm,float fSpeed,CCoreObjectFollower* pObj);
		~CFolObjFollowState(void);
		
		void SetIdealSpeed(float fSpeed);	//理想的速度，未经过速度适配的原始速度

		float GetRealSpeed()const;
		void ModifySpeed(float fSpeedModifier);

		CFolMovement*			m_pCurMovement;			//这个路径不可能为空
		list<CFolMovement*>		m_lstPendingMovement;

		uint64					m_tcpNotifySpeedChanged;	//用于控制OnSpeedChanged频率不要过高

		CFolMovement* GetLastMovement()const;

		//fRealMovedDist,从TObjMoveState获得的对象当前移动状态的MovedDist
		float GetMovedDist(CObjMoveStateClient* pMovState)const;
		//float GetTotalMovedDistError()const;

		float GetTotalMovedDistError(CObjMoveStateClient* pMovState, uint32 uDelay)const;
		float GetEasyMovedDistError(CObjMoveStateClient* pMovState, uint32 uDelay)const;

		float GetRemoteLastPathMovedDist()const;
		
		//收到的服务器当前位置
		float GetRemoteTotalMovedDist()const;

		//预测的服务器当前位置
		float GetEasyRemoteTotalMovedDist()const;

		CFPos GetServerObjectPixelPos()const;

		bool RemoteObjectReachedCurPath()const;
		bool RemoteObjectReached()const;

		void OnStepped(float fLastPathMovedDist);

		float CalIdealDistByTime(int32 nTime)const;

#ifdef LOG_COREOBJ_MOVE
		CFPos				m_ServerReachedPos;
		float				m_fServerReachedDist;
		float				m_fReachedDist;
#endif

	private:
		//void SuspendMoving(const CGas2GacOC_Suspend_Follower_Moving* pCmd);
		//void ResumeMoving(uint32 uSuspendedTime);
#ifdef USE_RENDER_SLOT
		void OnUpdate(uint64 uLastFrameUsedTime);
		void AdjustSpeed(uint32 uMoveInterval);
#else
		void OnTick();	//移动速度匹配
		void AdjustSpeed();
#endif
		void SetSpeed(float fSpeed);
		float GetSpeed()const;

		//服务端对象在最后一条路径上的移动进度。由这个可以确定服务端对象的
		//当前位置，作为平滑的依据。这个数值小于0说明服务端对象已经完成了最后一个路径。
		float				m_fRemoteLastPathMovedDist;
		float				m_fSpeedModifier;		//速度修正量，用于根据行走进度的差异使客户端行走进度尽可能贴近服务端行走进度。
		uint64				m_tcpStep;

		CCoreObjectFollower*	m_pObj;

#ifdef LOG_COREOBJ_MOVE
		friend class CPrintMovedDist;
		bool		m_bNormalSpeed;
		CPrintMovedDist*		m_pPrintTick;
#endif
	};

}
