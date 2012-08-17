#pragma once
#include "CEditorClassPre.h"

namespace sqr_tools
{
	class CEditObserver
	{
	public:
		CEditObserver(void);
		CEditObserver(CEditSubject* pSubject);
		virtual ~CEditObserver();

		void			SetSubject(CEditSubject* pSubject);
		void			EnableObserve(bool isEnable);
		bool			Update(void);

	protected:
		virtual bool	_Update(void){ return true; };
		bool			m_bObserve;
		CEditSubject*	m_pSubject;
	};
}
