/**
*  @brief weibo call back
*  @file  wbcallback.cpp
*  @author loach 
*  @Email < loachmr@sina.com >
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

#include "stdafx.h"
#include <wbParser/splitstr.h>
#include <wbParser/wbParser.h>
#include <wbParser/wbParseMalloc.h>
#include "wbcallback.h"
#include <stdlib.h>
#include <string>
#include "CWeiBoServer.h"
#include "wbinit.h"
#include <tinyxml.h>

#include "CAppThreadEvent.h"
#include "CAppServer.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

// for declare function
#define WEIBO_struct_statuscallback_init(name) \
	static void Wb_struct_statuscallback_init_##name##_cb(int state , int errCode , void* pUserdata)

#define WEIBO_struct_statuscallback_init_fun(name) Wb_struct_statuscallback_init_##name##_cb

//
#define WEIBO_struct_bodycallback_init(name) \
	static void Wb_struct_bodycallback_init_##name##_cb(unsigned long httpCode , const char* body , unsigned long len , void* pUserdata)

#define WEIBO_struct_bodycallback_init_fun(name) Wb_struct_bodycallback_init_##name##_cb

//
#define WEIBO_struct_headerycallback_init(name) \
	static void Wb_struct_headercallback_init_##name##_cb(unsigned long httpCode , const char* header , unsigned long len , void* pUserdata)

#define WEIBO_struct_headercallback_init_fun(name) Wb_struct_headercallback_init_##name##_cb

#include <wbParser/wbParseWrapper.h>

#ifdef _MSC_VER
#	if defined(WEIBO_VC60)
#		pragma comment(lib,"WBParseWrapper_VC60.lib")
#	elif defined(WEIBO_VC70)
#		pragma comment(lib,"WBParseWrapper_VC70.lib")
#	elif defined(WEIBO_VC80)
#		pragma comment(lib,"WBParseWrapper_VC80.lib")
#	elif defined(WEIBO_VC90)
#		pragma comment(lib,"WBParseWrapper_VC90.lib")
#	endif //
#endif //_MSC_VER


class CWeiBoNetEvent : public CAppThreadEvent
{
public:
	CWeiBoNetEvent(CWeiBoServer* pWeiBo);
	CWeiBoNetEvent(CWeiBoServer* pWeiBo, const char* token, const char* tokenSecret, int option);
	~CWeiBoNetEvent();

private:
	CWeiBoServer* m_pWeiBo;
	char*	m_pToken;
	char*	m_pTokenSecret;
	int		m_option;
	bool	m_bSuc;
};

CWeiBoNetEvent::CWeiBoNetEvent(CWeiBoServer* pWeiBo)
:m_pWeiBo(pWeiBo),m_option(0),m_pToken(NULL),m_pTokenSecret(NULL),m_bSuc(false)
{

}

CWeiBoNetEvent::CWeiBoNetEvent(CWeiBoServer* pWeiBo, const char* token, const char* tokenSecret, int option)
:m_pWeiBo(pWeiBo),m_option(option),m_pToken(NULL),m_pTokenSecret(NULL), m_bSuc(true)
{
	if (tokenSecret)
	{
		int nLen = WB_REAL_LEN(OAUTH_TOKEN);
		m_pToken = new char[nLen];
		memcpy(m_pToken, token, nLen);
		nLen = WB_REAL_LEN(OAUTH_TOKEN_SECRET);
		m_pTokenSecret = new char[nLen];
		memcpy(m_pTokenSecret, tokenSecret, nLen);
	}
	else
	{
		int nLen = WB_REAL_LEN(UID);
		m_pToken = new char[nLen];
		memcpy(m_pToken, token, nLen);
	}
}

CWeiBoNetEvent::~CWeiBoNetEvent()
{
	if (m_bSuc)
		m_pWeiBo->RetToken(m_pToken, m_pTokenSecret, m_option);
	else
		m_pWeiBo->LoginFaild();
}


WEIBO_struct_statuscallback_init(BASE)
{
}

WEIBO_struct_bodycallback_init(BASE)
{
	printf("   BASE : \n    %s \n\n" , body );
}

//------------------------------------获取下行数据集(timeline)接口---------------------------------------------//
//

// 2.获取最新更新的公共微博消息
WEIBO_struct_statuscallback_init(GETSTATUSES_PUBLIC_TIMELINE)
{

}

WEIBO_struct_bodycallback_init(GETSTATUSES_PUBLIC_TIMELINE)
{
	printf("GETSTATUSES_PUBLIC_TIMELINE : \n    %s \n\n" , body );

}

// 3.获取当前用户所关注用户的最新微博信息
WEIBO_struct_statuscallback_init(GETSTATUSES_FRIENDS_TIMELINE)
{
}

WEIBO_struct_bodycallback_init(GETSTATUSES_FRIENDS_TIMELINE)
{
	printf("GETSTATUSES_FRIENDS_TIMELINE : \n    %s \n\n" , body );

}

// 4.获取用户发布的微博信息列表
WEIBO_struct_statuscallback_init(GETSTATUSES_USE_TIMELINE)
{

}
WEIBO_struct_bodycallback_init(GETSTATUSES_USE_TIMELINE)
{
	printf("   GETSTATUSES_USE_TIMELINE : \n    %s \n\n" , body );

}

// 5.获取@当前用户的微博列表
WEIBO_struct_statuscallback_init(GETSTATUSES_MENTIONS)
{

}
WEIBO_struct_bodycallback_init(GETSTATUSES_MENTIONS)
{
	printf("   GETSTATUSES_MENTIONS : \n    %s \n\n" , body );

}


// 6.获取当前用户发送及收到的评论列表
WEIBO_struct_statuscallback_init(GETSTATUSES_COMMENTS_TIMELINE)
{
}

WEIBO_struct_bodycallback_init(GETSTATUSES_COMMENTS_TIMELINE)
{
	printf("   GETSTATUSES_COMMENTS_TIMELINE : \n    %s \n\n" , body );

}

// 7.获取当前用户发出的评论
WEIBO_struct_statuscallback_init(GETSTATUSES_COMMENTS_BYME)
{

}
WEIBO_struct_bodycallback_init(GETSTATUSES_COMMENTS_BYME)
{
	printf("   GETSTATUSES_COMMENTS_BYME : \n    %s \n\n" , body );
}

// 8 .获取指定微博的评论列表
WEIBO_struct_statuscallback_init(GETSTATUSES_COMMENTS_LIST)
{

}

WEIBO_struct_bodycallback_init(GETSTATUSES_COMMENTS_LIST)
{
	printf("   GETSTATUSES_COMMENTS_LIST : \n    %s \n\n" , body );

}

//  9 .批量获取一组微博的评论数及转发数
WEIBO_struct_statuscallback_init(GETSTATUSES_COMMENTS_COUNTS)
{

}

WEIBO_struct_bodycallback_init(GETSTATUSES_COMMENTS_COUNTS)
{
	printf("   GETSTATUSES_COMMENTS_COUNTS : \n    %s \n\n" , body );

}


// 10.获取当前用户未读消息数
WEIBO_struct_statuscallback_init(GETSTATUSES_UNREAD)
{

}
WEIBO_struct_bodycallback_init(GETSTATUSES_UNREAD)
{
	printf("   GETSTATUSES_UNREAD : \n    %s \n\n" , body );

}

//---------------------------------------微博访问接口--------------------------------------------------//

// 11 根据ID获取单条微博信息内容
WEIBO_struct_statuscallback_init(GETSTATUSES_SHOW)
{
}
WEIBO_struct_bodycallback_init(GETSTATUSES_SHOW)
{
	printf("   GETSTATUSES_SHOW : \n    %s \n\n" , body );

}

// 12 根据微博ID和用户ID跳转到单条微博页面
WEIBO_struct_statuscallback_init(GOTOSTATUSES_ID)
{
}
WEIBO_struct_bodycallback_init(GOTOSTATUSES_ID)
{
	// 跳转页面
	printf("   GOTOSTATUSES_ID : \n    %s \n\n" , body );
}

// 13 发布一条微博信息
WEIBO_struct_statuscallback_init(PUTSTATUSES_UPDATE)
{
}
WEIBO_struct_bodycallback_init(PUTSTATUSES_UPDATE)
{
	printf("   PUTSTATUSES_UPDATE : \n    %s \n\n" , body );

}


// 14 上传图片并发布一条微博信息
WEIBO_struct_statuscallback_init(PUTSTATUSES_UPLOAD)
{

}
WEIBO_struct_bodycallback_init(PUTSTATUSES_UPLOAD)
{
	printf("   PUTSTATUSES_UPLOAD : \n    %s \n\n" , body );

}

// 15 删除一条微博信息
WEIBO_struct_statuscallback_init(PUTSTATUSES_DESTROY)
{

}
WEIBO_struct_bodycallback_init(PUTSTATUSES_DESTROY)
{
	printf("   PUTSTATUSES_DESTROY : \n    %s \n\n" , body );

}

// 16 转发一条微博信息（可加评论）
WEIBO_struct_statuscallback_init(PUTSTATUSES_REPOST)
{

}

WEIBO_struct_bodycallback_init(PUTSTATUSES_REPOST)
{
	printf("   PUTSTATUSES_REPOST : \n    %s \n\n" , body );

}



// 17 对一条微博信息进行评论
WEIBO_struct_statuscallback_init(PUTSTATUSES_COMMENT)
{

}
WEIBO_struct_bodycallback_init(PUTSTATUSES_COMMENT)
{
	printf("   PUTSTATUSES_COMMENT : \n    %s \n\n" , body );

}

// 18 删除当前用户的微博评论信息
WEIBO_struct_statuscallback_init(PUTSTATUSES_COMMENT_DESTROY)
{

}
WEIBO_struct_bodycallback_init(PUTSTATUSES_COMMENT_DESTROY)
{
	printf("   PUTSTATUSES_COMMENT_DESTROY : \n    %s \n\n" , body );

}


// 19 回复微博评论信息
WEIBO_struct_statuscallback_init(PUTSTATUSES_REPLY)
{

}
WEIBO_struct_bodycallback_init(PUTSTATUSES_REPLY)
{
	printf("   PUTSTATUSES_REPLY : \n    %s \n\n" , body );

}

//---------------------------------------用户接口--------------------------------------------------//



// 20 根据用户ID获取用户资料（授权用户）
WEIBO_struct_statuscallback_init(GETUSER_INFO)
{

}
WEIBO_struct_bodycallback_init(GETUSER_INFO)
{
	printf("   GETUSER_INFO : \n    %s \n\n" , body );
}


//  21 获取当前用户关注对象列表及最新一条微博信息
WEIBO_struct_statuscallback_init(GETFRINDS_LIST)
{

}
WEIBO_struct_bodycallback_init(GETFRINDS_LIST)
{
	printf("   GETFRINDS_LIST : \n    %s \n\n" , body );

}


// 22 获取当前用户粉丝列表及最新一条微博信息
WEIBO_struct_statuscallback_init(GETFOLLOWERS_LIST)
{

}
WEIBO_struct_bodycallback_init(GETFOLLOWERS_LIST)
{
	printf("   GETFOLLOWERS_LIST : \n    %s \n\n" , body );

}


//---------------------------------------私信接口--------------------------------------------------//

// 23 获取当前用户最新私信列表
WEIBO_struct_statuscallback_init(GETDIRECTMSG)
{

}
WEIBO_struct_bodycallback_init(GETDIRECTMSG)
{
	printf("   GETDIRECTMSG : \n    %s \n\n" , body );
}


// 24 获取当前用户发送的最新私信列表
WEIBO_struct_statuscallback_init(GETDIRESTMSG_SENT)
{

}
WEIBO_struct_bodycallback_init(GETDIRESTMSG_SENT)
{
	printf("   GETDIRESTMSG_SENT : \n    %s \n\n" , body );

}


// 25 发送一条私信
WEIBO_struct_statuscallback_init(PUTDIRECTMSG_NEW)
{

}
WEIBO_struct_bodycallback_init(PUTDIRECTMSG_NEW)
{
	printf("   PUTDIRECTMSG_NEW : \n    %s \n\n" , body );
}


//26 删除一条私信
WEIBO_struct_statuscallback_init(PUTDIRECTMSG_DESTROY)
{

}
WEIBO_struct_bodycallback_init(PUTDIRECTMSG_DESTROY)
{
	printf("   PUTDIRECTMSG_DESTROY : \n    %s \n\n" , body );
}

//26 删除一条私信
WEIBO_struct_statuscallback_init(GETDIRECTMSG_WITH)
{

}
WEIBO_struct_bodycallback_init(GETDIRECTMSG_WITH)
{
	printf("   PUTDIRECTMSG_DESTROY : \n    %s \n\n" , body );
}


//---------------------------------------关注接口--------------------------------------------------//

// 27 关注某用户
WEIBO_struct_statuscallback_init(PUTFRIENDSHIPS_CREATE)
{

}
WEIBO_struct_bodycallback_init(PUTFRIENDSHIPS_CREATE)
{
	printf("   PUTFRIENDSHIPS_CREATE : \n    %s \n\n" , body );

}

// 批量关注接口
WEIBO_struct_statuscallback_init(PUTFRIENDSHIPS_CREATE_BATCH)
{

}
WEIBO_struct_bodycallback_init(PUTFRIENDSHIPS_CREATE_BATCH)
{
	printf("   PUTFRIENDSHIPS_CREATE_BATCH : \n    %s \n\n" , body );

}

// 28 取消关注
WEIBO_struct_statuscallback_init(PUTFRIENDSHIPS_DESTROY)
{

}
WEIBO_struct_bodycallback_init(PUTFRIENDSHIPS_DESTROY)
{
	printf("   PUTFRIENDSHIPS_DESTROY : \n    %s \n\n" , body );

}

// 29 是否关注某用户
WEIBO_struct_statuscallback_init(GETFRIENDSHIPS_EXISTS)
{

}
WEIBO_struct_bodycallback_init(GETFRIENDSHIPS_EXISTS)
{
	printf("   GETFRIENDSHIPS_EXISTS : \n    %s \n\n" , body );
	// TODO:
}

//
WEIBO_struct_statuscallback_init(GETFRIENDSHIPS_BATCH_EXISTS)
{

}
WEIBO_struct_bodycallback_init(GETFRIENDSHIPS_BATCH_EXISTS)
{
	printf("   GETFRIENDSHIPS_BATCH_EXISTS : \n    %s \n\n" , body );
	// TODO:
}




//
//---------------------------------------Social Graph--------------------------------------------------//

//  30 获取用户关注对象uid列表
WEIBO_struct_statuscallback_init(GETFRIEND_IDS)
{

}
WEIBO_struct_bodycallback_init(GETFRIEND_IDS)
{
	printf("   GETFRIEND_IDS : \n    %s \n\n" , body );

}


// 31 获取用户粉丝对象uid列表
WEIBO_struct_statuscallback_init(GETFOLLOWER_IDS)
{

}
WEIBO_struct_bodycallback_init(GETFOLLOWER_IDS)
{
	printf("   GETFOLLOWER_IDS : \n    %s \n\n" , body );

}

//---------------------------------------账号接口--------------------------------------------------//



//32 验证当前用户身份是否合法
WEIBO_struct_statuscallback_init(GETACCOUNT_VERIFY)
{

}

WEIBO_struct_bodycallback_init(GETACCOUNT_VERIFY)
{
	CWeiBoServer* pWeiBo = (CWeiBoServer*)pUserdata;
	if( pWeiBo )
	{
		if (200 == httpCode)
		{
			TiXmlDocument doc;
			doc.Parse(body, 0, TIXML_ENCODING_UTF8);
			if( doc.Error())
			{
				//错误处理
			}
			TiXmlElement *pXml = new TiXmlElement( *doc.RootElement() );
			TiXmlElement *pChild = pXml->FirstChildElement("id");
			if (pChild)
			{
				const char* uid = pChild->GetText();
				CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo, uid, NULL, WEIBO_OPTION(GETACCOUNT_VERIFY)));
			}
			else
				CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo)); //错误处理
			delete pXml;
		}
		else
			CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo)); //错误处理
	}
	else
	{
		CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo)); // 错误处理
	}
}

//33 获取当前用户API访问频率限制
WEIBO_struct_statuscallback_init(GETACCOUNT_RATELIMIT)
{

}
WEIBO_struct_bodycallback_init(GETACCOUNT_RATELIMIT)
{
	printf("   GETACCOUNT_RATELIMIT : \n    %s \n\n" , body );

}

//34 当前用户退出登录
WEIBO_struct_statuscallback_init(PUTACCOUNT_QUITSESSION)
{

}
WEIBO_struct_bodycallback_init(PUTACCOUNT_QUITSESSION)
{
	printf("   PUTACCOUNT_QUITSESSION : \n    %s \n\n" , body );

}

// 35 更改头像
WEIBO_struct_statuscallback_init(PUTACCOUNT_UPDATE_PROFILEIMAGE)
{

}
WEIBO_struct_bodycallback_init(PUTACCOUNT_UPDATE_PROFILEIMAGE)
{
	printf("   PUTACCOUNT_UPDATE_PROFILEIMAGE : \n    %s \n\n" , body );

}


// 36 更改资料
WEIBO_struct_statuscallback_init(PUTACCOUNT_UPDATE_PROFILE)
{

}
WEIBO_struct_bodycallback_init(PUTACCOUNT_UPDATE_PROFILE)
{
	printf("   PUTACCOUNT_UPDATE_PROFILE : \n    %s \n\n" , body );

}

//37 注册新浪微博帐号
WEIBO_struct_statuscallback_init(PUTACCOUNT_REGISTER)
{

}
WEIBO_struct_bodycallback_init(PUTACCOUNT_REGISTER)
{
	printf("   PUTACCOUNT_REGISTER : \n    %s \n\n" , body );
	//TODO :
}

//--------------------------------------- 收藏接口--------------------------------------------------//
// 38 获取当前用户的收藏列表 
WEIBO_struct_statuscallback_init(GETFAVORITES)
{

}

WEIBO_struct_bodycallback_init(GETFAVORITES)
{
	printf("   GETFAVORITES : \n    %s \n\n" , body );

}

// 39 添加收藏 
WEIBO_struct_statuscallback_init(PUTFAVORITES_CREATE)
{

}

WEIBO_struct_bodycallback_init(PUTFAVORITES_CREATE)
{
	printf("   PUTFAVORITES_CREATE : \n    %s \n\n" , body );

}

// 40 删除当前用户收藏的微博信息 
WEIBO_struct_statuscallback_init(PUTFAVORITES_DESTROY)
{

}

WEIBO_struct_bodycallback_init(PUTFAVORITES_DESTROY)
{
	printf("   PUTFAVORITES_DESTROY : \n    %s \n\n" , body );

}


//登录/OAuth

WEIBO_struct_statuscallback_init(OAUTH_REQUEST_TOKEN)
{

}

//#define __USE_TEST_EXPLOER__

#ifdef __USE_TEST_EXPLOER__
#include <Windows.h>
#endif //#ifdef __USE_TEST_EXPLOER__

WEIBO_struct_bodycallback_init(OAUTH_REQUEST_TOKEN)// 获取未授权的Request Token
{
	CWeiBoServer* pWeiBo = (CWeiBoServer*)pUserdata;
	t_wb_oauth_access_token poauth_acess;
	Wb_init_wb_struct( WEIBO_OPTION(OAUTH_ACCESS_TOKEN), &poauth_acess);
	if( pWeiBo )
	{
		if(200 == httpCode)
		{
			wb_parse_oauth(body, poauth_acess.wbauth_.oauth_token_ , poauth_acess.wbauth_.oauth_token_secret_,0 );
			CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo, poauth_acess.wbauth_.oauth_token_, poauth_acess.wbauth_.oauth_token_secret_, WEIBO_OPTION(OAUTH_REQUEST_TOKEN)));
		}
		else
			CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo)); //错误处理
	}
	else
	{
		CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo)); // 错误处理
	}
}

//
// 请求用户授权Token 
WEIBO_struct_statuscallback_init(OAUTH_AUTHORIZE)
{	
}

WEIBO_struct_bodycallback_init(OAUTH_AUTHORIZE)
{
/*	struct t_wb_oauth* poauth = (struct t_wb_oauth*)pUserdata;

	printf("   OAUTH_ACCESS_TOKEN : \n    %s \n\n" , body );

	if( poauth && httpCode == 200 )
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_JSON(body);
		if( pRoot )
		{
			wbParserNS::GetCHAR_Key_JSON("oauth_verifier", pRoot , poauth->oauth_verifier_ , 64);
			wbParserNS::Parse_free_JSON(pRoot);
		}
		printf("oauth_acess_token =  %s \n"          , poauth->oauth_token_);
		printf("oauth_acess_token_verifier =  %s \n" , poauth->oauth_verifier_ );
	}
*/
}

