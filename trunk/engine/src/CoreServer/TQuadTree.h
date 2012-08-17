#pragma once
#include "CPos.h"
#include "CRectangle.h"
#include "CQuadMallocObject.h"
#include "TQuadAllocator.h"

namespace sqr
{
	template<typename T> struct TRect;
	typedef TRect<float> CFRect;

	template<typename Traits>
	class TQuadNode;

	template<typename Traits>
	class TQuadTree : public CQuadMallocObject
	{
	protected:
		typedef typename Traits::QuadNode		ImpQuadNode_t;
		typedef typename Traits::QuadObject		ImpQuadObject_t;

		friend class TQuadNode<Traits>;

		TQuadTree(uint32 uWidth, uint32 uHeight, uint32 uMinSpliteSize);
		~TQuadTree();

		const CFRect& GetSceneBox() const;  //得到四叉树对应的场景实际的rc box

		void InsertSizeRectToQuad(ImpQuadObject_t* pObj);
		void InsertViewRectToQuad(ImpQuadObject_t* pObj);
		void RemoveSizeRectFrQuad(ImpQuadObject_t* pObj);
		void RemoveViewRectFrQuad(ImpQuadObject_t* pObj);

		void ResetViewRectInQuad(ImpQuadObject_t* pObj, const CFPos& NewPos);
		void ResetSizeRectInQuad(ImpQuadObject_t* pObj, const CFPos& NewPos);
		
		//下面两个函数是对象移动的时候调用的	
		void InsertObjectToQuad(ImpQuadObject_t* pObject);
		void RemoveObjectFrQuad(ImpQuadObject_t* pObject);

		//平面划分策略，看是否需要进行平面切分
		//以后可能会有其他集中划分策略，所以可以采用strategy模式
		bool BeNeedQuadNodeSplited(ImpQuadNode_t* pNode, ImpQuadObject_t* pObject);

		//查找position所在的node
		static ImpQuadNode_t* FindUpQuadNodeWithPos(const CFPos& pos, ImpQuadNode_t* pNode);
		static ImpQuadNode_t* FindDownQuadNodeWithPos(const CFPos& pos, ImpQuadNode_t* pNode);
		
		//查找最小包含rect的node
		static ImpQuadNode_t* FindUpQuadNodeWithRect(const CFRect& rcRect, ImpQuadNode_t* pNode);
		static ImpQuadNode_t* FindDownQuadNodeWithRect(const CFRect& rcRect, ImpQuadNode_t* pNode);

		static ImpQuadNode_t* SelectChildNodeWithPos(const CFPos& pos, ImpQuadNode_t* pNode);
		static ImpQuadNode_t* SelectChildNodeWithRect(const CFRect& rcRect, ImpQuadNode_t* pNode);

	protected:
		ImpQuadNode_t*		m_pRootNode;
		uint32				m_uMinSpliteSize;		//最小划分平面
		CFRect				m_rcSceneBox;

		typedef std::list<ImpQuadObject_t*, TQuadAllocator<ImpQuadObject_t*> > ListQuadObject_t;
	};

}

