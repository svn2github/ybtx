#pragma once
#ifndef __WB_PARSE_WRAPPER_H__
#define __WB_PARSE_WRAPPER_H__

#include "wbParseStruct.h"

//#if (defined(WIN32) || defined(_WIN32) ) && !defined(WEIBO_PARSER_STATICLIB)
//#	if defined(BUILDING_LIBWBPARSER)
//#	define WBPARSER_API  __declspec(dllexport)
//#	else
//#	define WBPARSER_API  __declspec(dllimport)
//#	endif
//#else
// 其他平台的，或者是静态链接
#	define WBPARSER_API
//#endif


#define WBPARSE_FUNC_NAME(interfaceType,interfaceName) Parse_##interfaceType##_##interfaceName
#define WBFREE_FUNC_NAME(interfaceType,interfaceName)  Free_##interfaceType##_##interfaceName

// Macro for Parsing
#define WBPARSE_FUNC_DECLARE(interfaceType,interfaceName)\
	WBPARSE_FUNC_NAME(interfaceType,interfaceName)(const char* sc,const int len,int &iOutCount,void **ppout,t_WBParse_Extern_info *pExternInfo,t_wbParse_Cursor **ppCursor)

#define WBPARSE_FUNC_IMPLEMENTS(interfaceType,interfaceName)\
	WBPARSE_FUNC_NAME(interfaceType,interfaceName)(const char* sc,const int len,int &iOutCount,void **ppout,t_WBParse_Extern_info *pExternInfo,t_wbParse_Cursor **ppCursor )

// Macro for Free 
#define WBFREE_FUNC_DECLARE(interfaceType,interfaceName)\
	WBFREE_FUNC_NAME(interfaceType,interfaceName)(WBPARSE_HANDLE handle)

#define WBFREE_FUNC_IMPLEMENTS(interfaceType,interfaceName)\
	WBFREE_FUNC_NAME(interfaceType,interfaceName)(WBPARSE_HANDLE handle)


// Macro for using
#define USE_WBPARSE_FUNC(interfaceType,interfaceName,sc,len,iOutCount,ppout,pExternInfo,ppCursorInfo)\
	WBPARSE_FUNC_NAME(interfaceType,interfaceName)(sc,len,iOutCount,ppout,pExternInfo,ppCursorInfo);

