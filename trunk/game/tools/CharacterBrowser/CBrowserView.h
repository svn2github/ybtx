#pragma once
#include "EditorView\CEditorView.h"

class CBrowserView : public CEditorView
{
	enum CtrlState
	{
		CS_OUTCTRL,
		CS_MOVE,
		CS_ZOOM,
		CS_ROTATE,
	};

public:
	CBrowserView(void);
	virtual ~CBrowserView() {} ;
	CtrlState	m_eCtrlState;
	CPoint		m_CurPoint;
protected:
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnRButtonUp(UINT nFlags, CPoint point);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnMButtonDown(UINT nFlags, CPoint point);
	void OnMButtonUp(UINT nFlags, CPoint point);
};