#pragma once 

#include "ThreadTypes.h"
#include "NetworkDefs.h"
#include "IPipe.h"
#include "IPipeHandler.h"
#include "CAddress.h"
#include "CAsynPipeHandler.h"

class CPipeJob;
class CMulticastPipeJob;
class CPipeResult;
class CSyncPipe;
class CAsynPipe;

class CPipeThread;
class CAsynPipeReactor;
class CSyncPipeReactor;


class CPipeThreadMgr
{
	friend class CPipeJob;
	friend class CPipeResult;
	friend class CSyncPipe;
	friend class CAsynPipe;
	friend class CPipeThreadEndedResult;
	friend class CPipeSetThreadJob;
	friend class CPipeThread;
	friend class CAsynPipeHandler;
	friend class CPipeTrMsgBufferSwapper;
	
public:
	static void Init(uint32 num, CAsynPipeReactor* pReactor);
	static void Unit();

	static CPipeThreadMgr* Inst();

	inline CPipeThread* GetThread(size_t stIndex)const
	{
		return m_vecThread[stIndex];
	}

	inline size_t GetThreadSize()const
	{
		return m_vecThread.size();
	}

	//—∞’“ø’œ–œﬂ≥Ã
	uint32 GetIdleThreadID();

public:
	void Halt();
	bool Halted();

private:
	void CreatePipeThreads(uint32 num);
	void ReleaseThread();
	CPipeThreadMgr(uint32 num, CAsynPipeReactor* pReactor);
	~CPipeThreadMgr();

	void AddPipeResult(CPipeResult* pResult, CPipeThread* pThread);

	CSyncPipeReactor* GetSyncPipeReactor(uint32 uIndex)const;

	void ReactorCancelBlock();
	
	void OnHalted();

private:
	bool					m_bEndThreads;
	CAsynPipeReactor*		m_pAsynPipeReactor;

	vector<CPipeThread*>	m_vecThread;

	uint32					m_uExecThreadNum;

	static CPipeThreadMgr*	ms_pInst;
};

