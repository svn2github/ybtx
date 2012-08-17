#pragma once
#include "stdafx.h"
#include "IDName.h"

sqr::IDNAME::IDNAME()
{
	memset( m_Buf, 0, 16 );
}

sqr::IDNAME::IDNAME( const char* str )
{
	memset( m_Buf, 0, 16 );
	if ( str )
	{
		size_t len = min( strlen( str ), 15 );
		memcpy( m_Buf, str, len );
	}
}

const IDNAME& sqr::IDNAME::operator=( const IDNAME& BN )
{
	memset(m_Buf,0,16);
	m_Buf[0] = BN.m_Buf[0];
	m_Buf[1] = BN.m_Buf[1];
	//m_Buf[2] = BN.m_Buf[2];
	//m_Buf[3] = BN.m_Buf[3];
	return *this;
}

const IDNAME& sqr::IDNAME::operator=( const char* str )
{
	memset( m_Buf, 0, 16 );
	if ( str )
	{
		size_t len = min( strlen( str ), 15 );
		memcpy( m_Buf, str, len );
	}
	return *this;
}

bool sqr::IDNAME::operator==( const IDNAME& BN ) const
{
	return 	m_Buf[0] == BN.m_Buf[0] && m_Buf[1] == BN.m_Buf[1];
}

bool sqr::IDNAME::operator!=( const IDNAME& BN ) const
{
	return 	m_Buf[0] != BN.m_Buf[0] || m_Buf[1] != BN.m_Buf[1];
}

sqr::IDNAME::operator const char*() const
{
	return (const char*)m_Buf;
}