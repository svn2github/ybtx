#include "StdAfx.h"
#include "Vector3EditCtrl.h"
#include "Vector3EditCtrlImp.h"
#include "Vector3EditDialog.h"

namespace sqr_tools
{
	char* Vector3EditCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(Vector3EditCtrlImp);
	}

	System::Void  Vector3EditCtrl::InitOtherComponent()
	{
		
	}

	System::Void  Vector3EditCtrl::DestroyOtherComponent()
	{
	}

	void	Vector3EditCtrl::Update(void)
	{
		// 根据数据类型，设置控件的属性
		Vector3EditCtrlImp* Vector3Editimp = (Vector3EditCtrlImp*)m_pCtrl;

		this->VectorXUpDown->Location = System::Drawing::Point(6, 3);
		this->VectorYUpDown->Location = System::Drawing::Point(81, 3);
		this->VectorZUpDown->Location = System::Drawing::Point(156, 3);
		this->VectorEditButton->Location = System::Drawing::Point(231, 3);
		this->Size = System::Drawing::Size(324, 28);

		// content
		Double fx = Vector3Editimp->GetImpXValue();
		Double fy = Vector3Editimp->GetImpYValue();
		Double fz = Vector3Editimp->GetImpZValue();
		this->VectorXUpDown->Text = fx.ToString();
		this->VectorYUpDown->Text = fy.ToString();
		this->VectorZUpDown->Text = fz.ToString();
	}

	System::Void Vector3EditCtrl::VectorEditButton_Click( System::Object^ sender, System::EventArgs^ e )
	{
		// 编辑向量
		Vector3EditCtrlImp* Vector3Editimp = (Vector3EditCtrlImp*)m_pCtrl;
		Vector3EditDialog m_Vec3Dlg;
		m_Vec3Dlg.SetVecEditCtrl(Vector3Editimp);
		m_Vec3Dlg.ShowDialog();
	}

	System::Void Vector3EditCtrl::VectorXUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		float val = (float)(Convert::ToDouble(VectorXUpDown->Value));
		Vector3EditCtrlImp* VecEditimp = (Vector3EditCtrlImp*)m_pCtrl;
		VecEditimp->SetImpXValue(val);
	}

	System::Void Vector3EditCtrl::VectorYUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		float val = (float)(Convert::ToDouble(VectorYUpDown->Value));
		Vector3EditCtrlImp* VecEditimp = (Vector3EditCtrlImp*)m_pCtrl;
		VecEditimp->SetImpYValue(val);
	}

	System::Void Vector3EditCtrl::VectorZUpDown_ValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		float val = (float)(Convert::ToDouble(VectorZUpDown->Value));
		Vector3EditCtrlImp* VecEditimp = (Vector3EditCtrlImp*)m_pCtrl;
		VecEditimp->SetImpZValue(val);
	}
}