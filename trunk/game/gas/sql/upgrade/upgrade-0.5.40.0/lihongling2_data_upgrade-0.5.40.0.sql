insert into tbl_grid_in_room(cs_uId,gir_uRoomIndex,gir_uPos)
select cs_uId,isr_uRoomIndex,isr_uPos 
from tbl_item_store_room 
group by cs_uId,isr_uRoomIndex,isr_uPos;

insert into tbl_item_in_grid(is_uId,gir_uGridID)
select room.is_uId,igp.gir_uGridID 
from tbl_grid_in_room as igp,tbl_item_store_room as room
where room.isr_uRoomIndex = igp.gir_uRoomIndex and room.isr_uPos = igp.gir_uPos and room.cs_uId = igp.cs_uId;

insert into tbl_grid_info(gir_uGridID,gi_uCount,is_uType,is_sName)
select igp.gir_uGridID,ic.ic_uCount,ic.is_uType,ic.is_sName
from tbl_grid_in_room as igp,tbl_item_count as ic
where igp.cs_uId = ic.cs_uId and igp.gir_uRoomIndex = ic.isr_uRoomIndex and  igp.gir_uPos = ic.isr_uPos;