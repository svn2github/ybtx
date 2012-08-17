#pragma once
#include "TSingleton.h"
//#include "PathBrowser\PathBrowser.h"
#include "LocalPathBrowser.h"

namespace sqr_tools
{
	class CCharacterBrowserContext;
}

namespace sqr
{
	class CharacterBrowserBriage : public Singleton<CharacterBrowserBriage>
	{
	private:
		CLocalPathBrowser			m_PathBrowser;
		void*						m_pShowWndHandle;
		CCharacterBrowserContext*	m_pContext;

	private:
		void InitConfig();
		void InitLua();
		void Release();

	public:
		CharacterBrowserBriage(void);
		~CharacterBrowserBriage(void);

	public:
		bool InitEnviroment();
		void OnIdle();
		void OnSize();

		void RegisterWinFormHandle(void* handle);
		void InitRenderMgr();
	};

}
