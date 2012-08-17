#pragma once
#include "TQuadTree.h"
#include "CTraitsCoreQuad.h"

namespace sqr
{
	class CCoreObjectServer;

	class CCoreQuadScene : public TQuadTree<CTraitsCoreQuad>
	{
	public:	
		typedef TQuadTree<CTraitsCoreQuad>	Parent_t;
		friend class CCoreQuadObject;

		CCoreQuadScene(uint32 uWidth, uint32 uHeight, uint32 uPrecision);
		~CCoreQuadScene();
	
		void QueryObject(void (*pFunc)(void*, uint32), void* pVecObject,const CFPos& pos, float fGridRadius)const;
		void QueryObject(void (*pFunc)(void*, uint32), void* pVecObject, CCoreQuadObject* pObj, const CFPos& OffsetPos, float fRadius) const;

		uint32 GetPrecision() const;

	private:
		void SetObjectPos(CCoreQuadObject* pObject, const CFPos& NewPos);
		/*void InsertSizeRectToQuad(CCoreQuadObject* pObj);
		void RemoveSizeRectFrQuad(CCoreQuadObject* pObj);
		void ResetSizeRectInQuad(CCoreQuadObject* pObj, const CFPos& NewPos);*/

	private:
		static void TravelUpQuadForObject(CCoreQuadNode* pNode, void (*pFunc)(void*, uint32), void* pVecObject, const CFPos& pos, float fGridRadius, const CFRect& rect);	
		static void TravelDownQuadForObject(CCoreQuadNode* pNode, void (*pFunc)(void*, uint32), void* pVecObject, const CFPos& pos, float fGridRadius, const CFRect& rect);	
		static void TravelQuadForObject(CCoreQuadNode* pNode, void (*pFunc)(void*, uint32), void* pVecObject, const CFPos& pos, float fGridRadius, const CFRect& rect);	
	
	private:
		uint32		m_uPrecision;

		typedef list<CCoreQuadObject*, TQuadAllocator<CCoreQuadObject*> >	ListQuadObject_t;
	};

}
