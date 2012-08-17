#pragma once
#include "TQuadTree.h"
#include "QuadTreeFun.h"
#include "TQuadNode.inl"
#include "ExpHelper.h"

template<typename Traits>
TQuadTree<Traits>::TQuadTree(uint32 uWidth, uint32 uHeight, uint32 uMinSpliteSize)
: m_uMinSpliteSize(uMinSpliteSize)
, m_rcSceneBox(0, (float)uHeight, (float)uWidth, 0)
{
	const CFRect& rcBox = GetQuadRect(uWidth, uHeight);
	m_pRootNode = new ImpQuadNode_t(NULL, rcBox);
}

template<typename Traits>
TQuadTree<Traits>::~TQuadTree()
{
	delete m_pRootNode;
}

template<typename Traits>
const CFRect& TQuadTree<Traits>::GetSceneBox() const
{
	return m_rcSceneBox;
}

//--------------------------------------------------------------------

//第一次设置size调用，加入size rect的同时我们要加入obj

template<typename Traits>
void TQuadTree<Traits>::InsertSizeRectToQuad(ImpQuadObject_t* pObject)
{
	const CFPos& pos = pObject->GetPosition();

	ImpQuadNode_t* pQuadNode;

	if (pObject->m_pViewRectNode)
	{
		pQuadNode = FindDownQuadNodeWithPos(pos, pObject->m_pViewRectNode);
	}
	else
	{
		pQuadNode = FindDownQuadNodeWithPos(pos, m_pRootNode);
	}

	//看是否需要划分平面
	//不需要，直接挂载
	//需要，划分平面，把原先节点上面的对象重新挂载
	//再次判断是否需要划分平面，
	bool bSplited = false;
	while (BeNeedQuadNodeSplited(pQuadNode, pObject))
	{
		//创建子节点
		bSplited = true;
		pQuadNode->CreateChildNodes();
		pQuadNode = FindDownQuadNodeWithPos(pos, pQuadNode);
	}

	//将对象加入quad node的set中
	pQuadNode->AddObjectToNode(pObject);

	//插入size rect，跟obj对象一起插入
	const CFRect& rcSizeRect = pObject->GetSizeRect();
	ImpQuadNode_t* pSizeNode = FindUpQuadNodeWithRect(rcSizeRect, pQuadNode);
	pSizeNode->AddSizeRectToNode(pObject);

	//如果view rect存在，并且平面进行了划分，view rect可能进入子平面了
	if (pObject->m_pViewRectNode && bSplited)
	{
		const CFRect& rect = pObject->GetViewRect();
		if (BeInQuadNodeBox(rect, pQuadNode->m_rcQuadNodeBox))
		{
			pObject->m_pViewRectNode->RemoveViewRectFromNode(pObject);
			pQuadNode->AddViewRectToNode(pObject);
		}
	}
}

template<typename Traits>
void TQuadTree<Traits>::RemoveSizeRectFrQuad(ImpQuadObject_t* pObject)
{	
	ImpQuadNode_t* pSizeNode = pObject->m_pSizeRectNode;
	pSizeNode->RemoveSizeRectFromNode(pObject);

	ImpQuadNode_t* pNode = pObject->m_pObjectNode;
	pNode->RemoveObjectFromNode(pObject);

	ImpQuadNode_t* pParent =  pNode->m_pParentNode;

	if (pParent == NULL)
	{
		return;
	}

	bool bMergeNeeded = pParent->BeMergeChildNodesNeeded();

	if (bMergeNeeded)
	{
		pParent->MergeChildNodes();
	}
}

//----------------------------------------------------------------------------

//第一次设置eye sight调用

template<typename Traits>
void TQuadTree<Traits>::InsertViewRectToQuad(ImpQuadObject_t* pObject)
{
	const CFRect& rcObjRect = pObject->GetViewRect();

	//够包裹view rect的最小节点
	ImpQuadNode_t* pNode;
	if (pObject->m_pObjectNode)
	{
		pNode = FindUpQuadNodeWithRect(rcObjRect, pObject->m_pObjectNode);
	}
	else
	{
		pNode = FindDownQuadNodeWithRect(rcObjRect, m_pRootNode);
	}

	//将对象view rect加入quad node中
	pNode->AddViewRectToNode(pObject);
}


