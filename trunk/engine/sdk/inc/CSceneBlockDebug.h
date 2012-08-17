#pragma once
#include "GraphicBase.h"
#include "CSceneDebugHelper.h"
#include "FindPathDefs.h"

namespace sqr
{
	class CRenderScene;
	class CSceneBlockDebugNode:public CSceneDebugNode
	{
	public:
		CSceneBlockDebugNode(ERenderObjStyle style , CRenderScene* pScene);
		~CSceneBlockDebugNode(){}
				void			RenderDebug();
		virtual EBarrierType	GetBarrierType(CPos pos);

	protected:
		ERenderObjStyle m_RenderObjStyle;
		CRenderScene*	m_pRenderScene;
		uint32			m_uRange;
	};
}