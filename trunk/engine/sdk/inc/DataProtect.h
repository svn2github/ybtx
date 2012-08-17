#ifndef _DATAPROTECT_H
#define _DATAPROTECT_H

#include "VirtualizerSDK.h"

namespace sqr
{
namespace DataProtect
{
template<class T>
class CDataProtectMgr
{
	map<unsigned,T*> m_mapValues;

	inline unsigned Void2Unsigned( const uint8* p )
	{
		unsigned short* pU16 = (unsigned short*)&p;
		unsigned short m = pU16[0]*0xe941 + pU16[1]*0x9843;
		unsigned short n = pU16[0]*0xab28 + pU16[1]*0xc4a5;
		return ( unsigned(m)<<16 )|n;
	}

public:
	void RegistValue( uint8* p )
	{
		VIRTUALIZER_START
		m_mapValues[ Void2Unsigned( p ) ] = NULL;
		VIRTUALIZER_END
	}

	void RemoveValue( uint8* p )
	{
		VIRTUALIZER_START
		map<unsigned,T*>::iterator it = m_mapValues.find( Void2Unsigned( p ) );
		delete []it->second;
		m_mapValues.erase( it );
		VIRTUALIZER_END
	}

	T Get( const uint8* p )
	{
		VIRTUALIZER_START
		map<unsigned,T*>::iterator it = m_mapValues.find( Void2Unsigned( p ) );
		Ast( it != m_mapValues.end() );

		T a = it->second[ uint8( p[0] + 5 ) ];
		uint8* pBuf = (uint8*)&a;
		for ( int i = 0; i < sizeof(T); i++ )
			pBuf[i] = pBuf[i]^0x78;

		VIRTUALIZER_END
		return a;
	}

	const T& Set( uint8* p, const T& value )
	{
		VIRTUALIZER_START
		T* pNew = new T[256];
		map<unsigned,T*>::iterator it = m_mapValues.find( Void2Unsigned( p ) );
		Ast( it != m_mapValues.end() );
		delete []it->second;
		it->second = pNew;
		p[0] = CRandGen::Rand( 0, 256 );

		T a = value;
		uint8* pBuf = (uint8*)&a;
		for ( int i = 0; i < sizeof(T); i++ )
			pBuf[i] = pBuf[i]^0x78;
		it->second[uint8( p[0] + 5 ) ] = a;

		VIRTUALIZER_END
		return value;
	}

	static CDataProtectMgr<T>& Instance()
	{
		static CDataProtectMgr<T> _instance;
		return _instance;
	}
};

template<class T>
class CDataProtect
{
	uint8 uMagicValue;
public:
	CDataProtect()
	{
		CDataProtectMgr<T>::Instance().RegistValue( &uMagicValue );
	}

	CDataProtect( const T& Value )
	{
		CDataProtectMgr<T>::Instance().RegistValue( &uMagicValue );
		CDataProtectMgr<T>::Instance().Set( &uMagicValue, Value );
	}

	~CDataProtect()
	{
		CDataProtectMgr<T>::Instance().RemoveValue( &uMagicValue );
	}

	operator T() const
	{
		return CDataProtectMgr<T>::Instance().Get( &uMagicValue );
	}

	const T& operator= ( const T& Value )
	{
		return CDataProtectMgr<T>::Instance().Set( &uMagicValue, Value );
	}

	const T& operator= ( const CDataProtect<T>& Value )
	{
		return CDataProtectMgr<T>::Instance().Set( &uMagicValue, (T)Value );
	}
};
}
}

::DataProtect;

#endif