#include "stdafx.h"
#include "CameraPathEdit.h"
#include "CMapEditCamera.inl"
#include "CWindowTarget.h"
#include "CCameraManager.h"
#include "CCameraBase.inl"
#include <cassert>
#include "CCoder.h"

namespace sqr
{
	int CCameraPathEdit::AddCamAnimKey( float fNewKeyTime )
	{
		CCamera* pCamera = CMainWindowTarget::GetInst()->GetCamera();
		int nNewPos = m_PositionCurve.Add( pCamera->GetPosition(), fNewKeyTime );
		m_TargetCurve.Add( m_TargetPos, fNewKeyTime );
		m_FovCurve.Add( CVector3f(CMath::ToDegree( pCamera->getFOVy() ), 0.0f, 0.0f), 
						   fNewKeyTime );
		
		return nNewPos;
	}	
 
	void CCameraPathEdit::DelCamAnimKey( uint32 uAnimKeyIndex )
	{
		m_PositionCurve.Delete( uAnimKeyIndex );
		m_TargetCurve.Delete( uAnimKeyIndex );
		m_FovCurve.Delete( uAnimKeyIndex );
	}

	void CCameraPathEdit::SetHeadFrame( const Key &camera_key )
	{
		if ( m_PositionCurve.GetNumberOfKeys() == 0 )
		{
			return;
		}

		m_PositionCurve.SetKeyValue( 0, camera_key.position );
		m_TargetCurve.SetKeyValue( 0, camera_key.target );
		m_FovCurve.SetKeyValue( 0, camera_key.fov );
	}

	void CCameraPathEdit::SetTailFrame( const Key &camera_key )
	{
		if ( m_PositionCurve.GetNumberOfKeys() == 0 )
		{
			return;
		}

		m_PositionCurve.SetKeyValue( m_PositionCurve.GetNumberOfKeys()-1, camera_key.position );
		m_TargetCurve.SetKeyValue( m_TargetCurve.GetNumberOfKeys()-1, camera_key.target );
		m_FovCurve.SetKeyValue( m_FovCurve.GetNumberOfKeys()-1, camera_key.fov );
	}

	void CCameraPathEdit::SetKeyTime( float fNewTime, uint32 uKeyIndex )
	{
		assert( fNewTime >= 0.0f && uKeyIndex >= 0 && uKeyIndex < GetKeyCount() );

		m_PositionCurve.SetKeyTime( uKeyIndex, fNewTime );
		m_TargetCurve.SetKeyTime( uKeyIndex, fNewTime );
		m_FovCurve.SetKeyTime( uKeyIndex, fNewTime );
	}

	void CCameraPathEdit::SetKeyValue( uint32 uKeyIndex )
	{
		assert( uKeyIndex >= 0 && uKeyIndex < GetKeyCount() );

		CCameraBase* pCamera = CCameraManager::GetInst()->GetActiveCamera();

		m_PositionCurve.SetKeyValue( uKeyIndex, pCamera->GetNewPosition() );
		m_TargetCurve.SetKeyValue( uKeyIndex, pCamera->GetNewTarget() );
		m_FovCurve.SetKeyValue( uKeyIndex, CVector3f(pCamera->GetFovY(), 0.0f, 0.0f) );
	}

	float CCameraPathEdit::GetLength() const
	{
		int numKeys = m_PositionCurve.GetNumberOfKeys();
		if ( numKeys == 0 ) return 0.0f;
		return m_PositionCurve.GetKeyTime( numKeys-1 );
	}

	float CCameraPathEdit::GetAnimKeyTime( int uKeyIndex ) const
	{
		assert( m_PositionCurve.GetNumberOfKeys() > 0 );

		return m_PositionCurve.GetKeyTime( uKeyIndex );
	}

	uint32 CCameraPathEdit::GetKeyCount() const
	{
		return m_PositionCurve.GetNumberOfKeys();
	}

	void CCameraPathEdit::GetKeyValue( Key &sFrameValue, uint32 uFrameIndex ) const
	{
		assert( m_PositionCurve.GetNumberOfKeys() != 0 &&
			uFrameIndex >=0 && uFrameIndex < m_PositionCurve.GetNumberOfKeys() );

		sFrameValue.position = m_PositionCurve.GetKeyValue( uFrameIndex );
		sFrameValue.target	 = m_TargetCurve.GetKeyValue( uFrameIndex );
		sFrameValue.fov		 = m_FovCurve.GetKeyValue( uFrameIndex );
	}

	void CCameraPathEdit::GetHeadFrame( Key &camera_key ) const
	{
		if ( m_PositionCurve.GetNumberOfKeys() == 0 ) return;

		camera_key.position = m_PositionCurve.GetKeyValue(0);
		camera_key.target	= m_TargetCurve.GetKeyValue(0);
		camera_key.fov		= m_FovCurve.GetKeyValue(0);
	}

