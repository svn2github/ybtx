#pragma once
#include "TRefPointer.h"
#include "Cmd.h"

namespace sqr
{
	SmartPointer(CTObject);

	class CCmdHideObject : public CCmd
	{
		struct SHideModelInfo
		{
			// real time params
			CTObjectPtr pObject;

			bool  bHide;

			SHideModelInfo() : pObject(NULL), bHide(false) {}
			~SHideModelInfo() {}
		};

		SHideModelInfo m_diff;

	public:
		CCmdHideObject()
		{
			;
		}

		~CCmdHideObject() { }

		virtual void _Execute();
		virtual void _UnExecute();

		void SetObject(CTObject * p);

		void SetBHideModel(bool bhide);
	};
}