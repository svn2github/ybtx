#include "stdafx.h"
#include "CSyncPipeOutBuffer.h"
#include "TSqrAllocator.inl"
#include "Memory.h"
#include "ExpHelper.h"
#include "CompressLZO.h"
#include "PipeTypes.h"
#include "CCompressedDataHeader.h"
#include "CMemData.inl"
#include "CByteEncoder.inl"


CSyncPipeOutBuffer::CSyncPipeOutBuffer(size_t stOutBufferSize,bool bCompressData,bool bEncryptData)
:m_bufCompressedData( CNetworkSendBufferMallocObject::GetPool() )
,m_bCompressData ( bCompressData )
,m_bEncryptData( bEncryptData )
{
	m_stOutBufferSize=stOutBufferSize;
	m_stOutBufferSegNum=0;
	OutBufferClear();
}

CSyncPipeOutBuffer::~CSyncPipeOutBuffer(void)
{
	for(BufferList_t::iterator it=m_lstOutBuffer.begin();it!=m_lstOutBuffer.end();++it)
	{
		SqrDelete(*it, CNetworkSendBufferMallocObject::GetPool());
	}
}

void CSyncPipeOutBuffer::OutBufferClear()
{
	m_stOutHeadDataBegin=0;
	m_stOutTailDataEnd=0;
	
	for(BufferList_t::iterator it=m_lstOutBuffer.begin();it!=m_lstOutBuffer.end();++it)
	{
		SqrDelete(*it, CNetworkSendBufferMallocObject::GetPool());
	}

	m_lstOutBuffer.clear();
	m_lstOutBuffer.push_back((char*)SqrNew(WRITE_BUFFER_CHUNK_SIZE * sizeof(char), 
		CNetworkSendBufferMallocObject::GetPool()));
	m_stOutBufferSegNum=1;

	m_bufCompressedData.Clear();
}

#ifdef _WIN32
void CSyncPipeOutBuffer::PrepareSendData(WSABUF*& pBuffer,DWORD& dwBufferCount)
{
	if( !m_bCompressData )
	{
		if( m_stOutTailDataEnd>0 )
		{
			dwBufferCount=(DWORD)m_stOutBufferSegNum;
			pBuffer=new WSABUF[dwBufferCount];
			pBuffer[0].buf=m_lstOutBuffer.front()+m_stOutHeadDataBegin;

			BufferList_t::iterator it=++m_lstOutBuffer.begin();
			BufferList_t::iterator end=m_lstOutBuffer.end();

			if( it==end )
			{
				pBuffer[0].len=(u_long)(m_stOutTailDataEnd-m_stOutHeadDataBegin);
			}
			else
			{
				pBuffer[0].len=(u_long)(WRITE_BUFFER_CHUNK_SIZE-m_stOutHeadDataBegin);

				if(it==--end)
				{
					pBuffer[1].buf=*it;
					pBuffer[1].len=(u_long)m_stOutTailDataEnd;
				}
				else
				{
					for(size_t i=1;it!=end;++it,++i)
					{
						pBuffer[i].buf=*it;
						pBuffer[i].len=WRITE_BUFFER_CHUNK_SIZE;
					}
					pBuffer[m_stOutBufferSegNum-1].buf=*end;
					pBuffer[m_stOutBufferSegNum-1].len=(u_long)m_stOutTailDataEnd;
				}
			}
		}
		else
		{
			dwBufferCount=(uint32)(m_stOutBufferSegNum-1);
			pBuffer=new WSABUF[dwBufferCount];
			pBuffer[0].buf=m_lstOutBuffer.front()+m_stOutHeadDataBegin;
			pBuffer[0].len=(u_long)(WRITE_BUFFER_CHUNK_SIZE-m_stOutHeadDataBegin);

			BufferList_t::iterator it=++m_lstOutBuffer.begin();
			BufferList_t::iterator end=--m_lstOutBuffer.end();

			for(size_t i=1;it!=end;++it,++i)
			{
				pBuffer[i].buf=*it;
				pBuffer[i].len=WRITE_BUFFER_CHUNK_SIZE;
			}
		}
	}
	else
	{
		CompressDataIfNeeded();

		dwBufferCount=1;
		pBuffer=new WSABUF;
		pBuffer->buf=m_bufCompressedData.Data();
		pBuffer->len=u_long(m_bufCompressedData.Size());
		/*FILE* fp=fopen("send.txt","a");
		fwrite( pBuffer->buf,1,pBuffer->len,fp);
		fclose(fp);*/
	}
}
#endif




