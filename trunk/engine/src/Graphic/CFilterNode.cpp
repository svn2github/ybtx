#include "stdafx.h"
#include "CFilterNode.h"
#include "CRenderPipeline.h"
#include "CVertexDeclaration.h"
#include "CRenderList.h"

bool  CFilterNodePtrComp::operator()(const FilterNode* pLeft, const FilterNode* pRight) const
{
	return pLeft->sortSeed[0] <pRight->sortSeed[0] ||
	(pLeft->sortSeed[0]==pRight->sortSeed[0] && ( pLeft->sortSeed[1] < pRight->sortSeed[1] || 
	( pLeft->sortSeed[1] == pRight->sortSeed[1] && pLeft<pRight)));
}

//------------------------------------------------------------------------------
FilterNode& FilterNode::operator=( const FilterNode& fn )
{
	CRenderCommand::operator =(fn);
	m_RP = fn.m_RP;
	m_eType = fn.m_eType;

	sortSeed.resize(2,0);
	sortSeed[0] = (uint)m_RP.m_RS.m_Texture_S0;
	sortSeed[1] = 0;
	if(vertexData && vertexData->vertexBufferBinding)
	{
		sortSeed[1] = (uint)vertexData->vertexBufferBinding->GetBuffer(0);
	}

	m_ParentList = fn.m_ParentList;
	m_node = fn.m_node;
	pRenderStyle = fn.pRenderStyle;
	pUser = fn.pUser;
	m_ParentGroup = fn.m_ParentGroup;
	pAABox = fn.pAABox;
	return *this;
}

CRenderCommand&		FilterNode::operator=(const CRenderCommand& Com)
{
	CRenderCommand::operator =(Com);
	return *this;
}

CRenderOperation&	FilterNode::operator=(const CRenderOperation& op)
{
	CRenderOperation::operator = (op);
	return *this;
}

//------------------------------------------------------------------------------
FilterNode::FilterNode( const FilterNode& fn )
: CRenderCommand(fn)
{
	m_RP = fn.m_RP;
	m_eType = fn.m_eType;
	m_RP.m_RS.TexAddRef();

	sortSeed.resize(2,0);
	sortSeed[0] = (uint)m_RP.m_RS.m_Texture_S0;
	sortSeed[1] = 0;
	if(vertexData && vertexData->vertexBufferBinding)
	{
		sortSeed[1] = (uint)vertexData->vertexBufferBinding->GetBuffer(0);
	}

	m_ParentList = fn.m_ParentList;
	m_ParentGroup = fn.m_ParentGroup;
	m_node = fn.m_node;
	
	pRenderStyle = fn.pRenderStyle;
	pUser = fn.pUser;
	pAABox = fn.pAABox;
}

//------------------------------------------------------------------------------
FilterNode::FilterNode()
		:pRenderStyle(NULL)
		,pUser(NULL)
		,m_ParentList(NULL)
		,m_ParentGroup(NULL)
		,pAABox(NULL)
{
	sortSeed.resize(2,0);
}

FilterNode::~FilterNode()
{
	Release();
	m_RP.m_RS.TexRelease();
}

void	FilterNode::_Remove(void)
{
	SQR_TRY
	{
		if(m_ParentList)
		{
			m_ParentList->erase(this);
			m_ParentList = NULL;
		}
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);	
	}
	SQR_TRY_END
}

void FilterNode::Release(void)
{
	SQR_TRY
	{
		void (*temp_release)(CRenderCommand& Com);
		temp_release = param_release;
		param_release = NullRelease;
		_Remove();
		temp_release(*this);
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);	
	}
	SQR_TRY_END
}