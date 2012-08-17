gas_require "world/tong_area/DynamicCreateMgrInc"
gac_gas_require "activity/npc/LifeOriginCommon"
gas_require "world/npc/LifeOrigin"

g_DynamicCreateMgr = CDynamicCreateMgr:new()
g_DynamicCreateMgr.m_DynamicIndexTbl = {} --动态摆怪文件列表
g_DynamicCreateMgr.m_DataTbl = {}
g_DynamicCreateMgr.m_DynamicPathTbl = {} --动态巡逻路径表

AddCheckLeakFilterObj(g_DynamicCreateMgr.m_DataTbl)


function CDynamicCreateMgr:Init(tbl)
	g_DynamicCreateMgr.m_DynamicIndexTbl = tbl or {}
	AddCheckLeakFilterObj(g_DynamicCreateMgr.m_DynamicIndexTbl)
	
	for name, fileName in pairs(g_DynamicCreateMgr.m_DynamicIndexTbl) do
		local filePath = "scene/dynamic_file/" .. fileName
		cfg_require (filePath)
	end
end

function CDynamicCreateMgr:AddDynamicData(fileName, tbl)
	self.m_DataTbl[fileName] = tbl
	
	if tbl.npc then
		g_LifeOrigin:AddSinglePointBornTbl(fileName, tbl.npc)
	end
	if tbl.obj then
		g_IntObjServerMgr:AddIntObjTbl(fileName, tbl.obj)
	end
	if tbl.trap then
		g_TrapServerMgr:AddTrapTbl(fileName, tbl.trap)
	end
end

function CDynamicCreateMgr:InitPath(tbl)
	g_DynamicCreateMgr.m_DynamicPathTbl = tbl or {}
	AddCheckLeakFilterObj(g_DynamicCreateMgr.m_DynamicPathTbl)
end

cfg_require "scene/dynamic_file/DynamicFileList"   --里面使用到CDynamicCreateMgr:Init 函数 所以放它后面
cfg_require "scene/DynamicPath"

function CDynamicCreateMgr:Create(scene, name, centerPos)
	local key = g_DynamicCreateMgr.m_DynamicIndexTbl[name]
	local NpcTbl = {}
	local createTbl = g_DynamicCreateMgr.m_DataTbl[key]
	if createTbl then
		scene.m_DynamicCenterPos = centerPos or createTbl.centerPos  --centerPos为nil则是绝对坐标创建,(指摆怪编辑器上的位置)
		local function Init()
			if createTbl.npc then
				NpcTbl = g_NpcBornMgr:CreateSceneNpc(scene, key, true)
			end
			if createTbl.obj then
				g_IntObjServerMgr:CreateSceneIntObj(scene, key, true)
			end
			if createTbl.trap then
				g_TrapServerMgr:CreateSceneTrap(scene, key, true)
			end
		end
		apcall(Init)
		scene.m_DynamicCenterPos = nil
	end
	return NpcTbl
end

function CDynamicCreateMgr:CreatePath(Npc, sceneName, pathName, centerPos)
	if Npc:HaveBeSetPath() then
		local str = "Npc:【" .. Npc.m_Name .. "】在场景【" .. sceneName .. "】已经在摆怪编辑器里面设置了巡逻路径，出生脚本中又再次设置了路径！这是不允许的！请转交策划处理！";
		error(str)
	end
	
	local Setting = self.m_DynamicPathTbl[sceneName] and self.m_DynamicPathTbl[sceneName][pathName]
	local CenterPos = centerPos or Setting.CenterPos
	local centerPosX, centerPosY = 0, 0
	if CenterPos then
		if CenterPos.x and CenterPos.y then
			centerPosX = CenterPos.x
			centerPosY = CenterPos.y
		else
			centerPosX = CenterPos[1]
			centerPosY = CenterPos[2]
		end
	end
	
	if Setting and ( Setting.MoveType == 4 or Setting.MoveType == 5 or Setting.Path[2] ) then
		local centerPosTbl = {centerPosX, centerPosY}
		g_NpcBornMgr:SetNpcMovePath(Npc, Setting, centerPosTbl)
	else
		local str1 = "Npc:【" .. Npc.m_Name .. "】在场景【" .. sceneName .. "】中动态路径设置错误！路径必须不能为空！且除单线巡逻外至少有两个巡逻点,因为站在一个点上是没法巡逻的！！！";
		error(str1)
	end 
end

--针对修行塔等做的特定的添加动态路径，与CreatePath不同
function CDynamicCreateMgr:CreateMercenaryPath(Npc, sceneName, pathName)
	if Npc:HaveBeSetPath() then
		local str = "Npc:【" .. Npc.m_Name .. "】在场景【" .. sceneName .. "】已经在摆怪编辑器里面设置了巡逻路径，出生脚本中又再次设置了路径！这是不允许的！请转交策划处理！";
		error(str)
	end
	
	local Setting = self.m_DynamicPathTbl[sceneName] and self.m_DynamicPathTbl[sceneName][pathName]
	local CenterPos = Npc.m_Scene.m_CenterPos or Setting.CenterPos
	local centerPosX, centerPosY = 0, 0
	if CenterPos then
		centerPosX = CenterPos[1]
		centerPosY = CenterPos[2]
	end
	
	if Setting and ( Setting.MoveType == 4 or Setting.Path[2] ) then
		local centerPosTbl = {centerPosX, centerPosY}
		g_NpcBornMgr:SetNpcMovePath(Npc, Setting, centerPosTbl)
	else
		local str1 = "Npc:【" .. Npc.m_Name .. "】在场景【" .. sceneName .. "】中动态路径设置错误！路径必须不能为空！且除单线巡逻外至少有两个巡逻点,因为站在一个点上是没法巡逻的！！！";
		error(str1)
	end 
end

function CDynamicCreateMgr:GetPathFirstPos(Npc, sceneName, pathName, centerPos)
	if Npc:HaveBeSetPath() then
		local str = "Npc:【" .. Npc.m_Name .. "】在场景【" .. sceneName .. "】已经在摆怪编辑器里面设置了巡逻路径，出生脚本中又再次设置了路径！这是不允许的！请转交策划处理！";
		error(str)
	end
	
	local path = self.m_DynamicPathTbl[sceneName] and self.m_DynamicPathTbl[sceneName][pathName]
	local CenterPos = nil
	if centerPos then
		CenterPos = centerPos or path.CenterPos
	else
		CenterPos = Npc.m_Scene.m_CenterPos or path.CenterPos
	end
	local centerPosX, centerPosY = 0, 0
	if CenterPos then
		centerPosX = CenterPos[1]
		centerPosY = CenterPos[2]
	end
	
	local firstPos = {}
	local point = path.Path[1]
	if point then
		firstPos.x = point[1] + centerPosX
		firstPos.y = point[2] + centerPosY
		return firstPos
	end
end
