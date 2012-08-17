#pragma once
#include "CRefObject.h"
#include "CSceneMallocObject.h"

namespace sqr
{
	class CSceneDebugNode : public CRefObject
	{
	public:
		CSceneDebugNode(){}
		~CSceneDebugNode(){}
		virtual void RenderDebug() = 0;
	};

	typedef SVector<CSceneDebugNode*> SceneDebugPool;

	class CSceneDebugMgr
	{
	public:
		CSceneDebugMgr();
		~CSceneDebugMgr();
		SceneDebugPool m_SceneDebugList;

		void RegistDebugNode(CSceneDebugNode* pDebugNode);
		void DelelteDebugNode(CSceneDebugNode* pDebugNode);	
	protected:
		void SceneDebugRun();
		void SceneDebugClear();
	};
}
