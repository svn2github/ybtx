#pragma once

#include "TQuadNode.h"
#include "CTraitsCoreQuad.h"

namespace sqr
{
	class CCoreQuadNode;
	class CCoreQuadObject;
	class CSyncCoreObjectServer;

	class CCoreQuadNode : public TQuadNode<CTraitsCoreQuad>
	{
	public:
		friend class CCoreQuadObject;

		typedef TQuadNode<CTraitsCoreQuad> Parent_t;

		CCoreQuadNode(CCoreQuadNode* pParentNode, const CFRect& rcRect);
		~CCoreQuadNode();
	};

}

