#pragma once


//所有Core对象的基类
#include "CPos.h"
#include "CDynamicObject.h"
#include "FindPathDefs.h"
#include "CoreObjectDefs.h"
#include "CCoreObjMallocObject.h"
#include "TCoreSceneAllocator.h"


namespace sqr
{
	//********************************************************************************************
	//
	//		注意：此类是客户端所有对象和服务端所有对象的公共基类的模版，是抽象类，不能独立产生对象
	//	    由于客户端对象和服务端对象是不需要在运行时被作为相同的基类对象使用的，为了清晰思路起见：
	//
	//			    + + + + + + + +不要在这个类里面添加任何虚函数+ + + + + + + +
	//
	//*********************************************************************************************

#ifdef GetObject
#undef GetObject
#endif

	template<typename ValueType>
	class TIDPtrMap;

	class CPixelPath;
	class CMedObjFollowState;
	class CTestSuiteCoreObjectServer;

	template<typename Traits>
	class TCoreScene;

	template<typename Traits>
	class TObjMoveState;

	template<typename Traits>
	class TTimeSystem;

	template < class HolderType, class PointeeHolderType >
	class TPtRefee;

	template<typename Traits>
	class TBaseCOEvent;

	class CCoreObjectClient;
	class CSyncCoreObjectServer;

	class IDistortedTime;
	class CDistortedTimeObj;

	template<typename Traits>
	class TCoreObject
		:public virtual CDynamicObject
		,public CCoreObjMallocObject
	{
		typedef typename Traits::CoreObject_t			ImpCoreObject_t;
		typedef typename Traits::ObjMoveState_t			ImpObjMoveState_t;
		typedef typename Traits::CoreObjectHandler_t	ImpHandler_t;
		typedef typename Traits::CoreScene_t			ImpCoreScene_t;
		typedef typename Traits::MetaScene_t			ImpMetaScene_t;
		typedef typename Traits::TimeSystem_t			ImpTimeSystem_t;
		typedef TTimeSystem<Traits>						TimeSystem_t;

		friend class CCoreObjectClient;
		friend class CSyncCoreObjectServer;
		//friend class TBindObject<ImpCoreObject_t>;

	public:
		//--------下列函数供外部类调用--------

		static ImpCoreObject_t* GetObject(uint32 uLocalID);

		ImpCoreScene_t* GetScene()const;

		void SetHandler(ImpHandler_t* pHandler);
		ImpHandler_t* GetHandler()const;

		void GetGridPos(CPos& GridPos)const;

		const CFPos& GetPixelPos()const;
		void GetPixelPos(CFPos& pos);
		uint32	GetLocalID()const;

		float GetSpeed()const;

		IDistortedTime* GetDistortedTime()const;

		bool IsBindingObj() const {return false;}

		//********上面的函数供外部类调用********

	protected:

		friend class CTestSuiteCoreObjectServer;
		//----------下列函数专供派生类调用------------

		friend class TCoreScene<Traits>;
		friend class TObjMoveState<Traits>;
		friend class CMedObjFollowState;
		template<typename>
		friend class TBaseCOEvent;

		TCoreObject(ImpHandler_t* pHandler,const CFPos& PixelPos,ImpCoreScene_t* pScene);
		~TCoreObject();

		/*
		@.Int作为前缀的函数对对象的状态数值进行真正的修改，对象处于被动和主动状态最终都是使用Int系列函数来修改对象状态
		@.非Int系列函数用于给用户对处于主动状态的对象直接操作。这些函数会判断对象的当前运行状态，给出成功或者错误码
		@.Derived作为后缀的函数供派生类重写，以便发送网络命令
		@.Int作为前缀，同时Derived作为后缀的函数，用于让派生类修改更多的数值(例如Visible和Observer)，也可以完成发送网
		络命令等功能。需要非常注意的是，这类型的函数不应该由派生类触发任何回调函数。
		*/

		ESetSpeedResult IntSetSpeed(float fSpeed);
		//派生类决定MoveTo请求是否被执行（Follower对象总是不会被执行，Director对象则根据是否处于主动状态来决定能否执行）
		EMoveToResult IntMoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,
			EBarrierType eBarrierType,float fReachDist, float fMovedDist, CPixelPath* pPixelPath, int32 nMaxStep=-1);

		EStopMovingResult IntStopMoving();
		//当对象的位置被外部类修改时，子类可以根据对象的状态决定是否进行位置修改，仅面向SetPixelPos函数，其他函数不得调用。
		ESetPosResult IntSetPixelPos(const CFPos& PixelPos);


		//进行一个从指定起点到指定目标点的移动行为,DoMove自己是不会修改对象的位置的。
		//只有移动中的对象才有速度。
		//uPixelSpeed Pixel per second
		//fBeginDist从距离起点指定距离的位置开始移动，主要用于创建移动中的Follower对象，这种对象在创建时刻就处在整条移动路径的中间的某个位置。
		//fReachDist移动到距离终点这么多距离时停止。
		//返回false表示目标点与当前点重合
		EMoveToResult DoMove(const CFPos& PixelPosBegin,const CFPos& PixelPosEnd,float fSpeed
			,EFindPathType eFindPathType,EBarrierType eBarrierType,float fBeginDist, CPixelPath* pPath, int32 nMaxStep=-1);
		void DoMove(CPixelPath* pPath,float fSpeed,float fBeginDist);
		//返回false表示原本就已经处于停止状态
		bool IntStopDoingMove();
		bool IntIsDoingMove()const;
		//return false表示范围超过场景大小	//仅仅修改数值
		void RawSetPixelPos(const CFPos& PixelPos);
		bool IntSetPositionSingle(const CFPos& PixelPos);
		bool IntGetCurPixelWayPoint(CFPos& PixelPos)const;


		//下面两个函数都会修改对象的位置，dictator对象会修改visible的位置，mediator对象会修改Observer的位置
		//IntSetPositionDerived用于直接设定坐标的位置变化
		void IntSetPositionDerived(const CFPos& PixelPos);	//return false表示超出地图范围

		ImpObjMoveState_t* GetMovState()const;

		bool IntSetPosition(const CFPos& PixelPos);
		//ESetPosResult IntSetPixelPos( const CFPos& PixelPos );

		float GetRealMovedDistDerived()const;

		CDistortedTimeObj* GetDistortedTimeObj()const;
		//******上面的函数专供派生类调用

	private:
		//-----------下列函数专供TObjMoveState来调用-----------

		void IntOnMoveEnded();


		//服务端对象应在Step被调用时将移动进度告诉客户端对象
		//客户端对象应在Step被调用时调整自己的移动速度

		//******上面的函数专供TObjMoveState来调用



		//------------下列内容专供自己使用----------------
		uint32				m_uID;
		ImpHandler_t*		m_pHandler;
		ImpObjMoveState_t*	m_pMovState;
		CFPos				m_Pos;


		typedef TPtRefee< ImpCoreObject_t , TBaseCOEvent<Traits> >	RefeeByEvent_t;
		RefeeByEvent_t*	m_pRefByEvent;

		typedef TIDPtrMap<ImpCoreObject_t>			MapObject_t;
		typedef list<ImpCoreObject_t*,TCoreSceneAllocator<ImpCoreObject_t*> >				ListObject_t;

		static MapObject_t& GetObjectMap();

		typename ListObject_t::iterator		m_itObjectListInScene;

		void OnWayPointChangedDerived();

		CDistortedTimeObj*		m_pDistortedTimeObj;
		//********上面的内容专供自己使用****************
	};
}
