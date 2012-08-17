#pragma once
#include "CPos.h"
#include "CRectangle.h"
#include "CQuadMallocObject.h"
#include "TQuadAllocator.h"

namespace sqr
{
	class CSyncCoreObjectServer;
	class CCoreQuadNode;
	class CCoreQuadScene;
	class CTraitsCoreQuad;

	template<typename Traits>
	class TQuadNode;

	template<typename Traits>
	class TQuadTree;

	template<typename T> struct TRect;
	typedef TRect<float>	CFRect;

	//场景4叉树管理对象
	class CCoreQuadObject : public CQuadMallocObject
	{
	public:
		friend class TQuadNode<CTraitsCoreQuad>;
		friend class CCoreQuadNode;
		friend class TQuadTree<CTraitsCoreQuad>;
		friend class CCoreQuadScene;

		CCoreQuadObject(uint32 uObjID, const CFPos& pos);
		~CCoreQuadObject();

		void InsertToScene(CCoreQuadScene* pScene);
		void RemoveFrScene(CCoreQuadScene* pScene);

		void RawSetPos(const CFPos& pos);
		void SetPos(const CFPos& pos, CCoreQuadScene* pScene);
		void ForceSetPos(const CFPos& pos, CCoreQuadScene* pScene);
		const CFPos& GetPosition() const;

		void SetSize(float fSize, CCoreQuadScene* pScene);
		void RawSetSize(float fSize);

		float GetSize() const;

	private:
		void SetViewRect(const CFPos& NewPos, const CFRect& rcMaxBox);
		const CFRect& GetViewRect() const;

		void SetSizeRect(const CFPos& NewPos, const CFRect& rcMaxBox);
		const CFRect& GetSizeRect() const;

		void SetPosition(const CFPos& pos);
		void IntSetPos(const CFPos& pos, CCoreQuadScene* pScene);

	private:	
		uint32				m_uObjectID;
		CFPos				m_Pos;
		CFRect				m_rcSizeRect;
		float				m_fSize;
		
		CCoreQuadNode*	m_pObjectNode;
		CCoreQuadNode*	m_pViewRectNode;
		CCoreQuadNode*	m_pSizeRectNode;

		typedef list<CCoreQuadObject*, TQuadAllocator<CCoreQuadObject*> >	ListQuadObject_t;

		ListQuadObject_t::iterator	m_ItObject;
		ListQuadObject_t::iterator	m_ItViewRect;
		ListQuadObject_t::iterator	m_ItSizeRect;

		static ListQuadObject_t	ms_DummyList;
	};
}

