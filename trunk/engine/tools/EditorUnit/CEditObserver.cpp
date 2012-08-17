#include "stdafx.h"
#include "CEditObserver.h"
#include "CEditSubject.h"

CEditObserver::CEditObserver(void)
: m_pSubject(NULL)
, m_bObserve(true)
{

}

CEditObserver::CEditObserver(CEditSubject* pSubject)
: m_pSubject(pSubject)
, m_bObserve(true)
{
	m_pSubject->AddObserver(this);
	Update();
}	

CEditObserver::~CEditObserver()
{
	if(m_pSubject)
		m_pSubject->RemoveObserver(this);
}

void	CEditObserver::SetSubject(CEditSubject* pSubject)
{
	if(pSubject != m_pSubject)
	{
		if(m_pSubject!=NULL)
			m_pSubject->RemoveObserver(this);
		m_pSubject = pSubject;
		if(m_pSubject!=NULL)
			m_pSubject->AddObserver(this);
	}
	Update();
}

void	CEditObserver::EnableObserve(bool isEnable)
{
	m_bObserve = isEnable;
	Update();
}

bool	CEditObserver::Update(void)
{
	if(m_bObserve)
		return _Update();
	return true;
}