//
// 获取授权过的Access Token
WEIBO_struct_statuscallback_init(OAUTH_ACCESS_TOKEN)
{

}

WEIBO_struct_bodycallback_init(OAUTH_ACCESS_TOKEN)
{
	// 分析把数据写到 t_wb_oauth,这个是最终结果（oauth_token AND oauth_token_secret)
	// 以后所有其他请求，就需要 使用这个 t_wb_oauth 中的数据了

	CWeiBoServer* pWeiBo = (CWeiBoServer*)pUserdata;
	t_wb_oauth poauth;
	Wb_init_wb_struct( WEIBO_OPTION(BASE), &poauth);
	if( pWeiBo )
	{ 
		if (200 == httpCode)
		{
			wb_parse_oauth(body, poauth.oauth_token_, poauth.oauth_token_secret_, 0);
			CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo, poauth.oauth_token_, poauth.oauth_token_secret_, WEIBO_OPTION(OAUTH_ACCESS_TOKEN)));
		}
		else
			CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo)); //错误处理
	}
	else
	{
		CAppServer::Inst()->PostThreadAppEvent(new CWeiBoNetEvent(pWeiBo)); // 错误处理
	}
	printf("   OAUTH_ACCESS_TOKEN : \n    %s \n\n" , body );
}
// 获取授权过的Access Token
WEIBO_struct_statuscallback_init(XAUTH_ACCESS_TOKEN)
{

}
WEIBO_struct_bodycallback_init(XAUTH_ACCESS_TOKEN)
{
	// 分析把数据写到 t_wb_oauth,这个是最终结果（oauth_token AND oauth_token_secret)
	// 以后所有其他请求，就需要 使用这个 t_wb_oauth 中的数据了

	//struct t_wb_oauth* poauth = (struct t_wb_oauth*)pUserdata;

	//if( poauth && httpCode == 200 )
	//{
	//	wb_parse_oauth(body , poauth->oauth_token_ , poauth->oauth_token_secret_ ,0);

	//	printf("   oauth_acess_token =  %s \n" , poauth->oauth_token_ );
	//	printf("   oauth_acess_token_secret =  %s \n" , poauth->oauth_token_secret_ );
	//}
	//else
	//{
	//	printf("   OAUTH_ACCESS_TOKEN : \n    %s \n\n" , body );
	//}
}

