#pragma once
#include "Cmd.h"
#include "TRefPointer.h"
#include "CVector3.h"

namespace sqr
{
	SmartPointer(CTObject);

	class CCmdMoveObject : public CCmd
	{
		struct SDiffMoveInfo
		{
			// two way real-time params
			CTObjectPtr pObject;

			float fRotation;
			float fShiftXEx;
			float fShiftZEx;

			CVector3f vGroupOffset;

			// memento
			int nCenterGridIndex;
			SDiffMoveInfo() : nCenterGridIndex(-1), pObject(NULL) { }
		};

		SDiffMoveInfo m_diff;
		int m_nOffset;

	public:

		CCmdMoveObject()
		{
			m_nOffset = 0;
		}

		~CCmdMoveObject()
		{
		}

		virtual void _Execute();
		virtual void _UnExecute();

		void SetObject( CTObject * p)
		{
			m_diff.pObject = p;
		}

		void SetCenterGridIndex(int n)
		{
			m_diff.nCenterGridIndex = n;
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

		void SetGroupOffset(CVector3f vGroupOffset)
		{
			m_diff.vGroupOffset = vGroupOffset;
		}

		bool IsMoving()
		{
			return true;
		};
	};
}