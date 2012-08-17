/**
*  @brief struct define,for Unit test.
*  @file  WeiboParseStruct.h
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


#pragma once
#ifndef __WB_PARSER_STRUCT_DEFINE_H__
#define __WB_PARSER_STRUCT_DEFINE_H__

#include <string.h>
#include <stdlib.h>

/**
* @brief 一些结构的定义
* 
* @author welbon
*
*/

#define WTRUE  true
#define WFALSE false

#ifndef BUILD_CHARACTER_SET
typedef char WBParseCHAR;
#	define _WBC(c) c
#	define WBPARSER_COPY(dst,size,source,srcsize) strncpy(dst,source,size)
#else

typedef TCHAR WBParseCHAR;
//
#	define _WBC(c) _T(c)
#	define WBPARSER_COPY(dst,size,source,srcsize) _tcsncpy_s(dst,size,source,srcsize)

#endif // BUILD_CHARACTER_SET

typedef int   WBParseINT;
typedef long  WBParseLONG;
typedef bool  WBParseBOOL;


namespace wbParserNS
{

#define WBPARSER_UINT_DEFINE(name,val)     const unsigned int WBPARSER_##name##_LEN = val
#define WBPARSER_USE_LEN(name)             WBPARSER_##name##_LEN
#define WBPARSER_REAL_LEN(name)            WBPARSER_##name##_LEN + 1 ///< 实际的长度
#define WBPARSER_OPTION_TYPE(name)         WBPT_OPTION_TYPE_##name   ///< 解析的类型
#define WBPARSER_INFOR_TYPE(name)		   WBPT_INFOR_TYPE_##name    ///< 结构的类型


	// 基本信息
	WBPARSER_UINT_DEFINE(created_at,255);		    	 ///< 创建时间
	WBPARSER_UINT_DEFINE(id,64);				    	 ///< 微博ID
	WBPARSER_UINT_DEFINE(sender_id,64);			    	 ///< 发送人UID
	WBPARSER_UINT_DEFINE(recipient_id,64);				 ///< 接受人UID
	WBPARSER_UINT_DEFINE(text,2048);					 ///< 微博信息内容
	WBPARSER_UINT_DEFINE(source,255);				     ///< 微博来源
	WBPARSER_UINT_DEFINE(in_reply_to_status_id,64);     ///< 回复ID
	WBPARSER_UINT_DEFINE(in_reply_to_user_id  ,64);     ///< 回复人UID
	WBPARSER_UINT_DEFINE(in_reply_to_screen_name,255);   ///< 回复人昵称
	WBPARSER_UINT_DEFINE(thumbnail_pic,1024);            ///< 缩略图
	WBPARSER_UINT_DEFINE(bmiddle_pic,1024);              ///< 中型图片
	WBPARSER_UINT_DEFINE(original_pic,1024);             ///< 原始图片

	WBPARSER_UINT_DEFINE(screen_name,255);             ///< 微博昵称
	WBPARSER_UINT_DEFINE(name,255);                    ///< 友好显示名称


	WBPARSER_UINT_DEFINE(province,128);
	WBPARSER_UINT_DEFINE(city,128);
	WBPARSER_UINT_DEFINE(location,128);
	WBPARSER_UINT_DEFINE(description,1024);
	WBPARSER_UINT_DEFINE(url,1024);
	WBPARSER_UINT_DEFINE(profile_image_url,1024);
	WBPARSER_UINT_DEFINE(domain,1024);
	WBPARSER_UINT_DEFINE(gender,32);

	WBPARSER_UINT_DEFINE(created_time,255);
	WBPARSER_UINT_DEFINE(following,32);
	WBPARSER_UINT_DEFINE(verified,32);

	WBPARSER_UINT_DEFINE(geo,255);
	WBPARSER_UINT_DEFINE(type,255);
	WBPARSER_UINT_DEFINE(coordinates,255);

	//
	WBPARSER_UINT_DEFINE(status,255);
	WBPARSER_UINT_DEFINE(reset_time,255);

	// using in emotion
	WBPARSER_UINT_DEFINE(phrase,255);
	WBPARSER_UINT_DEFINE(category,255);


