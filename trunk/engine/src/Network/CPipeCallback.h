#pragma once 
#include "IPipeHandler.h"
#include "PatternMQ.h"

namespace sqr
{
	class CMemPool;
	class IPipe;
}

class CPipeCallback
	:public CPtMQMsg
{
public:
	CPipeCallback(CPtMQMsgOwner* pOwne);
	virtual ~CPipeCallback();
	virtual void Callback(IPipeHandler* pHandler)=0;
	
	//	void* operator new(size_t stSize,CMemPool* pPool);
	//	void operator delete(void* pMem,CMemPool* pPool);
	//	void operator delete(void* pMem){}
	void Dispatch(CPtMQMsgOwner* pOwner);

};

class COnNewDataReceivedCallback
	: public CPipeCallback
{
public:
	COnNewDataReceivedCallback(CPtMQMsgOwner*);
	void Callback(IPipeHandler* pHandler);
};

class COnSomeDataSentCallback
	: public CPipeCallback
{
public:
	COnSomeDataSentCallback(CPtMQMsgOwner*, uint32 uSentSize);
	void Callback(IPipeHandler* pHandler);
private:
	uint32	m_uSentSize;
};

class COnSendBufferFreeCallback
	: public CPipeCallback
{
public:
	COnSendBufferFreeCallback(CPtMQMsgOwner*);
	void Callback(IPipeHandler* pHandler);
};

class COnConnectedCallback
	: public CPipeCallback
{
public:
	COnConnectedCallback(CPtMQMsgOwner*);
	void Callback(IPipeHandler* pHandler);
};

class COnDisconnectedCallback
	: public CPipeCallback
{
private:
	EPipeDisconnectReason	m_eReason;
	bool					m_bGracefully;
public:
	COnDisconnectedCallback(CPtMQMsgOwner* pOwne,EPipeDisconnectReason eReason, bool bGracefully);
	void Callback(IPipeHandler* pHandler);
};

class COnConnectFailedCallback
	: public CPipeCallback
{
private:
	EPipeConnFailedReason	m_eReason;
public:
	COnConnectFailedCallback(CPtMQMsgOwner* pOwne, EPipeConnFailedReason eReason);
	void Callback(IPipeHandler* pHandler);
};

class CSyncPipeReactor;

class COnAcceptedCallback
	: public CPipeCallback
{
private:
	IPipe*				m_pNewPipe;
public:
	COnAcceptedCallback(CPtMQMsgOwner* pOwne,IPipe* pNewPipe);
	void Callback(IPipeHandler* pHandler);
};

class COnListenFailedCallback
	: public CPipeCallback
{
public:
	COnListenFailedCallback(CPtMQMsgOwner*);
	void Callback(IPipeHandler* pHandler);
};

class COnListenSuccessedCallback
	: public CPipeCallback
{
public:
	COnListenSuccessedCallback(CPtMQMsgOwner*);
	void Callback(IPipeHandler* pHandler);
};
