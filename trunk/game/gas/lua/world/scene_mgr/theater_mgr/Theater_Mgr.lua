cfg_load "npc/NpcTheater_Server"

CTheaterMgr = class()

local function TriggerTheater(TheaterName, SceneId, StarterId, RandomN)
	local tbl = {}
	local Scene = g_SceneMgr:GetScene(SceneId)
	local TheaterMgr = Scene.m_TheaterMgr
	for _, i in pairs(NpcTheater_Server:GetKeys()) do
		local v = NpcTheater_Server(i)
		if string.sub(i, 1, string.len(i)-3) == TheaterName then
			local num = tonumber(string.sub(i, string.len(i)-2, -1))
			tbl[num] = {}
			tbl[num].StepName = i
			tbl[num].Action = v("Action")
			tbl[num].NpcName = v("NpcName")
			tbl[num].Parameter1 = v("Parameter1")
			tbl[num].Parameter2 = v("Parameter2")
			tbl[num].Parameter3 = v("Parameter3")
			if v("NpcName") then
				TheaterMgr.m_NpcTheaterMap[v("NpcName")] = TheaterName
			end
		end
	end
	if tbl == {} then
		error("名为【" .. TheaterName .. "】的剧场不存在，请检查配置表")
		return
	end
	TheaterMgr.m_TheaterTbl[TheaterName] = CTheaterFun:new(TheaterName, tbl, SceneId, StarterId, RandomN)
	TheaterMgr.m_TheaterTbl[TheaterName]:RunTheater()
end

function CTheaterMgr:Ctor(Scene)
	RegMemCheckTbl("Theater"..Scene.m_SceneName..Scene.m_SceneId, Scene.m_TheaterMgr)		
	self.m_NpcTheaterMap = {}
	self.m_TheaterTbl = {}
	self.m_BeLockTbl = {}
	self.m_SceneId = Scene.m_SceneId
end

--暴露给外面触发执行的剧场
function CTheaterMgr:RunTriggerTheater(TheaterName, StarterId, RandomN)
	local Scene = g_SceneMgr:GetScene(self.m_SceneId)
	if Scene == nil then
		LogErr("剧场运行错误!","触发剧场的场景错误!SceneId("..self.m_SceneId..").")
		return
	end
	if self:BeTheaterLocked(TheaterName) then
		print("剧场 ：【" .. TheaterName .. "】 已被锁定，不能再执行，必须先解锁！")
		return ("剧场 ：【" .. TheaterName .. "】 已被锁定，不能再执行，必须先解锁！")
	else
		if Scene.m_TheaterMgr.m_TheaterTbl[TheaterName] then
			print("剧场 ：【" .. TheaterName .. "】 正在运行中！")
			return ("剧场 ：【" .. TheaterName .. "】 正在运行中！")
		else
			TriggerTheater(TheaterName, self.m_SceneId, StarterId, RandomN)
		end
	end
	return false
end

function CTheaterMgr:RunTriggerTheaterBySex(TheaterName, StarterId)
	local Player = CCharacterDictator_GetCharacterByID(StarterId)
	local SexId = Player.m_Properties:GetSex()
	if SexId == ECharSex.eCS_Male then
		self:RunTriggerTheater(TheaterName, StarterId, 1)
	elseif SexId == ECharSex.eCS_Female then
		self:RunTriggerTheater(TheaterName, StarterId, 2)
	end
end

function CTheaterMgr:RunTriggerRandomTheater(TheaterName, StarterId)
	self:RunTriggerTheater(TheaterName, StarterId, true)
end

function CTheaterMgr:StartServerRunTheater()
	self:RunTriggerTheater("启动剧场", nil)
end

function CTheaterMgr:BeTheaterLocked(TheaterName)
	local Scene = g_SceneMgr:GetScene(self.m_SceneId)
	if Scene.m_TheaterMgr.m_BeLockTbl[TheaterName] then
		return true
	end
end

function CTheaterMgr:LockTheater(TheaterName)
	self.m_BeLockTbl[TheaterName] = true
end

function CTheaterMgr:UnLockTheater(TheaterName)
	self.m_BeLockTbl[TheaterName] = nil
end

function CTheaterMgr:DestroySceneTheater()
	if self.m_NpcTheaterMap then
		for i, v in pairs(self.m_NpcTheaterMap) do
			self.m_NpcTheaterMap[i] = nil
		end
		self.m_NpcTheaterMap = nil
	end
	
	if self.m_BeLockTbl then
		for i, v in pairs(self.m_BeLockTbl) do
			self.m_BeLockTbl[i] = nil
		end
		self.m_BeLockTbl = nil
	end
	
	if self.m_TheaterTbl then
		for i, Theater in pairs(self.m_TheaterTbl) do
			if Theater then
				Theater.m_IsClosed = true
				Theater.m_Sleep:Wake()
			end
		end
		self.m_TheaterTbl = nil
	end
	--local Scene = g_SceneMgr:GetScene(self.m_SceneId)
	--self.m_SceneId = nil
	--Scene.m_TheaterMgr = nil
	self = nil
end
