#pragma once
#include "ExpHelper.h"

namespace sqr
{

/// 智能指针类.
/// 需要T有AddRef()和Release()方法
template<class T>
class TRefPointer
{
public:
	/// 构造函数
	TRefPointer();
	/// 从C++指针构造
	TRefPointer(T* p);
	/// 从智能指针构造
	TRefPointer(const TRefPointer<T>& p);
	/// 析构函数
	~TRefPointer();
	/// 赋值
	void operator=(const TRefPointer<T>& rhs);
	/// 赋值
	void operator=(T* rhs);
	/// 相等
	bool operator==(const TRefPointer<T>& rhs) const;
	/// 不等于
	bool operator!=(const TRefPointer<T>& rhs) const;
	/// 相等
	bool operator==(const T* rhs) const;
	/// 不等
	bool operator!=(const T* rhs) const;
	/// 安全的->运算符
	T* operator->() const;
	/// 安全的取值运算符
	T& operator*() const;
	/// 安全的指针转换
	operator T*() const;
	/// 不安全的, 转换为任意类型指针
	template<class U> const TRefPointer<U>& cast() const;
	/// 检查是否合法
	bool isvalid() const;
	/// 直接返回指针 (空指针会Ast)
	T* get() const;
	/// 直接返回指针 (空指针直接返回)
	T* get_unsafe() const;
	/// 返回指针地址 (替代&)
	T** address();

private:
	T* ptr;
};
/// 用于智能指针声明的宏定义
#define SmartPointer(classname) \
	class classname; \
	typedef TRefPointer<classname> classname##Ptr

//------------------------------------------------------------------------------
template<class T>
T** TRefPointer<T>::address()
{
	return &this->ptr;
}

//------------------------------------------------------------------------------
template<class T>
T* TRefPointer<T>::get_unsafe() const
{
	return (this->ptr);
}
//------------------------------------------------------------------------------
template<class T>
T* TRefPointer<T>::get() const
{
	Ast(NULL != this->ptr);
	return this->ptr;
}
//------------------------------------------------------------------------------
template<class T>
bool TRefPointer<T>::isvalid() const
{
	return (NULL != this->ptr);
}
//------------------------------------------------------------------------------
template<class T>
template<class U> const TRefPointer<U>&
TRefPointer<T>::cast() const
{
	return *(TRefPointer<U>*)this;
}
//------------------------------------------------------------------------------
template<class T>
TRefPointer<T>::operator T*() const
{
	//Ast(NULL != this->ptr);
	return this->ptr;
}
//------------------------------------------------------------------------------
template<class T>
T& TRefPointer<T>::operator*() const
{
	Ast(NULL != this->ptr);
	return *this->ptr;
}
//------------------------------------------------------------------------------
template<class T>
T* TRefPointer<T>::operator->() const
{
	Ast(NULL != this->ptr);
	return this->ptr;
}
//------------------------------------------------------------------------------
template<class T>
bool TRefPointer<T>::operator!=( const T* rhs ) const
{
	return (this->ptr != rhs);
}
//------------------------------------------------------------------------------
template<class T>
bool TRefPointer<T>::operator==( const T* rhs ) const
{
	return (this->ptr == rhs);
}
//------------------------------------------------------------------------------
template<class T>
bool TRefPointer<T>::operator!=( const TRefPointer<T>& rhs ) const
{
	return (this->ptr != rhs.ptr);
}
//------------------------------------------------------------------------------
template<class T>
bool TRefPointer<T>::operator==( const TRefPointer<T>& rhs ) const
{
	return (this->ptr == rhs.ptr);
}
//------------------------------------------------------------------------------
template<class T>
void TRefPointer<T>::operator=( T* rhs )
{
	if (this->ptr != rhs)
	{
		if (NULL != this->ptr)
		{
			this->ptr->Release();
		}
		this->ptr = rhs;
		if (NULL != this->ptr)
		{
			this->ptr->AddRef();
		}
	}
}
//------------------------------------------------------------------------------
template<class T>
void TRefPointer<T>::operator=( const TRefPointer<T>& rhs )
{
	if (this->ptr != rhs.ptr)
	{
		if (NULL != this->ptr)
		{
			this->ptr->Release();
		}
		this->ptr = rhs.ptr;
		if (NULL != this->ptr)
		{
			this->ptr->AddRef();
		}
	}
}
//------------------------------------------------------------------------------
template<class T>
TRefPointer<T>::~TRefPointer()
{
	if (NULL != this->ptr)
	{
		this->ptr->Release();
		this->ptr = NULL;
	}
}
//------------------------------------------------------------------------------
template<class T>
TRefPointer<T>::TRefPointer( const TRefPointer<T>& p )
		: ptr(p.ptr)
{
	if (NULL != this->ptr)
	{
		this->ptr->AddRef();
	}
}
//------------------------------------------------------------------------------
template<class T>
TRefPointer<T>::TRefPointer( T* p )
		: ptr(p)
{
	if (NULL != this->ptr)
	{
		this->ptr->AddRef();
	}
}
//------------------------------------------------------------------------------
template<class T>
TRefPointer<T>::TRefPointer()
		: ptr(NULL)
{
	// empty
}

}// namespace sqr
