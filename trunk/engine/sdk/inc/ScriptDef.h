#pragma once

#include "BaseTypes.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "CBaseObject.h"

//#if defined(_WIN32)
//
//#ifdef SCRIPT_EXPORTS
//#define SCRIPT_API __declspec(dllexport)
//#else
//#define SCRIPT_API __declspec(dllimport)
//#endif
//
//#else
#define SCRIPT_API
//#endif

#include <list>
#include <vector>

namespace sqr
{
#define MAX_BASE_CLASS_NUM 5

	class CScriptData;
	class CCallBackProfiler;
	class CClass;
	class CBaseObject;

	typedef uint64	CFuncPointer;
	typedef void*	HPACKAGE;
	typedef void*	(*REG_CREATE_FUN)();
	typedef void	(*REG_DELETE_FUN)( void* );
	
	typedef bool	(*PACK_FUNCTION)( uint32, void*, void*, void*, size_t );
	typedef size_t	(*UNPACK_FUNCTION)( uint32, HPACKAGE, void*, void*, size_t);

	typedef void	(*POST_PACKFUNCTION)(uint32 uPostNum, const char*, size_t , const char*, size_t);
	typedef void	(*ON_UNPACKFUNCTION)(void*, const char*, size_t, const char*, size_t);

	template<class T>			void* RegCreate()
	{
		return (CBaseObject*)(new T());
	}

	template<class T>			void* RegCreateIsolated()
	{
		return new T();
	}

	template<class T>			void  RegDelete( void * pObject )
	{
		CBaseObject* pBaseObject = (CBaseObject*)(pObject);
		delete pBaseObject;
	}
	
	template<class T>			void  RegDeleteIsolated( void * pObject )
	{
		delete (T*)(pObject);
	}

}
