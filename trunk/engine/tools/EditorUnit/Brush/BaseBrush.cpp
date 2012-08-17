#include "StdAfx.h"
#include "Brush/BaseBrush.h"
#include "Brush/SquareBrushImp.h"
#include "Brush/CircleBrushImp.h"
#include "Brush/RegularGridBrushImp.h"
#include "CEventState.h"
#include "CEditContext.h"
#include "CDataScene.h"
#include "BaseHelper.h"
#include "CmdMgr.h"
#include "TraceHelper.h"

CBaseBrush::CBaseBrush(void)
{
	m_pContext		 = NULL;
	m_pBaseBrushDraw = NULL;
}

CBaseBrush::~CBaseBrush(void)
{
	SafeDelete(m_pBaseBrushDraw);
}

bool CBaseBrush::MsgProc( const CMsg& msg )
{
	switch( msg.msgSource )
	{
	case MGS_SYSMSG:
		{
			switch( msg.msgType.sys_type )
			{
			case MGT_SYS_INIT:
				{
					if(m_pOwner)
						_Init(m_pOwner);
				}
		
				break;

			case MGT_SYS_RENDER:
				{
					uint32 uWidth  = m_pContext->GetDataScene()->GetWidthInGrid();
					uint32 uHeight = m_pContext->GetDataScene()->GetHeightInGrid();
					if( uWidth == 0 || uHeight == 0 )
						return true;

					m_pBaseBrushDraw->SetBrushCenter(m_vecBrushCenter);
					Render();
				}
				
				return false;
			}
			break;
		}

	case MGS_MOUSEMSG:
		{
			switch( msg.msgType.mouse_type )
			{
			case MGT_MOUSE_LBNDOWN:
				{
					return false;
				}

			case MGT_MOUSE_LBNUP:
				{
					LButtonUpEvent(msg.msgInfo.mouse_info);
					return false;
				}

			case MGT_MOUSE_MOVE:
				{
					MouseMoveEvent(msg.msgInfo.mouse_info);
					if ( CEventState::GetInst()->GetState(MGT_MOUSE_LBUTTON) == MGT_MOUSE_LBNDOWN )
					{
						MoveLButtonDownEvent(msg.msgInfo.mouse_info);
						return false;
					}

					return true;
				}
				break;

			default:
				break;
			}
		}
		break;

	case MGS_KEYMSG:
		{
			if( m_pContext == NULL )
				return true;

			switch(msg.msgType.key_type)
			{
			case MGT_KEY_DOWN:
				return KeyDownEvent(msg.msgInfo.key_info);

			case MGT_KEY_UP:
				KeyUpEvent(msg.msgInfo.key_info);
				break;

			default:
				break;
			}

			return true;
		}
	default:
		break;
	}

	return true;
}

void CBaseBrush::MouseMoveEvent(MSG_MOUSE_INF mouse)
{
	m_mouseX = mouse.X;
	m_mouseY = mouse.Y;

	UpdateMouse();
}

void CBaseBrush::MoveLButtonDownEvent(MSG_MOUSE_INF mouse)
{

}

void CBaseBrush::LButtonUpEvent(MSG_MOUSE_INF mouse)
{

}

bool CBaseBrush::KeyDownEvent( MSG_KEY_INF key )
{
	if ( ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN ) )
		CtrlKeyDownEvent(key);
	else
	{
		if ( ( CEventState::GetInst()->GetState(MGI_KEY_ShiftKey) == MGT_KEY_DOWN ) )
			LShiftKeyDownEvent(key);
	}

	switch( key )
	{
	case MGI_KEY_Equals:	
		{
			m_pBaseBrushDraw->IncBrushOuterWidth();
			m_pBaseBrushDraw->IncBrushOuterHeight();

			m_pBaseBrushDraw->UpdateControlPoints();
		}
		break;

	case MGI_KEY_Minus:	
		{
			m_pBaseBrushDraw->DecBrushOuterWidth();
			m_pBaseBrushDraw->DecBrushOuterHeight();

			m_pBaseBrushDraw->UpdateControlPoints();
		}
		break;

	case MGI_KEY_B:
	case MGI_KEY_W:
	case MGI_KEY_S:
	case MGI_KEY_D:
	case MGI_KEY_A:
	case MGI_KEY_Q:
	case MGI_KEY_E:
	case MGI_KEY_Question:
		return true;

	default:
		break;
	}

	return false;
}

void CBaseBrush::CtrlKeyDownEvent(MSG_KEY_INF key)
{

}

void CBaseBrush::LShiftKeyDownEvent(MSG_KEY_INF key)
{
	switch( key )
	{
	case MGI_KEY_NumPad0:	
		{
			
		}
		break;

	default:
		break;
	}
}

void CBaseBrush::KeyUpEvent( MSG_KEY_INF key )
{
}

void CBaseBrush::_Init( CEventOwner* pOwner )
{
	m_pContext = dynamic_cast<CEditContext*>(pOwner);
}

void CBaseBrush::SetBaseBrushType( const BRUSH_TYPE type )
{
	if( m_pBaseBrushDraw )
	{
		SafeDelete(m_pBaseBrushDraw);
	}

	switch (type)
	{
	case BRUSH_SQUARE:
		m_pBaseBrushDraw = new CSquareBrushImp;
		break;

	case BRUSH_CIRCLE:
		m_pBaseBrushDraw = new CCircleBrushImp;
		break;

	case BRUSH_REGULAR:
		m_pBaseBrushDraw = new CRegularGridBrushImp;
		break;

	default:
		break;
	}

	if( m_pBaseBrushDraw )
		m_pBaseBrushDraw->SetEditContext(m_pContext);
}

void CBaseBrush::SetBrushIsShowGrid( const bool bShow )
{
	if( m_pBaseBrushDraw )
		m_pBaseBrushDraw->SetIsShowGrid(bShow);
}

void CBaseBrush::SetBrushUnit( const int nUnit )
{
	if( m_pBaseBrushDraw )
		m_pBaseBrushDraw->SetBrushUnit(nUnit);
}

void CBaseBrush::SetBaseBrushTexture(const string& texname)
{
	if( m_pBaseBrushDraw )
		m_pBaseBrushDraw->SetBrushTexture(texname);
}


void CBaseBrush::Render( void )
{
 	if( m_pBaseBrushDraw )
 		m_pBaseBrushDraw->Draw();
}

void CBaseBrush::UpdateMouse()
{
	if( m_pContext )
	{
		uint32 uWidth  = m_pContext->GetDataScene()->GetWidthInGrid();
		uint32 uHeight = m_pContext->GetDataScene()->GetHeightInGrid();
		if( uWidth != 0 && uHeight != 0 )
		{
			CIRect rect;
			m_pContext->GetWindowTarget()->GetRect(rect);

			int	width  = rect.right - rect.left;
			int	height = rect.bottom - rect.top;
			float	x  = float(m_mouseX)/float(width);
			float	y  = float(m_mouseY)/float(height);

			m_pContext->SelectPosByCursor(x, y);
			m_vecBrushCenter = m_pContext->GetSelectedLogicPoint();
			if(m_pBaseBrushDraw)
			{
				m_pBaseBrushDraw->SetBrushCenter(m_vecBrushCenter);
				m_pBaseBrushDraw->UpdateControlPoints();
			}
		}
	}


}

URegionID CBaseBrush::GetRegionID( const CVector3f& v3 )
{
	URegionID id;

	id.X = int16(CMath::Abs(v3.x / (eSGridSpan * eSRegionSpan)));
	id.Z = int16(CMath::Abs(v3.z / (eSGridSpan * eSRegionSpan)));
	return id;
}