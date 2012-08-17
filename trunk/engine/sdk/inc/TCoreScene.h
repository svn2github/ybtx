#pragma once

#include "FindPathDefs.h"
#include "TPart.h"
#include "CDynamicObject.h"
#include "TCoreSceneAllocator.h"

#ifdef GetObject
#undef GetObject
#endif

namespace sqr
{
	//********************************************************************************************
	//
	//		注意：此类是客户端场景和服务端场景公共基类的模版，是抽象类，不能独立产生场景
	//	    由于客户端场景和服务端场景是不需要在运行时被作为相同的基类场景使用的，为了清晰思路：
	//
	//			    + + + + + + + +不要在这个类里面添加任何虚函数+ + + + + + + +
	//
	//*********************************************************************************************
	template<class T> struct TPos;
	typedef TPos<int32> CPos;
	typedef TPos<float> CFPos;

	class CPixelPath;
	class CDynamicBarrierInfo;

	template<typename Traits>
	class TMetaScene;

	template<typename Traits>
	class TBaseMetaSceneServer;

	template<typename Traits>
	class TCoreObject;

	template<typename Traits>
	class TCoreSceneMgr;

	class CDistortedTimeObj;

	template<typename Traits>
	class TCoreScene
		:public virtual CDynamicObject
		,public TPart<typename Traits::CoreScene_t,typename Traits::MetaScene_t>
		,public TPart<typename Traits::CoreScene_t,typename Traits::CoreSceneMgr_t>
		,public CCoreSceneMallocObject
	{
	protected:
		typedef typename Traits::CoreObject_t				ImpCoreObject_t;
		typedef TCoreObject<Traits>							CoreObject_t;
		typedef typename Traits::CoreSceneMgr_t				ImpCoreSceneMgr_t;
		typedef TCoreSceneMgr<Traits>						CoreSceneMgr_t;
		typedef typename Traits::MetaScene_t				ImpMetaScene_t;
		typedef TMetaScene<Traits>							MetaScene_t;
		typedef typename Traits::CoreScene_t				ImpCoreScene_t;
		typedef TCoreScene<Traits>							CoreScene_t;
		typedef TPart<ImpCoreScene_t,ImpMetaScene_t>		ParentMetaPart_t;
		typedef TPart<ImpCoreScene_t,ImpCoreSceneMgr_t>		ParentMgrPart_t;

		typedef list<ImpCoreObject_t*,TCoreSceneAllocator<ImpCoreObject_t*> >						ListObject_t;
	
		friend class TCoreObject<Traits>;
		friend class TCoreSceneMgr<Traits>;
		friend class TBaseMetaSceneServer<Traits>;

		TCoreScene(MetaScene_t* pMetaScene, CoreSceneMgr_t* pMgr);
		~TCoreScene(void);

		bool HasObject()const;
		//给CoreSceneClient用于DestroyAllObject
		ImpCoreObject_t* GetFirstObject()const;
		const ListObject_t& GetAllObject()const;
		void AddObject(ImpCoreObject_t* pObj);
		void DelObject(ImpCoreObject_t* pObj);

	public:
		typedef vector<CPos> DynamicBarrierArea;
		ImpCoreSceneMgr_t* GetCoreSceneMgr()const;
		ImpMetaScene_t* GetMetaScene()const;

		CPixelPath* CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType, EBarrierType eBarrierType, int32 nMaxStep);
		CPixelPath* CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType, EBarrierType eBarrierType);

		CPixelPath* CreatePath(const int16* pPathData, uint16 uPathDataLen, const CFPos& PixelPosBegin, const CFPos& PixelPosEnd);

		bool IsGridValid(const CPos& GridPos) const;
		
		EBarrierType GetBarrier( const CPos& GridPos )const;

	private:
		ListObject_t	m_lstObject;
		CDynamicBarrierInfo*	m_pBarrierInfo;

	protected:
		void SetDynamicBarrierInfo(uint32 uOwnerID, const DynamicBarrierArea& dynamicBarrier, EBarrierType eBarrierType);
	};
}

