#include "stdafx.h"
#include "Vector3MapCtrlImp.h" 
#include "CCombinCmd.h"
namespace sqr_tools
{
	class CVecMapInsCmd : public CPropBaseCmd<Vector3MapCtrlImp>
	{
	public:
		CVecMapInsCmd( Vector3MapCtrlImp* pCtrl, float pos, CVector3f* val )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			if(val!=NULL)
			{
				m_vData = *val;
			}
			else
				m_vData = CVector3f(1.0f,1.0f,1.0f);
		
		}

		void _Execute(void)
		{
			((CVector3fMapData*)(GetCtrl()->m_pCtrlData->m_pData))->InsertMapNode(m_fPos, m_vData);
			GetCtrl()->m_fCurPercent = m_fPos;
			GetCtrl()->OnDataChanged();
		}

		void _UnExecute(void)
		{
			((CVector3fMapData*)(GetCtrl()->m_pCtrlData->m_pData))->EraseMapNode(m_fPos);
			GetCtrl()->OnDataChanged();
		}

	protected:
		float		m_fPos;
		CVector3f	m_vData;
	};

	class CVecMapDelCmd : public CPropBaseCmd<Vector3MapCtrlImp>
	{
	public:
		CVecMapDelCmd( Vector3MapCtrlImp* pCtrl, float pos )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			CVector3f* pVec = ((CVector3fMapData*)(GetCtrl()->m_pCtrlData->m_pData))->GetVector3Node(m_fPos);
			if( pVec )
				m_vData = *pVec;
		}

		void _Execute(void)
		{
			((CVector3fMapData*)(GetCtrl()->m_pCtrlData->m_pData))->EraseMapNode(m_fPos);
			GetCtrl()->OnDataChanged();
		}

		void _UnExecute(void)
		{
			((CVector3fMapData*)(GetCtrl()->m_pCtrlData->m_pData))->InsertMapNode(m_fPos, m_vData);
			GetCtrl()->m_fCurPercent = m_fPos;
			GetCtrl()->OnDataChanged();
		}

	protected:
		float		m_fPos;
		CVector3f	m_vData;
	};;

	DEF_UI_IMP(Vector3MapCtrlImp);

	Vector3MapCtrlImp::Vector3MapCtrlImp(void* param)
		: Vector3EditCtrlImp(param)
		, m_fCurPercent(0.0f)
	{
		
	}

	Vector3MapCtrlImp::~Vector3MapCtrlImp(void)
	{
		
	}

	int Vector3MapCtrlImp::GetVector3MapCount()
	{
		if (m_pCtrlData)
			return ((CVector3fMapData*)(m_pCtrlData->m_pData))->GetVector3MapCount();
		return 0;
	}

	float Vector3MapCtrlImp::GetVector3MapPer(int n)
	{
		if (m_pCtrlData)
			return ((CVector3fMapData*)(m_pCtrlData->m_pData))->GetVector3MapIndex(n);
		return 0.0f;
	}

	float Vector3MapCtrlImp::GetVector3MapXVal(float idx)
	{
		if (m_pCtrlData)
			return ((CVector3fMapData*)(m_pCtrlData->m_pData))->GetVector3MapXVal(idx);
		return 0.0f;; 
	}

	float Vector3MapCtrlImp::GetVector3MapYVal(float idx)
	{
		if (m_pCtrlData)
			return ((CVector3fMapData*)(m_pCtrlData->m_pData))->GetVector3MapYVal(idx);
		return 0.0f;;
	}

	float Vector3MapCtrlImp::GetVector3MapZVal(float idx)
	{
		if (m_pCtrlData)
			return ((CVector3fMapData*)(m_pCtrlData->m_pData))->GetVector3MapZVal(idx);
		return 0.0f;
	}

	float	Vector3MapCtrlImp::GetImpXValue()
	{
		return GetVector3MapXVal(m_fCurPercent);
	}

	float	Vector3MapCtrlImp::GetImpYValue()
	{
		return GetVector3MapYVal(m_fCurPercent);
	}

	float	Vector3MapCtrlImp::GetImpZValue()
	{
		return GetVector3MapZVal(m_fCurPercent);
	}

	void Vector3MapCtrlImp::SetCurPercent(float per)
	{
		m_fCurPercent = per;
	}

	float Vector3MapCtrlImp::GetCurPercent()
	{
		return m_fCurPercent;
	}


	CVector3f* Vector3MapCtrlImp::GetVector3Node(float idx)
	{
		if (m_pCtrlData)
			return ((CVector3fMapData*)(m_pCtrlData->m_pData))->GetVector3Node(idx);
		return NULL;
	}


	void Vector3MapCtrlImp::SetCurVecToPer(float per)
	{
		if (m_pCtrlData)
		{
			if (per != m_fCurPercent)
			{
				// 插入新节点，删除老节点
				CVector3f* vec = ((CVector3fMapData*)(m_pCtrlData->m_pData))->GetVector3Node(m_fCurPercent);

				float PreCur = m_fCurPercent;
				m_fCurPercent = per;

				CCombinCmd* pCmd = new CCombinCmd;
				pCmd->Push(new CVecMapInsCmd(this,m_fCurPercent,vec));
				pCmd->Push(new CVecMapDelCmd(this,PreCur));
				pCmd->Execute();	
			}
		}
	}

	void Vector3MapCtrlImp::DelCurPerNode()
	{
		if (m_pCtrlData)
			(new CVecMapDelCmd(this,m_fCurPercent))->Execute();
	}

	void Vector3MapCtrlImp::InsertNewNode(float idx)
	{
		if (m_pCtrlData)
		{
			(new CVecMapInsCmd(this,idx,GetVector3Node(idx)))->Execute();
		}
	}

	void	Vector3MapCtrlImp::_SetImpXValue(float x)
	{
		CVector3f* pVec = GetVector3Node(m_fCurPercent);
		if (pVec)
		{
			pVec->x = x;
			OnDataChanged();
		}
	}

	void	Vector3MapCtrlImp::_SetImpYValue(float y)
	{
		CVector3f* pVec = GetVector3Node(m_fCurPercent);
		if (pVec)
		{
			pVec->y = y;
			OnDataChanged();
		}
	}

	void	Vector3MapCtrlImp::_SetImpZValue(float z)
	{
		CVector3f* pVec = GetVector3Node(m_fCurPercent);
		if (pVec)
		{
			pVec->z = z;	
			OnDataChanged();
		}
	}
}