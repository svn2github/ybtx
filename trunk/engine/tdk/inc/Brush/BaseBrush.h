#pragma once
#include "CEventDelegate.h"
#include "CMsg.h"
#include "BaseBrushEnum.h"
#include "CVector3.h"
#include "CSceneBase.h"

namespace sqr_tools
{
	class CEditContext;
	class CBaseBrushDraw;

	class CBaseBrush : public CEventDelegate
	{
	public:
		CBaseBrush(void);
		virtual ~CBaseBrush(void);

		bool MsgProc(const CMsg& msg);

		virtual void MouseMoveEvent(MSG_MOUSE_INF mouse);
		virtual void MoveLButtonDownEvent(MSG_MOUSE_INF mouse);
		virtual void LButtonUpEvent(MSG_MOUSE_INF mouse);
		virtual bool KeyDownEvent(MSG_KEY_INF key);
		virtual void CtrlKeyDownEvent(MSG_KEY_INF key);
		virtual void LShiftKeyDownEvent(MSG_KEY_INF key);
		virtual void KeyUpEvent(MSG_KEY_INF key);

		virtual void SetBaseBrushType(const BRUSH_TYPE type);
		virtual void SetBrushIsShowGrid(const bool bShow);
		virtual void SetBrushUnit(const int nUnit);
		virtual void SetBaseBrushTexture(const string& texname);
		virtual void UpdateMouse();
		virtual void Render(void);

		URegionID GetRegionID(const CVector3f& v3);

	protected:
		CEditContext*	m_pContext;
		unsigned short	m_mouseX;
		unsigned short	m_mouseY;
		CVector3f		m_vecBrushCenter;
		CBaseBrushDraw* m_pBaseBrushDraw;

		void _Init( CEventOwner* pOwner );
	};
}