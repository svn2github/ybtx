#pragma once
#include "TMsgDispatcher.h"
#include "CBaseMsgDispatcher.h"
#include "CConnStress.h"
#include "TComMsgDispatcher.h"

namespace sqr
{
	class CConnStress;
}

template < typename ImpClass, typename Traits >
class CStressMsgDispatcher
	: public TComMsgDispatcher<ImpClass,Traits,CConnStress>
{
	friend class TComMsgDispatcher<ImpClass,Traits, CConnStress>;
};
