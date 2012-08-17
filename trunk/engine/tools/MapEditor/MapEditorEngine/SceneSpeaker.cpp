#include "stdafx.h"
//#include "Color.h"
#include "CVector3.h"
#include "SceneSpeaker.h"
#include "CMapEditApp.h"
#include "Operator.h"

namespace bly
{
	CSceneSpeaker::CSceneSpeaker():m_vPos(0.0f, 0.0f, 0.0f),
		m_fMinDistance(0.0f), m_fMaxDistance(0.0f), m_fVolume(0.0f), m_bIsSceneSpeaker(false)/*, m_pSound(NULL)*/
	{
	}

	//添加资源释放 baozi
	CSceneSpeaker::~CSceneSpeaker()
	{
// 		SafeDelete(m_pSound);
	}

	void CSceneSpeaker::SetPos(CVector3f& vPos)
	{
// 		if( m_pSound )
// 		{
// 			m_vPos = vPos;
// 
// 			if(!m_pSound->SetPosition(&m_vPos))
// 				MessageBox(NULL, "Sound SetPosition Fail", "GERROR", MB_OK);
// 		}
	}

	CVector3f* CSceneSpeaker::GetPos()
	{
		return &m_vPos;
	}

	void CSceneSpeaker::SetEventName(std::string & strEventName)
	{
// 		m_strEventName = strEventName;
// 
// 		IAudio* sys = bly::CMapEditApp::GetInstance()->GetOperator()->GetAudioSystem();
// 		if( m_pSound )
// 		{
// 			sys->FreeSound( m_pSound );
// 			m_pSound = NULL;
// 		}
// 
// 		if( !sys->CreateSound( m_strEventName.c_str(), &m_pSound ) )
// 			MessageBox( NULL, "CreateSound Fail", m_strEventName.c_str(), MB_OK );
	}

	std::string* CSceneSpeaker::GetEventName()
	{
		return &m_strEventName;
	}

	void CSceneSpeaker::SetMinDistance(float fMinDistance)
	{
		m_fMinDistance = fMinDistance;
	}

	float CSceneSpeaker::GetMinDistance()
	{
// 		if(m_pSound)
// 		{
// 			float fMaxDis;
// 			m_pSound->GetDistance( &m_fMinDistance, &fMaxDis );
// 			return m_fMinDistance;
// 		}
// 		else
			return 0.0f;
	}

	void CSceneSpeaker::SetMaxDistance(float fMaxDistance)
	{
		m_fMaxDistance = fMaxDistance;
	}

	float CSceneSpeaker::GetMaxDistance()
	{
// 		if(m_pSound)
// 		{
// 			float fMinDis;
// 			m_pSound->GetDistance( &fMinDis, &m_fMaxDistance );
// 			return m_fMaxDistance;
// 		}
// 		else
			return 0.0f;
	}

	void CSceneSpeaker::SetVolume(float fVolume)
	{
// 		if(m_pSound)
// 		{
// 			m_fVolume = fVolume;
// 
// 			if(!m_pSound->SetVolume(m_fVolume))
// 				MessageBox( NULL, "SetVolume Fail", "GERROR", MB_OK );
// 		}
	}

	float CSceneSpeaker::GetVolume()
	{
// 		if(m_pSound)
// 		{
// 			m_fVolume = m_pSound->GetVolume();
// 			return m_fVolume;
// 		}
// 		else
			return 0.0f;
	}

	void CSceneSpeaker::SetInfo(std::string & strInfo)
	{
		m_strInfo = strInfo;
	}

	std::string* CSceneSpeaker::GetInfo()
	{
		return &m_strInfo;
	}

	void CSceneSpeaker::/*PlaySound*/PlaySceneSound()
	{
// 		if(m_pSound)
// 		{
// 			if(!m_pSound->Play())
// 				MessageBox( NULL, "Play Sound Fail", "GERROR", MB_OK );
// 		}
	}

	void CSceneSpeaker::StopSound()
	{
// 		if(m_pSound)
// 		{
// 			if(!m_pSound->Stop())
// 				MessageBox( NULL, "Stop Sound Fail", "GERROR", MB_OK );
// 		}
	}
}