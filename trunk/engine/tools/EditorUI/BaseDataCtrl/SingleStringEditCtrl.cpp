#include "StdAfx.h"
#include "SingleStringEditCtrl.h"
#include "SingleStringEditCtrlImp.h"
#include "CLRStringHelper.h"
#include "CueListDialog.h"
#include "BaseDataCtrl/CueListDialog.h"

namespace sqr_tools
{
	char* SingleStringEditCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(SingleStringEditCtrlImp);
	}

	System::Void  SingleStringEditCtrl::InitOtherComponent()
	{
	}

	System::Void  SingleStringEditCtrl::DestroyOtherComponent()
	{
	}

	void	SingleStringEditCtrl::SetCueListDlg(CueListDialog^ cueListDlg)
	{
		m_CueListDlg = cueListDlg;
	}

	void	SingleStringEditCtrl::Update(void)
	{
		// 根据数据类型，设置控件的属性
		SingleStringEditCtrlImp* StrEditimp = (SingleStringEditCtrlImp*)m_pCtrl;
		this->EditStringtextBox->Location = System::Drawing::Point(4, 3);
		this->Size = System::Drawing::Size(273, 30);
		ECtrlDataType tp = StrEditimp->GetDataType();
		if (tp == E_STRINGBUTTON || tp == E_ESTRINGBUTTON || tp == E_CUEESTRING)
		{
			ReadButton->Show();
			if (StrEditimp->GetDataHelpInfo())
			{
				// 设置FILTER
				String^ clrFilter = gcnew String(StrEditimp->GetDataHelpInfo());
				StrOpenFileDialog->Filter = clrFilter;
			}
			this->Size = System::Drawing::Size(339, 30);
		}
		else
		{
			ReadButton->Hide();
			this->Size = System::Drawing::Size(273, 30);
		}

		// content
		System::String^ clrStr = gcnew System::String(StrEditimp->GetImpSingleStrValue());
		this->EditStringtextBox->Text = clrStr;
	}

	System::Void SingleStringEditCtrl::EditStringtextBox_Leave( System::Object^ sender, System::EventArgs^ e )
	{
		_SetImpData();
	}

	System::Void SingleStringEditCtrl::EditStringtextBox_KeyDown( System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e )
	{
		if (e->KeyCode == Keys::Return)
				_SetImpData();
	}

	void SingleStringEditCtrl::_SetImpData()
	{
		System::String^ clrStr = this->EditStringtextBox->Text;
		std::string stdstr;
		CLRMarshalString( clrStr, stdstr);
		SingleStringEditCtrlImp* StrEditimp = (SingleStringEditCtrlImp*)m_pCtrl;
		StrEditimp->SetImpSingleStrValue(stdstr.c_str());
	}


	System::Void SingleStringEditCtrl::ReadButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		// 读文件
		// 设置字符串
		SingleStringEditCtrlImp* StrEditimp = (SingleStringEditCtrlImp*)m_pCtrl;
		ECtrlDataType tp = StrEditimp->GetDataType();
		if (tp == E_CUEESTRING)
		{
			// 处理的应该是音效打开窗口
			if (m_CueListDlg)
			{
				m_CueListDlg->SetCueEditData(StrEditimp, StrEditimp->GetImpSingleStrValue());
				m_CueListDlg->ShowDialog();
			}
			
		}
		else
		{
			if (StrOpenFileDialog->ShowDialog() == DialogResult::OK)
			{
				for ( int i = 0; i < StrOpenFileDialog->FileNames->Length; ++i )
				{
					String^ strClr = StrOpenFileDialog->FileNames[i];
					std::string strCPlus;
					CLRMarshalString(strClr, strCPlus);

					StrEditimp->SetImpStrWithFullPath(strCPlus.c_str());
				}
			}
		}
		Update();
	}
}