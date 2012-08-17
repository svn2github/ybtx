#pragma once 
#include "IPipeHandler.h"
#include "CAsynPipe.h"

class CPipeThread;

class CPipeResult
{
public:
	void Init(int32 uPipeID, uint32 uSessionID);
	int32 m_uPipeId;
	uint32 m_uSessionID;

	CPipeResult(int32 uPipeID, uint32 uSessionID);

	inline void Add(CPipeThread* pThread);
	virtual ~CPipeResult(){}

	inline void* operator new(size_t stSize, CPipeThread* pPool);
	void operator delete(void* pMem, CPipeThread* pPool){}
	void operator delete(void* pMem){}

	inline void* AllocMem(size_t stSize, CPipeThread *pThread);
	inline char* CloneString(const char* szStr, CPipeThread *pThread);
	inline void* CloneData(const void* pData, size_t stSize, CPipeThread *pThread);

};

class CPipeNewDataReceivedResult: public CPipeResult
{
public:
	CPipeNewDataReceivedResult(int32 nPipeId,uint32 uSessionID,const char* pData, uint32 uDataSize, CPipeThread* pThread);
	~CPipeNewDataReceivedResult();
private:
	void*	m_pData;
	uint32	m_uRecvDataSize;
};

class CPipeConnectSuccResult: public CPipeResult
{
public:
	CPipeConnectSuccResult(int32 nPipeId, uint32 uSessionID, const CAddress& LocalAddr, const CAddress& RemoteAddr, CPipeThread* pThread);
	~CPipeConnectSuccResult();

private:
	char*		m_szLocalAddr;
	uint32		m_uLocalPort;
	char*		m_szRemoteAddr;
	uint32		m_uRemotePort;
};

class CPipeDisconnectResult: public CPipeResult
{
public:
	CPipeDisconnectResult(int32 nPipeId, uint32 uSessionID, 
		EPipeDisconnectReason eReason, bool bGracefully, CPipeThread* pThread);
	~CPipeDisconnectResult();
	
private:
	EPipeDisconnectReason m_eReason;
	bool m_bGracefully;
};

class CPipeConnectFailResult: public CPipeResult
{
public:
	CPipeConnectFailResult(int32 nPipeId, uint32 uSessionID, EPipeConnFailedReason eReason);
	~CPipeConnectFailResult();

private:
	EPipeConnFailedReason m_eReason;
};

class CPipeAcceptedResult: public CPipeResult
{
public:
	CPipeAcceptedResult(int32 nPipeId,uint32 uSessionID, int32 newPipeId, uint32 threadId, 
			uint32 oldthreadid, IPipe* pPipe, const CAddress& LocalAddr, const CAddress& RemoteAddr, CPipeThread* pThread);
	~CPipeAcceptedResult();

private:
	int32 m_nNewPipeID;
	char*		m_szLocalAddr;
	uint32		m_uLocalPort;
	char*		m_szRemoteAddr;
	uint32		m_uRemotePort;
	uint32		m_uThreadId;
	IPipe*		m_pPipe;
	uint32		m_uOldThreadId;
};


class CPipeSomeDateSentResult: public CPipeResult
{
public:
	CPipeSomeDateSentResult(int32 nPipeId, uint32 uSessionID, uint32 uSentSize);
	~CPipeSomeDateSentResult();

private:
	uint32	m_uSentSize;
};

class CSendBufferFullResult: public CPipeResult
{
public:
	CSendBufferFullResult(int32 nPipeId, uint32 uSessionID, bool bFlag);
	~CSendBufferFullResult();

private:
	bool m_bFlag;
};

class CPipeListenFailedResult: public CPipeResult
{
public:
	CPipeListenFailedResult(int32 nPipeId, uint32 uSessionID);
	~CPipeListenFailedResult();
};

class CPipeListenSuccessedResult: public CPipeResult
{
public:
	CPipeListenSuccessedResult(int32 nPipeId, uint32 uSessionID);
	~CPipeListenSuccessedResult();
};

class CPipeSetThreadResult: public CPipeResult
{
public:
	CPipeSetThreadResult(int32 nPipeId, uint32 uSessionId, IPipe* pPipe, uint32 uOldThreadID,bool bFreqDSCallback);
	~CPipeSetThreadResult();
	
private:
	IPipe*	m_pPipe;
	uint32	m_uOldThreadId;
	bool	m_bFreqDSCallback;
};


class CPipeThreadEndedResult : public CPipeResult
{
public:
	CPipeThreadEndedResult();
	~CPipeThreadEndedResult();
};