// 表情
WEIBO_struct_statuscallback_init(GET_EMOTIONS)
{

}

WEIBO_struct_bodycallback_init(GET_EMOTIONS)
{
	printf("   GET_EMOTIONS : \n    %s \n\n" , body );
}

// 用户SEARCH
WEIBO_struct_statuscallback_init(GET_USERS_SEARCH)
{

}

WEIBO_struct_bodycallback_init(GET_USERS_SEARCH)
{
	printf("   GET_USERS_SEARCH : \n    %s \n\n" , body );
}

// 微博SEARCH
WEIBO_struct_statuscallback_init(GET_WB_SEARCH)
{

}

WEIBO_struct_bodycallback_init(GET_WB_SEARCH)
{
	printf("   GET_WB_SEARCH : \n    %s \n\n" , body );
}

// 
WEIBO_struct_statuscallback_init(GET_STATUSES_SEARCH)
{

}

WEIBO_struct_bodycallback_init(GET_STATUSES_SEARCH)
{
	printf("   GET_STATUSES_SEARCH : \n    %s \n\n" , body );
}

// 省份
WEIBO_struct_statuscallback_init(GET_PROVINCES)
{

}

WEIBO_struct_bodycallback_init(GET_PROVINCES)
{
	printf("   GET_PROVINCES : \n    %s \n\n" , body );
}

