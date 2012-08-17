#include "stdafx.h"
#include "AutoDataHelper.h" 
#include "AutoPropToCtrlImp.h"

AutoDataHelper::AutoDataHelper()
{
}

AutoDataHelper::~AutoDataHelper()
{
	// 析构的时候，取消已经注册该数据的控件
	set<AutoPropToCtrlImp*>::iterator APitr = m_pAPToCtrlSet.begin();
	while(APitr != m_pAPToCtrlSet.end())
	{
		(*APitr++)->SetEditPropContent(NULL);
	}
	m_pAPToCtrlSet.clear();

	vector<EAutoCtrlData*>::iterator itr = m_pKeyUintData.begin();
	while(itr != m_pKeyUintData.end())
	{
		delete *itr;
		++itr;
	}
	m_pKeyUintData.clear();
}

void	AutoDataHelper::UIShowUpdate()
{
	_UIDataUpdate();
	set<AutoPropToCtrlImp*>::iterator APitr = m_pAPToCtrlSet.begin();
	while(APitr != m_pAPToCtrlSet.end())
	{
		(*APitr)->Update();
		++APitr;
	}
}

vector<EAutoCtrlData*>*	AutoDataHelper::GetKeyUintDatas()
{
	_UIDataUpdate();
	return &m_pKeyUintData;
}

void	AutoDataHelper::_RegAPToCtrlImp(AutoPropToCtrlImp* ptr)
{
	m_pAPToCtrlSet.insert(ptr);
}

void	AutoDataHelper::_UnRegAPToCtrlImp(AutoPropToCtrlImp* ptr)
{
	set<AutoPropToCtrlImp*>::iterator APitr = m_pAPToCtrlSet.find(ptr);
	if (APitr != m_pAPToCtrlSet.end())
		m_pAPToCtrlSet.erase(APitr);
}

void* AutoDataHelper::_RegCtrlData(ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj, 
								  const char* title, const char* helpinfo, void* data)
{
	ECtrlData*	tempData =  ECtrlData::CreateCtrlData(tp, fun, obj,  helpinfo, data);
	EAutoCtrlData* tempCtrlData = new EAutoCtrlData(title, tempData);
	m_pKeyUintData.push_back(tempCtrlData);
	return tempData->m_pAssistInfo;
}

void* AutoDataHelper::_ResetCtrlData(ECtrlDataType tp, void* fun, AutoDataHelper* obj, 
								  const char* title, const char* helpinfo, void* data)
{
	vector<EAutoCtrlData*>::iterator itr = m_pKeyUintData.begin();
	vector<EAutoCtrlData*>::iterator eitr = m_pKeyUintData.end();
	while(itr != eitr)
	{
		ECtrlData*	ctrlData = (*itr)->m_pCtrlData;
		if (ctrlData)
		{
			ECtrlDataType eDataType = ctrlData->m_eDataType;
			if (tp == eDataType)
			{
				// 分析类型
				switch(tp)
				{
				case E_COMBO:
					{
						CComboCtrlData* CombData = (CComboCtrlData*)(ctrlData->m_pData);
						if (CombData->m_pVal == data)
						{
							CombData->ResetHelper();
							ctrlData->m_pAssistInfo = CombData->m_pHelper;
							return ctrlData->m_pAssistInfo;
						}
					}
					break;
				default:
					break;
				}
			}
		}
		++itr;
	}

	return NULL;
}

void sqr_tools::AutoUpdate( AutoDataHelper* pData )
{
	static bool isLock = false;
	if(isLock)
		return;
	isLock = true;
	pData->UIShowUpdate();
	isLock = false;
}