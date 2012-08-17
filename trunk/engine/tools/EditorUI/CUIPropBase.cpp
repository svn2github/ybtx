#include "StdAfx.h"
#include "CUIPropBase.h"
#include "CCtrlPropBase.h"

namespace sqr_tools {

	CUIPropBase::CUIPropBase(void)
	{
	}

	CUIPropBase::~CUIPropBase(void)
	{
	}

	void CUIPropBase::SetContentData(void* content)
	{
		CCtrlPropBase* CtrlProp = (CCtrlPropBase*)m_pCtrl;
		CtrlProp->SetEditCtrlData(content);	
	}
}