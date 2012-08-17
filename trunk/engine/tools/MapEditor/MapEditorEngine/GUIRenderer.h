#pragma once

#include "CRendererBase.h"

namespace sqr
{
	class CEngineImpl;

	class CGUIRenderer :public CRendererBase
	{
	public:
		CGUIRenderer(void);
		~CGUIRenderer(void);

		virtual void Render();

		void SetEngineImpl(CEngineImpl* p);
	private:
		CEngineImpl *m_pImpl;
		
	};

}