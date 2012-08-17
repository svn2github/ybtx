#include "stdafx.h"
#include "ScriptX.h"
#include "CBaseScriptApp.h"
#include "IPipe.h"
#include "IPipeHandler.h"
#include "CAddress.h"
#include "ISend.h"
#include "NetworkHelper.h"
#include "TSqrAllocator.inl"


int32 CBaseScriptApp::RegisterNetwork(CScript& Script)
{
	REGIST_GLOBALFUNCTION(Script, RandGetHostByName, "s:s");
	REGIST_B_CLASS( Script, CAddress );
	REGIST_ABSTRACT_B_CLASS( Script, IPipe );
	REGIST_B_CLASS( Script, IPipeHandler );

	//×¢²áCAddress
	REGIST_CLASSFUNCTION( Script, CAddress, SetAddress, "s" );
	REGIST_CLASSFUNCTION( Script, CAddress, GetAddress, ":s" );
	REGIST_CLASSFUNCTION( Script, CAddress, SetPort, "I" );
	REGIST_CLASSFUNCTION( Script, CAddress, GetPort, ":I" );

	//×¢²áIPipe
	REGIST_CLASSFUNCTION( Script, IPipe, IsConnected, ":b" );
	REGIST_CLASSFUNCTION( Script, IPipe, Connect, "(CAddress):b" );
	REGIST_CLASSFUNCTION_OVERLOAD( Script, IPipe, void(IPipe::*)(), ShutDown, ShutDown, "" );
	REGIST_CLASSFUNCTION( Script, IPipe, Send, "mt:t" );
	REGIST_CLASSFUNCTION( Script, IPipe, Listen, "(CAddress):b");
	REGIST_CLASSFUNCTION( Script, IPipe, SetHandler, "(IPipeHandler)");

	REGIST_CLASSFUNCTION( Script, IPipe, GetRecvData, ":m" );
	REGIST_CLASSFUNCTION( Script, IPipe, GetRecvDataSize, ":t" );
	REGIST_CLASSFUNCTION( Script, IPipe, PopRecvData, "t" );
	REGIST_CLASSFUNCTION( Script, IPipe, ClearRecvBuffer, "" );
	REGIST_CLASSFUNCTION( Script, IPipe, GetRecvBufferSize, ":t" );

	REGIST_CLASSFUNCTION( Script, IPipe, GetLocalAddress, "(CAddress):b" );
	REGIST_CLASSFUNCTION( Script, IPipe, GetRemoteAddress, "(CAddress):b" );

	REGIST_CLASSFUNCTION( Script, IPipe, Release, "" );

	//×¢²áIPipeHandler
	REGIST_CALLBACKFUNCTION( Script, IPipeHandler, OnConnected, "" );
	REGIST_CALLBACKFUNCTION( Script, IPipeHandler, OnConnectFailed, "I" );
	REGIST_CALLBACKFUNCTION( Script, IPipeHandler, OnDisconnected, "Ib" );
	REGIST_CALLBACKFUNCTION( Script, IPipeHandler, OnAccepted, "(IPipe)" );
	REGIST_CALLBACKFUNCTION( Script, IPipeHandler, OnDataReceived, "" );
	REGIST_CALLBACKFUNCTION( Script, IPipeHandler, OnDataSent, "I" );


	REGIST_ENUMTYPE( Script, EPipeCreationFlag );
	REGIST_ENUMERATION( Script, EPipeCreationFlag, ePCF_SendWayEncryption );
	REGIST_ENUMERATION( Script, EPipeCreationFlag, ePCF_RecvWayDecryption );
	REGIST_ENUMERATION( Script, EPipeCreationFlag, ePCF_SendWayCompression );
	REGIST_ENUMERATION( Script, EPipeCreationFlag, ePCF_RecvWayDecompression );

	return 1;
}

