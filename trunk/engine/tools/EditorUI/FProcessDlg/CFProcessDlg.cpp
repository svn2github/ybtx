#include "StdAfx.h"
#include "CFProcessDlg.h"
#include "FileProcessCtrl/FileProcessCtrlImp.h"
#include "ProgressCtrl/CProgressCtrlImp.h"

CFileProcessCtrlImp *pImp = NULL;

System::Void CFProcessDlg::CFProcessDlg_FormClosed( System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e )
{
	if(this->Visible)
	{
		this->Visible=false;
	}
	//if( pImp )
	//{
	//	delete pImp;
	//	pImp = NULL;
	//}
}

void CFProcessDlg::BindPathBrowserCtrl()
{
	//pImp = new CFileProcessCtrlImp;
	////this->cFileProcessCtrl1->SetCoreCtrl( (void*)pImp ) ;

	//CProgressCtrlImp *pCurProgressImp	= pImp->GetCurProgressImp();
	//CProgressCtrlImp *pTotalProgressImp	= pImp->GetTotalProgressImp();
	//this->cFileProcessCtrl1->SetCurProgressCtrl((void*)pCurProgressImp);
	//this->cFileProcessCtrl1->SetTotalProgressCtrl((void*)pTotalProgressImp);
};

void CFProcessDlg::UpdateCtrl()
{
	this->cFileProcessCtrl1->Update();
}


