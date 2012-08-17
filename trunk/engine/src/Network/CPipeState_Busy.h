#pragma once
#include "CPipeState.h"


class CPipeState_Busy
	:public CPipeState	
{
public:
	SOCKET	m_Socket;

	CPipeState_Busy(CSyncPipe* pPipe);
	CPipeState_Busy(CSyncPipe* pPipe,const SOCKET& Socket);
	~CPipeState_Busy();

#ifndef _WIN32
	void EpollCtl(int32 nOp,uint32 uEvent)const;
#endif

	virtual bool IsBusy()const;
	virtual void ProcessEvent(bool bRead,bool bWrite,bool bError)=0;

	static SOCKET CreateSocket();
};


#ifdef _WIN32
class CStateWithEvent
{
private:
	uint32		m_uIndex;

protected:
	CPipeEventObject	m_Event;

	inline void EventSelect(uint32 uEvent);
	void EventAdd(uint32 uEvent);
	void EventDel();

protected:
	uint32		m_uIndexOfTempVec;
	void AddEventFromTemp(void);

public:
	CStateWithEvent();
	virtual CPipeState* GetPipeState()const=0;
};
#endif


