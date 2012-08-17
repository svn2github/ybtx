#include "StdAfx.h"
#include "GUIRenderer.h"
#include "Scene.h"
#include "EngineImpl.h"
#include "CEditRenderPipeline.h"
#include "CGraphic.h"

namespace sqr
{

CGUIRenderer::CGUIRenderer(void)
{
	m_pImpl=NULL;
}

CGUIRenderer::~CGUIRenderer(void)
{
	if(CMainWindowTarget::HasInst())
	{
		CMainWindowTarget::GetInst()->DeleteRenderer(this);
	}
}

void CGUIRenderer::SetEngineImpl(CEngineImpl* p)
{
	m_pImpl=p;
}

void CGUIRenderer::Render()
{
	return;
}

}
