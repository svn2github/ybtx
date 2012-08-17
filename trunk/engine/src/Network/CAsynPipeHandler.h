#pragma once 
#include "IPipeHandler.h"
#include "CNetworkMallocObject.h"
#include "TNetworkAllocator.h"


class CSyncPipe;

struct UnsendData
{
	char*	data;
	size_t	size;
};

class CAsynPipeHandler
	:public IPipeHandler
	,public CNetworkMallocObject
{
public:
	typedef set<IPipe*,less<IPipe*>,TNetworkAllocator<IPipe*> > OrphanPipeSet_t;
	typedef vector<OrphanPipeSet_t,TNetworkAllocator<OrphanPipeSet_t> > VecOrphanPipeSet_t;

	//ÒòÎª
	static VecOrphanPipeSet_t ms_vecOrphanPipeSet;
	
public:
	CAsynPipeHandler(int32 uPipeID,bool bFreqDSCallback);
	~CAsynPipeHandler();

public:
	void OnConnected();
	void OnConnectFailed(EPipeConnFailedReason eReason);
	void OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully);
	void OnListenFailed();
	void OnListenSuccessed();
	void OnAccepted(IPipe* pPipe);
	void OnDataReceived();
	void OnDataSent(uint32 uSentSize);
	void OnSendBufferFree();
	void IncreSession();
	int32 GetLocalID()const;
	void PushUnsendData(char*, size_t);
	void PopUnsendData();
	void SetPipe(CSyncPipe* pPipe);
	void SetAsynInbufFull(bool flag);
	void SetThreadId(uint32);
	uint32 GetThreadId()const;
	void OnThreadSet(int32 nListenPipeId, IPipe* pPipe, uint32 uOldThreadId);
	bool OutBufIsFull()const;
public:
	static CSyncPipe* GetPipeFromList(uint32 uThreadId, int32 nPipeId);
	
	static bool RemoveWaitedPipeFromList(uint32 uThreadId, IPipe* pPipe);

private:
	uint32 m_uSessionID;
	
	CSyncPipe* m_pPipe;
	
	int32  m_nLocalID;
	
	static int32 ms_nGlobalID;

	typedef list<UnsendData*,TNetworkAllocator<UnsendData*> >	UnsendDatList_t;
	UnsendDatList_t m_lstUnsendData;
	//size_t m_uSendSize;
	bool	m_bAsynInbufFull;
	bool	m_bSendBufferFull;
	
	uint32	m_uThreadId;
	bool	m_bFreqDSCallback;
};

