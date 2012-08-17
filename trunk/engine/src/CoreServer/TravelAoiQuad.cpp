#include "stdafx.h"
#include "CAoiQuadTree.h"
#include "QuadTreeFun.h"
#include "CSynAoiDimObj.h"
#include "CAoiQuadNode.h"
#include "TSqrAllocator.inl"

//-------------------------------------------------------------

void CAoiQuadTree::TravelDownQuadForObservee(CAoiQuadNode* pNode, VecObject_t& vecObj, const CFRect& rect)
{
	if (pNode == NULL)
	{
		return;
	}

	if (!BeOverlappedQuadNode(pNode->m_rcQuadNodeBox, rect))
	{
		return;
	}

	ListQuadObject_t::const_iterator iter = pNode->m_lstSizeRect.begin();
	ListQuadObject_t::const_iterator iter_end = pNode->m_lstSizeRect.end();

	while(iter != iter_end)
	{
		if(BeOverlappedQuadNode((*iter)->GetSizeRect(), rect))
		{
			vecObj.push_back(*iter);
		}
		++iter;
	}

	if(!pNode->m_pNEChildNode)
		return;

	TravelDownQuadForObservee(pNode->m_pNEChildNode, vecObj, rect);
	TravelDownQuadForObservee(pNode->m_pNWChildNode, vecObj, rect);
	TravelDownQuadForObservee(pNode->m_pSWChildNode, vecObj, rect);
	TravelDownQuadForObservee(pNode->m_pSEChildNode, vecObj, rect);

}

void CAoiQuadTree::TravelUpQuadForObservee(CAoiQuadNode* pNode, VecObject_t&vecObj, const CFRect& rect)
{
	while(pNode)
	{
		ListQuadObject_t::const_iterator iter = pNode->m_lstSizeRect.begin();
		ListQuadObject_t::const_iterator iter_end = pNode->m_lstSizeRect.end();
		while(iter != iter_end)
		{
			if(BeOverlappedQuadNode((*iter)->GetSizeRect(), rect))
			{
				vecObj.push_back(*iter);
			}
			++iter;
		}
		pNode = pNode->m_pParentNode;
	}
}

void CAoiQuadTree::TravelQuadForObservee(CAoiQuadNode* pNode, VecObject_t&vecObj, const CFRect& rect)
{
	//首先向下查找
	TravelDownQuadForObservee(pNode, vecObj, rect);

	//然后向上
	TravelUpQuadForObservee(pNode->m_pParentNode, vecObj, rect);
}

//---------------------------------------------------------------------

void CAoiQuadTree::TravelDownQuadForObservee(CAoiQuadNode* pNode, VecObject_t&vecObj, 
												  const CFRect& MyOldRect, const CFRect& MyNewRect) 
{
	if (pNode == NULL)
	{
		return;
	}

	if (!BeOverlappedQuadNode(pNode->m_rcQuadNodeBox, MyOldRect))
	{
		return;
	}

	ListQuadObject_t::const_iterator iter = pNode->m_lstSizeRect.begin();
	ListQuadObject_t::const_iterator iter_end = pNode->m_lstSizeRect.end();
	while(iter != iter_end)
	{
		const CFRect& rcRect = (*iter)->GetSizeRect();
		if(BeOverlappedQuadNode(rcRect, MyOldRect) &&
			! BeOverlappedQuadNode(rcRect, MyNewRect))
		{
			vecObj.push_back(*iter);
		}
		++iter;
	}

	if(!pNode->m_pNEChildNode)
		return;

	TravelDownQuadForObservee(pNode->m_pNEChildNode, vecObj, MyOldRect, MyNewRect);
	TravelDownQuadForObservee(pNode->m_pNWChildNode, vecObj, MyOldRect, MyNewRect);
	TravelDownQuadForObservee(pNode->m_pSWChildNode, vecObj, MyOldRect, MyNewRect);
	TravelDownQuadForObservee(pNode->m_pSEChildNode, vecObj, MyOldRect, MyNewRect);
}

void CAoiQuadTree::TravelUpQuadForObservee(CAoiQuadNode* pNode, VecObject_t&vecObj, 
												  const CFRect& MyOldRect, const CFRect& MyNewRect) 
{
	while(pNode)
	{
		ListQuadObject_t::const_iterator iter = pNode->m_lstSizeRect.begin();
		ListQuadObject_t::const_iterator iter_end = pNode->m_lstSizeRect.end();
		while(iter != iter_end)
		{
			const CFRect& rcRect = (*iter)->GetSizeRect();
			if(BeOverlappedQuadNode(rcRect, MyOldRect) &&
				! BeOverlappedQuadNode(rcRect, MyNewRect))
			{
				vecObj.push_back(*iter);
			}
			++iter;
		}
		pNode = pNode->m_pParentNode;
	}
}

void CAoiQuadTree::TravelQuadForObservee(CAoiQuadNode* pNode, VecObject_t&vecObj, 
												  const CFRect& MyOldRect, const CFRect& MyNewRect) 
{
	TravelDownQuadForObservee(pNode, vecObj, MyOldRect, MyNewRect);
	TravelUpQuadForObservee(pNode->m_pParentNode, vecObj, MyOldRect, MyNewRect);
}

