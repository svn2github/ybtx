#pragma once

#include <libWeibo/weibo.h>
#include "CDynamicObject.h"
#include "ThreadTypes.h"
#include "CallBackX.h"


struct t_wb_allstruct
{
	struct t_wb_oauth  oauth;
	struct t_wb_get_statuses_public_timeline   stat_pub_tl;
	struct t_wb_get_statuses_friends_timeline  stat_fri_tl;
	struct t_wb_get_statuses_user_timeline     stat_usr_tl;
	struct t_wb_get_statuses_mentions          stat_at_mention;
	struct t_wb_get_statuses_comments_timeline stat_com_tl;
	struct t_wb_get_statuses_comments_byme     stat_com_byme;
	struct t_wb_get_statuses_comments_list     stat_coms_list;
	struct t_wb_get_statuses_counts            stat_coms_counts;
	struct t_wb_get_statuses_unread            stat_unread;
	//
	struct t_wb_get_statuses_show              stat_show;
	struct t_wb_goto_user_status_id            usr_statid;
	struct t_wb_put_statuses_update            stat_update;
	struct t_wb_put_statuses_upload            stat_upload;
	struct t_wb_put_statuses_upload_pic        stat_upload_pic;
	struct t_wb_put_statuses_destroy           stat_destroy;
	struct t_wb_put_statuses_repost            stat_repost;
	struct t_wb_put_statuses_comment           stat_comment;
	struct t_wb_put_statuses_comment_destroy   stat_comment_destroy;
	struct t_wb_put_statuses_reply             stat_reply;
	//
	struct t_wb_get_users_show                 usr_show;
	struct t_wb_get_statuses_friends           usr_friends;
	struct t_wb_get_statuses_followers         usr_followers;
	//
	struct t_wb_get_direct_message             dirt_message;
	struct t_wb_get_direct_message_sent        dirt_message_sent;
	struct t_wb_put_direct_message_new         dirt_message_new;
	struct t_wb_put_direct_message_destroy     dirt_message_destroy;
	struct t_wb_get_direct_message_with        dirt_message_with;
	//
	struct t_wb_put_friendships_create         ships_create;
	struct t_wb_put_friendships_create_batch   ships_create_batch;
	struct t_wb_put_friendships_destroy        ships_destroy;
	struct t_wb_get_friendships_show           ships_show;
	//
	struct t_wb_get_friends_ids                friends_ids;
	struct t_wb_get_followers_ids              follower_ids;
	//
	struct t_wb_get_account_verify_credentials account_verify;
	struct t_wb_get_account_rate_limit_status  account_ratelimit;
	struct t_wb_put_account_end_session        account_endsession;
	struct t_wb_put_account_up_profileimage    account_up_image;
	struct t_wb_put_account_up_profile         account_uprofile;
	struct t_wb_put_account_register		   account_register;
	//
	struct t_wb_get_favorites                  favos_list;
	struct t_wb_put_favorites_create           favos_create;
	struct t_wb_put_favorites_destroy          favos_destroy;
	//
	struct t_wb_oauth_request_token            oauth_reqtoken;
	struct t_wb_authorize					   oauth_authorize;
	struct t_wb_oauth_access_token             oauth_accesstoken;

	// 表情
	struct t_wb_emotions                       emotions;
	//
	struct t_wb_users_search                   user_search;
	struct t_wb_search					       search;
	struct t_wb_statuses_search                statuses_search;

	struct t_wb_provinces                      provinces;

	struct t_wb_cookie                         cookie;
	struct t_wb_updateTGT					   updateTGT;

	//
	struct t_wb_users_hot					   users_hot;
	struct t_wb_users_remark				   users_remark;
	struct t_wb_users_suggestions			   users_suggestions;

	// 话题接口
	struct t_wb_trends							trends;
	struct t_wb_trends_statuses					trend_statuses;
	struct t_wb_trends_follow					trend_follow;
	struct t_wb_trends_destroy					trend_destroy;
	struct t_wb_trends_hourly					trend_hourly;
	struct t_wb_trends_daily					trend_daily;
	struct t_wb_trends_weekly					trend_weekly;

