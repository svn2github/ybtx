#include "stdafx.h"
#include "CCoreQuadNode.h"
#include "CoreCommon.h"
#include "CSyncCoreObjectServer.h"
#include "CCoreQuadObject.h"
#include "TQuadNode.inl"
#include "TSqrAllocator.inl"

//-------------------------------------------------
CCoreQuadNode::CCoreQuadNode(CCoreQuadNode* pParentNode, const CFRect& rcRect)
: Parent_t(pParentNode, rcRect)
{

}


CCoreQuadNode::~CCoreQuadNode()
{

}

