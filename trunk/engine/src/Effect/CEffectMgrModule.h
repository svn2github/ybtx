#pragma once
#include "IEffectMgrModule.h"
#include "CEffectManager.h"

class CEffectMgrModule : public IEffectMgrModule
{
	bool m_bEnableAsyncRead;
public:

	virtual IEffectManager* CreateEffectMgr(void)
	{
		return new CEffectManager();
	};

	virtual void EnableAsyncRead( bool bEnable )
	{
		m_bEnableAsyncRead = bEnable;
	}

	bool IsEnableAsyncRead()
	{ 
		return m_bEnableAsyncRead; 
	}
};