#pragma once
#include "CConnServer.h"
#include "CApexConnHandler.h"
#include "CDynamicObject.h"
#include "CLog.h"
#include <map>

#define HEAD_PACKETLEN_SIZE 4	
#define HEAD_MSGID_SIZE 1
#define HEAD_SENDID_SIZE 4
class IApexProxyCallBackHandler;
class CGac2GasCPP_ApexProxyMsg;

class CApexProxy
	: public virtual CDynamicObject
{
public:
	CApexProxy();
	~CApexProxy();
	typedef map<uint32, uint32>	ConnectionIDMap;
	static const CApexProxy* GetInst() { return ms_pInst; };
	static void CreateInst();
	static void DestroyInst();
	
	void StartApexProxy(const CAddress& addr);
	void StopApexProxy();

	uint32 RecvConnData(void* pData, size_t uSize);
	void NetSendToGameClient(uint32 nSendId,TCHAR * pBuffer,uint32 uSize);
	void GameServerKillUser(uint32 nId,uint32 Action);

	CConnServer* GetConnctionById(uint32 uId);
	uint32 GetIdBuConnction(CConnServer* conn);
	void OnApexProxyCommand(const CGac2GasCPP_ApexProxyMsg* pCmd, CConnServer* conn);
	uint32 NoticeApexProxy_UserData(uint32 nSendId, char* pBuf, uint32 nBufLen);
	uint32 NoticeApexProxy_UserLogin(uint32 nSendId, TCHAR* PlayAccount, CConnServer* conn);
	uint32 NoticeApexProxy_UserLogout(uint32 nSendId, TCHAR* PlayAccount);
	void SetApexProxyCallBackHandler(IApexProxyCallBackHandler* pApexProxyCallBackHandler);
	IApexProxyCallBackHandler* GetApexProxyCallBackHandler();
private:
	static CApexProxy* ms_pInst;
	ConnectionIDMap m_ConnIDMap;
	CApexConnHandler* m_ApexConnHandler;
	IPipe*	m_Pipe;
	CLog* m_ApexLog;
	void SendToItemServer(char MsgID, uint32 nSendId, TCHAR* pBuf, uint32 nBufLen);
	IApexProxyCallBackHandler* m_pApexProxyCallBackHandler;
};
