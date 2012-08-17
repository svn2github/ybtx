#pragma once
#include "TConnectionMgr.h"
#include "BaseHelper.h"

template<typename Traits>
TConnectionMgr<Traits>::TConnectionMgr()
:m_pHandler(NULL)
{
	ImpConn_t::InitMsgHandlerContainer();
	ImpConn_t::RegisterMsgHandler();
}

template<typename Traits>
TConnectionMgr<Traits>::~TConnectionMgr()
{
	SetHandler(NULL);

	typename ListConn_t::iterator iter = m_listAllConn.begin();
	for(;iter!=m_listAllConn.end();iter++)
		delete *iter;

	ImpConn_t::UninitMsgHandlerContainer();
}


template<typename Traits>
void TConnectionMgr<Traits>::SetHandler(ImpConnMgrHandler_t* pHandler)
{
	AdvDelRef(m_pHandler);
	m_pHandler=pHandler;
	AdvAddRef(m_pHandler);
}

template<typename Traits>
typename TConnectionMgr<Traits>::ImpConnMgrHandler_t*
TConnectionMgr<Traits>::GetHandler()const
{
	return m_pHandler;
}

template<typename Traits>
void TConnectionMgr<Traits>::DoShutDownConn(ImpConn_t* pConn)
{
	pConn->UnRegDoShutdownTick();

	pConn->m_pPipe->ShutDown();
	pConn->m_pPipe->Release();
	pConn->m_pPipe=NULL;

	m_listAllConn.erase( pConn->m_itListConn );
	delete pConn;
}

template<typename Traits>
void TConnectionMgr<Traits>::ShutDownAllConn()
{
	// shutdownËùÓÐµÄconnection
	typename ListConn_t::iterator iter = m_listAllConn.begin();
	while( !m_listAllConn.empty() )
	{
		ImpConn_t* pConn = m_listAllConn.front();
		pConn->DoShutDownAndCallback();
	}
}



template<typename Traits>
typename Traits::ConnMgr_t*& TConnectionMgr<Traits>::IntInst()
{
	static ImpConnMgr_t* ls_Inst=NULL;
	return ls_Inst;
}


template<typename Traits>
typename Traits::ConnMgr_t* TConnectionMgr<Traits>::Inst()
{
	return IntInst();
}