//cookie方式
WEIBO_struct_statuscallback_init(COOKIE)
{

}


WEIBO_struct_bodycallback_init(COOKIE)
{
	struct t_wb_oauth* poauth = (struct t_wb_oauth*)pUserdata;

	//
	int iCount = 0;
	wbParserNS::t_wbParse_Cookie cookie;
	wbParserNS::t_wbParse_Cookie *pCookie = &cookie;
	wbParserNS::USE_WBPARSE_FUNC(Cookie,BASE,body,len,iCount,(void**)&pCookie,NULL,NULL);
	
	strcpy(poauth->tgt_,pCookie->tgt);

	printf("   COOKIE : \n    %s \n\n" , body );
}

WEIBO_struct_headerycallback_init(COOKIE)
{
	struct t_wb_oauth* poauth = (struct t_wb_oauth*)pUserdata;

	printf("   COOKIE : \n    %s \n\n" , header );
	if( poauth && httpCode == 200 )
	{
		wb_parse_cookie(header , poauth->oauth_token_ , poauth->oauth_token_secret_ );

		printf("   SUE =  %s \n" , poauth->oauth_token_ );
		printf("   SUP =  %s \n" , poauth->oauth_token_secret_ );
	}
}

//cookie方式
WEIBO_struct_statuscallback_init(UPDATETGT)
{

}

