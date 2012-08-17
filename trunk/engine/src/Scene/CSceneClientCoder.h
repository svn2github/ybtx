#pragma once
#include "CSceneAmpCoder.h"
#include "CSceneArgCoder.h"
#include "CSceneArsCoder.h"
#include "CSceneClassDefine.h"

namespace sqr
{
	class CSceneClientCoder : public CSceneCoder
	{
	public:
		bool SceneDeCode( CDataScene* pSceneManager );
	protected:
		static const SString FileName_Add[ESFT_COUNT];
		CSceneAmpCoder	m_AmpCoder;
		CSceneArgCoder	m_ArgCoder;
		CSceneArsCoder	m_ArsCoder;
	};
}