/**
*  @brief  Interface function implements
*  @file   weibo.h
*  @author loach 
*  @Email <loachmr@sina.com>
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


#ifndef __weibo_SDK_H__
#define __weibo_SDK_H__

/*
 *
 * 针对新浪的开放平台的接口进行C++处理(http://open.t.sina.com.cn/wiki/index.php)
 * 方便开发者调用
 *
 * 版权属于 新浪中国
 *
 * 作者：loach
 *
 * 时间：2010-06-04
 *
 * 修改： 2010-09-26
 *    1 添加 获取表情的接口
 *    2 添加 搜索接口
 *   
*/

#ifdef  __cplusplus
extern "C" {
#endif

#if (defined(WIN32) || defined(_WIN32) ) && !defined(WEIBO_STATICLIB)

#if defined(BUILDING_LIBWEIBO)
#define WEIBO_API  __declspec(dllexport)
#else
#define WEIBO_API  __declspec(dllimport)
#endif

#else
	// 其他平台的，或者是静态链接
#define WEIBO_API
#endif

#ifndef BUILD_CHARACTER_SET

	typedef char WBChar;

#else

    // for unicode 字符值
#if (defined(UNICODE) || defined(_UNICODE) )

	typedef wchar_t WBChar;

#else

	typedef char WBChar;

#endif

#endif // BUILD_CHARACTER_SET

	//
#define WB_MAX_LEN_DEFINE(name , val ) const unsigned int WB_##name##_LEN = val
#define WB_USE_LEN(name) WB_##name##_LEN

	// 变量的长度
	WB_MAX_LEN_DEFINE(UID , 128 );
	WB_MAX_LEN_DEFINE(PWD , 64 );
	WB_MAX_LEN_DEFINE(OAUTH_TOKEN,1024); // ==> cookie's SUE
	WB_MAX_LEN_DEFINE(OAUTH_TOKEN_SECRET,2048); // ==> cookie's SUP		
	WB_MAX_LEN_DEFINE(OAUTH_VERIFIER,256);
	//
	WB_MAX_LEN_DEFINE(OPT_VALUE, 64);
	WB_MAX_LEN_DEFINE(WEIBO_ID , 64);
	WB_MAX_LEN_DEFINE(WEIBO_IDS, 2048); ///< ID列表，以逗号分隔
	WB_MAX_LEN_DEFINE(WB_INFO  , 280);
	
	//
	WB_MAX_LEN_DEFINE(PROFILE_NAME, 20);
	WB_MAX_LEN_DEFINE(PROFILE_GENDER, 2);
	WB_MAX_LEN_DEFINE(PROFILE_EMAIL,100);
	WB_MAX_LEN_DEFINE(PROFILE_PROVINCE, 20);
	WB_MAX_LEN_DEFINE(PROFILE_CITY, 20);
	WB_MAX_LEN_DEFINE(PROFILE_LOCATION,30);
	WB_MAX_LEN_DEFINE(PROFILE_DESCRIPTION,160);

	// 地理信息长度
	WB_MAX_LEN_DEFINE(GEOCODE,128);

	// 时间
	WB_MAX_LEN_DEFINE(TIME,32);

	//
	WB_MAX_LEN_DEFINE(DIRECTMSG, 600);

	//
    WB_MAX_LEN_DEFINE(POST, 2048);
	// url length
	WB_MAX_LEN_DEFINE(URL , 1024);
	WB_MAX_LEN_DEFINE(MAX_PATH, 512);
	WB_MAX_LEN_DEFINE(IP, 100);

	// tags
	WB_MAX_LEN_DEFINE(SCREEN_NAME,100);
	WB_MAX_LEN_DEFINE(TAGS_INFO,2048);

	// 实际的长度
#define WB_REAL_LEN(name) WB_##name##_LEN + 1

	// 请求格式，返回数据格式
	typedef enum
	{
		E_FORMAT_JSON = 0,
		E_FORMAT_XML,
	}WEIBORequestFormat;

	// 根据 OPTION 可以对其上结构，通过weibo_opt_initstruct函数初始化
#define WEIBO_OPTION(name) WEIBO_OPT_##name
#define WEIBO_OPTION_DEFINE(name , val) WEIBO_OPTION(name) = val

	typedef enum
	{
		WEIBO_OPTION_DEFINE(UNK , 0),
		WEIBO_OPTION(BASE), //1

		//获取下行数据集(timeline)
		WEIBO_OPTION(GETSTATUSES_PUBLIC_TIMELINE), //2 获取最新更新的公共微博消息 
		WEIBO_OPTION(GETSTATUSES_FRIENDS_TIMELINE), //3 获取当前用户所关注用户的最新微博信息 (别名: statuses/home_timeline) 
		WEIBO_OPTION(GETSTATUSES_USE_TIMELINE), //4 获取用户发布的微博信息列表 
		WEIBO_OPTION(GETSTATUSES_MENTIONS), //5 获取@当前用户的微博列表 
		WEIBO_OPTION(GETSTATUSES_COMMENTS_TIMELINE), //6 获取当前用户发送及收到的评论列表
		WEIBO_OPTION(GETSTATUSES_COMMENTS_BYME), //7 获取当前用户发出的评论 
		WEIBO_OPTION(GETSTATUSES_COMMENTS_TOME), //8 获取当前用户收到的评论 
		WEIBO_OPTION(GETSTATUSES_COMMENTS_LIST), //9 获取指定微博的评论列表 
		WEIBO_OPTION(GETSTATUSES_COMMENTS_COUNTS), //10 批量获取一组微博的评论数及转发数 
		WEIBO_OPTION(GETSTATUSES_UNREAD), //11 获取当前用户未读消息数 
		WEIBO_OPTION(PUTSTATUSES_RESET_COUNT),//12 未读消息数清零接口 

		//微博访问
		WEIBO_OPTION(GETSTATUSES_SHOW), //13 根据ID获取单条微博信息内容 
		WEIBO_OPTION(GOTOSTATUSES_ID),//14 根据微博ID和用户ID跳转到单条微博页面 
		WEIBO_OPTION(PUTSTATUSES_UPDATE),//15 发布一条微博信息 
		WEIBO_OPTION(PUTSTATUSES_UPLOAD),//16 上传图片并发布一条微博信息 
		WEIBO_OPTION(PUTSTATUSES_DESTROY),//18 删除一条微博信息 
		WEIBO_OPTION(PUTSTATUSES_REPOST),//19 转发一条微博信息（可加评论） 
		WEIBO_OPTION(PUTSTATUSES_COMMENT),//20 对一条微博信息进行评论 
		WEIBO_OPTION(PUTSTATUSES_COMMENT_DESTROY),//21 删除当前用户的微博评论信息 
		WEIBO_OPTION(PUTSTATUSES_REPLY),//22 回复微博评论信息 

		//用户
		WEIBO_OPTION(GETUSER_INFO),//23 根据用户ID获取用户资料（授权用户） 
		WEIBO_OPTION(GETFRINDS_LIST),//24 获取当前用户关注对象列表及最新一条微博信息 
		WEIBO_OPTION(GETFOLLOWERS_LIST),//25 获取当前用户粉丝列表及最新一条微博信息 

		//私信
		WEIBO_OPTION(GETDIRECTMSG),//26 获取当前用户最新私信列表 
		WEIBO_OPTION(GETDIRESTMSG_SENT),//27 获取当前用户发送的最新私信列表
		WEIBO_OPTION(PUTDIRECTMSG_NEW),//28 发送一条私信 
		WEIBO_OPTION(PUTDIRECTMSG_DESTROY),//29 删除一条私信 
		WEIBO_OPTION(GETDIRECTMSG_WITH),//30获取用户往来私信列表
		
		//关注
		WEIBO_OPTION(PUTFRIENDSHIPS_CREATE),//31 关注某用户 
		WEIBO_OPTION(PUTFRIENDSHIPS_CREATE_BATCH),//32 add by welbon,2011-01-21 批量关注接口
		WEIBO_OPTION(PUTFRIENDSHIPS_DESTROY),//33 取消关注 
		WEIBO_OPTION(GETFRIENDSHIPS_EXISTS),//34 判断两个用户是否有关注关系，返回两个用户关系的详细情况
		WEIBO_OPTION(GETFRIENDSHIPS_BATCH_EXISTS),//35 批量获取一组用户是否为好友

        //Social Graph
		WEIBO_OPTION(GETFRIEND_IDS),//36 关注列表
		WEIBO_OPTION(GETFOLLOWER_IDS),// 37 粉丝列表
		
		//账号 
	    WEIBO_OPTION(GETACCOUNT_VERIFY),//38 验证当前用户身份是否合法 
		WEIBO_OPTION(GETACCOUNT_RATELIMIT),//39 获取当前用户API访问频率限制 
		WEIBO_OPTION(PUTACCOUNT_QUITSESSION),//40 当前用户退出登录 
		WEIBO_OPTION(PUTACCOUNT_UPDATE_PROFILEIMAGE),//41 更改头像
		WEIBO_OPTION(PUTACCOUNT_UPDATE_PROFILE),//42 更改资料
		WEIBO_OPTION(PUTACCOUNT_REGISTER),//43
		
		// 收藏
		WEIBO_OPTION(GETFAVORITES),//44 获取当前用户的收藏列表 
		WEIBO_OPTION(PUTFAVORITES_CREATE),//45 添加收藏 
		WEIBO_OPTION(PUTFAVORITES_DESTROY),// 46 删除当前用户收藏的微博信息 

		//登录/OAuth
		WEIBO_OPTION(OAUTH_REQUEST_TOKEN),//47 获取未授权的Request Token 
		WEIBO_OPTION(OAUTH_AUTHORIZE),//48 请求用户授权Token 
		WEIBO_OPTION(OAUTH_ACCESS_TOKEN),// 49 获取授权过的Access Token

		// 表情
		WEIBO_OPTION(GET_EMOTIONS),// 50 返回新浪微博官方所有表情、魔法表情的相关信息。包括短语、表情类型、表情分类，是否热门等。 
		
		// 用户搜索 
		WEIBO_OPTION(GET_USERS_SEARCH),// 51 搜索微博用户,返回关键字匹配的微博用户，(仅对新浪合作开发者开放) 
		
		// 微博搜索 
		WEIBO_OPTION(GET_WB_SEARCH),//52 返回关键字匹配的微博，(仅对新浪合作开发者开放) 
		WEIBO_OPTION(GET_STATUSES_SEARCH),// 53 搜索微博(多条件组合) (仅对合作开发者开放) 

		WEIBO_OPTION(GET_PROVINCES), //54 省份城市编码表 

		WEIBO_OPTION(COOKIE),// 56 cookie
		WEIBO_OPTION(UPDATETGT), // 57 更新cookie

		//自定义URL
		WEIBO_OPTION(CUSTOM),// 58

		WEIBO_OPTION(GET_USERS_HOT),//63 获取系统推荐用户
		WEIBO_OPTION(POST_USERS_REMARK),//64 更新修改当前登录用户所关注的某个好友的备注信息New!
		WEIBO_OPTION(GET_USERS_SUGGESTIONS), // 65 Users/suggestions 返回当前用户可能感兴趣的用户

		// 话题接口 ,by welbon,2011-01-10
		WEIBO_OPTION(GET_TRENDS),// 66 trends 获取某人的话题
		WEIBO_OPTION(GET_TRENDS_STATUSES),//67 trends/statuses 获取某一话题下的微博
		WEIBO_OPTION(POST_TRENDS_FOLLOW),//68 trends/follow 关注某一个话题
		WEIBO_OPTION(DELETE_TRENDS_DESTROY),//69 trends/destroy 取消关注的某一个话题
		WEIBO_OPTION(GET_TRENDS_HOURLY),//70 trends/destroy 按小时返回热门话题
		WEIBO_OPTION(GET_TRENDS_DAYLIY),//71 trends/daily 按日期返回热门话题。返回某一日期的话题
		WEIBO_OPTION(GET_TRENDS_WEEKLIY),// 72 trends/weekly 按周返回热门话题。返回某一日期之前某一周的话题

		// 黑名单接口 ,by welbon,2011-01-10
		WEIBO_OPTION(POST_BLOCKS_CREATE),//73 将某用户加入黑名单
		WEIBO_OPTION(POST_BLOCKS_DESTROY),//74 将某用户移出黑名单
		WEIBO_OPTION(GET_BLOCKS_EXISTS),//75 检测某用户是否是黑名单用户
		WEIBO_OPTION(GET_BLOCKS_BLOCKING),//76 列出黑名单用户(输出用户详细信息)
		WEIBO_OPTION(GET_BLOCKS_BLOCKING_IDS),//77 列出分页黑名单用户（只输出id）

		//用户标签接口 ,by welbon,2011-01-10
		WEIBO_OPTION(GET_TAGS),//78 tags 返回指定用户的标签列表
		WEIBO_OPTION(POST_TAGS_CREATE),//79 tags/create 添加用户标签
		WEIBO_OPTION(GET_TAGS_SUGGESTIONS),//80 tags/suggestions 返回用户感兴趣的标签
		WEIBO_OPTION(POST_TAGS_DESTROY),//81 tags/destroy 删除标签
		WEIBO_OPTION(POST_TAGS_DESTROY_BATCH),// 82 tags/destroy_batch 批量删除标签

		//关系中心
		WEIBO_OPTION(POST_INVITE_MAILCONTACT),//83 邀请邮箱联系人
		WEIBO_OPTION(POST_INVITE_MSNCONTACT), //84 邀请MSN联系人
		WEIBO_OPTION(POST_INVITE_SENDMAILS),  // 85 发送邀请邮件

		//
		WEIBO_OPTION(GET_MEDIA_SHORTURL_BATCH),///< 86 批量取得多个短链的富内容信息

		//登录/XAuth
		WEIBO_OPTION(XAUTH_ACCESS_TOKEN),//87  获取授权过的Access Token

		
		WEIBO_OPTION(LAST),
	}WEIBOoption;

	const int OPT_PARAMS_COUNTS = 10;
	/** 动态参数 */
	struct t_wb_option_params
	{
		enum
		{
			WBOPTType_unk = 0,
			WBOPTType_since_id,///< 返回微博idID比数值指定的since_id大（比since_id时间晚的）的微博信息。
			WBOPTType_max_id,///< 返回微博idID不大于指定的max_id(时间不晚于max_id)的微博信息。
			WBOPTType_count,///< 返回的最大记录数count（即页面大小）
			WBOPTType_page,///< 按页号查询 
			WBOPTType_cursor,
			WBOPTType_role,
			WBOPTType_follow,
			WBOPTType_with_new_status, ///< for 获取当前用户Web未读消息数
			WBOPTType_feature, ///< 返回指定类型的微博信息内容。
		};
		WBChar szOpt_[OPT_PARAMS_COUNTS][ WB_REAL_LEN(OPT_VALUE) ];///< 根据设置的type_，所以有 4组
	};
    

	//用户 id 值（三种表示)
	struct t_wb_uid
	{
		// uidtype 类型,用户 id 值（三种表示)
		enum
		{ 
			IDType_unk=0,
			IDType_id,
			IDType_screenname,
			IDType_userid,
		};
		int uidtype_;
		WBChar uid_[ WB_REAL_LEN(UID) ];
	};

	/** 请求状态回调函数 */
	typedef void (*fun_wb_status_callback)(int state , int errCode , void* pUserdata);

	// body 数据 
	typedef void (*fun_wb_body_callback)(unsigned long httpCode , const char* body , unsigned long len , void* pUserdata);

	// header 数据
	typedef void (*fun_wb_header_callback)(unsigned long httpcode,const char* header , unsigned long len , void* pUsrData);

	/** 每个包必定需要这个 */
	struct t_wb_oauth
	{
		union{
			char oauth_token_[ WB_REAL_LEN(OAUTH_TOKEN) ];
			char sue_[ WB_REAL_LEN(OAUTH_TOKEN) ];
		};
		union{
			char oauth_token_secret_[ WB_REAL_LEN(OAUTH_TOKEN_SECRET) ];
			char sup_[ WB_REAL_LEN(OAUTH_TOKEN_SECRET) ];
		};
		union{
			//  从官方网页获取到 认证返回的验证码 ，为了oauth第三步 ,获取授权过的Access Token
			char oauth_verifier_[ WB_REAL_LEN(OAUTH_VERIFIER) ];
			char tgt_[ WB_REAL_LEN(OAUTH_VERIFIER) ];
		};
		int  format_; ///< json/xml
		int  cookie_; ///< 是否cookie方式

		// 每个请求都需在这个回调函数
		fun_wb_status_callback fstatus_cb;
		fun_wb_body_callback   fbody_cb;
		fun_wb_header_callback fheader_cb;
	};

	/** 基结构（方便程序中，进行转化 ） */
	struct t_wb_base
	{
		t_wb_oauth wbauth_;
	};

	//////////////////////////////////////////////////////////////////////////////////////
	//获取下行数据集

	// 获取最新更新的公共微博消息 
	struct t_wb_get_statuses_public_timeline
	{
		t_wb_oauth wbauth_;
	};

	// 返回用户所有关注用户最新n条微博信息。和用户“我的首页”返回内容相同。
	struct t_wb_get_statuses_friends_timeline
	{
		t_wb_oauth wbauth_;

		t_wb_option_params wbopt_;
	};

	// 返回用户的发布的最近n条信息，和用户微博页面返回内容是一致的。此接口也可以请求其他用户的最新发表微博。
	struct t_wb_get_statuses_user_timeline
	{
		t_wb_oauth wbauth_;

		// 可以用来请求其他用户 
		t_wb_uid wbuid_;

		t_wb_option_params wbopt_;
	};

	//返回最新n条@我的微博 
	struct t_wb_get_statuses_mentions
	{
		t_wb_oauth wbauth_;

		t_wb_option_params wbopt_;
	};

	//获取当前用户发送及收到的评论列表 
	struct t_wb_get_statuses_comments_timeline
	{
		t_wb_oauth wbauth_;

		t_wb_option_params wbopt_;
	};

	//获取当前用户发出的评论 
	struct t_wb_get_statuses_comments_byme
	{
		t_wb_oauth wbauth_;

		t_wb_option_params wbopt_;
	};

	//获取当前用户收到的评论 
	#define t_wb_get_statuses_comments_tome t_wb_get_statuses_comments_byme


	// 获取指定微博的评论列表 
	struct t_wb_get_statuses_comments_list
	{
		t_wb_oauth wbauth_;

		WBChar wbId_[ WB_REAL_LEN(WEIBO_ID) ]; 

		t_wb_option_params wbopt_;
	};

	const int BATCH_WBIDS_COUNTS = 100;
	/*
	 * 批量统计微博的(评论数，转发数 , 一次最多100个)
	*/
	struct t_wb_get_statuses_counts 
	{
		t_wb_oauth wbauth_;

		WBChar wbIds_[BATCH_WBIDS_COUNTS][WB_REAL_LEN(WEIBO_ID)];///<微博ID , 最多 100
	};

	//获取当前用户Web未读消息数，包括@我的, 新评论，新私信，新粉丝数。
	struct t_wb_get_statuses_unread
	{
		t_wb_oauth wbauth_;

		t_wb_option_params wbopt_;
	};

	// 将当前用户指定类型的未读消息数清0。
	struct t_wb_get_statuses_reset_count
	{
		t_wb_oauth wbauth_;

		enum Type
		{
			Ty_comments = 1,
			///<1. 评论数
			Ty_atme,
			///< 2. @me数
			Ty_directmsg,
			///<3. 私信数
			Ty_friends,
			///<4. 关注数 
		};
		int type_; ///< 参考 Type值
	};
	//////////////////////////////////////////////////////////////////////////////////////
	//微博访问

	//获取单条ID的微博信息，作者信息将同时返回。
	struct t_wb_get_statuses_show
	{
		t_wb_oauth wbauth_;

		WBChar wbId_[ WB_REAL_LEN(WEIBO_ID) ]; 
	};

	//单条微博的Web访问路径。可以通过此url跳转到微博对应的Web网页。
	struct t_wb_goto_user_status_id
	{
		t_wb_oauth wbauth_;

		WBChar uid_[  WB_REAL_LEN(UID) ];
		WBChar wbId_[ WB_REAL_LEN(WEIBO_ID) ];
	};

	/*
	 * 发布一条微博信息。为防止重复，发布的信息与当前最新信息一样话，将会被忽略。 
	*/
	struct t_wb_put_statuses_update
	{
		t_wb_oauth wbauth_;

		WBChar szwbInfo_[WB_REAL_LEN(WB_INFO)];// 要更新的微博信息,信息内容部超过140个汉字。 

	};

	/*上传图片及发布微博信息
	 * 图片可以设置图片文件的路径，也可以直接设置图片的二进制数据来进行上传
	*/
	struct t_wb_put_statuses_upload
	{
		t_wb_oauth wbauth_;

		WBChar szImagePath_[WB_REAL_LEN(MAX_PATH) ];///< 图片路径

		WBChar szwbInfo_[WB_REAL_LEN(WB_INFO)];// 要更新的微博信息,信息内容部超过140个汉字。 
	};

	/*上传图片
	* 图片可以设置图片文件的路径，也可以直接设置图片的二进制数据来进行上传
	*/
	struct t_wb_put_statuses_upload_pic
	{
		t_wb_oauth wbauth_;
		WBChar szImagePath_[ WB_REAL_LEN(MAX_PATH) ];///< 图片路径
	};


	/*删除微博。注意：只能删除自己发布的信息。 
	*/
	struct t_wb_put_statuses_destroy
	{
		t_wb_oauth wbauth_;

		WBChar wbId_[WB_REAL_LEN(WEIBO_ID)];//删除的微博ID
	};

	/*转发一条微博信息。请求必须用POST方式提交*/
	struct t_wb_put_statuses_repost
	{
		t_wb_oauth wbauth_;

		WBChar wbId_[WB_REAL_LEN(WEIBO_ID)];///< 转发的微博ID 

		WBChar szwbInfo_[WB_REAL_LEN(WB_INFO)]; ///< 转发信息。信息内容不超过140个汉字
		
		int iCommentFlags ; ///< 是否在转发的同时发表评论
	};

	//对一条微博信息进行评论
	struct t_wb_put_statuses_comment
	{
		t_wb_oauth wbauth_;

		WBChar wbId_[WB_REAL_LEN(WEIBO_ID)];///< 要评论的微博id 
		WBChar wbCId_[WB_REAL_LEN(WEIBO_ID)]; ///< 选填参数， 要评论的评论id 
		WBChar szwbInfo_[WB_REAL_LEN(WB_INFO)]; ///< 评论内容。,信息内容不超过140个汉字。 
	};

	//删除微博的评论。注意：只能删除自己发布的信息。 
	struct t_wb_put_statuses_comment_destroy
	{
		t_wb_oauth wbauth_;

		WBChar wbId_[WB_REAL_LEN(WEIBO_ID)];///< 微博id[只为客户端本身使用]

		WBChar wbCId_[WB_REAL_LEN(WEIBO_ID)];///< 要删除的评论id 
	};

	/*对一条微博评论信息进行回复。请求必须用POST方式提交。为防止重复，发布的信息与最后一条评论/回复信息一样话，将会被忽略。 
	*/ 
	struct t_wb_put_statuses_reply 
	{
		t_wb_oauth wbauth_;

		WBChar wbId_[WB_REAL_LEN(WEIBO_ID)];//要回复的评论的微博id 
		WBChar wbCId_[WB_REAL_LEN(WEIBO_ID)];//要评论的评论id 
		WBChar szwbInfo_[WB_REAL_LEN(WB_INFO)];// 评论内容,信息内容不超过140个汉字。 
	};

	///////////////////////////////////////////////////////////////////////////////////////
	//用户接口 

	// 根据用户ID获取用户资料（授权用户）为了保护用户隐私，只有用户设置了公开或对粉丝设置了公开的数据才会返回。  
	struct t_wb_get_users_show
	{
		t_wb_oauth wbauth_;

		t_wb_uid  wbuid_;
	};

	/** 获取当前用户关注对象列表及最新一条微博信息 
	 * 返回用户关注对象列表，并返回最新微博文章。按关注人的关注时间倒序返回，
	 * 每次返回N个,通过cursor参数来取得多于N的关注人。当然也可以通过ID,nickname,user_id参数来获取其他人的关注人列表。 
	 */
	struct t_wb_get_statuses_friends
	{
		t_wb_oauth wbauth_;

		t_wb_uid  wbuid_;

		// 支持 cursor AND count
		t_wb_option_params wbopt_;
	};

	/**
	 * 返回用户的粉丝列表，并返回粉丝的最新微博。
	 * 按粉丝的关注时间倒序返回，每次返回100个,通过cursor参数来取得多于100的粉丝。注意目前接口最多只返回5000个粉丝。
	*/
    #define t_wb_get_statuses_followers t_wb_get_statuses_friends

	/* 系统推荐用户 
	* 分类，返回某一类别的推荐用户，默认为default。
	* default：人气关注
	* ent：影视名星
	* hk_famous：港台名人
	* model：模特
	* cooking：美食&健康
	* sport：体育名人
	* finance：商界名人
	* tech：IT互联网
	* singer：歌手
	* writer：作家
	* moderator：主持人
	* medium：媒体总编
	* stockplayer：炒股高手
	*/
	struct t_wb_users_hot
	{
		t_wb_oauth wbauth_;

		// 推荐种类，如果不在以上分类中，返回空列表。
		WBChar category_[WB_REAL_LEN(WEIBO_ID)];
	};

	/** 更新当前登录用户所关注的某个好友的备注信息 */
	struct t_wb_users_remark
	{
		t_wb_oauth wbauth_;

		//用户ID号
		WBChar userId_[WB_REAL_LEN(WEIBO_ID)];

		//更新的备注
		WBChar remark_[WB_REAL_LEN(WB_INFO)];
	};

	/** 返回当前用户可能感兴趣的用户 */
	struct t_wb_users_suggestions
	{
		t_wb_oauth wbauth_;

		//是否返回推荐原因，可选值1/0。当值为1，返回结果中增加推荐原因，会大幅改变返回值格式。
		int with_reason;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//私信

	/** 获取当前用户最新私信列表 
	 * 返回用户的最新n条私信，并包含发送者和接受者的详细资料。
	 */
	struct t_wb_get_direct_message
	{
		t_wb_oauth wbauth_;

		t_wb_option_params wbopt_;
	};

	// 返回登录用户已发送最新私信
	struct t_wb_get_direct_message_sent
	{
		t_wb_oauth wbauth_;

		t_wb_option_params wbopt_;
	};

	/*
	 * 发送一条私信
	*/
	struct t_wb_put_direct_message_new 
	{
		t_wb_oauth wbauth_;

		t_wb_uid  wbuid_;

		WBChar szText_[WB_REAL_LEN(DIRECTMSG)];//私信内容
	};

	/*
	 * 按ID删除私信。操作用户必须为私信的接收人。
	*/
	struct t_wb_put_direct_message_destroy
	{
		t_wb_oauth wbauth_;

		WBChar wbId_[WB_REAL_LEN(WEIBO_ID)];//要删除的私信主键ID. 
	};

	/*
	 * 按ID获取往来私信列表。操作用户必须为私信的接收人。
	*/
	struct t_wb_get_direct_message_with
	{
		t_wb_oauth wbauth_;

		int count_; //返回私信条数，默认为50。
		int page_;  //返回结果的页序号。

		WBChar wbuid_[WB_REAL_LEN(WEIBO_ID)];//ID
	};

	////////////////////////////////////////////////////////////////////////////
	// 关注接口

	// 关注某用户 
	struct t_wb_put_friendships_create
	{
		t_wb_oauth wbauth_;

		t_wb_uid wbuid_;
	};

	// 批量关注接口
	struct t_wb_put_friendships_create_batch
	{
		t_wb_oauth wbauth_;

		WBChar wbIDs_[WB_REAL_LEN(WEIBO_IDS)];//批量关注ID列表，逗号分隔 
	};

	// 取消关注 
	struct t_wb_put_friendships_destroy 
	{
		t_wb_oauth wbauth_;

		t_wb_uid wbuid_;

		int is_follower;
	};


	/* 判断两个用户是否有关注关系，返回两个用户关系的详细情况  */
	struct t_wb_get_friendships_show
	{
		t_wb_oauth wbauth_;

		t_wb_uid wbuid_source_;

		t_wb_uid wbuid_target_;
	};

	/* 批量获取一组用户是否为好友 */
	struct t_wb_get_friendships_batchexist
	{
		t_wb_oauth wbauth_;

		//
		WBChar wbIDs_[WB_REAL_LEN(WEIBO_IDS)];//批量获取用户关系的ID列表，逗号分隔
	};


	////////////////////////////////////////////////////////////////////////////
	// Social Graph

	//获取用户关注对象uid列表 
	struct t_wb_get_friends_ids
	{
		t_wb_oauth wbauth_;

		t_wb_uid wbuid_;

		// support count AND cursor 
		t_wb_option_params wbopt_;
	};

	// 获取用户粉丝对象uid列表
    #define t_wb_get_followers_ids t_wb_get_friends_ids


	////////////////////////////////////////////////////////////////////////////
	//账号接口

	/**
	 *
	 * 如果用户新浪通行证身份验证成功且用户已经开通微博则返回 http状态为 200；
	 如果是不则返回401的状态和错误信息。此方法用了判断用户身份是否合法且已经开通微博。
	 */
	struct t_wb_get_account_verify_credentials
	{
		t_wb_oauth wbauth_;
	};

	// 关于API的访问频率限制。返回当前小时还能访问的次数。频率限制是根据用户请求来做的限制，具体可以参加频率限制说明。 
    #define t_wb_get_account_rate_limit_status t_wb_get_account_verify_credentials
	
	//清除已验证用户的session，退出登录，并将cookie设为null。主要用于widget等web应用场合。
    #define t_wb_put_account_end_session t_wb_get_account_verify_credentials


	/** 更改自己的头像 , 必须为小于700K的有效的GIF, JPG, 或 PNG 图片. 如果图片大于500像素将按比例缩放 */
	struct t_wb_put_account_up_profileimage
	{
		t_wb_oauth wbauth_;

		WBChar szImagePath_[ WB_REAL_LEN(MAX_PATH) ];///< 绝对文件名
	};

	/** 用户信息 */
	struct t_wb_profile
	{
		//昵称
		WBChar szName_[WB_REAL_LEN(PROFILE_NAME)];
		//性别
		WBChar szGender_[WB_REAL_LEN(PROFILE_GENDER)];
		//省份ID
		WBChar szProvince_[WB_REAL_LEN(PROFILE_PROVINCE)];
		//城市ID
		WBChar szCity_[ WB_REAL_LEN(PROFILE_CITY) ];
		//描述
		WBChar szDescription_[WB_REAL_LEN(PROFILE_DESCRIPTION)];
	};

	/*自定义微博页面的参数。只会修改参数更新项。*/
	struct t_wb_put_account_up_profile 
	{
		t_wb_oauth wbauth_;

		struct t_wb_profile profile_;
	};

	/** 注册［不支持,官方网站注册］
	 * 该接口为受限接口（只对受邀请的合作伙伴开放）。 
	*/
	struct t_wb_put_account_register
	{
		t_wb_oauth wbauth_;

		// nick. 昵称，必须参数.不超过20个汉字 
		WBChar szNick_[ WB_REAL_LEN(PROFILE_NAME)];

		// gender 性别，必须参数. m,男，f,女。 
		WBChar szGender_[ WB_REAL_LEN(PROFILE_GENDER) ];

		// province 可选参数. 参考省份城市编码表  
		WBChar szProvince_[WB_REAL_LEN(PROFILE_PROVINCE)];//省份ID

		// city 可选参数. 参考省份城市编码表,1000为不限 
		WBChar szCity_[ WB_REAL_LEN(PROFILE_CITY) ];//城市ID

    	// email 注册邮箱 必须参数.
		WBChar szEmail_[WB_REAL_LEN(PROFILE_EMAIL)];

		// password 密码 必须参数. 
		WBChar szPwd_[ WB_REAL_LEN(PWD)];

		// ip 必须参数，注册用户用户当前真实的IP。
		WBChar szIP_[ WB_REAL_LEN(IP) ];//城市ID
	};

    ////////////////////////////////////////////////////////////////////////////
	// 收藏接口 

	// 获取当前用户的收藏列表 
    struct t_wb_get_favorites
	{
		t_wb_oauth wbauth_;

		// page
		t_wb_option_params wbopt_;
	};
	
	//添加收藏 
	struct t_wb_put_favorites_create
	{
		t_wb_oauth wbauth_;

		WBChar wbId_[WB_REAL_LEN(WEIBO_ID)];
	};

	//删除当前用户收藏的微博信息
    #define t_wb_put_favorites_destroy t_wb_put_favorites_create

	////////////////////////////////////////////////////////////////////////////
	//登录/OAuth接口

	// 第三放应用使用开始的request_token来换取用户授权过的Access_token
	struct t_wb_oauth_request_token
	{
		t_wb_oauth wbauth_;
	};

	// 第三方应用使用 request_token 来获取认证返回 verify code
	struct t_wb_authorize
	{
		t_wb_oauth wbauth_;

		WBChar  usrid_[ WB_REAL_LEN(UID) ];
		WBChar  usrpwd_[ WB_REAL_LEN(PWD) ];
	};



	// 第三放应用使用开始的request_token来换取用户授权过的Access_token
	struct t_wb_oauth_access_token
	{
		t_wb_oauth wbauth_;
	};

	// 第三方应用使用 xauth 来获取换取用户授权过的Access_token
	struct t_wb_xauth_access_token
	{
		t_wb_oauth wbauth_;

		WBChar  usrid_[ WB_REAL_LEN(UID) ];
		WBChar  usrpwd_[ WB_REAL_LEN(PWD) ];
		WBChar  authmode_[ WB_REAL_LEN(PWD) ];
	};

	// 表情(无参数)
	struct t_wb_emotions 
	{
		t_wb_oauth wbauth_;
	};

	/** 用户搜索 
	*/
	struct t_wb_users_search
	{
		t_wb_oauth wbauth_;

		WBChar q_[ WB_REAL_LEN(WB_INFO) ]; ///< 选填参数. 查询的关键字。必须进行url encode 
		///< 示例: http://api.t.sina.com.cn/users/search.json?q=api 

		WBChar snick_[WB_REAL_LEN(PROFILE_NAME)]; ///< 选填参数. 搜索范围包含昵称 (0是不包含，1为包含) 
		///< 示例: http://api.t.sina.com.cn/users/search.json?q=api&snick=0 

		int sdomain_; ///< 选填参数. 搜索范围包含个性域名 (0是不包含，1为包含) 
		///< 示例: http://api.t.sina.com.cn/users/search.json?q=api&sdomain=1 

		int sintro_;///< 选填参数. 搜索范围包含简介 (0是不包含，1为包含) 
		///< 示例: http://api.t.sina.com.cn/users/search.json?q=api&sintro=1 

		int province_;///< 省份ID 
		///< 示例: http://api.t.sina.com.cn/users/search.json?province=11 

		int city_;///< 城市ID 
		///< 示例: http://api.t.sina.com.cn/users/search.json?province=11&city=1 

		WBChar gender_[ WB_REAL_LEN(PROFILE_GENDER) ];///< 性别(m为男，f为女) 
		///< 示例: http://api.t.sina.com.cn/users/search.json?gender=m 

		WBChar comorsch_[ WB_REAL_LEN(WB_INFO) ];///< 公司学校名称 
		///< 示例: http://api.t.sina.com.cn/users/search.json?comorsch=新浪 

		int sort_;///< 排序方式(1为按更新时间，2为按粉丝数) 
		///< 示例: http://api.t.sina.com.cn/users/search.json?q=api&sort=1 

		int page_;///< 页码 
		///< 示例: http://api.t.sina.com.cn/users/search.json?q=api&sort=1&page=1 

		int count_;///<分页大小(默认返回10条) 
		///< 示例: http://api.t.sina.com.cn/users/search.json?q=api&sort=1&page=1&count=10 
	};

	// search 搜索微博文章 
	struct t_wb_search
	{
		t_wb_oauth wbauth_;

		WBChar q_[WB_REAL_LEN(WB_INFO)];///< 选填参数. 查询的关键字。必须进行url encode 
		///< 示例: http://api.t.sina.com.cn/search.json?q=api 

		WBChar geocode_[WB_REAL_LEN(GEOCODE) ];///< 可选. 返回指定经纬度附近的微博信息。经纬度参数格式是"纬度,经度,半径", 半径只支持km公里，为了和标准接口兼容，我们也接受 "mi" (miles)参数，但只会当做 "km" 来查询，暂时未做转换，格式需要url encode编码，如下 
		///< 示例: http://api.t.sina.com.cn/search.json?geocode=40.757929%2C-73.985506%2C25km 

		int sort_;

		int page_;///< 选填参数，页码(从1开始, 默认1) 
		///< 示例: http://api.t.sina.com.cn/search.json?q=api&sort=1&page=1 

		int count_;///< (别名：rpp)：选填参数，每页返回的微博数，默认返回10条，最大200 
		///< 示例: http://api.t.sina.com.cn/search.json?q=api&rpp=10 
	};

	// 搜索微博(多条件组合)
	struct t_wb_statuses_search 
	{
		t_wb_oauth wbauth_;

		WBChar q_[WB_REAL_LEN(WB_INFO)];///< 选填参数. 查询的关键字。必须进行url encode 
		///< 示例: http://api.t.sina.com.cn/statuses/search.json?q=api 

		int filter_ori_;///< 选填参数.过滤器，是否原创 (0为全部，5为原创，4为转发，默认0) 
		///< 示例: http://api.t.sina.com.cn/statuses/search.json?q=api&filter_ori=0 

		int filter_pic_;///< 选填参数. 过滤器，是否含图 (0为全部，1为含图，2为不含图) 
		///< 示例: http://api.t.sina.com.cn/statuses/search.json?q=api&filter_pic=1 

		WBChar fuid_[WB_REAL_LEN(UID)];///< 选填参数. 微博作者UID 
		///< 示例: http://api.t.sina.com.cn/statuses/search.json?q=api&fuid=11075 

		int province_;///< 省份ID 
		///< 示例: http://api.t.sina.com.cn/statuses/search.json?province=11 

		int city_;///< 城市ID 
		///< 示例: http://api.t.sina.com.cn/statuses/search.json?province=11&city=1 

		WBChar starttime_[WB_REAL_LEN(TIME)];///< 开始时间(unix时间戳) 
		///< 示例: http://api.t.sina.com.cn/statuses/search.json?q=api&starttime=1256289598 

		WBChar endtime_[WB_REAL_LEN(TIME)];///< 截止时间 (unix时间戳) 
		///< 示例: http://api.t.sina.com.cn/statuses/search.json?q=api&endtime=1256289598 

		int sort_;///<

		int page_;///< 选填参数，页码(从1开始, 默认1) 
		///< 示例: http://api.t.sina.com.cn/statuses/search.json?q=api&sort=1&page=1 

		int count_;///<(别名：rpp)：选填参数，每页返回的微博数，默认返回10条，最大200 
        ///< 示例: http://api.t.sina.com.cn/statuses/search.json?q=api&sort=1&page=1&count=10 
	};

	// 省份城市编码表 
	struct t_wb_provinces
	{
		t_wb_oauth wbauth_;
	};

	// cookie 方式
	struct t_wb_cookie
	{
		t_wb_oauth wbauth_;

		WBChar  usrid_[ WB_REAL_LEN(UID) ];
		WBChar  usrpwd_[ WB_REAL_LEN(PWD) ];
	};

	// update tgt
	#define t_wb_updateTGT t_wb_cookie
	//struct t_wb_updateTGT 
	//{
	//	t_wb_oauth wbauth_;

	//	WBChar tgt_[ WB_REAL_LEN(OAUTH_VERIFIER) ]; ///< VERIFY TGT
	//	WBChar usrid_[ WB_REAL_LEN(OAUTH_TOKEN) ];///< SUE
	//	WBChar usrpwd_[ WB_REAL_LEN(OAUTH_TOKEN_SECRET) ]; ///< SUP
	//};

	/*
	数据请求,自定义的
	*/
	struct t_wb_custom
	{
		t_wb_oauth wbauth_;

		WBChar szUrl_[WB_REAL_LEN(URL)];
		WBChar szPost_[WB_REAL_LEN(POST)]; ///< 参数
		int    method_; ///< 请求方法，POST， GET
	};

	////////////////////////////////////////////////////////////////////////////
	// 话题接口

	/** 获取某人话题 */
	struct t_wb_trends
	{
		t_wb_oauth wbauth_;

		//
		WBChar  usrid_[ WB_REAL_LEN(UID) ];///< 用户id（必选）
		int     page_;  ///< 页码，缺省值为1
		int     count_;///< 每页返回的记录数，缺省值为10
	};

	/** 获取某一话题下的微博 */
	struct t_wb_trends_statuses
	{
		t_wb_oauth wbauth_;

		WBChar  terndname_[ WB_REAL_LEN(WB_INFO) ];///< 话题关键字（必选参数）
	};

	/** 关注某一个话题 */
	struct t_wb_trends_follow
	{
		t_wb_oauth wbauth_;

		WBChar  terndname_[ WB_REAL_LEN(WB_INFO) ];///< 话题关键字（必选参数）
	};

	/** 取消关注某一个话题 */
	struct t_wb_trends_destroy
	{
		t_wb_oauth wbauth_;

		WBChar  trendid_[ WB_REAL_LEN(UID) ];///< 话题ID（必选参数）
	};

	/**返回最近一小时内的热门话题。*/
	struct t_wb_trends_hourly
	{
		t_wb_oauth wbauth_;

		int baseapp_;///< 是否基于当前应用来获取数据。1表示基于当前应用来获取数据。
	};

	/**按日期返回热门话题。返回某一日期的话题。*/
	struct t_wb_trends_daily
	{
		t_wb_oauth wbauth_;

		int baseapp_;///< 是否基于当前应用来获取数据。1表示基于当前应用来获取数据。
	};

	/**按周返回热门话题。返回某一日期之前某一周的话题。*/
	struct t_wb_trends_weekly
	{
		t_wb_oauth wbauth_;

		int baseapp_;///< 是否基于当前应用来获取数据。1表示基于当前应用来获取数据。
	};

	////////////////////////////////////////////////////////////////////////////
	// 黑名单接口

	/** 将某用户加入黑名单 */
	struct t_wb_blocks_create
	{
		t_wb_oauth wbauth_;

		//
		WBChar  usrid_[ WB_REAL_LEN(UID) ];///< 用户id（必选）
		WBChar  screenname_[ WB_REAL_LEN(SCREEN_NAME) ];///< 微博昵称
	};

	/** 将某用户移出黑名单 */
	struct t_wb_blocks_destroy
	{
		t_wb_oauth wbauth_;

		//
		WBChar  usrid_[ WB_REAL_LEN(UID) ];///< 用户id（必选）
		WBChar  screenname_[ WB_REAL_LEN(SCREEN_NAME) ];///< 微博昵称
	};

	/** 检测某用户是否是黑名单用户 */
	struct t_wb_blocks_exist
	{
		t_wb_oauth wbauth_;

		//
		WBChar  usrid_[ WB_REAL_LEN(UID) ];///< 用户id（必选）
		WBChar  screenname_[ WB_REAL_LEN(SCREEN_NAME) ];///< 微博昵称
	};

	/** 列出黑名单用户(输出用户详细信息) */
	struct t_wb_blocks_blocking
	{
		t_wb_oauth wbauth_;

		//
		int page_; ///< 页码。
		int count_;///< 单页记录数。
	};

	/** 列出分页黑名单用户（只输出id） */
	struct t_wb_blocks_blocking_ids
	{
		t_wb_oauth wbauth_;

		//
		int page_; ///< 页码。
		int count_;///< 单页记录数。
	};

	////////////////////////////////////////////////////////////////////////////
	// 用户标签接口

	/** 返回指定用户的标签列表 */
	struct t_wb_tags
	{
		t_wb_oauth wbauth_;

		//
		WBChar  usrid_[ WB_REAL_LEN(UID) ];///< 要获取的标签列表所属的用户ID（必选）
		int count_;///< 单页记录数，默认20，最大200
		int page_; ///< 页码。
	};

	/** 返回指定用户的标签列表 */
	struct t_wb_tags_create
	{
		t_wb_oauth wbauth_;

		//
		WBChar  tags_[ WB_REAL_LEN(TAGS_INFO) ];///< 要创建的一组标签，用半角逗号隔开。（必选）
	};

	/** 返回用户感兴趣的标签 */
	struct t_wb_tags_suggestions
	{
		t_wb_oauth wbauth_;

		int count_;///< 单页记录数。默认10，最大10。
		int page_;///<	默认1 页码。由于推荐标签是随机返回，故此特性暂不支持。
	};

	/**  删除标签 */
	struct t_wb_tags_destroy
	{
		t_wb_oauth wbauth_;

		WBChar tagId_[ WB_REAL_LEN(UID) ];///< 要删除的标签ID（必选）
	};

	/** 批量删除标签 */
	struct t_wb_tags_destroy_batch
	{
		t_wb_oauth wbauth_;

		WBChar ids_[WB_REAL_LEN(TAGS_INFO) ];///< 要删除的一组标签ID，以半角逗号隔开，一次最多提交20个ID。（必选）
	};

	////////////////////////////////////////////////////////////////////////////
	// 关系中心

	/** 邀请邮箱联系人 */
	struct t_wb_invite_mailcontect
	{
		t_wb_oauth wbauth_;

		WBChar usrid_[ WB_REAL_LEN(UID) ];
		WBChar usrpwd_[ WB_REAL_LEN(PWD) ];
		int    type_;
	};

	/** 邀请MSN联系人 */
	struct t_wb_invite_msncontect
	{
		t_wb_oauth wbauth_;

		//
		WBChar usrid_[ WB_REAL_LEN(UID) ];
		WBChar usrpwd_[ WB_REAL_LEN(PWD) ];
	};

	/** 发送邀请邮件 */
	struct t_wb_invite_sendmails
	{
		//
		t_wb_oauth wbauth_;

		//
		WBChar myusrid_[ WB_REAL_LEN(UID) ];
		WBChar nickname_[ WB_REAL_LEN(SCREEN_NAME) ];
		WBChar mailtype_[ 32 ];
		WBChar maillist_[ 2048 ];
	};

	/** 批量获取短链接地址 */
	struct t_wb_media_shorturl_batch
	{
		t_wb_oauth wbauth_;

		//url ids
		WBChar urlids_ [ WB_REAL_LEN(WEIBO_IDS) ];
	};

	////////////////////////////////////////////////////////////////////////////

	/** url 直接生成，通过 loWeibo_get_url() */
	struct t_wb_URI
	{
		char uri_[ WB_REAL_LEN(URL) ];
		char post_[ WB_REAL_LEN(POST) ];
	};

	/** */
	struct t_wb_REQ_EX
	{
		// json, xml(default = json )
		int format_;

		// cookie 方式 | cookie= 0 ==> ouath 方式
		int cookie_;

		// appkey = source
		char appkey_[256];
		char secret_[256];

		// ip地址
		char ip_[64];
	};

	/** 任务包*/
	typedef struct t_wb_task TASK_DATA , *PTASK_DATA;
	typedef void (*WBTaskCB)(const char* taskid , void*& userdata , void*& serviced);
	struct t_wb_task
	{
		char  taskid[64];
		void* userdata; ///< 用户定义数据
		void* serviced; ///< 保留字段
		WBTaskCB cb;
	};
	
	/** 进行 oauth 结构的copy ,只对  struct t_wb_oauth 下面三个成员变量进行copy 
		char oauth_token_[ WB_REAL_LEN(OAUTH_TOKEN) ];
		char oauth_token_secret_[ WB_REAL_LEN(OAUTH_TOKEN_SECRET) ];

		//  从官方网页获取到 认证返回的验证码 ，为了oauth第三步 ,获取授权过的Access Token
		char oauth_verifier_[ WB_REAL_LEN(OAUTH_VERIFIER) ];
	*/
	WEIBO_API void loWeibo_oauth_copy(struct t_wb_oauth* pdest , const struct t_wb_oauth* psrc);

	// 消毁掉 对象 request（内部有自已消毁机制） ， 回调 WEIBO_OPTION(REQ_DESTROY_FUNCTION)
	typedef void (*fun_req_destroy)(void* pUserdata);

	// 微博请求对象
	typedef void WEIBORequest;

	/** 创建一个请求，每个事件都需要 创建一个请求（一定要通过 weibo_destroy_request（） 进行消毁 */
	WEIBO_API WEIBORequest* loWeibo_request_create(void);

	/** 错误值 */
	typedef enum
	{
		ERR_createhttpobject_nil = -100,
		ERR_checkvalid_fail,
		ERR_url_set_fail,
		ERR_requestParam_nil,
		ERR_reqSessionParam_nil,
		ERR_createhttpSession_nil,
		ERR_Fail = -1,
		ERR_OK = 0,
	}WEIBOError;

	// 请求的option值
	typedef enum
	{
		WEIBO_OPTION_DEFINE(REQ_UNK,0),
		WEIBO_OPTION(REQ_DESTROY_FUNCTION), ///< 当请求被消毁时，通过回调函数告开发者

		WEIBO_OPTION(REQ_SENDDATA), ///< 发送的数据
		WEIBO_OPTION(REQ_USERDATA), ///< 开发者自定义数据,如果设置了它，它会随着 回调函数返回给开发者使用
		WEIBO_OPTION(REQ_PROXYDATA),///< 请求的代理，目前不支持
		WEIBO_OPTION(REQ_RESULT_DATAFORMAT),///< 请求的返回包的，参考 WEIBORequestFormat 格式(E_FORMAT_JSON = json, E_FORMAT_XML = xml )
		WEIBO_OPTION(REQ_COOKIE), ///< 使用 cookie 方式 , loWeibo_request_setopt( reqobj , WEIBO_OPTION(REQ_COOKIE) ,1);

		WEIBO_OPTION(REQ_APPKEY), ///< set app key , loWeibo_request_setopt( reqobj , WEIBO_OPTION(REQ_APPKEY) ,"1122333");
		WEIBO_OPTION(REQ_SECRETKEY), ///< set app secret, loWeibo_request_setopt( reqobj , WEIBO_OPTION(REQ_SECRETKEY) ,"bbbbb");
		WEIBO_OPTION(REQ_IP), ///< set ip addre
	   WEIBO_OPTION(REQ_LAST),
	}WEIBOReqoption;

	/** 进行一些参数操作填写，然后执行(回调函数的设置...) */
	WEIBO_API int  loWeibo_request_setopt(WEIBORequest* request , WEIBOReqoption option , ...);

	/** 执行,等待回调数据的执行，其执行结果，会通过 weibo_request_setopt（）设置的方法，告之开发者
	 *
	 * @param bAsyn = false 同步执行（线程阻塞 )
	 *              = true  表示异步
	 *
	 * @return 请参考 WEIBOError
	*/
	WEIBO_API int  loWeibo_request_start(WEIBORequest* request , bool bAsyn);

	/** 等待执行（与异步 loWeibo_request_start 执行可以结合 ， 达到同步作用 ）*/
	WEIBO_API void loWeibo_request_wait(WEIBORequest* request);

	/** 停止 */
	WEIBO_API int  loWeibo_request_stop(WEIBORequest* request);

	/** 消毁这个请求 */
	WEIBO_API int  loWeibo_request_destroy(WEIBORequest* request);

	/** 
	 * @brief 直接生成URL信息，内部只提供一个算法把所需要的信息组织在一起，方便调用
	 *
	 * @param ［out] outURI,返回生成URL信息
	 * @param  [in] option 参考  WEIBOoption
	 * @param  [in] t_wb   根据 WEIBOoption值对应的不同结构对象
	 * @param  [in] req_ex 一些附加的认证条件内容
	 *
	 * @return 0 ,成功
	 */
	WEIBO_API int  loWeibo_get_url(struct t_wb_URI* outURI/*[out]*/ , WEIBOoption option , const void* t_wb , const struct t_wb_REQ_EX* req_ex);

#ifdef  __cplusplus
}
#endif

/**
 * @exmaple:
 *   
 *  WEIBORequest* pWBReq = loWeibo_request_create();
 *  if( pWBReq )
 *  {
 	   struct t_wb_oauth_request_token   oauth_reqtoken;
	   
	   // 使用json方式
	   loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_JSON);

	   // 请求时，发送的请求包内容
	   loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(OAUTH_REQUEST_TOKEN) , &oauth_reqtoken );

	   // 开始请求
	   loWeibo_request_start(pWBReq , false);

	   // 消毁
       loWeibo_request_destroy(pWBReq);

 *  }
 *
 *
*/

#endif /* __weibo_SDK_H__ */
