#include "stdafx.h"
#include "CGameGacCameraNode.h"
#include "CGameGacCamera.h"
#include "EditStateOpertiaonMgr.h"
#include "CEventState.h"

namespace sqr
{
	CGameGacCameraNode::CGameGacCameraNode( CGameGacCamera *pCam)
		:m_pCamera(pCam)
	{
		m_iX = 0;
		m_iY = 0;
	}

	void CGameGacCameraNode::OnEvent( const CMsg& msg )
	{
		if ( !CEditStateOpertiaonMgr::GetInst()->GetIsGameView() || m_pCamera == NULL )
			return;

		switch ( msg.msgSource )
		{

		case MGS_KEYMSG:
			{
				switch(msg.msgType.key_type)
				{
				case MGT_KEY_DOWN:
					{
						if ( msg.msgInfo.key_info == MGI_KEY_A )
						{
							m_pCamera->MoveLeft();
						}

						// right
						if ( msg.msgInfo.key_info == MGI_KEY_D )
						{
							m_pCamera->MoveRight();
						}

						// up
						if ( msg.msgInfo.key_info == MGI_KEY_W )
						{
							m_pCamera->MoveFront();
						}

						// down
						if ( msg.msgInfo.key_info == MGI_KEY_S )
						{
							m_pCamera->MoveBack();
						}
					}
				}
			}
			break;
		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_WHEEL:
					{
						m_pCamera->SetCameraOffset(-float(msg.msgInfo._iinfo));
						break;
					}

				case MGT_MOUSE_MOVE :
					{
						if ( CEventState::GetInst()->GetState(MGT_MOUSE_RBUTTON) == MGT_MOUSE_RBNDOWN )
						{
							float pitch = 0.004f * ( msg.msgInfo.mouse_info.Y - m_iY );
							float yaw	= 0.004f * ( msg.msgInfo.mouse_info.X - m_iX );
											
							float fDegree = m_pCamera->GetCameraYRotate();
							fDegree += yaw;
							m_pCamera->SetCameraYRotate(fDegree);
						}
						m_iY = msg.msgInfo.mouse_info.Y;
						m_iX = msg.msgInfo.mouse_info.X;
					}
					break;
				}
			}
		

		default:
			break;
		}
	}

	void CGameGacCameraNode::Render()
	{

	}
}