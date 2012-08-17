#include "stdafx.h"
#include "CEditObserver.h"
#include "CEditSubject.h"

CEditSubject::CEditSubject()
{

}

CEditSubject::~CEditSubject()
{
	ObserverSet::iterator it = m_setObserver.begin();
	while( it!=m_setObserver.end() )
	{
		(*it)->SetSubject(NULL);
		it = m_setObserver.begin();
	}
}

bool CEditSubject::DataChange()
{
	bool bSucceed = true;
	ObserverSet::iterator it,eit = m_setObserver.end();
	for( it = m_setObserver.begin(); it!=eit; ++it )
		bSucceed &= (*it)->Update();
	return bSucceed;
}

bool CEditSubject::AddObserver(CEditObserver* pObsever)
{
	if(pObsever==NULL)
		return false;

	ObserverSet::iterator it = m_setObserver.find(pObsever);
	if( it == m_setObserver.end() )
	{
		m_setObserver.insert(pObsever);
		return true;
	}
	return false;
}

bool CEditSubject::RemoveObserver(CEditObserver* pObsever)
{
	ObserverSet::iterator it = m_setObserver.find(pObsever);
	if( it != m_setObserver.end() )
	{
		m_setObserver.erase(it);
		return true;
	}
	return false;
}