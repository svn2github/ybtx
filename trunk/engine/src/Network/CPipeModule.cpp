#include "stdafx.h"
#include "CPipeModule.h"
#include <algorithm>
#include "CSyncPipe.h"
#include "CSyncPipeReactor.h"
#include "CAsynPipe.h"
#include "CAsynPipeReactor.h"
#include "ExpHelper.h"
#include "ThreadHelper.h"

int SocketGetLastError()
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}


CPipeModule::CPipeModule(void)
{
#ifdef _WIN32
	WORD wVersion;
	WSADATA wsaData;

	wVersion = MAKEWORD(2, 2);
	int nResult= WSAStartup(wVersion, &wsaData);
	if(nResult)
	{
		stringstream strm;
		strm<<"WSAStartup failed with error code:"<<nResult<<ends;
		GenErr(strm.str());
	}
#endif
}

CPipeModule::~CPipeModule(void)
{
#ifdef _WIN32
	int nResult= WSACleanup();
	if(nResult)
	{
		stringstream strm;
		strm<<"WSACleanup failed with error code:"<<nResult<<ends;
		GenErr(strm.str());
	}
#endif
}

IPipeReactor* CPipeModule::GetSyncIPipeReactor()
{
	return new CSyncPipeReactor;
}

IPipeReactor* CPipeModule::GetAsynIPipeReactor(uint32 uThreadNum)
{
	CAsynPipeReactor::SetThreadNum(uThreadNum);
	return new CAsynPipeReactor;
}


static CPipeModule*	ms_pInst=NULL;
static uint32 ms_uCount=0;

namespace sqr
{

	IPipeModule* GetIPipeModule()
	{
		if(ms_uCount++==0)
			ms_pInst=new CPipeModule;
		return ms_pInst;
	}


	void ReleaseIPipeModule(IPipeModule* pModule)
	{
		if(--ms_uCount==0)
		{
			delete ms_pInst;
			ms_pInst=NULL;
		}
	}

}

