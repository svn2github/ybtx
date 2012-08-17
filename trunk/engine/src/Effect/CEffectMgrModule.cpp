#include "stdafx.h"
#include "CEffectMgrModule.h"

namespace sqr
{

	EFFECT_API IEffectMgrModule* GetIEffectMgrModule()
	{
		static CEffectMgrModule	s_Instance;
		return &s_Instance;
	}

}