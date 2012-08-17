//////////////////////////////////////////////////////////////////////////////////////////////
// 邀请接口 ,by welbon,2011-01-10

WEIBO_struct_statuscallback_init(POST_INVITE_MAILCONTACT)
{

}
WEIBO_struct_bodycallback_init(POST_INVITE_MAILCONTACT)
{
	printf("   POST_INVITE_MAILCONTACT : \n    %s \n\n" , body );

	int iOutCount = 0;
	wbParserNS::t_wbParse_InviteContact *pInviteContact = NULL;
	wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC( InviteContact,Mail,body,len,iOutCount,(void**)&pInviteContact,NULL,NULL );
	if( handle ){
		wbParserNS::USE_WBFREE_FUNC(InviteContact,Mail,handle);
	}
}


//tags/create 添加用户标签
WEIBO_struct_statuscallback_init(POST_INVITE_MSNCONTACT)
{

}
WEIBO_struct_bodycallback_init(POST_INVITE_MSNCONTACT)
{
	printf("   POST_INVITE_MSNCONTACT : \n    %s \n\n" , body );

	int iOutCount = 0;
	wbParserNS::t_wbParse_InviteContact *pInviteContact = NULL;
	wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC( InviteContact,MSN,body,len,iOutCount,(void**)&pInviteContact,NULL,NULL );
	if( handle ){
		wbParserNS::USE_WBFREE_FUNC(InviteContact,MSN,handle);
	}

}


//tags/suggestions 返回用户感兴趣的标签
WEIBO_struct_statuscallback_init(POST_INVITE_SENDMAILS)
{

}
WEIBO_struct_bodycallback_init(POST_INVITE_SENDMAILS)
{
	printf("   POST_INVITE_SENDMAILS : \n    %s \n\n" , body );

	int iOutCount = 0;
	wbParserNS::t_wbParse_Ret *pRet = NULL;
	wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC( InviteContact,SendMail,body,len,iOutCount,(void**)&pRet,NULL,NULL );
	if( handle ){
		wbParserNS::USE_WBFREE_FUNC(InviteContact,SendMail,handle);
	}
}