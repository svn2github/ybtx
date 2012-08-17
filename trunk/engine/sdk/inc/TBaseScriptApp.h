#pragma once
#include "CBaseScriptApp.h"

namespace sqr
{
	template<typename ImpRunner_t>
	class TBaseScriptApp
		:public CBaseScriptApp
	{
	public:
		static ImpRunner_t* Inst();

		CScript* CreateMainVM();
	};
}

