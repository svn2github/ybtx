#pragma once
#include "TAppWithConsole.h"

template<typename Traits>
TAppWithConsole<Traits>::TAppWithConsole(void)
{
#ifdef _WIN32
	if( ImpAppConfig_t::Inst()->ShowConsole() )
	{
		ShowConsole(true);
		SetConsoleTitle( ImpAppConfig_t::Inst()->GetConsoleTitle().c_str() );
		//定位errlog的输出到控制台
		RedirectIOToConsole(true);
	}
#endif
}

template<typename Traits>
TAppWithConsole<Traits>::~TAppWithConsole(void)
{
#ifdef _WIN32
	if( ImpAppConfig_t::Inst()->ShowConsole() )
	{	
		RedirectIOToConsole(false);
		ShowConsole(false);
	}
#endif
}
