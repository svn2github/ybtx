// TestNetwork.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "IPipeModule.h"
#include "IPipeHandler.h"
#include "ExpHelper.h"
#include "Memory.h"


IPipeModule* gs_pPipeModule=NULL;
IPipeReactor* gs_pReactor=NULL;

CAddress	gs_Addr;


//打印程序的用法
static void PrintUsage()
{
	puts("Usage: TestNetwork Address Port Operation");
	puts("  Operations:");
	puts("		server		run as a server.");
	puts("		client		run as a client.");
}


void RunAsServer();
void RunAsClient();

int main(int argc, char* argv[])
{
	if( argc < 4 )
	{
		PrintUsage();
		return 1;
	}

	EnableCatchError( false );

	gs_Addr.SetAddress( argv[1] );
	gs_Addr.SetPort( atoi(argv[2]) );

	gs_pPipeModule=GetIPipeModule();


	string sOperation=argv[3];

	SQR_TRY
	{
		if(sOperation=="server")
		{
			RunAsServer();
		}
		else if(sOperation=="client")
		{
			RunAsClient();
		}
		else
		{
			PrintUsage();
			return 1;
		}
	}
	SQR_CATCH( exp )
	{
		puts( exp.ErrorTitle() );
	}
	SQR_TRY_END;


	gs_pReactor->Release();
	gs_pReactor=NULL;

	ReleaseIPipeModule( gs_pPipeModule);
	gs_pPipeModule=NULL;
	return 0;
}




