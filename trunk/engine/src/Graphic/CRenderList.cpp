#include "stdafx.h"
#include "CRenderList.h"
#include "TSqrAllocator.inl"

bool CRenderList::insert(FilterNode* pNode)
{
	pNode->_Remove();
	pair< MapList::iterator, bool >	p = FindList.insert(MapList::value_type(pNode,TraverseList.begin()));

	if(!p.second)
	{
		GfkLogErr("CRenderList异常","节点重复插入");
		return false;
	}

	MapList::iterator it = p.first;
	++it;
	if(it != FindList.end())
	{
		p.first->second = TraverseList.insert(it->second,pNode);
	}
	else
	{
		TraverseList.push_back(pNode);
		p.first->second  = --TraverseList.end();
	}
	pNode->m_ParentList = this;
	pNode->m_node = p.first;
	return true;
}
CRenderList::CRenderList(void)
{

}

CRenderList::~CRenderList(void)
{
	CRenderList::iterator it = TraverseList.begin();
	CRenderList::iterator eit = TraverseList.end();
	for (;it!=eit;++it)
		(*it)->m_ParentList = NULL;
}


void CRenderList::erase(const FilterNode* pNode)
{
	size_t check = TraverseList.size();
	TraverseList.erase(pNode->m_node->second);
	FindList.erase(pNode->m_node);
	Ast(TraverseList.size()<check && FindList.size() == TraverseList.size() &&"CRenderList::erase");
}

CRenderList::iterator CRenderList::begin(void)
{
	return TraverseList.begin();
}

CRenderList::iterator CRenderList::end(void)
{
	return TraverseList.end();
}

bool CRenderList::empty(void) const
{
	return TraverseList.empty();
}

void CRenderList::clear(void)
{
	FindList.clear();
	TraverseList.clear();
}