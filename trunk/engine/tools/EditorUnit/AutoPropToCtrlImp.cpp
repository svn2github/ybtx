#include "stdafx.h"
#include "AutoPropToCtrlImp.h"
#include "AutoDataHelper.h"

DEF_UI_IMP(AutoPropToCtrlImp);
AutoPropToCtrlImp::AutoPropToCtrlImp(void* param)
: m_pADHelper(NULL)
{

}

AutoPropToCtrlImp::~AutoPropToCtrlImp(void)
{
	if (m_pADHelper)
	{
		AutoDataHelper* pADHelper =  static_cast<AutoDataHelper*>(m_pADHelper);
		pADHelper->_UnRegAPToCtrlImp(this);
	}
}

void AutoPropToCtrlImp::SetEditPropContent(void* content)
{
	// 清空之前的
	if (m_pADHelper)
	{
		AutoDataHelper* pADHelper =  static_cast<AutoDataHelper*>(m_pADHelper);
		pADHelper->_UnRegAPToCtrlImp(this);
	}
		
	if (content)
	{
		AutoDataHelper* ADHelper =  static_cast<AutoDataHelper*>(content);
		m_pEditData = ADHelper->GetKeyUintDatas();
		// 需要注册一些东西过去,用于声明周期的管理
		ADHelper->_RegAPToCtrlImp(this);
	}
	else
		m_pEditData = NULL;
	m_pADHelper = content;
	Update();
}

vector<EAutoCtrlData*>* AutoPropToCtrlImp::_GetPropData()
{
	if (m_pEditData)
		return static_cast<vector<EAutoCtrlData*>*>(m_pEditData);
	return NULL;
}

int AutoPropToCtrlImp::GetEditPropUintCount()
{
	vector<EAutoCtrlData*>* propData = _GetPropData();
	if (propData)
		return (int)(propData->size());
	return 0;
}

ECtrlDataType AutoPropToCtrlImp::GetEditPropUintType(int idx)
{
	vector<EAutoCtrlData*>* propData = _GetPropData();
	if (propData)
	{
		if (((*propData)[idx])->m_pCtrlData)
			return 	((*propData)[idx])->m_pCtrlData->m_eDataType;
	}
	return E_TYPECOUNT;
}

void* AutoPropToCtrlImp::GetEditPropUintData(int idx)
{
	vector<EAutoCtrlData*>* propData = _GetPropData();
	if (propData)
		return (*propData)[idx]->m_pCtrlData;
	return NULL;
}

const char*	AutoPropToCtrlImp::GetEditPropTitle(int idx)
{
	vector<EAutoCtrlData*>* propData = _GetPropData();
	if (propData)
	{
		return (*propData)[idx]->m_Title.c_str();
	}
	return NULL;
}


