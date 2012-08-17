#pragma once

#include "CNpcAIMallocObject.h"

class CNpcEventMsg;

class INpcEventHandler
	: public CNpcAIMallocObject 
{
	public:
		virtual ~INpcEventHandler() { }
		virtual void Handle(CNpcEventMsg* pEvent) = 0; 
};

