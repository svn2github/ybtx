#include "stdafx.h"
#include "CAppClient.h"
#include "CConnClient.h"
#include "CApexClient.h"
#include "PtrlGac2GasCPPDcl.h"
#include "PtrlGac2GasCPPDef.h"
#include "CAppThreadEvent.h"

//
//pfRec 是 Apex Client 提供的数据处理函数;当,GameClient 收到GameServer发来属于Apex 的数据的时候
//		调用这个函数指针处理;详见:  int		NoticeApec_UserData(const char * pBuf,int nBufLen);
//
_FUNC_C_REC			pfRec					= NULL;

static uint32	m_uApexNetTime = 0;

//通知ApexClient,向GameSever发送消息
//参数
//pBuffer	: 发送数据的缓冲区
//nLen		: 发送数据的长度

class CApexNetEvent : public CAppThreadEvent
{
public:
	CApexNetEvent(uint32 uTime, const char* pBuffer, int nLen);
	~CApexNetEvent();

private:
	uint32			m_uTime;
	char*			m_pBuffer;
	int				m_nLen;
};

CApexNetEvent::CApexNetEvent(uint32 uTime, const char* pBuffer, int nLen)
: m_uTime(uTime)
, m_nLen(nLen)
{
	m_pBuffer = new char[nLen];
	memcpy(m_pBuffer, pBuffer, nLen);
}

CApexNetEvent::~CApexNetEvent()
{
	CGac2GasCPP_ApexProxyMsg cmd;	
	cmd.uLen = (uint16)m_nLen;
	CConnClient* pConn = CConnClient::Inst();
	if (!pConn)
	{
		delete [] m_pBuffer;
		return;
	}
	
	if(m_uTime != m_uApexNetTime)
	{
		delete [] m_pBuffer;
		return;
	}

	CConnClient::Inst()->SendCmd(&cmd);
	CConnClient::Inst()->Send(m_pBuffer,m_nLen);
	delete [] m_pBuffer;
}

long NetSendToGameServer(const char * pBuffer,int nLen)
{
	CAppClient::Inst()->PostThreadAppEvent(new CApexNetEvent(m_uApexNetTime, pBuffer, nLen));
	return 0;
}

//
//功能		:	启动ApexClient;
//
int	StartApexClient()
{
	m_uApexNetTime++;

	//启动ApexClient;	NetSendToGameServer 是ApexClient与GameServer发消息函数
	//					pfRec				是Apex 提供的接受函数;
	long    re= CHCStart(NetSendToGameServer,pfRec);
	return 0;
}

//
//停止ApexClient
//
int	StopApexClient()
{
	//停止 ApecClient
	CHCEnd();
	return 0;
}

//
//通知ApexClient,有数据从 GameServer 发过来
//			
//
//参数
//pBuf			: 接收数据的缓冲区
//nBufLen		: 接收数据的长度
int	NoticeApec_UserData(const char * pBuf,int nBufLen)
{
	if(pfRec)
	{
		pfRec(pBuf,nBufLen);
	}
	return 0;
}



