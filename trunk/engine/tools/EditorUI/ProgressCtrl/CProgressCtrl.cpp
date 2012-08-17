#include "StdAfx.h"
#include "CProgressCtrl.h"
#include "ProgressCtrl/CProgressCtrlImp.h"

void sqr_tools::CProgressCtrl::Update( void )
{
	CProgressCtrlImp *pImp = (CProgressCtrlImp*)m_pCtrl;
	if( pImp )
	{
		int nMax = pImp->GetProgressMaxValue();
		if( nMax != 0 )
		{
			this->progressBar1->Step	= pImp->GetProgressStep();
			this->progressBar1->Maximum = pImp->GetProgressMaxValue();
			this->progressBar1->Minimum	= pImp->GetProgressMinValue();
		}
		this->progressBar1->Value = pImp->GetProgressValue();

		Refresh();
	}
}
