#include "stdafx.h"
#include "CEditSet.h"
#include "BaseHelper.h"
#include "CEditModel.h"

CEditResSet::CEditResSet()
{
	m_bAmbientEffect = false;
	m_pEffectGroup = NULL;
}

CEditResSet::~CEditResSet()
{
	if (m_pEffectGroup)
		m_pEffectGroup =NULL;
}

bool CEditResSetUnit::NameExist( const string& s )
{
	size_t size = m_unitNames.size();
	for ( size_t i = 0; i < size; ++i )
	{
		if ( m_unitNames[i].compare(s) == 0 )
			return true;
	}

	return false;
}

CEdit2DResSetUnit::CEdit2DResSetUnit()
{
	m_eTransType = ETT_VERTEX;
}

ETRANS_TYPE CEdit2DResSetUnit::GetTransType()
{
	return m_eTransType;
}

void CEdit2DResSetUnit::SetTransType(ETRANS_TYPE e)
{
	m_eTransType = e;
}

//------------------------------------------------------------------------------
CEditResSetGroup::~CEditResSetGroup()
{
	size_t size = m_resSetUnits.size();
	for (size_t i = 0; i < size; ++i)
	{
		SafeDelete(m_resSetUnits[i]);
	}
	m_resSetUnits.clear();
}

bool CEditResSetGroup::NameExist( const string& s )
{
	string strItemName;
	CEditModel* pEditModel = NULL;
	size_t size = m_resSetUnits.size();
	for ( size_t i = 0; i < size; ++i )
	{
		strItemName = m_resSetUnits[i]->GetNameByIndex(0);
		if ( strItemName.compare(s) == 0 )
			return true;
	}

	return false;
}

//------------------------------------------------------------------------------
CEditModelResSetGroup::~CEditModelResSetGroup()
{
	size_t size = m_resModelSetUnits.size();
	for (size_t i = 0; i < size; ++i)
	{
		SafeDelete(m_resModelSetUnits[i]);
	}
	m_resModelSetUnits.clear();
}

bool CEditModelResSetGroup::NameExist( const string& s )
{
	string strModelName;
	CEditModel* pEditModel = NULL;
	size_t size = m_resModelSetUnits.size();
	for ( size_t i = 0; i < size; ++i )
	{
		CEditModel* pEditModel = m_resModelSetUnits[i]->m_pEditModel;
		if( pEditModel )
		{
			strModelName = pEditModel->GetModelName(0);
			if ( strModelName.compare(s) == 0 )
				return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
CEditDimSet::~CEditDimSet()
{
	size_t size = m_resSetGroups.size();
	for (size_t i = 0; i < size; ++i)
	{
		SafeDelete(m_resSetGroups[i]);
	}
	m_resSetGroups.clear();
}

CEdit3DDimSet::~CEdit3DDimSet()
{
	size_t size = m_resSetGroups.size();
	for (size_t i = 0; i < size; ++i)
	{
		SafeDelete(m_resSetGroups[i]);
	}
	m_resSetGroups.clear();
}

CEditResSetEditModelUnit::~CEditResSetEditModelUnit()
{
	SafeDelete(m_pEditModel);
}