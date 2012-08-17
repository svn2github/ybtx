#include "StdAfx.h"
#include "ClrMsgDefine.h"
#include "CLightView.h"
#include "LightView\CLightViewImp.h"

char*	CLightView::GetImpTypeName(void)
{
	return GET_IMP_NAME(CLightViewImp);
}

void*	CLightView::GetImpParam(void)
{
	return this->m_Light3DView->Handle.ToPointer();
}

System::Void CLightView::UpdateTimer()
{
	CLightViewImp *pImp = (CLightViewImp*)m_pCtrl;
	if(pImp)
	{
		pImp->UpdateCamPos();
	}
}

System::Void CLightView::btColorSet_Click(System::Object^  sender, System::EventArgs^  e) 
{
	CLightViewImp *pImp = (CLightViewImp*)m_pCtrl;
	if(this->ColorSetDlg->ShowDialog()==DialogResult::OK)
	{
		if(this->rbtGAColor->Checked==true)
			pImp->SetAmbientColor(ColorSetDlg->Color.ToArgb());
		if(this->rbtGSColor->Checked==true)
			pImp->SetSunColor(ColorSetDlg->Color.ToArgb());
		if(this->rbtRAColor->Checked==true)
			pImp->SetPlayerAmbientColor(ColorSetDlg->Color.ToArgb());
		if(this->rbtRSColor->Checked==true)
			pImp->SetPlayerSunColor(ColorSetDlg->Color.ToArgb());
	}
}

System::Void CLightView::btApp_Click(System::Object^  sender, System::EventArgs^  e)
{
	CLightViewImp *pImp = (CLightViewImp*)m_pCtrl;
	if(pImp)
		pImp->Ensure();
}

System::Void CLightView::btReset_Click(System::Object^  sender, System::EventArgs^  e)
{
	CLightViewImp *pImp = (CLightViewImp*)m_pCtrl;
	if(pImp)
		pImp->Reset();
}

System::Void CLightView::m_Light3DView_MouseEnter(System::Object^  sender, System::EventArgs^  e)
{
	this->m_Light3DView->Focus();
}

System::Void CLightView::m_Light3DView_MouseLeave(System::Object^  sender, System::EventArgs^  e)
{
}

System::Void CLightView::m_Light3DView_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	System::Windows::Forms::Message tmpMsg;
	tmpMsg.HWnd=this->Handle;
	tmpMsg.Msg=WM_MOUSEMOVE;
	int tmpwParam=0;
	if(e->Button == System::Windows::Forms::MouseButtons::Left)
		tmpwParam=tmpwParam&0x0001;
	if(e->Button == System::Windows::Forms::MouseButtons::Right)
		tmpwParam=tmpwParam&0x0002;
	if(e->Button == System::Windows::Forms::MouseButtons::Middle)
		tmpwParam=tmpwParam&0x0010;

	int tmplParam=0;
	tmplParam=e->Y;
	tmplParam=(tmplParam<<16)|(e->X);
	tmpMsg.WParam=(System::IntPtr)tmpwParam;
	tmpMsg.LParam=(System::IntPtr)tmplParam;

	this->WndProc(tmpMsg);
}
System::Void CLightView::m_Light3DView_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	System::Windows::Forms::Message tmpMsg;
	tmpMsg.HWnd=this->Handle;
	int tmpwParam=0;
	if(e->Button == System::Windows::Forms::MouseButtons::Left)
	{
		tmpMsg.Msg=WM_LBUTTONDOWN;
		tmpwParam=tmpwParam&0x0001;
	}
	if(e->Button == System::Windows::Forms::MouseButtons::Right)
	{
		tmpMsg.Msg=WM_RBUTTONDOWN;
		tmpwParam=tmpwParam&0x0002;
	}
	if(e->Button == System::Windows::Forms::MouseButtons::Middle)
	{
		tmpMsg.Msg=WM_MBUTTONDOWN;
		tmpwParam=tmpwParam&0x0010;
	}
	int tmplParam=0;
	tmplParam=e->Y;
	tmplParam=(tmplParam<<16)|(e->X);
	tmpMsg.WParam=(System::IntPtr)tmpwParam;
	tmpMsg.LParam=(System::IntPtr)tmplParam;

	this->WndProc(tmpMsg);
}
System::Void CLightView::m_Light3DView_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) 
{
	System::Windows::Forms::Message tmpMsg;
	tmpMsg.HWnd=this->Handle;
	int tmpwParam=0;
	if(e->Button == System::Windows::Forms::MouseButtons::Left)
	{
		tmpMsg.Msg=WM_LBUTTONUP;
		tmpwParam=tmpwParam&0x0001;
	}
	if(e->Button == System::Windows::Forms::MouseButtons::Right)
	{
		tmpMsg.Msg=WM_RBUTTONUP;
		tmpwParam=tmpwParam&0x0002;
	}
	if(e->Button == System::Windows::Forms::MouseButtons::Middle)
	{
		tmpMsg.Msg=WM_MBUTTONUP;
		tmpwParam=tmpwParam&0x0010;
	}
	int tmplParam=0;
	tmplParam=e->Y;
	tmplParam=(tmplParam<<16)|(e->X);
	tmpMsg.WParam=(System::IntPtr)tmpwParam;
	tmpMsg.LParam=(System::IntPtr)tmplParam;

	this->WndProc(tmpMsg);
}
System::Void CLightView::m_Light3DView_MouseWheel(System::Object^  sender, System::EventArgs^  e)
{

}