	//
	WBPARSER_UINT_DEFINE(request,255);
	WBPARSER_UINT_DEFINE(error,255);
	WBPARSER_UINT_DEFINE(error_code,64);
	WBPARSER_UINT_DEFINE(TGT,255);
	WBPARSER_UINT_DEFINE(GROUPNAME,255);
	WBPARSER_UINT_DEFINE(email,255);
	// Tags
	WBPARSER_UINT_DEFINE(TAGS_ID,64);
	WBPARSER_UINT_DEFINE(TAGS_NAME,255);
	//Trend
	WBPARSER_UINT_DEFINE(TREND_ID,64);
	WBPARSER_UINT_DEFINE(TREND_NAME,255);
	

	struct t_wbParse_Status;
	struct t_wbParse_User;
	struct t_wbParse_Geo;
	struct t_wbParse_Comment;
	struct t_wbParse_CommentCounts ;
	struct t_wbParse_ShipshowBase ;
	struct t_wbParse_Shipshow ;
	struct t_wbParse_LimitStatus ;
	struct t_wbParse_Error;
	struct t_wbParse_Unread ;
	struct t_wbParse_DirectMessage ;

	typedef enum 
	{
		EPS_OPENAPI ,  ///< from OpenAPI Http 
		EPS_STREAM  ,  ///< from STREAM
		EPS_XMPP    ,  ///< from xmpp

	} eParseSource;

	/** 解析信息头，每个结构体必须要有的 by welbon,2011-01-08*/
	struct t_wbParse_Header
	{
		int eSource;///< 信息来源
	};
	

	/// 认证返回信息
	struct t_wbParse_Oauth
	{
		t_wbParse_Header ph_; ///< 解析头

		char oauth_token[ 257 ];
		char oauth_token_secret[ 257 ];

		char uid_[128];

		//  从官方网页获取到 认证返回的验证码 ，为了oauth第三步 ,获取授权过的Access Token
		char oauth_verifier[ 65 ];
	};

	/** cookie 信息 */
	struct t_wbParse_Cookie
	{
		t_wbParse_Header ph_; ///< 解析头

		// cookie
		int  error;
		char SUE[1024];
		char SUP[2048];
		char tgt[WBPARSER_REAL_LEN(TGT)];
		//
		char ticket[256];
		char publickey[512];
		char keyversion[64];
		char uid[128];

		__int64 lservertime;//获取TGT时服务器时间
	    __int64 llocaltime;//TGT返回时本机时间
	};


	/** 更新TGT结果 */
	struct t_wbParse_updateTGT
	{
		t_wbParse_Header ph_; ///< 解析头

		int  error;
		char SUE[1024];
		char SUP[2048];
		char tgt[WBPARSER_REAL_LEN(TGT)];///< TGT	
	};

	struct t_wbParse_Cursor
	{
		t_wbParse_Header ph_; ///< 解析头

		long long nextcur;
		long long prevcur;
	};

