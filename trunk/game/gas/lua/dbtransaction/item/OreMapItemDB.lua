gac_gas_require "item/item_info_mgr/ItemInfoMgr"
cfg_load "item/OreAreaMap_Common"
cfg_load "liveskill/OreMapPosition_Common"
gac_gas_require "activity/scene/LoadPositionCfg"
gac_gas_require "activity/quest/QuestMgr"

local GetOreMapPosition = GetOreMapPosition
local GetCfgTransformValue = GetCfgTransformValue
local StmtContainer = class()
local event_type_tbl = event_type_tbl
local OreAreaMap_Common = OreAreaMap_Common
local OreMapPosition_Common = OreMapPosition_Common
local CfgLogErr = CfgLogErr

local OreMapItemDB = CreateDbBox(...)
local NpcPosTbl = {}
--========================================================================
local function RandomPosByScene(SceneName)
	local Info = OreMapPosition_Common(SceneName)
	if Info and Info("Camp") ~= 0 then
		if not  NpcPosTbl[SceneName] then
			NpcPosTbl[SceneName] = GetCfgTransformValue(false, "OreMapPosition_Common", SceneName, "PosTable")
		end
		local n = math.random(table.maxn(NpcPosTbl[SceneName]))
		return GetOreMapPosition(NpcPosTbl[SceneName][n])
	end
end

local function RandomPosByCampScene(Camp, ItemName)
	local nameTbl = {}
	local Level = OreAreaMap_Common(ItemName,"SkillLevel")
	local flag = false
	for _, i in pairs(OreMapPosition_Common:GetKeys()) do
		local info = OreMapPosition_Common(i)
		if info("Camp") == Camp and Level >= info("MinLevel") and Level <= info("MaxLevel") then
			table.insert(nameTbl, i)
			flag = true
		end
	end
	if not flag then
		CfgLogErr("矿洞地图坐标表填写错误!","矿藏图("..ItemName..")无法随机到("..Camp..")阵营,等级("..Level..")的地图.")
	end
	local n = math.random(table.maxn(nameTbl))
	return RandomPosByScene(nameTbl[n])
end
--========================================================================
local StmtDef = {
			"_SelectOreMap",
			--查询矿藏图的动态信息
			[[ 
				select is_uId,io_sName,io_uPosX,io_uPosY from tbl_item_oremap where is_uId = ?
			]]
}
DefineSql (StmtDef,StmtContainer)


local StmtDef = {
			"_UpdateOreMap",
			--查询矿藏图的动态信息
			[[ 
				update tbl_item_oremap set io_sName = ?,io_uPosX = ?,io_uPosY = ? where is_uId = ?
			]]
}
DefineSql (StmtDef,StmtContainer)


--通过物品ID查询物品动态表
function OreMapItemDB.QueryMapInfo(data)
	local nCharID = data["nCharID"]
	local nRoom = data["nRoom"]
	local nPos = data["nPos"]
	local ItemName = data["ItemName"]
	local tbl = nil

	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local ItemType,sItemName,ItemId = g_RoomMgr.GetOneItemByPosition(nCharID,nRoom,nPos)
	if ItemType ~= 30 or sItemName ~= ItemName then
		return false
	end
	local smithing_skill_mgr = RequireDbBox("LiveSkillBaseDB")
	local skillLevel = smithing_skill_mgr.GetSkillLevelByType(data["nCharID"], "采矿")
	local isImmeEnter = OreAreaMap_Common(sItemName,"ImmeEnter")
	if isImmeEnter == 0 then
		local res = StmtContainer._SelectOreMap:ExecStat(ItemId)
		if res:GetRowNum()> 0 then
			if res:GetData(0,2) == 1 then
				local campRet = StmtContainer._QueryCharCamp:ExecStat(nCharID)
				local uPosX, uPosY, sSceneName = RandomPosByCampScene(campRet:GetData(0,0), sItemName)
				StmtContainer._UpdateOreMap:ExecStat(sSceneName,uPosX, uPosY,ItemId)
				tbl= {
							ItemId,
							sSceneName,
							uPosX,
							uPosY,
							skillLevel
							}
			else
				tbl= {
							res:GetData(0,0),
							res:GetData(0,1),
							res:GetData(0,2),
							res:GetData(0,3),
							skillLevel
							}
			end
			res:Release()
		end
		return tbl
	elseif isImmeEnter == 1 then
		tbl = {ItemId,skillLevel}
		return tbl
	end
end
--========================================================================
local StmtDef = {
			"_SelectOreMapSceneName",
			--查询矿藏图的动态信息
			[[ 
				select io_sName from tbl_item_oremap where is_uId = ?
			]]
}
DefineSql (StmtDef,StmtContainer)

--通过物品ID查询物品地图
function OreMapItemDB.QueryItemSceneName(ItemId)
	local result = StmtContainer._SelectOreMapSceneName:ExecStat(ItemId)
	if result:GetRowNum() ~= 0 then
		local sceneName = result:GetData(0,0)
		return sceneName
	end
end
--========================================================================
local StmtDef = {
	"_SaveOreMap",
	--保存矿藏图的动态信息
	[[ 
		insert into tbl_item_oremap values(?,?,?,?)
	]]
}
DefineSql (StmtDef,StmtContainer)

local StmtDef = {
	"_QueryCharCamp",
	[[ 
		select cs_uCamp from tbl_char_static where cs_uId = ?
	]]
}
DefineSql (StmtDef,StmtContainer)

--添加新矿藏图时候给矿藏图的动态信息表里添加记录
function OreMapItemDB.SaveOreMapById(ItemId,sItemName,charId)
	local isImmeEnter = OreAreaMap_Common(sItemName,"ImmeEnter")
	if isImmeEnter == 0 then
		local campRet = StmtContainer._QueryCharCamp:ExecStat(charId)
		local uPosX, uPosY, sSceneName = RandomPosByCampScene(campRet:GetData(0,0), sItemName)
		StmtContainer._SaveOreMap:ExecSql("",ItemId,sSceneName,uPosX, uPosY)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	else
		return true
	end
end
--========================================================================
--通过物品ID删除物品动态表(根据物品ID)
function OreMapItemDB.DeleteOreMapByID(data)
	local nCharID = data["nCharID"]
	local ItemId = data["ItemId"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")

	local bRet = g_RoomMgr.DelItemByID(nCharID,ItemId,event_type_tbl["物品使用"])
	return bRet
end

SetDbLocalFuncType(OreMapItemDB.QueryMapInfo)

return OreMapItemDB
