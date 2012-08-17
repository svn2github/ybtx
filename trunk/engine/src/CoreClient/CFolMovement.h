#pragma once
#include "FindPathDefs.h"
#include "CPos.h"


namespace sqr
{

	enum EFolMovementType{eFMT_MoveTo,eFMT_ChangePath};

	class CCoreSceneClient;
	class CPixelPath;
	class CServerTimeGuesser;


	class CFolMovement
	{
	public:
		EFolMovementType	m_eType;

		CPixelPath* ClonePixelPath()const;

		bool IsModified()const;

		const CPixelPath* GetOriginPath()const;

		CFPos GetPathBeginPixelPos()const;		//获得原始路径的起点
		CFPos GetPathEndPixelPos()const;			//获得原始路径的终点

		CFPos GetMovementBeginPixelPos()const;	//获得实际路径的起点	(可能被回归路径或者中途变更路径所修改)
		CFPos GetMovementEndPixelPos()const;		//获得实际路径的终点	(可能被回归路径或者中途变更路径所修改)

		float GetSpeed()const{return m_fSpeed;}
		void SetSpeed(float fSpeed){m_fSpeed=fSpeed;}
		void CutPath(float fChangeDist);
		bool PathIsCut()const;		//路径被切断，也就是说，主动方触发了ChangePath
		bool GetPixelPos(CFPos& PixelPos,float fDist)const;
		
		//起点到终点的移动距离不一定等于GetPathLength(),因为路径是有可能走不完的。
		float GetPathLength()const;

		static CFolMovement* Create(CCoreSceneClient* pScene,const CFPos& PixelPosBegin,const CFPos& PixelPosEnd
			,EFolMovementType eType,float fSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType,float fEndDist, uint64 uServerBaseTime);
		static CFolMovement* Create(CCoreSceneClient* pScene,const int16* pPathData, uint16 uPathDataLen,const CFPos& PixelPosBegin,const CFPos& PixelPosEnd
			,EFolMovementType eType,float fSpeed,float fEndDist, uint64 uServerBaseTime);

		~CFolMovement();
	
		uint64 GetEstimatedServerTime()const;
		uint64 GetElapsedTime()const;

		float GetMovedDistWhenRemoteReached()const;
		void SetMovedDistWhenRemoteReached(float fMovedDistWhenRemoteReached);
		float GetMovedDistWhenRemoteSuspended()const;
		void SetMovedDistWhenRemoteSuspended(float fMovedDistWhenRemoteSuspended);
		bool RemoteReached()const;
		float GetRemoteMovedDist()const;

		int32 GetTimeErr()const;
		float CalIdealDistByTimeInMovement(int32 nTime)const;
		void AccumulateSuspendedTime(uint32 uSuspendedTime);
		//debug
		bool	m_bChangePath;
	private:
		CFolMovement(CCoreSceneClient* pScene,const CFPos& PixelPosBegin,const CFPos& PixelPosEnd
			,EFolMovementType eType,float fSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType,float fEndDist, uint64 uServerBaseTime);

		
		CFolMovement::CFolMovement(CCoreSceneClient* pScene,const int16* pPathData, uint16 uPathDataLen, const CFPos& PixelPosBegin ,const CFPos& PixelPosEnd
			,EFolMovementType eType,float fSpeed,float fEndDist, uint64 uServerBaseTime);


		CPixelPath*			m_pPath;
		float				m_fPathLength;	//该数值必定小于等于m_pPath->GetPathPixelLength()
		bool				m_bPathIsCut;
		float				m_fSpeed;
		CServerTimeGuesser*	m_EstimatedServerTime;
		float				m_fMovedDistWhenRemoteSuspended;
		float				m_fMovedDistWhenRemoteReached;
		uint32				m_uTotalSuspendedTime;
	};

}