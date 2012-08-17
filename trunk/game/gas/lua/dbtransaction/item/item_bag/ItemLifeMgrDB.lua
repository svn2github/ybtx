gac_gas_require "item/item_info_mgr/ItemInfoMgr"
local g_ItemInfoMgr = CItemInfoMgr:new()		

local StmtOperater = {}	
local ItemLifeMgrDB = CreateDbBox(...)

local StmtDef=
{
    "_InsertIntoLife",
    "insert into tbl_item_life(is_uId,il_nLeftTime,il_nLoadTime,il_uStyle) values(?,?,now(),?);" 
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetOneItemLife",
    [[
    	select il_nLeftTime,il_uStyle,unix_timestamp(il_nLoadTime),unix_timestamp(now())  
    	from tbl_item_life where is_uId = ?
    ]] 
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetLifeById",
    "select count(*) from tbl_item_life where is_uId = ?;" 
}
DefineSql(StmtDef,StmtOperater)
local StmtDef=
{
    "_InsertIntoLifeByDBtime",
    "insert into tbl_item_life(is_uId,il_nLeftTime,il_nLoadTime,il_uStyle) values(?,unix_timestamp(now()),now(),?);" 
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetItemLeftTimeByChar",
    [[
    	select 
    			il.is_uId,tis.is_uType,tis.is_sName,il.il_nLeftTime,il.il_uStyle,unix_timestamp(il.il_nLoadTime),unix_timestamp(now())
    	from 
    			tbl_item_life as il,tbl_item_in_grid as isr,tbl_item_static as tis,tbl_grid_in_room igp
    	where 
    		il.is_uId = isr.is_uId
   		and 
    		isr.is_uId = tis.is_uId
    	and isr.gir_uGridID = igp.gir_uGridID
    	and igp.cs_uId = ?
    	
    	union 
    
    	select 
    		il.is_uId,tis.is_uType,tis.is_sName,il.il_nLeftTime,il.il_uStyle,unix_timestamp(il.il_nLoadTime),unix_timestamp(now())
    	from 
    		tbl_item_life as il,tbl_item_equip as ie,tbl_item_static as tis
    	where
    		il.is_uId = ie.is_uId
    	and
    		ie.is_uId = tis.is_uId
    	and ie.cs_uId = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_CleanUpItemLife",
    [[
    	update tbl_item_life set  il_nLeftTime = 0
    	where is_uId = ?
    ]] 
}
DefineSql(StmtDef,StmtOperater)

function ItemLifeMgrDB.CleanUpItemLife(nItemID)
	StmtOperater._CleanUpItemLife:ExecStat(nItemID)
end

local StmtDef=
{
    "_GetOnlyItemLeftTime",
    [[
    	select il_nLeftTime from  tbl_item_life
    	where is_uId = ?
    ]] 
}
DefineSql(StmtDef,StmtOperater)
function ItemLifeMgrDB.GetOnlyItemLeftTime(nItemID)
	local res = StmtOperater._GetOnlyItemLeftTime:ExecStat(nItemID)
	if res:GetRowNum() == 0 then return end
	return res:GetData(0,0)
end

function ItemLifeMgrDB.GetItemLeftTimeByPlayer(data)
	local nCharID = data["char_id"]
	return ItemLifeMgrDB.GetItemLeftTimeByChar(nCharID)
end

function ItemLifeMgrDB.GetItemLeftTimeByChar(nCharID)
	local nLifeStyle = 1  --在线计时
	--修改剩余时间
	StmtOperater._UpdateOnlineItemLifeInfo:ExecSql('',nCharID,nLifeStyle)
	--修改所有的物品加载时间
	StmtOperater._UpdateItemLoadTime:ExecSql('',nCharID)
	local query_result = StmtOperater._GetItemLeftTimeByChar:ExecStat(nCharID,nCharID)
  local row = query_result:GetRowNum()
	local res = {}
	for i = 1, row do
		local nItemID,nItemType,sItemName,nLeftTime,nTimeStyle,nLoadTime,now_time = query_result:GetData(i-1,0),query_result:GetData(i-1,1),query_result:GetData(i-1,2),
							query_result:GetData(i-1,3),query_result:GetData(i-1,4),query_result:GetData(i-1,5),query_result:GetData(i-1,6)
		local LifeTable = g_ItemInfoMgr:GetItemLifeInfo(nItemType,sItemName)
		if LifeTable  then
			local uLifcycle = tonumber(LifeTable("Lifecycle"))
			if uLifcycle and uLifcycle > 0 then
				if not(nLeftTime == 0) then
					if nLeftTime > 0 then
						if nTimeStyle == 2 then
								--如果是持续计时
							nLeftTime = (LifeTable("Lifecycle") or 0) -(now_time - nLeftTime)
						else
							--如果是在线计时，则用剩余时间减去玩家在线时间
							nLeftTime = nLeftTime - (now_time - nLoadTime)
						end
						if nLeftTime < 0 then
							nLeftTime = 0
							ItemLifeMgrDB.CleanUpItemLife(nItemID)
						end
					else
						nLeftTime = 0
						ItemLifeMgrDB.CleanUpItemLife(nItemID)
					end
				end
				table.insert(res,{nItemID,nLeftTime, nItemType,sItemName})
			end
		end
	end
	return res
end

function ItemLifeMgrDB.AddItemLifeInfo(sBigID,sIndex,ItemID,nStartType)
	--生命周期相关
	local LifeTable = g_ItemInfoMgr:GetItemLifeInfo(sBigID,sIndex)
	if LifeTable and (nStartType == LifeTable("TimeStartType")) then
		--TimeStartType==0说明产出的时候就开始计时
		local uLifcycle = tonumber(LifeTable("Lifecycle"))
		if uLifcycle and uLifcycle > 0 then
			local nLeftTime,nStyle = uLifcycle,tonumber(LifeTable("LifeStyle")) or 2
			local tbl_count = StmtOperater._GetLifeById:ExecStat(ItemID)
			if tbl_count(1,1) == 0 then
				if nStyle == 2 then
					--持续计时
					StmtOperater._InsertIntoLifeByDBtime:ExecSql("",ItemID,nStyle)
				else
					StmtOperater._InsertIntoLife:ExecSql("",ItemID,nLeftTime,nStyle)
				end
				if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
					return 
				end
			end
		end
	end	
	return true
end

--得到某物品的剩余时间
function ItemLifeMgrDB.GetOneItemLife(nItemID,ItemType,ItemName)
	local LifeTable = g_ItemInfoMgr:GetItemLifeInfo(ItemType,ItemName)
	local life = -1
	if LifeTable  then
		local uLifcycle = tonumber(LifeTable("Lifecycle"))
		if uLifcycle and uLifcycle > 0 then
		 	local info = StmtOperater._GetOneItemLife:ExecSql("nnnn",nItemID)
		 	if info:GetRowNum() == 0 then 
		 		return -1 
		 	end
		 	local nTimeLeft,nTimeStyle,nLoadTime,now_time = info:GetNumber(0,0),info:GetNumber(0,1),info:GetNumber(0,2),info:GetNumber(0,3)
			if not (0 == nTimeLeft) then
				if nTimeLeft > 0 then
					if nTimeStyle == 2 then
						--如果是持续计时
						nTimeLeft = (LifeTable("Lifecycle") or 0) -(now_time - nTimeLeft)
					else
						--如果是在线计时，则用剩余时间减去玩家在线时间
						nTimeLeft = nTimeLeft - (now_time - nLoadTime)
					end
					if nTimeLeft < 0 then
						nTimeLeft = 0
						ItemLifeMgrDB.CleanUpItemLife(nItemID)
					end
				else
					nTimeLeft = 0
					ItemLifeMgrDB.CleanUpItemLife(nItemID)
				end
			end
			life = nTimeLeft
		end
	end
	return life
end

StmtDef=
{
    "_GetItemTypeAndPosByID",
    [[ 
   	select 
   		item.is_uType,item.is_sName,life.il_nLeftTime,life.il_uStyle,unix_timestamp(life.il_nLoadTime)
    from	tbl_item_life as life
    	join 
    		tbl_item_static as item 
    			on life.is_uId = item.is_uId 
    where 
    	life.is_uId = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)
function ItemLifeMgrDB.GetItemLifeInfoByID(uItemId)
	local res = StmtOperater._GetItemTypeAndPosByID:ExecStat(uItemId )
	return res
end

--得到所有有生命周期的计时类型为持续计时的物品信息，2为持续计时
local StmtDef=
{
    "_UpdateOnlineItemLifeInfo",
    [[
    	update 	tbl_item_life as life,
    					tbl_item_in_grid as room,
    					tbl_char_onlinetime as online,
    					tbl_grid_in_room as igp	
    	set 		
    				life.il_nLeftTime = life.il_nLeftTime - (online.co_dtLastLogOutTime - life.il_nLoadTime)
    	where 	
    				life.is_uId = room.is_uId
    		and igp.gir_uGridID = room.gir_uGridID
    		and	igp.cs_uId = online.cs_uId 
    		and 
    				igp.cs_uId = ?
    		and 
    				life.il_uStyle = ?
   	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_UpdateItemLoadTime",
    [[
    	update 	tbl_item_life as life,tbl_item_in_grid as room,tbl_grid_in_room as igp	
    	set    life.il_nLoadTime = now()
    	where 	
    			life.is_uId = room.is_uId
    		and igp.gir_uGridID = room.gir_uGridID
    		and
    			igp.cs_uId = ?
   	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_UpdateItemLeftTime",
    [[
    	update 	tbl_item_life as life,tbl_item_in_grid as room,tbl_grid_in_room as igp	
    	set    life.il_nLeftTime = ?
    	where 	
    			life.is_uId = room.is_uId
    		and
    			igp.gir_uGridID = room.gir_uGridID
    		and
    		 	life.il_nLeftTime < 0
    		and
    			igp.cs_uId = ?
   	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetDBTime",
    [[
    	select unix_timestamp(now())
   	]]
}
DefineSql(StmtDef,StmtOperater)
function ItemLifeMgrDB.GetUnixTimeFromDB()
	local tbl = StmtOperater._GetDBTime:ExecSql('n')
	return tbl:GetNumber(0,0)
end

--【判断某物品是否过期】
function ItemLifeMgrDB.JudgeItemTime(nType,sName,nTimeLeft,nTimeStyle,nLoadTime)

	local PropTable = g_ItemInfoMgr:GetItemLifeInfo(nType,sName)
	if PropTable == nil then
		--说明不是计时物品
		return true
	end
	local now_time = ItemLifeMgrDB.GetUnixTimeFromDB()
	if not(nTimeLeft == 0) then
		if nTimeStyle == 2 then
			--如果是持续计时
			nTimeLeft = (PropTable("Lifecycle") or 0) -(now_time - nTimeLeft)
		else
			--如果是在线计时，则用剩余时间减去玩家在线时间
			nTimeLeft = nTimeLeft - (now_time - nLoadTime)
		end
	end
	return nTimeLeft > 0
end

function ItemLifeMgrDB.IsTimeOutItem(nItemID)
	local item_info = ItemLifeMgrDB.GetItemLifeInfoByID(nItemID)
	if item_info:GetRowNum() == 0 then
		--说明不是计时物品
		return true
	end
	local nType,sName = tonumber(item_info(1,1)),item_info(1,2)
	local nTimeLeft,nTimeStyle,nLoadTime = item_info(1,3),item_info(1,4),item_info(1,5)
	local bFlag = ItemLifeMgrDB.JudgeItemTime(nType,sName,nTimeLeft,nTimeStyle,nLoadTime)
	if not bFlag then
		ItemLifeMgrDB.CleanUpItemLife(nItemID)
	end
	return bFlag
end

StmtDef=
{
    "_GetItemLifeByPos",
    [[ 
   	select 
   		item.is_uType,item.is_sName,life.il_nLeftTime,life.il_uStyle,unix_timestamp(life.il_nLoadTime)
    from	tbl_item_life as life
    	join
    		tbl_item_in_grid as room 
    			on	life.is_uId = room.is_uId
    	join tbl_grid_in_room as igp	
    		on igp.gir_uGridID = room.gir_uGridID
    	join 
    		tbl_item_static as item 
    			on room.is_uId = item.is_uId 
    where igp.cs_uId = ? 
    	and igp.gir_uRoomIndex = ?
    	and igp.gir_uPos = ?
    	limit 1
    ]]
}
DefineSql(StmtDef,StmtOperater)

function ItemLifeMgrDB.GetItemLifeByPos(nCharID,Index,Pos)
	local query_res = StmtOperater._GetItemLifeByPos:ExecStat(nCharID,Index,Pos)

	return query_res
end
return ItemLifeMgrDB