	void CCameraPathEdit::GetTailFrame( Key &camera_key ) const
	{
		if ( m_PositionCurve.GetNumberOfKeys() == 0 ) return;

		camera_key.position = m_PositionCurve.GetKeyValue(m_PositionCurve.GetNumberOfKeys()-1);
		camera_key.target	= m_TargetCurve.GetKeyValue(m_TargetCurve.GetNumberOfKeys()-1);
		camera_key.fov		= m_FovCurve.GetKeyValue(m_FovCurve.GetNumberOfKeys()-1);
	}

	void CCameraPathEdit::BinSave(FILE * fp)
	{
		uint32 uKeyCount = GetKeyCount();
		fwrite(&uKeyCount, sizeof(uint32), 1, fp);	

		CVector3f	data;
		float		time;
		for ( uint32 i=0; i<uKeyCount; ++i )
		{
			// 保存 m_vecPositionCurve
			data = m_PositionCurve.GetKeyValue(i);
			time = m_PositionCurve.GetKeyTime(i);
			fwrite( &data, sizeof(CVector3f), 1, fp );
			fwrite( &time, sizeof(float), 1, fp );

			// 保存 m_vecPositionCurve
			data = m_TargetCurve.GetKeyValue(i);
			time = m_TargetCurve.GetKeyTime(i);
			fwrite( &data, sizeof(CVector3f), 1, fp );
			fwrite( &time, sizeof(float), 1, fp );

			// 保存 m_vecPositionCurve
			data = m_FovCurve.GetKeyValue(i);
			time = m_FovCurve.GetKeyTime(i);
			fwrite( &data, sizeof(CVector3f), 1, fp );
			fwrite( &time, sizeof(float), 1, fp );
		}
	}

	void CCameraPathEdit::SaveCameraPathInfo(WriteDataInf* wdi)
	{
		uint32 uKeyCount = GetKeyCount();
		wdi->Write(&uKeyCount, sizeof(uKeyCount));

		CVector3f	data;
		float		time;
		for ( uint32 i=0; i<uKeyCount; ++i )
		{
			// 保存 m_vecPositionCurve
			data = m_PositionCurve.GetKeyValue(i);
			time = m_PositionCurve.GetKeyTime(i);
			wdi->Write(&data, sizeof(data));
			wdi->Write(&time, sizeof(time));

			// 保存 m_vecPositionCurve
			data = m_TargetCurve.GetKeyValue(i);
			time = m_TargetCurve.GetKeyTime(i);
			wdi->Write(&data, sizeof(data));
			wdi->Write(&time, sizeof(time));

			// 保存 m_vecPositionCurve
			data = m_FovCurve.GetKeyValue(i);
			time = m_FovCurve.GetKeyTime(i);
			wdi->Write(&data, sizeof(data));
			wdi->Write(&time, sizeof(time));
		}
	}

	void CCameraPathEdit::BinLoad( FILE *fp )
	{
		this->Clear();

		uint32 uKeyCount = 0;
		fread(&uKeyCount, sizeof(uint32), 1, fp);	

		CVector3f data;
		float time;
		for ( uint32 i=0; i<uKeyCount; ++i )
		{
			// 读取 m_PositionCurve
			fread( &data, sizeof(CVector3f), 1, fp );
			fread( &time, sizeof(float), 1, fp );
			m_PositionCurve.Add( data, time );

			// 读取 m_vecTargetCurve
			fread( &data, sizeof(CVector3f), 1, fp );
			fread( &time, sizeof(float), 1, fp );
			m_TargetCurve.Add( data, time );

			// 读取 m_vecFovCurve
			fread( &data, sizeof(CVector3f), 1, fp );
			fread( &time, sizeof(float), 1, fp );
			m_FovCurve.Add( data, time );
		}
	}

	void CCameraPathEdit::LoadCameraPathInfo(CBufFile* tData)
	{
		this->Clear();

		uint32 uKeyCount = 0;
		tData->SafeRead( &uKeyCount, sizeof(uKeyCount) );

		CVector3f data;
		float time;
		for ( uint32 i=0; i<uKeyCount; ++i )
		{
			// 读取 m_PositionCurve
			tData->SafeRead( &data, sizeof(data) );
			tData->SafeRead( &time, sizeof(time) );
			m_PositionCurve.Add( data, time );

			// 读取 m_vecTargetCurve
			tData->SafeRead( &data, sizeof(data) );
			tData->SafeRead( &time, sizeof(time) );
			m_TargetCurve.Add( data, time );

			// 读取 m_vecFovCurve
			tData->SafeRead( &data, sizeof(data) );
			tData->SafeRead( &time, sizeof(time) );
			m_FovCurve.Add( data, time );
		}
	}

	int CCameraPathEdit::GetCurrentKeyIndex() const
	{
		return m_PositionCurve.GetCurKeyIndex();
	}
}