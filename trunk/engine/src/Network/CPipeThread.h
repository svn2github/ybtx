#pragma once 
#include "ThreadTypes.h"
#include "TTrThreadMsgBuffer.h"
#include "TNetworkAllocator.h"
#include <stack>

class CPipeThreadMgr;
class CPipeJob;
class CPipeResult;
class CSyncPipeReactor;
class CSyncPipe;
class CAsynPipeHandler;


class CPipeThreadTraits
{
public:
	typedef CPipeJob	LeftMsgType;
	typedef CPipeResult	RightMsgType;
};

class CPipeThread
	: public TTrThreadMsgBuffer<CPipeThreadTraits>
{
	typedef TTrThreadMsgBuffer<CPipeThreadTraits>	Parent_t;
	friend class TTrThreadMsgBuffer<CPipeThreadTraits>;

	friend class CPipeThreadMgr;
	friend class CAsynPipeHandler;
	friend class CPipeJob;

public:
	CPipeThread(CPipeThreadMgr* pMgr,CSyncPipeReactor* pReactor, uint32 uThreadId);
	~CPipeThread();

	void Quit();

	size_t	m_stPipeNum;


#ifdef _WIN32
	typedef hash_map<int32, CSyncPipe*, hash_compare<int32,less<int32> >,TNetworkAllocator<pair<int32,CSyncPipe*> > > MapSyncPipe;
#else
	typedef hash_map<int32, CSyncPipe*, hash<int32> ,equal_to<int32>,TNetworkAllocator<CSyncPipe*> > MapSyncPipe;
#endif

	MapSyncPipe	m_mapId2SyncPipe;

	void AddPipe(int32 nPipeId,CSyncPipe* pPipe);
	void DelPipe(int32 nPipeId);
	CSyncPipe* GetPipe(int32 nPipeId)const;

protected:
	void OnWakeRight();
	void OnLeftException(CError& error){}
	void OnRightException(CError& error){}

private:
	HTHREAD				m_hThread;
	CPipeThreadMgr*		m_pThreadMgr;
	CSyncPipeReactor*	m_pReactor;
	uint32				m_uThreadId;

	static void Execute(void* pParam);
	void DoExecute();

	bool			m_bQuit;
};

