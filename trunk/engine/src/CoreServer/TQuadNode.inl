#pragma once
#include "TQuadNode.h"
#include "QuadTreeFun.h"
#include "ExpHelper.h"


template<typename Traits>
TQuadNode<Traits>::TQuadNode(ImpQuadNode_t* pParentNode, const CFRect& rcBox)
: m_pParentNode(pParentNode)
, m_pNEChildNode(NULL)
, m_pNWChildNode(NULL)
, m_pSWChildNode(NULL)
, m_pSEChildNode(NULL)
, m_rcQuadNodeBox(rcBox)
{

}


template<typename Traits>
TQuadNode<Traits>::~TQuadNode()
{
	DeleteChildNodes();
	
	m_lstObject.clear();
	m_lstViewRect.clear();
	m_lstSizeRect.clear();
}

template<typename Traits>
void TQuadNode<Traits>::AddObjectToNode(ImpQuadObject_t* pObj)
{
	Ast(*pObj->m_ItObject == NULL);
	m_lstObject.push_front(pObj);
	pObj->m_ItObject = m_lstObject.begin();	

	pObj->m_pObjectNode = static_cast<ImpQuadNode_t*>(this);
}

template<typename Traits>
void TQuadNode<Traits>::RemoveObjectFromNode(ImpQuadObject_t* pObj)
{
	Ast(*pObj->m_ItObject != NULL);
	m_lstObject.erase(pObj->m_ItObject);
	pObj->m_ItObject = pObj->ms_DummyList.begin();

	pObj->m_pObjectNode = NULL;
}

template<typename Traits>
void TQuadNode<Traits>::AddSizeRectToNode(ImpQuadObject_t* pObj)
{
	Ast(*pObj->m_ItSizeRect == NULL);
	m_lstSizeRect.push_front(pObj);
	pObj->m_ItSizeRect = m_lstSizeRect.begin();	
	pObj->m_pSizeRectNode = static_cast<ImpQuadNode_t*>(this);
}


template<typename Traits>
void TQuadNode<Traits>::RemoveSizeRectFromNode(ImpQuadObject_t* pObj)
{
	Ast(*pObj->m_ItSizeRect != NULL);
	m_lstSizeRect.erase(pObj->m_ItSizeRect);
	pObj->m_ItSizeRect = pObj->ms_DummyList.begin();
	pObj->m_pSizeRectNode = NULL;
}


template<typename Traits>
void TQuadNode<Traits>::AddViewRectToNode(ImpQuadObject_t* pObj)
{
	Ast(*pObj->m_ItViewRect == NULL);
	m_lstViewRect.push_front(pObj);
	pObj->m_ItViewRect = m_lstViewRect.begin();
	pObj->m_pViewRectNode = static_cast<ImpQuadNode_t*>(this);
}


template<typename Traits>
void TQuadNode<Traits>::RemoveViewRectFromNode(ImpQuadObject_t* pObj)
{
	Ast(*pObj->m_ItViewRect != NULL);
	m_lstViewRect.erase(pObj->m_ItViewRect);
	pObj->m_ItViewRect = pObj->ms_DummyList.begin();

	pObj->m_pViewRectNode = NULL;
}

//------------------------------------------------------------

template<typename Traits>
void TQuadNode<Traits>::CreateChildNodes()
{
	CFRect rcChildBox;
	float fCenterX = (m_rcQuadNodeBox.right - m_rcQuadNodeBox.left) / 2;
	float fCenterY = (m_rcQuadNodeBox.bottom - m_rcQuadNodeBox.top) / 2;
	
	ImpQuadNode_t* pParent = static_cast<ImpQuadNode_t*>(this);

	rcChildBox.left = m_rcQuadNodeBox.left + fCenterX;
	rcChildBox.right = m_rcQuadNodeBox.right;
	rcChildBox.top = m_rcQuadNodeBox.top;
	rcChildBox.bottom = m_rcQuadNodeBox.top + fCenterY;

	m_pNEChildNode = new ImpQuadNode_t(pParent, rcChildBox);

	rcChildBox.left = m_rcQuadNodeBox.left ;
	rcChildBox.right = m_rcQuadNodeBox.left + fCenterX;
	rcChildBox.top = m_rcQuadNodeBox.top;
	rcChildBox.bottom = m_rcQuadNodeBox.top + fCenterY;

	m_pNWChildNode = new ImpQuadNode_t(pParent, rcChildBox);

	rcChildBox.left = m_rcQuadNodeBox.left ;
	rcChildBox.right = m_rcQuadNodeBox.left + fCenterX;
	rcChildBox.top = m_rcQuadNodeBox.top + fCenterY;
	rcChildBox.bottom = m_rcQuadNodeBox.bottom;

	m_pSWChildNode = new ImpQuadNode_t(pParent, rcChildBox);

	rcChildBox.left = m_rcQuadNodeBox.left + fCenterX;
	rcChildBox.right = m_rcQuadNodeBox.right;
	rcChildBox.top = m_rcQuadNodeBox.top + fCenterY;
	rcChildBox.bottom = m_rcQuadNodeBox.bottom;

	m_pSEChildNode = new ImpQuadNode_t(pParent, rcChildBox);

	SetChildNodesData();
}


