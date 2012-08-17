//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyC.h"

#define MAX_FILE_NAME_SAVED  	   1024
#define TEXT_BUFFER_SZIE	 	   4096

#define BITS                       15
#define MAX_CODE                   ( ( 1 << BITS ) - 1 )
#define TABLE_SIZE                 35023L
#define TABLE_BANKS                ( ( TABLE_SIZE >> 8 ) + 1 )
#define LZW_END_OF_STREAM          256
#define BUMP_CODE                  257
#define FLUSH_CODE                 258
#define FIRST_CODE                 259
#define LZW_UNUSED                 -1

namespace bc
{
namespace io
{

class CMemFile
{
	class CInputBitStream
	{
		BYTE *m_pData;
	
		int m_byMask;
		int m_iRack;
	
		int m_iPosition;
		int m_iLength;
	
	public:
		CInputBitStream(BYTE *pData,int iSize);		
		~CInputBitStream(){};
	
		int InputBit();
		DWORD InputBits(int iCount);
	};
	
	class Compress_LZW15
	{
		struct dictionary {
			int code_value;
			int parent_code;
			char character;
		} *dict;
		
		
		char decode_stack[ TABLE_SIZE ];
		unsigned int next_code;
		int current_code_bits;
		unsigned int next_bump_code;

		void InitializeDictionary();
		unsigned int find_child_node( int parent_code, int child_character );
		unsigned int decode_string( unsigned int offset, unsigned int code );

	public:
		Compress_LZW15(void);
		~Compress_LZW15(void);

		void ExpandData(CInputBitStream *input,BYTE *outputData);
	};

public:
	BYTE * m_pbyData;

	int m_iPosition;	
	int m_iLength;

	char m_pszFileName[MAX_FILE_NAME_SAVED];

public:
	CMemFile(void);
	~CMemFile(void);

	BOOL Open(const char * pszFileName);
	void Close();

	const BYTE * GetData() const { return m_pbyData;}

	BOOL ReadChar(unsigned char &c);
	int  Read(void *pbyBuffer, int iSize);
	char * ReadLine(char * pszLineBuffer, int iBufferLen);

	int  Seek(int iPosition,int iOrigin = 0);

	void SetPosition(int iPosition) { m_iPosition = iPosition;}
	int  GetPosition() const  { return m_iPosition;}

	int  GetLength() const { return m_iLength;	}

	void SkipBytes(int iBytes);

	const char * GetPathName() const { return m_pszFileName;}
};

}
}