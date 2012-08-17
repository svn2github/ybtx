-- yy 09 05 14 

local ChannelID = ChannelIdTbl["NPC"]

--NPC只能用以下几个频道说话
local ChannelIdTbl = {
	["系统"] = 1, --"系统"
	["世界"] = 2, --"世界"
	["地图"] = 4, --"地图"
	["附近"] = 5, --"附近"
}

local ChannelConnTbl = {
	[1] = function () return g_AllPlayerSender end, --"系统"
	[2] = function () return g_AllPlayerSender end, --"世界"
	[4] = function (Npc) return Npc.m_Scene.m_CoreScene end, --"地图"
	[5] = function (Npc) return Npc:GetIS(0) end, --"附近"
	[9] = function (Npc) return Npc:GetIS(0) end, --"NPC"
}

--得到设置的概率
local function RandomIsShow(ContentCfg)
	local TriggerProbability = 0
	local Arg = ContentCfg.Arg
	local rate = Arg
	if IsTable(Arg) then
		rate = Arg[2]
	end
	rate = tonumber(rate)
	if not IsNumber(rate) or rate == 1 then
		return true
	end
	return (math.random(1,100)) <= (rate*100)
end

local function TalkingInBattling(Tick, Npc,ContentCfg,NpcEntityID, NpcName)
	if RandomIsShow(ContentCfg) then
		if IsCppBound(Npc) then
			local ContentTbl = ContentCfg["ContentTbl"]
			local Info = math.random(1,table.maxn(ContentTbl))
			if not ContentCfg.Channel or ContentCfg.Channel == "" then
				return
			end
			for _, ChannelTbl in pairs(ContentCfg.Channel) do
				for _, Channel in pairs(ChannelTbl) do
					local SendId = ChannelIdTbl[Channel] or ChannelID
					Gas2Gac:RetShowTextNpc( ChannelConnTbl[SendId](Npc), NpcEntityID, NpcName, SendId, Info, 5)
				end
			end
		end
	end
end

--NPC  出生时显示对话
local function BornShow( Npc, NpcName, ContentCfg, NpcEntityID, SkillName)
	if RandomIsShow(ContentCfg) then
		local function tickfun()
			local ContentTbl = ContentCfg["ContentTbl"]
			local Info = math.random(1,table.maxn(ContentTbl))
			if not ContentCfg.Channel or ContentCfg.Channel == "" then
				return
			end
			for _, ChannelTbl in pairs(ContentCfg.Channel) do
				for _, Channel in pairs(ChannelTbl) do
					local SendId = ChannelIdTbl[Channel] or ChannelID
					Gas2Gac:RetShowTextNpc( ChannelConnTbl[SendId](Npc), NpcEntityID, NpcName, SendId, Info, 1)
				end
			end
		end
		RegisterOnceTick(Npc, "BornShowContentTick", tickfun, 300)
	end
end 

--NPC  死亡时显示对话
local function DeadShow( Npc, NpcName, ContentCfg, NpcEntityID, SkillName)
	if RandomIsShow(ContentCfg) then
		local ContentTbl = ContentCfg["ContentTbl"]
		local Info = math.random(1,table.maxn(ContentTbl))
		if not ContentCfg.Channel or ContentCfg.Channel == "" then
			return
		end
		for _, ChannelTbl in pairs(ContentCfg.Channel) do
			for _, Channel in pairs(ChannelTbl) do
				local SendId = ChannelIdTbl[Channel] or ChannelID
				Gas2Gac:RetShowTextNpc( ChannelConnTbl[SendId](Npc), NpcEntityID, NpcName, SendId, Info, 2)
			end
		end
	end
end
 
--NPC  脱离战斗显示对话
local function LeaveBattleState( Npc, NpcName, ContentCfg, NpcEntityID, SkillName)
	if RandomIsShow(ContentCfg) then
		local ContentTbl = ContentCfg["ContentTbl"]
		local Info = math.random(1,table.maxn(ContentTbl))
		if not ContentCfg.Channel or ContentCfg.Channel == "" then
			return
		end
		for _, ChannelTbl in pairs(ContentCfg.Channel) do
			for _, Channel in pairs(ChannelTbl) do
				local SendId = ChannelIdTbl[Channel] or ChannelID
				Gas2Gac:RetShowTextNpc( ChannelConnTbl[SendId](Npc), NpcEntityID, NpcName, SendId, Info, 4)
			end
		end
	end
end

--NPC　释放技能 显示对话
local function DoSkill( Npc, NpcName, ContentCfg, NpcEntityID, SkillName, TargetID)
	if SkillName == nil or SkillName == "" then
		return
	end
	NpcShowContent( "技能警告提示", Npc, SkillName, TargetID)
	if ContentCfg[SkillName] then
		local ContentTbl = ContentCfg[SkillName]["ContentTbl"]
		local Info = math.random(1,table.maxn(ContentTbl))
		if ContentCfg[SkillName].Channel and ContentCfg[SkillName].Channel ~= "" then
			for _, ChannelTbl in pairs(ContentCfg[SkillName].Channel) do
				for _, Channel in pairs(ChannelTbl) do
					local SendId = ChannelIdTbl[Channel] or ChannelID
					Gas2Gac:NpcDoSkillTalk( ChannelConnTbl[SendId](Npc), NpcEntityID, NpcName, SendId, Info, SkillName)
				end
			end
		end
	end
	return
