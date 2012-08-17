////////////////////////////////////////////////////////////////////////
//
// 话题接口 ,by welbon,2011-01-10

//trends 获取某人的话题
WEIBO_struct_statuscallback_init(GET_TRENDS)
{
}
WEIBO_struct_bodycallback_init(GET_TRENDS)
{
	printf("   GET_TRENDS : \n    %s \n\n" , body );
}

//trends/statuses 获取某一话题下的微博
WEIBO_struct_statuscallback_init(GET_TRENDS_STATUSES)
{
}
WEIBO_struct_bodycallback_init(GET_TRENDS_STATUSES)
{
	printf("   GET_TRENDS_STATUSES : \n    %s \n\n" , body );
}

//trends/follow 关注某一个话题
WEIBO_struct_statuscallback_init(POST_TRENDS_FOLLOW)
{
}
WEIBO_struct_bodycallback_init(POST_TRENDS_FOLLOW)
{
	printf("   POST_TRENDS_FOLLOW : \n    %s \n\n" , body );
}

//trends/destroy 取消关注的某一个话题
WEIBO_struct_statuscallback_init(DELETE_TRENDS_DESTROY)
{
}
WEIBO_struct_bodycallback_init(DELETE_TRENDS_DESTROY)
{
	printf("   DELETE_TRENDS_DESTROY : \n    %s \n\n" , body );
}


//trends/hourly 按小时返回热门话题
WEIBO_struct_statuscallback_init(GET_TRENDS_HOURLY)
{
}
WEIBO_struct_bodycallback_init(GET_TRENDS_HOURLY)
{
	printf("   GET_TRENDS_HOURLY : \n    %s \n\n" , body );
}


//trends/daily 按日期返回热门话题。返回某一日期的话题
WEIBO_struct_statuscallback_init(GET_TRENDS_DAYLIY)
{

}
WEIBO_struct_bodycallback_init(GET_TRENDS_DAYLIY)
{
	printf("   GET_TRENDS_DAYLIY : \n    %s \n\n" , body );
}


//trends/weekly 按周返回热门话题。返回某一日期之前某一周的话题
WEIBO_struct_statuscallback_init(GET_TRENDS_WEEKLIY)
{

}
WEIBO_struct_bodycallback_init(GET_TRENDS_WEEKLIY)
{
	printf("   GET_TRENDS_WEEKLIY : \n    %s \n\n" , body );
}