#pragma once
#include "CEventCollector.h"
#include "CTempCmdPool.h"
#include "ICtrlImpFactory.h"
#include <map>

using namespace sqr;


#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif

#define GET_IMP_NAME(class_name) #class_name

#define REG_UI_IMP(class_name) \
	public: \
	virtual const string&		GetTypeName(void); \
	static CCtrlBase*			CreateNewCtrl(void* param); \
	virtual	void				Destroy(void); \
	static const string			st_TypeName; \

#define DEF_BASE_IMP(class_name) \
	const string	class_name::st_TypeName = GET_IMP_NAME(class_name); \
	const string&	class_name::GetTypeName(void) { return class_name::st_TypeName; }

#define DEF_UI_IMP(class_name) \
	DEF_BASE_IMP(class_name); \
	CCtrlBase*		class_name::CreateNewCtrl(void* param){ return new class_name(param); } \
	void			class_name::Destroy(void) { delete this; } \


#define DEF_SINGLE_UI_IMP(class_name) \
	DEF_BASE_IMP(class_name); \
	CCtrlBase*	class_name::CreateNewCtrl(void* param){ return class_name::GetInst(); } \
	void		class_name::Destroy(void) { } \

namespace sqr_tools
{
	//Runtime Imp
	class CCtrlBase;
	class CEditContext;
	//This is the base class of all the controls in C++ lib;
	typedef void UICallBack(void* param);
	typedef CCtrlBase* (*CreateNewCtrl)(void*);

	class CCtrlBase 
		: public CEventCollector
		, public CTempCmdPool
	{
		REG_UI_IMP(CCtrlBase);
	public:
		CCtrlBase(void);
		virtual ~CCtrlBase(void);

	public:
		virtual bool _ProcMsg(const CMsg& msg );

		//UI MSG
		virtual bool OnCtrlSize(int width,int height);
		virtual bool OnCtrlCreate();
		virtual bool OnCtrlPaint();
		virtual bool OnCtrlClose();
		virtual bool OnCtrlQuit();
		//KEY MSG
		virtual bool OnCtrlKeyDown(MSG_KEY_INF key);
		virtual bool OnCtrlKeyUp(MSG_KEY_INF key);
		virtual bool OnCtrlKeyPress(MSG_KEY_INF key);
		//MOUSE MSG
		virtual bool OnCtrlMouseMove(int x,int y);
		virtual bool OnCtrlLBtDown(int x,int y);
		virtual bool OnCtrlLBtUp(int x,int y);
		virtual bool OnCtrlLBtDLK(int x,int y);
		virtual bool OnCtrlRBtDown(int x,int y);
		virtual bool OnCtrlRBtUp(int x,int y);
		virtual bool OnCtrlRBtDLK(int x,int y);
		virtual bool OnCtrlMBtDown(int x,int y);
		virtual bool OnCtrlMBtUp(int x,int y);
		virtual bool OnCtrlMBtDLK(int x,int y);
		virtual bool OnCtrlMouseWheel(int delta);
		//Default Msg
		virtual bool	OnCtrlDefaultMsg(const CMsg& msg);
		virtual void	RegisterUICallBack( void* key, UICallBack* pCall );
		virtual void	Update(void);
		virtual void	SetContext(CEditContext* pContext);
	protected:
		typedef std::map<void*,UICallBack*> CallBackPool;
		CallBackPool	m_CallBackPool;
		CEditContext*	m_pOwnContext;
	};

}
