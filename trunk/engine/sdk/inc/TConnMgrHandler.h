#pragma once
#include "CDynamicObject.h"

namespace sqr
{
	template<typename Traits>
	class TConnMgrHandler : public virtual CDynamicObject
	{
	private:
		typedef typename Traits::Conn_t	ImpConn_t;
	public:
		virtual void OnDisconnect(ImpConn_t* pConn) {};		//函数返回之前，pConn都是完全可用的，并且仍然处于Connected状态
		virtual void OnDataReceived(ImpConn_t* pConn) {};
		virtual void OnError(ImpConn_t* pConn,const char* szError) {};
		virtual ~TConnMgrHandler(){}
	};
}


