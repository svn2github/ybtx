#pragma once 
#include "CAoiQuadTree.h"

namespace sqr
{
	class CSynAoiScene;

	template<class T> struct TRect;
	typedef TRect<float>	 CFRect;

	template<class T> struct TPos;
	typedef TPos<float> CFPos;

	class CAoiQuadTree;

	class CSynAoiScene;
	class CSynAoiDimObj;

	class CSynAoiDimScene : public CAoiQuadTree
	{
		friend class CSynAoiDimObj;

	public:
		CSynAoiDimScene(CSynAoiScene* pScene, 
			uint32 uGridWidth, uint32 uGridHeight, uint32 uPrecision);
		~CSynAoiDimScene();

		void Release();

		bool IsInSyncAoi(const CFPos& OffsetGridPos)const;

		void SetPrecision(uint32 uPrecision);
		uint32	GetPrecision() const;

	private:
		CSynAoiScene*	m_pScene;
		uint32			m_uPrecison;
	};

}

