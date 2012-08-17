#include "StdAfx.h"
#include "CViewCtrl.h"
#include "CCtrlBase.h"
#include "CCoreViewImp.h"

char*	CViewCtrl::GetImpTypeName(void)
{
	return GET_IMP_NAME(CCoreViewImp);
}

void*	CViewCtrl::GetImpParam(void)
{
	return this->Handle.ToPointer();
}

System::Void CViewCtrl::CViewCtrl_MouseEnter( System::Object^ sender, System::EventArgs^ e )
{
	this->Focus();
}