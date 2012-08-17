#include "stdafx.h"
#include "CGameGacCamera.h"
#include "CGameGacCameraNode.h"
#include "CRTSCameraCtrller.h"
#include "IoSystem.h"
#include "BaseHelper.h"
#include "CXmlConfig.inl"
#include "CWindowTarget.h"
#include "ExsertStructDB.h"

namespace sqr
{
	static const float MOVE_VIEW_Y_SPEED	= 3.0f;
	static const float MOVE_SPEED_RATIO		= 64.0f;
	static const float ZOOM_SPEED_RATIO		= 10.0f;


	CGameGacCamera::CGameGacCamera(CCameraManager* camMgr) 
		:CCameraBase(camMgr)
		,EventProc(NULL)
	{
		m_pRTSCameraCtrller = new CRTSCameraCtrller;
		m_fCamDist = 0.0f;
		this->LoadGameCameraConfig();
		//类型
		SetCamType(eCT_GameView);
		//消息处理
		EventProc=new CGameGacCameraNode(this);
	}

	CGameGacCamera::~CGameGacCamera()
	{
		SafeDelete(m_pRTSCameraCtrller);
	}

	CtrlState	CGameGacCamera::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
	{
		return m_pRTSCameraCtrller->DoCtrl(uCurTime);;
	}

	void CGameGacCamera::_DoCameraCtrl( uint32 uCurTime )
	{
		m_pRTSCameraCtrller->DoCtrl(uCurTime);
	}

	void CGameGacCamera::Move( CVector3f v )
	{
	}

	void CGameGacCamera::MoveLeft()
	{
		CVector3f vecTarget = m_pRTSCameraCtrller->GetTargetPos();
 		vecTarget.x -= GRID_SPACE;
		m_pRTSCameraCtrller->SetTargetPos(vecTarget);
	}

	void CGameGacCamera::MoveRight()
	{
		CVector3f vecTarget = m_pRTSCameraCtrller->GetTargetPos();
		vecTarget.x += GRID_SPACE;
		m_pRTSCameraCtrller->SetTargetPos(vecTarget);
	}

	void CGameGacCamera::MoveFront()
	{
		CVector3f vecTarget = m_pRTSCameraCtrller->GetTargetPos();
		vecTarget.z += GRID_SPACE;
		m_pRTSCameraCtrller->SetTargetPos(vecTarget);
	}

	void CGameGacCamera::MoveBack()
	{
		CVector3f vecTarget = m_pRTSCameraCtrller->GetTargetPos();
		vecTarget.z -= GRID_SPACE;
		m_pRTSCameraCtrller->SetTargetPos(vecTarget);
	}

	void CGameGacCamera::BindGameCamera( const DWORD time )
	{
		m_pRTSCameraCtrller->Bind(CMainWindowTarget::GetInst()->GetCamera());
		m_pRTSCameraCtrller->BeginCtrl(time);
		m_pRTSCameraCtrller->DoCtrl(time);
	}

	void CGameGacCamera::SetCameraOffset( const float MouseDelta )
	{
		float fCamOffset = m_fCamDist + MouseDelta * 5.0f;
		m_fCamDist = limit2( fCamOffset, m_pRTSCameraCtrller->GetMinDist(), m_pRTSCameraCtrller->GetMaxDist() );
		m_pRTSCameraCtrller->SetDist(m_fCamDist);
	}

	void CGameGacCamera::SetCameraTarget( const CVector3f& target )
	{
		m_pRTSCameraCtrller->SetTargetPos(target);
	}

	void CGameGacCamera::SetCameraYRotate( float dgree )
	{
		m_pRTSCameraCtrller->SetYRotateDegree(dgree);
	}

	float CGameGacCamera::GetCameraYRotate()
	{
		return m_pRTSCameraCtrller->GetYRotateDegree();
	}

	CGameGacCameraNode* CGameGacCamera::GetEventPorcess() const
	{
		return EventProc;
	}

	void CGameGacCamera::DoCtrlUpdate(uint32 uCurTime)
	{
		if(m_pRTSCameraCtrller)
			m_pRTSCameraCtrller->DoCtrl(uCurTime);
	}

