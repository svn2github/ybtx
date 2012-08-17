#pragma once
#ifdef _WIN32



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
					 );

#endif
