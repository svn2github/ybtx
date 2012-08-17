#pragma once
#include "CPathDirectNode.h"
#include "CFindPathAllocator.h"

namespace sqr
{

	class CPathDirect
		:public CFindPathMallocObject
	{
	public:
		typedef vector<CPathDirectNode,CFindPathAllocator<CPathDirectNode> >		DirectNodeList_t;	

		CPathDirect();
		void Clear();
		void AddPos(int x, int y, float nLength);
		uint32	GetPosNum();
		CPathDirectNode* GetDirectNode(uint32 nIndex);

		void SetNearSize(float fNearSize);
		float GetNearSize();

		void ResetEndPos(const CPos& pos);
		const CPos& GetEndPos();
	private:
		DirectNodeList_t m_VecNodeList;
		float	m_fNearSize;	//认为接近目标点的距离
	};
}

