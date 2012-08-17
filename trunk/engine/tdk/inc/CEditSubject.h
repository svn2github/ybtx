#pragma once
#include "CEditorClassPre.h"
#include <set>
namespace sqr_tools
{
	class CEditSubject
	{
		friend class CEditObserver;
	public:
		CEditSubject();
		virtual ~CEditSubject();

		bool	DataChange();
	protected:
		bool	AddObserver(CEditObserver* pObsever);
		bool	RemoveObserver(CEditObserver* pObsever);

		typedef set<CEditObserver*> ObserverSet;
		ObserverSet	m_setObserver;
	};
}