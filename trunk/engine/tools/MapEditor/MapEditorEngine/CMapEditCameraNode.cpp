#include "stdafx.h"
#include "CMapEditCameraNode.h"
#include "CMapEditCamera.inl"
#include "EditStateOpertiaonMgr.h"
#include "CEventState.h"

namespace sqr
{
	CMapEditCameraNode::CMapEditCameraNode(CMapEditCamera* cam)
		:m_pCamera(cam)
	{
	}

	void CMapEditCameraNode::OnEvent( const CMsg& msg )
	{
		if ( CEditStateOpertiaonMgr::GetInst()->GetIsGameView() || !m_pCamera->m_pCtrlNode || !m_pCamera->m_isInCtrl )
			return;

		switch ( msg.msgSource )
		{
		case MGS_KEYMSG:
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


				// up another type
				if ( msg.msgInfo.key_info == MGI_KEY_Q )
				{
					m_pCamera->Zoom(-1.0f);
				}

				// down another type
				if ( msg.msgInfo.key_info == MGI_KEY_E )
				{
					m_pCamera->Zoom(1.0f);
				}
				break;
			}
		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_WHEEL:
				{
					m_pCamera->Zoom(float(msg.msgInfo._iinfo));
				}
				break;
				case MGT_MOUSE_MOVE :
				{
					if ( CEventState::GetInst()->GetState(MGT_MOUSE_RBUTTON)==MGT_MOUSE_RBNDOWN ) // right button down rotate
					{
						float pitch = 0.004f * ( msg.msgInfo.mouse_info.Y - m_iY );
						float yaw	= 0.004f * ( msg.msgInfo.mouse_info.X - m_iX );
						// 绕目标点旋转
						m_pCamera->RotatePosition(yaw, pitch);
					}

					if (CEventState::GetInst()->GetState(MGT_MOUSE_MBUTTON)==MGT_MOUSE_MBNDOWN )
					{
						MSG_MOUSE_INF inf = CEventState::GetInst()->GetMousePosition();
						m_pCamera->Zoom(float( m_iY - msg.msgInfo.mouse_info.Y));
						
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

	void CMapEditCameraNode::Render()
	{

	}
}