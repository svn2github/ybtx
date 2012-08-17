gac_gas_require "relation/tong/TongMgr"
cfg_load "npc/BuildingData"
gac_gas_require "relation/tong/TongProdItemMgr"
local BuildingItemMgr  = CTongProdItemInfoMgr:new()
local BuildingData = BuildingData
local LogErr = LogErr
local os = os
local g_TongMgr = CTongMgr:new()
local StmtOperater = {}	
local g_ItemInfoMgr = CItemInfoMgr:new()
local event_type_tbl = event_type_tbl
------------------------------------
local CTongBuildingBox = CreateDbBox(...)
----------------------------------------sql相关---------------------------------------------------------------------

--【获得建筑状态】
local StmtDef = {
    	"_GetBuildStateByID",   
    	[[ 
    		select B.bt_uState,B.bt_sBuildName from tbl_building_tong as B,tbl_member_tong as M 
    		where B.t_uId = M.t_uId and M.cs_uId = ? and B.bt_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【修改某一个建筑状态】
local StmtDef = {
    	"_UpdateBuildState",	
    	[[  update  tbl_building_tong  set bt_uState = ? where bt_uId = ?  ]]
}    
DefineSql ( StmtDef, StmtOperater )

--【修改某一个建筑等级】
local StmtDef = {
			"_UpdateBuildLevel",
 			[[ update tbl_building_tong set bt_uLevel = ? where bt_uId = ? ]]
}
DefineSql( StmtDef, StmtOperater)

--【根据建筑id获得建筑名称】
local StmtDef = {
    	"_SelectBuildingName",
    	[[ select bt_sBuildName,bt_uLevel from tbl_building_tong  where bt_uId = ? and t_uId = ?]]
}    
DefineSql ( StmtDef, StmtOperater )

--【消耗资源】
local StmtDef = {
    	"_UpdateTongResource",
    	[[ 
    		update  tbl_tong  
    		set    t_uResource = t_uResource+ ?
    		where  t_uResource+ ? >= 0 and t_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【获得某帮会当前拥有的建筑数量】
local StmtDef = {
    	"_CountBuildingByID",
    	[[ select count(*) from tbl_building_tong where t_uId = ? ]]
}    
DefineSql ( StmtDef, StmtOperater )

--【查询建筑信息】
local StmtDef = {
    	"_SelectBuildInfo",
    	[[ 
    		 select build.bt_uId, build.bt_sBuildName, build.bt_uPosX, build.bt_uPosY, 
    		 				life.bl_uLife, build.bt_uState, unix_timestamp(life.bl_dtLastTime),
    		 				 build.t_uId, build.bt_uLevel
    		 from tbl_building_tong as build,tbl_building_life as life
    		 
    		 where build.bt_uId = life.bt_uId
    		 	and 	build.t_uId = ? 
    		 	and build.bt_uState not in(?,?,?)
    	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"GetBuildingInfoById",
    	[[ 
    		 select build.bt_uId, build.bt_sBuildName, build.bt_uPosX, build.bt_uPosY, 
    		 				life.bl_uLife, build.bt_uState, unix_timestamp(life.bl_dtLastTime),
    		 				 build.t_uId, build.bt_uLevel
    		 from tbl_building_tong as build,tbl_building_life as life
    		 
    		 where build.bt_uId = life.bt_uId
    		 	and build.bt_uId = ?
    		 	and build.bt_uState not in(?,?,?)
    	]]
}
DefineSql ( StmtDef, StmtOperater )

--【删除建筑】
local StmtDef = {
    	"_DeleteBuilding",
    	[[ delete from tbl_building_tong  where  bt_uId = ? ]]
}    
DefineSql ( StmtDef, StmtOperater )

--【删除帮会的所有建筑】
local StmtDef = {
    	"_ClearTongBuilding",
    	[[ delete from tbl_building_tong  where  t_uId = ? ]]
}    
DefineSql ( StmtDef, StmtOperater )


--【查询建筑信息】
local StmtDef = {
    	"_SelectBuildingInfo",
    	[[ 
    		select  build.bt_sBuildName, life.bl_uLife, build.bt_uState 
    	  	from    tbl_building_tong  as build,tbl_building_life as life
    		where   build.bt_uId =life.bt_uId 
    		and build.bt_uId = ? and build.t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【得到帮会中等级最大的值】
local StmtDef = 
{
		"_SelectMaxLevelPlayer",
		[[
			select max( cb_uLevel)
			from tbl_char_basic, tbl_member_tong
			where tbl_char_basic.cs_uId = tbl_member_tong.cs_uId and tbl_member_tong.t_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

--【添加正在建造的建筑模型到模型表】
local StmtDef = 
{
		"_AddBuildingItem",
		[[
			insert into tbl_building_item(bt_uId,bi_uDepotIndex,bi_dtStartTime) values(?,?,now())
		]]
}
DefineSql ( StmtDef, StmtOperater )

--【获得正在建造的建筑模型信息】
local StmtDef = 
{
		"_GetCreatingBuildItem",
		[[
			select TB.bt_sBuildName,unix_timestamp(now())-unix_timestamp(BI.bi_dtStartTime),TB.bt_uId,BI.bi_dtStartTime
			from tbl_building_tong as TB,tbl_building_item as BI
			where TB.bt_uId = BI.bt_uId
			and 
			 TB.bt_uState = ? and TB.t_uId = ? limit ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_DetCreatingBuildItem",
		[[
			delete from tbl_building_item where bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_UpdateBuildingState",
		[[
			update tbl_building_tong set bt_uState  = ? 
			where t_uId = ? and bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_UpdateBuildItemTime",
		[[
			update tbl_building_item set bi_dtStartTime = date_add(?,interval ? second)
			where bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )
local StmtDef = 
{
		"_UpdateBuildItemStartTime",
		[[
			update tbl_building_item set bi_dtStartTime = now()
			where bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_GetOneWaitItem",
		[[
			select build.bt_uId from tbl_building_tong as build,tbl_building_item as item
			where build.bt_uId = item.bt_uId
			and build.t_uId = ? and build.bt_uState  = ? 
			order by item.bi_dtStartTime asc 
			limit 1
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_GetListBuildItem",
		[[
			select bt_sBuildName,bt_uId from tbl_building_tong
			where  bt_uState = ? and t_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_CountBuildByName",
		[[
			select count(*) from tbl_building_tong
			where  bt_sBuildName = ? and t_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_GetOneTongBuildID",
		[[
			select bt_uId from tbl_building_tong
			where  t_uId = ? and bt_sBuildName = ? and bt_uState = ? limit 1
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_UpdateBuildInfoByID",
		[[
			update tbl_building_tong set bt_uPosX =?,bt_uPosY = ?,bt_uState = ?,bt_uLevel = ?
			where bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_GetBuildingLifeInfo",
		[[
			select bl_uLife, unix_timestamp(bl_dtLastTime)
			from tbl_building_life
			where  bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater)

local StmtDef = 
{
		"_GetBuildingLife",
		[[
			select bl_uLife
			from tbl_building_life
			where  bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater)

local StmtDef = 
{
		"SetBuildingLifeInfo",
		[[
			update tbl_building_life
			set bl_uLife = ?, bl_dtLastTime = from_unixtime(?)
			where  bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater)

local StmtDef = 
{
		"SetBuildingLife",
		[[
			update tbl_building_life
			set bl_uLife = ?
			where  bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater)

local StmtDef = {
    	"_DelTongNeedFireInfoById",
    	[[ 
    		delete from tbl_tong_needfire where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

------------------------------------------------数据库操作相关----------------------------------------------------------------
function CTongBuildingBox.GetOneTongBuildID(uTongID,sName,nState) 
	local res = StmtOperater._GetOneTongBuildID:ExecSql('n',uTongID,sName,nState)
	if res:GetRowNum() == 0 then
		return 0
	else
		return res:GetNumber(0,0)
	end
end

function CTongBuildingBox.CountBuildByName(uTongID,sName)
	local res = StmtOperater._CountBuildByName:ExecSql('n',sName,uTongID)
	local nCount = res:GetNumber(0,0)
	return nCount
end

--【获得某帮会当前拥有的建筑数量】
function CTongBuildingBox.CountBuildingByID(uTongID)
	local tblCount = StmtOperater._CountBuildingByID:ExecSql('n',uTongID)
	if  tblCount:GetRowNum() == 0 then
		return 0
	end
	return tblCount:GetData(0,0)
end

--【修改资源】
function CTongBuildingBox.UpdateTongResource(res,tong_id,event_type)
   StmtOperater._UpdateTongResource:ExecSql('',res,res,tong_id)
   if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
  	 CancelTran()
  	 return
   end
  local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveTongResourceInfo(tong_id,res,event_type)
  return true
end

--【得到建筑信息】
function CTongBuildingBox.GetBuildInfo(tong_id)
  local query_result = StmtOperater._SelectBuildInfo:ExecStat(tong_id,g_TongMgr:GetBuildingState("模型"),g_TongMgr:GetBuildingState("等待建造"),g_TongMgr:GetBuildingState("建造"))
   
	return query_result
end

function CTongBuildingBox.GetBuildingInfoById(buildingId)
	local result = StmtOperater.GetBuildingInfoById:ExecStat(buildingId, g_TongMgr:GetBuildingState("模型"),g_TongMgr:GetBuildingState("等待建造"),g_TongMgr:GetBuildingState("建造"))
	local buildingInfo
	if result:GetRowNum() ~=0  then
		buildingInfo = result:GetRow(0)
	end
	result:Release()
	return buildingInfo
end

local BaseCampTbl = {}
BaseCampTbl[1] = "艾米大本营"
BaseCampTbl[2] = "神圣大本营"
BaseCampTbl[3] = "修斯大本营"

function CTongBuildingBox.CanRemoveABuild(building_id,tong_id)
	--判断被拆除的建筑是否是大本营，大本营不能拆除
	--用帮会id做条件，是为了防止外挂
   local building_name = StmtOperater._SelectBuildingInfo:ExecSql('s[32]nn',building_id,tong_id)
   if building_name:GetRowNum() == 0 then
   		return 
   end
   local tong_box = RequireDbBox("GasTongBasicDB")
   local nCamp = tong_box.GetTongCampByID(tong_id)
   if building_name:GetData(0,0) == BaseCampTbl[nCamp] then
   		--"大本营不能被拆除"
   		return false,9134
   end
   return  true
end
local StmtDef = 
{
		"_GetCdepotByBuildID",
		[[
			select bi_uDepotIndex from tbl_building_item where bt_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )
function CTongBuildingBox.GetCdepotByBuildID(build_id)
	  local res = StmtOperater._GetCdepotByBuildID:ExecSql('n',build_id)
	  if res:GetRowNum() == 0 then
	  	return 0
	  end
	  return res:GetNumber(0,0)
end

local StmtDef = 
{
		"_GetAllCreatingBuild",
		[[
			select bt.bt_uId,bt.bt_sBuildName from tbl_building_tong as bt,tbl_building_item as bi 
			where  bt.bt_uId = bi.bt_uId and bt.t_uId = ? 
			and bt.bt_uState in(?,?) and bi.bi_uDepotIndex = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_GetAllBuildName",
		[[
			select bt_sBuildName from tbl_building_tong where t_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

--【用来处理建造到期建筑模型的接口】
function CTongBuildingBox.DealWithBuildingItem(tong_id,charId,sceneName)
	local bFlag = true
	local RetRes,nRetMoney = 0,0
	local tblRetRes = {}
	local g_LogMgr = RequireDbBox("LogMgrDB")

	local function deal_with_info(build_id,build_name,pstime,start_time)  
   	if not build_id then
   		--把一个等待队列中的物品改为建造状态
   		local tblID = StmtOperater._GetOneWaitItem:ExecSql('n',tong_id,g_TongMgr:GetBuildingState("等待建造"))
			if tblID:GetRowNum() > 0 then
   			StmtOperater._UpdateBuildState:ExecSql('',g_TongMgr:GetBuildingState("建造"),tblID:GetData(0,0))
			end
			bFlag = false
			return
		end
		local nTongItemBigID = g_ItemInfoMgr:GetTongItemBigID()
		local NeedTime = g_ItemInfoMgr:GetItemInfo(nTongItemBigID, build_name,"NeedTime")
   	if (pstime >= NeedTime) then
   	  --时间到
   	  --产出新的帮会建筑物品
   	 	local depot_index = CTongBuildingBox.GetCdepotByBuildID(build_id)
			if depot_index > 0 then
   	  		local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
   				local cdepotMgr = RequireDbBox("GasCDepotMgrDB")
   				local tong_box = RequireDbBox("GasTongBasicDB")
					
					local params= {}
					params.m_nType = nTongItemBigID
					params.m_sName = build_name
					params.m_nCharID = charId
					params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(nTongItemBigID, build_name,"BindingStyle")
					params.m_sCreateType = event_type_tbl["帮会建筑物品产出"]
					local item_id = g_RoomMgr.CreateItem(params)
					if not item_id then return end
   				local nRes = cdepotMgr.AutoAddItems(tong_box.GetTongDepotID(tong_id),nTongItemBigID, build_name,{item_id},depot_index)
   				if IsNumber(nRes) then
   					--仓库满了，删除队列中的所有排在该仓库物品，返还资源
   					local tblCreatingBuild = StmtOperater._GetAllCreatingBuild:ExecSql('ns[32]',tong_id, g_TongMgr:GetBuildingState("建造"),g_TongMgr:GetBuildingState("等待建造"),depot_index)
   					local row = tblCreatingBuild:GetRowNum()
   					for i =1,row do
   						local nBuildID,sBuildName = tblCreatingBuild:GetData(i-1,0),tblCreatingBuild:GetData(i-1,1)
   						local nAddRes = g_ItemInfoMgr:GetItemInfo(nTongItemBigID,sBuildName,"NeedRes")
   						local nAddMoney = g_ItemInfoMgr:GetItemInfo(nTongItemBigID,sBuildName,"NeedTongMoney")
							RetRes 		= RetRes + nAddRes
							nRetMoney = nRetMoney + nAddMoney
							StmtOperater._DeleteBuilding:ExecSql('',nBuildID)
							g_LogMgr.TongItemProduceEventLog(tong_id,sBuildName,2,0)
						end
						tblRetRes = CTongBuildingBox.GivebackResAndMoney(tong_id,RetRes,nRetMoney)
   				else
   					--删除建造完的物品信息
   					StmtOperater._DetCreatingBuildItem:ExecSql('',build_id)
   					--修改建造完的建筑状态
   					StmtOperater._UpdateBuildingState:ExecSql('',g_TongMgr:GetBuildingState("模型"),tong_id,build_id)
					end
					--把一个等待队列中的物品改为建造状态
					local tblID = StmtOperater._GetOneWaitItem:ExecSql('n',tong_id,g_TongMgr:GetBuildingState("等待建造"))
					if tblID:GetRowNum() > 0 then
   					StmtOperater._UpdateBuildItemTime:ExecSql('',start_time,NeedTime,tblID:GetData(0,0))
   					StmtOperater._UpdateBuildState:ExecSql('',g_TongMgr:GetBuildingState("建造"),tblID:GetData(0,0))
						g_LogMgr.TongItemProduceEventLog(tong_id,build_name,3,0)
					end 
				else
					bFlag = false
				end
			else
				bFlag = false
			end
	end
	while(bFlag) do
		local tblInfo = StmtOperater._GetCreatingBuildItem:ExecSql('s[32]nns[32]',g_TongMgr:GetBuildingState("建造"),tong_id,g_TongMgr:GetBuildWaitNum())
		if tblInfo:GetRowNum() > 0 then
			local build_id,build_name,pstime,start_time = tblInfo:GetNumber(0,2),tblInfo:GetString(0,0),tblInfo:GetNumber(0,1),tblInfo:GetString(0,3)
			deal_with_info(build_id,build_name,pstime,start_time)
		else
			deal_with_info()
		end
	end
	return tblRetRes
end
-----------------------------------------RPC调用相关---------------------------------------------------
--【得到建筑信息】
function CTongBuildingBox.GetTongBuildingInfoRPC(parameters)
	local uPlayerID = parameters.uPlayerID
	
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	
	return CTongBuildingBox.GetBuildInfo(uTongID)
	
end


-- 【获得正在制造和等待建造的建筑模型信息】
function CTongBuildingBox.GetCreatingBuildingItemInfoDB(parameters)
	local uPlayerID 	= parameters.uPlayerID
	local sceneName		= parameters.sceneName
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	--先处理一下时间到的建筑
	local tblRetRes = CTongBuildingBox.DealWithBuildingItem(uTongID,uPlayerID,sceneName)
	local tblCreatingInfo = StmtOperater._GetCreatingBuildItem:ExecSql('s[32]nns[32]',g_TongMgr:GetBuildingState("建造"),uTongID,g_TongMgr:GetBuildWaitNum())
	local tbl = {}
	if tblCreatingInfo:GetRowNum() == 0 then
		table.insert(tbl,"")
		table.insert(tbl,0)
		table.insert(tbl,0)
	else
		table.insert(tbl,tblCreatingInfo:GetData(0,0))
		table.insert(tbl,tblCreatingInfo:GetData(0,2))
		table.insert(tbl,tblCreatingInfo:GetData(0,1))
	end
	
	local tblWaitingInfo = StmtOperater._GetListBuildItem:ExecSql('s[32]n',g_TongMgr:GetBuildingState("等待建造"),uTongID)
	local row = tblWaitingInfo:GetRowNum()
	if row == 0 then
		for i =1,6 do
			table.insert(tbl,"")
			table.insert(tbl,0)
		end
	else
		for i =1,row do
			table.insert(tbl,tblWaitingInfo:GetData(i-1,0))
			table.insert(tbl,tblWaitingInfo:GetData(i-1,1))
		end
		if row < 6 then
			for j = 1,(6 - row) do
				table.insert(tbl,"")
				table.insert(tbl,0)
			end
		end
	end
	return tbl,tblRetRes
end

-- 【制造建筑模型】
function CTongBuildingBox.CreateBuildingItemDB(parameters)
	local uPlayerID 	= parameters.uPlayerID
	local sBuildingName = parameters.sBuildingName
	local nDepot		 	= parameters.nDepot
	local sceneName			= parameters.sceneName
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	if not tong_box.JudgePurview(uPlayerID, "CreateBuilding") then
  	return 9006
  end
  local tblWaitingInfo = StmtOperater._GetListBuildItem:ExecSql('s[32]n',g_TongMgr:GetBuildingState("等待建造"),uTongID)
	if tblWaitingInfo:GetRowNum() >= g_TongMgr:GetBuildWaitNum() then
		--等待队列已满
		return 9180
	end
	
	--判断仓库使用权限
  if not tong_box.JudgePurview(uPlayerID,g_TongMgr:GetDepotStrByPage(nDepot)) then
  	return 9305
  end
  local cdepotMgr = RequireDbBox("GasCDepotMgrDB")
  if not cdepotMgr.HaveEnoughRoomPos(tong_box.GetTongDepotID(uTongID),g_ItemInfoMgr:GetTongItemBigID(),sBuildingName,nDepot,1) then
  	return 9309
  end
	--判断该帮会建筑物品在对应的配置表中是否存在
	local ItemType = g_ItemInfoMgr:GetTongItemBigID()
	if (g_ItemInfoMgr:GetItemInfo(ItemType, sBuildingName,"Type") ~= 1) then 
		return  9167
	end
	
	local tong_level = tong_box.GetTongLevel(uTongID)
	local need_money = g_ItemInfoMgr:GetItemInfo(ItemType, sBuildingName,"NeedTongMoney")
	local need_tong_level = g_ItemInfoMgr:GetItemInfo(ItemType, sBuildingName,"NeedTongLevel")
	local number_limit = g_ItemInfoMgr:GetItemInfo(ItemType, sBuildingName,"NumLimit")
	if string.find(sBuildingName,"小型防御设施") then
		number_limit = number_limit + tong_box.GetTongLevel(uTongID) -1
	end
	local need_res = g_ItemInfoMgr:GetItemInfo(ItemType, sBuildingName,"NeedRes")
	if need_tong_level > tong_level then
		return 9173,need_tong_level
	end
	if number_limit and number_limit > 0 then
		--数量已经达到上限
		local build_num = CTongBuildingBox.CountBuildByName(uTongID,sBuildingName)
		if build_num >= number_limit then
			return 9192
		end
	end
	local tong_camp = tong_box.GetTongCampByID(uTongID)
	local ncamp = BuildingItemMgr:GetCampByBuildName(sBuildingName)
	if ncamp ~=0 and ncamp ~=100 and ncamp ~= tong_camp then
		return 9031
	end
	--消耗资源
	if need_res and need_res > 0 then
		if not CTongBuildingBox.UpdateTongResource(-need_res,uTongID,event_type_tbl["佣兵小队建筑制造"]) then
			return 9010
		end
	end
	--消耗金钱
	if need_money and need_money > 0 then
		if not tong_box.UpdateTongMoney({["uTongID"]=uTongID,["uMoney"]= -need_money,["nEventType"] = event_type_tbl["佣兵小队建筑制造"]}) then
			CancelTran()
			return 9030
		end
	end
	local sState = "建造"
	local tblCreatingInfo = StmtOperater._GetCreatingBuildItem:ExecSql('s[32]nns[32]',g_TongMgr:GetBuildingState("建造"),uTongID,g_TongMgr:GetBuildWaitNum())
	if tblCreatingInfo:GetRowNum() > 0 then
		--如果有正在建造的，就加入等待队列
		sState = "等待建造"
	end
	local building_id = tong_box.AddTongBuild(uTongID,sBuildingName, 0,0,g_TongMgr:GetBuildingState(sState), 0) 
	StmtOperater._AddBuildingItem:ExecSql('',building_id,nDepot)
	
	if building_id > 0 then
  	local ex = RequireDbBox("Exchanger")
		local player_name = ex.getPlayerNameById(uPlayerID)
		local sMsg = "2012_" .. player_name .. ",item:" .. ItemType .. "|" .. sBuildingName
  	if not tong_box.AddTongLogs(uTongID,sMsg, g_TongMgr:GetLogType("建设")) then
  		return 9017
  	end
  	local g_LogMgr = RequireDbBox("LogMgrDB")
		g_LogMgr.TongItemProduceEventLog(uTongID,sBuildingName,1,uPlayerID)
	end
	return true,CTongBuildingBox.GetCreatingBuildingItemInfoDB({["uPlayerID"] = uPlayerID,["sceneName"] = sceneName})
end

function CTongBuildingBox.DealWithItem(parameters)
	local sName = parameters.sName
	local nIndex = parameters.nIndex
	local nPos = parameters.nPos
	local x = parameters.x
	local y = parameters.y
	local uPlayerID = parameters.uPlayerID
	local sceneName = parameters.sceneName
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	if uTongID == 0 then
		return false, 9401
	end
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local ItemType, ItemName, ItemCount = g_RoomMgr.GetTypeCountByPosition(uPlayerID, nIndex, nPos)
	if ItemName ~= sName or ItemCount <= 0 then
		return false
	end
	local res = g_RoomMgr.DelItemByPos(uPlayerID, nIndex, nPos, 1,event_type_tbl["物品使用"])
	
	local buildingInfo = {}
	buildingInfo[1] = sName
	buildingInfo[2] = x
	buildingInfo[3] = y
	buildingInfo[4] = uTongID
	buildingInfo[5] = CTongBuildingBox.GetTongMaxLevel(uTongID)
	buildingInfo[6] = uPlayerID
	
	return true, res, tong_box.GetTongCampByID(uTongID), buildingInfo
	
end

--【建造建筑】
function CTongBuildingBox.CreateBuilding(parameters)
	local sName = parameters.sName
	local nIndex = parameters.nIndex
	local nPos = parameters.nPos
	local x = parameters.x
	local y = parameters.y
	local uPlayerID = parameters.uPlayerID
	local sceneName = parameters.sceneName
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	if uTongID == 0 then
		return false, 9401
	end
	if uTongID ~= parameters["uTongID"] then
		return false, 9405
	end
	
	local WarZoneDB = RequireDbBox("WarZoneDB")	
	local sceneType, sceneId, serverId, warZoneId, stationId = WarZoneDB.GetTongStationInfo({["tongId"] = uTongID})
	if sceneId ~= parameters["uSceneID"] or
		(sceneType == 7 and stationId ~= parameters["stationId"]) then
		return false, 9407
	end
	
	local Radius = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), sName,"Radius")
	
	local buildingInfo = CTongBuildingBox.GetBuildInfo(uTongID)
	local row = buildingInfo:GetRowNum()
	for i =1,row do
		local dx = buildingInfo(i,3) - x
		local dy = buildingInfo(i,4) - y
		local dRadius = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), buildingInfo(i,2),"Radius")
		local disSq = math.sqrt(dx * dx + dy * dy)
		if disSq < (Radius + dRadius) then
			return false, 9408
		end
	end
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local ItemType, ItemName, ItemCount = g_RoomMgr.GetTypeCountByPosition(uPlayerID, nIndex, nPos)
	if ItemName ~= sName or ItemCount <= 0 then
		--物品不正确
		return false, 9336
	end
	local building_id = CTongBuildingBox.GetOneTongBuildID(uTongID,sName,g_TongMgr:GetBuildingState("模型"))
	local uLevel =  CTongBuildingBox.GetTongMaxLevel(uTongID)
	StmtOperater._UpdateBuildInfoByID:ExecSql('',x,y,g_TongMgr:GetBuildingState("正常"),uLevel,building_id)

	local res = g_RoomMgr.DelItemByPos(uPlayerID, nIndex, nPos, 1,event_type_tbl["物品使用"])
	
	local thisBuilding = CTongBuildingBox.GetBuildingInfoById(building_id)
	if not thisBuilding then
		return false, 9343
	end
	
	local camp = tong_box.GetTongCampByID(uTongID)
	
	local argTbl = {}
	argTbl.TongId = uTongID
	argTbl.SceneType = sceneType
	argTbl.SceneId = sceneId
	argTbl.Camp = camp
	if sceneType == 7 then
		argTbl.WarZoneId = warZoneId
		argTbl.StationId = stationId
	end
	argTbl.BuildingInfo = thisBuilding
	
	if serverId ~= 0 then
		Db2GasCall("CreateTongBuilding", argTbl, serverId)
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongBuildingEventLog(uTongID,ItemName,1,1,uPlayerID,uTongID)
	
	return true, res, tong_box.GetTongMemberInitiators(uTongID)
end



--【拆除建筑】
function CTongBuildingBox.RemoveBuilding(parameters)
	local build_id = parameters.build_id
	local uPlayerID = parameters.uPlayerID
	local hpRate = parameters.hpRate
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID) 
	if not tong_box.JudgePurview(uPlayerID, "BackoutBuilding") then
		return false, 9006
	end
	
	--判断是否可以拆除
	local bFlag,uMsgID = CTongBuildingBox.CanRemoveABuild(build_id,uTongID)
	if not bFlag then
		return bFlag,uMsgID
	end
	
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tblBuildName = StmtOperater._SelectBuildingName:ExecStat(build_id,uTongID)
	if tblBuildName:GetRowNum() == 0 then return end
	local sBuildName = tblBuildName:GetData(0,0)
	
	--返还资源
	local nRes, nRetMoney, nPorp = 0, 0, 0.75
	local nTongItemBigID = g_ItemInfoMgr:GetTongItemBigID()
	local nAddRes = g_ItemInfoMgr:GetItemInfo(nTongItemBigID, sBuildName, "NeedRes")
	local nAddMoney = g_ItemInfoMgr:GetItemInfo(nTongItemBigID, sBuildName, "NeedTongMoney")
	nRetMoney	= nAddMoney * nPorp * hpRate
	nRes		= nAddRes * nPorp * hpRate
	CTongBuildingBox.GivebackResAndMoney(uTongID, nRes, nRetMoney)
	
	StmtOperater._DeleteBuilding:ExecSql('',build_id)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
		CancelTran()
		return
	end
	
	--添加日志信息
	local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
	local sMsg = "2011_" .. player_name .. ",item:" .. g_ItemInfoMgr:GetTongItemBigID() .. "|" .. sBuildName
	if not tong_box.AddTongLogs(uTongID,sMsg, g_TongMgr:GetLogType("建设")) then
		CancelTran()
		return
	end
	if bFlag then
		local WarZoneDB = RequireDbBox("WarZoneDB")
		local sceneType, sceneId, serverId, warZoneId, stationId = WarZoneDB.GetTongStationInfo({["tongId"] = uTongID})
		local argTbl = {}
		argTbl.TongId = uTongID
		argTbl.BuildingId = build_id
		Db2GasCall("RemoveBuilding", argTbl, serverId)
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongBuildingEventLog(uTongID,sBuildName,tblBuildName(1,2),2,uPlayerID,uTongID)
	if string.find(sBuildName,"篝火") then
		StmtOperater._DelTongNeedFireInfoById:ExecStat(uTongID)
	end
	return bFlag, uMsgID,uTongID,sBuildName
end

function CTongBuildingBox.ClearTongBuilding(tongId)
	StmtOperater._ClearTongBuilding:ExecStat(tongId)
	return (g_DbChannelMgr:LastAffectedRowNum() > 0)
end
--【建筑被打爆】
function CTongBuildingBox.DestroyBuilding(parameters)
	local uTongID = parameters.uTongID
	local building_id = parameters.build_id

	local tong_box = RequireDbBox("GasTongBasicDB")
 	local build_name = StmtOperater._SelectBuildingName:ExecStat(building_id,uTongID)
 	
  if 	build_name:GetRowNum() == 0 then return end
 	local sBuildName = build_name:GetData(0,0)
	StmtOperater._DeleteBuilding:ExecSql('',building_id)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
  	CancelTran()
 		return
	end
	--添加日志信息
	local ex = RequireDbBox("Exchanger")
	local sMsg = "2013_item:" .. g_ItemInfoMgr:GetTongItemBigID() .. "|" .. sBuildName
 	if not tong_box.AddTongLogs(uTongID,sMsg, g_TongMgr:GetLogType("建设")) then
		CancelTran()
		return
	end
	
	local RetTongID = uTongID
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local sceneType, sceneId, serverId, warZoneId, stationId = WarZoneDB.GetTongStationInfo({["tongId"] = uTongID})
	if serverId == 0 then
		return true,RetTongID
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongBuildingEventLog(uTongID,sBuildName,build_name(1,2),2,0,0)
	
	local argTbl = {}
	argTbl.TongId = uTongID
	argTbl.BuildingId = building_id
	Db2GasCall("DestroyBuilding", argTbl, serverId)
	return true,RetTongID
end

function CTongBuildingBox.GivebackResAndMoney(tong_id,nRetRes,nRetMoney)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local upper_res = tong_box.GetUpperLimitResource(tong_id)
	local now_res = tong_box.GetTongResource(tong_id)
	local nDropRes = 0
	if now_res + nRetRes > upper_res then
		if now_res > upper_res then
			nDropRes = nRetRes
		else
			nDropRes = now_res + nRetRes - upper_res
		end
		nRetRes = upper_res - now_res
	end
	if nRetRes > 0 then
		CTongBuildingBox.UpdateTongResource(nRetRes,tong_id,event_type_tbl["佣兵小队取消物品制造"])
	end
	if nRetMoney > 0 then
		tong_box.UpdateTongMoney({["uTongID"]= tong_id,["uMoney"]= nRetMoney,["nEventType"] = event_type_tbl["佣兵小队取消物品制造"]})
	end
	local tbl = {}
	tbl.m_nRetRes 	= nRetRes
	tbl.m_nRetMoney = nRetMoney
	tbl.m_nDropRes 	= nDropRes
	
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local sceneType, sceneId, serverId, warZoneId, stationId = WarZoneDB.GetTongStationInfo({["tongId"] = tong_id})
	local TongStation = {}
	TongStation.sceneId = sceneId
	TongStation.sceneType = sceneType
	TongStation.stationId = stationId
	tbl.m_tblStationInfo = TongStation
	return tbl
end

--【取消建筑建造】
function CTongBuildingBox.CancelMakeBuildingDB(params)
	local nBuildID = params.nBuildID
	local uPlayerID = params.uPlayerID
	local sceneName = params.sceneName
	local tong_box = RequireDbBox("GasTongBasicDB")
	if not tong_box.JudgePurview(uPlayerID, "CancelCreateBuilding") then
  	return 9006
  end
	local nState = StmtOperater._GetBuildStateByID:ExecSql('ns[96]',uPlayerID,nBuildID)
	if nState:GetRowNum() == 0 then return end
	local state,build_name = nState:GetData(0,0),nState:GetData(0,1)
	if (state ~= g_TongMgr:GetBuildingState("建造")) 
		and (g_TongMgr:GetBuildingState("等待建造") ~= state) then
		return
	end
	--返还资源
	local tong_id = tong_box.GetTongID(uPlayerID) 
  local nTongItemBigID = g_ItemInfoMgr:GetTongItemBigID()
  local nRes,nRetMoney,nPorp = 0,0,1
  local nAddRes = g_ItemInfoMgr:GetItemInfo(nTongItemBigID, build_name,"NeedRes")
	local nAddMoney = g_ItemInfoMgr:GetItemInfo(nTongItemBigID, build_name,"NeedTongMoney")
	nRetMoney = nAddMoney * nPorp
	nRes 			= nAddRes * nPorp
	local tblRetRes = CTongBuildingBox.GivebackResAndMoney(tong_id,nRes,nRetMoney)
	
	StmtOperater._DeleteBuilding:ExecSql('',nBuildID)
	if state == g_TongMgr:GetBuildingState("建造") then
		--把一个等待队列中的物品改为建造状态
   	local tblID = StmtOperater._GetOneWaitItem:ExecSql('n',tong_id,g_TongMgr:GetBuildingState("等待建造"))
		if tblID:GetRowNum() > 0 then
   		StmtOperater._UpdateBuildItemStartTime:ExecSql('',tblID:GetData(0,0))
   		StmtOperater._UpdateBuildState:ExecSql('',g_TongMgr:GetBuildingState("建造"),tblID:GetData(0,0))
		end
	end
	--添加log
	local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
	local sMsg = "2017_" .. player_name .. ",item:" .. nTongItemBigID .. "|" .. build_name
  if not tong_box.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("建设")) then
  	return
  end
  local g_LogMgr = RequireDbBox("LogMgrDB")
  g_LogMgr.TongItemProduceEventLog(tong_id,build_name,2,uPlayerID)
	local info = CTongBuildingBox.GetCreatingBuildingItemInfoDB({["uPlayerID"] = uPlayerID,["sceneName"] = sceneName})
	return info,tblRetRes
end
---------------------------
--parameters["isRepair"] 为是否计算维修.
--在创建场景时候 加载帮会建筑调用时传true, 别的场合没必要
function CTongBuildingBox.GetTongBuildingInfo(parameters)
	local uTongId = parameters["uTongId"]
	local result = {}
	local TongBasicDB = RequireDbBox("GasTongBasicDB")
	result["TongCamp"] = TongBasicDB.GetTongCampByID(uTongId)
	if result["TongCamp"] == 0 then --说明不存在的帮会
		return false
	end
	local buildinginfo = CTongBuildingBox.GetBuildInfo(uTongId)
	local tbl = {}
	for i =1,buildinginfo:GetRowNum() do
		table.insert(tbl,{buildinginfo(i,1),buildinginfo(i,2),buildinginfo(i,3),buildinginfo(i,4),buildinginfo(i,5),buildinginfo(i,6),buildinginfo(i,7),buildinginfo(i,8),buildinginfo(i,9)})
	end
	result["BuildingTbl"] = tbl
	if parameters["isRepair"] then
		for _, v in pairs(result["BuildingTbl"]) do
			v[5] = v[5] + CTongBuildingBox.RepairSingleBuilding(v[1], v[5])
		end
	end
	return result
end

function CTongBuildingBox.GetTongChallengeBuildingInfo(parameters)
	local uChallengerId = parameters["uChallengerId"]
	local challengerResult = {}
	local TongBasicDB = RequireDbBox("GasTongBasicDB")
	challengerResult["TongCamp"] = TongBasicDB.GetTongCampByID(uChallengerId)
	if challengerResult["TongCamp"] == 0 then --说明不存在的帮会
		return false
	end
	local chanllengeRes = CTongBuildingBox.GetBuildInfo(uChallengerId)
	local tbl1 = {}
	for i =1,chanllengeRes:GetRowNum() do
		table.insert(tbl1,{chanllengeRes(i,1),chanllengeRes(i,2),chanllengeRes(i,3),chanllengeRes(i,4),chanllengeRes(i,5),chanllengeRes(i,6),chanllengeRes(i,7),chanllengeRes(i,8),chanllengeRes(i,9)})
	end
	challengerResult["BuildingTbl"] = tbl1
	if parameters["isRepair"] then
		for _, v in pairs(challengerResult["BuildingTbl"]) do
			v[5] = v[5] + CTongBuildingBox.RepairSingleBuilding(v[1], v[5])
		end
	end
	
	local uTargetId = parameters["uTargetId"]
	local targetResult = {}
	targetResult["TongCamp"] = TongBasicDB.GetTongCampByID(uTargetId)
	if targetResult["TongCamp"] == 0 then --说明不存在的帮会
		return false
	end
	local TargetRes = CTongBuildingBox.GetBuildInfo(uTargetId)
	local tbl2 = {}
	for i =1,TargetRes:GetRowNum() do
		table.insert(tbl2,{TargetRes(i,1),TargetRes(i,2),TargetRes(i,3),TargetRes(i,4),TargetRes(i,5),TargetRes(i,6),TargetRes(i,7),TargetRes(i,8),TargetRes(i,9)})
	end
	targetResult["BuildingTbl"] = tbl2
	if parameters["isRepair"] then
		for _, v in pairs(targetResult["BuildingTbl"]) do
			v[5] = v[5] + CTongBuildingBox.RepairSingleBuilding(v[1], v[5])
		end
	end
	
	return challengerResult, targetResult
end

function CTongBuildingBox.GetTongMaxLevel(uTongId)
	local result = StmtOperater._SelectMaxLevelPlayer:ExecSql("n", uTongId)
	local level
	if result:GetRowNum() ~= 0 then
		level = result:GetData(0,0)
	end
	result:Release()
	return level
end

function CTongBuildingBox.SelectMaxLevelPlayer(uTongId)
	local result = StmtOperater._SelectMaxLevelPlayer:ExecSql("n", uTongId)
	local level
	if result:GetRowNum() ~= 0 then
		level = result:GetData(0,0)
	end
	result:Release()
	return level
end

function CTongBuildingBox.GetTongMaxLevel(uTongId)
	local result = StmtOperater._SelectMaxLevelPlayer:ExecSql("n", uTongId)
	local level
	if result:GetRowNum() ~= 0 then
		level = result:GetData(0,0)
	end
	result:Release()
	return level
end


function CTongBuildingBox.SetBuildingLifeInfo(buildingId, hp, time)
	if time then
		StmtOperater.SetBuildingLifeInfo:ExecSql("", hp, time, buildingId)
	else
		StmtOperater.SetBuildingLife:ExecSql("", hp, buildingId)
	end
	--return g_DbChannelMgr:LastAffectedRowNum()>0
	--连续更新相同hp给同一个建筑会返回失败造成错误，例如战区大本营被爆，又抢回，又被爆
	return true
end

function CTongBuildingBox.UpdateTongBuildingLifeInfo(parameters)
	local buildingHpTbl = parameters["BuildingHpTbl"]
	local tongId = parameters["uTongID"]
	if buildingHpTbl then
		for buildingId, hp in pairs(buildingHpTbl) do
			CTongBuildingBox.SetBuildingLifeInfo(buildingId, hp)
		end
	end
end

function CTongBuildingBox.GetBuildingLife(buildingId)
	local result = StmtOperater._GetBuildingLife:ExecSql("n", buildingId)
	local hp
	if result:GetRowNum() ~= 0 then
		hp = result:GetData(0, 0)
	end
	result:Release()
	return hp
end

local REPAIR_INTERVAL = 6 * 60
local COST_RATE = 0.5
function CTongBuildingBox.RepairSingleBuilding(buildingId, hp)
	local buildingInfo = CTongBuildingBox.GetBuildingInfoById(buildingId)
	if buildingInfo then--建筑存在
		local name = buildingInfo[2]
		local lastUpdateTime = buildingInfo[7]
		local tongId = buildingInfo[8]
		local now = os.time()
		local tempHp = hp
		--print("名字",name)
		if hp < 1 then
			local repairRate = BuildingData(name, "RepairRate") / 100 --策划填表是按百分比填的, 程序里为了方便都是按比例, 所以 /100
			local repairHp = repairRate * (now - lastUpdateTime)/REPAIR_INTERVAL
			repairHp = math.min(repairHp, 1 - hp)
			local tong_box = RequireDbBox("GasTongBasicDB")
			local res = tong_box.GetTongResource(tongId)
			local NeedRes = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), name,"NeedRes")
			local costRes = math.ceil(NeedRes * repairHp * COST_RATE) --黑心点 ^_^, 不用floor
			--print("生命值小于 1 需要维修, 维修值为 ", repairHp, " 剩余资源", res, " 维修花费资源",costRes)
			if costRes > res then
				costRes = res
				repairHp = res/(NeedRes * COST_RATE)
				--print("由于维修资源不够, 维修值调整为 ", repairHp, " 消耗资源为",costRes)
			end
			if costRes > 0 then
				CTongBuildingBox.UpdateTongResource(-costRes, tongId,event_type_tbl["佣兵小队建筑修理"])
			end
			hp = hp + repairHp
		end
		CTongBuildingBox.SetBuildingLifeInfo(buildingId, hp, now)
		return hp - tempHp
	end
end

function CTongBuildingBox.AutoRepairBuilding(data)
	local tongId = data["tongId"]
	local buildingTbl = data["buildingTbl"]
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local sceneType, sceneId, serverId, warZoneId, stationId = WarZoneDB.GetTongStationInfo({["tongId"] = tongId})
	if serverId == 0 then
		return
	end
	local result = {}

	for buildingId, hp in pairs(buildingTbl) do
		--print("准备维修建筑", buildingId , "场景中的生命值为 ", hp)
		local addHp = CTongBuildingBox.RepairSingleBuilding(buildingId, math.max(math.min(hp, 1), 0) )
		if addHp and addHp ~= 0 then
			result[buildingId] = addHp
		end
	end
		
	if next(result) then
		local argTbl = {}
		argTbl.TongId = tongId
		argTbl.AutoRepairHpTbl = result
		
		Db2GasCall("AutoRepairBuilding", argTbl, serverId)
	end
end


--【获得帮会成员的最高等级】
function CTongBuildingBox.UpdateBuildingLevel(data)
	local uPlayerID = data["uPlayID"]
	local buildingId = data["buildingId"]
	local costResource = data["uCostResource"]
	local curLevel = data["uCurrentLevel"]
	local TongBasicDB = RequireDbBox("GasTongBasicDB")
	local uPlayerTongID = TongBasicDB.GetTongID(uPlayerID)
	if not uPlayerTongID or uPlayerTongID == 0 then
		return false
	end
	--获得等级
	local newLevel = CTongBuildingBox.SelectMaxLevelPlayer(uPlayerTongID)
	if curLevel >= newLevel then
		return false, 9335
	end
	if not TongBasicDB.JudgePurview(uPlayerID, "UpdateLevel") then
		return false, 9006
	end
	local tong_box = RequireDbBox("GasTongBasicDB")
	local res = tong_box.GetTongResource(uPlayerTongID)
	if res < costResource then
		return false, 9310
	end
	if costResource > 0 then
		if not CTongBuildingBox.UpdateTongResource(-costResource, uPlayerTongID,event_type_tbl["佣兵小队更新建筑等级"]) then
			return false,9310
		end
	end
	
	local buildingInfo = CTongBuildingBox.GetBuildingInfoById(buildingId)
	if not buildingInfo or buildingInfo[8] ~= uPlayerTongID then
		return false
	end
	
	--更新等级
	StmtOperater._UpdateBuildLevel:ExecSql('', newLevel, buildingId)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
  	CancelTran()
 		return false
	end
	
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local sceneType, sceneId, serverId, warZoneId, stationId = WarZoneDB.GetTongStationInfo({["tongId"] = uPlayerTongID})
	local argTbl = {}
	argTbl.TongId = uPlayerTongID
	argTbl.BuildingId = buildingId
	argTbl.NewLevel = newLevel
	
	Db2GasCall("UpdateBuildingLevel", argTbl, serverId)
	return true,newLevel
end

function CTongBuildingBox.GetAllBuildName(CharId)
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local TongId = GasTongBasicDB.GetTongID(CharId)
	local res = StmtOperater._GetAllBuildName:ExecStat(TongId)
	if res then
		local BuildTbl = {}
		for i=0, res:GetRowNum()-1 do
			table.insert(BuildTbl, res:GetData(i, 0))
		end
		return BuildTbl
	end
end

return CTongBuildingBox
