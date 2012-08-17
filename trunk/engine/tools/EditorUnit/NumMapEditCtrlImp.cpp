#include "stdafx.h"
#include "NumMapEditCtrlImp.h" 
#include "CMath.h"
#include "CCombinCmd.h"

#define NUM_MAP_MAX		99999.0f
#define NUM_MAP_MIN		-99999.0f
#define NUM_MAP_NORMAL_HEIGHT 50.0f
namespace sqr_tools
{
	class CNumSetCmd : public CPropBaseCmd<NumMapEditCtrlImp>
	{
	public:
		CNumSetCmd( NumMapEditCtrlImp* pCtrl, float pos, float val )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			m_bInsert = !((CFloatMapData*)(GetCtrl()->m_pCtrlData->m_pData))->GetMapValue(m_fPos,m_fOld);
			m_fData = val;
		}

		void _Execute(void)
		{
			((CFloatMapData*)(GetCtrl()->m_pCtrlData->m_pData))->SetMapData(m_fPos,m_fData);
			GetCtrl()->OnDataChanged();
		}

		void _UnExecute(void)
		{
			if(m_bInsert)
				((CFloatMapData*)(GetCtrl()->m_pCtrlData->m_pData))->EraseMapData(m_fPos);
			else
				((CFloatMapData*)(GetCtrl()->m_pCtrlData->m_pData))->SetMapData(m_fPos,m_fOld);
			GetCtrl()->OnDataChanged();
		}