WEIBO_struct_bodycallback_init(UPDATETGT)
{
	struct t_wb_oauth* poauth = (struct t_wb_oauth*)pUserdata;

	printf("   COOKIE : \n    %s \n\n" , body );
}

WEIBO_struct_headerycallback_init(UPDATETGT)
{
	struct t_wb_oauth* poauth = (struct t_wb_oauth*)pUserdata;

	printf("   COOKIE : \n    %s \n\n" , header );
	if( poauth && httpCode == 200 )
	{
		wb_parse_cookie(header , poauth->oauth_token_ , poauth->oauth_token_secret_ );

		printf("   SUE =  %s \n" , poauth->oauth_token_ );
		printf("   SUP =  %s \n" , poauth->oauth_token_secret_ );
	}
}

//自定义URL
WEIBO_struct_statuscallback_init(CUSTOM)
{

}

WEIBO_struct_bodycallback_init(CUSTOM)
{

}

WEIBO_struct_statuscallback_init(GETSTATUSES_COMMENTS_TOME)
{
}
WEIBO_struct_bodycallback_init(GETSTATUSES_COMMENTS_TOME)
{
}

WEIBO_struct_statuscallback_init(PUTSTATUSES_RESET_COUNT)
{
}
WEIBO_struct_bodycallback_init(PUTSTATUSES_RESET_COUNT)
{
}


