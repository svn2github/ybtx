gas_require "fb_game/random_game/RandomGameMgrInc"

g_RandomGameMgr = CRandomGameMgr:new()
function RandomGameTrigger(TriggerType, Npc, Player)
	if Npc.m_CreatorEntityID then
		local CreatorEntityID = Npc.m_CreatorEntityID
		local Creator = CEntityServerManager_GetEntityByID(CreatorEntityID)
		if not IsServerObjValid(Creator) then
			return
		end
		if Creator.m_RandomGame then
			Creator.m_RandomGame:TriggerEvent(TriggerType, Npc, Player)
		end
	end
end

function CRandomGameMgr:Ctor(GameName)
	self.m_AllTriggerIndex = {}
	self.m_GameName = GameName
	self.m_info = g_RandomGame[GameName]
	self.m_RandomR = nil
	self.m_RandomL = nil
end

--∑÷≈‰ÕÊ∑®
function CRandomGameMgr:AssignGame()
	local info = self.m_info
	local RMax = table.maxn(info.Subject)
	local LMax = table.maxn(info.Subject[1])
	local i = 0
	while i < info.SubjectNum do
		local r = math.random(1,RMax)
		local l = math.random(1,LMax)
		local index = info.Subject[r][l]
		if not self.m_AllTriggerIndex[index] then
--			print(index)
			self.m_AllTriggerIndex[index] = true
			i = i + 1
		end
		
		if info.SubjectNum == 1 then
			self.m_RandomR = r
			self.m_RandomL = l
		end
	end
end

function CRandomGameMgr:StartGame()
	self:AssignGame()
end

function CRandomGameMgr:NextGame()
	self:AssignGame()
end

function CRandomGameMgr:TriggerEvent(TriggerType, Npc, Master)
	local NpcName = Npc.m_Properties:GetCharName()
	local info = self.m_info
	local AllScript = nil
	if self.m_AllTriggerIndex[NpcName] then
		AllScript = info.SelectTrigger
	else
		AllScript = info.OtherTrigger
		if self.m_RandomR then
			for l, v in pairs(info.Subject[self.m_RandomR]) do
				if NpcName == v then
--					print("if NpcName == v then",NpcName)
					AllScript = info.HintTrigger
					break
				end
			end
			for r, v in pairs(info.Subject) do
				if NpcName == info.Subject[r][self.m_RandomL] then
--					print("if NpcName == info.Subject[r][self.m_RandomL] then",NpcName)
					AllScript = info.HintTrigger
					break
				end
			end
		end
	end
	for i, Script in pairs(AllScript) do
		if Script[1] == TriggerType then
			for k = 2, #(Script) do
--				print(TriggerType,Script[k])
				g_TriggerScript:RunScriptByIndex(Script[k], Npc, Master)
			end
		end
	end
end
