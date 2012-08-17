#pragma once
#include "Cmd.h"
#include "TRefPointer.h"


namespace sqr
{
	SmartPointer(CTObject);

	class CCmdLiftMoveObject : public CCmd
	{
		struct SDiffLiftMoveInfo
		{
			// real time params
			CTObjectPtr pObject;

			float fShiftXEx;
			float fShiftZEx;

			// memento
			int nCenterGridIndex;

			SDiffLiftMoveInfo() : nCenterGridIndex(-1), pObject(NULL) {}
			~SDiffLiftMoveInfo() {}
		};

		SDiffLiftMoveInfo m_diff;

	public:
		CCmdLiftMoveObject()
		{
			;
		}

		~CCmdLiftMoveObject() { }

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
	};
}