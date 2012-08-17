gac_gas_require "item/item_info_mgr/ItemInfoMgr"

local CGasBattleArrayBooks = class()
local g_ItemInfoMgr = CItemInfoMgr:new()
local event_type_tbl = event_type_tbl

local BattleArrayBooksSql = CreateDbBox(...)
local StmtDef = {
			"_SaveBattleShape",
			--保存阵型信息
			[[ 
				replace into tbl_battle_shape(cs_uId,bs_sName,bs_uPos1,bs_uPos2,bs_uPos3,bs_uPos4,bs_uPos5,bs_uFlag) values(?,?,?,?,?,?,?,?)
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_UpdateBattleShape",
			--更新阵型信息(根据玩家ID和阵法名称)
			[[
				update tbl_battle_shape set bs_uPos1=? , bs_uPos2=? , bs_uPos3=? , bs_uPos4=? , bs_uPos5=?,bs_uFlag = 1 where cs_uId=? and bs_sName=?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)


local StmtDef = {
			"_QueryBattleShape",
			--查询阵型信息(根据玩家ID和阵法名称)
			[[ 
					select bs_uId,bs_uPos1, bs_uPos2,bs_uPos3, bs_uPos4,bs_uPos5 ,bs_uFlag  from tbl_battle_shape where cs_uId=? and bs_sName=? 
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_SaveArrayBattle",
			--保存列阵信息
			[[ 
				insert into	tbl_battle_array(bs_uId,t_uId,ba_uLoc1,ba_uLoc2,ba_uLoc3,ba_uLoc4,ba_uLoc5) values(?,?,?,?,?,?,?)
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_UpdateArrayBattle",
			--更新列阵信息(根据流水号和小队ID)
			[[ 
				update tbl_battle_array set ba_uLoc1=? , ba_uLoc2=? , ba_uLoc3=? , ba_uLoc4=? ,ba_uLoc5=?  where bs_uId=? and t_uId=?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_QueryArrayBattle",
			--查询列阵信息(根据流水号和小队ID)
			[[ 
				select ba_uLoc1,ba_uLoc2,ba_uLoc3,ba_uLoc4,ba_uLoc5 from	tbl_battle_array where bs_uId=? and t_uId=?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_QueryBattleName",
			--查询阵型名称(根据流水号)
			[[ 
				select bs_sName from tbl_battle_shape where bs_uId=?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_DeleteArrayBattle",
			--删除列阵信息(根据流水号和小队ID)
			[[ 
				delete from	tbl_battle_array where bs_uId = ? and t_uId = ?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_UpdateBattleShapeFlag",
			--设置标志变量(根据小队ID和阵法名称)
			[[ 
				update tbl_battle_shape set bs_uFlag = ? where cs_uId=? and bs_sName= ?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)


local StmtDef = {
			"_UpdateBattleBook",
			--更新阵法书(根据阵法书ID)
			[[ 
				update tbl_item_battlebook set ib_uPos1 = ?,ib_uPos2 = ?,ib_uPos3 = ?,ib_uPos4 = ?,ib_uPos5=? where is_uId = ?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_DeleteBattleBook",
			--删除阵型信息(根据物品ID删除物品静态表)
			[[ 
				delete from	tbl_item_battlebook where is_uId = ?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_SelectArrayBook",
			--查询阵法书的动态信息
			[[ 
				select is_uId,ib_uPos1,ib_uPos2,ib_uPos3,ib_uPos4,ib_uPos5 from tbl_item_battlebook where is_uId = ?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_SaveSkillBattle",
			--保存新技能
			[[
				replace into tbl_battle_skill_icon values(?,?,?)
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

local StmtDef = {
			"_DeleteSkillBattle",
			--更新技能
			[[ 
				delete from tbl_battle_skill_icon where cs_uId = ? and bsi_uPos = ?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)

------------------------------------------------------------------------
-- @brief 保存技能
function BattleArrayBooksSql.SaveSkillBattle(data)
	local b_name = data["b_name"]
	local b_pos = data["b_pos"]
	local playerId = data["playerId"]
	if b_pos >= 1 and b_pos <= 8 then
		if b_name == "" then
			CGasBattleArrayBooks._DeleteSkillBattle:ExecSql("",playerId,b_pos)
			return g_DbChannelMgr:LastAffectedRowNum() > 0
		else
			CGasBattleArrayBooks._SaveSkillBattle:ExecSql("",playerId,b_pos,b_name)
			return g_DbChannelMgr:LastAffectedRowNum() > 0
		end
	else
		return 110017
	end
end
-------------------------------------------------------------------------
--通过物品ID查询物品动态表(根据物品ID)
function BattleArrayBooksSql.SelectBattleBookByID(ItemId)
	local res = CGasBattleArrayBooks._SelectArrayBook:ExecStat(ItemId)
	local tbl = {}
	if res:GetRowNum()>0 then
		tbl[1] = res:GetData(0,0)
		tbl[2] = res:GetData(0,1)
		tbl[3] = res:GetData(0,2)
		tbl[4] = res:GetData(0,3)
		tbl[5] = res:GetData(0,4)
		tbl[6] = res:GetData(0,5)
	end
	return tbl
end
--------------------------------------------------------------------------
--查询阵型信息具体实现(根据玩家ID和阵法名称)
function BattleArrayBooksSql.SelectBattleShape(data)
	local PlayerId = tonumber(data["PlayerId"])
	local b_sName = tostring(data["b_sName"])
	local tbl = {}
	
	if PlayerId ~= nil and PlayerId > 0 and assert(IsString(b_sName)) then
		local res = CGasBattleArrayBooks._QueryBattleShape:ExecSql("nnnnnnn",PlayerId,b_sName)
		if nil ~= res and res:GetRowNum()>0 then   
			tbl[1] = res:GetData(0,0)
			tbl[2] = res:GetData(0,1)
			tbl[3] = res:GetData(0,2)
			tbl[4] = res:GetData(0,3)
			tbl[5] = res:GetData(0,4)
			tbl[6] = res:GetData(0,5)
			tbl[7] = res:GetData(0,6)
		end
	end
	return tbl
end
--------------------------------------------------------------------------
--查询列阵信息(根据阵法流水号和小队ID)
function BattleArrayBooksSql.SelectArrayBattleInfo(data)
	local PlayerId = tonumber(data["PlayerId"])
	local b_sName = tostring(data["b_sName"])
	local b_uId = data["b_uId"]
	
	local queryData = {
										["PlayerId"] = PlayerId,
										["b_sName"] = b_sName
										}
	
	local team = RequireDbBox("GasTeamDB")
  local teamId = team.GetTeamID(PlayerId)	
  
	--查询阵型信息(根据玩家ID和阵法名称)
	local battleshaperesult = BattleArrayBooksSql.SelectBattleShape(queryData)
	--如果该阵型存在再查询与该阵型对应的列阵信息
	
	local ret
	if #battleshaperesult > 0 then
		b_uId = battleshaperesult[1]
		ret = CGasBattleArrayBooks._QueryArrayBattle:ExecSql("nnnnn",b_uId,teamId)
	end
	local ret_row = {}
	if ret:GetRowNum() ~= 0 then
		ret_row[1] = ret:GetData(0,0)
		ret_row[2] = ret:GetData(0,1)
		ret_row[3] = ret:GetData(0,2)
		ret_row[4] = ret:GetData(0,3)
		ret_row[5] = ret:GetData(0,4)
	end
	local team_box = RequireDbBox("GasTeamDB")
	local uCapatinID =  team_box.GetCaptain(teamId)
	return battleshaperesult,ret_row,uCapatinID
end
--------------------------------------------------------------------------
--删除列阵信息(根据阵法流水号和小队ID)
function BattleArrayBooksSql.DeleteArrayBattleInfo(data)
	local b_uId = tonumber(data["b_uId"])
	local teamId = tonumber(data["teamId"])
	if assert(IsNumber(b_uId)) and assert(IsNumber(teamId)) then
		CGasBattleArrayBooks._DeleteArrayBattle:ExecSql("",b_uId,teamId)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
	return false
end
--------------------------------------------------------------------------
--更改阵法的标识变量
function BattleArrayBooksSql.UpdateBattleShapeFlag(data)
	local PlayerId = tonumber(data["PlayerId"])
	local b_sName = tostring(data["b_sName"])
	if assert(IsNumber(PlayerId)) and assert(IsString(b_sName)) then
		CGasBattleArrayBooks._UpdateBattleShapeFlag:ExecSql("",0,PlayerId,b_sName)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
	return false
end
--------------------------------------------------------------------------
--更新阵法书的动态信息
function BattleArrayBooksSql.UpdateBattleBook(data)
	local ItemId = tonumber(data["ItemId"])
	local PlayerId = tonumber(data["PlayerId"])
	local b_sName = tostring(data["b_sName"])
	
	local querydata = {
								["PlayerId"] = PlayerId,
								["b_sName"] = b_sName
								}
	local tbl = BattleArrayBooksSql.SelectBattleShape(querydata)
	if #tbl > 0 and assert(IsNumber(ItemId)) then
		CGasBattleArrayBooks._UpdateBattleBook:ExecSql("",tbl[2],tbl[3],tbl[4],tbl[5],tbl[6],ItemId)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
	return false
end
--------------------------------------------------------------------------
--更新阵法信息
function BattleArrayBooksSql.UpdateBattleShape(data)
	local PlayerId = data["PlayerId"]
	local b_sName = data["b_sName"]
	local b_uPos1 = data["b_uPos1"]
	local b_uPos2 = data["b_uPos2"]
	local b_uPos3 = data["b_uPos3"]
	local b_uPos4 = data["b_uPos4"]
	local b_uPos5 = data["b_uPos5"]

	CGasBattleArrayBooks._UpdateBattleShape:ExecSql("",b_uPos1,b_uPos2,b_uPos3,b_uPos4,b_uPos5,PlayerId,b_sName)
	
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
--------------------------------------------------------------------------
local StmtDef = {
			"_QueryBattleBookByItemId",
			--玩家包裹里面是否还有阵法书(根据物品ID)
			[[ 
					select is_uId from tbl_item_in_grid as iip,tbl_grid_in_room as igp
					where iip.gir_uGridID = igp.gir_uGridID and cs_uId=? and gir_uRoomIndex=? and gir_uPos=?
			]]
}
DefineSql (StmtDef,CGasBattleArrayBooks)


--保存阵型信息
function BattleArrayBooksSql.SaveBattleShape(data)
	local PlayerId = data["PlayerId"]
	local b_sName = data["b_sName"]
	local b_uPos1 = data["b_uPos1"]
	local b_uPos2 = data["b_uPos2"]
	local b_uPos3 = data["b_uPos3"]
	local b_uPos4 = data["b_uPos4"]
	local b_uPos5 = data["b_uPos5"]
	local nRoomIndex = data["nRoomIndex"]
	local nPos = data["nPos"]
	local queryData = {
											["PlayerId"] = PlayerId,
											["b_sName"] = b_sName
										}
	local updateData = {
											["PlayerId"] = PlayerId,
											["b_sName"] = b_sName,
											["b_uPos1"] = b_uPos1,
											["b_uPos2"] = b_uPos2,
											["b_uPos3"] = b_uPos3,
											["b_uPos4"] = b_uPos4,
											["b_uPos5"] = b_uPos5
											}
	local bookInfo = CGasBattleArrayBooks._QueryBattleBookByItemId:ExecSql("n",PlayerId,nRoomIndex,nPos)
	--包裹里不存在改阵法书
	if bookInfo and bookInfo:GetRowNum() == 0 then
		return false
	end
	--阵法流水号
	local a = 0 
	local res = nil
	--查询阵型库里面有没有保存过该阵型
	local tbl = BattleArrayBooksSql.SelectBattleShape(queryData)
	--保存过就更新该阵型
	if #tbl > 0 and tbl[7] == 0 then
		BattleArrayBooksSql.UpdateBattleShape(updateData)
		a = tbl[1]
	elseif (#tbl == 0) then
		--没保存过就重新插入新记录
		CGasBattleArrayBooks._SaveBattleShape:ExecSql("",PlayerId,b_sName,b_uPos1,b_uPos2,b_uPos3,b_uPos4,b_uPos5,1)
		a = g_DbChannelMgr:LastInsertId()
	end
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if (nRoomIndex ~= -1 and nPos ~= -1) then 
		local ItemsType,ItemName,ItemCount = g_RoomMgr.GetTypeCountByPosition(PlayerId,nRoomIndex,nPos)
		if ( tonumber(ItemsType) == 26 and ItemName == b_sName) then
			res = g_RoomMgr.DelItemByPos(PlayerId,nRoomIndex,nPos,1,event_type_tbl["物品使用"])
			if IsNumber(res) then
				return false
			end
		end
	end
	return a,res
end
--------------------------------------------------------------------------
--查询阵型名称
function BattleArrayBooksSql.SelectBattleName(data)
	local b_uId = data["b_uId"]
	local ret = nil
	if assert(IsNumber(b_uId)) then
		local tbl = CGasBattleArrayBooks._QueryBattleName:ExecSql("s[32]",b_uId)
		if (tbl:GetRowNum() > 0)then
			ret = tbl:Data(0,0)
		end
	end
	return ret
end
--------------------------------------------------------------------------
--保存列阵信息
function BattleArrayBooksSql.SaveBattleArray(data)
	local b_uId = data["b_uId"]
	local b_sName = data["b_sName"]
	local PlayerId = data["PlayerId"]
	local ab_uLoc1 = BattleArrayBooksSql.CheckBattleArrayLoc(data["ab_uLoc1"])
	local ab_uLoc2 = BattleArrayBooksSql.CheckBattleArrayLoc(data["ab_uLoc2"])
	local ab_uLoc3 = BattleArrayBooksSql.CheckBattleArrayLoc(data["ab_uLoc3"])
	local ab_uLoc4 = BattleArrayBooksSql.CheckBattleArrayLoc(data["ab_uLoc4"])
	local ab_uLoc5 = BattleArrayBooksSql.CheckBattleArrayLoc(data["ab_uLoc5"])
	
	local team = RequireDbBox("GasTeamDB")
  local teamId = team.GetTeamID(PlayerId)			
  
	local PlayerIdTbl = {}
	local tbl = {ab_uLoc1, ab_uLoc2, ab_uLoc3, ab_uLoc4, ab_uLoc5}
	for i, p in pairs(tbl) do
		table.insert(PlayerIdTbl, p)
	end
	table.sort(PlayerIdTbl, function(a, b) return a < b end)
	--获得小队里所有玩家ID
	local team_box = RequireDbBox("GasTeamDB")
	local tblMembers = team_box.GetTeamMembers(teamId)
	table.sort(tblMembers, function(a, b) return a[1] < b[1] end)
	
	if(#PlayerIdTbl ~= #tblMembers) then 
		return false
	end

	for i=1, #tblMembers do
		if(PlayerIdTbl[i] ~= tblMembers[i][1])then 
			return false 
		end
	end

	local queryNameData = {
										["b_uId"] = b_uId
										}
	local queryInfoData = {
										["b_uId"] = b_uId,
										["teamId"] = teamId,
										["b_sName"] = b_sName,
										["PlayerId"] = PlayerId 
										}
	local updateInfoData = {
										["ab_uLoc1"] = ab_uLoc1,
										["ab_uLoc2"] = ab_uLoc2,
										["ab_uLoc3"] = ab_uLoc3,
										["ab_uLoc4"] = ab_uLoc4,
										["ab_uLoc5"] = ab_uLoc5,
										["b_uId"] = b_uId,
										["teamId"] = teamId
										}
	local ret = false
	--根据阵法流水号查询阵法名称
	local result = BattleArrayBooksSql.SelectBattleName(queryNameData)
	
	if( not result or result ~= b_sName) then
		return ret
	end
	
	--查询列阵信息
	local _,query_list = BattleArrayBooksSql.SelectArrayBattleInfo(queryInfoData)
	if #query_list > 0 then
		BattleArrayBooksSql.UpdateArrayBattle(updateInfoData)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	else
		CGasBattleArrayBooks._SaveArrayBattle:ExecSql("",b_uId,teamId,ab_uLoc1,ab_uLoc2,ab_uLoc3,ab_uLoc4,ab_uLoc5)
		return g_DbChannelMgr:LastAffectedRowNum() > 0	
	end
end
--------------------------------------------------------------------------
--更新列阵信息
function BattleArrayBooksSql.UpdateArrayBattle(data)
	local ab_uLoc1 = data["ab_uLoc1"]
	local ab_uLoc2 = data["ab_uLoc2"]
	local ab_uLoc3 = data["ab_uLoc3"]
	local ab_uLoc4 = data["ab_uLoc4"]
	local ab_uLoc5 = data["ab_uLoc5"]
	local b_uId = data["b_uId"]
	local teamId = data["teamId"]
	CGasBattleArrayBooks._UpdateArrayBattle:ExecSql("",ab_uLoc1,ab_uLoc2,ab_uLoc3,ab_uLoc4,ab_uLoc5,b_uId,teamId)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
--------------------------------------------------------------------------
--判断客户端传过来的参数是不是为0，为0即是空标记
function BattleArrayBooksSql.CheckBattleArrayLoc(ab_uLoc)
	if ab_uLoc == 0 then
		return nil
	end
		return ab_uLoc
end
--------------------------------------------------------------------------
----阵法还原成阵法书
function BattleArrayBooksSql.RevertToBattleArrayBook(data)
	local PlayerId = tonumber(data["PlayerId"])
	local b_sName = tostring(data["b_sName"])
	local b_uId = data["b_uId"]
	local arraybattledata = {
													["b_uId"] = b_uId,
													["teamId"] = teamId,
													["PlayerId"] = PlayerId,
													["b_sName"] = b_sName
													}
	local battleshapedata = {
													["PlayerId"] = PlayerId,
													["b_sName"] = b_sName
													}			
	local team = RequireDbBox("GasTeamDB")
  local teamId = team.GetTeamID(PlayerId)																		
	--查询列阵信息
	local _,arraybattleInfo = BattleArrayBooksSql.SelectArrayBattleInfo(arraybattledata)
	
	--有列阵信息就删除，注意：因为再列阵信息表里有个阵型信息表的字段，且是外键关联，所以一定得先删列阵信息，后删阵型信息。
	if #arraybattleInfo > 0 then
		BattleArrayBooksSql.DeleteArrayBattleInfo(arraybattledata)	
	end
	
	--查询玩家是否拥有该阵法
	local queryData = {
										["PlayerId"] = PlayerId,
										["b_sName"] = b_sName
										}
	--查询阵型信息(根据玩家ID和阵法名称)
	local battleshaperesult = BattleArrayBooksSql.SelectBattleShape(queryData)
	if nil == battleshaperesult then
		return 
	end
	
	if #battleshaperesult == 0 or battleshaperesult[7] == 0 then
		return
	end

	--更改阵法的标识变量
	BattleArrayBooksSql.UpdateBattleShapeFlag(battleshapedata)
	--向包裹里添加要转换的阵法书
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local nBigID = g_ItemInfoMgr:GetBattleArrayBooksBigID()
	local params= {}
	params.m_nType = nBigID
	params.m_sName =b_sName
	params.m_nBindingType = g_ItemInfoMgr:GetItemInfo( nBigID,b_sName,"BindingStyle" ) or 0
	params.m_nCharID =PlayerId
	params.m_sCreateType = event_type_tbl["阵法还原成阵法书"]
	local item_id = g_RoomMgr.CreateItem(params)

	local updatebattlebook = {
													["PlayerId"] = PlayerId,
													["b_sName"] = b_sName,
													["ItemId"] = item_id
													}
	local param = {}
	param.m_nCharID = PlayerId
	param.m_nItemType = 26
	param.m_sItemName = b_sName
	param.m_tblItemID = {{item_id}}
	local putitemtobag = RequireDbBox("CPutItemToBagDB")
	local succ,info = putitemtobag.AddCountItem(param)
	--更新阵法书的动态信息
	BattleArrayBooksSql.UpdateBattleBook(updatebattlebook)
  return succ,info 
end
--------------------------------------------------------------------------------

SetDbLocalFuncType(BattleArrayBooksSql.SaveBattleShape)
SetDbLocalFuncType(BattleArrayBooksSql.SaveBattleArray)
SetDbLocalFuncType(BattleArrayBooksSql.SelectArrayBattleInfo)
SetDbLocalFuncType(BattleArrayBooksSql.RevertToBattleArrayBook)
SetDbLocalFuncType(BattleArrayBooksSql.SaveSkillBattle)
return BattleArrayBooksSql
