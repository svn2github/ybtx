

#清空商店出售表里的物品
delete im from tbl_player_sell_item_id diit,tbl_item_static im where diit.psii_nItemId = im.is_uId;
#清空商店出售物品静态表
delete from tbl_npcshop_player_sell_item;

#创建要删的物品临时表
create table tbl_del_item_id_temp(is_uId bigint not null, primary key(is_uId));

#将不在包裹里的物品放入临时表
insert into tbl_del_item_id_temp select ist.is_uId 
from tbl_item_static ist left join tbl_item_in_grid iig on ist.is_uId = iig.is_uId where iig.is_uId is null; 

#从临时表删除在订单里的物品
delete diit from tbl_del_item_id_temp diit,tbl_item_market im where diit.is_uId = im.is_uId;

#从临时表删除在代工订单里的物品
delete diit from tbl_del_item_id_temp diit,tbl_contract_manufacture_order_item im where diit.is_uId = im.is_uId;

#从临时表删除在邮件里的物品
delete diit from tbl_del_item_id_temp diit,tbl_mail_item im where diit.is_uId = im.is_uId;

#从临时表删除在身上的装备物品
delete diit from tbl_del_item_id_temp diit,tbl_item_equip im where diit.is_uId = im.is_uId;

#从临时表删除在仓库的物品
delete diit from tbl_del_item_id_temp diit,tbl_item_collectivity_depot im where diit.is_uId = im.is_uId;

#从临时表删除被使用的背包物品
delete diit from tbl_del_item_id_temp diit,tbl_item_bag_in_use im where diit.is_uId = im.is_uId;

#将要删的物品从静态表里删除
delete im from tbl_del_item_id_temp diit,tbl_item_static im where diit.is_uId = im.is_uId;


##重新计算物品数量
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