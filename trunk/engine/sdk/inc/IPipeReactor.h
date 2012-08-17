#pragma once

/*

·同时请求多个连接
·连接请求可以在不同的Reactor之间转移
·连接请求可以中断
·需要收到连接成功和失败的通知

·Reactor之间可以传递Pipe
·Disconnect的触发和PipeReadBuffer的清空不是同一件事情
*/
#include "PipeTypes.h"
#include "IPipe.h"

namespace sqr
{
	class IPipeReactor
		:public virtual CDynamicObject
	{
	public:
		virtual IMulticast* CreateMulticast()=0;

		virtual void DestroyMulticast(IMulticast*)=0;

		virtual IPipe* CreateIPipe(IPipeHandler* pHandler,
			unsigned int uSendBufferSize,
			unsigned int uRecvBufferSize,
			uint8 uFlag)=0;

		virtual size_t GetPipeNum()const=0;

		virtual void CancelBlock()=0;

		virtual uint32 GetEvent(unsigned uWaitTime=0) = 0;

		virtual bool Release()=0;

		virtual ~IPipeReactor(){}
	};

}

