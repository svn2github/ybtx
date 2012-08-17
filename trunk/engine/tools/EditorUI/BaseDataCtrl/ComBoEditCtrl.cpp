#include "StdAfx.h"
#include "ComBoEditCtrl.h"
#include "ComBoEditCtrlImp.h"
#include "CLRStringHelper.h" 
#include "AutoPropData.h"

namespace sqr_tools
{
	char* ComBoEditCtrl::GetImpTypeName( void )
	{
		return GET_IMP_NAME(ComBoEditCtrlImp);
	}

	System::Void  ComBoEditCtrl::InitOtherComponent()
	{
	}

	System::Void  ComBoEditCtrl::DestroyOtherComponent()
	{
	}

	void  ComBoEditCtrl::_ResetItem( int Value, CCombDataHelper* pDataHelper )
	{
		this->EditComboBox->Items->Clear();

		for ( int i=0; i<pDataHelper->GetItemNum(); ++i )
		{
			String^ sysStr = gcnew String(pDataHelper->GetItemName(i));
			this->EditComboBox->Items->Add(sysStr);
		}

		this->EditComboBox->SelectedIndex = pDataHelper->GetIdxByValue(Value);
	}

	void	ComBoEditCtrl::Update(void)
	{
		// 根据数据类型，设置控件的属性
		ComBoEditCtrlImp* ComBoEditimp = (ComBoEditCtrlImp*)m_pCtrl;

		this->EditComboBox->Location = System::Drawing::Point(4, 3);
		this->Size = System::Drawing::Size(175, 28);

		// content
		CCombDataHelper* pDataHelper = ComBoEditimp->GetImpDataItems();
		int value = ComBoEditimp->GetImpDataValue();
		
		if (pDataHelper)
			_ResetItem( value, pDataHelper );
	}

	System::Void ComBoEditCtrl::EditComboBox_SelectedValueChanged( System::Object^ sender, System::EventArgs^ e )
	{
		ComBoEditCtrlImp* ComBoEditimp = (ComBoEditCtrlImp*)m_pCtrl;
		CCombDataHelper* pDataHelper = ComBoEditimp->GetImpDataItems();
		ComBoEditimp->SetImpDataValue(pDataHelper->GetValueByIdx((int)this->EditComboBox->SelectedIndex));
	}
	
}
