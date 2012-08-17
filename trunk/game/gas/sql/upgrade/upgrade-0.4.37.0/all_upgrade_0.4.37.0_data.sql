set autocommit=0;
#--------------------------更新sql代码分割线---------------------------------------
create table tbl_linshibiao as select cs_uId from tbl_expert_smithing 
where es_uType in("配饰","戒指","项链");

select count(*) from tbl_commerce_skill as cs,tbl_linshibiao as tl
where cs.cs_uId = tl.cs_uId
and cs.cs_sSkillName in("锻造","铸甲");

update tbl_commerce_skill as cs,tbl_linshibiao as tl
set cs.cs_sSkillName = "工艺"
where cs.cs_uId = tl.cs_uId
and cs.cs_sSkillName in("锻造","铸甲");

select count(*) from  tbl_specialize_smithing as ss,tbl_linshibiao as tl
where ss.cs_uId = tl.cs_uId;

update tbl_specialize_smithing as ss,tbl_linshibiao as tl
set ss.ss_uSkillType = 3
where ss.cs_uId = tl.cs_uId;

update tbl_expert_smithing as es,tbl_linshibiao as tl
set es.es_uSkillType = 3
where es.cs_uId = tl.cs_uId;

drop table tbl_linshibiao;

delete from tbl_item_static where is_sName = "蜘蛛嫩肉";
delete from tbl_item_static where is_sName = "1级佩饰图样";
delete from tbl_item_static where is_sName = "1级戒指模具";
delete from tbl_item_static where is_sName = "2级项链图样";
delete from tbl_item_static where is_sName = "2级戒指模具";
delete from tbl_item_static where is_sName = "陈旧的书套";
delete from tbl_item_static where is_sName = "1级戒指模具";
delete from tbl_item_static where is_sName = "铁皮书套";
delete from tbl_item_static where is_sName = "玉竹蜜酒";

#--------------------------更新sql代码分割线---------------------------------------
insert into tbl_database_upgrade_record values ("all_upgrade_0.4.37.0_data.sql");
commit;

