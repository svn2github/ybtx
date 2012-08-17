#include "StdAfx.h"
#include "TerrainMeshObject.h"
#include "basehelper.h"
#include "ModelGroup.h"

namespace sqr
{
	CTObject::CTObject()
	{
		m_nResSetIndex	= 0;
		m_eAutoGenerationType = EAGT_NONE;
		m_bShadow = false;
		m_pModelGroup = NULL;
		m_nCenterGridIndex = INVALID;
		m_nResSetIndex = 0;
		m_nResCueIndex = 0;
	}

	CTObject::~CTObject()
	{
		SafeDelete(m_pModelGroup);
	}

	int CTObject::GetResSetIndex()
	{
		return m_nResSetIndex;
	}

	void CTObject::SetResSetIndex( int n )
	{
		m_nResSetIndex = n;
	}

	int CTObject::GetResCueIndex()
	{
		return m_nResCueIndex;
	}

	void CTObject::SetResCueIndex( int n )
	{
		m_nResCueIndex = n;
	}

	void CTObject::SetCenterGridIndex( int n )
	{
		m_nCenterGridIndex = n;
		m_pModelGroup->SetCenterGridIndex(n);
	}

	int CTObject::GetCenterGridIndex() const
	{
		return m_nCenterGridIndex;
	}

	void CTObject::SetCastShadow( bool b )
	{
		m_bShadow = b;
	}

	void CTObject::SwitchShadow()
	{
		m_bShadow = !m_bShadow;
	}

	bool CTObject::IsCastShadow()
	{
		return m_bShadow;
	}

	void CTObject::Render()
	{
		if ( this->GetModelGroup() )
			this->GetModelGroup()->Render();
	}

	int CTObject::GetCenterGridIndexCurrent()
	{
		return this->GetModelGroup()->GetCenterGridIndexCurrent();
	}

	void CTObject::SetModelGroup( CEditModelGroup * p )
	{
		m_pModelGroup = p;
	}

	CEditModelGroup * CTObject::GetModelGroup()
	{
		return m_pModelGroup;
	}

	void CTObject::SetClassName( const string & strClassName )
	{
		m_strClassName = strClassName;
	}

	sqr::EAUTO_GENERATION_TYPE CTObject::GetAutoGenerationType()
	{
		return m_eAutoGenerationType;
	}

	void CTObject::SetAutoGenerationType( sqr::EAUTO_GENERATION_TYPE e )
	{
		m_eAutoGenerationType = e;
	}
}