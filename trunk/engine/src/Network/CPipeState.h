#pragma once

#include "CAddress.h"
#include "CNetworkMallocObject.h"

#ifdef _WIN32
#include "CPipeEventObject.h"
#endif

class CSyncPipe;

class CPipeState
	:public CNetworkMallocObject
{
private:
	CSyncPipe*	m_pPipe;
protected:
	void ReportStateError(const char* szFunName,const char* szStateName)const;	
public:
	inline CSyncPipe*	GetPipe()const
	{
		return m_pPipe;
	}

	CPipeState(CSyncPipe* pPipe);
	virtual ~CPipeState(void);	
	
	//for CPipe to implement IPipe
	virtual bool Listen(const CAddress& Address)=0;
	virtual bool Connect(const CAddress& Address)=0;
	virtual void ShutDown(bool IfCallback)=0;
	virtual void Register()=0;
	virtual void Unregister()=0;
	virtual size_t Send(const void* pData,size_t stSize);
	virtual float GetDataFlowSpeed();
	virtual bool IsConnecting()const;

	virtual bool IsBusy()const;
};


