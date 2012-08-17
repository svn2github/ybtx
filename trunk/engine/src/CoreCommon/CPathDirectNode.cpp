#include "stdafx.h"

#include "CPathDirectNode.h"

CPathDirectNode::CPathDirectNode()
{

}

CPathDirectNode::CPathDirectNode(int x, int y, float flength)
	:m_pos(x, y)
	,m_fRemainderLength(flength)
{
}


