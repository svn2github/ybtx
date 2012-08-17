
drop table if exists tmp_1;
drop table if exists tmp_2;

create table tmp_1 select c_sName, max(cb_uLevel) as cb_uLevel from tbl_char_info group by c_sName having count(c_sName)>1;
delete from tbl_char_info where c_sName not in (select c_sName from tmp_1);

delete a from tbl_char_info a, tmp_1 b where a.c_sName=b.c_sName and a.cb_uLevel<b.cb_uLevel;

create table tmp_2 select * from tbl_char_info;
delete from tbl_char_info where cs_uId not in (select min(cs_uId) from tmp_2 group by c_sName);
