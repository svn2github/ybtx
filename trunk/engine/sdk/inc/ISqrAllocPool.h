#pragma once
#include "ModuleDefs.h"

namespace sqr
{
	class MODULE_API ISqrAllocPool		
	{
	public:
		virtual size_t GetMemUsage()const=0;
		virtual size_t GetCapacity()const=0;
		virtual const char* GetPoolName()const=0;
		virtual bool CheckLeakageEnabled()const=0;
		virtual void Trim()=0;
	protected:
		virtual ~ISqrAllocPool(){};
	};

}

