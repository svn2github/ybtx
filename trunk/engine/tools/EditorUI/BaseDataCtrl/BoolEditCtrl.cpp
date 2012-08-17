#include "StdAfx.h"
#include "BoolEditCtrl.h"
#include "BoolEditCtrlImp.h"

namespace sqr_tools
{
	char* BoolEditCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(BoolEditCtrlImp);
	}

	System::Void  BoolEditCtrl::InitOtherComponent()
	{
	}

	System::Void  BoolEditCtrl::DestroyOtherComponent()
	{
	}

	void	BoolEditCtrl::Update(void)
	{
		// 根据数据类型，设置控件的属性
		BoolEditCtrlImp* BoolEditimp = (BoolEditCtrlImp*)m_pCtrl;
		this->DataCheckBox->Location = System::Drawing::Point(4, 3);
		this->Size = System::Drawing::Size(52, 26);
		// content
		bool check = BoolEditimp->GetImpBoolValue();
		if (check)
		{
			DataCheckBox->Text = "是";
			DataCheckBox->CheckState = System::Windows::Forms::CheckState::Checked;
		}
		else
		{
			DataCheckBox->Text = "否";
			DataCheckBox->CheckState = System::Windows::Forms::CheckState::Unchecked;
		}
	}

	System::Void BoolEditCtrl::DataCheckBox_CheckStateChanged( System::Object^ sender, System::EventArgs^ e )
	{
		bool checkstate = false;
		if (DataCheckBox->CheckState == System::Windows::Forms::CheckState::Checked)
			checkstate = true;
		BoolEditCtrlImp* BoolEditimp = (BoolEditCtrlImp*)m_pCtrl;
		BoolEditimp->SetImpBoolValue(checkstate);
	}

	System::Void BoolEditCtrl::DataCheckBox_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		_KeyDown(sender,e);
	}

	System::Void BoolEditCtrl::DataCheckBox_KeyUp( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		_KeyUp(sender,e);
	}
}