	protected:
		float	m_fPos;
		float	m_fOld;
		float	m_fData;
		bool	m_bInsert;
	};

	class CNumDelCmd : public CPropBaseCmd<NumMapEditCtrlImp>
	{
	public:
		CNumDelCmd( NumMapEditCtrlImp* pCtrl, float pos )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			if( !((CFloatMapData*)(GetCtrl()->m_pCtrlData->m_pData))->GetMapValue(pos,m_fOld) )
				m_isValid = false;
		}

		void _Execute(void)
		{
			((CFloatMapData*)(GetCtrl()->m_pCtrlData->m_pData))->EraseMapData(m_fPos);
			GetCtrl()->OnDataChanged();
		}

		void _UnExecute(void)
		{
			((CFloatMapData*)(GetCtrl()->m_pCtrlData->m_pData))->SetMapData(m_fPos,m_fOld);
			GetCtrl()->OnDataChanged();
		}

	protected:
		float	m_fPos;
		float	m_fOld;
	};

	class CNumSetMaxCmd : public CPropBaseCmd<NumMapEditCtrlImp>
	{
	public:
		CNumSetMaxCmd( NumMapEditCtrlImp* pCtrl, float max )
			: CPropBaseCmd(pCtrl)
		{
			m_fMax = max;
			m_fOld = GetCtrl()->m_fMaxVal;
		}

		void _Execute(void)
		{
			GetCtrl()->m_fMaxVal = m_fMax;	
		}

		void _UnExecute(void)
		{
			GetCtrl()->m_fMaxVal = m_fOld;	
			GetCtrl()->OnDataChanged();
		}

	protected:
		float	m_fMax;
		float	m_fOld;
	};

	class CNumSetMinCmd : public CPropBaseCmd<NumMapEditCtrlImp>
	{
	public:
		CNumSetMinCmd( NumMapEditCtrlImp* pCtrl, float max )
			: CPropBaseCmd(pCtrl)
		{
			m_fMin = max;
			m_fOld = GetCtrl()->m_fMinVal;
		}

		void _Execute(void)
		{
			GetCtrl()->m_fMinVal = m_fMin;	
		}

		void _UnExecute(void)
		{
			GetCtrl()->m_fMinVal = m_fOld;	
			GetCtrl()->OnDataChanged();
		}

	protected:
		float	m_fMin;
		float	m_fOld;
	};

	DEF_UI_IMP(NumMapEditCtrlImp);
	NumMapEditCtrlImp::NumMapEditCtrlImp(void* param)
		: m_fMaxVal(NUM_MAP_NORMAL_HEIGHT)
		, m_fMinVal(-NUM_MAP_NORMAL_HEIGHT)
	{
		
	}

	NumMapEditCtrlImp::~NumMapEditCtrlImp(void)
	{
		
	}

	void NumMapEditCtrlImp::_ResetMapData(CCombinCmd* pCmd,float min,float max)
	{
		int ct = GetNumMapCount();
		for(int i = 0; i < ct; ++i)
		{
			float idx = GetNumMapIdx(i);
			float val = GetNumMapVal(i);
			val = limit2(val,min,max);

			pCmd->Push(new CNumSetCmd(this,idx,val));
		}
	}

	void NumMapEditCtrlImp::SetEditCtrlData(void* ctrldata)
	{
		CCtrlPropBase::SetEditCtrlData(ctrldata);
		_UpdateMaxMin();
		CCtrlBase::Update();
	}

	void NumMapEditCtrlImp::_UpdateMaxMin()
	{
		int ct = GetNumMapCount();
		float tempMax = NUM_MAP_MIN, tempMin = NUM_MAP_MAX;
		for (int i = 0; i < ct; ++i)
		{
			float val = GetNumMapVal(i);
			tempMax > val ? tempMax = tempMax : tempMax = val;
			tempMin < val ? tempMin = val : tempMin = tempMin;
		}
		if (tempMax < NUM_MAP_NORMAL_HEIGHT)
			m_fMaxVal = NUM_MAP_NORMAL_HEIGHT;
		else
			m_fMaxVal = tempMax + NUM_MAP_NORMAL_HEIGHT;

		if (tempMin > -NUM_MAP_NORMAL_HEIGHT)
			m_fMinVal = -NUM_MAP_NORMAL_HEIGHT;
		else
			m_fMinVal = m_fMinVal - NUM_MAP_NORMAL_HEIGHT;

		ECtrlDataType tp = GetDataType();
		if (tp == E_BYTEMap || tp == E_EBYTEMap)
		{
			m_fMaxVal = 255.0f;
			m_fMinVal = 0.0f;
		}
	}

	int NumMapEditCtrlImp::GetNumMapCount()
	{
		if (m_pCtrlData)
			return ((CFloatMapData*)(m_pCtrlData->m_pData))->GetMapCount();
		return 0;
	}

	float NumMapEditCtrlImp::GetNumMapIdx(int i)
	{
		if (m_pCtrlData)
			return ((CFloatMapData*)(m_pCtrlData->m_pData))->GetMapIndex(i);
		return 0.0f;
	}

	float NumMapEditCtrlImp::GetNumMapVal(int i)
	{
		if (m_pCtrlData)
			return ((CFloatMapData*)(m_pCtrlData->m_pData))->GetMapValue(i);
		return 0.0f;
	}

	float NumMapEditCtrlImp::GetNumMapValPercent(int i)
	{
		float per = GetNumMapVal(i);
		per = abs(m_fMaxVal - per) / abs(m_fMaxVal -m_fMinVal);
		return per;
	}

	float NumMapEditCtrlImp::GetCurPercent()
	{
		return m_fCurPercent;
	}

	float NumMapEditCtrlImp::GetZeroPercent()
	{
		float per = 0.0f;
		per = abs(m_fMaxVal - per) / abs(m_fMaxVal -m_fMinVal);
		return per;
	}

	void NumMapEditCtrlImp::SetCurPercent(float per)
	{
		m_fCurPercent = per;
	}

	void NumMapEditCtrlImp::SetCurNodeVal(float per)
	{
		SetNumMapVal(m_fCurPercent, per);
	}

	float NumMapEditCtrlImp::GetMaxVal()
	{
		return m_fMaxVal;
	}

	float NumMapEditCtrlImp::GetMinVal()
	{
		return m_fMinVal;
	}

	void NumMapEditCtrlImp::SetMaxVal(float max)
	{
		ECtrlDataType tp = GetDataType();
		if (tp == E_BYTEMap || tp == E_EBYTEMap)
			max = 255.0f;
		if(max == GetMaxVal())
			return;
		CCombinCmd* pMaxCmd = new CCombinCmd;
		pMaxCmd->Push(new CNumSetMaxCmd(this,max));
		_ResetMapData(pMaxCmd,m_fMinVal,max);
		pMaxCmd->Execute();
	}

	void NumMapEditCtrlImp::SetMinVal(float min)
	{
		ECtrlDataType tp = GetDataType();
		if (tp == E_BYTEMap || tp == E_EBYTEMap)
			min = 0.0f;
		if(min == GetMinVal())
			return;
		CCombinCmd* pMinCmd = new CCombinCmd;
		pMinCmd->Push(new CNumSetMinCmd(this,min));
		_ResetMapData(pMinCmd,min,m_fMaxVal);
		pMinCmd->Execute();
	}

	void NumMapEditCtrlImp::SetNumMapVal(float idx, float per)
	{
		float val = m_fMaxVal - per * abs(m_fMaxVal -m_fMinVal);
		(new CNumSetCmd(this,idx,val))->Execute();
	}	
	
	void NumMapEditCtrlImp::DelCurPerNode()
	{
		(new CNumDelCmd(this,m_fCurPercent))->Execute();
	}
}
