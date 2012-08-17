#pragma once
#include "TExpSafeApp.h"


namespace sqr
{
	template <typename Traits>
	class TMetaSceneMgr;

	template <typename Traits>
	class TCoreSceneMgr;

	template<typename Traits>
	class TCoreApp
		:public TExpSafeApp<Traits>
	{
	private:
		typedef typename Traits::MetaSceneMgr_t			ImpMetaSceneMgr_t;
		typedef typename Traits::CoreSceneMgr_t			ImpCoreSceneMgr_t;

		typedef TMetaSceneMgr<Traits>	MetaSceneMgr_t;
		typedef TCoreSceneMgr<Traits>	CoreSceneMgr_t;

	public:
		TCoreApp(void);
		~TCoreApp(void);
	};
}

