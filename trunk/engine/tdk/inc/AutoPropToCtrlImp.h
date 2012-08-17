#pragma once
#include "CCtrlBase.h"
#include "AutoPropData.h"

class EDITEFFECTKEY;
namespace sqr_tools
{
	class CEditObserver;
	class AutoPropToCtrlImp 
		: public CCtrlBase
	{
		REG_UI_IMP(AutoPropToCtrlImp);
	public:
		AutoPropToCtrlImp(void* param);
		~AutoPropToCtrlImp(void);

	public:
		virtual void			SetEditPropContent(void* content);
		virtual int				GetEditPropUintCount();
		virtual ECtrlDataType	GetEditPropUintType(int idx);
		virtual void*			GetEditPropUintData(int idx);
		virtual const char*		GetEditPropTitle(int idx);

	private:
		virtual vector<EAutoCtrlData*>*	_GetPropData();
	private:
		void*					m_pEditData;
		void*					m_pADHelper;
	};
}