template<typename Traits>
void TQuadTree<Traits>::RemoveViewRectFrQuad(ImpQuadObject_t* pObject)
{
	ImpQuadNode_t* pNode = pObject->m_pViewRectNode;
	pNode->RemoveViewRectFromNode(pObject);
}


//----------------------------------------------------------

template<typename Traits>
void TQuadTree<Traits>::InsertObjectToQuad(ImpQuadObject_t* pObject)
{
	const CFPos& pos = pObject->GetPosition();

	//找到object坐标对应的叶子节点
	ImpQuadNode_t* pQuadNode = pObject->m_pObjectNode;

	/*
	如果obj node存在，这时候的node是指向移动前的节点的
	首先从先前节点向上找到包含新的pos的节点，然后在从该节点
	向下查找到能够包含pos的叶子节点
	*/
	pQuadNode = FindUpQuadNodeWithPos(pos, pQuadNode);
	pQuadNode = FindDownQuadNodeWithPos(pos, pQuadNode);


	while (BeNeedQuadNodeSplited(pQuadNode, pObject))
	{
		//创建子节点
		pQuadNode->CreateChildNodes();
		pQuadNode = FindDownQuadNodeWithPos(pos, pQuadNode);
	}

	//将对象加入quad node的set中
	pQuadNode->AddObjectToNode(pObject);

	ResetSizeRectInQuad(pObject, pos);
	ResetViewRectInQuad(pObject, pos);
}


template<typename Traits>
void TQuadTree<Traits>::RemoveObjectFrQuad(ImpQuadObject_t* pObject)
{
	//首先把obj从node节点中删除
	ImpQuadNode_t* pNode = pObject->m_pObjectNode;
	pNode->RemoveObjectFromNode(pObject);

	//这里我们把obj node设置为pNode，因为等下算移动之后插入节点的时候要根据
	//原先所在pNode的节点向上查找
	pObject->m_pObjectNode = pNode;

	ImpQuadNode_t* pParent =  pNode->m_pParentNode;

	if (pParent == NULL)
	{
		return;
	}

	//然后判断删除obj之后该node的父节点是否需要归并子节点
	bool bMergeNeeded = pParent->BeMergeChildNodesNeeded();

	if (bMergeNeeded)
	{
		//把obj node指向parent节点
		//因为随后我们需要通过这个节点向上查找包裹新的pos的节点
		pObject->m_pObjectNode = pParent;

		pParent->MergeChildNodes();
	}

}


template<typename Traits>
void TQuadTree<Traits>::ResetSizeRectInQuad(ImpQuadObject_t* pObject, const CFPos& NewPos)
{
	ImpQuadNode_t* pSizeNode = pObject->m_pSizeRectNode;
	if (!pSizeNode)
		return;

	pObject->SetSizeRect(NewPos, GetSceneBox());

	const CFRect& NewRect = pObject->GetSizeRect();

	//如果新的size rect也仍然在原先的节点内
	if (BeInQuadNodeBox(NewRect, pSizeNode->m_rcQuadNodeBox))
	{

		//size rect可能进入了该节点的子节点
		ImpQuadNode_t* pChildNode = FindDownQuadNodeWithRect(NewRect, pSizeNode);
		if (pChildNode == pSizeNode)
		{
			return;
		}
		else
		{
			pSizeNode->RemoveSizeRectFromNode(pObject);
			pChildNode->AddSizeRectToNode(pObject);		
			return;
		}

	}
	else
	{
		pSizeNode->RemoveSizeRectFromNode(pObject);
		//首先沿着我本来节点向上查找知道能包裹住NewRect的节点
		pSizeNode = FindUpQuadNodeWithRect(NewRect, pSizeNode);
		//然后从该节点向下查找
		pSizeNode = FindDownQuadNodeWithRect(NewRect, pSizeNode);

		pSizeNode->AddSizeRectToNode(pObject);
		return;
	}	
}

