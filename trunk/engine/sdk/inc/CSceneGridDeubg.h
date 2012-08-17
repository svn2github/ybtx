#pragma once
#include "GraphicBase.h"
#include "CSceneDebugHelper.h"

namespace sqr
{
	class CRenderScene;
	class CSceneGridDebugNode:public CSceneDebugNode
	{
	public:
		CSceneGridDebugNode(ERenderObjStyle style , CRenderScene* pScene );
		
		~CSceneGridDebugNode(){}
		
		void RenderDebug();

	protected:
		ERenderObjStyle m_RenderObjStyle;
		CRenderScene*	m_pRenderScene;
		uint32			m_uRange;
	};
}