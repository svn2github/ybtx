#pragma once
#include "ModuleDefs.h"
#include "ThreadTypes.h"

namespace sqr
{
	class MODULE_API CLock
	{
	private:
		HLOCK	m_Lock;
	public:
		HLOCK* GetLock();
		void Lock();
		void Unlock();
		CLock();
		~CLock();
	};
}

