#include "stdafx.h"
#include "CSyncPipeInBuffer.h"
#include "Memory.h"
#include "ExpHelper.h"
#include "CompressLZO.h"
#include "PipeTypes.h"
#include "ErrLogHelper.h"
#include "CByteDecoder.h"
#include "CCompressedDataHeader.h"

inline void CSyncPipeInBuffer::CTinyBuffer::Init(size_t stBufferSize)
{
	m_stBufferSize	=stBufferSize;
	m_szBuffer = (char*)SqrNew(m_stBufferSize * sizeof(char), CNetworkRecvBufferMallocObject::GetPool());
	m_stDataSize = 0;
}

inline void CSyncPipeInBuffer::CTinyBuffer::Init()
{
	m_szBuffer=NULL;
	m_stBufferSize=0;
	m_stDataSize=0;
}

inline void CSyncPipeInBuffer::CTinyBuffer::Unit()
{
	if( m_szBuffer )
		SqrDelete( m_szBuffer, CNetworkRecvBufferMallocObject::GetPool() );
}

inline char* CSyncPipeInBuffer::CTinyBuffer::GetWritePos()const
{
	return m_szBuffer + m_stDataSize;
}

inline size_t CSyncPipeInBuffer::CTinyBuffer::GetFreeSize()const
{
	return m_stBufferSize - m_stDataSize;
}

CSyncPipeInBuffer::CSyncPipeInBuffer(size_t stInBufferSize,bool bProcessCompressedData,bool bDecryptData)
:m_stMaxInBufferSize( stInBufferSize )
,m_bDecompressData ( bProcessCompressedData )
,m_bDecryptData ( bDecryptData )
{
	if( m_bDecompressData )
	{
		m_Buffer.Init( max<size_t>( stInBufferSize, WRITE_BUFFER_CHUNK_SIZE ) );
		//这是bzip2的压缩后数据大小的上界的计算方法，参见CompressBZIP2.cpp或者bzip2文档
		m_CompressedBuffer.Init( size_t( WRITE_BUFFER_CHUNK_SIZE * 1.01f+0.5f) + 600 );		
	}
	else
	{		
		if( bDecryptData )
		{
			//至少要留4个字节存放握手协议			
			m_Buffer.Init( max<size_t>( stInBufferSize , 4 ) );
		}
		else
		{
			m_Buffer.Init( stInBufferSize );
		}
		m_CompressedBuffer.Init();
	}

	m_bRecvHandShaken	=false;	
}

CSyncPipeInBuffer::~CSyncPipeInBuffer(void)
{
	m_Buffer.Unit();
	m_CompressedBuffer.Unit();
}

void CSyncPipeInBuffer::InBufferClear()
{
	m_bRecvHandShaken = false;
	m_Buffer.m_stDataSize=0;
	m_CompressedBuffer.m_stDataSize=0;
}


char* CSyncPipeInBuffer::InBufferGetWritePos()
{
	if( !m_bDecompressData )
		return m_Buffer.GetWritePos();

	return m_CompressedBuffer.GetWritePos();
}

size_t CSyncPipeInBuffer::InBufferGetLeftSpaceSize()const
{
	if( !m_bDecompressData )
		return m_Buffer.GetFreeSize();

	return m_CompressedBuffer.GetFreeSize();
}

#ifdef _WIN32

CSyncPipeInBuffer::EProcessRecvDataResult CSyncPipeInBuffer::InBufferTellAddedDataPosAndSize(char* pBeginPos,size_t stNewDataSize)
{
	char* pWritePos = InBufferGetWritePos();

	if( pWritePos != pBeginPos )
	{
		Ast( pWritePos < pBeginPos );
		memmove( pWritePos, pBeginPos, stNewDataSize );
	}
	return InBufferTellAddedDataSize( stNewDataSize );
}

#endif

CSyncPipeInBuffer::EProcessRecvDataResult CSyncPipeInBuffer::InBufferTellAddedDataSize(size_t stNewDataSize)
{
	if( m_bDecompressData )
	{
		if( !DecodeBuffer( m_CompressedBuffer, stNewDataSize ) )
			return ePRDR_Nothing;

		return DecompressedData();
	}
	else
	{	
		if( !DecodeBuffer( m_Buffer, stNewDataSize ) )
			return ePRDR_Nothing;

		return m_Buffer.m_stDataSize > 0 ? ePRDR_NewData : ePRDR_Nothing;
	}
}

bool CSyncPipeInBuffer::DecodeBuffer( CTinyBuffer& Buffer , size_t stNewDataSize )
{
	Buffer.m_stDataSize += stNewDataSize;

	if( m_bDecryptData && (!m_bRecvHandShaken) )
	{
		if( Buffer.m_stDataSize < 4 )
		{
			//puts("data not long enough for shake hand");
			return false;	//长度不足4，握手key数据无法获取
		}

		m_bRecvHandShaken = true;
		//puts("hand shaken.");

		m_Decoder.Reset( * reinterpret_cast<uint32*>( Buffer.m_szBuffer ) );
		Buffer.m_stDataSize-= sizeof(uint32);

		memmove(  Buffer.m_szBuffer,  Buffer.m_szBuffer+4 , Buffer.m_stDataSize );

		stNewDataSize = Buffer.m_stDataSize;

		if( stNewDataSize == 0 )
			return false;	
	}

	if( m_bDecryptData )
		m_Decoder.DecodeBuffer( Buffer.GetWritePos()-stNewDataSize, stNewDataSize );

	return true;
}


