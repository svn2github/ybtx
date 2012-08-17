#pragma once
#include "ThreadHelper.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace sqr
{
	inline int32 Atomic_CompareAndSwap(int32 volatile* pDest, int32 CompareValue, int32 NewValue)
	{
#ifdef _WIN32
		return ::InterlockedCompareExchange( reinterpret_cast<LONG volatile*>(pDest),NewValue,CompareValue );
#else
		return __sync_val_compare_and_swap(pDest, CompareValue, NewValue);
#endif
	}

#if defined(_WIN64) || (!defined(_WIN32))
	inline int64 Atomic_CompareAndSwap(int64 volatile* pDest, int64 CompareValue, int64 NewValue)
	{
#ifdef _WIN32
		return ::InterlockedCompareExchange64( reinterpret_cast<LONGLONG volatile*>(pDest),NewValue,CompareValue );
#else
		return __sync_val_compare_and_swap(pDest, CompareValue, NewValue);
#endif
	}
#endif

	inline void* Atomic_CompareAndSwapPtr(void*volatile* pDest, void* CompareValue, void* NewValue)
	{
#ifdef _WIN32
		return ::InterlockedCompareExchangePointer( pDest, NewValue, CompareValue );
#else
		return __sync_val_compare_and_swap(pDest, CompareValue, NewValue);
#endif
	}








	inline int32 Atomic_Swap(int32 volatile* pDest, int32 NewValue)
	{
#ifdef _WIN32
		return InterlockedExchange( reinterpret_cast<LONG volatile*>(pDest),NewValue );		
#else
		return __sync_lock_test_and_set( pDest, NewValue );
#endif
	}

#if defined(_WIN64) || (!defined(_WIN32))
	inline int64 Atomic_Swap(int64 volatile* pDest, int64 NewValue)
	{
#ifdef _WIN32
		return InterlockedExchange64( reinterpret_cast<LONGLONG volatile*>(pDest),NewValue );		
#else
		return __sync_lock_test_and_set( pDest, NewValue );
#endif
	}
#endif

	inline void* Atomic_SwapPtr(void*volatile* pDest, void* NewValue)
	{
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
		return reinterpret_cast<void*>
			( InterlockedExchangePointer( reinterpret_cast<PVOID volatile*>(pDest), reinterpret_cast<PVOID>(NewValue) ) );
#pragma warning(pop)
#else
		return __sync_lock_test_and_set( pDest, NewValue );
#endif
	}









	inline int32 Atomic_FetchAndAdd(volatile int32 * pDest, int32 AddValue)
	{
#ifdef _WIN32
		return ::InterlockedExchangeAdd( reinterpret_cast<LONG volatile*>(pDest), AddValue );
#else
		return __sync_fetch_and_add( pDest, AddValue );
#endif
	}

#if defined(_WIN64) || (!defined(_WIN32))
	inline int64 Atomic_FetchAndAdd(volatile int64 * pDest, int64 AddValue)
	{
#ifdef _WIN32
		return InterlockedExchangeAdd64( reinterpret_cast<LONGLONG volatile*>(pDest), AddValue );
#else
		return __sync_fetch_and_add( pDest, AddValue );
#endif
	}
#endif
}

