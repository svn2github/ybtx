###G:/ybtxver/online_stable/code/game/gas/sql/upgrade/game_upgrade/icpeoly/1_icepoly_upgrade.sql###
delete from tbl_market_purchasing_cfg_info;


###G:/ybtxver/online_stable/code/game/gas/sql/upgrade/game_upgrade/lihongling/6_lihongling_data_upgrade.sql###
delete from tbl_grid_info;
insert into tbl_grid_info(gir_uGridID,gi_uCount,is_uType,is_sName)
    	select 
    		gir_uGridID,count(1),item.is_uType,item.is_sName
    	from 
    		tbl_item_in_grid as room,
    		tbl_item_static as item 
    	where 
    		item.is_uId = room.is_uId
    	group by 
    		gir_uGridID;

delete gir from tbl_grid_in_room as gir left join tbl_grid_info as gi 
on(gi.gir_uGridID = gir.gir_uGridID) where  gi.gir_uGridID is null;




###G:/ybtxver/online_stable/code/game/gas/sql/upgrade/game_upgrade/lihongling/7_lihongling_upgrade.sql###
alter table tbl_gm_compenate add column gc_uBeginTime		bigint unsigned not null after gc_uItemName;