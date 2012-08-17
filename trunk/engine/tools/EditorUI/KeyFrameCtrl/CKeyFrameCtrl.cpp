#include "StdAfx.h"
#include "CKeyFrameCtrl.h"
#include "KeyFrameCtrl/CKeyFrameCtrlImp.h"

#define KeyFrameCtrlImp ((CKeyFrameCtrlImp*)GetCoreCtrl())

System::Void CKeyFrameCtrl::setFrameString_button_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( KeyFrameCtrlImp )
	{
		String^ strClr = this->frameString_textBox->Text;
		string strCPlus;
		strCPlus = MagStringToUnmagString(strClr);
		KeyFrameCtrlImp->SetKeyFrame(strCPlus);
	}
}

void CKeyFrameCtrl::Update( void )
{
	if( KeyFrameCtrlImp )
	{
		string strKeyFrame = KeyFrameCtrlImp->GetKeyFrame();
		this->frameString_textBox->Text = UnmagStringToMagString((char*)(strKeyFrame.c_str()));
	}
}

char* CKeyFrameCtrl::GetImpTypeName( void )
{
	return GET_IMP_NAME(CKeyFrameCtrlImp);
}

void* sqr_tools::CKeyFrameCtrl::GetImpParam( void )
{
	return this->Handle.ToPointer();
}

System::Void sqr_tools::CKeyFrameCtrl::SaveKeyInfoBtn_Click( System::Object^ sender, System::EventArgs^ e )
{
	if( KeyFrameCtrlImp )
	{
		String^ msg = gcnew String(KeyFrameCtrlImp->SaveKeyframeInfo().c_str());
		MessageBox::Show(msg, "ÌבÊ¾", MessageBoxButtons::OK);
	}
}