// 获取系统推荐用户
WEIBO_struct_statuscallback_init(GET_USERS_HOT)
{

}

WEIBO_struct_bodycallback_init(GET_USERS_HOT)
{
	printf("   GET_USERS_HOT : \n    %s \n\n" , body );
}

//更新修改当前登录用户所关注的某个好友的备注信息New!
WEIBO_struct_statuscallback_init(POST_USERS_REMARK)
{

}

WEIBO_struct_bodycallback_init(POST_USERS_REMARK)
{
	printf("   POST_USERS_REMARK : \n    %s \n\n" , body );
}


//Users/suggestions 返回当前用户可能感兴趣的用户
WEIBO_struct_statuscallback_init(GET_USERS_SUGGESTIONS)
{

}

WEIBO_struct_bodycallback_init(GET_USERS_SUGGESTIONS)
{
	printf("   GET_USERS_SUGGESTIONS : \n    %s \n\n" , body );
}




#include "callback/wbcb_block.inl"
#include "callback/wbcb_hotpoint.inl"
#include "callback/wbcb_tags.inl"
#include "callback/wbcb_trends.inl"
#include "callback/wbcb_invitecontact.inl"

//media 
WEIBO_struct_statuscallback_init(GET_MEDIA_SHORTURL_BATCH)
{

}
WEIBO_struct_bodycallback_init(GET_MEDIA_SHORTURL_BATCH)
{
	printf("   GET_MEDIA_SHORTURL_BATCH : \n    %s \n\n" , body );

	//int iOutCount = 0;
	//wbParserNS::t_wbParse_Media_ShortUrlBatch *pRet = NULL;
	//wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC( MEDIA,SHORTURL,body,len,iOutCount,(void**)&pRet,NULL,NULL );
	//if( handle ){
	//	wbParserNS::USE_WBFREE_FUNC( MEDIA,SHORTURL,handle);
	//}
}

