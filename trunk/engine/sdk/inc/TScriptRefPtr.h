#pragma once
#include "CScript.h"

namespace sqr
{
	class CTestRefObj;
	template< typename type >
	class TScriptRefPtr
	{
		friend class CTestRefObj;	//for test
	public:
		TScriptRefPtr( CScript& Script )
			: m_pScript( &Script )
			, m_pData( NULL )
		{}

		~TScriptRefPtr()
		{
			m_pScript->UnrefScriptObj( m_pData );
		}
		type* operator = ( type* ptr )
		{
			if ( m_pData != ptr )
			{
				m_pScript->UnrefScriptObj( m_pData );
				m_pData = ptr;
				m_pScript->RefScriptObj( m_pData );
			}

			return m_pData;
		}

		type& operator*()
		{
			return *m_pData;
		}

		operator bool()
		{
			return (m_pData != NULL) ;
		}

	protected:
		CScript* m_pScript;
		type*		m_pData;
	};

	template<>
	class TScriptRefPtr<void>
	{
		friend class CTestRefObj;	//for test
	public:
		TScriptRefPtr( CScript& Script )
			: m_pScript( &Script )
			, m_pData( NULL )
		{}

		~TScriptRefPtr()
		{
			m_pScript->UnrefScriptObj( m_pData );
		}
		void* operator = ( void* ptr )
		{
			if ( m_pData != ptr )
			{
				m_pScript->UnrefScriptObj( m_pData );
				m_pData = ptr;
				m_pScript->RefScriptObj( m_pData );
			}

			return m_pData;
		}

		operator bool()
		{
			return (m_pData != NULL) ;
		}

		void* const addr()
		{
			return m_pData;
		}

	protected:
		CScript* m_pScript;
		void*		m_pData;
	};

	template<>
	class TScriptRefPtr<char*>
	{
		friend class CTestRefObj;	//for test
	public:
		TScriptRefPtr( CScript& Script )
			: m_pScript( &Script )
			, m_str( NULL )
		{}

		~TScriptRefPtr()
		{
			m_pScript->UnrefScriptObj( (void*)m_str );
		}
		char* operator = ( char* str )
		{
			if ( m_str != str )
			{
				m_pScript->UnrefScriptObj( (void*)m_str );
				m_str = str;
				m_pScript->RefScriptObj( (void*)m_str );
			}

			return m_str;
		}

		const char* addr()
		{
			return m_str;
		}

		char& operator[](int idx)
		{
			return m_str[idx];
		}

		operator bool()
		{
			return (m_str != NULL) ;
		}

	protected:
		CScript* m_pScript;
		char*		m_str;
	};
}

using namespace sqr;
