#pragma once
#include "CCoreSceneMallocObject.h"

namespace sqr
{
	class CSyncCoreSceneServer;

	class CAoiSceneHandler
		:public CCoreSceneMallocObject
	{
	public:
		CAoiSceneHandler(CSyncCoreSceneServer* pScene);
		virtual ~CAoiSceneHandler();

		virtual void OnHalted();

	private:
		CSyncCoreSceneServer*	m_pScene;
	};

}