	/** 用户信息 */
	struct t_wbParse_User 
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						///< 用户UID
		WBParseCHAR screen_name[WBPARSER_REAL_LEN(screen_name)];	///< 微博昵称
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];					///< 友好显示名称，如Tim Yang(此特性暂不支持)
		WBParseCHAR province[WBPARSER_REAL_LEN(province)];			///< 省份编码（参考省份编码表）
		WBParseCHAR city[WBPARSER_REAL_LEN(city)];					///< 城市编码（参考城市编码表）
		WBParseCHAR location[WBPARSER_REAL_LEN(location)];			///< 地址
		WBParseCHAR description[WBPARSER_REAL_LEN(description)];	///< 个人描叙
		WBParseCHAR url[WBPARSER_REAL_LEN(url)];					///< 用户博客地址
		WBParseCHAR domain[WBPARSER_REAL_LEN(domain)];				///< 用户个性化域名
		WBParseCHAR gender[WBPARSER_REAL_LEN(gender)];				///< 性别,m--男，f--女,n--未知
		WBParseCHAR profile_image_url[WBPARSER_REAL_LEN(profile_image_url)];///< 自定义图像
		WBParseCHAR email[WBPARSER_REAL_LEN(email)];				///<邮箱

		WBParseINT followers_count;    ///< 粉丝数
		WBParseINT friends_count;      ///< 关注数
		WBParseINT statuses_count;     ///< 微博数
		WBParseINT favourites_count;   ///< 收藏数


		WBParseBOOL geo_enabled; ///< 地理位置开关
		WBParseBOOL allow_all_act_msg;
		WBParseBOOL following;  ///< 是否已关注(此特性暂不支持)
		WBParseBOOL verified;   ///< 加V标示

		WBParseCHAR created_at[WBPARSER_REAL_LEN(created_at)];///< 创建时间

		t_wbParse_Status *last_status; ///< 最后一条信息
	};

	/** 地址位置信息 */
	struct t_wbParse_Geo
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR type[WBPARSER_REAL_LEN(type)]; //
		WBParseCHAR coordinates[WBPARSER_REAL_LEN(coordinates)];
	};

	/** 状态信息 */
	struct t_wbParse_Status
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR created_at[WBPARSER_REAL_LEN(created_at)];   ///<创建时间
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];					   ///<微博ID
		WBParseCHAR text[WBPARSER_REAL_LEN(text)];				   ///<微博信息内容
		WBParseCHAR source[WBPARSER_REAL_LEN(source)];			   ///<微博来源

		WBParseBOOL favorited;									   ///<是否已收藏
		WBParseBOOL truncated;                                     ///<是否被截断


		WBParseCHAR in_reply_to_status_id[WBPARSER_REAL_LEN(in_reply_to_status_id)];   ///<回复ID
		WBParseCHAR in_reply_to_user_id[WBPARSER_REAL_LEN(in_reply_to_user_id)];     ///<回复人UID
		WBParseCHAR in_reply_to_screen_name[WBPARSER_REAL_LEN(in_reply_to_screen_name)]; ///<回复人昵称

		t_wbParse_Geo    *geo; ///< 地理位置

		WBParseCHAR thumbnail_pic[WBPARSER_REAL_LEN(thumbnail_pic)];         ///<缩略图
		WBParseCHAR bmiddle_pic[WBPARSER_REAL_LEN(bmiddle_pic)];             ///<中型图片
		WBParseCHAR original_pic[WBPARSER_REAL_LEN(original_pic)];           ///<原始图片

		t_wbParse_User   *user;//作者信息
		t_wbParse_Status *retweeted_status ; //转发的博文，内容为status，如果不是转发，则没有此字段
	};

	/** 评论信息 */
	struct t_wbParse_Comment
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR id[WBPARSER_REAL_LEN(id)];///< 评论ID
		WBParseCHAR text[WBPARSER_REAL_LEN(text)];///< 评论内容
		WBParseCHAR source[WBPARSER_REAL_LEN(source)];///<评论来源

		//
		WBParseBOOL favorited;///<是否收藏
		WBParseBOOL truncated;///<是否被截断

		WBParseCHAR created_at[WBPARSER_REAL_LEN(created_at)];///<评论时间

		t_wbParse_User    *user;     ///<评论人信息,结构参考user
		t_wbParse_Status  *status;   ///<评论的微博,结构参考status
		t_wbParse_Comment *reply_comment;///<评论来源，数据结构跟comment一致
	};

	/** 评论计数 */
	struct t_wbParse_CommentCounts
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR id[WBPARSER_REAL_LEN(id)];///< 评论ID
		WBParseINT  comments;  ///<评论数
		WBParseINT  rt;        ///<转发数
	};

	/** 未读消息数 */
	struct t_wbParse_Unread
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseINT comments; ///<未读评论
		WBParseINT mentions; ///<@我的
		WBParseINT dm;       ///<私信
		WBParseINT followers;///<//新粉丝数
		WBParseINT new_status;///< new weibo
	};

	/** 私信信息 */
	struct t_wbParse_DirectMessage
	{
		t_wbParse_Header ph_;

		WBParseCHAR id[WBPARSER_REAL_LEN(id)];///<私信ID
		WBParseCHAR text[WBPARSER_REAL_LEN(text)];///< 私信内容
		WBParseCHAR sender_id[WBPARSER_REAL_LEN(sender_id)];///<发送人UID
		WBParseCHAR recipient_id[WBPARSER_REAL_LEN(recipient_id)];///< 接受人UID
		WBParseCHAR created_at[WBPARSER_REAL_LEN(created_at)];///<发送时间
		WBParseCHAR sender_screen_name[WBPARSER_REAL_LEN(screen_name)];///<发送人昵称
		WBParseCHAR recipient_screen_name[WBPARSER_REAL_LEN(screen_name)];///<接受人昵称

		t_wbParse_User *sender;///< 发送人信息，参考user说明
		t_wbParse_User *recipient;///<接受人信息，参考user说明
	};

	/** 好友关系 */
	struct t_wbParse_Shipshow
	{
		t_wbParse_Header ph_; ///< 解析头

		struct Item
		{
			WBParseCHAR id[WBPARSER_REAL_LEN(id)];
			WBParseCHAR screen_name[WBPARSER_REAL_LEN(screen_name)];

			WBParseBOOL following ;
			WBParseBOOL followed_by ;
			WBParseBOOL notifications_enabled ;
		};
		Item *source;
		Item *target;
	};

	/** 频率限制 */
	struct t_wbParse_LimitStatus
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseINT  hourly_limit;
		WBParseINT  reset_time_in_seconds;
		WBParseINT  remaining_hits;
		WBParseCHAR reset_time[WBPARSER_REAL_LEN(reset_time)];
	};

	/** 表情 */
	struct t_wbParse_Emotion
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR phrase[WBPARSER_REAL_LEN(phrase)];
		WBParseCHAR type[WBPARSER_REAL_LEN(type)];
		WBParseCHAR url	[WBPARSER_REAL_LEN(url)];
		WBParseCHAR category[WBPARSER_REAL_LEN(category)];

		//
		WBParseBOOL is_hot;
		WBParseBOOL is_common;
		WBParseINT	order_number;
	};

	/** 错误 */
	struct t_wbParse_Error 
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR request[WBPARSER_REAL_LEN(request)];
		WBParseCHAR error[WBPARSER_REAL_LEN(error)];
		WBParseCHAR error_code[WBPARSER_REAL_LEN(error_code)];
		unsigned int error_sub_code; ///< 子错误信息
	};

	/** 为富媒体而定 */
	struct t_wbParse_media_item
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR id[WBPARSER_REAL_LEN(id)];			 ///<mediaid
		WBParseCHAR url[WBPARSER_REAL_LEN(url)];		 ///<media- url
		WBParseCHAR screen[WBPARSER_REAL_LEN(url)];		 ///<media- screen
		WBParseCHAR type[WBPARSER_REAL_LEN(type)];		 ///<media- type:webpage,video,audio,event,magic,vote
		WBParseCHAR title[WBPARSER_REAL_LEN(name)];		 ///<media- title
		WBParseCHAR flash[WBPARSER_REAL_LEN(url)];		 ///<media- flash
		//
		struct MediaExternItem
		{
			// extern information
			WBParseCHAR ext_from[32]; 
			WBParseCHAR ext_shorturl[WBPARSER_REAL_LEN(url)];	 ///<media- shorturl
			WBParseCHAR ext_description[WBPARSER_REAL_LEN(text)];///<media- description
			WBParseCHAR ext_lastmodified[ WBPARSER_REAL_LEN(created_time) ];///<media- lastmodify

			// extern - music 
			WBParseCHAR ext_audio_artist[WBPARSER_REAL_LEN(name)];
			WBParseCHAR ext_audio_album[WBPARSER_REAL_LEN(name)];
			WBParseCHAR ext_audio_appkey[256];

			// extern - event
			WBParseCHAR ext_event_id[WBPARSER_REAL_LEN(id)];

			// extern - vote
			WBParseCHAR ext_vote_uid[WBPARSER_REAL_LEN(id)];

		} media_ext;
	};

