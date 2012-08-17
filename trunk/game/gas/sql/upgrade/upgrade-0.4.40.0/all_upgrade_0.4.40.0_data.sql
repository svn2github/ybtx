set autocommit=0;
#--------------------------更新sql代码分割线---------------------------------------

update tbl_item_static set is_uType = 43 where is_uType  = 1 and is_sName like "%重铸石%";
update tbl_market_sell_order set mso_sItemType = 43 where mso_sItemName like "%重铸石%" and mso_sItemType = 1;
update tbl_npcshop_player_sell_item set npsi_sItemType = 43 where npsi_sItemName like "%重铸石%" and npsi_sItemType= 1;
update tbl_item_boxitem_pickup set ibp_uType = 43 where ibp_sName like "%重铸石%"  and ibp_uType  = 1;

delete from tbl_item_static where is_sName = "洛克的货箱（艾米）" and is_uType = 24;
delete from tbl_item_static where is_sName = "洛克的货箱（神圣）" and is_uType = 24;
delete from tbl_item_static where is_sName = "洛克的货箱（修斯）" and is_uType = 24;

delete from tbl_item_static where is_sName = "蜘蛛嫩肉";
delete from tbl_item_static where is_sName = "1级佩饰图样";
delete from tbl_item_static where is_sName = "1级戒指模具";
delete from tbl_item_static where is_sName = "2级项链图样";
delete from tbl_item_static where is_sName = "2级戒指模具";
delete from tbl_item_static where is_sName = "陈旧的书套";
delete from tbl_item_static where is_sName = "1级戒指模具";
delete from tbl_item_static where is_sName = "铁皮书套";
delete from tbl_item_static where is_sName = "玉竹蜜酒";
delete from tbl_item_static where is_sName = "1级项链图样";


delete from tbl_item_boxitem_pickup where ibp_sName = "蜘蛛嫩肉";
delete from tbl_item_boxitem_pickup where ibp_sName = "1级佩饰图样";
delete from tbl_item_boxitem_pickup where ibp_sName = "1级戒指模具";
delete from tbl_item_boxitem_pickup where ibp_sName = "1级项链图样";
delete from tbl_item_boxitem_pickup where ibp_sName = "2级项链图样";
delete from tbl_item_boxitem_pickup where ibp_sName = "2级戒指模具";
delete from tbl_item_boxitem_pickup where ibp_sName = "陈旧的书套";
delete from tbl_item_boxitem_pickup where ibp_sName = "1级戒指模具";
delete from tbl_item_boxitem_pickup where ibp_sName = "铁皮书套";
delete from tbl_item_boxitem_pickup where ibp_sName = "玉竹蜜酒";

delete from tbl_shortcut where sc_Arg1 = "蜘蛛嫩肉";
delete from tbl_shortcut where sc_Arg1 = "1级佩饰图样";
delete from tbl_shortcut where sc_Arg1 = "1级戒指模具";
delete from tbl_shortcut where sc_Arg1 = "2级项链图样";
delete from tbl_shortcut where sc_Arg1 = "2级戒指模具";
delete from tbl_shortcut where sc_Arg1 = "陈旧的书套";
delete from tbl_shortcut where sc_Arg1 = "1级戒指模具";
delete from tbl_shortcut where sc_Arg1 = "铁皮书套";
delete from tbl_shortcut where sc_Arg1 = "玉竹蜜酒";
delete from tbl_shortcut where sc_Arg1 = "1级项链图样";

#--------------------------更新sql代码分割线---------------------------------------
insert into tbl_database_upgrade_record values ("all_upgrade_0.4.40.0_data.sql");
commit;
