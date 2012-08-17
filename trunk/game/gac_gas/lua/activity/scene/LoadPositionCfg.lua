cfg_load "scene/Position_Common"
cfg_load "liveskill/OreMap_Pos_Common"
gac_gas_require "scene_mgr/SceneCfg"

--local function GetItemTbl(item)
--	local tbl = loadstring("return" .. item)()
--	return tbl
--end

--加载Position_Common表
--local function LoadPosition_Common()
--	for r, TblInfo in pairs(Position_Common) do 
--		local Position = GetItemTbl(TblInfo.Position)
--		rawset(TblInfo, "Position", Position)	
--	end 
--end

--LoadPosition_Common()
local function CheckPosition(scene, PosX, PosY, str, PosID)
	if scene then
		local CoreScene = scene.m_CoreScene
		local pos = CPos:new(PosX, PosY)
		local barrierType = CoreScene:GetBarrier(pos)
		if barrierType == EBarrierType.eBT_HighBarrier then
			CfgLogErr("坐标配置表填写错误！",str.." ID("..PosID..") 所填坐标("..PosX..","..PosY..")在地图("..scene.m_SceneName..")是障碍")
		elseif barrierType == EBarrierType.eBT_OutRange then
			CfgLogErr("坐标配置表填写错误！",str.." ID("..PosID..") 所填坐标("..PosX..","..PosY..")超出地图("..scene.m_SceneName..")范围")
		end
	end
end

function GetScenePosition(PosID, scene)
	PosID = tonumber(PosID)
	if Position_Common(PosID) then
		local PosX = Position_Common(PosID, "PosX")
		local PosY = Position_Common(PosID, "PosY")
		local SceneName = Position_Common(PosID, "SceneName")
		CheckPosition(scene, PosX, PosY, "Position_Common配置表", PosID)
		return PosX, PosY, SceneName
	else
		CfgLogErr("Position_Common配置表填写错误！","坐标ID("..PosID..")不存在")
	end
end

function GetScenePositionTbl(PosID, scene)
	PosID = tonumber(PosID)
	if Position_Common(PosID) then
		local PosX = Position_Common(PosID, "PosX")
		local PosY = Position_Common(PosID, "PosY")
		local SceneName = Position_Common(PosID, "SceneName")
		CheckPosition(scene, PosX, PosY, "Position_Common配置表", PosID)
		return {PosX, PosY}, SceneName
	else
		CfgLogErr("Position_Common配置表填写错误！","坐标ID("..PosID..")不存在")
	end
end

--获取矿洞坐标信息
function GetOreMapPosition(PosID, scene)
	PosID = tonumber(PosID)
	if OreMap_Pos_Common(PosID) then
		local PosX = OreMap_Pos_Common(PosID, "PosX")
		local PosY = OreMap_Pos_Common(PosID, "PosY")
		local SceneName = OreMap_Pos_Common(PosID, "SceneName")
		CheckPosition(scene, PosX, PosY, "OreMap_Pos_Common配置表", PosID)
		return PosX, PosY, SceneName
	else
		CfgLogErr("OreMap_Pos_Common配置表填写错误！","坐标ID("..PosID..")不存在")
	end
end
