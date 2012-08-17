#include "stdafx.h"
#include "IPipeHandler.h"
#include "IPipeReactor.h"
#include "IPipe.h"
#include "IPipeModule.h"
#include "IMulticast.h"
#include "CAddress.h"
#include "TestNetwork.h"



static int gs_nPipeNum=0;


const size_t g_stMulticastNum=20;
IMulticast* g_aryMulticast[g_stMulticastNum];



class CServerPipeHandler
	:public IPipeHandler
{
public:
	IPipe*	m_pPipe;

	virtual void OnDisconnected(EPipeDisconnectReason eReason,bool bGracefully)
	{
		puts("OnDisconnected");
		m_pPipe->Release();
		delete this;

		gs_nPipeNum--;
	}
	virtual void OnDataReceived()
	{
		//puts("data received");
		m_pPipe->PopRecvData( m_pPipe->GetRecvDataSize() );

		static char szData[]="1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+|";

		m_pPipe->Send(szData,sizeof(szData));
		
		IMulticast * pma=g_aryMulticast[rand()%g_stMulticastNum];
		IMulticast * pmb=g_aryMulticast[rand()%g_stMulticastNum];
		pma->SendCmd(szData,rand()%10);

		pma->DelPipe( m_pPipe );
		pmb->AddPipe( m_pPipe );


		//int n=rand()%10;

		//for( int i=0;i<n ;++i )
		//{
		//	if( sizeof(szData) != m_pPipe->Send(szData,sizeof(szData)) )
		//		puts("send buffer full.");
		//}

		if( ((rand() <<16) | rand())==1 )
		{
			m_pPipe->ShutDown();
			m_pPipe->Release();
			delete this;
		}
	}	
};

class CListenPipeHandler
	:public IPipeHandler
{
private:
	virtual void OnAccepted(IPipe* pPipe)
	{
		if( rand() % 20 == 1 )
		{
			puts("not accept");
			return;
		}
		CServerPipeHandler* pHandler=new CServerPipeHandler;
		pHandler->m_pPipe=pPipe;
		pPipe->SetHandler( pHandler );		

		gs_nPipeNum++;

		puts("accepted");


		IMulticast* pm=g_aryMulticast[rand()%g_stMulticastNum];

		pm->AddPipe( pPipe );
	}
	virtual void OnListenFailed()
	{
		puts("listen failed!");
		exit(1);
	}
};

void RunAsServer()
{	
	gs_pReactor=gs_pPipeModule->GetAsynIPipeReactor(2);


	IPipe*	pPipe=gs_pReactor->CreateIPipe( new CListenPipeHandler, 10240,1024 
		, ePCF_RecvWayDecryption|ePCF_SendWayCompression );

	for(size_t i=0;i<g_stMulticastNum;++i)
	{
		g_aryMulticast[i]=gs_pReactor->CreateMulticast();
	}

	pPipe->Listen( gs_Addr );

	for(;;)
	{
		switch( gs_pReactor->GetEvent( PIPEINFINITE ) )
		{
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

	for(size_t i=0;i<g_stMulticastNum;++i)
	{
		gs_pReactor->DestroyMulticast(g_aryMulticast[i]);
	}

	pPipe->Release();
}

