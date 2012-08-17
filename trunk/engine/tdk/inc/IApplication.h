#pragma once
#include "TSingleton.h"

namespace sqr_tools
{
	class ICtrlImpFactory;
}

namespace sqr
{
	class CTick;
	class IApplication : public Singleton<IApplication>
	{
	public:
		virtual void	Close(void)			= 0;
		virtual uint32	GetCurTime(void)	= 0;
		virtual void	RegisterTick(CTick* pTick,uint32 uCyc)	= 0;
		virtual void	UnRegisterTick(CTick* pTick)			= 0;
		virtual void	Run(void)								= 0;
		virtual ICtrlImpFactory*	GetCtrlImpFactory(void)		= 0;	

		static	void	InitApp(void);
	};
}