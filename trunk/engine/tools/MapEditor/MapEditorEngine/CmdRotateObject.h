#pragma once
#include "TRefPointer.h"
#include "Cmd.h"

namespace sqr
{
	SmartPointer(CTObject);

	class CCmdRotateObject : public CCmd
	{
		struct SDiffRotateInfo
		{
			CTObjectPtr pObject;

			float fRotation;
			float fShiftXEx;
			float fShiftZEx;
		};

		SDiffRotateInfo m_diff;

	public:
		CCmdRotateObject() { }

		~CCmdRotateObject() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void SetObject( CTObject * p)
		{
			m_diff.pObject = p;
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