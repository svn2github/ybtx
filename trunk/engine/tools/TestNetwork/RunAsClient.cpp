#include "stdafx.h"
#include "IPipeHandler.h"
#include "IPipeReactor.h"
#include "IPipe.h"
#include "IPipeModule.h"
#include "CAddress.h"
#include "TestNetwork.h"
#include "ExpHelper.h"






class CClientPipeHandler:
	public IPipeHandler
{
public:
	IPipe*	m_pPipe;

	virtual void OnConnected()
	{
		m_pPipe->Send("hello",5);
		puts( "client onconnected." );
	}

	virtual void OnConnectFailed(EPipeConnFailedReason eReason)
	{
		puts( "client onconnectfailed." );
	}

	virtual void OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully)
	{
		puts( "client disconnected." );
	}

	virtual void OnDataReceived()
	{
		m_pPipe->PopRecvData( m_pPipe->GetRecvDataSize() );

		for(int i=0;i<100;++i)
			m_pPipe->Send("great",5);
		//puts( "on new data received." );
	}
};


int const gs_nPipeNum=500;
IPipe* gs_aryPipe[gs_nPipeNum];


void RunAsClient()
{
	gs_pReactor=gs_pPipeModule->GetSyncIPipeReactor();


	for( int i=0;i<gs_nPipeNum;++i)
	{
		CClientPipeHandler* pHandler=new CClientPipeHandler;

		IPipe* pPipe=gs_pReactor->
			CreateIPipe( pHandler, 1024,10240 , ePCF_SendWayEncryption|ePCF_RecvWayDecompression );
		pHandler->m_pPipe=pPipe;
		gs_aryPipe[i]=pPipe;
	}


	for(;;)
	{
		int nIndex=rand()%gs_nPipeNum;

		IPipe* pPipe=gs_aryPipe[nIndex];

		if( pPipe->IsConnected() )
		{
			if( ( (rand() <<16) | rand() ) == 1 )
			{
				pPipe->ShutDown();				
			}
			else
			{
				static char szData[]="abcdefghijklmnopqrstuvwxyz";
				pPipe->Send( szData,sizeof(szData) );
			}
		}
		else if( pPipe->IsConnecting() )
		{
			if( rand() == 1 )
			{
				puts("shutdown");
				pPipe->ShutDown();
			}
		}
		else
		{
			pPipe->Connect( gs_Addr );
			puts("connect");
			Ast( pPipe->IsConnecting() );
		}

		for(;;)
		{
			switch( gs_pReactor->GetEvent(0) )
			{
			case eGER_TimedOut:
				goto out;
#ifdef _WIN32
			case eGER_SysMsg:
				{
					MSG msg;

					while( PeekMessageW( &msg, NULL, 0, 0, PM_NOREMOVE ) )
						GetMessage( &msg, NULL, 0, 0 );				
				}
#endif
			default:
				break;
			}
		}
out:;
	}


	for( int i=0;i<gs_nPipeNum;++i)
		gs_aryPipe[i]->Release();
}