//-------------------------------------------------------------------------------------------//

static
struct t_wb_callback_byloach callback_byloach[  WEIBO_OPTION(LAST) ] =
{
#define INIT_CALLBACK_BYLOACH(NAME) {  WEIBO_struct_statuscallback_init_fun(NAME) , WEIBO_struct_bodycallback_init_fun(NAME) , NULL}
#define INIT_CALLBACK_BYWELBON(NAME) { WEIBO_struct_statuscallback_init_fun(NAME) , WEIBO_struct_bodycallback_init_fun(NAME) , NULL}

	{0,0,0},
	INIT_CALLBACK_BYLOACH(BASE),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_PUBLIC_TIMELINE),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_FRIENDS_TIMELINE),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_USE_TIMELINE),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_MENTIONS),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_COMMENTS_TIMELINE),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_COMMENTS_BYME),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_COMMENTS_TOME),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_COMMENTS_LIST),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_COMMENTS_COUNTS),
	INIT_CALLBACK_BYLOACH(GETSTATUSES_UNREAD),
	INIT_CALLBACK_BYLOACH(PUTSTATUSES_RESET_COUNT),

	//微博访问
	INIT_CALLBACK_BYLOACH(GETSTATUSES_SHOW),
	INIT_CALLBACK_BYLOACH(GOTOSTATUSES_ID),
	INIT_CALLBACK_BYLOACH(PUTSTATUSES_UPDATE),
	INIT_CALLBACK_BYLOACH(PUTSTATUSES_UPLOAD),
	INIT_CALLBACK_BYLOACH(PUTSTATUSES_DESTROY),
	INIT_CALLBACK_BYLOACH(PUTSTATUSES_REPOST),
	INIT_CALLBACK_BYLOACH(PUTSTATUSES_COMMENT),
	INIT_CALLBACK_BYLOACH(PUTSTATUSES_COMMENT_DESTROY),
	INIT_CALLBACK_BYLOACH(PUTSTATUSES_REPLY),

	//用户
	INIT_CALLBACK_BYLOACH(GETUSER_INFO),
	INIT_CALLBACK_BYLOACH(GETFRINDS_LIST),
	INIT_CALLBACK_BYLOACH(GETFOLLOWERS_LIST),

	//私信
	INIT_CALLBACK_BYLOACH(GETDIRECTMSG),
	INIT_CALLBACK_BYLOACH(GETDIRESTMSG_SENT),
	INIT_CALLBACK_BYLOACH(PUTDIRECTMSG_NEW),
	INIT_CALLBACK_BYLOACH(PUTDIRECTMSG_DESTROY),
	INIT_CALLBACK_BYLOACH(GETDIRECTMSG_WITH),

	//关注
	INIT_CALLBACK_BYLOACH(PUTFRIENDSHIPS_CREATE),
	INIT_CALLBACK_BYLOACH(PUTFRIENDSHIPS_CREATE_BATCH),
	INIT_CALLBACK_BYLOACH(PUTFRIENDSHIPS_DESTROY),
	INIT_CALLBACK_BYLOACH(GETFRIENDSHIPS_EXISTS),
	INIT_CALLBACK_BYLOACH(GETFRIENDSHIPS_BATCH_EXISTS),

	//Social Graph
	INIT_CALLBACK_BYLOACH(GETFRIEND_IDS),
	INIT_CALLBACK_BYLOACH(GETFOLLOWER_IDS),

	//账号
	INIT_CALLBACK_BYLOACH(GETACCOUNT_VERIFY),
	INIT_CALLBACK_BYLOACH(GETACCOUNT_RATELIMIT),
	INIT_CALLBACK_BYLOACH(PUTACCOUNT_QUITSESSION),
	INIT_CALLBACK_BYLOACH(PUTACCOUNT_UPDATE_PROFILEIMAGE),
	INIT_CALLBACK_BYLOACH(PUTACCOUNT_UPDATE_PROFILE),
	INIT_CALLBACK_BYLOACH(PUTACCOUNT_REGISTER),

	// 收藏
	INIT_CALLBACK_BYLOACH(GETFAVORITES),
	INIT_CALLBACK_BYLOACH(PUTFAVORITES_CREATE),
	INIT_CALLBACK_BYLOACH(PUTFAVORITES_DESTROY),

	//登录/OAuth
	INIT_CALLBACK_BYLOACH(OAUTH_REQUEST_TOKEN),
	INIT_CALLBACK_BYLOACH(OAUTH_AUTHORIZE),
	INIT_CALLBACK_BYLOACH(OAUTH_ACCESS_TOKEN),

	INIT_CALLBACK_BYLOACH(GET_EMOTIONS),
	INIT_CALLBACK_BYLOACH(GET_USERS_SEARCH),
	INIT_CALLBACK_BYLOACH(GET_WB_SEARCH),
	INIT_CALLBACK_BYLOACH(GET_STATUSES_SEARCH),
	INIT_CALLBACK_BYLOACH(GET_PROVINCES),

	// COOKIE
	{ WEIBO_struct_statuscallback_init_fun(COOKIE) , WEIBO_struct_bodycallback_init_fun(COOKIE) , WEIBO_struct_headercallback_init_fun(COOKIE)},
	INIT_CALLBACK_BYLOACH(UPDATETGT),// UPDATETGT,

	//自定义URL
	INIT_CALLBACK_BYLOACH(CUSTOM),

	//
	INIT_CALLBACK_BYWELBON(GET_USERS_HOT),// 获取系统推荐用户
	INIT_CALLBACK_BYWELBON(POST_USERS_REMARK),//更新修改当前登录用户所关注的某个好友的备注信息New!
	INIT_CALLBACK_BYWELBON(GET_USERS_SUGGESTIONS), //Users/suggestions 返回当前用户可能感兴趣的用户

	// 话题接口 ,by welbon,2011-01-10
	INIT_CALLBACK_BYWELBON(GET_TRENDS),//trends 获取某人的话题
	INIT_CALLBACK_BYWELBON(GET_TRENDS_STATUSES),//trends/statuses 获取某一话题下的微博
	INIT_CALLBACK_BYWELBON(POST_TRENDS_FOLLOW),//trends/follow 关注某一个话题
	INIT_CALLBACK_BYWELBON(DELETE_TRENDS_DESTROY),//trends/destroy 取消关注的某一个话题
	INIT_CALLBACK_BYWELBON(GET_TRENDS_HOURLY),//trends/destroy 按小时返回热门话题
	INIT_CALLBACK_BYWELBON(GET_TRENDS_DAYLIY),//trends/daily 按日期返回热门话题。返回某一日期的话题
	INIT_CALLBACK_BYWELBON(GET_TRENDS_WEEKLIY),//trends/weekly 按周返回热门话题。返回某一日期之前某一周的话题

	// 黑名单接口 ,by welbon,2011-01-10
	INIT_CALLBACK_BYWELBON(POST_BLOCKS_CREATE),//将某用户加入黑名单
	INIT_CALLBACK_BYWELBON(POST_BLOCKS_DESTROY),//将某用户移出黑名单
	INIT_CALLBACK_BYWELBON(GET_BLOCKS_EXISTS),//检测某用户是否是黑名单用户
	INIT_CALLBACK_BYWELBON(GET_BLOCKS_BLOCKING),//列出黑名单用户(输出用户详细信息)
	INIT_CALLBACK_BYWELBON(GET_BLOCKS_BLOCKING_IDS),//列出分页黑名单用户（只输出id）

	//用户标签接口 ,by welbon,2011-01-10
	INIT_CALLBACK_BYWELBON(GET_TAGS),//tags 返回指定用户的标签列表
	INIT_CALLBACK_BYWELBON(POST_TAGS_CREATE),//tags/create 添加用户标签
	INIT_CALLBACK_BYWELBON(GET_TAGS_SUGGESTIONS),//tags/suggestions 返回用户感兴趣的标签
	INIT_CALLBACK_BYWELBON(POST_TAGS_DESTROY),//tags/destroy 删除标签
	INIT_CALLBACK_BYWELBON(POST_TAGS_DESTROY_BATCH),//tags/destroy_batch 批量删除标签

	// 邀请接口
	INIT_CALLBACK_BYWELBON(POST_INVITE_MAILCONTACT),//邀请邮箱联系人
	INIT_CALLBACK_BYWELBON(POST_INVITE_MSNCONTACT), //邀请MSN联系人
	INIT_CALLBACK_BYWELBON(POST_INVITE_SENDMAILS),  //发送邀请邮件

	// media
	INIT_CALLBACK_BYWELBON(GET_MEDIA_SHORTURL_BATCH),// 批量获取短链接
    //登录/XAuth
	INIT_CALLBACK_BYLOACH(XAUTH_ACCESS_TOKEN),

};

struct t_wb_callback_byloach* Wb_get_struct_callback( int option )
{
	if( WEIBO_OPTION(UNK) >= option ||
		WEIBO_OPTION(LAST) <= option )
	{
		option = 0;
	}
	return &callback_byloach[option];
}