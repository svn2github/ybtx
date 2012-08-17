#pragma once

#include "CGraphic.h"
#include "IEffectManager.h"
#include "EffectMgrDefs.h"

namespace sqr
{

	class IEffectMgrModule : public CEffectMallocObject
	{
	public:
		virtual IEffectManager* CreateEffectMgr( void ) = 0;
		virtual void		    EnableAsyncRead( bool bEnable ) = 0;
	};

	EFFECT_API IEffectMgrModule* GetIEffectMgrModule();

}