#pragma once 

#include "IPipe.h"
#include "IPipeHandler.h"
#include "CAsynPipeInBuffer.h"
#include "TNetworkAllocator.h"

namespace sqr
{
	class IdMaker;
}

enum CAsynPipeState
{
	eAPS_Busy = 1,	
	eAPS_Disconnected,
	eAPS_Connected,
	eAPS_Connecting,
	eAPS_Listening,
};

class CAsynPipeReactor;

class CAsynPipe
	: public IPipe
	, public CNetworkMallocObject
{
	static const uint8 MAX_STATISTIC_DATAFLOW_TIME;

public:

	CAsynPipeState		m_eState;

	IPipeHandler*		m_pHandler;

	CAsynPipeReactor*	m_pReactor;

	CAsynPipeInBuffer	m_InBuffer;

	size_t				m_stInBufferSize;
	size_t				m_stOutBufferSize;

public:
	static void InitStaticInfo();
	static void ReleaesIdMaker();
	static CAsynPipe* GetAsynPipe(uint32 id);
	static uint32 GetGlobalId();

private:
	static IdMaker* m_idMaker;
	typedef vector<CAsynPipe*,TNetworkAllocator<CAsynPipe*> > VecAsynPipe_t;
	static VecAsynPipe_t m_vecAsynPipe;
	const static uint32 M_SBLOCK;

	void IntSetHandler(IPipeHandler* pHandler);
	
public:
	CAsynPipe(IPipeHandler* pHandler,size_t stOutBufferSize,size_t stInBufferSize, int32 nPipeID=0);
	~CAsynPipe();

	void SetHandler(IPipeHandler* pHandler);
	IPipeHandler* GetHandler()const;

	IPipeReactor* GetReactor()const;

	bool IsConnected()const;
	bool IsListening()const;
	bool IsConnecting()const;

	bool Listen(const CAddress& Address);
	bool Connect(const CAddress& Address);
	void ShutDown();
	void ShutDown(bool);

	size_t Send(const void* pData,size_t stSize);
	size_t GetSendBufferSize()const;

	void* GetRecvData()const;
	size_t GetRecvDataSize()const;
	void PopRecvData(size_t stSize);
	void ClearRecvBuffer();
	size_t GetRecvBufferSize()const;
	bool RecvBufferFull()const;

	bool GetLocalAddress(CAddress& Address)const;
	bool GetRemoteAddress(CAddress& Address)const;

	void Release();

	void CopyDataFromSyncPipe(char* pData, uint32 uDataSize);

	int32 GetLocalID()const;
	inline uint32 GetSessionID()const { return m_uSessionID; }
	void IncreSessionID();
	
	void SetLocalAddr(const CAddress& LocalAddr);
	void SetRemoteAddr(const CAddress& RemoteAddr);

	//Multicast method
	bool DelFrMulticast(IMulticast*);
	bool AddToMulticast(IMulticast*);
	void DelFromAllMulticast();
	void SetSendBufferFull(bool bFlag);
	
	//thread info
	void SetThreadId(uint32);
	uint32 GetThreadId()const;
private:
	int32		m_uID;
	uint32		m_uSessionID;
	static		int32 m_uGlobalID;

	CAddress	m_LocalAddr;
	CAddress	m_RemoteAddr;
	
	bool		m_bSendBufferFull;
	uint32		m_uThreadId;
	
	typedef set<IMulticast*,less<IMulticast*>,TNetworkAllocator<IMulticast*> >	SetWithinMulticast_t;
	SetWithinMulticast_t  m_setWithinMulticast;	
};
