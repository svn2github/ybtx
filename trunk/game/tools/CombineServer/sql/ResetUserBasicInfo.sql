update tbl_user_basic as ub,
tbl_user_static as us,
medi_database.tbl_user_info as tmp 
set 
ub.ub_uOnlineTime = tmp.ub_uOnlineTime,
ub.ub_uYuanBao = tmp.ub_uYuanBao,
ub.ub_uAgreedVer = tmp.ub_uAgreedVer
where us.us_uId = ub.us_uId
and us.us_sName = tmp.us_sName;