#pragma once
#include "TBaseScriptApp.h"


namespace sqr
{
	class CAppConfigStress;

	class CScriptAppStress
		:public TBaseScriptApp<CScriptAppStress>
	{
	public:
		typedef CAppConfigStress	Config_t;

		CScriptAppStress(const char* szConfig);
		~CScriptAppStress(void);

		string Run( const char* szFileName);

	private:
		void RegisterCoreScript();
	};
}

