#pragma once
#include "AutoPropData.h"
#include "CVector3.h"

namespace sqr_tools
{
	class AutoPropToCtrlImp;
	class AutoDataHelper 
	{
	public:
		AutoDataHelper();
		~AutoDataHelper();

	public:
		vector<EAutoCtrlData*>*	GetKeyUintDatas();
		void					_RegAPToCtrlImp(AutoPropToCtrlImp* ptr);
		void					_UnRegAPToCtrlImp(AutoPropToCtrlImp* ptr);
		void					UIShowUpdate();
		

	protected:
		void*					_RegCtrlData(ECtrlDataType tp, PFN_UPDATECALLBACK fun, AutoDataHelper* obj, const char* title
												, const char* helpinfo, void* data);
		void*					_ResetCtrlData(ECtrlDataType tp, void* fun, AutoDataHelper* obj, const char* title
												, const char* helpinfo, void* data);
		virtual void			_UIDataUpdate(){};
	protected:
		vector<EAutoCtrlData*>		m_pKeyUintData;
		set<AutoPropToCtrlImp*>		m_pAPToCtrlSet;
	};

	extern void AutoUpdate(AutoDataHelper* pData);
}