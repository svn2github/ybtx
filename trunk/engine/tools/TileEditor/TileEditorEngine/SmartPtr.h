//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

namespace bg
{
namespace sc
{

template <class T>
class Ptr
{
public:
	Ptr() { m_object = 0; }
	
	~Ptr()	{ if ( m_object ) m_object->drop(); }
	
	Ptr(const Ptr<T>& other) 
	{
		T * obj = other.ptr(); 
		if ( obj ) obj->grab(); 
		m_object = obj;
	}
	
	Ptr(T * other)
	{
		if ( other ) other->grab(); 
		m_object = other;
	}
	
	Ptr<T>& operator = (const Ptr<T> & other)
	{
		T * obj = other.ptr(); 
		
		if ( obj ) 	
		{
			obj->grab();
		}
		
		if ( m_object )
		{ 
			m_object->drop();
		} 
		
		m_object = obj; 
		return * this;
	}
	
	bool operator == (const T * other) const
	{
		return m_object == other;
	}
	
	bool operator == ( const Ptr<T>& other ) const
	{
		return m_object == other.m_object;
	}

	//bool operator != ( const T* other ) const
	//{
	//	return m_object != other;
	//}
	
	bool operator != ( const Ptr<T>& other ) const
	{
		return m_object != other.m_object;
	}

	T& operator * () const
	{
		return *m_object;
	}

	T* operator -> () const
	{
		return m_object;
	}

	operator T * () const
	{
		return m_object;
	}

	T * ptr() const
	{
		return m_object;
	}

	// for handwork grab;
	void grab() 
	{
		if ( m_object ) m_object->grab();
	}

	// for handwork grab;
	void drop() 
	{
		if ( m_object ) m_object->drop();
	}

private:
	T * m_object;
};


}
}