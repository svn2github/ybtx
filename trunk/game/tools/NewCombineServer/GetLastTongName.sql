
drop table if exists tmp_1;
drop table if exists tmp_2;

create table tmp_1 select t_sName, max(t_uLevel) as t_uLevel from tbl_tong_info group by t_sName having count(t_sName)>1;
delete from tbl_tong_info where t_sName not in (select t_sName from tmp_1);

#首先删除同名中等级低的
delete a from tbl_tong_info a, tmp_1 b where a.t_sName=b.t_sName and a.t_uLevel<b.t_uLevel;

#剩下同名且等级相同的情况下，保留最小的服务器id
create table tmp_2 select * from tbl_tong_info;
delete from tbl_tong_info where t_uId not in (select min(t_uId) from tmp_2 group by t_sName);

