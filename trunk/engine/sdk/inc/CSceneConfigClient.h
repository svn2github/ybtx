#pragma once
#include "CSceneConfig.h"

namespace sqr
{
	class CIniFile;

	class CSceneConfigClient
		:public CSceneConfig
	{
	public:
		CSceneConfigClient(const string& sSceneConfig,CIniFile* pIniFile);
		~CSceneConfigClient(void);

		void LoadUserConfig();
		/// ±£¥Ê≈‰÷√µΩini
		void SaveConfig();	
		
	private:		
		CIniFile*	m_pIniFile;
	};

}