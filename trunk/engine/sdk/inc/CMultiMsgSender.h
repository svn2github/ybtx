#pragma once

#include "ISend.h"
#include "TIDPtrMap.h"
#include "CConnMallocObject.h"

namespace sqr
{
	class CConnServer;
	class IMulticast;

	class CMultiMsgSender
		:public ISend
		,public CConnMallocObject
	{
	public:
		CMultiMsgSender();
		~CMultiMsgSender();
		uint32 Send(const void* pData, uint32 uSize);
		void AddPipe(CConnServer*);
		void DelPipe(CConnServer*);
		void Release();
	//	void MulticastSendCmd(IMulticast* pMulticast, const void* pData,uint32 uSize) const;
		uint32 GetGlobleID() {return m_uID;};
	private:
		typedef TIDPtrMap<CMultiMsgSender> MapMultiMsgSender;
		static MapMultiMsgSender& GetMultiMsgSender();
		uint32 m_uID;
	};
}
