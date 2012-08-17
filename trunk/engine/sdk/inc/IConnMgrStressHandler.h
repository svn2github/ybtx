#pragma once
#include "TConnMgrHandler.h"
#include "CTraitsStress.h"

namespace sqr
{
class CConnStress;

class IConnMgrStressHandler
			:public TConnMgrHandler<CTraitsStress>
{
public:
	virtual void OnConnected(CConnStress* pConn) {};
	virtual void OnConnectFailed(CConnStress* pConn) {};
};
}
