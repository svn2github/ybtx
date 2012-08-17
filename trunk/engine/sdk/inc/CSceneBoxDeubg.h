#pragma once
#include "CSceneDebugHelper.h"
#include "CRenderScene.h"

namespace sqr
{
	class CSceneBoxDebugNode:public CSceneDebugNode
	{
	public:
		CSceneBoxDebugNode(CRenderScene* pRenderScene):m_pRenderScene(pRenderScene){}
		~CSceneBoxDebugNode(){}
		void RenderDebug();
		void RenderDebugChild(CRenderObject* pRenderObj);
	protected:
		CRenderScene*	m_pRenderScene;
	};
}