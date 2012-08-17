#pragma once 
#include "CPos.h"
#include "CQuadMallocObject.h"
#include "TQuadAllocator.h"

namespace sqr
{
	class CAsyAoiScene;
	class CAsyAoiDimObj;

	class CAsyAoiDimScene : public CQuadMallocObject
	{
		friend class CAsyAoiDimObj;

	public:
		CAsyAoiDimScene(CAsyAoiScene* pScene, uint32 uPrecision);
		~CAsyAoiDimScene();

		void Release();
		
		bool IsInSyncAoi(const CPos& OffsetGridPos)const;
		uint32	GetPrecision() const;
		void SetPrecision(uint32 uDimension);

	private:
		CAsyAoiScene*	m_pScene;
		uint32			m_uPrecision;
	};
}

