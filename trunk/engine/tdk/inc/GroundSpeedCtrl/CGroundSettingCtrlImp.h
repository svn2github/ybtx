#pragma once

#include "CCtrlBaseObserver.h"

namespace sqr_tools
{
	class CGridNode;
	class CEditAnimate;
	class CGroundSettingCtrlImp :public CCtrlBaseObserver
	{
		REG_UI_IMP(CGroundSettingCtrlImp);
	public:
		CGroundSettingCtrlImp(void* param);
		~CGroundSettingCtrlImp(void);

	public:
		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveNode( CRenderNode* pNode );
		virtual void NotifyObserver(void);
		virtual void SetGroundSpeed(const float fSpeed);
		virtual float GetGroundSpeed() const;
		virtual void ShowGrid(bool isShow);
		virtual void Update();

	private:
		CEditAnimate*  m_pAnimate;
		CGridNode*	m_pGridNode;
		bool m_bPlayState;
	};

}
