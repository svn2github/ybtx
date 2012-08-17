#pragma once

namespace sqr
{
	class CLogicThreadJob
	{
	public:
		void* operator new(size_t stSize);
		void operator delete(void* pMem){}

		void* AllocMem(size_t stSize);
		char* CloneString(const char* szStr);
		void* CloneData(const void* pData, size_t stSize);

		void Add();

		virtual ~CLogicThreadJob(){}
	};
}
