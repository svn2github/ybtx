/**
*  @brief test.cpp,main
*  @file  test.cpp
*  @author welbon 
*  @Email < libo.weng@gmail.com >
*
* Copyright (C) 1996-2010 SINA Corporation, All Rights Reserved
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*/

//#if (defined(WIN32)) || (defined(WINCE))
#include <stdafx.h>
//#endif //WIN32||WINCE

#include "targetver.h"
#include <stdio.h>
#include "config.h"

#ifdef WIN32
#	include <tchar.h>
#	include <Windows.h>
#endif //WIN32

#include "wbinit.h"
#include <stdlib.h>
#include <string>
#include <assert.h>
#include "wbcallback.h"

#include "option.h"
#include "CWeiBoServer.h"

//#define LIB_loadWEIBOSDK
//#define WEIBO_STATICLIB

#pragma warning(disable:4996)

#if defined(WIN32) || defined(WINCE)// WIN
//
#	if 1//(defined(LIB_loadWEIBOSDK)) || (defined(WEIBO_STATICLIB))

#		if defined(WEIBO_VC80)
#			pragma comment(lib ,"WeiBoU_VC80.lib")
#			pragma comment(lib ,"WBParseWrapper_VC80.lib")
#		elif defined(WEIBO_VC90)
#			pragma comment(lib ,"WeiBoU_VC90.lib")
#			pragma comment(lib ,"WBParseWrapper_VC90.lib")
#		endif

#	else
//	//动态加载
#	endif //LIB_loadWEIBOSDK || WEIBO_STATICLIB
//	
#	if defined(WINCE) // WINCE系统
#		define GETPROCADDRESS GetProcAddressA
#		define SYSTEM(s)
#	else				
#		define GETPROCADDRESS GetProcAddress
#		define SYSTEM(s)	  system(s) 
#	endif 
//
#	include <Windows.h>
#else
#	define GETPROCADDRESS
#	define SYSTEM(s)
#endif // WIN

static
bool load_wb_interface(t_wb_interface* pwb_interface)
{
	memset(pwb_interface , 0 , sizeof(struct t_wb_interface) );

  
	// 若没有定义静态加载标志，或者没有定义WIN32和WINCE
#if 1//(defined(WEIBO_STATICLIB)) || (defined(LIB_loadWEIBOSDK)) || (!defined(WIN32) && !defined(WINCE))

	pwb_interface->pfun_oauth_copy      = loWeibo_oauth_copy;
	pwb_interface->pfun_request_create  = loWeibo_request_create;
	pwb_interface->pfun_request_setopt  = loWeibo_request_setopt;
	pwb_interface->pfun_request_start   = loWeibo_request_start;
	pwb_interface->pfun_request_wait    = loWeibo_request_wait;
	pwb_interface->pfun_request_stop    = loWeibo_request_stop;
	pwb_interface->pfun_request_destroy = loWeibo_request_destroy;
	pwb_interface->pfun_get_url         = loWeibo_get_url;

#else // 动态加载

	WCHAR appPath[256] = {0};
/*	GetModuleFileNameW( NULL , appPath , 256 );
	WCHAR* pz = wcsrchr(appPath , '\\');
	if( !pz )
	{
		pz = wcsrchr(appPath , '/');
	}
	if( pz )
	{
		*(pz+1) = '\0';
	}*/
#if defined( WEIBO_VC90 )
	wcscat(appPath , L"WeiboU_VC90.dll");
	pwb_interface->dll_ = ::LoadLibraryW(appPath);
#elif defined( WEIBO_VC80)
	wcscat(appPath , L"WeiboU_VC80.dll");
	pwb_interface->dll_ = ::LoadLibraryW(appPath);
#endif

	if( !pwb_interface->dll_ )
	{
		FILE* fp = _wfopen(appPath,L"rb");
		if( !fp )
		{
			MessageBox(NULL,appPath,_T("file is not exist!"),MB_OK);
		}
		else
		{
			fclose(fp);
		}
		MessageBox(NULL,appPath,_T("load Weibo DLL failure!"),MB_OK);
		return false;
	}

	pwb_interface->pfun_oauth_copy      = (fun_loWeibo_oauth_copy)::GETPROCADDRESS( pwb_interface->dll_ , "loWeibo_oauth_copy");
	pwb_interface->pfun_request_create  = (fun_loWeibo_request_create)::GETPROCADDRESS( pwb_interface->dll_ , "loWeibo_request_create");
	pwb_interface->pfun_request_setopt  = (fun_loWeibo_request_setopt)::GETPROCADDRESS( pwb_interface->dll_ , "loWeibo_request_setopt");
	pwb_interface->pfun_request_start   = (fun_loWeibo_request_start)::GETPROCADDRESS( pwb_interface->dll_ , "loWeibo_request_start");
	pwb_interface->pfun_request_wait    = (fun_loWeibo_request_wait)::GETPROCADDRESS( pwb_interface->dll_ , "loWeibo_request_wait");
	pwb_interface->pfun_request_stop    = (fun_loWeibo_request_stop)::GETPROCADDRESS( pwb_interface->dll_ , "loWeibo_request_stop");
	pwb_interface->pfun_request_destroy = (fun_loWeibo_request_destroy)::GETPROCADDRESS( pwb_interface->dll_ , "loWeibo_request_destroy");
	pwb_interface->pfun_get_url         = (fun_loWeibo_get_url)::GETPROCADDRESS( pwb_interface->dll_ , "loWeibo_get_url");
	//
	return ( pwb_interface->pfun_oauth_copy &&	pwb_interface->pfun_request_create &&	pwb_interface->pfun_request_setopt &&
		pwb_interface->pfun_request_start && pwb_interface->pfun_request_wait && pwb_interface->pfun_request_stop &&
		pwb_interface->pfun_request_destroy &&	pwb_interface->pfun_get_url );

#endif //(defined(WEIBO_STATICLIB)) || (defined(LIB_loadWEIBOSDK)) 

	return true;
}





