#include "stdafx.h"
#include "CBrowserView.h"
#include "GTRenderMgr.h"

CBrowserView::CBrowserView()
: m_eCtrlState(CS_OUTCTRL)
{

}

void CBrowserView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_eCtrlState = CS_MOVE;
}

void CBrowserView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_eCtrlState = CS_OUTCTRL;
}

BOOL CBrowserView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CTPSCameraCtrller& Ctrl = CGTRenderMgr::GetInst()->m_TPSCtrller;
	Ctrl.SetTargetDist( Ctrl.GetTargetDist() + zDelta );
	return TRUE;
}

void CBrowserView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CTPSCameraCtrller& Ctrl = CGTRenderMgr::GetInst()->m_TPSCtrller;
	Ctrl.SetXRotateDegree(CMath::qtr_pi);
	Ctrl.SetYRotateDegree(CMath::qtr_pi + CMath::half_pi);
	Ctrl.SetTargetDist(640.0f);
	Ctrl.SetTargetPos(CVector3f::ZERO);
}

void CBrowserView::OnMouseMove(UINT nFlags, CPoint point)
{
	switch( m_eCtrlState )
	{
	case CS_ROTATE:
		{
			CTPSCameraCtrller& Ctrl = CGTRenderMgr::GetInst()->m_TPSCtrller;
			Ctrl.SetXRotateDegree( Ctrl.GetXRotateDegree() + float(point.y - m_CurPoint.y)/100.0f );
			Ctrl.SetYRotateDegree( Ctrl.GetYRotateDegree() + float(point.x - m_CurPoint.x)/100.0f );
		}
		break;
	case CS_MOVE:
		{
			CTPSCameraCtrller& Ctrl = CGTRenderMgr::GetInst()->m_TPSCtrller;
			Ctrl.MoveTargetPos(  CVector3f((point.x - m_CurPoint.x)*0.5f,(m_CurPoint.y - point.y)*0.5f,0.0f) );
		}
		break;
	case CS_ZOOM:
		{
			CTPSCameraCtrller& Ctrl = CGTRenderMgr::GetInst()->m_TPSCtrller;
			Ctrl.SetTargetDist( Ctrl.GetTargetDist() + m_CurPoint.y - point.y );
		}
		break;
	default:
		break;
	}
	m_CurPoint = point;
}

void CBrowserView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_eCtrlState = CS_ROTATE;
}

void CBrowserView::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_eCtrlState = CS_OUTCTRL;
}

void CBrowserView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_eCtrlState = CS_ZOOM;
}

void CBrowserView::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_eCtrlState = CS_OUTCTRL;
}