template<typename Traits>
void TQuadTree<Traits>::ResetViewRectInQuad(ImpQuadObject_t* pObject, const CFPos& NewPos)
{
	ImpQuadNode_t* pViewNode = pObject->m_pViewRectNode;
	if (!pViewNode)
		return;

	pObject->SetViewRect(NewPos, GetSceneBox());

	const CFRect& NewRect = pObject->GetViewRect();

	//如果新的view rect也仍然在原先的节点内
	if (BeInQuadNodeBox(NewRect, pViewNode->m_rcQuadNodeBox))
	{

		//view rect可能进入了该节点的子节点
		ImpQuadNode_t* pChildNode = FindDownQuadNodeWithRect(NewRect, pViewNode);
		if (pChildNode == pViewNode)
		{
			return;
		}
		else
		{
			pViewNode->RemoveViewRectFromNode(pObject);
			pChildNode->AddViewRectToNode(pObject);		
			return;
		}

	}
	else
	{
		pViewNode->RemoveViewRectFromNode(pObject);
		//首先沿着我本来节点向上查找知道能包裹住NewRect的节点
		pViewNode = FindUpQuadNodeWithRect(NewRect, pViewNode);
		//然后从该节点向下查找
		pViewNode = FindDownQuadNodeWithRect(NewRect, pViewNode);

		pViewNode->AddViewRectToNode(pObject);
		return;
	}
}


//------------------------------------------------------------------
template<typename Traits>
typename TQuadTree<Traits>::ImpQuadNode_t* TQuadTree<Traits>::FindUpQuadNodeWithPos(const CFPos& pos, ImpQuadNode_t* pNode)
{
	while (pNode)
	{
		if (BeInQuadNodeBox(pos, pNode->m_rcQuadNodeBox))
		{
			return pNode;
		}
		pNode = pNode->m_pParentNode;
	}
	
	ostringstream strm;
	strm << "pos : (" << pos.x << ", " << pos.y << " )" << endl;
	GenErr("pos 超出范围", strm.str().c_str());
	return NULL;
}

template<typename Traits>
typename TQuadTree<Traits>::ImpQuadNode_t* TQuadTree<Traits>::SelectChildNodeWithPos(const CFPos& pos, ImpQuadNode_t* pNode)
{
	if(pNode->m_pNEChildNode)
	{
		if (pos.x > pNode->m_pNEChildNode->m_rcQuadNodeBox.left)
		{
			if (pos.y >= pNode->m_pNEChildNode->m_rcQuadNodeBox.bottom)
			{
				return pNode->m_pNEChildNode;
			}
			else
			{
				return pNode->m_pSEChildNode;
			}
		}
		else if(pos.x <= pNode->m_pNWChildNode->m_rcQuadNodeBox.right)
		{
			if (pos.y >= pNode->m_pNWChildNode->m_rcQuadNodeBox.bottom)
			{
				return pNode->m_pNWChildNode;
			}
			else
			{
				return pNode->m_pSWChildNode;
			}
		}

	}
	return NULL;
}

template<typename Traits>
typename TQuadTree<Traits>::ImpQuadNode_t* TQuadTree<Traits>::FindDownQuadNodeWithPos(const CFPos& pos, ImpQuadNode_t* pNode)
{
	ImpQuadNode_t* pSelectNode = pNode;
	ImpQuadNode_t* pChildNode = pNode;

	if (BeInQuadNodeBox(pos, pNode->m_rcQuadNodeBox))
	{
		if(pNode->m_pNEChildNode)
		{
			while(pSelectNode)
			{
				pChildNode = pSelectNode;
				pSelectNode = SelectChildNodeWithPos(pos, pSelectNode);
			}
			if(pChildNode != pNode)
				return pChildNode;
		}

		return pNode;
	}
	

	//如果找不到，则向上查找
	return FindUpQuadNodeWithPos(pos, pNode->m_pParentNode);
}

//-------------------------------------------------------------------

template<typename Traits>
typename TQuadTree<Traits>::ImpQuadNode_t* TQuadTree<Traits>::FindUpQuadNodeWithRect(const CFRect& rcRect, ImpQuadNode_t* pNode)
{
	while (pNode)
	{
		if (BeInQuadNodeBox(rcRect, pNode->m_rcQuadNodeBox))
		{
			return pNode;
		}
		pNode = pNode->m_pParentNode;
	}

	GenErr("view rect超出范围");
	return NULL;
}

