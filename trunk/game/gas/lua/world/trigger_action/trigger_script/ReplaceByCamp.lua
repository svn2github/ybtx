local function Script(Arg, Trigger, Player)

	if not Player or not IsCppBound(Player) then
		return
	end
	
	if g_WarZoneMgr.m_ResourceOccupyState == "Open" then
	
		if Trigger.m_Replace then
			MsgToConn(Player.m_Conn,3292)
			return
		end
		
		local function SuccFun(data)
		
			if Trigger then
				Trigger.m_Replace = nil
			end
			if g_WarZoneMgr.m_ResourceOccupyState == "Open" then
				if not IsCppBound(Player) then
					return
				end
				if not IsServerObjValid(Trigger) then
					return
				end
				g_TriggerScript.ScriptFun["Ìæ»»"](Arg[data["Camp"]], Trigger, Player)
			end
		end
		
		local function FailFun(data)
			Trigger.m_Replace = nil
		end
		
		local data = {}
		data["Camp"] = Player:CppGetCamp()
		
	 	if CommLoadProgress(Player, 60000, SuccFun, FailFun, data) then
	 		Trigger.m_Replace = true
	 	end
	end
end

return Script