void CSyncPipeOutBuffer::CompressDataIfNeeded()
{
	if( !m_bufCompressedData.Empty() )
		return;

	const size_t stChunkDataSize = IntGetChunkDataSize();

	if( stChunkDataSize == 0 )
		return;

	const char* szChunkData = IntGetChunkData();

	//2 byte 未压缩长度
	//1 byte 标记是否压缩
	//2 byte 压缩后长度（如果未压缩，则没有这2byte）
	
	//150是一个经验数值，长度达到150的时候，压缩率一般能到70%，太小的数据包不进入压缩，可以减少cpu消耗
	if( stChunkDataSize < 150 )
	{
		m_bufCompressedData.ReSize( sizeof( CDataHeader ) + stChunkDataSize );

		CDataHeader* pHeader=m_bufCompressedData.AsPtr<CDataHeader>();
		
		pHeader->m_uCompressed = 0;
		pHeader->m_uDataLen = uint16(stChunkDataSize);
		
		if( m_bEncryptData )
		{
			char* p = reinterpret_cast<char*>( pHeader );

			//header的长度为2，所以执行两次byte encode.
			*p=m_Encoder.Encode(*p);
			++p;
			*p=m_Encoder.Encode(*p);
			++p;
			
			char* const pEnd = p + stChunkDataSize;

			for( ; p<pEnd ; ++p,++szChunkData )
				*p=m_Encoder.Encode(*szChunkData);
		}
		else
		{
			m_bufCompressedData.Assign( sizeof( CDataHeader ) , szChunkData , stChunkDataSize );
		}

		//printf("not compress stChunkDataSize:%d m_sCompressedDataSize:%d\n",int(stChunkDataSize),int(m_bufCompressedData.Size()));
	}
	else
	{
		m_bufCompressedData.ReSize( sizeof( CCompressedDataHeader) );
		
		//切记设置数据要在这个函数后面，因为压缩完之后，string内部的所有内存可能都已经重新分配了
		CompressBufferLZO( m_bufCompressedData,szChunkData ,stChunkDataSize );

		CCompressedDataHeader* pHeader= m_bufCompressedData.AsPtr<CCompressedDataHeader>();

		pHeader->m_uCompressed = 1;
		pHeader->m_uDataLen = uint16(stChunkDataSize);

		Ast( m_bufCompressedData.Size() <= 0xffff );
		
		pHeader->m_uCompressedDataLen = uint16( m_bufCompressedData.Size()-sizeof( CCompressedDataHeader ) );
		
		if( m_bEncryptData )
			m_Encoder.EncodeBuffer( m_bufCompressedData.Data(),m_bufCompressedData.Size() );

		//printf("compress stChunkDataSize:%d stDataSize:%d m_sCompressedDataSize:%d\n"
		//	,int(stChunkDataSize),int(m_bufCompressedData.Size()-5),int(m_bufCompressedData.Size()));
	}

	IntPopChunkData( stChunkDataSize );
}



size_t CSyncPipeOutBuffer::OutBufferGetDataSize()const
{
	return (m_stOutBufferSegNum-1)*WRITE_BUFFER_CHUNK_SIZE+m_stOutTailDataEnd-m_stOutHeadDataBegin;
}


size_t CSyncPipeOutBuffer::OutBufferGetFreeSize()const
{
	return m_stOutBufferSize-OutBufferGetDataSize();
}

bool CSyncPipeOutBuffer::OutBufferIsFull()const
{
	return OutBufferGetFreeSize() == 0;
}


size_t CSyncPipeOutBuffer::OutBufferGetSize()const
{
	return m_stOutBufferSize;
}