	// 黑名单接口
	struct t_wb_blocks_create					block_create;
	struct t_wb_blocks_destroy					block_destroy;
	struct t_wb_blocks_exist					block_exist;
	struct t_wb_blocks_blocking					block_bloking;
	struct t_wb_blocks_blocking_ids				block_ids;

	// 用户标签接口
	struct t_wb_tags							tags;
	struct t_wb_tags_create						tags_create;
	struct t_wb_tags_suggestions				tags_suggestions;
	struct t_wb_tags_destroy					tags_destroy;
	struct t_wb_tags_destroy_batch				tags_destroy_batch;

	// 邀请接口
	struct t_wb_invite_mailcontect						invitec_mail;
	struct t_wb_invite_msncontect						invitec_msn;
	struct t_wb_invite_sendmails						invitec_sendmail;

	//
	struct t_wb_xauth_access_token						xauth_accesstoken;

	//media
	struct t_wb_media_shorturl_batch					media_shorturl_batch;

};

/** oauth copy */
typedef void (*fun_loWeibo_oauth_copy)(struct t_wb_oauth* pdest , const struct t_wb_oauth* psrc);

/** 创建一个请求，每个事件都需要 创建一个请求（一定要通过 weibo_destroy_request（） 进行消毁 */
typedef WEIBORequest* (*fun_loWeibo_request_create)(void);

/** 进行一些参数操作填写，然后执行(回调函数的设置...) */
typedef  int (*fun_loWeibo_request_setopt)(WEIBORequest* request , WEIBOReqoption option , ...);

/** 执行,等待回调数据的执行，其执行结果，会通过 weibo_request_setopt（）设置的方法，告之开发者
*
* @param bAsyn = false 同步执行（线程阻塞 )
*              = true  表示异步
*/
typedef  int (*fun_loWeibo_request_start)(WEIBORequest* request , bool bAsyn);

/** 等待执行（与异步 loWeibo_request_start 执行可以结合 ， 达到同步作用 ）*/
typedef  void (*fun_loWeibo_request_wait)(WEIBORequest* request);

/** 停止 */
typedef  int (*fun_loWeibo_request_stop)(WEIBORequest* request);

/** 消毁这个请求 */
typedef  int (*fun_loWeibo_request_destroy)(WEIBORequest* request);

/** 生成URL信息 */
typedef  int (*fun_loWeibo_get_url)(struct t_wb_URI* outURI , WEIBOoption option , const void* t_wb , const struct t_wb_REQ_EX* req_ex);

struct t_wb_interface
{
	fun_loWeibo_oauth_copy      pfun_oauth_copy;
	fun_loWeibo_request_create  pfun_request_create;
	fun_loWeibo_request_setopt  pfun_request_setopt;
	fun_loWeibo_request_start   pfun_request_start;
	fun_loWeibo_request_wait    pfun_request_wait;
	fun_loWeibo_request_stop    pfun_request_stop;
	fun_loWeibo_request_destroy pfun_request_destroy;
	fun_loWeibo_get_url         pfun_get_url;

	// 动态方式加载 
#if 0//(!defined(WEIBO_STATICLIB)) && (!defined(LIB_loadWEIBOSDK)) &&( (defined(WIN32)) || (defined(WINCE)))
	HMODULE dll_;
#endif
};



class CWeiBoServer:public virtual CDynamicObject
{

private:
	t_wb_interface wb_interface;
	t_wb_allstruct twball;
	WEIBORequest* wbRequest;

public:
	void RequestStart();
	void RetToken(const char* token, const char* secret, int option);
	void GetAccessToken(const char* sAuthCode);
	void GetWeiboInfo(WEIBORequest* wbRequest, int option, struct t_wb_allstruct* pall, const struct t_wb_interface* pwb_interface,void *pUserdata = 0);
	
	virtual void OpenWebBrowser(const char* token){ CALL_CLASS_CALLBACK_1(token) };
	virtual void WeiBoLogin(const char* sUId){ CALL_CLASS_CALLBACK_1(sUId) };
	virtual void LoginFaild(){ CALL_CLASS_CALLBACK() };

	CWeiBoServer(void);
	~CWeiBoServer(void);
};
