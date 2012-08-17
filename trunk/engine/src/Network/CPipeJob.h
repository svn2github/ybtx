#pragma once 
#include "IPipeReactor.h"


class CPipeThread;

class CPipeJob
{
public:
	CPipeJob(CPipeThread* pThread);
	CPipeJob(int32 nPipeId, CPipeThread* pThread);

	inline void Add(CPipeThread* pThread);
	virtual ~CPipeJob(){}

	inline void* operator new(size_t stSize,CPipeThread* pThread);
	inline void operator delete(void* pMem,CPipeThread* pThread){}
	inline void operator delete(void* pMem){}

	inline void* AllocMem(size_t stSize, CPipeThread *pThread);
	inline char* CloneString(const char* szStr, CPipeThread *pThread);
	inline void* CloneData(const void* pData, size_t stSize, CPipeThread *pThread);


protected:
	int32	m_nPipeId;
	uint32	m_uThreadId;
	IPipeReactor* m_pReactor;
};

class CPipeSendDataJob: public CPipeJob
{
public:
	CPipeSendDataJob(int32 nPipeId, const char* pData, uint32 size, CPipeThread* pThread);
	~CPipeSendDataJob();
private:
	char*	m_pSendData;
	uint32	m_uSize;
};

class CPipeConnectJob: public CPipeJob
{
public:
	CPipeConnectJob(int32 nPipeId, const CAddress& address, CPipeThread* pThread);
	~CPipeConnectJob();
private:
	char*		m_szAddress;
	uint32		m_uPort;
};

class CPipeListenJob: public CPipeJob
{
public:
	CPipeListenJob(int32 nPipeId, const CAddress& address, CPipeThread* pThread);
	~CPipeListenJob();
private:
	char*		m_szAddress;
	uint32		m_uPort;
};

class CPipeShutdownJob: public CPipeJob
{
public:
	CPipeShutdownJob(int32 nPipeId, bool bNeedCallback, CPipeThread* pThread);
	~CPipeShutdownJob();
private:
	bool m_bNeedCallback;
};

class CPipeCreateJob: public CPipeJob
{
private:
	uint8	m_uCreationFlag;
	uint32	m_uSendBufferSize;
	uint32	m_uRecvBufferSize;
public:
	CPipeCreateJob(int32 nPipeId, uint8 uEncodingFlag, CPipeThread* pThread,uint32 uSendBufferSize,uint32 uRecvBufferSize);
	~CPipeCreateJob();
};

class CPipeReleaseJob: public CPipeJob
{
public:
	CPipeReleaseJob(int32 nPipeId, CPipeThread* pThread);
	~CPipeReleaseJob();
};

class CPipeSetThreadJob: public CPipeJob
{
public:
	CPipeSetThreadJob(uint32 uOldThreadId, int32 nListenPipeId, IPipe* pPipe, CPipeThread* pThread,bool bFreqDSCallback);
	~CPipeSetThreadJob();
private:
	IPipe*	m_pPipe;
	uint32	m_uOldThreadId;
	uint32	m_uLocalId;
	bool	m_bFreqDSCallback;
};

class CPipeInbufFullJob: public CPipeJob
{
public:
	CPipeInbufFullJob(int32 nPipeId, bool flag, CPipeThread* pThread);
	~CPipeInbufFullJob();
private:
	bool m_bFull;
};

class CRemoveWaitedPipeFromListJob: public CPipeJob
{
public:
	CRemoveWaitedPipeFromListJob(IPipe* pPipe, CPipeThread* pThread);
	~CRemoveWaitedPipeFromListJob();
private:
	IPipe* m_pPipe;
};

class CMulticastAddPipe: public CPipeJob
{
public:
	CMulticastAddPipe(size_t uMulticastID, int nPipeID, CPipeThread* pThread);
	~CMulticastAddPipe();
private:
	size_t			m_uMulticastID;
};


class CMulticastDelPipe: public CPipeJob
{
public:
	CMulticastDelPipe(size_t uMulticastID, int nPipeID, CPipeThread* pThread);
	~CMulticastDelPipe();
private:
	size_t			m_uMulticastID;
};

class CPipeDelFromAllMulticastJob: public CPipeJob
{
public:
	CPipeDelFromAllMulticastJob(int32 nPipeId, CPipeThread* pThread);
	~CPipeDelFromAllMulticastJob();
};


class CMulticastMsgJob: public CPipeJob
{
public:
	CMulticastMsgJob(size_t uMulticastID, const void* pData,size_t stSize, CPipeThread* pThread);
	~CMulticastMsgJob();
private:
	size_t			m_uMulticastID;
	char*			m_pData;
	size_t			m_stDataSize;
};

class CAddMulticastJob: public CPipeJob
{
public:
	CAddMulticastJob(size_t uMulticastID, CPipeThread* pThread);
	~CAddMulticastJob();
private:
	size_t			m_uMulticastID;
};

class CDelMulticastJob: public CPipeJob
{
public:
	CDelMulticastJob(size_t uMulticastID, CPipeThread* pThread);
	~CDelMulticastJob();
private:
	size_t			m_uMulticastID;
};

class CPipeEndThreadJob : public CPipeJob
{
public:
	CPipeEndThreadJob(CPipeThread* pThread);
	~CPipeEndThreadJob();
private:
	CPipeThread*	m_pThread;
};

