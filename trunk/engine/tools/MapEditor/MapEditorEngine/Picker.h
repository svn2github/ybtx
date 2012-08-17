#pragma once
#include "TRefPointer.h"
#include "TSingleton.h"
#include "CVector3.h"

namespace sqr
{
	SmartPointer(CTObject);
	class CMapEditObject;

	class CPicker : public Singleton<CPicker>
	{
		CVector3f m_vecRay[2]; // ori and dir
		CTObjectPtr m_pLastSelected;
		CMapEditObject *m_pMapEditObject;
		set<int> m_vecGridIndices;

	public:

		CPicker();
		virtual ~CPicker();

		int   GetPickPos(CTObject ** pp, CVector3f * pvHitPoint, vector< CTObject * > * pVecObjectOlds = NULL);
		float GetHeight(float fX, float fZ);
		bool  GetIntersected(CVector3f & vHitPoint, int * pnGridIndexIntersected = NULL);
		void  Evaluate(CVector3f vStart, CVector3f vEnd, DWORD dwSearchCnt, vector<DWORD> & vecGridIndices);
		void  PickSceneObject(const DWORD& gridindex);
	};
}