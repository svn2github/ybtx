#include "stdafx.h"
#include "ComBoEditCtrlImp.h" 
namespace sqr_tools
{
	class ComBoCmd : public CPropBaseCmd<ComBoEditCtrlImp>
	{
	public:
		ComBoCmd( ComBoEditCtrlImp* pCtrl, int value)
			: CPropBaseCmd(pCtrl)
		{
			m_bOld = GetCtrl()->GetImpDataValue();
			m_bData = value;
			Execute();
		}

		void _Execute(void)
		{
			GetCtrl()->_SetImpDataValue(m_bData);
		}

		void _UnExecute(void)
		{
			GetCtrl()->_SetImpDataValue(m_bOld);
		}

	protected:
		int	m_bOld;
		int	m_bData;
	};

	DEF_UI_IMP(ComBoEditCtrlImp);

	ComBoEditCtrlImp::ComBoEditCtrlImp(void* param)
	{
	}

	ComBoEditCtrlImp::~ComBoEditCtrlImp(void)
	{
	}

	void ComBoEditCtrlImp::SetImpDataValue(int value)
	{
		if (m_pCtrlData && GetImpDataValue()!=value)
		{
			new ComBoCmd(this,value);
		}
	}

	int ComBoEditCtrlImp::GetImpDataValue()
	{
		if (m_pCtrlData)
		{
			return *(int*)((CComboCtrlData*)(m_pCtrlData->m_pData))->m_pVal;
		}
		return 0;
	}

	CCombDataHelper* ComBoEditCtrlImp::GetImpDataItems()
	{
		if (m_pCtrlData)
		{
			return ((CComboCtrlData*)(m_pCtrlData->m_pData))->m_pHelper;
		}
		return NULL;
	}

	void ComBoEditCtrlImp::_SetImpDataValue( int value )
	{
		if (m_pCtrlData && GetImpDataValue()!=value)
		{
			*(int*)((CComboCtrlData*)(m_pCtrlData->m_pData))->m_pVal = value;
			OnDataChanged();
		}
	}
}
