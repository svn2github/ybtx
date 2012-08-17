#pragma once
#include <cstring>
#include "CDynamicObject.h"
#include "CompileTimeHelper.h"
#include <sstream>
#include "ExpHelper.h"

namespace sqr
{
	/*
		注意，这个头文件主要用于c++与lua之间数据传递的
		对于一般的基本类型，我们可以很好的处理，但是对于类对象
		在编译相应的函数之前，这个类必须得定义，不允许只前置声明
		因为如果只有前置声明，没有定义，对于一个safe类，编译器很可能
		不会去调用对应的CBaseObject重载函数
		为了确定我们类定义了，我们使用sizeof来进行类型判断
	*/

	/*
		如果class没有被定义，那么sizeof得到的值为0
		于是会构造一个CompileTimeError<false>()的对象，但是这个对象不存在
		编译其会报错
		为什么不直接使用char ay[sizeof(class)]这样的代码
		因为在gcc下面，ay是一个没有使用的变量，会出warning，而现在warning会当成error处理
	*/
	 

	template<typename T>
	struct TypeWrapper{};

	//----------------------------------------------------------------------------

	template<typename T>
	inline T* _ScriptGetValue(void* pData, void*)
	{
		return (T*)pData;
	}

	template<typename T>
	inline T* _ScriptGetValue(void* pData, CBaseObject*)
	{
		T* pObject = dynamic_cast<T*>((CBaseObject*)pData);
		if(pData && !pObject)
		{	
			ostringstream str;
			str << "需求类型为 " << typeid(T).name(); 
			GenErr("Script 传入的对象参数类型不匹配，对象可能已经损坏", str.str().c_str());
		}
		return pObject;
	}

	template<typename T>
	inline T* _ScriptGetValue(void* pData, const void*)
	{
		return (T*)pData;
	}

	template<typename T>
	inline T* _ScriptGetValue(void* pData, const CBaseObject*)
	{
		T* pObject =  dynamic_cast<T*>((CBaseObject*)pData);
		if(pData && !pObject)
		{
			ostringstream str;
			str << "需求类型为 " << typeid(T).name(); 
			GenErr("Script 传入的对象参数类型不匹配，对象可能已经损坏", str.str().c_str());
		}
		return pObject;
	}

	//----------------------------------------------------------------------------

	template<typename T>
	inline T ScriptGetValue(TypeWrapper<T>, const char* pDataBuf, size_t stOffset)
	{
		return *(T*)((void*)(pDataBuf + stOffset));
	}

	inline void ScriptGetValue(TypeWrapper<void>, const char* pDataBuf, size_t stOffset)
	{}

	inline void* ScriptGetValue(TypeWrapper<void*>, const char* pDataBuf, size_t stOffset)
	{
		return *(void**)((void*)(pDataBuf + stOffset));
	}

	inline const void* ScriptGetValue(TypeWrapper<const void*>, const char* pDataBuf, size_t stOffset)
	{
		return *(void**)((void*)(pDataBuf + stOffset));
	}

	template<typename T>
	inline T* ScriptGetValue(TypeWrapper<T*>, const char* pDataBuf, size_t stOffset)
	{
		ASSERT_CLASS_DEFINED(T);

		T* pType = NULL;
		return (_ScriptGetValue<T>(*(void**)(pDataBuf + stOffset), pType));
	}

	template<typename T>
	inline const T* ScriptGetValue(TypeWrapper<const T*>, const char* pDataBuf, size_t stOffset)
	{
		ASSERT_CLASS_DEFINED(T);

		const T* pType = NULL;
		return (const T*)(_ScriptGetValue<T>(*(void**)(pDataBuf + stOffset), pType));
	}

	template<typename T>
	inline T& ScriptGetValue(TypeWrapper<T&>, const char* pDataBuf, size_t stOffset)
	{
		ASSERT_CLASS_DEFINED(T);

		T* pType = NULL;
		return *(_ScriptGetValue<T>(*(void**)(pDataBuf + stOffset), pType));
	}

	template<typename T>
	inline const T& ScriptGetValue(TypeWrapper<const T&>, const char* pDataBuf, size_t stOffset)
	{
		ASSERT_CLASS_DEFINED(T);

		const T* pType = NULL;
		return *(_ScriptGetValue<T>(*(void**)(pDataBuf + stOffset), pType));
	}

	
	//-------------------------------------------------------------------------------
		
	inline void _ScriptSetValue(CBaseObject* pData, void* pBuf)
	{
		memcpy(pBuf, &pData, sizeof(void*));
	}

	inline void _ScriptSetValue(void* pData, void* pBuf)
	{
		memcpy(pBuf, &pData, sizeof(void*));
	}

	inline void _ScriptSetValue(const CBaseObject* pData, void* pBuf)
	{
		memcpy(pBuf, &pData, sizeof(void*));
	}

	inline void _ScriptSetValue(const void* pData, void* pBuf)
	{
		memcpy(pBuf, &pData, sizeof(void*));
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	inline void ScriptSetValue(TypeWrapper<T>, T& Value, char* pDataBuf, size_t stOffset)
	{
		memcpy(pDataBuf + stOffset, (void*)&Value, sizeof(T));
	}

	template<typename T>
	inline void ScriptSetValue(TypeWrapper<T*>, T* Value, char* pDataBuf, size_t stOffset)
	{
		ASSERT_CLASS_DEFINED(T);

		_ScriptSetValue(Value, pDataBuf + stOffset);
	}

	inline void ScriptSetValue(TypeWrapper<void*>, void* Value, char* pDataBuf, size_t stOffset)
	{
		_ScriptSetValue(Value, pDataBuf + stOffset);
	}

	template<typename T>
	inline void ScriptSetValue(TypeWrapper<T&>, T& Value, char* pDataBuf, size_t stOffset)
	{
		ASSERT_CLASS_DEFINED(T);

		_ScriptSetValue(&Value, pDataBuf + stOffset);
	}

	template<typename T>
	inline void ScriptSetValue(TypeWrapper<const T*>, const T* Value, char* pDataBuf, size_t stOffset)
	{
		ASSERT_CLASS_DEFINED(T);

		_ScriptSetValue(Value, pDataBuf + stOffset);
	}

	template<typename T>
	inline void ScriptSetValue(TypeWrapper<const T&>, const T& Value, char* pDataBuf, size_t stOffset)
	{
		ASSERT_CLASS_DEFINED(T);

		_ScriptSetValue(&Value, pDataBuf + stOffset);
	}


	//-------------------------------------------------------------------------
	//for call back function
	template<typename RT>
	inline RT RetValue(TypeWrapper<RT>)
	{	return RT(); }

	template<typename RT>
	inline RT* RetValue(TypeWrapper<RT*>)
	{	return NULL; }

	template<typename RT>
	inline const RT* RetValue(TypeWrapper<const RT*>)
	{	return NULL; }
	
	template<typename RT>
	inline RT RetValue(TypeWrapper<RT&>)
	{	return RT(); }
	
	template<typename RT>
	inline RT RetValue(TypeWrapper<const RT&>)
	{	return RT(); }

	inline void RetValue(TypeWrapper<void>)
	{}
}
