insert into tbl_tong_get_proffer_time
(select * from tbl_tong_get_proffer_time_temp where tgptt_sTime > date(now()));

