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

class CRootObject
{
public:
	
	CRootObject( ) : m_refs(0)
	{
	}

	CRootObject( const CRootObject& ) : m_refs(0)
	{
	}

	virtual ~CRootObject()																		
	{
		assert( 0 == m_refs );
	}

	CRootObject& operator = ( const CRootObject& )
	{
		return * this;
	}

	void grab()
	{
		assert( m_refs >= 0 );
		InterlockedIncrement(&m_refs);
	}

	void drop()
	{
		assert( m_refs > 0 );
		if ( 0 == InterlockedDecrement(&m_refs) )
		{
			delete this;
		}
	}
	
private:
	long	m_refs;
};

}
}