#include "stdafx.h"
#include "BaseHelper.h"
#include "CBaseObject.h"
#include "ExpHelper.h"
#include <csignal>
#ifndef _WIN32
#include <dlfcn.h>
#endif


namespace sqr
{
	const char* GetSystemName()
	{
#ifdef WIN32
		return "Windows";
#else
		return "Other";
#endif
	}

	void NullScriptFunc(CBaseObject*){}

	static AddRefFuncType gs_AddRefFunc=&NullScriptFunc;
	static DelRefFuncType gs_DelRefFunc=&NullScriptFunc;
	static DisbindFuncType gs_DisbindFunc=&NullScriptFunc;

	void SetAddRefFunction(AddRefFuncType Fun)
	{
		gs_AddRefFunc = Fun ? Fun : NullScriptFunc;
	}

	AddRefFuncType GetAddRefFunction()
	{
		return gs_AddRefFunc;
	}

	void SetDelRefFunction(DelRefFuncType Fun)
	{
		gs_DelRefFunc = Fun ? Fun : NullScriptFunc;
	}

	DelRefFuncType GetDelRefFunction()
	{
		return gs_DelRefFunc;
	}

	void SetDisbindFunction(DisbindFuncType Fun)
	{
		gs_DisbindFunc = Fun ? Fun : NullScriptFunc;
	}

	DisbindFuncType GetDisbindFunction()
	{
		return gs_DisbindFunc;
	}

	//---------------------------------------------------
	void NullScriptPvFunc(void*){}

	static AddPvRefFuncType gs_AddPvRefFunc=&NullScriptPvFunc;
	static DelPvRefFuncType gs_DelPvRefFunc=&NullScriptPvFunc;
	static DisbindPvFuncType gs_DisbindPvFunc=&NullScriptPvFunc;

	void SetAddPvRefFunction(AddPvRefFuncType Fun)
	{
		gs_AddPvRefFunc = Fun ? Fun : NullScriptPvFunc;
	}

	AddPvRefFuncType GetAddPvRefFunction()
	{
		return gs_AddPvRefFunc;
	}

	void SetDelPvRefFunction(DelPvRefFuncType Fun)
	{
		gs_DelPvRefFunc = Fun ? Fun : NullScriptPvFunc;
	}

	DelPvRefFuncType GetDelPvRefFunction()
	{
		return gs_DelPvRefFunc;
	}

	void SetDisbindPvFunction(DisbindPvFuncType Fun)
	{
		gs_DisbindPvFunc = Fun ? Fun : NullScriptPvFunc;
	}

	DisbindPvFuncType GetDisbindPvFunction()
	{
		return gs_DisbindPvFunc;
	}

	//---------------------------------------------------

	void IntAdvAddRef(void* pObj)
	{
		GetAddPvRefFunction()(pObj);
	}

	void IntAdvAddRef(CBaseObject* pObj)
	{
		if( !pObj || !pObj->CreatedByScript() )
			return;
		GetAddRefFunction()(pObj);
	}

	void IntAdvDelRef(void* pObj)
	{
		GetDelPvRefFunction()(pObj);
	}

	void IntAdvDelRef(CBaseObject* pObj)
	{
		if( !pObj || !pObj->CreatedByScript() )
			return;
		GetDelRefFunction()(pObj);
	}

	void IntAdvDisbind(void* pObj)
	{
		GetDisbindPvFunction()(pObj);
	}

	void IntAdvDisbind(CBaseObject* pObj)
	{
		if(!pObj || !pObj->VisibleToScript())
			return;
		GetDisbindFunction()(pObj);
	}

	void AdvAddPvRef(void* pObject)
	{
		IntAdvAddRef(pObject);
	}

	void AdvDelPvRef(void* pObject)
	{
		IntAdvDelRef(pObject);
	}

	void AdvPvDisbind(void* pObject)
	{
		IntAdvDisbind(pObject);
	}

	//---------------------------------------------------

#ifndef _WIN32
	HMODULE LoadLibrary(const char* szModuleFileName)
	{
		return dlopen(szModuleFileName,RTLD_NOW);
	}

	bool FreeLibrary(HMODULE hModule)
	{
		return dlclose(hModule);
	}

	void* GetProcAddress(HMODULE hModule,const char* szRoutineName)
	{
		return dlsym(hModule,szRoutineName);
	}
#endif




#ifndef _WIN32
	void TermSigHandler(int32 signum, siginfo_t* info, void* context)
	{
		_exit(0);
	}
#else
	BOOL WINAPI TermSigHandler(DWORD dwCtrlType)
	{
		_exit(0);
		return TRUE;
	}
#endif

	void SetUpTermSigHandler()
	{
#ifndef _WIN32
		struct sigaction sa;
		sa.sa_sigaction=&TermSigHandler;
		if( -1==sigemptyset(&sa.sa_mask) )
			GenErrnoErr("sigemptyset failed.");

		if( -1 == sigaction(SIGTERM,&sa,NULL))
			GenErrnoErr("sigaction SIGTERM failed.");
		if( -1 == sigaction(SIGINT,&sa,NULL))
			GenErrnoErr("sigaction SIGINT failed.");
		if( -1 == sigaction(SIGHUP,&sa,NULL))
			GenErrnoErr("sigaction SIGHUP failed.");
#else
		if(!SetConsoleCtrlHandler(TermSigHandler,TRUE))
			GenErr("system call SetConsoleCtrlHandler failed.");
#endif
	}

}
