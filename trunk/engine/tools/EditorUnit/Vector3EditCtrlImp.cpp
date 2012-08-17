#include "stdafx.h"
#include "Vector3EditCtrlImp.h" 
#include "CVector3.h"
#include "CVector2.h"
#include "CTemporaryCmd.h"
namespace sqr_tools
{
	class CVecCmd  : public CPropBaseCmd<Vector3EditCtrlImp>
	{
	public:
		enum CmdVecType
		{
			Edit_X,
			Edit_Y,
			Edit_Z,
		};

	public:
		CVecCmd( Vector3EditCtrlImp* pCtrl, float value, CmdVecType type )
			: CPropBaseCmd(pCtrl)
		{
			m_Type = type;
			switch(m_Type)
			{
			case Edit_X:
				m_bOld = GetCtrl()->GetImpXValue();
				break;
			case Edit_Y:
				m_bOld = GetCtrl()->GetImpYValue();
				break;
			case Edit_Z:
				m_bOld = GetCtrl()->GetImpZValue();
				break;
			}
			m_bData = value;
			Execute();
		}

		void _Execute(void)
		{
			switch(m_Type)
			{
			case Edit_X:
				GetCtrl()->_SetImpXValue(m_bData);
				break;
			case Edit_Y:
				GetCtrl()->_SetImpYValue(m_bData);
				break;
			case Edit_Z:
				GetCtrl()->_SetImpZValue(m_bData);
				break;
			}
		}

		void _UnExecute(void)
		{
			switch(m_Type)
			{
			case Edit_X:
				GetCtrl()->_SetImpXValue(m_bOld);
				break;
			case Edit_Y:
				GetCtrl()->_SetImpYValue(m_bOld);
				break;
			case Edit_Z:
				GetCtrl()->_SetImpZValue(m_bOld);
				break;
			}
		}

	protected:
		CmdVecType			m_Type;
		float				m_bOld;
		float				m_bData;
	};

	DEF_UI_IMP(Vector3EditCtrlImp);
	Vector3EditCtrlImp::Vector3EditCtrlImp(void* param)
		: m_fz(0.0f)
	{
	}

	Vector3EditCtrlImp::~Vector3EditCtrlImp(void)
	{
	}

	void Vector3EditCtrlImp::SetImpXValue(float x)
	{
		if (m_pCtrlData&& GetImpXValue() != x)
		{
			new CVecCmd(this,x,CVecCmd::Edit_X);
		}
	}

	void Vector3EditCtrlImp::SetImpYValue(float y)
	{
		if (m_pCtrlData&& GetImpYValue() != y)
		{
			new CVecCmd(this,y,CVecCmd::Edit_Y);
		}
	}

	void Vector3EditCtrlImp::SetImpZValue(float z)
	{
		if (m_pCtrlData && GetImpZValue() != z)
		{
			new CVecCmd(this,z,CVecCmd::Edit_Z);
		}
	}

	void Vector3EditCtrlImp::_SetImpXValue(float x)
	{
		if (m_pCtrlData && GetImpXValue() != x)
		{
			if (m_pCtrlData->m_eDataType == E_VECTOR )
				(*((CVector3f*)(m_pCtrlData->m_pData))).x = x;
			else
				(*((CVector2f*)(m_pCtrlData->m_pData))).x = x;
			OnDataChanged();
		}
	}

	void Vector3EditCtrlImp::_SetImpYValue(float y)
	{
		if (m_pCtrlData && GetImpYValue() != y)
		{
			if (m_pCtrlData->m_eDataType == E_VECTOR )
				(*((CVector3f*)(m_pCtrlData->m_pData))).y = y;
			else
				(*((CVector2f*)(m_pCtrlData->m_pData))).y = y;
			OnDataChanged();
		}
	}

	void Vector3EditCtrlImp::_SetImpZValue(float z)
	{
		if (m_pCtrlData&& GetImpZValue() != z)
		{
			if (m_pCtrlData->m_eDataType == E_VECTOR )
				(*((CVector3f*)(m_pCtrlData->m_pData))).z = z;
			else
				m_fz = z;

			OnDataChanged();
		}
	}

	float Vector3EditCtrlImp::GetImpXValue()
	{
		if (m_pCtrlData)
		{
			if (m_pCtrlData->m_eDataType == E_VECTOR )
				return (*((CVector3f*)(m_pCtrlData->m_pData))).x;
			else
				return (*((CVector2f*)(m_pCtrlData->m_pData))).x;
		}
		return 0.0f;
	}

	float Vector3EditCtrlImp::GetImpYValue()
	{
		if (m_pCtrlData)
		{
			if (m_pCtrlData->m_eDataType == E_VECTOR )
				return (*((CVector3f*)(m_pCtrlData->m_pData))).y;
			else
				return (*((CVector2f*)(m_pCtrlData->m_pData))).y;
		}
			
		return 0.0f;
	}

	float Vector3EditCtrlImp::GetImpZValue()
	{
		if (m_pCtrlData)
		{
			if (m_pCtrlData->m_eDataType == E_VECTOR )
				return (*((CVector3f*)(m_pCtrlData->m_pData))).z;
			else
				return m_fz;
		}
			
		return 0.0f;
	}		
}
