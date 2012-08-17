#pragma once
#include "CBaseMsgDispatcher.h"
#include "CConnServer.h"

namespace sqr
{
	class CConnServer;
}

template < class ImpClass >
class CServerMsgDispatcher
	: public CBaseMsgDispatcher<ImpClass, CConnServer>
{
public:

};
