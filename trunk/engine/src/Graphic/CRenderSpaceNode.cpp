#include "stdafx.h"
#include "CRenderSpaceNode.h"

const DWORD CRenderSpaceNode::st_NodeType = 'NODE';

CRenderSpaceNode::CRenderSpaceNode()
: m_pParentSpace(NULL)
, m_uLinkID(0xfffffffe)
, m_eLinkType(LT_CENTER)
{
	m_pNodeHandler = NULL;
}

CRenderSpaceNode::~CRenderSpaceNode()
{
	if(m_pParentSpace)
		m_pParentSpace->DelChild( this );
	m_pNodeHandler = NULL;
}

DWORD CRenderSpaceNode::GetNodeType()
{ return st_NodeType; }

bool CRenderSpaceNode::UpdateTime(uint32 uCurTime)
{
	return false;
}