#define USE_WBFREE_FUNC(interfaceType,interfaceName,handle)\
	WBFREE_FUNC_NAME(interfaceType,interfaceName)(handle)

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus


	namespace wbParserNS
	{

		typedef void* WBPARSE_HANDLE;

		/** 回传的扩展信息 */
		struct t_WBParse_Extern_info
		{
			struct t_Media
			{
				int				 nCmd;		///< Command information
				int 			 iMediaSize;///< media size
				t_wbParse_Media* pMedia;	///< media信息
			} _Media;

			struct t_MMId
			{
				int              iMMIDtype; ///< MMID type
				t_wbParser_MMId* _pMMId;    ///< MMID
			} _MMId;
		};
		//下行数据集


		/** 2.最新的公共微博消息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,Public_Timeline);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,Public_Timeline);

		/** 3.当前登录用户及其所关注用户的最新微博消息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Statuses,Friends_Timeline);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Statuses,Friends_Timeline);

		/** 4.当前登录用户发布的微博消息列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,User_Timeline);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,User_Timeline);

		/** 5.@当前用户的微博列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,Metions);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,Metions);


		/** 6.当前用户发送及收到的评论列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,CommentsTimeLine);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,CommentsTimeLine);

		/** 7.获取当前用户发出的评论 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,Comments_BYME);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,Comments_BYME);

		/** 8.获取当前用户收到的评论 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,Comments_TOME);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,Comments_TOME);

		/** 9.根据微博消息ID返回某条微博消息的评论列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,Comments);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,Comments);

		/** 10.批量获取一组微博的评论数及转发数 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,CommentCounts);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,CommentCounts);

		/** 11.获取当前用户未读消息数 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,Unread);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,Unread);

		/** 未读消息数清零接口 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,ResetCount);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,ResetCount);

		/** 表情接口，获取表情列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Emotions,Timeline);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Emotions,Timeline);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//微博访问接口


		/** 根据ID获取单条微博信息内容 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Statuses,Show);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Statuses,Show);

		/** 发布一条微博信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Statuses,Update);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Statuses,Update);

		/** 上传图片并发布一条微博信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Statuses,Upload);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Statuses,Upload);

		/** 上传图片 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Statuses,Upload_Pic);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Statuses,Upload_Pic);

		/** 删除一条微博信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Statuses,Destroy);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Statuses,Destroy);

		/** 对一条微博信息进行评论 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Statuses,Comment);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Statuses,Comment);

		/** 删除当前用户的微博评论信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Statuses,Comment_Destroy);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Statuses,Comment_Destroy);

		/** 批量删除当前用户的微博评论信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Statuses,Comment_DestroyBatch);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Statuses,Comment_DestroyBatch);

		/** 回复微博评论信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,Comment_Reply);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,Comment_Reply);

		/** 转发一条微博 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,Repost);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,Repost);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 用户接口

		/** 根据用户ID获取用户资料（授权用户） */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( User,Show);
		WBPARSER_API void WBFREE_FUNC_DECLARE( User,Show);

		/** 获取用户关注列表及每个关注用户最新一条微博  */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,Friends);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,Friends);

		/** 获取用户粉丝列表及及每个粉丝用户最新一条微博 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Statuses,FollowerList);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Statuses,FollowerList);

		/** 获取系统推荐的用户 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Users,HotUser);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Users,HotUser);

		/** 更新当前登录用户所关注的某个好友的备注信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Users,Friends_UpdateRemark);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Users,Friends_UpdateRemark);

		/** 更新当前登录用户所关注的某个好友的备注信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Users,Suggestions);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Users,Suggestions);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 私信接口

		/*获取当前用户最新私信列表*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(DirectMessages,Timeline);
		WBPARSER_API void WBFREE_FUNC_DECLARE(DirectMessages,Timeline);

		/** 获取当前用户发送的最新私信列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(DirectMessages,Sent);
		WBPARSER_API void WBFREE_FUNC_DECLARE(DirectMessages,Sent);

		/** 发送一条私信 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(DirectMessages,New);
		WBPARSER_API void WBFREE_FUNC_DECLARE(DirectMessages,New);

		/** 删除一条私信 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(DirectMessages,Destroy);
		WBPARSER_API void WBFREE_FUNC_DECLARE(DirectMessages,Destroy);

		/** 当前与好友的来往私信列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(DirectMessages,With);
		WBPARSER_API void WBFREE_FUNC_DECLARE(DirectMessages,With);

		/** 批量删除私信 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(DirectMessages,Destroy_Batch);
		WBPARSER_API void WBFREE_FUNC_DECLARE(DirectMessages,Destroy_Batch);

		/*当前登陆用户有私信往来的用户列表*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(DirectMessages,UserList );
		WBPARSER_API void WBFREE_FUNC_DECLARE(DirectMessages,UserList);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//关注接口

		/** 关注某用户 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Friendship,Create);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Friendship,Create);

		/** 批量关注某用户 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Friendship,CreateBatch);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Friendship,CreateBatch);

		/** 取消关注 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Friendship,Destroy);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Friendship,Destroy);

		/** 是否关注某用户(推荐使用friendships/show) */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Friendship,Exist);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Friendship,Exist);

		/** 获取两个用户关系的详细情况 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Friendship,Show);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Friendship,Show);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Social Graph接口
		//
		/*获取用户关注对象uid列表*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Friends,ids);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Friends,ids);


		/* 获取用户粉丝对象uid列表*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Follower,ids);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Follower,ids);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//隐私设置接口

		/* 设置隐私信息*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Account,UpdatePrivacy);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Account,UpdatePrivacy);

		/* 获取隐私信息*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Account,GetPrivacy);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Account,GetPrivacy);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//黑名单接口

		/** 将某用户加入黑名单 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Block,Create);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Block,Create);

		/** 将某用户移出黑名单 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Block,Destroy);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Block,Destroy);

		/** 检测某用户是否是黑名单用户 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Block,Exist);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Block,Exist);

		/* 列出黑名单用户(输出用户详细信息) */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Block,Blocking );
		WBPARSER_API void WBFREE_FUNC_DECLARE(Block,Blocking);

		/** 列出分页黑名单用户（只输出id）*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Block,Blocking_IDs);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Block,Blocking_IDs);


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//用户标签接口

		/** 返回用户指定的标签列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Tags,List);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Tags,List);

		/** 返回用户指定的标签列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Tags,Create);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Tags,Create);

		/** 返回用户感兴趣的标签 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Tags,Suggestions);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Tags,Suggestions);

		/** 删除标签 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Tags,Destroy);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Tags,Destroy);

		/** 批量删除标签 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Tags,Destroy_Batch);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Tags,Destroy_Batch);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//账号接口

		/** 验证当前用户身份是否合法 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Account,Verify_Credentials);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Account,Verify_Credentials);

		/** 获取当前用户API访问频率限制 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Account,Rate_Limit);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Account,Rate_Limit);


		/** 当前用户退出登录 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Account,EndSession);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Account,EndSession);

		/**  更改头像 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Account,Update_Profile_Image);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Account,Update_Profile_Image);

		/**更改资料*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Account,Update_Profile);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Account,Update_Profile);

		/**注册新浪微博帐号*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Account,Register);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Account,Register);

		/**二次注册微博的接口*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Account,Activate);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Account,Activate);


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 收藏接口


		/** 获取当前用户的收藏列表 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Favorites,List);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Favorites,List);

		/** 添加收藏 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Favorites,Create);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Favorites,Create);

		/** 删除收藏信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Favorites,Destroy);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Favorites,Destroy);

		/** 批量删除收藏的微博信息 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Favorites,Destroy_batch);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Favorites,Destroy_batch);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 登录/OAuth接口
		//

		/** 获取未授权的Request Token */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Oauth,RequestToken);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Oauth,RequestToken);

		/** 请求用户授权Token */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Oauth,Oauthorize);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Oauth,Oauthorize);

		/**获取授权过的Access Token*/
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Oauth,AccessToken) ;
		WBPARSER_API void WBFREE_FUNC_DECLARE(Oauth,AccessToken);



		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 微博搜索 :Search API仅对新浪合作开发者开放，申请请联系 @微博开放平台
		//

		/** 搜索微博 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Search,Statuses);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Search,Statuses);

		/** 搜索用户 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Search,Users);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Search,Users);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 话题接口 ,by welbon,2011-01-10

		/** 获取某人的话题 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Trend,Trends);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Trend,Trends);

		/** 获取某一话题下的微博 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Trend,Statuses);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Trend,Statuses);

		/** 关注某一个话题 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Trend,Follow);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Trend,Follow);

		/** 取消关注的某一个话题 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Trend,Destroy);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Trend,Destroy);

		/** 按小时返回热门话题 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Trend,Hourly);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Trend,Hourly);

		/** 按日期返回热门话题。返回某一日期的话题 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Trend,Daily);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Trend,Daily);

		/** 按周返回热门话题。返回某一日期之前某一周的话题 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(Trend,Weekly);
		WBPARSER_API void WBFREE_FUNC_DECLARE(Trend,Weekly);



		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 邀请接口

		/** 邀请邮箱联系人 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(InviteContact,Mail);
		WBPARSER_API void WBFREE_FUNC_DECLARE(InviteContact,Mail);

		/** 邀请MSN联系人 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(InviteContact,MSN);
		WBPARSER_API void WBFREE_FUNC_DECLARE(InviteContact,MSN);

		/** 发送邀请邮件 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(InviteContact,SendMail);
		WBPARSER_API void WBFREE_FUNC_DECLARE(InviteContact,SendMail);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// MEDIA 
#ifdef _USE_GET_SHORTURL_BATCH

		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MEDIA,SHORTURL );
		WBPARSER_API void WBFREE_FUNC_DECLARE( MEDIA,SHORTURL );

#endif //#ifdef _USE_GET_SHORTURL_BATCH

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 一般解析

		/** 错误解析 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(WEIBO_ERR,ITEM);
		WBPARSER_API void WBFREE_FUNC_DECLARE(WEIBO_ERR,ITEM);

		/** cookie解析 */
		WBPARSER_API void WBPARSE_FUNC_DECLARE(Cookie,BASE);

		/** 解析cookie,返回指定的串 */
		WBPARSER_API int Parse_CookieStr(const char *sc,char *token,char *secret);
	}


#ifdef __cplusplus
}
#endif //__cplusplus


#endif //__WB_PARSE_WRAPPER_H__