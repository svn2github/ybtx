

alter table tbl_log_item_equip_advance add column liea_sAdvancedSoulRoot varchar(12) after liea_uAdvancedAttrValue4,
add column liea_sAdvancedAttr1	   varchar(48) after liea_sAdvancedSoulRoot,
add column liea_sAdvancedAttr2      varchar(48) after liea_sAdvancedAttr1,
add column liea_sAdvancedAttr3      varchar(48) after liea_sAdvancedAttr2,
add column liea_sAdvancedAttr4      varchar(48) after liea_sAdvancedAttr3;


alter table tbl_log_item_weapon 	add column liw_sIntenSoul varchar(6) after liw_uIntensifyQuality;
alter table tbl_log_item_armor 	add column lia_sIntenSoul varchar(6) after lia_uIntensifyQuality;
alter table tbl_log_item_shield 	add column lis_sIntenSoul varchar(6) after lis_uIntensifyQuality;
alter table tbl_log_item_ring 	add column lir_sIntenSoul varchar(6) after lir_uIntensifyQuality;

insert into tbl_database_upgrade_record values("log_upgrade-0.4.36.0.sql");
