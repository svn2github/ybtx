#pragma once

#include "IPipe.h"
#include "CSyncPipeInBuffer.h"
#include "CSyncPipeOutBuffer.h"
#include "IPipeHandler.h"
#include "CNetworkMallocObject.h"
#include "TNetworkAllocator.h"
#include "PatternMQ.h"

class CPipeState;
class CSyncPipeReactor;
class CPipeCallback;

class CMulticast;

class CSyncPipe
	:public IPipe
	,public CPtMQMsgOwner
	,public CNetworkMallocObject
{
	friend class CSyncPipeReactor;
	friend class CPipeState;
	static const uint8 MAX_STATISTIC_DATAFLOW_TIME;
public:
	CSyncPipe(IPipeHandler* pHandler,size_t stOutBufferSize,size_t stInBufferSize, uint8 uFlag);
	CSyncPipe(IPipeHandler* pHandler,size_t stOutBufferSize,size_t stInBufferSize,const SOCKET& Socket, uint8 uFlag, bool& bDisconnected );
	~CSyncPipe();

	//below is inline function 
	bool IsSyncCallback()const;
	void AddEventNewDataReceived();
	void AddEventSomeDataSent(uint32 uSentData);
	void AddEventConnected();
	void AddEventListenFailed();
	void AddEventListenSuccess();
	void AddEventDisconnected(EPipeDisconnectReason eReason,bool bGracefully);
	void AddEventConnectFailed(EPipeConnFailedReason eReason);
	void AddEventAccepted(CSyncPipe* pNewPipe);
	void AddEventSendBufferFree();
	//above is inline function

	void SetHandler(IPipeHandler* pHandler);
	IPipeHandler* GetHandler()const;

	IPipeReactor* GetReactor()const;
	bool IsConnected()const;
	bool IsConnecting()const;

	bool Listen(const CAddress& Address);
	bool Connect(const CAddress& Address);
	void ShutDown();
	void ShutDown(bool);

	size_t Send(const void* pData,size_t stSize);
		
	size_t GetSendBufferSize()const;
	size_t GetSendBufferFreeSize()const;
	size_t SendBufferResize(size_t stSize);

	void* GetRecvData()const;
	size_t GetRecvDataSize()const;
	void PopRecvData(size_t stSize);
	void ClearRecvBuffer();
	size_t GetRecvBufferSize()const;
	bool RecvBufferFull()const;
	
	bool GetLocalAddress(CAddress& Address)const;
	bool GetRemoteAddress(CAddress& Address)const;

	void Release();

	bool AddToMulticast(IMulticast* pMulticast);
	bool DelFrMulticast(IMulticast* pMulticast);
	void DelFromAllMulticast();
	
	bool NeedSomeDataSentCallback()const;

	void WriteHistory( const string& sHistory );
	void PrintAndClearHistory( ostream& os );

	float GetDataFlowSpeed();

public:
	CPipeState*			m_pState;

	IPipeHandler*		m_pHandler;

	CSyncPipeReactor*		m_pReactor;

	CSyncPipeInBuffer	m_InBuffer;
	CSyncPipeOutBuffer	m_OutBuffer;

	uint8			m_uCreationFlag;

private:
	void IntSetHandler( IPipeHandler* pHandler );

private:	
	typedef set<CMulticast*,less<CMulticast*>,TNetworkAllocator<CMulticast*> > SetWithinMulticast_t;
	SetWithinMulticast_t m_setWithinMulticast;

	queue<string>	m_quePipeStateHistory;
};

