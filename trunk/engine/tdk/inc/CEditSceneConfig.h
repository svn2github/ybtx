#pragma once

#include "CSceneConfig.h"

class TiXmlNode;
namespace sqr
{
	class CEditSceneConfig :public CSceneConfig
	{
	public:
		CEditSceneConfig(const string& ConfigFile);
		CEditSceneConfig(const string& ConfigFile,TiXmlElement* Node);
		~CEditSceneConfig(void);

		CEditSceneConfig* Inst();

		void DefaultSceneConfig();
		void AddSceneConfig(TiXmlNode* Node);
	};

}
