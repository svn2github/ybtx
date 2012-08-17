local function Script(Arg, Trigger, Player)

	local Scene = Trigger.m_Scene
	local function ResourceOccupyReset()
		local TempNpc = {}
		for i, Name in pairs(Arg[2]) do
			local SceneId = Scene.m_SceneId
			if g_NpcBornMgr._m_AllNpc[SceneId] then 
				for _, NpcObj in pairs(g_NpcBornMgr._m_AllNpc[SceneId]) do
					if NpcObj.m_Properties:GetCharName() == Name then
						table.insert(TempNpc, NpcObj)
					end
				end
			end
		end
		local ReplaceNpc = nil
		for _, NpcObj in pairs(TempNpc) do
			if ReplaceNpc then
				g_NpcServerMgr:DestroyServerNpcNow(NpcObj, false)
			else
				ReplaceNpc = true
				g_TriggerScript.ScriptFun["Ìæ»»"](Arg[3], NpcObj)
			end
		end
	end
	g_AlarmClock:AddTask("ResourceOccupyReset", Arg[1], ResourceOccupyReset)
end

return Script