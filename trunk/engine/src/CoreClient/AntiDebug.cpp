#include "stdafx.h"
#include "AntiDebug.h"
#include "CAppConfigClient.h"
#include "ExpHelper.h"
#include <shlobj.h>
#include <Psapi.h>
#include "StringHelper.h"

typedef struct _PROCESS_DEBUG_PORT_INFO 
{ 
	HANDLE DebugPort; 
}PROCESS_DEBUG_PORT_INFO;

typedef struct _PROCESS_BASIC_INFO
{
	DWORD ExitStatus;
	DWORD PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFO;

typedef long              NTSTATUS; 

enum PROCESS_INFO_CLASS
{
	ProcessBasicInfo = 0,
	ProcessDebugPort = 7
};

typedef NTSTATUS  (NTAPI *ZW_QUERY_INFORMATION_PROCESS)(IN HANDLE ProcessHandle, IN PROCESS_INFO_CLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength);
#define STATUS_SUCCESS    ((NTSTATUS)0L) 

//------------------------------------------------------------------------------
namespace sqr
{

	//------------------------------------------------------------------------------
	/**
	替代IsDebuggerPresent().
	*/
	bool CheckDebugger()
	{
		__asm
		{
			mov eax, fs:[30h] ;EAX =  TEB.ProcessEnvironmentBlock
				inc eax
				inc eax
				mov eax, [eax]
			and eax,0x000000ff	;AL  =  PEB.BeingDebugged
				test eax, eax
				jne rt_label
				jmp rf_label
		}
rt_label:
		return true;
rf_label:
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	使用ntdll_NtQueryInformationProcess()来查询ProcessDebugPort可以用来检测反调试。
	如果进程被调试，其返回值应为0xffffffff。
	*/
	bool
		CheckDebugPort()
	{
		HMODULE hModule = GetModuleHandle("ntdll.dll"); 
		ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess; 
		ZwQueryInformationProcess = (ZW_QUERY_INFORMATION_PROCESS)GetProcAddress(hModule, "ZwQueryInformationProcess"); 
		if (ZwQueryInformationProcess == NULL) 
			return false;
		PROCESS_DEBUG_PORT_INFO ProcessInfo; 
		if (STATUS_SUCCESS != ZwQueryInformationProcess(GetCurrentProcess(),
			ProcessDebugPort, &ProcessInfo, sizeof(ProcessInfo), NULL)) 
			return false;
		if(ProcessInfo.DebugPort)
			return true;
		else
			return false;
	}

	//------------------------------------------------------------------------------
	/**
	如果给CloseHandle()函数一个无效句柄作为输入参数，
	在无调试器时，将会返回一个错误代码，而有调试器存在时，
	将会触发一个EXCEPTION_INVALID_HANDLE (0xc0000008)的异常
	*/
	bool CheckClosehandle()
	{
		__try
		{
			CloseHandle(HANDLE(0x00001234));
			return false;
		}
		__except(1)
		{
			return true;
		}
	}

	//---------------------------------------------------------------------------
	bool
		GetParentProcessID(DWORD* outID)
	{
		HMODULE hModule = GetModuleHandle("ntdll.dll"); 
		ZW_QUERY_INFORMATION_PROCESS ZwQueryInformationProcess; 
		ZwQueryInformationProcess = (ZW_QUERY_INFORMATION_PROCESS)GetProcAddress(hModule, "ZwQueryInformationProcess"); 
		if (ZwQueryInformationProcess == NULL) 
			return false;
		PROCESS_BASIC_INFO BasicInfo;
		if (STATUS_SUCCESS != ZwQueryInformationProcess(GetCurrentProcess(),
			ProcessBasicInfo, &BasicInfo, sizeof(BasicInfo), NULL))
			return false;
		*outID = BasicInfo.InheritedFromUniqueProcessId;
		return true;
	}

	//---------------------------------------------------------------------------
	/**
	检测父进程异常.
	@todo 字符串加密
	*/
	bool ParentProcessIsGood(string& sParentProcessName)
	{
		DWORD dwParentID = 0;
		if ( !GetParentProcessID(&dwParentID) )
			return true;

		HANDLE hParent = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwParentID);
		if( !hParent )
			return true;

		char szProcessName[MAX_PATH];

		bool bResult=true;

		DWORD length = GetModuleBaseName(hParent, NULL, szProcessName, MAX_PATH);
		if ( 0 == length )
			goto out;

		sParentProcessName = szProcessName;
		sParentProcessName = tolower(sParentProcessName);

		if ( sParentProcessName.find("pixwin") != string::npos ||
			 sParentProcessName.find("nvperfhud") != string::npos ||
			 sParentProcessName.find("perfclient") != string::npos )
		{
			bResult = false;
		}

out:
		CloseHandle(hParent);
		return bResult;
	}

	//------------------------------------------------------------------------------
	void ConfirmNoDebuger()
	{
		if ( !CAppConfigClient::Inst()->IsAntiDebug() )
			return;

#ifdef _WIN32
		if( CheckDebugger() )
			GenErr("汇编发现调试器");

		if( CheckClosehandle() )
			GenErr("CloseHandle发现调试器");

		if( CheckDebugPort() )
			GenErr("QueryInformationProcess发现调试器");

		string sParentProcessName;
		if( !ParentProcessIsGood( sParentProcessName ) )
		{
			if( sParentProcessName.empty() )
				GenErr("不存在父进程");

			GenErr("发现不允许的父进程",sParentProcessName);
		}
#endif // _WIN32
	}

}// namespace sqr
