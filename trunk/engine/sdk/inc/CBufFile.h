#pragma once

#include "BaseTypes.h"
#include "ExpHelper.h"

namespace sqr
{
class CBufFile
{
protected:
	const char*	m_pBuf;
	size_t		m_uMaxSize;
	size_t		m_uCurPos;
	int32		m_uOffset;

	friend class CPkgFile;
	friend class CReadFileCallBackData;
public:
	CBufFile( const char* pBuf, size_t uMaxSize ) : m_pBuf( pBuf ), m_uMaxSize( uMaxSize ), m_uCurPos(0) {}
	CBufFile( const char* pBuf, size_t uMaxSize, int32 uOffset ) : 
		m_pBuf( pBuf ), m_uMaxSize( uMaxSize ), m_uCurPos(0), m_uOffset(uOffset) {}

	const char* GetBuf()const
	{
		return m_pBuf;
	}

	int32		GetBufSize()const
	{
		return (int32)m_uMaxSize;
	}

	int32		GetBufOffset()const
	{
		return m_uOffset;
	}

	int32		Tell()const
	{
		return (int32)m_uCurPos;
	}

	int32		Seek( int nPos,int origin=SEEK_SET )
	{
		size_t	uNewPos = 0;
		switch ( origin )
		{
			case SEEK_SET:
				uNewPos = nPos;
				break;
			case SEEK_CUR:
				uNewPos = m_uCurPos + nPos;
				break;
			case SEEK_END:
				uNewPos = m_uMaxSize + nPos;
				break;
		}
		if ( uNewPos > m_uMaxSize )
			return -1;

		m_uCurPos = uNewPos;
		return 0;
	}

	void Read( void* pBuf, size_t uLen )
	{
		if( m_uCurPos + uLen > m_uMaxSize )
		{
			GenErr("读取的数据长度超出文件的大小");
		}

		memcpy( pBuf, m_pBuf + m_uCurPos, uLen );
		m_uCurPos += uLen;
	}

	void read( void* pBuf, size_t uLen)
	{
		return Read(pBuf,uLen);
	}

	void SafeRead( void* pBuf, size_t uLen )
	{
		if( m_uCurPos + uLen > m_uMaxSize )
			return;
		memcpy( pBuf, m_pBuf + m_uCurPos, uLen );
		m_uCurPos += uLen;
	}

	template<class T>
	void SafeReadMap( T& buf )
	{
		size_t size = 0;
		SafeRead( &size );
		for ( size_t i = 0; i < size; ++i )
		{
			typename T::key_type		key;
			typename T::mapped_type	value;
			SafeRead(&key);
			SafeRead(&value);
		}
	}

	template<class T>
	void SafeRead( T* pBuf )
	{
		SafeRead( static_cast<void*>(pBuf), sizeof(T) );
	}

	template<class T>
	void operator >> ( T& pBuf )
	{
		SafeRead( static_cast<void*>(&pBuf), sizeof(T) );
	}
};

class CRefBufFile : public CBufFile
{
protected:
	uint32	m_Ref;
	uint32	m_uCheckCode;
	string	m_szFileName;

public:
	CRefBufFile( const char* pBuf, size_t uMaxSize, uint32 uCheckCode ) 
		: CBufFile( pBuf,uMaxSize )
		, m_Ref(0)
		, m_uCheckCode(uCheckCode)
		, m_szFileName("")
	{
	}

	CRefBufFile( const char* pBuf, size_t uMaxSize, int32 uOffset, uint32 uCheckCode ) 
		: CBufFile( pBuf,uMaxSize,uOffset )
		, m_Ref(0)
		, m_uCheckCode(uCheckCode)
		, m_szFileName("")
	{
	}

	~CRefBufFile() {if(m_pBuf)delete m_pBuf; }
	void	AddRef() { ++m_Ref; }
	void	Release() { if(0==(--m_Ref)){ delete this; }; }

	void	SetFileName( const char* szFileName )
	{
		if ( 0 != szFileName )
			m_szFileName = szFileName;
	}

	const char* 
			GetFileName() const
	{
		return m_szFileName.c_str();
	}

	uint32	GetCheckCode() const
	{
		return m_uCheckCode;
	}
};
}