#ifdef _USE_GET_SHORTURL_BATCH

	/** 批量获取 */
	struct t_wbParse_Media_ShortUrlBatch
	{
		t_wbParse_Header ph_; ///< 解析头

		struct ItemAnnotions
		{
			WBParseCHAR url[ WBPARSER_REAL_LEN(url) ];
			WBParseCHAR pic[ WBPARSER_REAL_LEN(url) ];
			WBParseCHAR title[ WBPARSER_REAL_LEN(name) ];
			WBParseINT  from;
		};

		WBParseINT  type_ ;
		WBParseCHAR urlshort_[ WBPARSER_REAL_LEN(url) ];
		WBParseCHAR urllong_[ WBPARSER_REAL_LEN(url) ];
		WBParseCHAR title_[ WBPARSER_REAL_LEN(name) ];
		WBParseCHAR	description_[ WBPARSER_REAL_LEN(text) ];
		WBParseCHAR lastmodified_[ WBPARSER_REAL_LEN(created_time) ];///<last - modified

		WBParseINT anCounts_;
		ItemAnnotions *annotions_;
	};
#endif //_USE_GET_SHORTURL_BATCH

	/** 上传图片 */
	struct t_wbParse_UploadPic
	{
		t_wbParse_Header ph_; ///< 解析头
		//
		WBParseCHAR pic_id[ WBPARSER_REAL_LEN(id) ];
		WBParseCHAR thumbnail_pic[ WBPARSER_REAL_LEN(url) ];
		WBParseCHAR bmiddle_pic[ WBPARSER_REAL_LEN(url) ];
		WBParseCHAR original_pic[ WBPARSER_REAL_LEN(url) ];
	};

	/** 相对微博而定 */
	struct t_wbParse_Media
	{
		t_wbParse_Header ph_; ///< 解析头

		int cmd;
		WBParseCHAR parentid[WBPARSER_REAL_LEN(id)];///<父微博ID（包含这条微博的父微博id)
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];///< 当前微博ID

		t_wbParse_media_item mitem;
	};


	/** IDs列表,by welbon,2010-12-29 */
	struct t_wbParse_IDs
	{
		long long id;
	};



	/** 一般的结果 */
	struct t_wbParse_Ret
	{
		t_wbParse_Header ph_; ///< 解析头

		//
		WBParseCHAR error_code_[WBPARSER_REAL_LEN(error_code)];
	};

	/** 邀请联系人 */
	struct t_wbParse_InviteContact 
	{
		t_wbParse_Header ph_; ///< 解析头

		/** 邀请的用户 */
		struct ItemUsr
		{
			WBParseCHAR uid_[ WBPARSER_USE_LEN(id) ];
			WBParseCHAR name_[ WBPARSER_USE_LEN(screen_name) ];
			WBParseCHAR iconurl_[ WBPARSER_USE_LEN(url) ];
			WBParseCHAR email_[ WBPARSER_USE_LEN(email) ];
		};
		struct ItemGroup
		{
			WBParseCHAR groupname_[ WBPARSER_REAL_LEN(GROUPNAME) ];
			ItemUsr *usrs_;
			int count_;
		};

		// 正在关注的用户列表
		int in_attend_usr_count_;
		ItemUsr *in_attend_usr_; 

		// 没有关注的用户(分组)
		int not_attend_group_count_;
		ItemGroup *not_attend_group_;

		// 未开通微博的用户(分组)
		int out_attend_group_count_;
		ItemGroup *out_attend_group_;
		//

		WBParseCHAR error_code_[ WBPARSER_REAL_LEN(error_code) ];
	};

	/** 用户TAG */
	struct t_wbParse_Tag
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR tagId_[ WBPARSER_REAL_LEN(TAGS_ID) ];
		WBParseCHAR tagName_[ WBPARSER_REAL_LEN(TAGS_NAME) ];
	};

	/** 话题 */
	struct t_wbParse_Trend
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR trendId_[ WBPARSER_REAL_LEN(TREND_ID)  ];
		WBParseCHAR trendNumber_[ WBPARSER_REAL_LEN(TREND_ID) ];
		WBParseCHAR trendHotword_[ WBPARSER_REAL_LEN(TREND_NAME) ];

	};

	/** 话题热点查询结构 */
	struct t_wbParse_TrendHotQuery
	{
		struct TrendItem {
			WBParseCHAR trendName_[	WBPARSER_REAL_LEN(TREND_NAME) ];
			WBParseCHAR trendQuery_[ WBPARSER_REAL_LEN(TREND_NAME) ];
		} ;

		int         trend_counts_;
		TrendItem  *trends_;
		WBParseCHAR time_[ WBPARSER_REAL_LEN(created_time) ];
	};

	/** 注册结果 */
	struct t_wbParse_Register_Ret
	{
		WBParseCHAR uid_[ WBPARSER_USE_LEN(id) ];
	};


	const long long  llMAX = 0xFFFFFFFFFFFFFFFF;
	/** 简单的 */
	struct t_wbParser_MMId
	{
		enum eType
		{
			EType_Unknow = -1,

			/** 我的评论 */
			EType_CommentsToMe = 0,

			/** 我发出去的 */
			EType_CommentsByMe,

			/** 我发与收到 */
			EType_CommentsTimeline,

			/** @我的 */
			EType_AtMe,

			/** 首页 */
			EType_Homes,

			/** 私信 */
			EType_DirectMsgs,

			/** 粉丝*/
			EType_Follower,

			EType_End,
		};
		struct IdValue
		{
			long long maxId;
			long long minId;
		};

		struct IdValue _idVal[EType_End];

		/** 是否需要加锁，防止非法访问？*/
	};
}

#endif //__WB_PARSER_STRUCT_DEFINE_H__
