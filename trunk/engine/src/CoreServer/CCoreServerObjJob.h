#pragma once
#include "CCoreServerJob.h"
#include "CPos.h"


namespace sqr
{
	class CSyncCoreObjectServer;
	class CPixelPath;

	class CCoreServerObjJob
		:public CCoreServerJob
	{
	protected:
		CCoreServerObjJob(uint32 uObjID);

		virtual ~CCoreServerObjJob(void);

		CSyncCoreObjectServer* GetCoreObj()const;
	protected:
		uint32 m_uObjID;
	};

	class CCoreObjMoveToJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjMoveToJob(uint32 uObjID, float fSpeed, uint32 uFindPathType, uint32 uBarrierType, float fReachDist, float fMovedDist, CPixelPath* pPixelPath);
		
		~CCoreObjMoveToJob();
	private:
		float	m_fSpeed;
		uint32	m_uFindPathType;
		uint32	m_uBarrierType;
		float	m_fReachDist;
		CPixelPath* m_pPixelPath;
		float	m_fMovedDist;
	};

	class CCoreObjMoveToLimitJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjMoveToLimitJob(uint32 uObjID, float fSpeed, uint32 uFindPathType, uint32 uBarrierType, float fReachDist, float fMovedDist, CPixelPath* pPixelPath, int32 nMaxStep);
		
		~CCoreObjMoveToLimitJob();
	private:
		float	m_fSpeed;
		uint32	m_uFindPathType;
		uint32	m_uBarrierType;
		float	m_fReachDist;
		int32	m_iMaxStep;
		CPixelPath* m_pPixelPath;
		float	m_fMovedDist;
	};

	class CCoreObjMoveToInPathJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjMoveToInPathJob(uint32 uObjID, CPixelPath* pPixelPath, float fSpeed, uint32 uFindPathType, uint32 uBarrierType, float fReachDist);
		
		~CCoreObjMoveToInPathJob();
	private:
		CFPos	m_PixelPosDes;
		CPixelPath* m_pPixelPath;
		float	m_fSpeed;
		uint32	m_uFindPathType;
		uint32	m_uBarrierType;
		float	m_fReachDist;
	};

	class CCoreObjStopMovingJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjStopMovingJob(uint32 uObjID);
		
		~CCoreObjStopMovingJob();
	};

	class CCoreObjWatchJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjWatchJob(uint32 uObjID, uint32 uHandlerID, uint32 uTargetObjID, float fPixelDistThreshold);
		
		~CCoreObjWatchJob();
	private:
		uint32 m_uHandlerID;
		uint32 m_uTargetObjID;
		float m_fPixelDistThreshold;
	};

	class CCoreObjStopWatchingJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjStopWatchingJob(uint32 uObjID, uint32 uHandlerID);
		
		~CCoreObjStopWatchingJob();
	private:
		uint32 m_uHandlerID;
	};
	
	class CCoreObjTraceJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjTraceJob(uint32 uObjID, uint32 uTargetID, float fSpeed, uint32 uFindPathType, uint32 uBarrierType, float fReachDist);
		
		~CCoreObjTraceJob();
	private:
		uint32 m_uTargetID;
		float m_fSpeed;
		uint32 m_uFindPathType;
		uint32 m_uBarrierType;
		float m_fReachDist;
	};

	class CCoreObjStopTracingJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjStopTracingJob(uint32 uObjID);
		
		~CCoreObjStopTracingJob();
	private:
	};

	class CCoreObjSetSpeedJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetSpeedJob(uint32 uObjID, float fSpeed);
		
		~CCoreObjSetSpeedJob();
	private:
		float m_fSpeed;
	};

	class CCoreObjSetSizeJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetSizeJob(uint32 uObjID, float fSize, uint32 uDimension);
		
		~CCoreObjSetSizeJob();
	private:
		float m_fSize;
		uint32 m_uDimension;
	};

	class CCoreObjSetEyeSightJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetEyeSightJob(uint32 uObjID, float fEyeSight, uint32 uDimension);
		
		~CCoreObjSetEyeSightJob();
	private:
		float m_fEyeSight;
		uint32 m_uDimension;
	};

	class CCoreObjSetStealthJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetStealthJob(uint32 uObjID, float fStealth, uint32 uDimension);
		
		~CCoreObjSetStealthJob();
	private:
		float m_fStealth;
		uint32 m_uDimension;
	};

	class CCoreObjSetKeennessJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetKeennessJob(uint32 uObjID, float fKeenness, uint32 uDimension);
		
		~CCoreObjSetKeennessJob();
	private:
		float m_fKeenness;
		uint32 m_uDimension;
	};

	class CCoreObjSetViewGroupJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetViewGroupJob(uint32 uObjID, uint32 uGroupID, uint32 uDimension);
		
		~CCoreObjSetViewGroupJob();
	private:
		uint32 m_uGroupID;
		uint32 m_uDimension;
	};

	class CCoreObjPostAoiMsgToSelfJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjPostAoiMsgToSelfJob(uint32 uObjID, const void* pContext);
		
		~CCoreObjPostAoiMsgToSelfJob();
	private:
		const void* m_pContext;
	};

	class CCoreObjPostEventJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjPostEventJob(uint32 uObjID, const void* pContext);
		
		~CCoreObjPostEventJob();
	private:
		const void* m_pContext;
	};

	class CCoreObjSetPixelPosJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetPixelPosJob(uint32 uObjID, const CFPos& PixelPos);
		
		~CCoreObjSetPixelPosJob();
	private:
		CFPos m_PixelPos;
	};

	class CCoreObjSetCoreSizeJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetCoreSizeJob(uint32 uObjID, float fSize);
		
		~CCoreObjSetCoreSizeJob();
	private:
		float m_fSize;
	};

	class CCoreObjBroadCastForViewAoiExtraConnJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjBroadCastForViewAoiExtraConnJob(uint32 uObjID, const void* pData, uint32 uSize, uint32 uExtraConn);
		
		~CCoreObjBroadCastForViewAoiExtraConnJob();
	private:
		void* m_pData;
		uint32 m_uSize;
		uint32 m_uExtraConn;
	};

	class CCoreObjBroadCastForSyncAoiExtraConnJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjBroadCastForSyncAoiExtraConnJob(uint32 uObjID, const void* pData, uint32 uSize, uint32 uExtraConn);
		
		~CCoreObjBroadCastForSyncAoiExtraConnJob();
	private:
		void* m_pData;
		uint32 m_uSize;
		uint32 m_uExtraConn;
	};

	class CCoreObjEnableFollowerDelayJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjEnableFollowerDelayJob(uint32 uObjID, bool bEnableFollowerDelay);
		
		~CCoreObjEnableFollowerDelayJob();
	private:
		bool m_bEnableFollowerDelay;
	};

	class CCoreObjSetBarrierJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetBarrierJob(uint32 uObjID, float fBarrierSize, uint32 uBarrierType);
		
		~CCoreObjSetBarrierJob();
	private:
		float m_fBarrierSize;
		uint32 m_uBarrierType;
	};

	class CCoreObjSyncBarrierExtraConnJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSyncBarrierExtraConnJob(uint32 uObjID, uint32 uExtraConn);
		
		~CCoreObjSyncBarrierExtraConnJob();
	private:
		uint32 m_uExtraConn;
	};

	class CCoreObjSetConnectionJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetConnectionJob(uint32 uObjID, uint32 uConn);
		
		~CCoreObjSetConnectionJob();
	private:
		uint32 m_uConn;
	};

	class CCoreObjSetDirectorMaxSpeedJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjSetDirectorMaxSpeedJob(uint32 uObjID, float fDirectorMaxSpeed);
		
		~CCoreObjSetDirectorMaxSpeedJob();
	private:
		float m_fDirectorMaxSpeed;
	};

	class CCoreObjCreateQuadObjJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjCreateQuadObjJob(uint32 uObjID, uint32 uSlot, const CFPos& PixelPos);
		
		~CCoreObjCreateQuadObjJob();
	private:
		uint32 m_uSlot;
		CFPos m_PixelPos;
	};

	class CCoreObjDestoryObjJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjDestoryObjJob(uint32 uObjID);
		
		~CCoreObjDestoryObjJob();
	};

	class CCoreObjTransferObjJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjTransferObjJob(uint32 uObjID, uint32 uTargetSceneID, const CFPos& PixelPos);
		
		~CCoreObjTransferObjJob();
	private:
		uint32						m_uObjID;
		uint32						m_uTargetSceneID;
		CFPos						m_pos;
	};

	class CCoreSetMoveSpeedCheckArgJob
		:public CCoreServerJob
	{
	public:
		CCoreSetMoveSpeedCheckArgJob(uint32 uClientTimeStampErrorThreshold, float fMaxStepCommandRate);
		
		~CCoreSetMoveSpeedCheckArgJob();
	private:
		uint32				m_uClientTimeStampErrorThreshold;
		float				m_fMaxStepCommandRate;
	};

	class CCoreObjBindToJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjBindToJob(uint32 uObjID, uint32 m_uParentObjID);

		~CCoreObjBindToJob();
	private:
		uint32	m_uParentObjID;
	};

	class CCoreObjDisbindFromParentJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjDisbindFromParentJob(uint32 uObjID);

		~CCoreObjDisbindFromParentJob();
	private:
	};

	class CCoreObjDisbindToChildJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjDisbindToChildJob(uint32 uObjID, uint32 m_uChildObjID);

		~CCoreObjDisbindToChildJob();
	private:
		uint32	m_uChildObjID;
	};

	class CCoreObjDisbindToAllChildJob
		:public CCoreServerObjJob
	{
	public:
		CCoreObjDisbindToAllChildJob(uint32 uObjID);

		~CCoreObjDisbindToAllChildJob();
	};

	class CAsynCoreObjHaltedJob
		: public CCoreServerObjJob
	{
	public:
		CAsynCoreObjHaltedJob(uint32 uObjID);
		~CAsynCoreObjHaltedJob();
	};
}



