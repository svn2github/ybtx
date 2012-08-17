#pragma once

#include "CNetworkMallocObject.h"
#include "TNetworkAllocator.h"
#include "CMemData.h"
#include "CByteEncoder.h"


class CSyncPipeOutBuffer
	:public CNetworkMallocObject
{
public:
	explicit CSyncPipeOutBuffer(size_t stOutBufferSize,bool bCompressData,bool bEncryptData);
	~CSyncPipeOutBuffer(void);

	size_t OutBufferPushData(const char* pData,size_t stDataSize);

	size_t OutBufferGetChunkDataSize();

	const char* OutBufferGetChunkData();

	void OutBufferPopChunkData(size_t stPopDataSize);

	bool OutBufferIsFull()const;
	size_t OutBufferGetSize()const;
	size_t OutBufferGetDataSize()const;
	size_t OutBufferGetFreeSize()const;

	void OutBufferClear();

	bool NoDataToSend()const;

#ifdef _WIN32
	void PrepareSendData(WSABUF*& pBuffer,DWORD& dwBufferCount);
#endif

	CByteEncoder m_Encoder;

private:

	size_t IntGetChunkDataSize();

	const char* IntGetChunkData();

	void IntPopChunkData(size_t stPopDataSize);	
private:	
	void CompressDataIfNeeded();
	
	
	size_t		m_stOutBufferSegNum;
	size_t		m_stOutBufferSize;

	typedef TNetworkSendBufAllocator<char*>	CharAlloc_t;
	typedef list<char*,CharAlloc_t >	BufferList_t;
	BufferList_t	m_lstOutBuffer;	

	size_t	m_stOutHeadDataBegin;	//输出缓冲区的第一个区块的数据起始的位置
	size_t	m_stOutTailDataEnd;		//输出缓冲区的最后一个区块的数据结尾的位置

	CMemData m_bufCompressedData;
	
	const bool	m_bCompressData;
	const bool	m_bEncryptData;

	void Clear();

};
