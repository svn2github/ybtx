#include "stdafx.h"
#include "CRecordCameraNode.h"
#include "CRecordCamera.inl"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "CEventState.h"
namespace sqr
{
	CRecordCameraNode::CRecordCameraNode( CRecordCamera *pCam)
		: m_pCamera(pCam)
	{
	}

	void CRecordCameraNode::OnEvent( const CMsg& msg )
	{
		COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
		if ( pOperator == NULL || !m_pCamera->m_pCtrlNode || !m_pCamera->m_isInCtrl )
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
						// up another type
						if ( msg.msgInfo.key_info == MGI_KEY_Q )
						{
							m_pCamera->Zoom(1.0f);
						}
						// down another type
						if ( msg.msgInfo.key_info == MGI_KEY_E )
						{
							m_pCamera->Zoom(-1.0f);
						}
					}
					break;
				}
				break;
			}
		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_WHEEL:
					{
						m_pCamera->Zoom(-float(msg.msgInfo._iinfo));
					}
				case MGT_MOUSE_MOVE :
					{
						if ( CEventState::GetInst()->GetState(MGT_MOUSE_RBUTTON) == MGT_MOUSE_RBNDOWN )
						{
							float pitch = 0.004f * ( msg.msgInfo.mouse_info.Y - m_iY );
							float yaw	= 0.004f * ( msg.msgInfo.mouse_info.X - m_iX );						
							// ÈÆ×ÔÉíÐý×ª
							m_pCamera->Rotate(yaw, pitch);
						}

						if (  CEventState::GetInst()->GetState(MGT_MOUSE_LBUTTON) == MGT_MOUSE_LBNDOWN )
						{
							m_pCamera->Zoom( float(m_iY-msg.msgInfo.mouse_info.Y) );
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

	void CRecordCameraNode::Render()
	{

	}
}