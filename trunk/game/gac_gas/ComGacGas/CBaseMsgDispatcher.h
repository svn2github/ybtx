#pragma once
#include "TMsgDispatcher.h"
#include "CConnMsgMallocObject.h"

namespace sqr
{
class CLogOwner;
}

template < typename ImpClass, typename Conn >
class CBaseMsgDispatcher
	: public TBaseMsgDispatcher<ImpClass, uint16>
	, public CConnMsgMallocObject
{
public:
	void OnCaughtNetException();
	bool DispatchIsCanceled()const;
	void SetCurDispatchConn(Conn *pConn);
	void SendErrMsg(const char* szError);

	sqr::CLogOwner* GetLogOwner();
	Conn* GetConnection() const{ return m_pCurrentConn; }

protected:
	Conn *m_pCurrentConn;
};
