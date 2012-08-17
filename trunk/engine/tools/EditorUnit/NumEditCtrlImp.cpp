#include "stdafx.h"
#include "NumEditCtrlImp.h" 

namespace sqr_tools
{
	class CNumIntCmd : public CPropBaseCmd<NumEditCtrlImp>
	{
	public:
		CNumIntCmd( NumEditCtrlImp* pCtrl, int value )
			: CPropBaseCmd(pCtrl)
		{
			m_bOld = GetCtrl()->GetImpIntValue();
			m_bData = value;
			Execute();
		}

		void _Execute(void)
		{
			GetCtrl()->_SetImpIntValue(m_bData);
		}

		void _UnExecute(void)
		{
			GetCtrl()->_SetImpIntValue(m_bOld);
		}

	protected:
		int		m_bOld;
		int		m_bData;
	};
	
	class CNumByteCmd : public CPropBaseCmd<NumEditCtrlImp>
	{
	public:
		CNumByteCmd( NumEditCtrlImp* pCtrl, int value )
			: CPropBaseCmd(pCtrl)
		{
			m_bOld = GetCtrl()->GetImpByteValue();
			m_bData = value;
			Execute();
		}

		void _Execute(void)
		{
			GetCtrl()->_SetImpByteValue(m_bData);
		}

		void _UnExecute(void)
		{
			GetCtrl()->_SetImpByteValue(m_bOld);
		}

	protected:
		int		m_bOld;
		int		m_bData;
	};

	class CNumFloatCmd : public CPropBaseCmd<NumEditCtrlImp>
	{
	public:
		CNumFloatCmd( NumEditCtrlImp* pCtrl, float value )
			: CPropBaseCmd(pCtrl)
		{
			m_bOld = GetCtrl()->GetImpFloatValue();
			m_bData = value;
			Execute();
		}

		void _Execute(void)
		{
			GetCtrl()->_SetImpFloatValue(m_bData);
		}

		void _UnExecute(void)
		{
			GetCtrl()->_SetImpFloatValue(m_bOld);
		}

	protected:
		float		m_bOld;
		float		m_bData;
	};

	DEF_UI_IMP(NumEditCtrlImp);

	NumEditCtrlImp::NumEditCtrlImp(void* param)
	{
	}

	NumEditCtrlImp::~NumEditCtrlImp(void)
	{
	}

	void NumEditCtrlImp::SetImpIntValue(int i)
	{
		if (m_pCtrlData && GetImpIntValue() != i )
		{
			new CNumIntCmd(this,i);
		}
	}

	void NumEditCtrlImp::SetImpByteValue(int i)
	{
		if (m_pCtrlData && GetImpByteValue() != i )
		{
			new CNumByteCmd(this,i);
		}
	}

	void NumEditCtrlImp::SetImpFloatValue(float f)
	{
		if (m_pCtrlData && GetImpFloatValue() != f)
		{
			new CNumFloatCmd(this,f);
		}
	}

	void NumEditCtrlImp::_SetImpIntValue(int i)
	{
		if (m_pCtrlData && GetImpIntValue() != i )
		{
			(*((int*)(m_pCtrlData->m_pData))) = i;
			OnDataChanged();
		}
	}

	void NumEditCtrlImp::_SetImpByteValue(int i)
	{
		if (m_pCtrlData && GetImpByteValue() != i )
		{
			(*((BYTE*)(m_pCtrlData->m_pData))) = (BYTE)i;
			OnDataChanged();
		}
	}

	void NumEditCtrlImp::_SetImpFloatValue(float f)
	{
		if (m_pCtrlData && GetImpFloatValue() != f)
		{
			(*((float*)(m_pCtrlData->m_pData))) = f;
			OnDataChanged();
		}
	}

	int NumEditCtrlImp::GetImpIntValue()
	{
		if (m_pCtrlData)
		{
			return (int)(*((int*)(m_pCtrlData->m_pData)));
		}
		return 0;
	}

	int NumEditCtrlImp::GetImpByteValue()
	{
		if (m_pCtrlData)
		{
			return (int)(*((BYTE*)(m_pCtrlData->m_pData)));
		}
		return 0;
	}

	float NumEditCtrlImp::GetImpFloatValue()
	{
		if (m_pCtrlData)
		{
			return (float)(*((float*)m_pCtrlData->m_pData));
		}
		return 0.0f;
	}
}
