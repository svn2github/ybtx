cfg_load "trigger/TimeTrigger_Server"
CTimeTrigger = class()

function CTimeTrigger:Ctor(Index, Trigger)
	self.m_Index = Index
	self.m_Time = GetCfgTransformValue(true, "TimeTrigger_Server", Index, "Time")
	self.m_Script = GetCfgTransformValue(true, "TimeTrigger_Server", Index, "Script")
	self.m_Trigger = Trigger
	self:StartTrigger()
end

function CTimeTrigger:StartTrigger()
	local function RunScript()
		if not IsServerObjValid(self.m_Trigger) then
			self:StopTrigger()
			return
		end
		self:TriggerScript(self.m_Trigger)
	end
	self.m_TaskIndex = g_AlarmClock:AddTask(self.m_Index, self.m_Time, RunScript)
end

function CTimeTrigger:StopTrigger()
	g_AlarmClock:RemoveTask(self.m_TaskIndex)
	if self.m_Trigger.m_TimeTrigger then
		self.m_Trigger.m_TimeTrigger = nil
	end
	self.m_Trigger = nil
	self = nil
end

function CTimeTrigger:TriggerScript(Npc)
	for _,Script in pairs(self.m_Script) do
		g_TriggerScript:RunScriptByIndex(Script, Npc)
	end
end