size_t CSyncPipeOutBuffer::OutBufferPushData(const char* pData,size_t stDataSize)
{
	//获取写缓冲区的总剩余空间
	size_t stLeftSize=m_stOutBufferSize-OutBufferGetDataSize();
	size_t stReturnValue;

	if( stLeftSize < stDataSize )
	{
		stReturnValue=stLeftSize;
		stDataSize=stLeftSize;
	}
	else
	{
		stReturnValue=stDataSize;
	}

	//不压缩的时候，才在这个函数里面进行加密，否则要在压缩函数完成压缩之后进行加密
	const bool bEncryptDataHere = (!m_bCompressData) && m_bEncryptData;

	while(stDataSize>0)
	{
		size_t stCopySize;	//最后一个区块只能放下这么多的数据

		if(WRITE_BUFFER_CHUNK_SIZE<=m_stOutTailDataEnd)
		{
			m_lstOutBuffer.push_back((char*)SqrNew(WRITE_BUFFER_CHUNK_SIZE * sizeof(char), 
				CNetworkSendBufferMallocObject::GetPool()));
			//m_lstOutBuffer.push_back(new char[WRITE_BUFFER_CHUNK_SIZE]);
			++m_stOutBufferSegNum;

			m_stOutTailDataEnd=0;
			stCopySize=WRITE_BUFFER_CHUNK_SIZE;
		}
		else
		{
			stCopySize=WRITE_BUFFER_CHUNK_SIZE-m_stOutTailDataEnd;	//最后一个区块只能放下这么多的数据
		}

		if(stDataSize<stCopySize)
			stCopySize=stDataSize;

		char*const pWritePos=m_lstOutBuffer.back()+m_stOutTailDataEnd;
		memcpy(pWritePos,pData,stCopySize);
		if( bEncryptDataHere )
			m_Encoder.EncodeBuffer( pWritePos, stCopySize );

		pData+=stCopySize;
		stDataSize-=stCopySize;
		m_stOutTailDataEnd+=stCopySize;
	}
	return stReturnValue;
}


size_t CSyncPipeOutBuffer::IntGetChunkDataSize()
{
	if( m_stOutBufferSegNum > 1 )
		return WRITE_BUFFER_CHUNK_SIZE-m_stOutHeadDataBegin;
	return m_stOutTailDataEnd-m_stOutHeadDataBegin;
}


const char* CSyncPipeOutBuffer::IntGetChunkData()
{
	return m_lstOutBuffer.front()+m_stOutHeadDataBegin;
}


void CSyncPipeOutBuffer::IntPopChunkData(size_t stPopDataSize)
{
	//这里不做Underflow检查,如果程序写得没错就不会underflow,但是inbuffer的overflow则不同,
	//即使程序写得没错,机器太慢也会造成overflow,因为来不及处理网络数据

	while( (stPopDataSize>=WRITE_BUFFER_CHUNK_SIZE-m_stOutHeadDataBegin) && (m_stOutBufferSegNum>1))
	{
		SqrDelete(m_lstOutBuffer.front(), CNetworkSendBufferMallocObject::GetPool());
		m_lstOutBuffer.pop_front();
		--m_stOutBufferSegNum;
		stPopDataSize-=WRITE_BUFFER_CHUNK_SIZE-m_stOutHeadDataBegin;
		m_stOutHeadDataBegin=0;
	}
	
	m_stOutHeadDataBegin+=stPopDataSize;
	
	if(m_stOutBufferSegNum==1)
	{
		Ast(m_stOutHeadDataBegin<=m_stOutTailDataEnd);

		if(m_stOutHeadDataBegin==m_stOutTailDataEnd)
			m_stOutHeadDataBegin=m_stOutTailDataEnd=0;
	}
}



size_t CSyncPipeOutBuffer::OutBufferGetChunkDataSize()
{
	if( !m_bCompressData )
		return IntGetChunkDataSize();

	CompressDataIfNeeded();
	
	return m_bufCompressedData.Size();
}


const char* CSyncPipeOutBuffer::OutBufferGetChunkData()
{
	if( !m_bCompressData )
		return IntGetChunkData();

	CompressDataIfNeeded();
	
	return m_bufCompressedData.Data();
}


void CSyncPipeOutBuffer::OutBufferPopChunkData(size_t stPopDataSize)
{
	if( !m_bCompressData )
		return IntPopChunkData( stPopDataSize );

	Ast( stPopDataSize <= m_bufCompressedData.Size() );
	m_bufCompressedData.Erase( 0,stPopDataSize );

	//printf("PopChunkData m_bufCompressedData.Size():%d\n",int(m_bufCompressedData.Size()));
}


bool CSyncPipeOutBuffer::NoDataToSend()const
{
	return m_bufCompressedData.Empty() && m_stOutHeadDataBegin==0 && m_stOutTailDataEnd==0;
}

