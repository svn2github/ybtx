#pragma once
#include "TBaseScriptApp.h"

namespace sqr
{
	class CAppConfigClient;

	class CScriptAppClient
		:public TBaseScriptApp<CScriptAppClient>
	{
	public:
		typedef CAppConfigClient	Config_t;

		CScriptAppClient( const char* szConfig, const char* szGfkConfig);
		~CScriptAppClient(void);

		string Run( const char* szFileName );

	private:
		void RegisterCoreScript();
	};
}
