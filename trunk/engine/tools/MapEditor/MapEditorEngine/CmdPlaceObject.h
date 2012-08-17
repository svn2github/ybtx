#pragma once
#include "TRefPointer.h"
#include "Cmd.h"
#include "CVector3.h"

namespace sqr
{
	SmartPointer(CTObject);

	class CCmdPlaceObject : public CCmd
	{
		struct SDiffPlaceInfo
		{
			// real-time params
			int nHotGridIndex;
			CTObjectPtr pObject;

			CVector3f vGroupOffset;

// 			// memento params
// 			vector<int> vecBlockGridIndices;

			SDiffPlaceInfo() : nHotGridIndex(-1) { }
			~SDiffPlaceInfo() {}
		};

		SDiffPlaceInfo m_diff;
		int m_nOffset;

		virtual void _Execute();
		virtual void _UnExecute();
	public:
		CCmdPlaceObject()
		{
			m_nOffset = 0;
		}

		~CCmdPlaceObject() {	}


		void SetObject(CTObject * p)
		{
			m_diff.pObject = p;
		}

		void SetHotGridIndex(int n)
		{
			m_diff.nHotGridIndex = n;
		}

		void SetGroupOffset(CVector3f vGroupOffset)
		{
			m_diff.vGroupOffset = vGroupOffset;
		}
	};
}