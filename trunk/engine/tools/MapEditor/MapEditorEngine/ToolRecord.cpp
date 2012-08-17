#include "stdafx.h"
#include "ToolRecord.h"
#include "ICamPathHandler.h"
#include "CRecordCamera.inl"
#include "CMapEditCamera.inl"
#include "CCameraBase.inl"
#include "CCameraManager.h"
#include "EditStateOpertiaonMgr.h"
#include "IApplication.h"
#include "CWindowTarget.h"
#include "CBufFile.h"
#include "CCoder.h"
#include "BaseHelper.h"

namespace sqr
{
	CToolRecord::CToolRecord()
		: m_nCurrentAnimIndex(-1)
		, m_nCurrentKeyIndex(-1)
		, m_fStartTime(0)
		, m_fPauseDelta(0)
		, m_nState(PLAY_STATE_STOP)
		, m_pHandler(0)
		, m_pRecordCam(NULL)
	{
	}

	CToolRecord::~CToolRecord()
	{
	}

	int CToolRecord::AddKey( float fNewKeyTime )
	{
		if ( m_nCurrentAnimIndex == -1 || !m_vecCamAnim[m_nCurrentAnimIndex] ) 
		{
			return INVALID_KEY_POS;
		}
		
		CVector3f	vecTargetPos;
		vecTargetPos = m_pRecordCam->GetNewTarget();

		m_vecCamAnim[m_nCurrentAnimIndex]->SetTargetPos(vecTargetPos);
		int nNewKeyPos = m_vecCamAnim[m_nCurrentAnimIndex]->AddCamAnimKey( fNewKeyTime );
		if ( nNewKeyPos != INVALID_KEY_POS )
			return nNewKeyPos;
		
		return INVALID_KEY_POS;
	}

	void CToolRecord::DeleteKey( uint32 uAnimKeyIndex )
	{
		if ( m_nCurrentAnimIndex == -1 || !m_vecCamAnim[m_nCurrentAnimIndex] ) 
		{
			return;
		}

		m_vecCamAnim[m_nCurrentAnimIndex]->DelCamAnimKey( uAnimKeyIndex );
	}

	int CToolRecord::GetCurrentAnim() const
	{
		return m_nCurrentAnimIndex;
	}

	void CToolRecord::UpdateFPSLimited()
	{
		if ( m_nState == PLAY_STATE_PLAYING )
		{
			CtrlState cs = m_vecCamAnim[m_nCurrentAnimIndex]->DoCtrl( IApplication::GetInst()->GetCurTime() );
			int nCurrentKey = m_vecCamAnim[m_nCurrentAnimIndex]->GetCurrentKeyIndex();

			if ( nCurrentKey != m_nCurrentKeyIndex )
			{
				m_nCurrentKeyIndex = nCurrentKey;
				m_pHandler->OnKeyChanged();
			}

			if ( cs == CS_END )
			{
				m_pRecordCam->Bind(CMainWindowTarget::GetInst()->GetCamera());
				m_pHandler->OnAnimEnd();
			}
		}
	}

	void CToolRecord::ResetStartTime( uint32 fStartTime )
	{	
		if ( m_nCurrentAnimIndex == -1 ) return;

		m_fStartTime = fStartTime ;
		m_vecCamAnim[m_nCurrentAnimIndex]->BeginCtrl( m_fStartTime );	
		m_vecCamAnim[m_nCurrentAnimIndex]->Build();
	}

	void CToolRecord::Clear()
	{
		Pause();

		CamAnimVector::iterator it = m_vecCamAnim.begin();
		for ( ; it!=m_vecCamAnim.end(); ++it )
		{
			if ( *it )
			{
				(*it)->Clear();
				SafeDelete( *it );
			}
		}

		m_vecCamAnim.clear();
	}

	void CToolRecord::Save(FILE * fp)
	{
		uint32 uAnimCount = m_vecCamAnim.size();
		fwrite( &uAnimCount, sizeof(uint32), 1, fp );

		for ( uint32 i=0; i<uAnimCount; ++i )
		{
			fwrite(&m_vecCamAnim[i],sizeof(int), 1, fp);

			if ( m_vecCamAnim[i] )
			{
				m_vecCamAnim[i]->BinSave(fp);
			}
		}
	}

	void CToolRecord::SaveRecordInfo(WriteDataInf* wdi)
	{
		uint32 uAnimCount = m_vecCamAnim.size();
		wdi->Write(&uAnimCount, sizeof(uAnimCount));

		for ( uint32 i=0; i<uAnimCount; ++i )
		{
			wdi->Write(&m_vecCamAnim[i], sizeof(int));

			if ( m_vecCamAnim[i] )
			{
				m_vecCamAnim[i]->SaveCameraPathInfo(wdi);
			}
		}
	}


