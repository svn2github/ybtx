#include "stdafx.h"
#include "RtxNotify/ApplicationApi.h"
#include "CodeCvs.h"

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 4)
	{
		return 0;
	}
	//argv : IP, Receiver, TiTle, Content
#ifdef _WIN32
	string sServerIP = utf16_to_gbk(argv[1]);
	string sReceiver = utf16_to_gbk(argv[2]);
	string sTitle	 = utf16_to_gbk(argv[3]);
	string sContent	 = utf16_to_gbk(argv[4]);
	HRESULT ret = SendNotifyA(sServerIP.c_str(),WORD(8006),sReceiver.c_str(),sTitle.c_str(),long(0),sContent.c_str());
	if (FAILED(ret))
	{
		return 0;
	}
#endif
	return 1;
}
