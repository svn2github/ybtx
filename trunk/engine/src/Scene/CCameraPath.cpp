#include "stdafx.h"
#include <cassert>
#include "CCamera.h"
#include "CCameraPath.h"
#include "CBufFile.h"


namespace sqr
{
	const float CCameraPath::LOOP_KEY = 33333300.0f;

	CCameraPath::CCameraPath()
	{
	}

	CCameraPath::~CCameraPath()
	{
		this->Clear();
	}

	void CCameraPath::Build()
	{
		m_PositionCurve.BuildCardinal( 0.5f );
		m_TargetCurve.BuildCardinal( 0.5f );
		m_FovCurve.BuildCardinal(0.5f);
	}

	void CCameraPath::Clear()
	{
		m_PositionCurve.Clear();
		m_TargetCurve.Clear();
		m_FovCurve.Clear();
	}

	CtrlState CCameraPath::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
	{
		float DeltaTime = float( uCurTime-m_uStartTime );
		m_TargetPos = m_TargetCurve.Get(DeltaTime);
		pCamera->SetPosition( m_PositionCurve.Get(DeltaTime) );
		pCamera->lookAt( m_TargetPos );
		pCamera->setFOVy( CMath::ToRadian(m_FovCurve.Get(DeltaTime).x) );
			
		uint32 last_key = m_PositionCurve.GetNumberOfKeys() - 1;
		if( last_key == (uint32)m_PositionCurve.GetCurKeyIndex() 
			&& m_PositionCurve.GetKeyTime(last_key) != LOOP_KEY )
			return CS_END;
		
		return CS_CTRL;
	}

	void CCameraPath::AddPositionKey( CVector3f key, const float fNewKeyTime )
	{
		m_PositionCurve.Add( key, fNewKeyTime );
	}

	void CCameraPath::AddTargetkey( CVector3f key, const float fNewKeyTime )
	{
		m_TargetCurve.Add( key, fNewKeyTime );
	}

	void CCameraPath::AddFovKey( CVector3f key, const float fNewKeyTime )
	{
		m_FovCurve.Add( key, fNewKeyTime );
	}
}