	void CToolRecord::Load( FILE * fp )
	{
		uint32 uAnimCount = 0;
		fread( &uAnimCount, sizeof(uint32), 1, fp );

		if ( uAnimCount > 0 )
		{
			m_vecCamAnim.resize( uAnimCount, NULL );
		}

		for ( uint32 i=0; i<uAnimCount; ++i )
		{
			int		pc		= 0;
			fread(&pc,sizeof(int),1,fp);

			if ( pc != NULL )
			{
				m_vecCamAnim[i] = new CCameraPathEdit;
				
				m_vecCamAnim[i]->BinLoad(fp);
				m_nCurrentAnimIndex = i;
			}
		}
	}

	void CToolRecord::LoadRecordInfo(CBufFile* tData)
	{
		uint32 uAnimCount = 0;
		tData->SafeRead( &uAnimCount, sizeof(uAnimCount) );

		for ( uint32 i=0; i < uAnimCount; ++i )
		{
			int		pc		= 0;
			tData->SafeRead( &pc, sizeof(int) );

			if ( pc != NULL )
			{
				m_vecCamAnim[i] = new CCameraPathEdit;

				m_vecCamAnim[i]->LoadCameraPathInfo(tData);
				m_nCurrentAnimIndex = i;
			}
		}
	}

	void CToolRecord::AddAnim()
	{
		m_vecCamAnim.push_back( new CCameraPathEdit );
		m_nCurrentAnimIndex = m_vecCamAnim.size() - 1;
	}

	uint32 CToolRecord::GetAnimCount() const
	{
		return m_vecCamAnim.size();
	}

	void CToolRecord::SetCurrentAnim( int uAnimIndex )
	{
		// °ó¶¨ÉãÏñ»ú
		m_nCurrentAnimIndex = uAnimIndex;
		PathBindCamera(uAnimIndex);
	}

	float CToolRecord::GetAnimLen( uint32 uAnimID ) const
	{
		CCameraPathEdit::Key	temp;
		if ( m_vecCamAnim[uAnimID] )
		{
			return m_vecCamAnim[uAnimID]->GetLength();
		}
		return 0;
	}

	void CToolRecord::DeleteAnim( uint32 uAnimIndex )
	{
		//ASSERT( uAnimIndex >= 0 && uAnimIndex < m_vecCamAnim.size() );

		//SafeDelete( m_vecCamAnim[uAnimIndex] );
		//m_vecCamAnim.erase( m_vecCamAnim.begin() + uAnimIndex );

		//if ( !m_vecCamAnim.empty() )
		//{
		//	m_nCurrentAnimIndex = max( (int)uAnimIndex-1, 0 );
		//}
		//else
		//{
		//	m_nCurrentAnimIndex = -1;
		//}
	}

	uint32 CToolRecord::GetKeyCount() const
	{
		if ( m_nCurrentAnimIndex == -1 )
		{
			return 0;
		}
		else
		{
			return m_vecCamAnim[m_nCurrentAnimIndex]->GetKeyCount();
		}
	}

	float CToolRecord::GetKeyTime( uint32 uKeyIndex ) const
	{
		return m_vecCamAnim[m_nCurrentAnimIndex]->GetAnimKeyTime( uKeyIndex );
	}

	void CToolRecord::GetKeyValue( CCameraPathEdit::Key &sKyeValue, uint32 uKeyIndex ) const
	{
		m_vecCamAnim[m_nCurrentAnimIndex]->GetKeyValue( sKyeValue, uKeyIndex );
	}

	float CToolRecord::GetKeyValueFov( uint32 uKeyIndex ) const
	{
		CCameraPathEdit::Key key;
		this->GetKeyValue(key, uKeyIndex);
		return key.fov.x;
	}

	void CToolRecord::SetCurrentKey( int uKeyIndex )
	{
		m_nCurrentKeyIndex = uKeyIndex;
	}

	int CToolRecord::GetCurrentKey() const
	{
		return m_nCurrentKeyIndex;
	}

	void CToolRecord::RePlay()
	{
		if ( m_nState != PLAY_STATE_PLAYING && GetKeyCount() != 0 )
		{
			m_nState = PLAY_STATE_PLAYING;
			PathBindCamera(m_nCurrentAnimIndex);
			ResetStartTime( IApplication::GetInst()->GetCurTime() );
			m_fPauseDelta = 0;
		}
	}

	void CToolRecord::Pause()
	{
		if ( m_nState == PLAY_STATE_PLAYING )
		{
			m_fPauseDelta	= IApplication::GetInst()->GetCurTime() - m_fStartTime;
			m_nState		= PLAY_STATE_PAUSE;
		}
	}

	void CToolRecord::Continue()
	{
		if ( m_nState == PLAY_STATE_PAUSE )
		{
			m_nState = PLAY_STATE_PLAYING;
			PathBindCamera(m_nCurrentAnimIndex);
			ResetStartTime( IApplication::GetInst()->GetCurTime() - m_fPauseDelta );
			m_fPauseDelta = 0;
		}
	}