template<typename Traits>
void TQuadNode<Traits>::SetChildNodesData()
{
	if (m_lstObject.empty())
	{
		return;
	}
	//如果此时父节点上面有其他对象，则把其他对象挂载到相应的子节点上面
	typename ListQuadObject_t::const_iterator iter = m_lstObject.begin();
	
	ImpQuadNode_t* pThis = static_cast<ImpQuadNode_t*>(this);

	while (iter != m_lstObject.end())
	{
		ImpQuadObject_t* pObj = *iter;
		const CFPos& pos = pObj->GetPosition();

		ImpQuadNode_t* pNode = ImpQuadTree_t::FindDownQuadNodeWithPos(pos, pThis);
		
		RemoveObjectFromNode(pObj);
		pNode->AddObjectToNode(pObj);
		
		iter = m_lstObject.begin();
	}
	
	iter = m_lstSizeRect.begin();
	while (iter != m_lstSizeRect.end())
	{
		ImpQuadObject_t* pObj = *iter;
		++iter;
		const CFRect& rect = pObj->GetSizeRect();

		ImpQuadNode_t* pNode = ImpQuadTree_t::FindDownQuadNodeWithRect(rect, pThis);

		if (pNode != pThis)
		{
			RemoveSizeRectFromNode(pObj);
			pNode->AddSizeRectToNode(pObj);
		}
	}	

	iter = m_lstViewRect.begin();
	while (iter != m_lstViewRect.end())
	{
		ImpQuadObject_t* pObj = *iter;
		++iter;
		const CFRect& rect = pObj->GetViewRect();

		ImpQuadNode_t* pNode = ImpQuadTree_t::FindDownQuadNodeWithRect(rect, pThis);

		if (pNode != pThis)
		{
			RemoveViewRectFromNode(pObj);
			pNode->AddViewRectToNode(pObj);
		}
	}
}


template<typename Traits>
void TQuadNode<Traits>::DeleteChildNodes()
{
	if (m_pNEChildNode)
	{
		delete m_pNEChildNode;
		m_pNEChildNode = NULL;

		delete m_pNWChildNode;
		m_pNWChildNode = NULL;
	
		delete m_pSWChildNode;
		m_pSWChildNode = NULL;
	
		delete m_pSEChildNode;
		m_pSEChildNode = NULL;
	}
}


template<typename Traits>
void TQuadNode<Traits>::MergeChildNodeData(ImpQuadNode_t* pParent, ImpQuadNode_t* pChild)
{
	ImpQuadObject_t* pObj;
	typename ListQuadObject_t::iterator iter = pChild->m_lstObject.begin();
	
	while (iter != pChild->m_lstObject.end())
	{
		pObj = *iter;
		pChild->RemoveObjectFromNode(pObj);
		pParent->AddObjectToNode(pObj);
		iter = pChild->m_lstObject.begin();
	}

	iter = pChild->m_lstSizeRect.begin();
	while (iter != pChild->m_lstSizeRect.end())
	{
		pObj = *iter;
		pChild->RemoveSizeRectFromNode(pObj);
		pParent->AddSizeRectToNode(pObj);
		iter = pChild->m_lstSizeRect.begin();
	}

	iter = pChild->m_lstViewRect.begin();
	while (iter != pChild->m_lstViewRect.end())
	{
		pObj = *iter;
		pChild->RemoveViewRectFromNode(pObj);
		pParent->AddViewRectToNode(pObj);
		iter = pChild->m_lstViewRect.begin();
	}

}


template<typename Traits>
void TQuadNode<Traits>::MergeChildNodes()
{
	if(m_pNEChildNode)
	{
		ImpQuadNode_t* pParent = static_cast<ImpQuadNode_t*>(this);
		MergeChildNodeData(pParent, m_pNEChildNode);
		MergeChildNodeData(pParent, m_pNWChildNode);
		MergeChildNodeData(pParent, m_pSWChildNode);
		MergeChildNodeData(pParent, m_pSEChildNode);
	}

	DeleteChildNodes();
}


template<typename Traits>
bool TQuadNode<Traits>::BeMergeChildNodesNeeded()
{
	//如果子节点拥有子节点，则不能够合并
	if (m_pNEChildNode->m_pNEChildNode != NULL ||
		m_pNWChildNode->m_pNEChildNode != NULL ||
		m_pSWChildNode->m_pNEChildNode != NULL ||
		m_pSEChildNode->m_pNEChildNode != NULL)
	{
		return false;
	}

	int HasDataChilds = 0;
	if (!m_pNEChildNode->m_lstObject.empty())
	{
		++HasDataChilds;
	}
	if (!m_pNEChildNode->m_lstObject.empty())
	{
		++HasDataChilds;
	}
	if (!m_pNEChildNode->m_lstObject.empty())
	{
		++HasDataChilds;
	}
	if (!m_pNEChildNode->m_lstObject.empty())
	{
		++HasDataChilds;
	}

	if (HasDataChilds >= 2)
	{
		return false;
	}

	return true;
}


template<typename Traits>
float TQuadNode<Traits>::QuadNodeBoxWidth() const
{
	return m_rcQuadNodeBox.right - m_rcQuadNodeBox.left;
}


template<typename Traits>
float TQuadNode<Traits>::QuadNodeBoxHeight() const
{
	return m_rcQuadNodeBox.top - m_rcQuadNodeBox.bottom;
}

