#pragma once
#include "ModuleDefs.h"
#include "CompileTimeHelper.h"

namespace sqr
{


#ifndef _WIN32

#define _stricmp		strcasecmp
#define _snprintf		snprintf
#define OutputDebugString(p)	fprintf(stderr,"%s",p);


	typedef void*	HMODULE;

	HMODULE	LoadLibrary(const char* szModuleFileName);
	bool FreeLibrary(HMODULE hModule);
	void* GetProcAddress(HMODULE hModule,const char* szRoutineName);

#endif

	class CBaseObject;

	//---------------------------------------------------
	typedef void (*AddRefFuncType)(CBaseObject*);
	typedef void (*DelRefFuncType)(CBaseObject*);
	typedef void (*DisbindFuncType)(CBaseObject*);

	MODULE_API void SetAddRefFunction(AddRefFuncType);
	MODULE_API AddRefFuncType GetAddRefFunction();

	MODULE_API void SetDelRefFunction(DelRefFuncType);
	MODULE_API DelRefFuncType GetDelRefFunction();

	MODULE_API void SetDisbindFunction(DisbindFuncType);
	MODULE_API DisbindFuncType GetDisbindFunction();

	//----------------------------------------------------
	typedef void (*AddPvRefFuncType)(void*);
	typedef void (*DelPvRefFuncType)(void*);
	typedef void (*DisbindPvFuncType)(void*);

	MODULE_API void SetAddPvRefFunction(AddPvRefFuncType);
	MODULE_API AddPvRefFuncType GetAddPvRefFunction();

	MODULE_API void SetDelPvRefFunction(DelPvRefFuncType);
	MODULE_API DelPvRefFuncType GetDelPvRefFunction();

	MODULE_API void SetDisbindPvFunction(DisbindPvFuncType);
	MODULE_API DisbindPvFuncType GetDisbindPvFunction();

	//----------------------------------------------------
	MODULE_API void IntAdvAddRef(void*);
	MODULE_API void IntAdvAddRef(CBaseObject*);

	MODULE_API void IntAdvDelRef(void*);
	MODULE_API void IntAdvDelRef(CBaseObject*);

	MODULE_API void IntAdvDisbind(void*);
	MODULE_API void IntAdvDisbind(CBaseObject*);

	template<typename T>
	void AdvAddRef(T* pObject)
	{
		ASSERT_CLASS_DEFINED(T);

		IntAdvAddRef(pObject);
	}

	
	template<typename T>
	void AdvDelRef(T* pObject)
	{
		ASSERT_CLASS_DEFINED(T);

		IntAdvDelRef(pObject);
	}

	template<typename T>
	void AdvDisbind(T* pObject)
	{
		ASSERT_CLASS_DEFINED(T);

		IntAdvDisbind(pObject);
	}

	MODULE_API void AdvAddPvRef(void* pObject);
	MODULE_API void AdvDelPvRef(void* pObject);
	MODULE_API void AdvPvDisbind(void* pObject);

	//----------------------------------------------------

	MODULE_API const char* GetSystemName();

	//安装杀进程的信号处理函数，不调用任何的全局对象的析构函数，不触发任何退出清理代码
	MODULE_API void SetUpTermSigHandler();

	// 由4个字符组成一个DWORD
	inline uint32 MakeDWORD(char ch0,char ch1,char ch2,char ch3)
	{
		return ((uint32)(uint8)(ch0) |
			((uint32)(uint8)(ch1) << 8) |
			((uint32)(uint8)(ch2) << 16) |
			((uint32)(uint8)(ch3) << 24 ));
	}

	template<typename Type>
	inline void SafeRelease(Type& p)
	{
		if (NULL!=p)
		{
			Type t = p;
			p=NULL;
			t->Release();
		}
	}

	template<typename Type>
	inline void SafeDelete(Type& p)
	{
		delete p;
		p=NULL;
	}

	template<typename Type>
	inline void SafeDelAry(Type& p)
	{
		delete[] p;
		p=NULL;
	}

	template <typename Type>
	inline void SafeDelCtn1(Type& Container)
	{
		for (typename Type::iterator it = Container.begin(); it != Container.end(); ++it)
		{
			SafeDelete(*it);
		}
		Container.clear();
	}

	template <typename Type>
	inline void SafeRelCtn1(Type& Container)
	{
		for (typename Type::iterator it = Container.begin(); it != Container.end(); ++it)
		{
			SafeRelease(*it);
		}
		Container.clear();
	}

	template <typename Type>
	inline void SafeDelCtn2(Type& Container)
	{
		for (typename Type::iterator it = Container.begin(); it != Container.end(); ++it)
		{
			SafeDelete(it->second);
		}
		Container.clear();
	}

	template<typename VEC_T> 
	void ClearVector(VEC_T& vector_t)
	{
		if(vector_t.empty())
			return;

		typename VEC_T::iterator it = vector_t.begin();

		for(; it != vector_t.end(); ++it)
		{
			SafeDelete(*it);
		}

		vector_t.clear();
	}

	template<typename VEC_T> 
	void SafeClearVector(VEC_T& vector_t)
	{
		while(!vector_t.empty())
		{
			typename VEC_T::iterator it = vector_t.begin();
			SafeDelete(*it);
			vector_t.erase(it);
		}
	}

	template<typename MAP_T> 
	void ClearMap(MAP_T& map_t)
	{
		if(map_t.empty())
			return;

		typename MAP_T::iterator it = map_t.begin();

		for(; it != map_t.end(); ++it)
		{
			SafeDelete(it->second);
		}

		map_t.clear();

	}

	template<typename MAP_T, typename MAPITR_T> 
	void EraseMapNode(MAP_T& map_t, MAPITR_T& it_t)
	{
		SafeDelete(it_t->second);
		map_t.erase(it_t);
	}


	template <class T>
	class Delete
	{
	public:
		void operator()(T t) const
		{
			delete t;
		}
	};
}

