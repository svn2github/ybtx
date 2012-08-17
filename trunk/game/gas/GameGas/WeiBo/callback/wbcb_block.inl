
//
WEIBO_struct_statuscallback_init(POST_BLOCKS_CREATE)
{

}
WEIBO_struct_bodycallback_init(POST_BLOCKS_CREATE)
{
	printf("   POST_BLOCKS_CREATE : \n    %s \n\n" , body );
}

// 
WEIBO_struct_statuscallback_init(POST_BLOCKS_DESTROY)
{

}
WEIBO_struct_bodycallback_init(POST_BLOCKS_DESTROY)
{
	printf("   POST_BLOCKS_DESTROY : \n    %s \n\n" , body );
}

// 
WEIBO_struct_statuscallback_init(GET_BLOCKS_EXISTS)
{

}
WEIBO_struct_bodycallback_init(GET_BLOCKS_EXISTS)
{
	printf("   GET_BLOCKS_EXISTS : \n    %s \n\n" , body );
}

// 
WEIBO_struct_statuscallback_init(GET_BLOCKS_BLOCKING)
{

}
WEIBO_struct_bodycallback_init(GET_BLOCKS_BLOCKING)
{
	printf("   GET_BLOCKS_BLOCKING : \n    %s \n\n" , body );
}


// 
WEIBO_struct_statuscallback_init(GET_BLOCKS_BLOCKING_IDS)
{

}
WEIBO_struct_bodycallback_init(GET_BLOCKS_BLOCKING_IDS)
{
	printf("   GET_BLOCKS_BLOCKING_IDS : \n    %s \n\n" , body );
}
