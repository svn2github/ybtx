#pragma once
#include "CTreeScene.h"

namespace sqr_tools
{
	void TSRenderAll(CRenderNode* pChild,void* param);
	void TSUnRenderAll(CRenderNode* pChild,void* param);
	void TSHideAll(CRenderNode* pChild,void* param);
	void TSShowAll(CRenderNode* pChild,void* param);
	void TSShowBox(CRenderNode* pChild,void* param);
}