static
void unload_wb_interface(t_wb_interface* pwb_interface)
{
#if 0//(!defined(WEIBO_STATICLIB)) && (!defined(LIB_loadWEIBOSDK)) && (defined(WIN32) || defined(WINCE))
	if( pwb_interface->dll_ )
	{
		FreeLibrary(pwb_interface->dll_);
		pwb_interface->dll_ = 0;
	}
#endif
}


#if (defined(WIN32) || defined(WINCE))

HMODULE _ParserDll = NULL;

#endif //(defined(WIN32) || defined(WINCE))

static 
bool load_wb_parser()
{
//#if (!defined(WEIBO_STATICLIB)) && (!defined(LIB_loadWEIBOSDK)) && (defined(WIN32) || defined(WINCE))
//
//#ifdef WEIBO_VC90
//	_ParserDll = ::LoadLibrary(L"WbParser_VC90.dll");
//#elif defined(WEIBO_VC80)
//	_ParserDll = ::LoadLibrary(L"WbParser_VC80.dll");
//#endif
//
//	if( !_ParserDll ){
//		return false;
//	}
//#endif //
	return true;
}

static 
void unload_wb_parser(void)
{
//#if (defined(WIN32) || defined(WINCE))
//	if( _ParserDll ){
//		::FreeLibrary(_ParserDll);
//		_ParserDll = NULL;
//	}
//#endif //#if (defined(WIN32) || defined(WINCE))
}

CWeiBoServer::CWeiBoServer(void)
{
	//CreateLock(&m_Lock);
}

CWeiBoServer::~CWeiBoServer(void)
{
	//DestroyLock(&m_Lock);
}

void CWeiBoServer::GetWeiboInfo(WEIBORequest* wbRequest, int option, struct t_wb_allstruct* pall, const struct t_wb_interface* pwb_interface, void *pUserdata /* = 0 */)
{
	void* p_twb = 0;

	switch( option )
	{
		//账号
	case WEIBO_OPTION(GETACCOUNT_VERIFY):
		{// 验证当前用户身份是否合法
			p_twb = &pall->account_verify;

			Wb_init_wb_struct( option , p_twb);
			// to fill other information

			// NOT NEED PARAM
		}
		break;
	}

	if( !p_twb )
		return ;

	// oauth copy 
	pwb_interface->pfun_oauth_copy( &((struct t_wb_base*)p_twb)->wbauth_ , &pall->oauth );
	pwb_interface->pfun_request_setopt(wbRequest  , WEIBO_OPTION(REQ_SENDDATA) , option , p_twb );
	pwb_interface->pfun_request_setopt(wbRequest  , WEIBO_OPTION(REQ_USERDATA) , pUserdata );
	pwb_interface->pfun_request_start(wbRequest   , true);
	//pwb_interface->pfun_request_stop(wbRequest);
}

