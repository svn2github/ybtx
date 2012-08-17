set autocommit=0;

select @nowdate:=now();
update tbl_char_onlinetime set co_dtLastLoginTime=@nowdate,co_dtLastLogOutTime=@nowdate;
#update tbl_char_deleted set cd_dtDelTime=@nowdate;

commit;