	void CGameGacCamera::LoadGameCameraConfig()
	{
		string config("cameracfg:GacConfig.xml");
		string strFileName = IoSystem::GetInst()->GetFullPath(config);
		ifstream iXml(strFileName.c_str());
		if ( !iXml.good() )
		{
			MessageBox(NULL,"请检查是否有cameracfg配置文件, 请检查或者全部更新", "提示",MB_OK);
			return;
		}

		string TempValue = "";
		float  fValue = 0;
		const float fPI=3.14159265f;

		CXmlConfig *pGameCameraConfig = new CXmlConfig( "config", iXml );
		TiXmlNode  *pGameCameraNode   = pGameCameraConfig->Get<TiXmlElement*>("camera_config");
		if( pGameCameraConfig )
		{
			TiXmlNode *pYDegreeNode = pGameCameraNode->FirstChild("ydegree");
			TiXmlElement *pElement  = pYDegreeNode->ToElement();
			if( pElement ) 
			{
				TempValue = pElement->GetText();
				pElement  = NULL;
				sscanf(TempValue.c_str(), "%f", &fValue);

				fValue = fValue * fPI / 180;
				m_pRTSCameraCtrller->SetYRadian(fValue);
			}

			TiXmlNode *pDefDistNode = pGameCameraNode->FirstChild("defdist");
			pElement = pDefDistNode->ToElement();
			if( pElement )
			{
				TempValue = pElement->GetText();
				pElement  = NULL;
				sscanf(TempValue.c_str(), "%f", &fValue);

				fValue = fValue * GRID_SPACE;
				m_fCamDist = fValue;
				m_pRTSCameraCtrller->SetDist(fValue);
			}

			TiXmlNode *pFarDisNode = pGameCameraNode->FirstChild("far_dist");
			TiXmlElement *pFarDisElement = pFarDisNode->ToElement();
			if( pFarDisElement )
			{
				TiXmlElement *pElement = NULL;

				pElement = pFarDisElement->FirstChildElement("xdegree");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * fPI / 180;
					m_pRTSCameraCtrller->SetMaxRadian(fValue);
				}

				pElement = pFarDisElement->FirstChildElement("fovdegree");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * fPI / 180;
					m_pRTSCameraCtrller->SetMaxFov(fValue);
				}

				pElement = pFarDisElement->FirstChildElement("dist");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * GRID_SPACE;
					m_pRTSCameraCtrller->SetMaxDist(fValue);
				}

				pElement = pFarDisElement->FirstChildElement("yoffset");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * GRID_SPACE;
					m_pRTSCameraCtrller->SetMaxYOffset(fValue);
				}

				pElement = pFarDisElement->FirstChildElement("nearplan");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * GRID_SPACE;
					m_pRTSCameraCtrller->SetMaxNearPlan(fValue);
				}
			}

			TiXmlNode *pNearDisNode = pGameCameraNode->FirstChild("near_dist");
			TiXmlElement *pNearDisElement = pNearDisNode->ToElement();
			if( pNearDisElement )
			{
				pElement = pNearDisElement->FirstChildElement("xdegree");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * fPI / 180;
					m_pRTSCameraCtrller->SetMinRadian(fValue);
				}

				pElement = pNearDisElement->FirstChildElement("fovdegree");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * fPI / 180;
					m_pRTSCameraCtrller->SetMinFov(fValue);
				}

				pElement = pNearDisElement->FirstChildElement("dist");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * GRID_SPACE;
					m_pRTSCameraCtrller->SetMinDist(fValue);
				}

				pElement = pNearDisElement->FirstChildElement("yoffset");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * GRID_SPACE;
					m_pRTSCameraCtrller->SetMinYOffset(fValue);
				}

				pElement = pNearDisElement->FirstChildElement("nearplan");
				if( pElement )
				{
					TempValue = pElement->GetText();
					pElement  = NULL;
					sscanf(TempValue.c_str(), "%f", &fValue);

					fValue = fValue * GRID_SPACE;
					m_pRTSCameraCtrller->SetMinNearPlan(fValue);
				}
			}

			TiXmlNode *pMaxHOffsetNode = pGameCameraNode->FirstChild("maxhoffset");
			pElement  = pMaxHOffsetNode->ToElement();
			if( pElement ) 
			{
				TempValue = pElement->GetText();
				pElement  = NULL;
				sscanf(TempValue.c_str(), "%f", &fValue);

				fValue = fValue * GRID_SPACE;
				m_pRTSCameraCtrller->SetMaxXZ_Dist(fValue);
			}

			TiXmlNode *pCamSpeed = pGameCameraNode->FirstChild("camspeed");
			pElement = pCamSpeed->ToElement();
			if( pElement )
			{
				TempValue = pElement->GetText();
				pElement  = NULL;
				sscanf(TempValue.c_str(), "%f", &fValue);

				fValue = fValue * GRID_SPACE;
				m_pRTSCameraCtrller->SetDistSpeed(fValue);
			}

			m_pRTSCameraCtrller->SetMaxYDegree(CMath::two_pi);
			m_pRTSCameraCtrller->SetYRotateDegree(0.0f);
		}
		
		SafeDelete(pGameCameraConfig);

		m_pRTSCameraCtrller->SetMaxYDegree(CMath::two_pi);
		m_pRTSCameraCtrller->SetYRotateDegree(0.0f);
	}
}