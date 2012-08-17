#pragma once


namespace sqr
{
	class CAsyncContext;

	class ICoreSceneServerHandler
		: public virtual CDynamicObject
	{
	public:
		virtual ~ICoreSceneServerHandler(){}
		virtual void OnDestroy(CAsyncContext* Context) {}
		virtual void OnDestroyed(CAsyncContext* pContext) {}
	};
}

