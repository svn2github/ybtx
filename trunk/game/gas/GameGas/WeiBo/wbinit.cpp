/**
*  @brief weibo initialize
*  @file  wbinit.cpp
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

#include "wbinit.h"
#include "wbcallback.h"
#include <libWeibo/weibo.h>
#include <stdlib.h>
#include <string.h>

// for declare function
#define WEIBO_struct_init_callback(name) \
	static bool Wb_initstruct_##name##_cb(void* t_wb)

// for enum
#define WEIBO_struct_init_fun(name) Wb_initstruct_##name##_cb

//
#define WEIBO_struct_cast( STRUCT ) \
    struct STRUCT* pstruct = (struct STRUCT*)t_wb;\
	memset(pstruct ,0 , sizeof(struct STRUCT) );

// 设置结构的回调函数
#define SET_struct_callback_object(option , poauth ) \
	{\
        struct t_wb_callback_byloach* t_cb = Wb_get_struct_callback( option );\
		(poauth).fstatus_cb = t_cb->fstatus_cb;\
		(poauth).fbody_cb = t_cb->fbody_cb;\
		(poauth).fheader_cb = t_cb->fheader_cb;\
	}

WEIBO_struct_init_callback(BASE)
{
	WEIBO_struct_cast(t_wb_oauth);

	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_PUBLIC_TIMELINE)
{
	WEIBO_struct_cast(t_wb_get_statuses_public_timeline);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_PUBLIC_TIMELINE) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_FRIENDS_TIMELINE)
{
	WEIBO_struct_cast(t_wb_get_statuses_friends_timeline);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_FRIENDS_TIMELINE) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_USE_TIMELINE)
{
	WEIBO_struct_cast(t_wb_get_statuses_user_timeline);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_USE_TIMELINE) , pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_MENTIONS)
{
	WEIBO_struct_cast(t_wb_get_statuses_mentions);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_MENTIONS) , pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_COMMENTS_TIMELINE)
{
	WEIBO_struct_cast(t_wb_get_statuses_comments_timeline);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_COMMENTS_TIMELINE) , pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_COMMENTS_BYME)
{
	WEIBO_struct_cast(t_wb_get_statuses_comments_byme);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_COMMENTS_BYME) , pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_COMMENTS_TOME)
{
	WEIBO_struct_cast(t_wb_get_statuses_comments_tome);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_COMMENTS_TOME) , pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_COMMENTS_LIST)
{
	WEIBO_struct_cast(t_wb_get_statuses_comments_list);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_COMMENTS_LIST) , pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_COMMENTS_COUNTS)
{
	WEIBO_struct_cast(t_wb_get_statuses_counts);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_COMMENTS_COUNTS) , pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(GETSTATUSES_UNREAD)
{
	WEIBO_struct_cast(t_wb_get_statuses_unread);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_UNREAD) , pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(PUTSTATUSES_RESET_COUNT)
{
	WEIBO_struct_cast(t_wb_get_statuses_reset_count);

	SET_struct_callback_object( WEIBO_OPTION(PUTSTATUSES_RESET_COUNT) , pstruct->wbauth_ );
	return true;
}

//微博访问
WEIBO_struct_init_callback(GETSTATUSES_SHOW)
{
	WEIBO_struct_cast(t_wb_get_statuses_show);

	SET_struct_callback_object( WEIBO_OPTION(GETSTATUSES_SHOW) , pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(GOTOSTATUSES_ID)
{
	WEIBO_struct_cast(t_wb_goto_user_status_id);

	SET_struct_callback_object( WEIBO_OPTION(GOTOSTATUSES_ID) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(PUTSTATUSES_UPDATE)
{
	WEIBO_struct_cast(t_wb_put_statuses_update);

	SET_struct_callback_object( WEIBO_OPTION(PUTSTATUSES_UPDATE) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(PUTSTATUSES_UPLOAD)
{
	WEIBO_struct_cast(t_wb_put_statuses_upload);

	SET_struct_callback_object( WEIBO_OPTION(PUTSTATUSES_UPLOAD) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(PUTSTATUSES_DESTROY)
{
	WEIBO_struct_cast(t_wb_put_statuses_destroy);

	SET_struct_callback_object( WEIBO_OPTION(PUTSTATUSES_DESTROY) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(PUTSTATUSES_REPOST)
{
	WEIBO_struct_cast(t_wb_put_statuses_repost);

	SET_struct_callback_object( WEIBO_OPTION(PUTSTATUSES_REPOST) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(PUTSTATUSES_COMMENT)
{
	WEIBO_struct_cast(t_wb_put_statuses_comment);

	SET_struct_callback_object( WEIBO_OPTION(PUTSTATUSES_COMMENT) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(PUTSTATUSES_COMMENT_DESTROY)
{
	WEIBO_struct_cast(t_wb_put_statuses_comment_destroy);

	SET_struct_callback_object( WEIBO_OPTION(PUTSTATUSES_COMMENT_DESTROY) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(PUTSTATUSES_REPLY)
{
	WEIBO_struct_cast(t_wb_put_statuses_reply);

	SET_struct_callback_object( WEIBO_OPTION(PUTSTATUSES_REPLY) , pstruct->wbauth_ );

	return true;
}

//用户
WEIBO_struct_init_callback(GETUSER_INFO)
{
	WEIBO_struct_cast(t_wb_get_users_show);

	SET_struct_callback_object( WEIBO_OPTION(GETUSER_INFO) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(GETFRINDS_LIST)
{
	WEIBO_struct_cast(t_wb_get_statuses_friends);

	SET_struct_callback_object( WEIBO_OPTION(GETFRINDS_LIST) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(GETFOLLOWERS_LIST)
{
	WEIBO_struct_cast(t_wb_get_statuses_followers);

	SET_struct_callback_object( WEIBO_OPTION(GETFOLLOWERS_LIST) , pstruct->wbauth_ );

	return true;
}


//私信
WEIBO_struct_init_callback(GETDIRECTMSG)
{
	WEIBO_struct_cast(t_wb_get_direct_message);

	SET_struct_callback_object( WEIBO_OPTION(GETDIRECTMSG) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(GETDIRESTMSG_SENT)
{
	WEIBO_struct_cast(t_wb_get_direct_message_sent);

	SET_struct_callback_object( WEIBO_OPTION(GETDIRESTMSG_SENT) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(PUTDIRECTMSG_NEW)
{
	WEIBO_struct_cast(t_wb_put_direct_message_new);

	SET_struct_callback_object( WEIBO_OPTION(PUTDIRECTMSG_NEW) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(PUTDIRECTMSG_DESTROY)
{
	WEIBO_struct_cast(t_wb_put_direct_message_destroy);

	SET_struct_callback_object( WEIBO_OPTION(PUTDIRECTMSG_DESTROY) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(GETDIRECTMSG_WITH)
{
	WEIBO_struct_cast(t_wb_get_direct_message_with);

	SET_struct_callback_object( WEIBO_OPTION(GETDIRECTMSG_WITH) , pstruct->wbauth_ );

	return true;
}



//关注
WEIBO_struct_init_callback(PUTFRIENDSHIPS_CREATE)
{
	WEIBO_struct_cast(t_wb_put_friendships_create);

	SET_struct_callback_object( WEIBO_OPTION(PUTFRIENDSHIPS_CREATE) , pstruct->wbauth_ );

	return true;
}

//关注
WEIBO_struct_init_callback(PUTFRIENDSHIPS_CREATE_BATCH)
{
	WEIBO_struct_cast(t_wb_put_friendships_create_batch);

	SET_struct_callback_object( WEIBO_OPTION(PUTFRIENDSHIPS_CREATE_BATCH) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(PUTFRIENDSHIPS_DESTROY)
{
	WEIBO_struct_cast(t_wb_put_friendships_destroy);

	SET_struct_callback_object( WEIBO_OPTION(PUTFRIENDSHIPS_DESTROY) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(GETFRIENDSHIPS_EXISTS)
{
	WEIBO_struct_cast(t_wb_get_friendships_show);

	SET_struct_callback_object( WEIBO_OPTION(GETFRIENDSHIPS_EXISTS) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(GETFRIENDSHIPS_BATCH_EXISTS)
{
	WEIBO_struct_cast(t_wb_get_friendships_batchexist);

	SET_struct_callback_object( WEIBO_OPTION(GETFRIENDSHIPS_BATCH_EXISTS) , pstruct->wbauth_ );

	return true;
}


//Social Graph
WEIBO_struct_init_callback(GETFRIEND_IDS)
{
	WEIBO_struct_cast(t_wb_get_friends_ids);

	SET_struct_callback_object( WEIBO_OPTION(GETFRIEND_IDS) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(GETFOLLOWER_IDS)
{
	WEIBO_struct_cast(t_wb_get_followers_ids);

	SET_struct_callback_object( WEIBO_OPTION(GETFOLLOWER_IDS) , pstruct->wbauth_ );

	return true;
}

//账号
WEIBO_struct_init_callback(GETACCOUNT_VERIFY)
{
	WEIBO_struct_cast(t_wb_get_account_verify_credentials);

	SET_struct_callback_object( WEIBO_OPTION(GETACCOUNT_VERIFY) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(GETACCOUNT_RATELIMIT)
{
	WEIBO_struct_cast(t_wb_get_account_rate_limit_status);

	SET_struct_callback_object( WEIBO_OPTION(GETACCOUNT_RATELIMIT) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(PUTACCOUNT_QUITSESSION)
{
	WEIBO_struct_cast(t_wb_put_account_end_session);

	SET_struct_callback_object( WEIBO_OPTION(PUTACCOUNT_QUITSESSION) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(PUTACCOUNT_UPDATE_PROFILEIMAGE)
{
	WEIBO_struct_cast(t_wb_put_account_up_profileimage);

	SET_struct_callback_object( WEIBO_OPTION(PUTACCOUNT_UPDATE_PROFILEIMAGE) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(PUTACCOUNT_UPDATE_PROFILE)
{
	WEIBO_struct_cast(t_wb_put_account_up_profile);

	SET_struct_callback_object( WEIBO_OPTION(PUTACCOUNT_UPDATE_PROFILE) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(PUTACCOUNT_REGISTER)
{
	return false;
}


// 收藏
WEIBO_struct_init_callback(GETFAVORITES)// 获取当前用户的收藏列表 
{
	WEIBO_struct_cast(t_wb_get_favorites);

	SET_struct_callback_object( WEIBO_OPTION(GETFAVORITES) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(PUTFAVORITES_CREATE)// 添加收藏 
{
	WEIBO_struct_cast(t_wb_put_favorites_create);

	SET_struct_callback_object( WEIBO_OPTION(PUTFAVORITES_CREATE) , pstruct->wbauth_ );

	return true;
}


WEIBO_struct_init_callback(PUTFAVORITES_DESTROY)// 删除当前用户收藏的微博信息 
{
	WEIBO_struct_cast(t_wb_put_favorites_destroy);

	SET_struct_callback_object( WEIBO_OPTION(PUTFAVORITES_DESTROY) , pstruct->wbauth_ );

	return true;
}


//登录/OAuth
WEIBO_struct_init_callback(OAUTH_REQUEST_TOKEN)// 获取未授权的Request Token
{
	WEIBO_struct_cast(t_wb_oauth_request_token);

	SET_struct_callback_object( WEIBO_OPTION(OAUTH_REQUEST_TOKEN) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(OAUTH_AUTHORIZE)// 请求用户授权Token 
{
	WEIBO_struct_cast(t_wb_authorize);

	SET_struct_callback_object( WEIBO_OPTION(OAUTH_AUTHORIZE) , pstruct->wbauth_ );

	return true;
}

WEIBO_struct_init_callback(OAUTH_ACCESS_TOKEN)// 获取授权过的Access Token
{
	WEIBO_struct_cast(t_wb_oauth_access_token);

	SET_struct_callback_object( WEIBO_OPTION(OAUTH_ACCESS_TOKEN) , pstruct->wbauth_ );

	return true;
}

//
WEIBO_struct_init_callback(GET_EMOTIONS)
{
	WEIBO_struct_cast(t_wb_emotions);
	SET_struct_callback_object( WEIBO_OPTION(GET_EMOTIONS) , pstruct->wbauth_ );
	return true;
}
//
WEIBO_struct_init_callback(GET_USERS_SEARCH)
{
	WEIBO_struct_cast(t_wb_users_search);
	SET_struct_callback_object( WEIBO_OPTION(GET_USERS_SEARCH) , pstruct->wbauth_ );
	return true;
}
//
WEIBO_struct_init_callback(GET_WB_SEARCH)
{
	WEIBO_struct_cast(t_wb_search);
	SET_struct_callback_object( WEIBO_OPTION(GET_WB_SEARCH) , pstruct->wbauth_ );
	return true;
}
//
WEIBO_struct_init_callback(GET_STATUSES_SEARCH)
{
	WEIBO_struct_cast(t_wb_statuses_search);
	SET_struct_callback_object( WEIBO_OPTION(GET_STATUSES_SEARCH) , pstruct->wbauth_ );
	return true;
}

// 省份
WEIBO_struct_init_callback(GET_PROVINCES)
{
	WEIBO_struct_cast(t_wb_provinces);
	SET_struct_callback_object( WEIBO_OPTION(GET_PROVINCES) , pstruct->wbauth_ );
	return true;
}

//自定义URL
WEIBO_struct_init_callback(COOKIE)
{
	WEIBO_struct_cast(t_wb_cookie);
	SET_struct_callback_object( WEIBO_OPTION(COOKIE) , pstruct->wbauth_ );
	return true;
}

//自定义URL
WEIBO_struct_init_callback(UPDATETGT)
{
	WEIBO_struct_cast(t_wb_updateTGT);
	SET_struct_callback_object( WEIBO_OPTION(UPDATETGT) , pstruct->wbauth_ );
	return true;
}

//自定义URL
WEIBO_struct_init_callback(CUSTOM)
{
	WEIBO_struct_cast(t_wb_custom);
    SET_struct_callback_object( WEIBO_OPTION(CUSTOM) , pstruct->wbauth_ );
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////
// 用户新接口

// 获取系统推荐用户
WEIBO_struct_init_callback(GET_USERS_HOT)
{
	WEIBO_struct_cast(t_wb_users_hot);
	SET_struct_callback_object( WEIBO_OPTION(GET_USERS_HOT) , pstruct->wbauth_ );
	return true;
}
//更新修改当前登录用户所关注的某个好友的备注信息New!
WEIBO_struct_init_callback(POST_USERS_REMARK)
{
	WEIBO_struct_cast(t_wb_users_remark);
	SET_struct_callback_object( WEIBO_OPTION(POST_USERS_REMARK) , pstruct->wbauth_ );
	return true;
}

//返回当前用户可能感兴趣的用户
WEIBO_struct_init_callback(GET_USERS_SUGGESTIONS)
{
	WEIBO_struct_cast(t_wb_users_suggestions);
	SET_struct_callback_object( WEIBO_OPTION(GET_USERS_SUGGESTIONS) , pstruct->wbauth_ );
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// 用户

// 话题接口 ,by welbon,2011-01-10
//trends 获取某人的话题
WEIBO_struct_init_callback(GET_TRENDS)
{
	WEIBO_struct_cast(t_wb_trends);
	SET_struct_callback_object( WEIBO_OPTION(GET_TRENDS) , pstruct->wbauth_ );
	return true;
}

//trends/statuses 获取某一话题下的微博
WEIBO_struct_init_callback(GET_TRENDS_STATUSES)
{
	WEIBO_struct_cast(t_wb_trends_statuses);
	SET_struct_callback_object( WEIBO_OPTION(GET_TRENDS_STATUSES) , pstruct->wbauth_ );
	return true;
}

//trends/follow 关注某一个话题
WEIBO_struct_init_callback(POST_TRENDS_FOLLOW)
{
	WEIBO_struct_cast(t_wb_trends_follow);
	SET_struct_callback_object( WEIBO_OPTION(POST_TRENDS_FOLLOW) , pstruct->wbauth_ );
	return true;
}

//trends/destroy 取消关注的某一个话题
WEIBO_struct_init_callback(DELETE_TRENDS_DESTROY)
{
	WEIBO_struct_cast(t_wb_trends_destroy);
	SET_struct_callback_object( WEIBO_OPTION(DELETE_TRENDS_DESTROY) , pstruct->wbauth_ );
	return true;
}

//trends/destroy 按小时返回热门话题
WEIBO_struct_init_callback(GET_TRENDS_HOURLY)
{
	WEIBO_struct_cast(t_wb_trends_hourly);
	SET_struct_callback_object( WEIBO_OPTION(GET_TRENDS_HOURLY), pstruct->wbauth_ );
	return true;
}

//trends/daily 按日期返回热门话题。返回某一日期的话题
WEIBO_struct_init_callback(GET_TRENDS_DAYLIY)
{
	WEIBO_struct_cast(t_wb_trends_daily);
	SET_struct_callback_object( WEIBO_OPTION(GET_TRENDS_DAYLIY), pstruct->wbauth_ );
	return true;
}

//trends/weekly 按周返回热门话题。返回某一日期之前某一周的话题
WEIBO_struct_init_callback(GET_TRENDS_WEEKLIY)
{
	WEIBO_struct_cast(t_wb_trends_weekly);
	SET_struct_callback_object( WEIBO_OPTION(GET_TRENDS_WEEKLIY), pstruct->wbauth_ );
	return true;
}

// 黑名单接口 ,by welbon,2011-01-10
//将某用户加入黑名单
WEIBO_struct_init_callback(POST_BLOCKS_CREATE)
{
	WEIBO_struct_cast(t_wb_blocks_create);
	SET_struct_callback_object( WEIBO_OPTION(POST_BLOCKS_CREATE), pstruct->wbauth_ );
	return true;
}
//将某用户移出黑名单
WEIBO_struct_init_callback(POST_BLOCKS_DESTROY)
{
	WEIBO_struct_cast(t_wb_blocks_destroy);
	SET_struct_callback_object( WEIBO_OPTION(POST_BLOCKS_DESTROY), pstruct->wbauth_ );
	return true;
}
//检测某用户是否是黑名单用户
WEIBO_struct_init_callback(GET_BLOCKS_EXISTS)
{
	WEIBO_struct_cast(t_wb_blocks_exist);
	SET_struct_callback_object( WEIBO_OPTION(GET_BLOCKS_EXISTS), pstruct->wbauth_ );
	return true;
}
//列出黑名单用户(输出用户详细信息)
WEIBO_struct_init_callback(GET_BLOCKS_BLOCKING)
{
	WEIBO_struct_cast(t_wb_blocks_blocking);
	SET_struct_callback_object( WEIBO_OPTION(GET_BLOCKS_BLOCKING), pstruct->wbauth_ );
	return true;
}

//列出分页黑名单用户（只输出id）
WEIBO_struct_init_callback(GET_BLOCKS_BLOCKING_IDS)
{
	WEIBO_struct_cast(t_wb_blocks_blocking_ids);
	SET_struct_callback_object( WEIBO_OPTION(GET_BLOCKS_BLOCKING_IDS), pstruct->wbauth_ );
	return true;
}

//用户标签接口 ,by welbon,2011-01-10

//tags 返回指定用户的标签列表
WEIBO_struct_init_callback(GET_TAGS)
{
	WEIBO_struct_cast(t_wb_tags);
	SET_struct_callback_object( WEIBO_OPTION(GET_TAGS), pstruct->wbauth_ );
	return true;
}

//tags/create 添加用户标签
WEIBO_struct_init_callback(POST_TAGS_CREATE)
{
	WEIBO_struct_cast(t_wb_tags_create);
	SET_struct_callback_object( WEIBO_OPTION(POST_TAGS_CREATE), pstruct->wbauth_ );
	return true;
}

//tags/suggestions 返回用户感兴趣的标签
WEIBO_struct_init_callback(GET_TAGS_SUGGESTIONS)
{
	WEIBO_struct_cast(t_wb_tags_suggestions);
	SET_struct_callback_object( WEIBO_OPTION(GET_TAGS_SUGGESTIONS), pstruct->wbauth_ );
	return true;
}

//tags/destroy 删除标签
WEIBO_struct_init_callback(POST_TAGS_DESTROY)
{
	WEIBO_struct_cast(t_wb_tags_destroy);
	SET_struct_callback_object( WEIBO_OPTION(POST_TAGS_DESTROY), pstruct->wbauth_ );
	return true;
}

//tags/destroy_batch 批量删除标签
WEIBO_struct_init_callback(POST_TAGS_DESTROY_BATCH)
{
	WEIBO_struct_cast(t_wb_tags_destroy_batch);
	SET_struct_callback_object( WEIBO_OPTION(POST_TAGS_DESTROY_BATCH), pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(POST_INVITE_MAILCONTACT)
{
	WEIBO_struct_cast(t_wb_invite_mailcontect );
	SET_struct_callback_object( WEIBO_OPTION(POST_INVITE_MAILCONTACT), pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(POST_INVITE_MSNCONTACT)
{
	WEIBO_struct_cast(t_wb_invite_msncontect);
	SET_struct_callback_object( WEIBO_OPTION(POST_INVITE_MSNCONTACT), pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(POST_INVITE_SENDMAILS)
{
	WEIBO_struct_cast(t_wb_invite_sendmails);
	SET_struct_callback_object( WEIBO_OPTION(POST_INVITE_SENDMAILS), pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(GET_MEDIA_SHORTURL_BATCH)
{
	WEIBO_struct_cast(t_wb_media_shorturl_batch);
	SET_struct_callback_object( WEIBO_OPTION(GET_MEDIA_SHORTURL_BATCH), pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(XAUTH_ACCESS_TOKEN)
{
	WEIBO_struct_cast(t_wb_xauth_access_token);
	SET_struct_callback_object( WEIBO_OPTION(XAUTH_ACCESS_TOKEN), pstruct->wbauth_ );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////

typedef bool (*fun_init_wb_struct_callback)(void* t_wb);
static fun_init_wb_struct_callback vector_init_callback[ WEIBO_OPTION(LAST) ] =
{
	NULL,
	WEIBO_struct_init_fun(BASE),
	WEIBO_struct_init_fun(GETSTATUSES_PUBLIC_TIMELINE),
	WEIBO_struct_init_fun(GETSTATUSES_FRIENDS_TIMELINE),
	WEIBO_struct_init_fun(GETSTATUSES_USE_TIMELINE),
	WEIBO_struct_init_fun(GETSTATUSES_MENTIONS),
	WEIBO_struct_init_fun(GETSTATUSES_COMMENTS_TIMELINE),
	WEIBO_struct_init_fun(GETSTATUSES_COMMENTS_BYME),
	WEIBO_struct_init_fun(GETSTATUSES_COMMENTS_TOME),
	WEIBO_struct_init_fun(GETSTATUSES_COMMENTS_LIST),
	WEIBO_struct_init_fun(GETSTATUSES_COMMENTS_COUNTS),
	WEIBO_struct_init_fun(GETSTATUSES_UNREAD),
	WEIBO_struct_init_fun(PUTSTATUSES_RESET_COUNT),

	//微博访问
	WEIBO_struct_init_fun(GETSTATUSES_SHOW),
	WEIBO_struct_init_fun(GOTOSTATUSES_ID),
	WEIBO_struct_init_fun(PUTSTATUSES_UPDATE),
	WEIBO_struct_init_fun(PUTSTATUSES_UPLOAD),
	WEIBO_struct_init_fun(PUTSTATUSES_DESTROY),
	WEIBO_struct_init_fun(PUTSTATUSES_REPOST),
	WEIBO_struct_init_fun(PUTSTATUSES_COMMENT),
	WEIBO_struct_init_fun(PUTSTATUSES_COMMENT_DESTROY),
	WEIBO_struct_init_fun(PUTSTATUSES_REPLY),

	//用户
	WEIBO_struct_init_fun(GETUSER_INFO),
	WEIBO_struct_init_fun(GETFRINDS_LIST),
	WEIBO_struct_init_fun(GETFOLLOWERS_LIST),

	//私信
	WEIBO_struct_init_fun(GETDIRECTMSG),
	WEIBO_struct_init_fun(GETDIRESTMSG_SENT),
	WEIBO_struct_init_fun(PUTDIRECTMSG_NEW),
	WEIBO_struct_init_fun(PUTDIRECTMSG_DESTROY),
	WEIBO_struct_init_fun(GETDIRECTMSG_WITH),
	

	//关注
	WEIBO_struct_init_fun(PUTFRIENDSHIPS_CREATE),
	WEIBO_struct_init_fun(PUTFRIENDSHIPS_CREATE_BATCH),
	WEIBO_struct_init_fun(PUTFRIENDSHIPS_DESTROY),
	WEIBO_struct_init_fun(GETFRIENDSHIPS_EXISTS),
	WEIBO_struct_init_fun(GETFRIENDSHIPS_BATCH_EXISTS),

	//Social Graph
	WEIBO_struct_init_fun(GETFRIEND_IDS),
	WEIBO_struct_init_fun(GETFOLLOWER_IDS),

	//账号
	WEIBO_struct_init_fun(GETACCOUNT_VERIFY),
	WEIBO_struct_init_fun(GETACCOUNT_RATELIMIT),
	WEIBO_struct_init_fun(PUTACCOUNT_QUITSESSION),
	WEIBO_struct_init_fun(PUTACCOUNT_UPDATE_PROFILEIMAGE),
	WEIBO_struct_init_fun(PUTACCOUNT_UPDATE_PROFILE),
	WEIBO_struct_init_fun(PUTACCOUNT_REGISTER),

	// 收藏
	WEIBO_struct_init_fun(GETFAVORITES),// 获取当前用户的收藏列表 
	WEIBO_struct_init_fun(PUTFAVORITES_CREATE),// 添加收藏 
	WEIBO_struct_init_fun(PUTFAVORITES_DESTROY),// 删除当前用户收藏的微博信息 

	//登录/OAuth
	WEIBO_struct_init_fun(OAUTH_REQUEST_TOKEN),// 获取未授权的Request Token 
	WEIBO_struct_init_fun(OAUTH_AUTHORIZE),// 请求用户授权Token 
	WEIBO_struct_init_fun(OAUTH_ACCESS_TOKEN),// 获取授权过的Access Token

	WEIBO_struct_init_fun(GET_EMOTIONS),
	WEIBO_struct_init_fun(GET_USERS_SEARCH),
	WEIBO_struct_init_fun(GET_WB_SEARCH),
	WEIBO_struct_init_fun(GET_STATUSES_SEARCH),
	WEIBO_struct_init_fun(GET_PROVINCES),
	WEIBO_struct_init_fun(COOKIE),
	WEIBO_struct_init_fun(UPDATETGT),//UPDATETGT,

	//自定义URL
	WEIBO_struct_init_fun(CUSTOM),

	//user new
	WEIBO_struct_init_fun(GET_USERS_HOT),// 获取系统推荐用户
	WEIBO_struct_init_fun(POST_USERS_REMARK),//更新修改当前登录用户所关注的某个好友的备注信息New!
	WEIBO_struct_init_fun(GET_USERS_SUGGESTIONS),//返回当前用户可能感兴趣的用户

	// 话题接口 ,by welbon,2011-01-10
	WEIBO_struct_init_fun(GET_TRENDS),//trends 获取某人的话题
	WEIBO_struct_init_fun(GET_TRENDS_STATUSES),//trends/statuses 获取某一话题下的微博
	WEIBO_struct_init_fun(POST_TRENDS_FOLLOW),//trends/follow 关注某一个话题
	WEIBO_struct_init_fun(DELETE_TRENDS_DESTROY),//trends/destroy 取消关注的某一个话题
	WEIBO_struct_init_fun(GET_TRENDS_HOURLY),//trends/destroy 按小时返回热门话题
	WEIBO_struct_init_fun(GET_TRENDS_DAYLIY),//trends/daily 按日期返回热门话题。返回某一日期的话题
	WEIBO_struct_init_fun(GET_TRENDS_WEEKLIY),//trends/weekly 按周返回热门话题。返回某一日期之前某一周的话题

	// 黑名单接口 ,by welbon,2011-01-10
	WEIBO_struct_init_fun(POST_BLOCKS_CREATE),//将某用户加入黑名单
	WEIBO_struct_init_fun(POST_BLOCKS_DESTROY),//将某用户移出黑名单
	WEIBO_struct_init_fun(GET_BLOCKS_EXISTS),//检测某用户是否是黑名单用户
	WEIBO_struct_init_fun(GET_BLOCKS_BLOCKING),//列出黑名单用户(输出用户详细信息)
	WEIBO_struct_init_fun(GET_BLOCKS_BLOCKING_IDS),//列出分页黑名单用户（只输出id）

	//用户标签接口 ,by welbon,2011-01-10
	WEIBO_struct_init_fun(GET_TAGS),//tags 返回指定用户的标签列表
	WEIBO_struct_init_fun(POST_TAGS_CREATE),//tags/create 添加用户标签
	WEIBO_struct_init_fun(GET_TAGS_SUGGESTIONS),//tags/suggestions 返回用户感兴趣的标签
	WEIBO_struct_init_fun(POST_TAGS_DESTROY),//tags/destroy 删除标签
	WEIBO_struct_init_fun(POST_TAGS_DESTROY_BATCH),//tags/destroy_batch 批量删除标签

	// 邀请接口
	WEIBO_struct_init_fun(POST_INVITE_MAILCONTACT),//邀请邮箱联系人
	WEIBO_struct_init_fun(POST_INVITE_MSNCONTACT), //邀请MSN联系人
	WEIBO_struct_init_fun(POST_INVITE_SENDMAILS),  //发送邀请邮件

	// Media
	WEIBO_struct_init_fun(GET_MEDIA_SHORTURL_BATCH),  //批量获取

	//XAuth
	WEIBO_struct_init_fun(XAUTH_ACCESS_TOKEN),  //
};


bool Wb_init_wb_struct( int option , void* t_wb)
{
	if( WEIBO_OPTION(UNK) >= option ||
        WEIBO_OPTION(LAST) <= option )
	{
		return false;
	}
	return ( vector_init_callback[ option ] ) ? vector_init_callback[ option ](t_wb):false;
}
