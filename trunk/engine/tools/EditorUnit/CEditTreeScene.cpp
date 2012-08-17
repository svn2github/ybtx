#include "stdafx.h"
#include "CEditTreeScene.h"
#include "CRenderDebugFun.h"
#include "CRenderObject.h"

void sqr_tools::TSRenderAll(CRenderNode* pChild,void* param)
{
	pChild->SetVisible(true);
	pChild->Render();
}

void sqr_tools::TSUnRenderAll(CRenderNode* pChild,void* param)
{
	pChild->SetVisible(false);
}

void sqr_tools::TSHideAll(CRenderNode* pChild,void* param)
{
	pChild->SetAlphaValue(0);
}

void sqr_tools::TSShowAll(CRenderNode* pChild,void* param)
{
	pChild->SetAlphaValue(255);
}


void sqr_tools::TSShowBox(CRenderNode* pChild,void* param)
{
	if(pChild->GetNodeType()==CRenderObject::st_RenderObjectType)
	{
		RenderBigBoxDebug((CRenderObject*)pChild);
	}
}