#include "stdafx.h"
#include "CAoiQuadNode.h"
#include "CSynAoiDimObj.h"
#include "TQuadNode.inl"
#include "TSqrAllocator.inl"

CAoiQuadNode::CAoiQuadNode(CAoiQuadNode* pParentNode, const CFRect& rcBox)
: Parent_t(pParentNode, rcBox)
{

}

CAoiQuadNode::~CAoiQuadNode()
{

}

