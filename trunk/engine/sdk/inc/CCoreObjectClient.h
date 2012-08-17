#pragma once

#include "TCoreObject.h"
#include "CTraitsClient.h"
#include "CoreObjectClientDefs.h"

namespace sqr
{
	class ICoreObjectClientHandler;
	class CCoreSceneClient;
	class CRenderObject;
	struct CMoveBeganDerivedArgs;
	struct CMovePathChangedDerivedArgs;
	struct CMoveSpeedChangedDerivedArgs;
	class CObjMoveStateClient;

	template <typename ObjectType>
	class TBindObject;

	class CCoreObjectClient
		:public TCoreObject<CTraitsClient>
	{
		friend class TBindObject<CCoreObjectClient>;
	public:
		bool GetCurPixelWayPoint(CFPos& PixelPos)const;
		bool GetCurGridWayPoint(CPos& GridPos)const;

		virtual EClientObjectType GetType()const=0;

		virtual bool IsMoving()const;
		virtual void LogMsg(const string& log);

		virtual bool BindTo(uint32 uParentID)	{return false;}
		virtual bool BindTo(CCoreObjectClient* pParent)	{return false;}
		virtual bool DisbindFromParent()	{return false;}
		virtual bool DisbindChild(uint32 uChildID)	{return false;}
		virtual bool DisbindAllChildren()	{return false;}
		virtual bool IsBindingObj() const	{return false;}

		//BindObj引擎线程和逻辑线程用
		virtual void OnBindTo(uint32 uParentID, bool bRet) {};			//客户端需要用来强行设置坐标
		void OnDisbindFromParent(bool bRet) {};
		void OnDisbindToChild(uint32 uChildID, bool bRet) {};
		void OnDisbindToAllChild(bool bRet) {};

		//BindObj给自身派生类用的回调，逻辑线程（修改AOI）专用
		void OnDisbindParentSingle() {};
		void OnDisbindChildSingle() {};
		void OnBeforeBindTo(uint32 uParentID, bool bRet) {};

		//BindObj客户端（预绑定关系）专用
		virtual void OnBeforeDisbindFromParent() {};
		virtual void OnBeforeDisbindToChild(uint32 uChildID) {};
		virtual void OnBeforeDisbindToAllChild() {};

	protected:
		typedef TCoreObject<CTraitsClient>	ParentObject_t;

		virtual bool BindObjIntSetPosition(const CFPos& PixelPos) {return true;}
		//virtual void BindObjNotifyPosChangedIfNeed() {}

		CCoreObjectClient(ICoreObjectClientHandler* pHandler,const CFPos& PixelPos);
		virtual ~CCoreObjectClient();

		ESetPosResult IntSetGridPos(const CPos& PixelPos);

		//下列函数是TCoreObject的模版虚函数，从CCoreObjectClient开始他们也变成了普通的虚函数

		virtual void IntSetPositionDerived(const CFPos& PixelPos);
		//下面函数不会修改父类函数的功能，重写的目的是为了把他们变成普通虚函数
		virtual void OnMoveSteppedDerived(const CFPos&);
		virtual void OnMoveBeganDerived(const CMoveBeganDerivedArgs& Args);
		virtual void OnEndMoveDerived();
		virtual void OnMoveStoppedDerived(float fStopDist);
		virtual void OnMovePathChangedDerived(const CMovePathChangedDerivedArgs& Args);
		virtual void OnMoveSpeedChangedDerived(const CMoveSpeedChangedDerivedArgs& Args);
		virtual void OnWayPointChangedDerived();
		virtual void OnPixelPosSetDerived(bool bSuperPosition);
		virtual float GetRealMovedDistDerived()const;
		//上面的函数是TCoreObject的模版虚函数，从CCoreObjectClient开始他们也变成了普通的虚函数

	private:
		friend class TCoreScene<CTraitsClient>;
		friend class TCoreObject<CTraitsClient>;
		friend class TObjMoveState<CTraitsClient>;
		friend class CCoreSceneClient;
		friend class CObjMoveStateClient;

		uint32 CreateID();
		bool DestroyID();

#ifdef USE_RENDER_SLOT
		virtual void OnSpeedAdjust(uint32 uMoveInterval);
#endif

		CCoreSceneClient* IntGetScene()const;
		void RefreshRenderObjectPos();

	};
}