	void CToolRecord::Stop()
	{
		m_nState		= PLAY_STATE_STOP;
		m_fPauseDelta	= 0;
		m_fStartTime	= 0;
		m_pRecordCam->Bind(CMainWindowTarget::GetInst()->GetCamera());
	}

	int CToolRecord::GetState()
	{
		return m_nState;
	}

	void CToolRecord::SetHandler( ICamPathHandler *pHandler )
	{
		m_pHandler = pHandler;
	}

	void CToolRecord::ViewOnKey( uint nKey )
	{
		assert( nKey >= 0 && nKey < GetKeyCount() );

		if ( m_nState == PLAY_STATE_PLAYING )
		{
			return;
		}

		Stop();
		PathBindCamera(m_nCurrentAnimIndex);
		ResetStartTime(m_fStartTime);
		float fKeyTime = m_vecCamAnim[m_nCurrentAnimIndex]->GetAnimKeyTime( nKey );
		m_vecCamAnim[m_nCurrentAnimIndex]->DoCtrl( uint32(fKeyTime+0.5) );

		CCamera *pRawCamera = CMainWindowTarget::GetInst()->GetCamera();
		CCameraManager::GetInst()->GetRecordCamera()->Bind(pRawCamera);
		CCameraManager::GetInst()->GetRecordCamera()->SetNewTarget(m_vecCamAnim[m_nCurrentAnimIndex]->GetTargetPos());
		CCameraManager::GetInst()->GetRecordCamera()->SetNewPosition(pRawCamera->GetPosition());
		CCameraManager::GetInst()->GetRecordCamera()->SetFovY(CMath::ToDegree(pRawCamera->getFOVy()));
	}

	void CToolRecord::SetKeyTime( float fNewTime, uint32 uKeyIndex )
	{
		m_vecCamAnim[m_nCurrentAnimIndex]->SetKeyTime( fNewTime, uKeyIndex );
	}

	void CToolRecord::SetKeyValue( uint32 uKeyIndex )
	{
		CVector3f	vecTargetPos;
		vecTargetPos = m_pRecordCam->GetNewTarget();

		m_vecCamAnim[m_nCurrentAnimIndex]->SetTargetPos(vecTargetPos);
		m_vecCamAnim[m_nCurrentAnimIndex]->SetKeyValue(uKeyIndex);
	}

	void CToolRecord::OnEvent( const CMsg& msg )
	{
		if ( CEditStateOpertiaonMgr::GetInst()->GetEditState() !=  CEditStateOpertiaonMgr::EES_RECORD )
		{
			return;
		}
	}

	void CToolRecord::PathBindCamera( uint32 uCurPath )
	{
		if ( GetAnimCount() == 0 || uCurPath<0 || uCurPath>GetAnimCount() )
			return;
		m_vecCamAnim[uCurPath]->Bind( CMainWindowTarget::GetInst()->GetCamera() );
	}

	void CToolRecord::SetUseFPSCam( bool bOperateTarget )
	{
		CRecordCamera *pFirstViewCamera = CCameraManager::GetInst()->GetRecordCamera();
		if ( pFirstViewCamera != CCameraManager::GetInst()->GetActiveCamera() )
			return;

		pFirstViewCamera->Bind(CMainWindowTarget::GetInst()->GetCamera());
		pFirstViewCamera->BeginCtrl(IApplication::GetInst()->GetCurTime());

		if ( bOperateTarget )
			pFirstViewCamera->SetType( CRecordCamera::eTP_Target );	
		else
			pFirstViewCamera->SetType( CRecordCamera::eTP_Position );
	}

	void CToolRecord::SetFov( float fFov )
	{
		m_pRecordCam->SetFovY(fFov);
	}

	float CToolRecord::GetFov() const
	{
		return m_pRecordCam->GetFovY();
	}

	void CToolRecord::SetCamSpeed( float fSpeed )
	{
		m_pRecordCam->SetMovSpeed(fSpeed);
	}

	float CToolRecord::GetCamSpeed()
	{
		return m_pRecordCam->GetMovSpeed();
	}

	float CToolRecord::GetLoopKeyMask() const
	{
		return CCameraPathEdit::LOOP_KEY;
	}

	void CToolRecord::SetCamPos(float x,float y,float z)
	{
		m_pRecordCam->SetPosition(CVector3f(x,y,z));
	}

	CVector3f CToolRecord::GetCamPos()
	{
		return m_pRecordCam->GetPosition();
	}

	void CToolRecord::SetCamTarget(float x,float y,float z)
	{
		m_pRecordCam->SetTarget(CVector3f(x,y,z));
	}

	CVector3f CToolRecord::GetCamTarget()
	{
		return m_pRecordCam->GetTarget();
	}
}
