#pragma once
#include "IPipeReactor.h"
#include "CSyncPipe.h"
#include "BaseHelper.h"
#include "TNetworkAllocator.h"

class CSyncPipe;
class CStateWithEvent;
class CPipeState_Busy;

namespace sqr
{
	class CPtMQMsgMgr;
}

class CSyncPipeReactor
	:public IPipeReactor
	,public CNetworkMallocObject
{
public:

	CSyncPipeReactor( bool bSyncCallback=false );
	~CSyncPipeReactor(void);

	IPipe* CreateIPipe(IPipeHandler* pHandler,unsigned int uSendBufferSize,unsigned int uRecvBufferSize, uint8 uFlag);

#ifndef _WIN32
	//return true表示过程中发生了异常
	bool ClearControlPipeData();

	int					m_fdEpoll;
	int					m_fdReadPipe;
	int					m_fdWritePipe;
#else
	
	typedef vector<CStateWithEvent*,TNetworkAllocator<CStateWithEvent*> >	 EventStateVec_t;
	EventStateVec_t		m_vecState;
	
	EventStateVec_t		m_vecStateTemp;

	typedef vector<WSAEVENT,TNetworkAllocator<WSAEVENT*> >				EventVec_t;
	EventVec_t			m_vecEvent;

	HANDLE					m_evCancelBlock;
	CSyncPipe*				m_pCallingBackPipe;
#endif
	uint32	m_uPipeNum;

	size_t GetPipeNum()const;

	void Register(IPipe* pPipe);
	void Unregister(IPipe* pPipe);

	uint32 GetEvent(unsigned uWaitTime);


	size_t GetPipes(IPipe**ppPipe,size_t stPipeNum);

	bool Release();

	void DestroyMulticast(IMulticast*);
	IMulticast* CreateMulticast();

	void CancelBlock();

//	void Poll(unsigned uWaitTime);
	bool DispatchEvent();

	typedef vector<char, TNetworkAllocator<char> > VecEncodeBuf_t;
	VecEncodeBuf_t	m_EncodeBuffer;
	CPtMQMsgMgr*	m_pMQMsgMgr;
	bool			m_bHasEvent;

	uint64			m_uLastCheckTime;
};
