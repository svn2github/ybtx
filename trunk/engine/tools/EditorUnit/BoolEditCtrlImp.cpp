#include "stdafx.h"
#include "BoolEditCtrlImp.h" 

namespace sqr_tools
{
	class CBoolEditCmd : public CPropBaseCmd<BoolEditCtrlImp>
	{
	public:
		CBoolEditCmd( BoolEditCtrlImp* pCtrl, bool b )
			: CPropBaseCmd(pCtrl)
		{
			m_bOld = GetCtrl()->GetImpBoolValue();
			m_bData = b;
			Execute();
		}

		void _Execute(void)
		{
			GetCtrl()->_SetBoolValue(m_bData);
		}

		void _UnExecute(void)
		{
			GetCtrl()->_SetBoolValue(m_bOld);
		}

	protected:
		bool	m_bOld;
		bool	m_bData;
	};

	DEF_UI_IMP(BoolEditCtrlImp);

	BoolEditCtrlImp::BoolEditCtrlImp(void* param)
	{
	}

	BoolEditCtrlImp::~BoolEditCtrlImp(void)
	{
	}

	void BoolEditCtrlImp::SetImpBoolValue(bool b)
	{
		if( m_pCtrlData && GetImpBoolValue()!= b )
		{
			new CBoolEditCmd(this,b);
		}
	}

	bool BoolEditCtrlImp::GetImpBoolValue()
	{
		if (m_pCtrlData)
		{
			return (bool)(*((bool*)(m_pCtrlData->m_pData)));
		}
		return false;
	}

	void BoolEditCtrlImp::_SetBoolValue(bool b)
	{
		if (m_pCtrlData && GetImpBoolValue()!= b )
		{
			*(bool*)(m_pCtrlData->m_pData) = b;
			OnDataChanged();
		}
	}
}
