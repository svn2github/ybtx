#pragma once
#include "GraphicBase.h"
#include "CSceneDebugHelper.h"

namespace sqr
{
	class CRenderScene;
}

namespace sqr_tools
{
	class CGridNode:public CSceneDebugNode
	{
	public:
		CGridNode(ERenderObjStyle style , CRenderScene* pScene );
		~CGridNode(){}

		virtual void RenderDebug();
		virtual float GetGroundSpeed(void);
		virtual void SetGroundSpeed(float speed);

	protected:
		float m_fGroundSpeed;
		ERenderObjStyle m_RenderObjStyle;
		CRenderScene*	m_pRenderScene;
		uint32			m_uRange;
		uint32			m_uPreTime;
		uint32			m_uCurTime;
		float			m_MoveNow;
	};
}