#pragma once
#include "TRefPointer.h"
#include "Cmd.h"

namespace sqr
{
	SmartPointer(CTObject);

	class CCmdDeleteObject : public CCmd
	{
		struct SDiffDeleteInfo
		{
			// real time params
			CTObjectPtr pObject;

			float fRotation;
			float fShiftXEx;
			float fShiftZEx;

			// memento
			int nCenterGridIndex;
			vector<int> vecBlockGridIndices;
			vector<DWORD> vecHeightVertexIndices;
			SDiffDeleteInfo() : nCenterGridIndex(-1), pObject(NULL) {}
			~SDiffDeleteInfo() {}
		};

		SDiffDeleteInfo m_diff;
		int m_nOffset;

	public:
		CCmdDeleteObject()
		{
			m_nOffset = 0;
		}

		~CCmdDeleteObject() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void SetObject(CTObject * p)
		{
			m_diff.pObject = p;
		}

		void SetCenterGridIndex(int n)
		{
			m_diff.nCenterGridIndex = n;
		}

		void AddBlockGridIndex(int n)
		{
			m_diff.vecBlockGridIndices.push_back(n);
		}

		void AddVertexIndex(DWORD dw)
		{
			if ( dw == -1 )
			{
				m_diff.vecHeightVertexIndices.push_back(dw);
				m_nOffset = m_diff.vecHeightVertexIndices.end() - m_diff.vecHeightVertexIndices.begin();
			}
			else if ( find(m_diff.vecHeightVertexIndices.begin()+m_nOffset,m_diff.vecHeightVertexIndices.end(),dw) == m_diff.vecHeightVertexIndices.end() )
			{
				m_diff.vecHeightVertexIndices.push_back(dw);
			}
		}

		void SetShiftXEx(float fShiftXEx)
		{
			m_diff.fShiftXEx = fShiftXEx;
		}

		void SetShiftZEx(float fShiftZEx)
		{
			m_diff.fShiftZEx = fShiftZEx;
		}

		void SetRotation(float fRotation)
		{
			m_diff.fRotation = fRotation;
		}
	};
}