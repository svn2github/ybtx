#pragma once
//$id$


#include "IPipe.h"


class  CTestScriptMemoryPipe:public IPipe
{
private:
	size_t m_nCurPos;
	size_t m_nDataSize;
	char m_Buff[70000];
public:
	CTestScriptMemoryPipe() { m_nCurPos = 0; }
	void SetHandler(IPipeHandler* pHandler){};
	IPipeHandler* GetHandler()const{return NULL;};

	IPipeReactor* GetReactor()const{return NULL;};
	bool IsConnected()const{return true;}
	bool IsConnecting()const{return false;}

	//Connection method
	bool Listen(const CAddress& Address){return true;};
	bool Connect(const CAddress& Address){return true;};
	void ShutDown(){};
	void ShutDown(bool){};
	bool IsSync()const{return true;}

	//SendBuffer method
	size_t Send(const void* pData,size_t stSize);
	//RecvBuffer method
	void* GetRecvData()const;
	size_t GetRecvDataSize()const;
	void PopRecvData(size_t stSize);
	void ClearRecvBuffer(){m_nCurPos = 0;};
	size_t GetRecvBufferSize()const{return 0;};
	bool RecvBufferFull()const{return false;}

	//Address method
	bool GetLocalAddress(CAddress& Address)const{return true;};
	bool GetRemoteAddress(CAddress& Address)const{return true;};
	bool DelFrMulticast(IMulticast *){return true;}
	bool AddToMulticast(IMulticast *){return true;}
	void SetEnableEndecode(bool){}
	size_t GetDataFlowStatistic(){return 0;}

	void Release(){};
};
