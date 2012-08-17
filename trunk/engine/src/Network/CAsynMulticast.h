#pragma once 
#include "IMulticast.h"
#include "IPipe.h"
#include "CNetworkMallocObject.h"
#include "TNetworkAllocator.h"

namespace sqr
{
	class IdMaker;
}

class CAsynMulticast
	: public IMulticast
	, public CNetworkMallocObject
{
public:
	typedef vector<CAsynMulticast*, TNetworkAllocator<CAsynMulticast*> > VecAsynMulticast_t;
	static VecAsynMulticast_t m_vecId2AsynMulticast;

#ifdef _WIN32
	typedef hash_map<size_t, IMulticast*, hash_compare<size_t, less<size_t> >, 
		TNetworkAllocator<pair<size_t, IMulticast*> > > MapSyncMulticast;
#else
	typedef hash_map<size_t, IMulticast*, hash<size_t>, equal_to<size_t>, 
		TNetworkAllocator<pair<size_t, IMulticast*> > > MapSyncMulticast;
#endif

	typedef vector<MapSyncMulticast, 
		TNetworkAllocator<MapSyncMulticast> > VecMulticast;

	static VecMulticast m_vecId2Multicast;

private:
	typedef set<IPipe*, less<IPipe*>, TNetworkAllocator<IPipe*> >	SetPipe_t;
	SetPipe_t		m_setPipeWithin;

	typedef vector<uint32, TNetworkAllocator<uint32> >	VecThreadInfo_t;
	VecThreadInfo_t		m_vecThreadInfo;

	uint32 m_uLocalID;
	static IdMaker* m_idMaker;
	const static uint32 m_uResizeUint;

public:
	CAsynMulticast();
	virtual ~CAsynMulticast();
	bool AddPipe(IPipe*);
	bool DelPipe(IPipe*);
	void Release();
	void SendCmd(const void* pData,size_t stSize);
	void IncrePipeNum(uint32 uThreadId);
	void DecrePipeNum(uint32 uThreadId);
	size_t GetLocalID()const;
	void SetLocalID(size_t uId){}
	void PushPipeEvent(IPipe*);
	bool Empty()const;

	void static InitStaticInfo(uint32 uThreadNum);
	void static ReleaseIdMaker();

protected:
	void ClearEventList();
};