void CWeiBoServer::RequestStart()
{
	memset(&twball , 0 , sizeof( struct t_wb_allstruct ) );

	//struct t_wb_cookie* ptest1 = (struct t_wb_cookie*)malloc( 1024 );

	// 静态方式
	if( !load_wb_interface(&wb_interface) )
	{
		unload_wb_interface(&wb_interface);
		return;
	}

#ifdef _USE_WEIBOPARSER
	if( !load_wb_parser() )
	{
		unload_wb_parser();
		//		assert(0);
	}
#endif //_USE_WEIBOPARSER

	//	struct t_wb_cookie* ptest5 = (struct t_wb_cookie*)malloc( 1024 );

	wbRequest = wb_interface.pfun_request_create();


	wb_interface.pfun_request_setopt(wbRequest , WEIBO_OPTION(REQ_COOKIE) , 0);//hack for old mode 0,1 oauth or xauth
	wb_interface.pfun_request_setopt(wbRequest , WEIBO_OPTION(REQ_APPKEY) , "3146556273");// set app key
	wb_interface.pfun_request_setopt(wbRequest , WEIBO_OPTION(REQ_SECRETKEY) , "c9c33a91bdabeea73ab1f0d446f6b325");// set app secret
	wb_interface.pfun_request_setopt(wbRequest , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) ,1);// set data format(josn=0/xml=1)

	Wb_init_wb_struct( WEIBO_OPTION(BASE) , &twball.oauth);

	//	struct t_wb_cookie* ptest7 = (struct t_wb_cookie*)malloc( 1024 );

	Wb_init_wb_struct( WEIBO_OPTION(OAUTH_REQUEST_TOKEN) , &twball.oauth_reqtoken );
	Wb_init_wb_struct( WEIBO_OPTION(OAUTH_ACCESS_TOKEN)  , &twball.oauth_accesstoken);

	// 1 oauth request token
	wb_interface.pfun_request_setopt(wbRequest , WEIBO_OPTION(REQ_SENDDATA) ,WEIBO_OPTION(OAUTH_REQUEST_TOKEN), &twball.oauth_reqtoken);
	wb_interface.pfun_request_setopt(wbRequest , WEIBO_OPTION(REQ_USERDATA) , this );

	wb_interface.pfun_request_start(wbRequest , true);
	//wb_interface.pfun_request_stop(wbRequest);


	//
	//	wb_interface.pfun_request_destroy(wbRequest);
	//
	//	// 释放接口
	//	unload_wb_interface(&wb_interface);
	//
	//	unload_wb_parser();
	//
}

// 请求 access token
void CWeiBoServer::GetAccessToken(const char* sAuthCode)
{
	strcpy(twball.oauth_accesstoken.wbauth_.oauth_verifier_, sAuthCode);
	wb_interface.pfun_request_setopt(wbRequest , WEIBO_OPTION(REQ_SENDDATA) , WEIBO_OPTION(OAUTH_ACCESS_TOKEN) , &twball.oauth_accesstoken);
	wb_interface.pfun_request_setopt(wbRequest , WEIBO_OPTION(REQ_USERDATA) , this );
	wb_interface.pfun_request_start(wbRequest , true);

	//wb_interface.pfun_request_stop(wbRequest);
}

void CWeiBoServer::RetToken(const char* token, const char* secret, int option)
{
	switch(option)
	{
	case WEIBO_OPTION(OAUTH_REQUEST_TOKEN):
		{
			strcpy(twball.oauth_accesstoken.wbauth_.oauth_token_, token);
			strcpy(twball.oauth_accesstoken.wbauth_.oauth_token_secret_, secret);
			OpenWebBrowser(token);
		}
		break;
	case WEIBO_OPTION(OAUTH_ACCESS_TOKEN):
		{
			strcpy(twball.oauth.oauth_token_, token);
			strcpy(twball.oauth.oauth_token_secret_, secret);
			GetWeiboInfo(wbRequest, WEIBO_OPTION(GETACCOUNT_VERIFY), &twball, &wb_interface, this);
		}
		break;
	case WEIBO_OPTION(GETACCOUNT_VERIFY):
		{
			WeiBoLogin(token);
		}
		break;
	}
}