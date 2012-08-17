#include <stdafx.h>
#include "CYYServer.h"
#include "ThreadHelper.h"
#define BUF_LEN 1024


CYYServer* CYYServer::ms_pInst = NULL;

CYYServer::CYYServer(void)
{
	CreateLock(&m_Lock);
}

CYYServer::~CYYServer(void)
{
	DestroyLock(&m_Lock);
}

void CYYServer::CreateInst(const char *confPath)
{
	ms_pInst = new CYYServer();
	ms_pInst->InitServerOpenId(confPath);
}

bool CYYServer::InitServerOpenId(const char *confPath)
{
	int32 bRet = YAuth_init2(confPath);
	bRet += 0;
	return true;
}

const char* CYYServer::RecvLoginRequest(uint32 nSendIp) // 收到登录请求
{
	Lock(&m_Lock);
	const char * lpkey = YAuth_genRequest( nSendIp ); 
	Unlock(&m_Lock);
	return lpkey; 
}

int CYYServer::RecvLoginInfo(char* recvbuf ,uint32 nSendIp) // 收到登录信息
{
	Lock(&m_Lock);
	int ret = YAuth_verifyToken(recvbuf, nSendIp);  
	Unlock(&m_Lock);
	return ret; 
}

//登录成功后取得验证信息
const char* CYYServer::GetUserID( char* recvbuf)
{ 
	//UID
	Lock(&m_Lock);
	char szTemp[1024] = {0};
	YAuth_getAttribute( recvbuf, "UserID", szTemp, sizeof(szTemp) ); 
	Unlock(&m_Lock);
	m_strUserID = szTemp;
	return m_strUserID.c_str();
}

const char* CYYServer::CardNumber( char* recvbuf)
{ 
	//联系方式，姓名，证件号，需要十六进制转换，UID不需要。
	Lock(&m_Lock);
	char szTemp[1024] = {0};
	char szTemp_2[1024] = {0};
	YAuth_getAttribute( recvbuf, "IDCardNumber", szTemp, sizeof(szTemp) );
	YAuth_fromHexToStr( szTemp, szTemp_2, sizeof(szTemp_2) );
	Unlock(&m_Lock);
	m_strCardNumber = szTemp_2;
	return m_strCardNumber.c_str();
}


void CYYServer::DestroyInst()
{
	YAuth_cleanUp();
}
