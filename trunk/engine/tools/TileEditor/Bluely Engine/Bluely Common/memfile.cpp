//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "memfile.h"
#include "Console.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bc
{
namespace io
{

CMemFile::CInputBitStream::CInputBitStream(BYTE * pData,int iSize)
{
	m_pData = pData;
	m_iLength = iSize;

	m_iRack  = 0;
	m_byMask = 0x80;
	m_iPosition = 0;
}

int CMemFile::CInputBitStream::InputBit()
{
	int value;

	if ( m_byMask == 0x80 ) 
	{
		m_iRack = m_pData[m_iPosition++];
		if ( m_iPosition > m_iLength)
			LogConsole("error: CMemFile::CInputBitStream : fatal error in InputBit...");
	}

	value = m_iRack & m_byMask;
	m_byMask >>= 1;
	if ( m_byMask == 0 )
	{
		m_byMask = 0x80;
	}

	return  (value) ? 1 : 0 ;
}

DWORD CMemFile::CInputBitStream::InputBits(int iCount)
{
	unsigned int mask;
	unsigned int return_value;

	mask = 1L << ( iCount - 1 );
	return_value = 0;

	while ( mask != 0)
	{
		if ( m_byMask == 0x80 ) 
		{
			m_iRack = m_pData[m_iPosition++];
			if ( m_iPosition > m_iLength)
				LogConsole("error : CMemFile::CInputBitStream : fatal error in InputBits...");
		}

		if ( m_iRack & m_byMask )
		{
			return_value |= mask;
		}

		mask >>= 1;
		m_byMask >>= 1;
		if ( m_byMask == 0 )
		{
			m_byMask = 0x80;
		}
	}
	return( return_value );
}

CMemFile::Compress_LZW15::Compress_LZW15(void)
{
	dict = new dictionary[TABLE_SIZE];
}

CMemFile::Compress_LZW15::~Compress_LZW15(void)
{
	SAFE_DELETE(dict);
}

void CMemFile::Compress_LZW15::InitializeDictionary()
{
	unsigned int i;

	for ( i = 0 ; i < TABLE_SIZE ; i++ )
		dict[ i].code_value = LZW_UNUSED;
	next_code = FIRST_CODE;
	putc( 'F', stdout );
	current_code_bits = 9;
	next_bump_code = 511;
}

void CMemFile::Compress_LZW15::ExpandData(CInputBitStream *input,BYTE *outputData)
{
	unsigned int new_code;
	unsigned int old_code;
	int character;
	unsigned int count;
	int iOutputCount = 0;

	for ( ; ; ) {
		InitializeDictionary();
		old_code = (unsigned int) input->InputBits(current_code_bits );
		if ( old_code == LZW_END_OF_STREAM )
			return;
		character = old_code;
		outputData[iOutputCount++] = old_code;
		for ( ; ; ) {
			new_code = (unsigned int) input->InputBits(current_code_bits );
			if ( new_code == LZW_END_OF_STREAM )
				return;
			if ( new_code == FLUSH_CODE )
				break;
			if ( new_code == BUMP_CODE ) {
				current_code_bits++;
				putc( 'B', stdout );
				continue;
			}
			if ( new_code >= next_code ) {
				decode_stack[ 0 ] = (char) character;
				count = decode_string( 1, old_code );
			}
			else
				count = decode_string( 0, new_code );
			character = decode_stack[ count - 1 ];
			while ( count > 0 )
				outputData[iOutputCount++] = decode_stack[ --count ];
			dict[ next_code ].parent_code = old_code;
			dict[ next_code ].character = (char) character;
			next_code++;
			old_code = new_code;
		}
	}
}

unsigned int CMemFile::Compress_LZW15::find_child_node(int parent_code,int child_character)
{
	unsigned int index;
	unsigned int offset;

	index = ( child_character << ( BITS - 8 ) ) ^ parent_code;
	if ( index == 0 )
		offset = 1;
	else
		offset = TABLE_SIZE - index;
	for ( ; ; ) {
		if ( dict[ index ].code_value == LZW_UNUSED )
			return( (unsigned int) index );
		if ( dict[ index ].parent_code == parent_code &&
			dict[ index ].character == (char) child_character )
			return( index );
		if ( (int) index >= offset )
			index -= offset;
		else
			index += TABLE_SIZE - offset;
	}
}

unsigned int CMemFile::Compress_LZW15::decode_string(unsigned int count,unsigned int code)
{
	while ( code > 255 ) {
		decode_stack[ count++ ] = dict[ code ].character;
		code = dict[ code ].parent_code;
	}
	decode_stack[ count++ ] = (char) code;
	return( count );
}

CMemFile::CMemFile()
{	
	m_pbyData = NULL;
	ZeroMemory(m_pszFileName,MAX_FILE_NAME_SAVED);
	m_iPosition = m_iLength = 0;
}

CMemFile::~CMemFile()
{
	assert(!m_pbyData); // file still open
	SAFE_DELETE(m_pbyData);
}

BOOL CMemFile::Open(const char * pszFileName)
{
	assert(!m_pbyData);

	BOOL bUsePackageFileMode = FALSE;

	FILE * pFile(NULL);	
	strncpy((char*)m_pszFileName,pszFileName,MAX_FILE_NAME_SAVED - 1);
	m_pszFileName[MAX_FILE_NAME_SAVED-1] = 0;
	pFile = fopen(pszFileName,"rb");
	if (!pFile)
	{
		// find the compressed version
		_snprintf(m_pszFileName,MAX_FILE_NAME_SAVED,"%s.package",pszFileName);
		pFile = fopen(m_pszFileName,"rb");	
		bUsePackageFileMode = TRUE;
	}

	if (!pFile)
	{
		LogConsole("warning: CMemFile : file open failed...");	
		return false;
	}

	fseek(pFile,0,SEEK_END);
	m_iLength = ftell(pFile);
	fseek(pFile,0,SEEK_SET);

	assert(m_iLength);

	m_pbyData = new unsigned char [m_iLength];

	long lReadSize = (long)fread(m_pbyData,1,m_iLength,pFile);

	assert(lReadSize == m_iLength);

	m_iPosition = 0;
	fclose(pFile);

	if (bUsePackageFileMode)
	{
		Seek(16 ,SEEK_SET);
		int iFileOriginSize;
		int iFileCompressSize;
		DWORD dwMethod;

		Read(&dwMethod,sizeof(DWORD));
		Read(&iFileOriginSize,sizeof(int));
		Read(&iFileCompressSize,sizeof(int));

		switch(dwMethod)
		{
		case 0x35312e4c://"L.15" L15
			{
				BYTE *pDecodeBuffer = new BYTE[iFileOriginSize];

				// compressed data
				CInputBitStream inputBits((m_pbyData + m_iPosition),iFileCompressSize);

				Compress_LZW15 decoder;
				decoder.ExpandData(&inputBits,pDecodeBuffer);

				// swap cache
				SAFE_DELETE(m_pbyData);
				m_pbyData = pDecodeBuffer;
				m_iPosition = 0;
				m_iLength = iFileOriginSize;
			}
		break;
		
		case 0x454e4f4e://"NONE" not compressed
			{
				BYTE *pDecodeBuffer = new BYTE[iFileOriginSize];

				memcpy(pDecodeBuffer,m_pbyData + m_iPosition,iFileOriginSize);

				SAFE_DELETE(m_pbyData);
				m_pbyData = pDecodeBuffer;

				m_iPosition = 0;
				m_iLength = iFileOriginSize;
			}
		break;
		}
	}

	return TRUE;
}

void CMemFile::Close()
{
	SAFE_DELETE(m_pbyData);
	m_iPosition = 0;
	m_iLength = -1;
}

int CMemFile::Seek(int iPosition,int iOrigin)
{
	int iCurPosition = m_iPosition;
	switch(iOrigin)
	{
	case SEEK_CUR:
		m_iPosition +=iPosition;
		break;
	case SEEK_END:
		m_iPosition  = m_iLength + iPosition; 
		assert(m_iPosition >= 0);
		break;
	case SEEK_SET:
	default: m_iPosition= iPosition;
	}
	return iCurPosition;
}

char * CMemFile::ReadLine(char * pszLineBuffer, int iBufferLen)
{
	int  iLineLen = 0;
	BOOL bCount = FALSE;

	ZeroMemory(pszLineBuffer,iBufferLen);

	// from now on
	while(m_iPosition < m_iLength && iLineLen < iBufferLen - 1)
	{
		char c = m_pbyData[m_iPosition++];
		if ( '\r' == c )
		{
			if (m_iPosition+1 < m_iLength)
			{
				c = m_pbyData[m_iPosition+1];
				if ( '\n' == c )
				{
					m_iPosition++;
				}
			}
			break;
		}
		else
			if ( '\n' == c )
			{
				break;
			}

			pszLineBuffer[iLineLen++] = c;

	}

	return pszLineBuffer;
}

int CMemFile::Read(void * pbyBuffer, int iSize)
{
	int iLeft = m_iLength - m_iPosition;
	int iWantReadSize  = (iSize > iLeft)? iLeft : iSize;
	assert(iWantReadSize>0);

	memcpy(pbyBuffer,m_pbyData+m_iPosition,iWantReadSize);

	m_iPosition +=iWantReadSize;

	return iWantReadSize;
}

BOOL CMemFile::ReadChar(unsigned char & c)
{
	if (m_iPosition >= m_iLength) return FALSE; // out of bounds

	c = m_pbyData[m_iPosition++];	
	return TRUE;
}

void CMemFile::SkipBytes(int iBytes)
{
	m_iPosition += iBytes;
}

}
}