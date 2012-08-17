set autocommit=0;

#记录所有服务器里某个用户名的第一个用户ID，这个ID为合并后的最终有效用户ID，以及某些数据（初始都为0），用于合并后重新计算用户的数据
insert ignore into medi_database.tbl_user_info select us_sName,us_uId,0,0,0 from tbl_user_static;

update 
medi_database.tbl_user_info as tmp, 
tbl_user_static as us,
tbl_user_basic as ub 
set 
tmp.ub_uOnlineTime = tmp.ub_uOnlineTime+ub.ub_uOnlineTime,
tmp.ub_uYuanBao = tmp.ub_uYuanBao+ub.ub_uYuanBao
where 
tmp.us_sName = us.us_sName 
and us.us_uId = ub.us_uId;

update 
medi_database.tbl_user_info as tmp, 
tbl_user_static as us,
tbl_user_basic as ub 
set 
tmp.ub_uAgreedVer = ub.ub_uAgreedVer
where 
tmp.us_sName = us.us_sName
and us.us_uId = ub.us_uId
and ub.ub_uAgreedVer > tmp.ub_uAgreedVer;

commit;
