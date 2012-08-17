#pragma once

#include "CCtrlBase.h"
#include "AutoPropData.h"
#include "CTemporaryCmd.h"

namespace sqr_tools
{
	class CCtrlPropBase 
		: public CCtrlBase
	{
	public:
		CCtrlPropBase();
		~CCtrlPropBase();

	public:
		virtual void			OnDataReFresh();	// 通知界面
		virtual void			RegCtrlPropData(ECtrlDataType tp, void* propdata);
		virtual void			SetEditCtrlData(void* ctrldata);
		virtual void*			GetEditData();
		virtual ECtrlDataType	GetDataType();
		
		virtual const char*		GetDataHelpInfo();
		
	protected:
		virtual void			OnDataChanged();	// 通知C++实体	
	protected:
		ECtrlData*				m_pCtrlData;
	};

	template<class T>
	class CPropBaseCmd :public CTemporaryCmd
	{
	public:
		CPropBaseCmd(T* pCtrl):CTemporaryCmd(pCtrl){}
	protected:
		virtual T* GetCtrl(void) 
		{ 
			return (T*)m_pPool; 
		}
	};

}