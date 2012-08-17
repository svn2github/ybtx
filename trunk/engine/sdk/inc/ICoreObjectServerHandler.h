#pragma once
#include "CDynamicObject.h"
#include "CoreObjectDefs.h"
#include "CPos.h"
#include "PatternRef.inl"

namespace sqr
{
	class ICoreObjectDictatorHandler;
	class ICoreObjectCalculatorHandler;
	class CCoreSceneServer;
	class CCoreObjectServer;

	class ICoreObjectServerHandler : public virtual CDynamicObject
	{
		friend class CCoreObjectServer;

	public:
		ICoreObjectServerHandler() { m_RefsByCoreObj.SetHolder(this); }

		//开始移动，已经进入移动状态，但是坐标还没有变化
		virtual void OnMoveBegan() {}
		//移动路径已改变
		virtual void OnMovePathChanged() {}
		//已经到达目标点，已经处于静止状态
		virtual void OnMoveEnded(EMoveEndedType, uint32 uMoveID) {}

		virtual void OnTraceEnded(ETraceEndedType) {}

		//如果pOldScene是NULL,则说明转场景失败。原因是目标场景已经被销毁。
		//该函数被触发之前，对象的移动状态将被停止。OnMoveEnded将会在OnTransfered回调之前触发。
		//DestroyObject将导致进行中的转场景行为被取消，OnTransfered回调也会被取消。
		virtual void OnTransfered(CCoreSceneServer* pOldScene, const CFPos& OldPos) {}

		virtual void OnAoiMsgFromSelf(const void* pContext){}

		//由PostEvent发送给当前对象的事件。
		virtual void OnEvent(const void* pData) {};

		virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension) {}
		virtual void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension) {}

		virtual void OnDestroy() {}

		//BindObj同步语义不需要发给逻辑线程
		//virtual void OnBindToParent(uint32 uObjID, bool bRet) {}
		//virtual void OnBindToChild(uint32 uObjID, bool bRet) {}
		//virtual void OnDisbindFromParent(uint32 uObjID, bool bRet) {}
		//virtual void OnDisbindToChild(uint32 uObjID, bool bRet) {}

		virtual ICoreObjectDictatorHandler* CastToObjDicHandler()const
		{
			return NULL;
		}
		virtual ICoreObjectCalculatorHandler* CastToObjCalHandler()const
		{
			return NULL;
		}
		virtual void* GetTag() const
		{
			return NULL;
		}
		virtual ~ICoreObjectServerHandler(){}
	private:
		TPtRefee<ICoreObjectServerHandler, CCoreObjectServer> m_RefsByCoreObj;
	};
}


