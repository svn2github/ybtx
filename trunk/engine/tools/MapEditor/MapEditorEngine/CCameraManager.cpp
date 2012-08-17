#include "stdafx.h"
#include "CCameraManager.h"
#include "CMapEditCamera.inl"
#include "CRecordCamera.inl"
#include "CGameGacCamera.h"
#include "BaseHelper.h"
#include "IApplication.h"
namespace sqr
{
	CCameraManager::CCameraManager()
	:m_pActiveCamera(NULL)
	{
		_Init();
	}

	CCameraManager::~CCameraManager()
	{
	}

	void CCameraManager::_Init()
	{
		m_CameraList[eCT_MapEdit]	= new CMapEditCamera(this);
		m_CameraList[eCT_FirstView] = new CRecordCamera(this);
		m_CameraList[eCT_GameView]  = new CGameGacCamera(this);

		m_pActiveCamera=m_CameraList[eCT_MapEdit];
	}

	CMapEditCamera* CCameraManager::GetMapEditCamera()
	{
		return dynamic_cast<CMapEditCamera*>( m_CameraList[eCT_MapEdit] );
	}

	CRecordCamera* CCameraManager::GetRecordCamera()
	{
		return dynamic_cast<CRecordCamera*>( m_CameraList[eCT_FirstView] );
	}	
	
	CGameGacCamera* CCameraManager::GetGameGacCamera()
	{
		return dynamic_cast<CGameGacCamera*>( m_CameraList[eCT_GameView] );
	}

	void CCameraManager::SetActiveCamera( CCameraBase *pCamera )
	{
		m_pActiveCamera = pCamera;
	}

	CCameraBase* CCameraManager::GetActiveCamera() const
	{
		return m_pActiveCamera;
	}

	void CCameraManager::Update()
	{
		if(m_pActiveCamera)
		{
			m_pActiveCamera->DoCtrlUpdate(IApplication::GetInst()->GetCurTime());
		}
	}	

	void CCameraManager::EventProcess(const CMsg& msg)
	{
		int type=m_pActiveCamera->GetCamType();
		switch(type)
		{
		case eCT_MapEdit:
			GetMapEditCamera()->GetEventPorcess()->OnEvent(msg);
			break;
		case eCT_FirstView:
			GetRecordCamera()->GetEventPorcess()->OnEvent(msg);
			break;
		case eCT_GameView:
			GetGameGacCamera()->GetEventPorcess()->OnEvent(msg);
			break;
		}
		
	}
}
