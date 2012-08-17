#pragma once 

#include "IPipeReactor.h"
#include "CAsynPipe.h"
#include "CNetworkMallocObject.h"

class CAsynPipeReactor
	: public sqr::IPipeReactor
	, public CNetworkMallocObject
{
	friend class CPipeThreadMgr;
public:
	CAsynPipeReactor();
	~CAsynPipeReactor();

	IPipe* CreateIPipe(IPipeHandler* pHandler,unsigned int uSendBufferSize,unsigned int uRecvBufferSize, uint8 uFlag);
	static void SetThreadNum(uint32);
#ifndef _WIN32
	int					m_fdEpoll;
	int					m_fdReadPipe;
	int					m_fdWritePipe;
#else
	typedef vector<WSAEVENT,TNetworkAllocator<WSAEVENT> >		VecEvent_t;
	VecEvent_t	m_vecEvent;

	HANDLE				m_evCancelBlock;
	typedef vector<HANDLE,TNetworkAllocator<HANDLE> >		VecThreadHandle_t;
	VecThreadHandle_t	m_vecThreadMsgHandler;
#endif

	static uint32 m_suThreadNum;
	uint32	m_uPipeNum;

	size_t GetPipeNum()const;

	void Register(IPipe* pPipe);
	void Unregister(IPipe* pPipe);

	uint32 GetEvent(unsigned uWaitTime);


	size_t GetPipes(IPipe**ppPipe,size_t stPipeNum);

	bool Release();

	//void IntCancelBlock();

	void PushPipeThreadMsg(uint32 uThreadId);
	void DestroyMulticast(IMulticast*);
	IMulticast* CreateMulticast();

private:
	void CancelBlock();
//	EGetEventResult InternalCheck(unsigned uWaitTime);
//	void Poll(unsigned uWaitTime);

//	bool m_bHaveNetMsg:1;
//	bool m_bUserCanceled;
};

