//////////////////////////////////////////////////////////////////////////////////////////////
// 用户标签接口 ,by welbon,2011-01-10

//tags 返回指定用户的标签列表
WEIBO_struct_statuscallback_init(GET_TAGS)
{

}
WEIBO_struct_bodycallback_init(GET_TAGS)
{
	printf("   GET_TAGS : \n    %s \n\n" , body );
}


//tags/create 添加用户标签
WEIBO_struct_statuscallback_init(POST_TAGS_CREATE)
{

}
WEIBO_struct_bodycallback_init(POST_TAGS_CREATE)
{
	printf("   POST_TAGS_CREATE : \n    %s \n\n" , body );
}


//tags/suggestions 返回用户感兴趣的标签
WEIBO_struct_statuscallback_init(GET_TAGS_SUGGESTIONS)
{

}
WEIBO_struct_bodycallback_init(GET_TAGS_SUGGESTIONS)
{
	printf("   GET_TAGS_SUGGESTIONS : \n    %s \n\n" , body );
}

//tags/destroy 删除标签
WEIBO_struct_statuscallback_init(POST_TAGS_DESTROY)
{

}
WEIBO_struct_bodycallback_init(POST_TAGS_DESTROY)
{
	printf("   POST_TAGS_DESTROY : \n    %s \n\n" , body );
}

//tags/destroy_batch 批量删除标签
WEIBO_struct_statuscallback_init(POST_TAGS_DESTROY_BATCH)
{

}
WEIBO_struct_bodycallback_init(POST_TAGS_DESTROY_BATCH)
{
	printf("   POST_TAGS_DESTROY_BATCH : \n    %s \n\n" , body );
}