//-------------------------------------------------------------------

void CAoiQuadTree::TravelDownQuadForObserver(CAoiQuadNode* pNode, VecObject_t&vecObj, const CFRect& rect)
{
	if (pNode == NULL)
	{
		return;
	}

	if (!BeOverlappedQuadNode(rect, pNode->m_rcQuadNodeBox))
	{
		return;
	}

	ListQuadObject_t::const_iterator iter = pNode->m_lstViewRect.begin();
	ListQuadObject_t::const_iterator iter_end = pNode->m_lstViewRect.end();
	while(iter != iter_end)
	{
		if(BeOverlappedQuadNode(rect, (*iter)->GetViewRect()))
		{
			vecObj.push_back(*iter);
		}
		++iter;
	}

	if(!pNode->m_pNEChildNode)
		return;

	TravelDownQuadForObserver(pNode->m_pNEChildNode, vecObj, rect);
	TravelDownQuadForObserver(pNode->m_pNWChildNode, vecObj, rect);
	TravelDownQuadForObserver(pNode->m_pSWChildNode, vecObj, rect);
	TravelDownQuadForObserver(pNode->m_pSEChildNode, vecObj, rect);

}

void CAoiQuadTree::TravelUpQuadForObserver(CAoiQuadNode* pNode, VecObject_t&vecObj, const CFRect& rect)
{
	while(pNode)
	{
		ListQuadObject_t::const_iterator iter = pNode->m_lstViewRect.begin();
		ListQuadObject_t::const_iterator iter_end = pNode->m_lstViewRect.end();
		while(iter != iter_end)
		{
			if(BeOverlappedQuadNode(rect, (*iter)->GetViewRect()))
			{
				vecObj.push_back(*iter);
			}
			++iter;
		}
		pNode = pNode->m_pParentNode;
	}
}

void CAoiQuadTree::TravelQuadForObserver(CAoiQuadNode* pNode, VecObject_t&vecObj, const CFRect& rect)
{
	//首先向下查找
	TravelDownQuadForObserver(pNode, vecObj, rect);

	//然后向上
	TravelUpQuadForObserver(pNode->m_pParentNode, vecObj, rect);
}

//-------------------------------------------------------------------

void CAoiQuadTree::TravelDownQuadForObserver(CAoiQuadNode* pNode, VecObject_t&vecObj, 
												  const CFRect& MyOldRect, const CFRect& MyNewRect) 
{
	if (pNode == NULL)
	{
		return;
	}

	if (!BeOverlappedQuadNode(MyOldRect, pNode->m_rcQuadNodeBox))
	{
		return;
	}

	ListQuadObject_t::const_iterator iter = pNode->m_lstViewRect.begin();
	ListQuadObject_t::const_iterator iter_end = pNode->m_lstViewRect.end();
	while(iter != iter_end)
	{
		const CFRect& rcRect = (*iter)->GetViewRect();
		if(BeOverlappedQuadNode(MyOldRect, rcRect) &&
			! BeOverlappedQuadNode(MyNewRect, rcRect))
		{
			vecObj.push_back(*iter);
		}
		++iter;
	}

	if(!pNode->m_pNEChildNode)
		return;

	TravelDownQuadForObserver(pNode->m_pNEChildNode, vecObj, MyOldRect, MyNewRect);
	TravelDownQuadForObserver(pNode->m_pNWChildNode, vecObj, MyOldRect, MyNewRect);
	TravelDownQuadForObserver(pNode->m_pSWChildNode, vecObj, MyOldRect, MyNewRect);
	TravelDownQuadForObserver(pNode->m_pSEChildNode, vecObj, MyOldRect, MyNewRect);
}

void CAoiQuadTree::TravelUpQuadForObserver(CAoiQuadNode* pNode, VecObject_t&vecObj, 
												  const CFRect& MyOldRect, const CFRect& MyNewRect) 
{
	while(pNode)
	{
		ListQuadObject_t::const_iterator iter = pNode->m_lstViewRect.begin();
		ListQuadObject_t::const_iterator iter_end = pNode->m_lstViewRect.end();
		while(iter != iter_end)
		{
			const CFRect& rcRect = (*iter)->GetViewRect();
			if(BeOverlappedQuadNode(MyOldRect, rcRect) &&
				! BeOverlappedQuadNode(MyNewRect, rcRect))
			{
				vecObj.push_back(*iter);
			}
			++iter;
		}
		pNode = pNode->m_pParentNode;
	}
}

void CAoiQuadTree::TravelQuadForObserver(CAoiQuadNode* pNode, VecObject_t&vecObj, 
												  const CFRect& MyOldRect, const CFRect& MyNewRect) 
{
	TravelDownQuadForObserver(pNode, vecObj, MyOldRect, MyNewRect);
	TravelUpQuadForObserver(pNode->m_pParentNode, vecObj, MyOldRect, MyNewRect);
}

