#include "stdafx.h"
#include "CMsg.h"

CMsg::CMsg( const MSG_SOURCE& fSource,const MSG_TYPE& fType,const MSG_INFO& fInfo )
: msgSource(fSource),msgType(fType),msgInfo(fInfo){}

