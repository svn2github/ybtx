#include "StdAfx.h"
#include "MapSetDlg.h"
#include "CMapEditApp.h"
#include "CMapMgr.h"

namespace MapEditor 
{

System::Void MapSetDlg::btNewOK_Click(System::Object^  sender, System::EventArgs^  e)
{
	int w=this->MapCtrl->GetMapWidth();
	int h=this->MapCtrl->GetMapHeight();
	System::String^ str=this->MapCtrl->GetMapName();
	System::String^ strH=this->MapCtrl->GetHMapName();
	if(w!=0 && h!=0 && w%4==0 && h%4==0)
	{
		CMapEditApp::GetInst()->GetMapMgr()->CreateMap(w,h);
	}
	else
		System::Windows::Forms::MessageBox::Show("ERROR 宽或者高为0 或者不是4的倍数");
	this->Close();

}

System::Void MapSetDlg::btNewCancle_Click(System::Object^  sender, System::EventArgs^  e)
{
	this->Close();
}

System::Void MapSetDlg::btExOK_Click(System::Object^  sender, System::EventArgs^  e)
{
	int srcW=4;
	int srcL=4;
	int tmpW=this->ExMapCtrl->GetExMapWidth();
	int tmpL=this->ExMapCtrl->GetExMapLength();
	int offW=0;
	int offL=0;
	if(this->ExMapCtrl->GetExMapWidthDir()==0)
	{
		//非正向扩展
		offW=tmpW;
		tmpW=srcW+tmpW*2;
	}
	else if(this->ExMapCtrl->GetExMapWidthDir()==1)
	{
		tmpW=srcW+tmpW;
	}
	else
	{
		offW=tmpW;
		tmpW=srcW+tmpW;
	}

	if(this->ExMapCtrl->GetExMapLengthDir()==0)
	{
		//非正向扩展
		offL=tmpL;
		tmpL=srcL+tmpL*2;
	}
	else if(this->ExMapCtrl->GetExMapLengthDir()==1)
	{
		tmpL=srcL+tmpL;
	}
	else
	{
		offL=tmpL;
		tmpL=srcL+tmpL;
	}

	CMapEditApp::GetInst()->GetMapMgr()->ResizeMap(tmpW,tmpL,offW,offL);

	this->Close();
}

System::Void MapSetDlg::btExCancle_Click(System::Object^  sender, System::EventArgs^  e)
{

	this->Close();
}


//!namespace
}