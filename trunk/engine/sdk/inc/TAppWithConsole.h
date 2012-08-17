#pragma once

namespace sqr
{
	template<typename Traits>
	class TAppWithConsole
	{
		typedef typename Traits::AppConfig_t			ImpAppConfig_t;
	public:
		TAppWithConsole(void);
		~TAppWithConsole(void);
	};
}
