#pragma once
#include "CDynamicObject.h"

namespace sqr
{
	class IPipe;
	class IMulticast
	{
	public:
		virtual void Release()=0;
		virtual bool AddPipe(IPipe*)=0;
		virtual bool DelPipe(IPipe*)=0;
		virtual bool Empty()const=0;
		virtual void SendCmd(const void* pData,size_t stSize)=0;
		virtual size_t GetLocalID()const=0;
		virtual void SetLocalID(size_t uId)=0;
		virtual ~IMulticast(){}
	};
}
