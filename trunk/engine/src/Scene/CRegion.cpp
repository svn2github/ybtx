#include "stdafx.h"
#include "CRegion.h"
#include "ExpHelper.h"
#include "CBaseScene.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	CRegion::CRegion(CBaseScene* pParent, URegionID ID)
		: m_pParentMgr(pParent)
		, m_ID(ID)
		, m_isValid(true)
	{
		Ast(pParent);
	}

	CRegion::~CRegion(void)
	{
		Destroy();
	}

	bool CRegion::AddCtrlNode( CElementNode* pNode, bool bSync )
	{
		if( pNode->DeCode(bSync))
		{
			m_NodeList.push_back(pNode);
			return true;
		}
		
		pNode->Destroy();
		return false;
	}

	void CRegion::DelCtrlNode( CElementManager* pEleMgr )
	{ 
		CtrlList::iterator it,eit = m_NodeList.end();
		for( it = m_NodeList.begin(); it!=eit; )
		{
			if( (*it)->GetParentManager() == pEleMgr )
			{
				(*it)->Destroy();
				it = m_NodeList.erase(it);
			}
			else
				++it;
		}
	}

	void CRegion::Destroy(void)
	{
		CtrlList::iterator it,eit = m_NodeList.end();
		for( it = m_NodeList.begin(); it!=eit; )
			(*it++)->Destroy();
		m_NodeList.clear();
	}

	void CRegion::ToRecycle(void)
	{
		CtrlList::iterator it,eit = m_NodeList.end();
		for( it = m_NodeList.begin(); it!=eit; ++it )
			(*it)->m_isValid = false;
	}

	void CRegion::SetValid(void)
	{
		CtrlList::iterator it,eit = m_NodeList.end();
		for( it = m_NodeList.begin(); it!=eit; ++it )
			(*it)->m_isValid = true;
	}
}