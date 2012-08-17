#include <stdafx.h>
#include "option.h"
#include <libWeibo/weibo.h>

struct tagOption
{
	int  iOpt;
	char text[255];
};

#define WEIBO_OPTION_MAP(opt) {WEIBO_OPTION(opt),#opt}

static const tagOption _option_map[] = 
{
	WEIBO_OPTION_MAP(UNK),
	WEIBO_OPTION_MAP(BASE), //1

	//获取下行数据集(timeline)
	WEIBO_OPTION_MAP(GETSTATUSES_PUBLIC_TIMELINE), //2 获取最新更新的公共微博消息 
	WEIBO_OPTION_MAP(GETSTATUSES_FRIENDS_TIMELINE), //3 获取当前用户所关注用户的最新微博信息 (别名: statuses/home_timeline) 
	WEIBO_OPTION_MAP(GETSTATUSES_USE_TIMELINE), //4 获取用户发布的微博信息列表 
	WEIBO_OPTION_MAP(GETSTATUSES_MENTIONS), //5 获取@当前用户的微博列表 
	WEIBO_OPTION_MAP(GETSTATUSES_COMMENTS_TIMELINE), //6 获取当前用户发送及收到的评论列表
	WEIBO_OPTION_MAP(GETSTATUSES_COMMENTS_BYME), //7 获取当前用户发出的评论 
	WEIBO_OPTION_MAP(GETSTATUSES_COMMENTS_TOME), //7 获取当前用户收到的评论 
	WEIBO_OPTION_MAP(GETSTATUSES_COMMENTS_LIST), //8 获取指定微博的评论列表 
	WEIBO_OPTION_MAP(GETSTATUSES_COMMENTS_COUNTS), //9 批量获取一组微博的评论数及转发数 
	WEIBO_OPTION_MAP(GETSTATUSES_UNREAD), //10 获取当前用户未读消息数 
	WEIBO_OPTION_MAP(PUTSTATUSES_RESET_COUNT),//未读消息数清零接口 

	//微博访问
	WEIBO_OPTION_MAP(GETSTATUSES_SHOW), //11 根据ID获取单条微博信息内容 
	WEIBO_OPTION_MAP(GOTOSTATUSES_ID),//12 根据微博ID和用户ID跳转到单条微博页面 
	WEIBO_OPTION_MAP(PUTSTATUSES_UPDATE),//13 发布一条微博信息 
	WEIBO_OPTION_MAP(PUTSTATUSES_UPLOAD),//14 上传图片并发布一条微博信息 
	WEIBO_OPTION_MAP(PUTSTATUSES_DESTROY),//15 删除一条微博信息 
	WEIBO_OPTION_MAP(PUTSTATUSES_REPOST),//16 转发一条微博信息（可加评论） 
	WEIBO_OPTION_MAP(PUTSTATUSES_COMMENT),//17 对一条微博信息进行评论 
	WEIBO_OPTION_MAP(PUTSTATUSES_COMMENT_DESTROY),//18 删除当前用户的微博评论信息 
	WEIBO_OPTION_MAP(PUTSTATUSES_REPLY),//19 回复微博评论信息 

	//用户
	WEIBO_OPTION_MAP(GETUSER_INFO),//20 根据用户ID获取用户资料（授权用户） 
	WEIBO_OPTION_MAP(GETFRINDS_LIST),//21 获取当前用户关注对象列表及最新一条微博信息 
	WEIBO_OPTION_MAP(GETFOLLOWERS_LIST),//22 获取当前用户粉丝列表及最新一条微博信息 

	//私信
	WEIBO_OPTION_MAP(GETDIRECTMSG),//23 获取当前用户最新私信列表 
	WEIBO_OPTION_MAP(GETDIRESTMSG_SENT),//24 获取当前用户发送的最新私信列表
	WEIBO_OPTION_MAP(PUTDIRECTMSG_NEW),//25 发送一条私信 
	WEIBO_OPTION_MAP(PUTDIRECTMSG_DESTROY),//26 删除一条私信 
	WEIBO_OPTION_MAP(GETDIRECTMSG_WITH),//往来私信列表

	//关注
	WEIBO_OPTION_MAP(PUTFRIENDSHIPS_CREATE),//27 关注某用户 
	WEIBO_OPTION_MAP(PUTFRIENDSHIPS_CREATE_BATCH),//批量关注 
	WEIBO_OPTION_MAP(PUTFRIENDSHIPS_DESTROY),//28 取消关注 
	WEIBO_OPTION_MAP(GETFRIENDSHIPS_EXISTS),//29 判断两个用户是否有关注关系，返回两个用户关系的详细情况
	WEIBO_OPTION_MAP(GETFRIENDSHIPS_BATCH_EXISTS),//

	//Social Graph
	WEIBO_OPTION_MAP(GETFRIEND_IDS),//30 关注列表
	WEIBO_OPTION_MAP(GETFOLLOWER_IDS),//31 粉丝列表

	//账号 
	WEIBO_OPTION_MAP(GETACCOUNT_VERIFY),//32 验证当前用户身份是否合法 
	WEIBO_OPTION_MAP(GETACCOUNT_RATELIMIT),//33 获取当前用户API访问频率限制 
	WEIBO_OPTION_MAP(PUTACCOUNT_QUITSESSION),//34 当前用户退出登录 
	WEIBO_OPTION_MAP(PUTACCOUNT_UPDATE_PROFILEIMAGE),//35 更改头像
	WEIBO_OPTION_MAP(PUTACCOUNT_UPDATE_PROFILE),//36 更改资料
	WEIBO_OPTION_MAP(PUTACCOUNT_REGISTER),//37

	// 收藏
	WEIBO_OPTION_MAP(GETFAVORITES),// 38获取当前用户的收藏列表 
	WEIBO_OPTION_MAP(PUTFAVORITES_CREATE),// 39添加收藏 
	WEIBO_OPTION_MAP(PUTFAVORITES_DESTROY),// 40删除当前用户收藏的微博信息 

	//登录/OAuth
	WEIBO_OPTION_MAP(OAUTH_REQUEST_TOKEN),// 41获取未授权的Request Token 
	WEIBO_OPTION_MAP(OAUTH_AUTHORIZE),// 42请求用户授权Token 
	WEIBO_OPTION_MAP(OAUTH_ACCESS_TOKEN),// 43获取授权过的Access Token

	// 表情
	WEIBO_OPTION_MAP(GET_EMOTIONS),// 44 返回新浪微博官方所有表情、魔法表情的相关信息。包括短语、表情类型、表情分类，是否热门等。 

	// 用户搜索 
	WEIBO_OPTION_MAP(GET_USERS_SEARCH),// 45 搜索微博用户,返回关键字匹配的微博用户，(仅对新浪合作开发者开放) 

	// 微博搜索 
	WEIBO_OPTION_MAP(GET_WB_SEARCH),// 46 返回关键字匹配的微博，(仅对新浪合作开发者开放) 
	WEIBO_OPTION_MAP(GET_STATUSES_SEARCH),//47 搜索微博(多条件组合) (仅对合作开发者开放) 

	WEIBO_OPTION_MAP(GET_PROVINCES), // 48 省份城市编码表 

	WEIBO_OPTION_MAP(COOKIE),// 50 cookie
	WEIBO_OPTION_MAP(UPDATETGT), // 51 更新cookie

	//自定义URL
	WEIBO_OPTION_MAP(CUSTOM),//

	WEIBO_OPTION_MAP(GET_USERS_HOT),// 获取系统推荐用户
	WEIBO_OPTION_MAP(POST_USERS_REMARK),//更新修改当前登录用户所关注的某个好友的备注信息New!
	WEIBO_OPTION_MAP(GET_USERS_SUGGESTIONS), //Users/suggestions 返回当前用户可能感兴趣的用户

	// 话题接口 ,by welbon,2011-01-10
	WEIBO_OPTION_MAP(GET_TRENDS),//trends 获取某人的话题
	WEIBO_OPTION_MAP(GET_TRENDS_STATUSES),//trends/statuses 获取某一话题下的微博
	WEIBO_OPTION_MAP(POST_TRENDS_FOLLOW),//trends/follow 关注某一个话题
	WEIBO_OPTION_MAP(DELETE_TRENDS_DESTROY),//trends/destroy 取消关注的某一个话题
	WEIBO_OPTION_MAP(GET_TRENDS_HOURLY),//trends/destroy 按小时返回热门话题
	WEIBO_OPTION_MAP(GET_TRENDS_DAYLIY),//trends/daily 按日期返回热门话题。返回某一日期的话题
	WEIBO_OPTION_MAP(GET_TRENDS_WEEKLIY),//trends/weekly 按周返回热门话题。返回某一日期之前某一周的话题

	// 黑名单接口 ,by welbon,2011-01-10
	WEIBO_OPTION_MAP(POST_BLOCKS_CREATE),//将某用户加入黑名单
	WEIBO_OPTION_MAP(POST_BLOCKS_DESTROY),//将某用户移出黑名单
	WEIBO_OPTION_MAP(GET_BLOCKS_EXISTS),//检测某用户是否是黑名单用户
	WEIBO_OPTION_MAP(GET_BLOCKS_BLOCKING),//列出黑名单用户(输出用户详细信息)
	WEIBO_OPTION_MAP(GET_BLOCKS_BLOCKING_IDS),//列出分页黑名单用户（只输出id）

	//用户标签接口 ,by welbon,2011-01-10
	WEIBO_OPTION_MAP(GET_TAGS),//tags 返回指定用户的标签列表
	WEIBO_OPTION_MAP(POST_TAGS_CREATE),//tags/create 添加用户标签
	WEIBO_OPTION_MAP(GET_TAGS_SUGGESTIONS),//tags/suggestions 返回用户感兴趣的标签
	WEIBO_OPTION_MAP(POST_TAGS_DESTROY),//tags/destroy 删除标签
	WEIBO_OPTION_MAP(POST_TAGS_DESTROY_BATCH),//tags/destroy_batch 批量删除标签

	// 邀请接口
	WEIBO_OPTION_MAP(POST_INVITE_MAILCONTACT),//邀请邮箱联系人
	WEIBO_OPTION_MAP(POST_INVITE_MSNCONTACT), //邀请MSN联系人
	WEIBO_OPTION_MAP(POST_INVITE_SENDMAILS),  //发送邀请邮件

	// Media
	WEIBO_OPTION_MAP(GET_MEDIA_SHORTURL_BATCH),  //批量获取SHORTURL

	//登录/XAuth
	WEIBO_OPTION(XAUTH_ACCESS_TOKEN),//获取授权过的Access Token
	
};

extern void print_weibo_command_list()
{
	printf("\r\nCommand list :\r\n");

	for( int i = 0; i < sizeof(_option_map)/sizeof(_option_map[0]); ++ i ){
		printf("value :%d,option:%s , \r\n",_option_map[i].iOpt,_option_map[i].text);
	}
}

extern int convert_key_to_number(const char* key)
{
	for( int i = 0; i < sizeof(_option_map)/sizeof(_option_map[0]); ++ i ){
		return (strcmp(key,_option_map[i].text) == 0) ? _option_map[i].iOpt : 0 ;
	}
	return 0;
}