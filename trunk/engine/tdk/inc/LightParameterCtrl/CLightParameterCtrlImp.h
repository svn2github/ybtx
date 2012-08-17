#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr
{
	class CLight;
}

namespace sqr_tools
{
	class CLightParameterCtrlImp :public CCtrlBaseObserver
	{
		REG_UI_IMP(CLightParameterCtrlImp);
	public:
		CLightParameterCtrlImp(void* param);
		~CLightParameterCtrlImp(void);

	private:
		CLight*	curLight;
	public:
		virtual void Update();
		virtual void ActiveNode( CRenderNode* pNode );
	};

}
