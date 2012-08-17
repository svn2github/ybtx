#pragma once 

#include <list>
#include "CRectangle.h"
#include "CPos.h"
#include "CQuadMallocObject.h"
#include "TQuadAllocator.h"

namespace sqr
{
	template<class T> struct TRect;
	typedef TRect<float>	CFRect;

	template<typename Traits>
	class TQuadTree;

	class CMemObjectPool;

	template<typename Traits>
	class TQuadNode : public CQuadMallocObject
	{
	protected:
		typedef typename Traits::QuadTree	ImpQuadTree_t;
		typedef typename Traits::QuadNode	ImpQuadNode_t;
		typedef typename Traits::QuadObject	ImpQuadObject_t;

		friend class TQuadTree<Traits>;
#ifdef _WIN32
		friend ImpQuadTree_t;
#else
		friend class Traits::QuadTree;
#endif

		TQuadNode(ImpQuadNode_t* pParentNode, const CFRect& rcBox);
		~TQuadNode();

		void AddSizeRectToNode(ImpQuadObject_t* pObj);
		void RemoveSizeRectFromNode(ImpQuadObject_t* pObj);

		void AddViewRectToNode(ImpQuadObject_t* pObj);
		void RemoveViewRectFromNode(ImpQuadObject_t* pObj);

		void AddObjectToNode(ImpQuadObject_t* pObj);
		void RemoveObjectFromNode(ImpQuadObject_t* pObj);

		//创建子节点以及删除子节点
		void CreateChildNodes();
		void DeleteChildNodes();

		//归并子节点，把子节点的数据移到父节点上面，并且删除子节点
		//这个函数只会使用在只有一层子节点的父节点上面，因为数据只可能存在于最底层的节点上面
		void MergeChildNodes();
		bool BeMergeChildNodesNeeded();

		void MergeChildNodeData(ImpQuadNode_t* pParent,ImpQuadNode_t* pChild);

		//切分平面后，父节点的一些数据会移动到叶子节点上面
		void SetChildNodesData();	

		float QuadNodeBoxWidth() const;
		float QuadNodeBoxHeight() const;

	protected:
		ImpQuadNode_t*	m_pParentNode;    //父节点

		//四叉树四个子节点
		/*
		NW  |  NE
		---------
		SW  |  SE
		*/
		ImpQuadNode_t*	m_pNEChildNode;
		ImpQuadNode_t*	m_pNWChildNode;
		ImpQuadNode_t*	m_pSWChildNode;
		ImpQuadNode_t*	m_pSEChildNode;

		typedef std::list<ImpQuadObject_t*, TQuadAllocator<ImpQuadObject_t*> > ListQuadObject_t;

		ListQuadObject_t			m_lstObject;		//挂载对象，对象只在最底层子节点才能挂载
		ListQuadObject_t			m_lstViewRect;		//挂载对象view rect，只有view rect完全在该node的box里面才能挂载
		ListQuadObject_t			m_lstSizeRect;		//挂载对象size rect，只有size rect完全在该node的box里面才能挂载
		

		CFRect				m_rcQuadNodeBox;		//该节点对应平面矩形
	};
		

}

