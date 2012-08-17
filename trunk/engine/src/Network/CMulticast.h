#pragma once 
#include "IMulticast.h"
#include "CNetworkMallocObject.h"
#include "TNetworkAllocator.h"

class CSyncPipe;

class CMulticast
	:public IMulticast
	,public CNetworkMallocObject
{
public:
	virtual ~CMulticast();
	bool AddPipe(IPipe*);
	bool DelPipe(IPipe*);
	void SetLocalID(size_t uLocalId);
	size_t GetLocalID()const{ return m_uLocalId; }
	void Release();
	void SendCmd(const void* pData,size_t stSize);
	bool Empty()const;

private:
	size_t			m_uLocalId;
	
	typedef map<CSyncPipe*,size_t,less<CSyncPipe*>,TNetworkAllocator<pair<CSyncPipe*,size_t> > > PipeMap_t;
	PipeMap_t	m_mapPipe;

	typedef deque< pair<CSyncPipe*,size_t*>,TNetworkAllocator< pair<CSyncPipe*,size_t*> > > PipeDeq_t;
	PipeDeq_t	m_deqPipe;
};

