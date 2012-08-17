cfg_load "item/MatchGameItem_Common"
local TempAreaTbl = {
	["艾米帝国幻兽先遣营地"] = true,
	["神圣防卫军幻兽骑士营地"]= true,
	["修斯新军幻兽先遣营地"]= true,
	["火狮子血誓营地"] = true,
	["神圣西征河谷营地"]= true,
	
	["河谷新军营地"]= true,
	["艾米火狮子凯旋营地"]= true,
	["神圣西征河阳营地"]= true,
	["河阳新军营地"]= true,
	
	["雪翼露台"]= true,
	["林苑小镇"]= true,
	["风歌小镇"]= true,

	["西林村"]= true,
	["神圣机密营地"]= true,
	["修斯西林船队"]= true,
	["西林交战区"]= true,
	
	["艾米前线传主城"]= true,
	["艾米前线传副本驻地"]= true,
	["艾米前线传花语平原"]= true,
	["神圣前线传主城"]= true,
	["神圣前线传副本驻地"]= true,
	["神圣前线传花语平原"]= true,
	["修斯前线传主城"]= true,
	["修斯前线传副本驻地"]= tr,
	["修斯前线传花语平原"]= true,
	["前线运输2"]= true,
	["前线运输"]= true,
	
}


local function DoMethod(Player, type, value)
	if type == "灵感值" then
		if IsCppBound(Player) then
			Player:AddPlayerAfflatusValue(value)
		end
	end
end

local function CanPk(Player, BuffSkillName)
	if string.find(BuffSkillName, "免死") then
		if IsCppBound(Player) then
			local scene = Player.m_Scene
			local Pos = CPos:new()
			Player:GetGridPos(Pos)
			local SceneName = scene.m_SceneName
			local areaName = g_AreaMgr:GetSceneAreaName(SceneName, Pos)
			if TempAreaTbl[areaName] then
				if string.find(areaName, "前线")  and Player:CppGetLevel() <= 40 then
					Player:CppSetPKState(false)
					Gas2Gac:UpdatePkSwitchState(Player.m_Conn)
					Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
					return
				end
				return 
			end
			local sceneType = scene.m_SceneAttr.SceneType
			if sceneType == 26 or sceneType == 27 or sceneType == 7 then
				return
			else
				Player:CppSetPKState(false)
				Gas2Gac:TransferSwitchState(Player.m_Conn,Player.m_SwitchState)
				Gas2Gac:UpdatePkSwitchState(Player.m_Conn)
				Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
			end
		end
	end
end

local function UseMatchGameItem(Conn, ItemName, RoomIndex, Pos)
	local Player = Conn.m_Player
	local CanUse = MatchGameItem_Common(ItemName,"CanUse")
	local BuffSkillName = MatchGameItem_Common(ItemName, "BuffName")
	
	local Arg = GetCfgTransformValue(false, "MatchGameItem_Common", ItemName, "Arg")
	--local Arg = MatchGameItem_Common(ItemName, "Arg")
	if CanUse ~= 1 then  --是否可以右键使用
		return 
	end
	
	if Player:CppGetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_InDoingSkill) then
		MsgToConn(Conn, 838)
		return
	end
	local function CallBack(res)
		if not res then
			return 
		end
		if BuffSkillName ~= nil and BuffSkillName ~= "" then
			Player:PlayerDoItemSkill(BuffSkillName, 1)
		end
		local len = table.getn(Arg)
		if len ~= 0 then
			local type = Arg[1]
			local value = Arg[2]
			DoMethod(Player, type, value)
		end
		CanPk(Player, BuffSkillName)
	end
	local data = {}
	data["PlayerID"]	= Player.m_uID
	data["ItemName"]	= ItemName
	data["RoomIndex"] = RoomIndex
	data["Pos"]				= Pos
	data["sceneName"] = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, "GatherLiveSkillDB", "DelMatchGameItem", CallBack, data)
end

return UseMatchGameItem
