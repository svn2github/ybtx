cfg_load "trigger/CountTrigger_Server"
CCountTrigger = class()

function CCountTrigger:AddCountAddFun(Npc)
	local name = Npc.m_Properties:GetCharName()
	for _, v in pairs(self.m_NameTbl) do
		if v == name then
			self.m_AddCount = self.m_AddCount + 1
			if self.m_AddCount >= self.m_Count then
				self:TriggerScript(Npc)
				Npc.m_Scene.m_CountTrigger = nil
				self = nil
			end
			break
		end
	end
end

function CCountTrigger:AddCountDelFun(Npc)
	local name = Npc.m_Properties:GetCharName()
	for _, v in pairs(self.m_NameTbl) do
		if v == name then
			if self.m_AddCount > 0 then
				self.m_AddCount = self.m_AddCount - 1
			end
			break
		end
	end
end

function CCountTrigger:DestoryCountDelFun(Npc)
	local name = Npc.m_Properties:GetCharName()
	for _, v in pairs(self.m_NameTbl) do
		if v == name then
			self.m_Count = self.m_Count - 1
			if self.m_Count <= 0 then
				self:TriggerScript(Npc)
				Npc.m_Scene.m_CountTrigger = nil
				self = nil
			end
			break
		end
	end
end

function CCountTrigger:AllDestoryAddFun(Npc)
	local name = Npc.m_Properties:GetCharName()
	for _, v in pairs(self.m_NameTbl) do
		if v == name then
			self.m_AddCount = self.m_AddCount + 1
		end
	end
end

function CCountTrigger:AllDestoryDelFun(Npc)
	local name = Npc.m_Properties:GetCharName()
	for _, v in pairs(self.m_NameTbl) do
		if v == name then
			self.m_AddCount = self.m_AddCount - 1
			if self.m_AddCount <= 0 then
				self:TriggerScript(Npc)
				Npc.m_Scene.m_CountTrigger = nil
				self = nil
			end
			break
		end
	end
end

local AddNpcFun = {
	[1] = true,
	[2] = CCountTrigger.AddCountAddFun,
	[3] = CCountTrigger.AllDestoryAddFun,
}

local DelNpcFun = {
	[1] = CCountTrigger.DestoryCountDelFun,
	[2] = CCountTrigger.AddCountDelFun,
	[3] = CCountTrigger.AllDestoryDelFun,
}

function CCountTrigger:Ctor(Index)
	self.m_Type = CountTrigger_Server(Index, "Type")
	self.m_NameTbl = GetCfgTransformValue(true, "CountTrigger_Server", Index, "Name")
	self.m_Count = CountTrigger_Server(Index, "Count")
	self.m_AddCount = 0
	self.m_Script = GetCfgTransformValue(true, "CountTrigger_Server", Index, "Script")
end

function CCountTrigger:AddNpcCount(Npc)
	if IsFunction(AddNpcFun[self.m_Type]) then
		AddNpcFun[self.m_Type](self, Npc)
	end
end

function CCountTrigger:MinusNpcCount(Npc)
	if IsFunction(DelNpcFun[self.m_Type]) then
		DelNpcFun[self.m_Type](self, Npc)
	end
end

function CCountTrigger:TriggerScript(Npc)
	for _,Script in pairs(self.m_Script) do
		g_TriggerScript:RunScriptByIndex(Script, Npc)
	end
end