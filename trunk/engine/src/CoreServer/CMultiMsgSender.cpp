#include "stdafx.h"
#include "CMultiMsgSender.h"
#include "CSynConnMgrServer.h"
#include "CConnMgrServer.h"
#include "CSynMultiMsgSender.h"
#include "CConnServer.h"
#include "TIDPtrMap.inl"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"
#include "CCoreConnServerJob.h"
#include "CCoreServerJob.inl"

CMultiMsgSender::MapMultiMsgSender& CMultiMsgSender::GetMultiMsgSender()
{
	static MapMultiMsgSender ms_MultiMsgSender;
	return ms_MultiMsgSender;
}


CMultiMsgSender::CMultiMsgSender()
{
	m_uID = GetMultiMsgSender().Add(this);
	(new CCoreCreateSynMultiMsgSenderJob(GetGlobleID()))->Add();
}

CMultiMsgSender::~CMultiMsgSender()
{
	Ver(GetMultiMsgSender().Del(m_uID));
}

uint32 CMultiMsgSender::Send(const void* pData, uint32 uSize)
{
	if (uSize == 0)
		return 0;
	(new CCoreMultiMsgSenderDoSendJob(m_uID, pData, uSize))->Add();
	return uSize;
}

void CMultiMsgSender::AddPipe(CConnServer* pConn)
{
	if (!pConn)
		return;
	uint32 uConnID = pConn->GetGlobalID();
	(new CCoreMultiMsgSenderAddPipeJob(m_uID, uConnID))->Add();
}

void CMultiMsgSender::DelPipe(CConnServer* pConn)
{
	if (!pConn)
		return;
	uint32 uConnID = pConn->GetGlobalID();
	(new CCoreMultiMsgSenderDelPipeJob(m_uID, uConnID))->Add();
}

void CMultiMsgSender::Release()
{
	(new CCoreMultiMsgSenderDestoryJob(m_uID))->Add();
	delete this;
}