end

local function CheckReplaceStr(Content, Replace, TargetID)
	if not TargetID then
		return
	end
	local Char = CCharacterDictator_GetCharacterByID(TargetID)
	if not IsCppBound(Char) then
		return
	end
	if not IsPlayer(Char) then
		return
	end
	if Replace == "#name#" then
		return Char.m_Properties:GetCharName()
	end
end

--NPC　技能警告提示
local function DoSkillAdvise( Npc, NpcName, ContentCfg, NpcEntityID, SkillName, TargetID)
	
	if SkillName == nil or SkillName == "" then
		return
	end
	if ContentCfg[SkillName] then
		local ContentTbl = ContentCfg[SkillName]["ContentTbl"]
		local index = math.random(1,table.maxn(ContentTbl))
		local Content = ContentTbl[index][1]
		local replaceStr = ContentCfg[SkillName]["Replace"][index]
		if not ContentCfg[SkillName].Channel or ContentCfg[SkillName].Channel == "" then
			return
		end
		if replaceStr then
			local Str = CheckReplaceStr(Content, replaceStr, TargetID)
			if Str then
				for _, ChannelTbl in pairs(ContentCfg[SkillName].Channel) do
					for _, Channel in pairs(ChannelTbl) do
						local SendId = ChannelIdTbl[Channel] or ChannelID
						Gas2Gac:NpcDoSkillAdviseRepl( ChannelConnTbl[SendId](Npc), NpcName, index, SkillName, replaceStr, Str)
					end
				end
			end
		else
			for _, ChannelTbl in pairs(ContentCfg[SkillName].Channel) do
				for _, Channel in pairs(ChannelTbl) do
					local SendId = ChannelIdTbl[Channel] or ChannelID
					Gas2Gac:NpcDoSkillAdvise( ChannelConnTbl[SendId](Npc), NpcName, index, SkillName)
				end
			end
		end
	end
	return
end

--NPC  战斗中说话TICK
local function RegisterTalkingInBattling( Npc, NpcName, NpcEntityID, NpcOperation)
	local BattlingCfg = g_NpcShowContentCfg[NpcName] and g_NpcShowContentCfg[NpcName]["战斗中"]
	if not BattlingCfg then
		return
	end
	
	if Npc.BattlingTick then
		UnRegisterTick(Npc.BattlingTick )
		Npc.BattlingTick  = nil
		if NpcOperation == "脱离战斗" or NpcOperation == "死亡" then
			return
		end
	else
		if NpcOperation ~= "进入战斗" then
			return
		end
	end
	
	local Arg = BattlingCfg.Arg
	local TickTime = 5000
	if IsNumber(Arg[1]) then
		TickTime = Arg[1]
	end
	Npc.BattlingTick = RegisterTick("BattlingTick",	TalkingInBattling, TickTime, Npc, BattlingCfg,NpcEntityID, NpcName)	
end

--NPC  进入战斗显示对话
local function EntryBattleState( Npc, NpcName, ContentCfg, NpcEntityID, SkillName)
	if not IsCppBound(Npc) or not Npc:CppIsLive() then
		return
	end
	if RandomIsShow(ContentCfg) then
		local ContentTbl = ContentCfg["ContentTbl"]
		local info = math.random(1,table.maxn(ContentTbl))
		if not ContentCfg.Channel or ContentCfg.Channel == "" then
			return
		end
		for _, ChannelTbl in pairs(ContentCfg.Channel) do
			for _, Channel in pairs(ChannelTbl) do
				local SendId = ChannelIdTbl[Channel] or ChannelID
				Gas2Gac:RetShowTextNpc( ChannelConnTbl[SendId](Npc), NpcEntityID, NpcName, SendId, info, 3)
			end
		end
	end
end

local g_Operation =
{
	["出生"] = BornShow,
	["死亡"] = DeadShow,
	["进入战斗"] = EntryBattleState,
	["脱离战斗"] = LeaveBattleState,
	["释放技能"] = DoSkill,
	["技能警告提示"] = DoSkillAdvise
}

--进入函数口  
function NpcShowContent( NpcOperation, Npc, SkillName, TargetID)
	
	local NpcName = Npc.m_Properties:GetCharName()   -- NpcName
	
	local cfg = g_NpcShowContentCfg[NpcName] 
	local ContentCfg = cfg and cfg[NpcOperation]
	local NpcEntityID = Npc:GetEntityID()
	RegisterTalkingInBattling( Npc, NpcName, NpcEntityID, NpcOperation)
	
	if not ContentCfg then
		return
	end
	if ContentCfg.Arg == 10 then
		local sMsg = ContentCfg.ShowContext
		if sMsg ~= nil and sMsg ~= "" then
			ShowContentToAllPlayer(sMsg)
		end
		return
	end
	
	g_Operation[NpcOperation]( Npc, NpcName, ContentCfg, NpcEntityID, SkillName, TargetID)
end

function ShowContentToAllPlayer(sMsg)
	CChatChannelMgr.SendMsg(sMsg, 0, 1)
	Gas2Gac:SysMovementNotifyToClient(g_AllPlayerSender, sMsg)
end