template<typename Traits>
typename TQuadTree<Traits>::ImpQuadNode_t* TQuadTree<Traits>::SelectChildNodeWithRect(const CFRect& rcRect, ImpQuadNode_t* pNode)
{
	if(pNode->m_pNEChildNode)
	{
		if (rcRect.left > pNode->m_pNEChildNode->m_rcQuadNodeBox.left)
		{
			if (rcRect.bottom >= pNode->m_pNEChildNode->m_rcQuadNodeBox.bottom)
			{
				return pNode->m_pNEChildNode;
			}
			else if(rcRect.top < pNode->m_pNEChildNode->m_rcQuadNodeBox.bottom)
			{
				return pNode->m_pSEChildNode;
			}
		}
		else if(rcRect.right <= pNode->m_pNWChildNode->m_rcQuadNodeBox.right)
		{
			if (rcRect.bottom >= pNode->m_pNWChildNode->m_rcQuadNodeBox.bottom)
			{
				return pNode->m_pNWChildNode;
			}
			else if(rcRect.top < pNode->m_pNWChildNode->m_rcQuadNodeBox.bottom)
			{
				return pNode->m_pSWChildNode;
			}
		}
	}
	return NULL;
}

template<typename Traits>
typename TQuadTree<Traits>::ImpQuadNode_t* TQuadTree<Traits>::FindDownQuadNodeWithRect(const CFRect& rcRect, ImpQuadNode_t* pNode) 
{
	ImpQuadNode_t* pSelectNode = pNode;
	ImpQuadNode_t* pChildNode = pNode;

	if (BeInQuadNodeBox(rcRect, pNode->m_rcQuadNodeBox))
	{
		if(pNode->m_pNEChildNode)
		{
			while(pSelectNode)
			{
				pChildNode = pSelectNode;
				pSelectNode = SelectChildNodeWithRect(rcRect, pSelectNode);
			}
			if(pChildNode != pNode)
				return pChildNode;
		}
		
		return pNode;
	}


	//如果node不存在，则向上查找
	return FindUpQuadNodeWithRect(rcRect, pNode->m_pParentNode);
}


//-------------------------------------------------------------------
template<typename Traits>
bool TQuadTree<Traits>::BeNeedQuadNodeSplited(ImpQuadNode_t* pNode, ImpQuadObject_t* pObject)
{
	if (pNode->QuadNodeBoxWidth() <= m_uMinSpliteSize || 
		pNode->QuadNodeBoxHeight() <= m_uMinSpliteSize)
	{
		return false;
	}


	//节点没有对象，直接挂载，不需要切分
	if (pNode->m_lstObject.empty())
	{
		return false;
	}
	else
	{
		float fXCenter = pNode->m_rcQuadNodeBox.left + 
			(pNode->m_rcQuadNodeBox.right - pNode->m_rcQuadNodeBox.left) / 2;

		const CFPos& pos = pObject->GetPosition();
		int iXPos = (int)pos.x / m_uMinSpliteSize;
		int iYPos = (int)pos.y / m_uMinSpliteSize;
		//如果aoi坐标在节点矩形的right或者中线上面上面，则属于这个节点
		if (pos.x == pNode->m_rcQuadNodeBox.right || pos.x == fXCenter)
			iXPos = iXPos - 1;
	
		typename ListQuadObject_t::const_iterator iter = pNode->m_lstObject.begin();
		const CFPos& OtPos = (*iter)->GetPosition();
		int iOtXPos = (int)OtPos.x / m_uMinSpliteSize;
		int iOtYPos = (int)OtPos.y / m_uMinSpliteSize;

		//如果aoi坐标在节点矩形的right或者中线上面上面，则属于这个节点
		if (OtPos.x == pNode->m_rcQuadNodeBox.right || OtPos.x == fXCenter)
			iOtXPos = iOtXPos - 1;
	
		if (iXPos == iOtXPos && iYPos == iOtYPos)
			return false;
		else
			return true;

		return false;
	}
}


