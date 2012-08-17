#pragma once
#include "CSceneManagerClient.h"
#include "CEditDataScene.h"
namespace sqr_tools
{
	class CYbtxGacCoder
		: public CSceneManagerClient
		, public CSceneCoder
	{
	public:
		static const string st_DefaultMapName;
		friend class CEditContext;

	public:
		CYbtxGacCoder(CDataScene* pScene);
		~CYbtxGacCoder();

		bool		SceneDeCode(CDataScene* pScene);
	};
}