#pragma once
#include "ISend.h"
#include "TConnAllocator.h"

namespace sqr
{
	class IMulticast;
	class CSynConnServer;

	class CSynMultiMsgSender
		:public ISend
		,public CConnMallocObject
	{
		friend class CMultiMsgSender;
		friend class CCoreMulticastSenderSendCmdJob;
	public:
		CSynMultiMsgSender(uint32 uID);
		~CSynMultiMsgSender();
		uint32 Send(const void* pData, uint32 uSize);
		void AddPipe(CSynConnServer*);
		void DelPipe(CSynConnServer*);
		void Release();
		ISend* GetIS()const;
		uint32 GetGlobleID()const{return m_uID;};
		static CSynMultiMsgSender* GetSynMultiMsgSender(uint32 uID);
	private:
		void MulticastSendCmd(IMulticast* pMulticast, 
			const void* pData,
			uint32 uSize) const;
		IMulticast* m_pMulticast;
		typedef vector<CSynMultiMsgSender*,TConnAllocator<CSynMultiMsgSender*> > IDVecSynMultiMsgSender;
		static IDVecSynMultiMsgSender& GetSynMapMsgSender();
		uint32 m_uID;
	};
}
