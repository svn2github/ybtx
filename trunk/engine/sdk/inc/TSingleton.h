#pragma once

#include "ExpHelper.h"
#include "BaseDefs.h"

namespace sqr
{
	//------------------------------------------------------------------------------
	/**
	@class TSingleton

	单件基类.
	无需显式创建和销毁, 在第一次调用时创建, 程序退出时static对象自动销毁.
	*/
	template <class T>
	class TSingleton
	{
	protected:
		TSingleton ()
		{
		}

	public:
		/// 取得唯一实例
		static T* GetInst()
		{
			static T Inst;
			return &Inst;
		}

	};


	//------------------------------------------------------------------------------
	/**
	@class Singleton

	单件基类.
	需要手动管理其生命周期的实现.
	适用于多个单件之间有构造/析构顺序依赖的情况.
	*/
	template <class T>
	class Singleton
	{
		static T* ms_Singleton;

	protected:
		/// 不推荐直接构造
		Singleton()
		{
			Ast(NULL == ms_Singleton);
			ms_Singleton = static_cast<T*>(this);
		}

		~Singleton()
		{
			Ast(NULL != ms_Singleton);
			ms_Singleton = NULL;
		}

	public:
		/// 取得唯一实例
		static T* GetInst()
		{
			Ast(NULL != ms_Singleton);
			return ms_Singleton;
		}

		/// 是否已经创建
		static bool HasInst()
		{
			return (NULL != ms_Singleton);
		}

		static void SetNull()
		{
			ms_Singleton = NULL;
		}

		/// 创建
		static T* Create()
		{
			Ast(NULL == ms_Singleton);
			return new T();
		}

		/// 销毁
		static void Destroy()
		{
			Ast(NULL != ms_Singleton);
			delete ms_Singleton;
			SetNull();
		}
	};

	template <class T> T* Singleton<T>::ms_Singleton = NULL;
	//------------------------------------------------------------------------------
}
