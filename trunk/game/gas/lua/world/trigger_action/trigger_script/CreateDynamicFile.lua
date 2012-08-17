local function Script(Arg, Trigger, Player)
	local thisArg = Arg
	local centerPos
	if thisArg[2] == "Npc为中心" then
		local pos = CPos:new()
		Trigger:GetGridPos(pos)
		centerPos = {pos.x, pos.y}
	elseif thisArg[2] then
		local ID = thisArg[2]
		local CreatScene = Trigger.m_Scene
		local x, y, scene = GetScenePosition(ID, CreatScene)
		if scene ~= CreatScene.m_SceneName then
			CfgLogErr("npc触发配置表脚本填写错误！",Trigger.m_Properties:GetCharName().."(创建动态摆怪文件)脚本中填写的坐标ID("..ID..")在坐标表中填写的地图(".. scene ..")与实际地图(".. CreatScene.m_SceneName ..")不符！")
			return
		end
		centerPos = {x,y}
	end
	
	g_DynamicCreateMgr:Create(Trigger.m_Scene, thisArg[1], centerPos)
end 

return Script