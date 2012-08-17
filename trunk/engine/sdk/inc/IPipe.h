#pragma once

#include "PipeTypes.h"
#include "CAddress.h"
#include "CDynamicObject.h"

namespace sqr
{
	class IPipeReactor;
	class IPipeHandler;
	class IMulticast;
	class IPipe
		:public virtual CDynamicObject
	{
	public:
		virtual void SetHandler(IPipeHandler* pHandler)=0;
		virtual IPipeHandler* GetHandler()const=0;

		virtual bool IsConnected()const=0;
		virtual bool IsConnecting()const=0;

		//Connection method
		virtual bool Listen(const CAddress& Address)=0;
		//调用Connect的时候会清空当前pipe的发送和接收缓冲区
		virtual bool Connect(const CAddress& Address)=0;
		virtual void ShutDown()=0;
		virtual void ShutDown(bool bNeedCallback)=0;

		//SendBuffer method
		virtual size_t Send(const void* pData,size_t stSize)=0;

		//RecvBuffer method
		virtual void* GetRecvData()const=0;
		virtual size_t GetRecvDataSize()const=0;
		virtual void PopRecvData(size_t stSize)=0;
		virtual void ClearRecvBuffer()=0;
		virtual size_t GetRecvBufferSize()const=0;
		virtual bool RecvBufferFull()const=0;

		//Address method
		virtual bool GetLocalAddress(CAddress& Address)const=0;
		virtual bool GetRemoteAddress(CAddress& Address)const=0;

		//Multicast method
		virtual bool DelFrMulticast(IMulticast*)=0;
		virtual bool AddToMulticast(IMulticast*)=0;

		virtual void DelFromAllMulticast(){};

		virtual void Release()=0;
	};
};

