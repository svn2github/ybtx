local function Script(Arg, Trigger, Player)
	if table.maxn(Trigger.m_OnTrapCharIDTbl) > 0 then
		return
	end
	
	if Arg[1] == nil or Arg[1] == "" then
		return
	end
	local SceneId = Player.m_Scene.m_SceneId
	local IntObj = GetOnlyIntObj(SceneId, Arg[1])
	if IntObj == nil then
		return
	end
	if next(Trigger.m_OnTrapCharIDTbl) then
		g_IntObjServerMgr:IntObjChangeState(IntObj, 0)
	else 
		g_IntObjServerMgr:IntObjChangeState(IntObj, 1)
	end
end

return Script
