
drop table if exists tmp_1;
drop table if exists tmp_2;

create table tmp_1 select ac_sName, max(ac_uLevel) as ac_uLevel from tbl_army_info group by ac_sName having count(ac_sName)>1;
delete from tbl_army_info where ac_sName not in (select ac_sName from tmp_1); 


delete a from tbl_army_info a, tmp_1 b where a.ac_sName=b.ac_sName and a.ac_uLevel<b.ac_uLevel;

create table tmp_2 select * from tbl_army_info;
delete from tbl_army_info where ac_uId not in (select min(ac_uId) from tmp_2 group by ac_sName);
