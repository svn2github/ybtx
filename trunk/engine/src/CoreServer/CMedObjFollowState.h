#pragma once
//$id$
//仲裁对象处于跟随状态时的状态信息

#include "CPos.h"
#include "FindPathDefs.h"
#include "TimeHelper.h"
#include "TCoreObjAllocator.h"
//#include "CDistortedTick.h"

namespace sqr
{
	class CSyncCoreObjectMediator;

	class CGac2GasOC_Move_Mediator_To;
	class CGac2GasOC_Change_Mediator_Move_Target;
	class CGac2GasOC_Change_Mediator_Move_Speed;
	class CGac2GasOC_Tell_Mediator_Stepped;
	class CGac2GasOC_Tell_Mediator_Stopped;

	class CPixelPath;

	class CMedObjFollowState
		:public CCoreObjMallocObject
		//,public CDistortedTick
	{

	public:
		CMedObjFollowState(CSyncCoreObjectMediator* pObject,const CGac2GasOC_Move_Mediator_To* pCmd);
		~CMedObjFollowState();

		void SetNewPath(const CGac2GasOC_Change_Mediator_Move_Target* pCmd);

		const CFPos& GetBeginPixelPos()const;
		const CFPos& GetEndPixelPos()const;
		//const WayPointDeque& GetWayPointDeq()const;

		EFindPathType GetFindPathType()const;
		EBarrierType GetBarrierType()const;

		float GetSpeed()const;
		bool SetSpeed(const CGac2GasOC_Change_Mediator_Move_Speed* pCmd);

		float GetMovedDist()const;
		float GetEndDist()const;

		//真实系统时间进行坐标运算,得出对象目前精准的坐标
		void GetRealCurPos(CFPos& PixelPos);
		float GetRealMovedDist()const;
		float CalcDirMovedDist()const;

		int32 GetCheatScore()const;

		void Step(float fNewMovedDist);

		const CPixelPath* GetPath()const;
		uint16 GetPixelPosOfPath(CFPos& PixelPos,float fDist)const;

	private:
		enum ESpeedCheckThreshold		{ eSCT_Standard = 10 };		//作弊分数上限
		enum ECheatScoreAdjustAmplitude { eCSAA_TickOnce = 1 };		//每次调整作弊分数的幅度
		enum ECheatScoreAdjustFrequency	{ eCSAF_TickOnce = 1000 };	//调整作弊分数的频率

		//void RegistDirStepCheckTick(uint32 uInterval);
		//uint32 EstimateDirStepFreq();
		//void OnTick();
		//void SuspendFollowerMoving();
		//void ResumeFollowerMoving();

		CSyncCoreObjectMediator*	m_pCoreObj;

		float				m_fSpeed;
		float				m_fMovedDist;
		float				m_fEndDist;

		CFPos				m_BeginPixelPos;
		CFPos				m_EndPixelPos;

		EFindPathType		m_eFindPathType;
		EBarrierType		m_eBarrierType;

		uint64				m_uLastStepTimePoint;	//上次step的时间点
		int32				m_iCheatScore;
		uint32				m_uClientTimeStamp;

		CPixelPath*			m_pPath;

		float				m_fBeginDist;
		uint64				m_uThisPathExpiredTime;
		float				m_fDirBeginDist;
		uint64				m_uDirThisPathExpiredTime;

		//bool				m_bDirStepped;
		//uint64				m_uMoveSuspendedTime;


	};

}

