#pragma once
#include "ICtrlImpFactory.h"
#include "TSingleton.h"
#include "CCtrlBase.h"
namespace sqr_tools
{
	class CCtrlImpFactory 
		: public ICtrlImpFactory
		, public Singleton<CCtrlImpFactory>
	{
	public:
		CCtrlImpFactory();
		virtual CCtrlBase*  CreateImp( char* class_name , void* pParam );
	protected:
		
		typedef map<string, CreateNewCtrl> CtrlIDMap;
		CtrlIDMap	st_IDMap; 
	};
}
