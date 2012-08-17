#pragma once
#include "CPipeState_Busy.h"
#include "TNetworkAllocator.h"
#include "CDataFlowCounter.h"
#include "IPipeHandler.h"

class CPipeState_Connected
	:public CPipeState_Busy
{
public:
	CPipeState_Connected(CSyncPipe* pPipe,const SOCKET& Socket,bool& bDisconnected);
	~CPipeState_Connected();

#ifdef _WIN32
	bool OverlappedRecv();
	bool OverlappedSend();
#else
	void EpollEventRegister();
#endif

	bool SendShakeHandMsg();

	void ProcessEvent(bool bRead,bool bWrite,bool bError);

	bool Listen(const CAddress& Address);
	bool Connect(const CAddress& Address);
	void ShutDown(bool IfCallback);
	void Register();
	void Unregister();
	size_t Send(const void* pData,size_t stSize);

	CDataFlowCounter	m_DataFlowCounter;

private:
	inline bool OnRecvCompletion(uint32 uError,uint32 uRecvSize);
	inline bool OnSendCompletion(uint32 uError,uint32 uRecvSize);

	float GetDataFlowSpeed();
	
#ifdef _WIN32
	WSAOVERLAPPED*	m_pOverlappedSend;
	WSAOVERLAPPED*	m_pOverlappedRecv;
	char*			m_pLastOverlappedRecvWritePos;
	
	static void CALLBACK RecvCompletionRoutine(uint32 uError,uint32 uRecvSize,WSAOVERLAPPED* pOverlapped,uint32 uFlags);
	static void CALLBACK SendCompletionRoutine(uint32 uError,uint32 uRecvSize,WSAOVERLAPPED* pOverlapped,uint32 uFlags);
#endif

	void AddDisconnectEventAndChangeState(EPipeDisconnectReason,bool bGracefully);

	bool SendWayEncryption()const;
	bool RecvWayDecryption()const;
	bool SendWayCompression()const;
	bool RecvWayDecompression()const;	
};

