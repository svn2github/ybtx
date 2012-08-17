/////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2004 LiChunXia
//	欢迎随便使用
//
//  不需要对象本身有引用计数功能，引用计数和对象共存亡
//////////////////////////////////////////////////////////////////////////

#ifndef _share_ptr_H_
#define _share_ptr_H_

template <class T>
class share_ptr
{
private:
	/*
	*  禁止下列语义？
	*/
	share_ptr()
	{
	}

public:
	/*
	* 使用这种语义
	*/
	share_ptr(T* otherObj)
	{
		//if ( otherObj )
		//{
		m_pObj  = otherObj;
		m_pnRef = new int(1);
		//}
	}

	~share_ptr()
	{
		Release();
	}

	/*
	*   减少引用计数
	*/
	void Release()
	{
		if ( m_pnRef && --*m_pnRef == 0 )
		{
			if ( m_pObj )
				m_pObj->Release();
			m_pObj = NULL;

			delete m_pnRef;
			m_pnRef = NULL;
		}
	}


	/*
	* 判断是否为空
	*/
	bool IsNull()
	{
		return ( m_pObj == NULL );
	}

	/*
	* 重新产生一个对象
	*/
	void Reset(T* otherObj = 0)
	{
		//if(otherObj)
		//{
		Release();

		m_pObj = otherObj;
		m_pnRef  = new int(1);
		//}
	}


	/*
	* 拷贝构造
	*/
	share_ptr(const share_ptr<T>& other)
	{
		m_pObj  = other.m_pObj;
		m_pnRef = other.m_pnRef;

		++(*m_pnRef);
	}

	/*
	* 赋值运算符
	*/
	share_ptr<T>& operator = (const share_ptr<T> &other)
	{
		if ( this == &other || this->m_pObj == other.m_pObj )
			return *this;

		Release();

		m_pObj  = other.m_pObj;
		m_pnRef = other.m_pnRef;

		++(*m_pnRef);

		return *this;
	}

	bool operator == (const T * otherObj) const
	{
		return m_pObj == otherObj;
	}

	bool operator == ( const share_ptr<T>& other ) const
	{
		return m_pObj == other.m_pObj;
	}

	bool operator != ( const share_ptr<T>& other ) const
	{
		return m_pObj != other.m_pObj;
	}

	T& operator * () const
	{
		return *m_pObj;
	}

	T* operator -> () const
	{
		return m_pObj;
	}

	operator T * () const
	{
		return m_pObj;
	}

	/*T * CSharePtr() const
	{
	return m_pObj;
	}*/

	T* Get() const
	{
		return m_pObj;
	}

	long UseCount() const
	{
		return *m_pnRef;
	}

	bool IsUnique() const
	{
		return (*m_pnRef == 1);
	}

private:
	T*   m_pObj;
	int* m_pnRef;
};

#endif