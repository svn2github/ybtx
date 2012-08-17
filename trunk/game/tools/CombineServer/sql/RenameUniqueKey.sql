set autocommit=0;

insert into medi_database.tbl_tong_info (select b.t_uId,b.t_sName from ybtx.tbl_tong a,tbl_tong b where a.t_sName = b.t_sname);
update medi_database.tbl_tong_info set t_sName = concat(t_sName,"&2") where database() = "ybtx_2"; 
update medi_database.tbl_tong_info set t_sName = concat(t_sName,"&3") where database() = "ybtx_3"; 
update medi_database.tbl_tong_info set t_sName = concat(t_sName,"&4") where database() = "ybtx_4"; 
update medi_database.tbl_tong_info a, tbl_tong b
set b.t_sName = a.t_sName
where a.t_uId = b.t_uId;

insert into medi_database.tbl_char_info (select b.cs_uId,b.c_sName from ybtx.tbl_char a,tbl_char b where a.c_sName = b.c_sName);
update medi_database.tbl_char_info set c_sName = concat(c_sName,"@2") where database() = "ybtx_2";
update medi_database.tbl_char_info set c_sName = concat(c_sName,"@3") where database() = "ybtx_3";
update medi_database.tbl_char_info set c_sName = concat(c_sName,"@4") where database() = "ybtx_4";
update medi_database.tbl_char_info a, tbl_char b
set b.c_sName = a.c_sName
where a.cs_uId = b.cs_uId;

commit;
