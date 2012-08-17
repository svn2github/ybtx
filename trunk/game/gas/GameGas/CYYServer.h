#pragma once
//#include <Windows.h>
#include "CDynamicObject.h"
#include "ThreadTypes.h"
#include "YY/yauthLib.h"

class CYYServer:public virtual CDynamicObject
{
private:
	static	CYYServer* ms_pInst;
	string	m_strUserID;
	string  m_strCardNumber;
	HLOCK	m_Lock;
	
	

public:

	bool	 InitServerOpenId(const char *confPath);						//初始化验证模块的接口库

	static const CYYServer* GetInst() { return ms_pInst; };
	static void CreateInst(const char *confPath);
	void DestroyInst();
	const char* RecvLoginRequest(uint32 nSendIp);
	int RecvLoginInfo(char* recvbuf,uint32 nSendIp); 
	const char* GetUserID(char* lpLoginInfo);
	const char* CardNumber(char* lpLoginInfo);

	CYYServer(void);
	~CYYServer(void);
};
