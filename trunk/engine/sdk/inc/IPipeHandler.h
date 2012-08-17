#pragma once
#include "CDynamicObject.h"

namespace sqr
{
	enum EPipeDisconnectReason
	{
		ePDR_REMOTE,
		ePDR_ERROR,
		ePDR_USER
	};

	enum EPipeConnFailedReason
	{
		ePCFR_UNREACHABLE,
		ePCFR_REFUSED,
		ePCFR_INVALIDADDR,
		ePCFR_TIMEDOUT,
		ePCFR_OSERROR,
		ePCFR_CANCELED
	};

	class IPipe;

	class IPipeHandler
		: public virtual CDynamicObject
	{
	public:
		virtual void OnConnected();
		virtual void OnListenFailed();
		virtual void OnListenSuccessed();
		virtual void OnConnectFailed(EPipeConnFailedReason eReason);
		virtual void OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully);
		virtual void OnAccepted(IPipe* pPipe);
		virtual void OnDataReceived();
		virtual void OnDataSent(uint32 uSentSize);
		virtual void OnSendBufferFree();
	protected:
		virtual ~IPipeHandler(){}
	};
}


