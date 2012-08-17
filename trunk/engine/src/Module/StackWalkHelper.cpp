#include "stdafx.h"
#ifdef _WIN32

#include "StackWalkHelper.h"
#include "CLock.inl"



static CLock gs_StackWalk64;


BOOL SafeStackWalk64(
					 DWORD                             MachineType,
					 HANDLE                            hProcess,
					 HANDLE                            hThread,
					 LPSTACKFRAME64                    StackFrame,
					 PVOID                             ContextRecord,
					 PREAD_PROCESS_MEMORY_ROUTINE64    ReadMemoryRoutine,
					 PFUNCTION_TABLE_ACCESS_ROUTINE64  FunctionTableAccessRoutine,
					 PGET_MODULE_BASE_ROUTINE64        GetModuleBaseRoutine,
					 PTRANSLATE_ADDRESS_ROUTINE64      TranslateAddress
					 )
{
	gs_StackWalk64.Lock();
	BOOL bResult = StackWalk64( MachineType,hProcess,hThread,StackFrame,ContextRecord,
		ReadMemoryRoutine,FunctionTableAccessRoutine,
		GetModuleBaseRoutine,TranslateAddress);
	gs_StackWalk64.Unlock();

	return bResult;
}




#endif


