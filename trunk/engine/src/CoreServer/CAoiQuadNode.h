#pragma once
#include "CTraitsAoiQuad.h"
#include "TQuadNode.h"

namespace sqr
{
	class CAoiQuadNode : public TQuadNode<CTraitsAoiQuad>
	{
	public:
		friend class CAoiQuadTree;
		friend class CSynAoiDimObj;
		typedef TQuadNode<CTraitsAoiQuad> Parent_t;

		CAoiQuadNode(CAoiQuadNode* pParentNode, const CFRect& rcBox);
		~CAoiQuadNode();
	};
}

