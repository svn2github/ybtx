#pragma once 

#include "CTick.h"

namespace sqr
{

	class CScript;

	template<typename Traits>
	class TScriptAppGCTick
		: public CTick
	{
	public:
		typedef typename Traits::App_t				ImpApp_t;
		typedef typename Traits::AppConfig_t		ImpAppConfig_t;

		TScriptAppGCTick(CScript* pScript);
		~TScriptAppGCTick();	

	protected:		
		void OnTick();
		CScript*	m_pScript;
		uint32 GetStep()const;
	private:		

		uint32	m_uStep;
	};
}

