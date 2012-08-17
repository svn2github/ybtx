#pragma once

//$id$
//CCoreObjectDirector,本对象实现主角等由客户端控制的对象的位置同步功能

#include "CCoreObjectFollower.h"


namespace sqr
{
	class CCoreSceneClient;
	class CDirObjActiveState;
	class ICoreObjectDirectorHandler;

	class CGas2GacOC_Create_Still_Object;
	class CGas2GacOC_Create_Moving_Object;
	class CGas2GacOC_Set_Director_Active;
	class CGas2GacOC_Set_Dir_Max_Speed_And_Move_To;
	class CGas2GacOC_Set_Dir_Max_Speed_Zero;
	class CGas2GacOC_Set_Dir_Max_Speed;
	class CGas2GacOCI_Director_Create_End;

	class CRenderScene;
	class CCoreObjectDirector
		:public CCoreObjectFollower
	{
	public:
		ICoreObjectDirectorHandler* GetHandler()const;
		EClientObjectType GetType()const;

		static CCoreObjectDirector* Inst();

		bool ControlCamera(bool bControl);

		bool IsPassive()const;
		bool IsMoving()const;
		CPixelPath* GetClonedMovePath()const;

		//Follower对象的这4个函数永远返回not allowed
		ESetSpeedResult SetSpeed(float fSpeed);
		EStopMovingResult StopMoving();

		EMoveToResult MoveToLimit(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType, EBarrierType eBarrierType,float fReachDist, int32 nMaxStep);
		EMoveToResult MoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist);

		ESetPosResult SetPixelPos(const CFPos& PixelPos);
		ESetPosResult SetGridPos(const CPos& GridPos);
		virtual void GetServerPixelPos(CFPos& PixelPos)const;

		float GetMaxSpeed()const;
	protected:
		CCoreObjectDirector(const CGas2GacOC_Create_Still_Object* pCmd);
		~CCoreObjectDirector(void);

	private:
		friend class CConnClient;
		friend class CCoreSceneClient;

		bool	m_bWaitingToBeActive;	//等待进入主动状态
		bool	m_bActive;
		CDirObjActiveState*		m_pActState;
		void RefreshCameraDest(const CFPos& posCMemoryCookie);

		float	m_fDirectorMaxSpeed;

		void TellMediatorActivated();
		void TellMedMaxSpeedSet(uint32 uSessionID)const;

		template<typename CmdType>
		void SendCmdToMediator(CmdType* pCmd)const;

		void OnServerCommand(const CGas2GacOC_Set_Dir_Max_Speed_And_Move_To* pCmd);
		void OnServerCommand(const CGas2GacOC_Set_Director_Active* pCmd);
		void OnServerCommand(const CGas2GacOC_Set_Dir_Max_Speed_Zero* pCmd);
		void OnServerCommand(const CGas2GacOC_Set_Dir_Max_Speed* pCmd);
		void OnServerCommand(const CGas2GacOCI_Director_Create_End* pCmd);

		void StopMovingWhenPassThrough();
		void SwitchActiveMode();
		void IntSetPositionDerived(const CFPos& PixelPos);

		void OnMoveSteppedDerived(const CFPos&);
		void OnMoveBeganDerived(const CMoveBeganDerivedArgs& Args);
		void OnEndMoveDerived();
		void OnMoveStoppedDerived(float fStopDist);
		void OnMovePathChangedDerived(const CMovePathChangedDerivedArgs& Args);
		void OnMoveSpeedChangedDerived(const CMoveSpeedChangedDerivedArgs& Args);

		static uint64& GetLastDistortedProcessTime();	//记录上次退出场景时的扭曲时间,为了保证切换到下个场景后扭曲时间一致
		static uint64& GetLastProcessTime();	//记录上次退出场景时的真实时间
		static uint64& GetLastDistortedFrameTime();
		static uint64& GetLastFrameTime();

		bool CheckSpeed(float fSpeed)const;
	};
}
