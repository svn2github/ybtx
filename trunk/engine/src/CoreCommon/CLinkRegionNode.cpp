#include "stdafx.h"
#include "CLinkRegionNode.h"

CLinkRegionNode::CLinkRegionNode()
{

}


CLinkRegionNode::~CLinkRegionNode(void)
{
	if (m_pLinkList)
	{
		delete [] m_pLinkList;
		m_pLinkList = NULL;
	}
}

CPos CLinkRegionNode::GetKeyPos()
{
	return CPos(m_nKeyPosX, m_nKeyPosY);
}


