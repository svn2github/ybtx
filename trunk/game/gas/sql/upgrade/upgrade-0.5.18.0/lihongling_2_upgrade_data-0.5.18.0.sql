insert into tbl_item_count 
    	select 
    		cs_uId,isr_uRoomIndex,isr_uPos,count(1),item.is_uType,item.is_sName
    	from 
    		tbl_item_store_room as room,
    		tbl_item_static as item 
    	where 
    		item.is_uId = room.is_uId
    	group by 
    		cs_uId,
    		isr_uRoomIndex,
    		isr_uPos;