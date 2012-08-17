/////////////////////////////////////////////////////////////////////
// 热点信息
// 
WEIBO_struct_statuscallback_init(HOT_REPOST_DAYLIY)
{

}


WEIBO_struct_bodycallback_init(HOT_REPOST_DAYLIY)
{
	printf("   HOT_REPOST_DAYLIY : \n    %s \n\n" , body );
}


// 
WEIBO_struct_statuscallback_init(HOT_REPOST_WEEKLIY)
{

}

WEIBO_struct_bodycallback_init(HOT_REPOST_WEEKLIY)
{
	printf("   HOT_REPOST_WEEKLIY : \n    %s \n\n" , body );
}


// 
WEIBO_struct_statuscallback_init(HOT_COMMENT_DAYLIY)
{

}

WEIBO_struct_bodycallback_init(HOT_COMMENT_DAYLIY)
{
	printf("   HOT_COMMENT_DAYLIY : \n    %s \n\n" , body );
}

// 
WEIBO_struct_statuscallback_init(HOT_COMMENT_WEEKLY)
{

}
WEIBO_struct_bodycallback_init(HOT_COMMENT_WEEKLY)
{
	printf("   HOT_COMMENT_WEEKLY : \n    %s \n\n" , body );
}