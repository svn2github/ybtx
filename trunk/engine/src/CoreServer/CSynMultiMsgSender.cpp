#include "stdafx.h"
#include "CSyncAppServer.h"
#include "CSynConnServer.h"
#include "CSynConnMgrServer.h"
#include "CSynMultiMsgSender.h"
#include "PrtlGas2GacGenericDef.h"
#include "IPipe.h"
#include "IMulticast.h"
#include "CConnMgrServer.h"
#include "CConnServer.h"
#include "TIDPtrMap.inl"
#include "TSqrAllocator.inl"

CSynMultiMsgSender::IDVecSynMultiMsgSender& CSynMultiMsgSender::GetSynMapMsgSender()
{
	static IDVecSynMultiMsgSender ms_MultiMsgSender;
	return ms_MultiMsgSender;
}

CSynMultiMsgSender::CSynMultiMsgSender(uint32 uID)
:m_pMulticast(CSyncAppServer::Inst()->GetIMulticast())
,m_uID(uID)
{
	const uint32 uIndex = TIDPtrMap<CSynMultiMsgSender*>::GetIndexByID(uID);
	IDVecSynMultiMsgSender& vecSender = GetSynMapMsgSender();
	if( vecSender.size() <= uIndex )
		vecSender.resize( uIndex+1 );
	vecSender[uIndex] = this;
}

CSynMultiMsgSender::~CSynMultiMsgSender()
{
	const uint32 uIndex = TIDPtrMap<CSynMultiMsgSender*>::GetIndexByID(m_uID);
	IDVecSynMultiMsgSender& vecSender = GetSynMapMsgSender();
	vecSender[uIndex] = NULL;
	m_pMulticast->Release();
}

uint32 CSynMultiMsgSender::Send(const void* pData, uint32 uSize)
{
	if( uSize <=0xff )
	{
		CGas2GacGC_Small_Shell_Message cmd;
		cmd.usbDataLength = static_cast<uint8>(uSize);
		MulticastSendCmd(m_pMulticast, 
			reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	}
	else if(uSize <= 0xffff )
	{
		CGas2GacGC_Middle_Shell_Message cmd;
		cmd.udbDataLength = static_cast<uint16>(uSize);
		MulticastSendCmd(m_pMulticast, 
			reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	}
	else
	{
		CGas2GacGC_Big_Shell_Message cmd;
		cmd.uqbDataLength = uSize;
		MulticastSendCmd(m_pMulticast, 
			reinterpret_cast<const void*>(&cmd), sizeof(cmd));
	}
	m_pMulticast->SendCmd(pData, uSize);
	return uSize;
}

void CSynMultiMsgSender::AddPipe(CSynConnServer* pConn)
{
	if (!pConn)
		return;
	m_pMulticast->AddPipe(pConn->GetPipe());
}

void CSynMultiMsgSender::DelPipe(CSynConnServer* pConn)
{
	if (!pConn)
		return;
	m_pMulticast->DelPipe(pConn->GetPipe());
}

void CSynMultiMsgSender::Release()
{
	delete this;
}

void CSynMultiMsgSender::MulticastSendCmd(IMulticast*pMulticast, 
									   const void* pData, uint32 uSize)const
{	
	pMulticast->SendCmd(pData, uSize);
}

ISend* CSynMultiMsgSender::GetIS()const
{
	return const_cast<CSynMultiMsgSender*>(this);
}

CSynMultiMsgSender* CSynMultiMsgSender::GetSynMultiMsgSender(uint32 uID)
{
	const uint32 uIndex = TIDPtrMap<CSynMultiMsgSender*>::GetIndexByID(uID);
	IDVecSynMultiMsgSender& MultiMsgSender = GetSynMapMsgSender();
	if( MultiMsgSender.size() <= uIndex )
		return NULL;
	return MultiMsgSender[ uIndex ];
}
