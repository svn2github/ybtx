
#在temp库执行的sql,进阶魂根的物品id和soul的对应表
create table tbl_temp_id_soul select a.tea_sAdvanceSoul as soul,  s.is_uId as id
from ybtx.tbl_item_static s, temp_equip_advancesoul a
where s.is_uType = a.tea_uType and s.is_sName = a.tea_sName;

#物品id和强化魂根的对应关系
create table temp_equip_inten_id_soul 
select tes_sIntenSoul as soul , is_uId as id from temp_equip_intensoul, ybtx.tbl_item_static 
where tes_uType = is_uType and tes_sName = is_sName;

#创建拍卖行中订单为装备的相关信息
create table temp_sell_order select i.mos_uId, is_uId, mso_sItemType from ybtx.tbl_item_market i, 
ybtx.tbl_market_sell_order o where i.mos_uId = o.mos_uId and mso_sItemType >= 5 and mso_sItemType <=9;

#装备进阶信息数据库表，添加进阶魂根和进阶属性字段
alter table tbl_item_equip_advance add column iea_sAdvancedSoulRoot varchar(12) after iea_uAdvancedAttrValue4,
add column iea_sAdvancedAttr1	   varchar(48) after iea_sAdvancedSoulRoot,
add column iea_sAdvancedAttr2      varchar(48) after iea_sAdvancedAttr1,
add column iea_sAdvancedAttr3      varchar(48) after iea_sAdvancedAttr2,
add column iea_sAdvancedAttr4      varchar(48) after iea_sAdvancedAttr3;

#装备相关基础数据库表中添加强化魂根字段
alter table tbl_item_weapon 	add column iw_sIntenSoul varchar(6) after iw_uIntensifyQuality;
alter table tbl_item_armor 	add column ia_sIntenSoul varchar(6) after ia_uIntensifyQuality;
alter table tbl_item_shield 	add column is_sIntenSoul varchar(6) after is_uIntensifyQuality;
alter table tbl_item_ring 	add column ir_sIntenSoul varchar(6) after ir_uIntensifyQuality;

#拍卖行中用到装备进阶魂根、强化魂根、可进阶上限、可强化上限，加如下数据库表，满足要求
create table tbl_market_equip_info(
	mos_uId			bigint(20) 	unsigned	not null,
	mei_uIntenQuality	tinyint		unsigned	not null,
	mei_uAdvanceQuality	tinyint 	unsigned	not null,
	mei_sIntenSoul		varchar(12)	not null,
	mei_sAdvanceSoul	varchar(12)	not null,

	key(mos_uId),
	foreign key(mos_uId)	references tbl_market_sell_order(mos_uId) 	on update cascade on delete cascade
)engine = innodb;

#删除拍卖行中装备可进阶上限、可强化上限、进阶魂根、强化魂根
alter table tbl_market_cfg_info drop column mci_uAdvanceSoulRoot,
drop column mci_uMaxAdvancePhase,
drop column mci_uIntenSoulRoot,
drop column mci_uMaxIntenPhase;