CSyncPipeInBuffer::EProcessRecvDataResult CSyncPipeInBuffer::DecompressedData()
{	
	EProcessRecvDataResult eResult = ePRDR_Nothing;
	
	for( ;; )
	{
		if( m_CompressedBuffer.m_stDataSize < sizeof( CDataHeader ) )
			break;

		CDataHeader* pHeader = reinterpret_cast<CDataHeader*>( m_CompressedBuffer.m_szBuffer );
		
		const bool bCompressed = pHeader->m_uCompressed != 0;

		size_t stOrgDataSize;
		size_t stTotalDataSize;
		char*const pWritePos=m_Buffer.GetWritePos();

		if( bCompressed )
		{
			if( m_CompressedBuffer.m_stDataSize < sizeof( CCompressedDataHeader ) )
			{
				//数据长度不足与获取基本的数据长度信息
				break;
			}

			CCompressedDataHeader* pCompressedHeader = reinterpret_cast<CCompressedDataHeader*>( pHeader );

			const size_t stDataSize = pCompressedHeader->m_uCompressedDataLen;

			stTotalDataSize = stDataSize + sizeof( CCompressedDataHeader );

			if( m_CompressedBuffer.m_stDataSize < stTotalDataSize )
			{
				//已经收到的数据不完整，不足以用于解压
				//printf("not enough data, stDataSize:%d m_CompressedBuffer.m_stDataSize:%d\n",int(stDataSize),int(m_CompressedBuffer.m_stDataSize));
				break;
			}

			stOrgDataSize = pCompressedHeader->m_uDataLen;

			if( m_Buffer.m_stBufferSize < m_Buffer.m_stDataSize + stOrgDataSize )
			{
				//缓冲区空间不足以放下解压后的数据			
				//puts("compress buffer full");
				break;
			}

			if( !DecompressBufferLZO( pWritePos , stOrgDataSize , m_CompressedBuffer.m_szBuffer + sizeof( CCompressedDataHeader ), stDataSize ) )
			{
				LogErr("网络数据解压失败");
				return ePRDR_Error;
			}

			//printf("compressed orgsize:%d datasize:%d m_Buffer.m_stDataSize:%d m_CompressedBuffer.m_stDataSize:%d\n"
			//	,int(stOrgDataSize),int(stDataSize),int(m_Buffer.m_stDataSize),int(m_CompressedBuffer.m_stDataSize));
		}
		else
		{
			stOrgDataSize = pHeader->m_uDataLen;

			stTotalDataSize = stOrgDataSize + sizeof( CDataHeader );

			if( m_CompressedBuffer.m_stDataSize < stTotalDataSize )
				break;

			//缓冲区空间不足
			if( m_Buffer.m_stBufferSize < m_Buffer.m_stDataSize + stOrgDataSize )
			{
				//puts("not compress buffer full");
				break;
			}

			memcpy( pWritePos , m_CompressedBuffer.m_szBuffer+ sizeof( CDataHeader ) , stOrgDataSize );

			//printf("not compressed orgsize:%d m_Buffer.m_stDataSize:%d m_CompressedBuffer.m_stDataSize:%d\n"
			//	,int(stOrgDataSize),int(m_Buffer.m_stDataSize),int(m_CompressedBuffer.m_stDataSize));
		}

		eResult = ePRDR_NewData;

		m_Buffer.m_stDataSize += stOrgDataSize;

		Ast( m_CompressedBuffer.m_stDataSize >= stTotalDataSize );
		m_CompressedBuffer.m_stDataSize -= stTotalDataSize;

		memmove( m_CompressedBuffer.m_szBuffer,m_CompressedBuffer.m_szBuffer+stTotalDataSize,m_CompressedBuffer.m_stDataSize );
		
		//printf("after copy m_Buffer.m_stDataSize:%d m_CompressedBuffer.m_stDataSize:%d\n",int(m_Buffer.m_stDataSize),int(m_CompressedBuffer.m_stDataSize));
	}

	//puts("leave compress");

	return eResult;
}



size_t CSyncPipeInBuffer::InBufferGetSize()const
{
	return min( m_Buffer.m_stBufferSize , m_stMaxInBufferSize );
}

bool CSyncPipeInBuffer::InBufferIsFull()const
{
	return InBufferGetDataSize() >= InBufferGetSize();
}


char* CSyncPipeInBuffer::InBufferGetData()const
{
	return m_Buffer.m_szBuffer;
}


size_t CSyncPipeInBuffer::InBufferGetDataSize()const
{
	if( m_bDecryptData && !m_bRecvHandShaken )
		return 0;

	return min( m_Buffer.m_stDataSize , m_stMaxInBufferSize );
}


bool CSyncPipeInBuffer::InBufferPopData(size_t stPopDataSize)
{
	if ( InBufferGetDataSize() < stPopDataSize )
	{
		ostringstream strm;
		strm << "数据长度: " << InBufferGetDataSize() << "  需要Pop的数据长度: " << stPopDataSize;
		GenErr("CSynPipeInBuffer Pop Error", strm.str().c_str());
	}

	const bool bExtraDataAvailable = ( m_Buffer.m_stDataSize>m_stMaxInBufferSize );

	m_Buffer.m_stDataSize-=stPopDataSize;
	memmove(m_Buffer.m_szBuffer,m_Buffer.m_szBuffer+stPopDataSize,m_Buffer.m_stDataSize);

	if( !m_bDecompressData )
		return bExtraDataAvailable;

	if( m_CompressedBuffer.m_stDataSize == 0 )
		return bExtraDataAvailable;

	const bool bHasNewDecompressedData = ( DecompressedData()==ePRDR_NewData );

	return bHasNewDecompressedData || bExtraDataAvailable ;
}
