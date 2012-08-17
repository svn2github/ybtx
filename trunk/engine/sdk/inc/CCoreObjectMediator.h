#pragma once
#include "CCoreObjectDictator.h"


namespace sqr
{
	class CCoreSceneServer;
	class CSyncCoreObjectMediator;
	class CConnServer;
	class ICoreObjectMediatorHandler;

	class CCoreObjOnTransferedResult;
	class CCoreObjOnMoveBeganResult;
	class CCoreObjOnMoveEndedResult;

	class CCoreObjectMediator
		:public CCoreObjectDictator
	{
		friend class CConnServer;
		friend class CCoreSceneServer;
		friend class CCoreObjOnTransferedResult;
		friend class CCoreObjOnMoveBeganResult;
		friend class CCoreObjOnMoveEndedResult;
	public:
		//这个函数是异步调用，返回false表示上一个异步请求还未完成
		bool SetConnection(CConnServer* pConn);
		CConnServer* GetConnection(void) const;

		static float GetDefaultDirectorMaxSpeed();
		void SetDirectorMaxSpeed(float fDirectorMaxSpeed);
		float GetDirectorMaxSpeed()const;
		
		ICoreObjectMediatorHandler* GetHandler()const;

		EServerObjectType GetType()const;
		virtual CCoreObjectMediator* CastToObjMediator();
		virtual CCoreObjectDictator* CastToObjDictator();

		EMoveToResult MoveTo(const CFPos& PixelPosDes,uint16 uPixelSpeed,
			EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist);
		ESetSpeedResult SetSpeed(float fSpeed);
		EStopMovingResult StopMoving();
		virtual void Trace(CCoreObjectServer* pTarget, float fSpeed
			, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist);
		virtual void StopTracing();

		void SetSyncEyeSize(float fEyeSight);
		float GetSyncEyeSize()const;

		void SetSyncKeenness(float Keenness);
		float GetSyncKeenness()const;

		ESetPosResult SetPixelPos(const CFPos& PixelPos);

		static void SetMoveSpeedCheckArg(uint32 uClientTimeStampErrorThreshold, float fMaxStepCommandRate);
	private:
		CCoreObjectMediator(ICoreObjectMediatorHandler* pHandler, CCoreSceneServer* pScene, uint32 uVarDefID, float fDirectorMaxSpeed);
		~CCoreObjectMediator(void);

		void BroadCastForViewAoi(const void* pData,uint32 uSize)const;
		void BroadCastForSyncAoi(const void* pData,uint32 uSize)const;

		//void SetActive(bool bActive);
		bool IsActive()const;

		CConnServer*		m_pConn;
		float				m_fDirectorMaxSpeed;
		float				m_fZeroDimEyeSight;

		void SyncBarrier()const;
	};
}
