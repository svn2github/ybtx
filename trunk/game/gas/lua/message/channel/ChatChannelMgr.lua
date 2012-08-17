
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
gas_require "message/channel/ChatCommand"

CChatChannelMgr = class()

local ChannelDB = "ChannelDB"

--[[
Character_Id 角色ID
uChannel_Id  频道ID
sMsg 信息内容
Npc_EntityID 为 NPc_EntityID
--]]

local ChannelNameTbl = {}
ChannelNameTbl[2] = "世界"
ChannelNameTbl[3] = "阵营"
ChannelNameTbl[4] = "地图"
ChannelNameTbl[5] = "附近"
ChannelNameTbl[6] = "队伍"
ChannelNameTbl[7] = "佣兵团"
ChannelNameTbl[8] = "团队"
ChannelNameTbl[9] = "NPC"
ChannelNameTbl[10] = "传声"


--传声
local getCHWorldPlayer = function (Character_Id, uChannel_Id, sMsg)
	local Player = g_GetPlayerInfo(Character_Id)
	if Player:CppGetLevel() < 10 then
		return
	end
	local uCamp = Player:CppGetBirthCamp()
	
	local function CallBack(succ,res)
		if succ then
			local charName = Player.m_Properties:GetCharName()
			for _, otherServerConn in pairs(g_OtherServerConnList) do
				Gas2GasCall:SendPlayerChannelMsg(otherServerConn, Character_Id, charName, uChannel_Id,sMsg, uCamp)
			end
			Gas2Gac:ChannelMessage(g_AllPlayerSender, Character_Id, charName,uChannel_Id,sMsg,uCamp)
		else
			MsgToConn(Player.m_Conn,209)
		end
	end
	local param = {	["charId"] = Character_Id,["sMsg"] = sMsg,["sChannelName"] = ChannelNameTbl[uChannel_Id]
									,["uServerId"] = g_CurServerId,["uChannel_Id"] = uChannel_Id}
	CallAccountManualTrans(Player.m_Conn.m_Account, ChannelDB, "SaveChannelChatInfo", CallBack, param)
end


--附近  
local getRegionPlayer = function (Character_Id, uChannel_Id, sMsg,Npc_EntityID)
	local Player = g_GetPlayerInfo(Character_Id)
	if Npc_EntityID ~= nil then
		local NpcObj = CCharacterDictator_GetCharacterByID(Npc_EntityID)
		if Character_Id == 0 then 
			Gas2Gac:SendNpcRpcMessage(NpcObj:GetIS(0), Npc_EntityID, uChannel_Id,sMsg)
		elseif Player  then
			Gas2Gac:SendNpcRpcMessage(Player:GetIS(0), Npc_EntityID, uChannel_Id,sMsg)	
			if Player:CppIsLive() then
				Gas2Gac:SendNpcRpcMessage(g_GetPlayerInfo(Character_Id).m_Conn, Npc_EntityID, uChannel_Id,sMsg)		
			end
		end
	elseif Player ~= nil then			
		Gas2Gac:ChannelMessage(Player:GetIS(0), Character_Id, g_GetPlayerInfo(Character_Id).m_Properties:GetCharName(),uChannel_Id,sMsg,0)		
		if Player:CppIsLive() then
			Gas2Gac:ChannelMessage(g_GetPlayerInfo(Character_Id).m_Conn, Character_Id, g_GetPlayerInfo(Character_Id).m_Properties:GetCharName(),uChannel_Id,sMsg,0)
		end
	end
end

--地图
local getMapPlayer = function (Character_Id, uChannel_Id, sMsg,Npc_EntityID)
	local Scene = nil
	if Npc_EntityID ~= nil then
		local NpcObj = CCharacterDictator_GetCharacterByID(Npc_EntityID)
		local NpcName = NpcObj.m_Properties:GetCharName()
		assert(NpcObj)
		Scene = NpcObj.m_Scene
		assert(Scene)
		Gas2Gac:SendNpcRpcMessage(Scene.m_CoreScene, NpcName, uChannel_Id,sMsg)
	else
		local player = g_GetPlayerInfo(Character_Id)
		if player and player:CppGetLevel() < 10 then
			return
		end
		Scene = g_GetPlayerInfo(Character_Id).m_Scene
		assert(Scene)
		Gas2Gac:ChannelMessage(Scene.m_CoreScene,Character_Id, 
			g_GetPlayerInfo(Character_Id).m_Properties:GetCharName(),uChannel_Id,sMsg,0)
			
		local param = {	["charId"] = Character_Id,["sMsg"] = sMsg,["sChannelName"] = ChannelNameTbl[uChannel_Id],["uServerId"] = g_CurServerId}
		CallAccountManualTrans(player.m_Conn.m_Account, ChannelDB, "SaveChannelChatInfo", nil, param)
	end
end

--世界
local getWorldPlayer = function (Character_Id, uChannel_Id, sMsg,Npc_EntityID)
	local player = g_GetPlayerInfo(Character_Id)
	if player:CppGetLevel() < 10 then
		return
	end
	local uCamp = player:CppGetBirthCamp()
	if Npc_EntityID ~= nil then
		local NpcObj = CCharacterDictator_GetCharacterByID(Npc_EntityID)
		local NpcName = NpcObj.m_Properties:GetCharName()
		if NpcObj == nil then
			return
		end
		for _, otherServerConn in pairs(g_OtherServerConnList) do
			Gas2GasCall:SendNpcChannelMsg(otherServerConn, NpcName,uChannel_Id,sMsg, 0)
		end
		Gas2Gac:SendNpcRpcMessage(g_AllPlayerSender,NpcName,uChannel_Id,sMsg)
	else
		local function CallBack(succ,msgId)
			if succ then
				local Player = g_GetPlayerInfo(Character_Id)
				local charName = Player.m_Properties:GetCharName()
				for _, otherServerConn in pairs(g_OtherServerConnList) do
					Gas2GasCall:SendPlayerChannelMsg(otherServerConn, Character_Id, charName, uChannel_Id,sMsg, uCamp)
				end
				Gas2Gac:ChannelMessage(g_AllPlayerSender, Character_Id, charName,uChannel_Id,sMsg,uCamp)
			else
				if msgId then
					MsgToConn(player.m_Conn,msgId)
				end
			end
		end
		local param = {	["charId"] = Character_Id,["sMsg"] = sMsg,["sChannelName"] = ChannelNameTbl[uChannel_Id]
		,["uServerId"] = g_CurServerId,["uChannel_Id"] = uChannel_Id}
		CallAccountManualTrans(player.m_Conn.m_Account, ChannelDB, "SaveChannelChatInfo", CallBack, param)
	end
end

--小队
local getTeamPlayer = function (Character_Id, uChannel_Id, sMsg, Npc_EntityID, IsSysFlag)
	local player = g_GetPlayerInfo(Character_Id)
	assert(player)
	local teamid = player.m_Properties:GetTeamID()
	local teamMembers = g_TeamMgr:GetMembers(teamid)
	local charName = player.m_Properties:GetCharName()
	
	if Npc_EntityID ~= nil then 
		local NpcObj = CCharacterDictator_GetCharacterByID(Npc_EntityID)
		local NpcName = NpcObj.m_Properties:GetCharName()
		for _, id in ipairs(teamMembers) do
			Gas2GacById:SendNpcRpcMessage(id,NpcName,uChannel_Id,sMsg)
		end
	else
		if not IsSysFlag then
			for _, id in ipairs(teamMembers) do
				Gas2GacById:ChannelMessage(id, Character_Id, charName,uChannel_Id,sMsg,0)
			end
		else
			for _, id in ipairs(teamMembers) do
				Gas2GacById:ChannelMessage(id, Character_Id, "系统",uChannel_Id,sMsg,0)
			end
		end
	end
end


--佣兵团
function CChatChannelMgr.SendArmyChannelMsg(Conn, NpcName,Character_Id, charName,ArmyCorpsID,uChannel_Id,sMsg)
	if NpcName ~= "" then
		Gas2Gac:SendNpcRpcMessage(g_GasArmyCorpsMgr:GetArmyCorpsMsgSender(ArmyCorpsID) ,NpcName, uChannel_Id,sMsg)
	else
		Gas2Gac:ChannelMessage(g_GasArmyCorpsMgr:GetArmyCorpsMsgSender(ArmyCorpsID), Character_Id, charName,uChannel_Id,sMsg,0)
	end
end

local getArmyPlayer = function (Character_Id, uChannel_Id, sMsg,Npc_EntityID)
	local Player = g_GetPlayerInfo(Character_Id)
	local ArmyCorpsID 
	if Player ~= nil then
		ArmyCorpsID = Player.m_uArmyCorpsID
		if ArmyCorpsID == 0 then
			return
		end
	end
	local charName = Player.m_Properties:GetCharName()
	local NpcName = "" 
	if Npc_EntityID then
		local NpcObj = CCharacterDictator_GetCharacterByID(Npc_EntityID)
		NpcName = NpcObj.m_Properties:GetCharName()				
	end	
	for serverId in pairs(g_ServerList) do
		Gas2GasAutoCall:SendArmyChannelMsg(GetServerAutoConn(serverId), NpcName,Character_Id, charName,ArmyCorpsID,uChannel_Id,sMsg)
	end
end


--佣兵小队
function CChatChannelMgr.SendTongChannelMsg(Conn, NpcName,Character_Id, charName,uTongID,uChannel_Id,sMsg)
	if NpcName ~= "" then
		Gas2Gac:SendNpcRpcMessage(g_GasTongMgr:GetTongMsgSender(uTongID) ,NpcName, uChannel_Id,sMsg)
	else
		Gas2Gac:ChannelMessage(g_GasTongMgr:GetTongMsgSender(uTongID), Character_Id, charName,uChannel_Id,sMsg,0)
	end
end

local getPartyPlayer = function (Character_Id, uChannel_Id, sMsg,Npc_EntityID)
	local Player = g_GetPlayerInfo(Character_Id)
	local uTongID
	if Player ~= nil then
		uTongID = Player.m_Properties:GetTongID()
		if uTongID == 0 then
			--还没有加入帮会
			MsgToConn(Player.m_Conn, 205)
			return
		end
	end
	local charName = Player.m_Properties:GetCharName()
	local NpcName = "" 
	if Npc_EntityID then
		local NpcObj = CCharacterDictator_GetCharacterByID(Npc_EntityID)
		NpcName = NpcObj.m_Properties:GetCharName()				
	end	
	for serverId in pairs(g_ServerList) do
		Gas2GasAutoCall:SendTongChannelMsg(GetServerAutoConn(serverId), NpcName,Character_Id, charName,uTongID,uChannel_Id,sMsg)
	end
end

--阵营
local getCampPlayer = function (Character_Id, uChannel_Id, sMsg,Npc_EntityID)
	local player = g_GetPlayerInfo(Character_Id)
	if player:CppGetLevel() < 10 then
		return
	end
	local uCamp = player:CppGetBirthCamp()
	local charName = player.m_Properties:GetCharName()
	local NpcName = nil
	if Npc_EntityID ~= nil then
		local NpcObj = CCharacterDictator_GetCharacterByID(Npc_EntityID)
		NpcName = NpcObj.m_Properties:GetCharName()		
	end
	if NpcName then
		for _, otherServerConn in pairs(g_OtherServerConnList) do
			Gas2GasCall:SendNpcChannelMsg(otherServerConn, NpcName, uChannel_Id,sMsg, uCamp)
		end
		Gas2Gac:SendNpcRpcMessage(CampPlayerSenderTbl[uCamp],NpcName,uChannel_Id,sMsg)
	else
		for _, otherServerConn in pairs(g_OtherServerConnList) do
			Gas2GasCall:SendPlayerChannelMsg(otherServerConn, Character_Id, charName, uChannel_Id,sMsg, uCamp)
		end
		Gas2Gac:ChannelMessage(CampPlayerSenderTbl[uCamp], Character_Id, charName,uChannel_Id,sMsg,0)		
		
		local param = {	["charId"] = Character_Id,["sMsg"] = sMsg,["sChannelName"] = ChannelNameTbl[uChannel_Id],["uServerId"] = g_CurServerId}
		CallAccountManualTrans(player.m_Conn.m_Account, ChannelDB, "SaveChannelChatInfo", nil, param)
	end	
end

local getGM = function (Character_Id, uChannel_Id, sMsg)
	for _, otherServerConn in pairs(g_OtherServerConnList) do
		Gas2GasCall:SendPlayerChannelMsg(otherServerConn, 0, "", uChannel_Id,sMsg, 0)
	end
	Gas2Gac:ChannelMessage(g_AllPlayerSender, 0, "",uChannel_Id,sMsg,0)
end

local ChannelMsgHandler = {} --这里面将真正发送消息给客户端
ChannelMsgHandler[1] = getGM							--系统	 ToDo 
ChannelMsgHandler[2] = getWorldPlayer			--世界
ChannelMsgHandler[3] = getCampPlayer			--阵营
ChannelMsgHandler[4] = getMapPlayer				--地图
ChannelMsgHandler[5] = getRegionPlayer		--附近
ChannelMsgHandler[6] = getTeamPlayer			--队伍
ChannelMsgHandler[7] = getPartyPlayer			--佣兵小队
ChannelMsgHandler[8] = getArmyPlayer			--佣兵团
ChannelMsgHandler[10] = getCHWorldPlayer	--传声

function CChatChannelMgr.Init( uPlayer_id,CChannelInitRes )
	local player = g_GetPlayerInfo(uPlayer_id)
	player.m_tblForbidChannel = {}
	local num = CChannelInitRes:GetRowNum()
	for i = 1, num do
		player.m_tblForbidChannel[CChannelInitRes(i,1)] = true
	end
end

function g_cmdPostBulletin(arg1, arg2, arg3)
	local sMsg = arg3
	CChatChannelMgr.SendMsg(sMsg, 0, 1)
end

local function CheckAvaliable(uPlayer_id, uChannel_Id)
	local player = g_GetPlayerInfo(uPlayer_id)
	if not player then
		return true
	end
	local tbl_forbid_channel = player.m_tblForbidChannel
	--0是禁言所有频道
	if tbl_forbid_channel[uChannel_Id] or tbl_forbid_channel[0] then
		return false
	end
	return true
end

function CChatChannelMgr.SendMsg(sMsg, uPlayer_Id, uChannel_Id , ... )
	local arg = {...}
	local channel_index = uChannel_Id
	if (channel_index == nil) then
		return
	end
	
	if not CheckAvaliable(uPlayer_Id, uChannel_Id) then
		MsgToConn(g_GetPlayerInfo(uPlayer_Id).m_Conn,11)
		return
	end
	local tp1 = unpack(arg)
	
	if (ChannelMsgHandler[channel_index] ~= nil) then
		ChannelMsgHandler[channel_index](uPlayer_Id, channel_index, sMsg, tp1, false)
	end

end
-- 系统发送消息
function CChatChannelMgr.SysSendMsg(sMsg, uPlayer_Id, uChannel_Id, ...)
	local arg = {...}
	local channel_index = uChannel_Id
	if (channel_index == nil) then
		return
	end
	
	if not CheckAvaliable(uPlayer_Id, uChannel_Id) then
		MsgToConn(g_GetPlayerInfo(uPlayer_Id).m_Conn,11)
		return
	end

	if (ChannelMsgHandler[channel_index] ~= nil) then
		ChannelMsgHandler[channel_index](uPlayer_Id, channel_index, sMsg, nil, true)
	end
	
end
-----------------------------------RPC Function ---------------------------------------------

function TalkToChannl(data)
	local Conn, Msg, Channel_id = unpack(data)
	Msg = string.gsub(Msg, "#","") 
	local player = Conn.m_Account
	if Channel_id == 1 then
		return
	end
	CChatChannelMgr.SendMsg(Msg, Conn.m_Player.m_uID, Channel_id)
end

function CChatChannelMgr.TalkToChannel(Conn, Msg, Channel_id)
	if Conn.m_Player == nil then
		return
	end
	if IsFunction(ChatCommandList[Msg]) then
		if ChatCommandList[Msg](Conn.m_Player) then
			return
		end
	end
	
	if  Conn.m_Player.m_TalkTimeTbl and Conn.m_Player.m_TalkTimeTbl[Channel_id] then
		local Time = GetProcessTime()/1000 - Conn.m_Player.m_TalkTimeTbl[Channel_id] 
		local TimeLimit = ChannelTimeLimitTbl[Channel_id] and ChannelTimeLimitTbl[Channel_id] or 0
		if Time < TimeLimit then
			return
		end
	else
		if not Conn.m_Player.m_TalkTimeTbl then
			Conn.m_Player.m_TalkTimeTbl = {}
		end
	end
 	local data = {Conn, Msg, Channel_id}
 	TalkToChannl(data)
 	Conn.m_Player.m_TalkTimeTbl[Channel_id]  = GetProcessTime()/1000
end

function SystemSendMsg( Conn, Msg, Channel_id)
	
	
end

----------------------------------------------聊天面板模块-------------------------------------
local PanelAction = 
	{
		SUBSCRIBE = 1,
		UN_SUBSCRIBE = 0
	}

--聊天页移动
function CChatChannelMgr.MovePosB2PosA( Conn, nPosA, nPosB)
	--local channel_db = (g_DBTransDef["ChannelDB"])
	local paramater = {}
	paramater["nPosA"] = nPosA
	paramater["nPosB"] = nPosB
	paramater["player_id"] = Conn.m_Player.m_uID
	CallAccountManualTrans(Conn.m_Account, ChannelDB, "move_posB_2_posA", nil, paramater)
end


--获取所有面板信息
function CChatChannelMgr.GetAllPanel(player,result)
	local Conn = player.m_Conn
	for i = 1, result:GetRowNum() do
		Gas2Gac:RetGetPanelInfo(Conn,result(i,1),result(i,2),result(i,3))
	end
end

function CChatChannelMgr.GetAllChanelPanelInfo(player,result)
	local Conn = player.m_Conn
	for i = 1, result:GetRowNum() do
			Gas2Gac:RetGetChannelPanelInfo(Conn,result(i,1),result(i,2),result(i,3))
	end
end

--设置聊天面板的频道
--[[
	参数定义如下：
		Channel_ID：	频道ID
		Action：		枚举类型，订阅or取消订阅
		Panel_Position：	面板位置
--]]
--设置频道信息
function CChatChannelMgr.SetChannelPanel(Conn, Channel_ID, Action, Panel_Position)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	Player.ChannelPanel = Player.ChannelPanel or {}
	local paramater = {}
	paramater["Channel_ID"] = Channel_ID
	paramater["Action"] = Action
	paramater["Panel_Position"] = Panel_Position
	paramater["player_id"] = Conn.m_Player.m_uID
	table.insert(Player.ChannelPanel,paramater)
end
function CChatChannelMgr.SetChannelPanelEnd(Conn)
	local Player = Conn.m_Player
	local paramater1 = Player.ChannelPanel or {}
	local paramater2 = Player.SetChannelPanel
	local paramater3 = Player.AddChannelPanel
	
	local function CallBack(bFlag1,bFlag2,uMsgID)
		if IsNumber(uMsgID) then
			MsgToConn(Conn,uMsgID)
		end
		if bFlag1 then
			if bFlag2 and Player.AddChannelPanel ~= nil then
				Gas2Gac:RetGetPanelInfo(Conn, Player.AddChannelPanel["Panel_Name"], Player.AddChannelPanel["Panel_Pos"], Player.AddChannelPanel["Action"])
			end
			Player.ChannelPanel = nil
			Player.SetChannelPanel = nil
			Player.AddChannelPanel = nil
		end
	end
	CallAccountManualTrans(Conn.m_Account, ChannelDB, "SetChannelPanelInfo", CallBack, {paramater1,paramater2,paramater3})
end
--把面板设置为默认信息
function CChatChannelMgr.SetDefaultChannelPanel(Conn)
	local function CallBack(bool,result)
		if bool then
			Gas2Gac:RetSetDefaultChannelPanel(Conn, true)
		else
			Gas2Gac:RetSetDefaultChannelPanel(Conn, false)
		end
	end
	
	--local channel_db = (g_DBTransDef["ChannelDB"])
	local paramater = {}
	paramater["player_id"] = Conn.m_Player.m_uID
	CallAccountManualTrans(Conn.m_Account, ChannelDB, "SetDefault", CallBack, paramater)
end

--重命名面板
function CChatChannelMgr.RenamePanel(Conn, Panel_Name, Panel_Pos)
	local function CallBack(bool,result)
		if bool then
			Gas2Gac:RetRenamePanel(Conn, true)
		else
			Gas2Gac:RetRenamePanel(Conn, false)
		end
	end
	
	--local channel_db = (g_DBTransDef["ChannelDB"])
	local paramater = {}
	paramater["Panel_Name"] = Panel_Name
	paramater["Panel_Pos"] = Panel_Pos
	paramater["player_id"] = Conn.m_Player.m_uID
	paramater["Action"] = PanelAction.SUBSCRIBE
	CallAccountManualTrans(Conn.m_Account, ChannelDB, "SetPanelInfo", CallBack, paramater)	
end

--删除面板
function CChatChannelMgr.DeletePanel(Conn, Panel_Name, Panel_Pos)
	local function CallBack(bool,result)
		if bool then
			Gas2Gac:RetDeletePanel(Conn, true)		
		else
			Gas2Gac:RetDeletePanel(Conn, false)
		end
	end
	
	--local channel_db = (g_DBTransDef["ChannelDB"])
	local paramater = {}
	paramater["Panel_Name"] = Panel_Name
	paramater["Panel_Pos"] = Panel_Pos
	paramater["player_id"] = Conn.m_Player.m_uID
	paramater["Action"] = PanelAction.UN_SUBSCRIBE
	CallAccountManualTrans(Conn.m_Account, ChannelDB, "DeletePanelInfo", CallBack, paramater)	
end

--添加新的面板
function CChatChannelMgr.AddPanel(Conn, Panel_Name, Panel_Pos)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	local paramater = {}
	paramater["Panel_Name"] = Panel_Name
	paramater["Panel_Pos"] = Panel_Pos
	paramater["player_id"] = Conn.m_Player.m_uID
	paramater["Action"] = PanelAction.SUBSCRIBE
	Player.AddChannelPanel = paramater
end

--把面板信息写入数据库
function CChatChannelMgr.SetPanel(Conn, Panel_Name, Panel_Pos)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	local paramater = {}
	paramater["Panel_Name"] = Panel_Name
	paramater["Panel_Pos"] = Panel_Pos
	paramater["player_id"] = Conn.m_Player.m_uID
	paramater["Action"] = PanelAction.SUBSCRIBE
	Player.SetChannelPanel = paramater
end


function CChatChannelMgr.SaveNotShowMsgWnd(Conn,sFirstTimeName)
	local paramater = {}
	paramater["PlayerId"] = Conn.m_Player.m_uID
	paramater["FirstTimeName"] = sFirstTimeName
	CallAccountAutoTrans(Conn.m_Account, "NoviceDirectDB", "InsertPlayerFirstFinishInfo", nil, paramater)	
end
-------------------------------------多服广播消息--------------------------

function CChatChannelMgr.SendPlayerChannelMsg(Conn, Character_Id, charName, uChannel_Id,sMsg, uProcess)
	if uChannel_Id == 1 or uChannel_Id == 2 or uChannel_Id == 10 then
		Gas2Gac:ChannelMessage(g_AllPlayerSender, Character_Id, charName, uChannel_Id,sMsg,uProcess or 0)
		return
	elseif uChannel_Id == 3 then
		Gas2Gac:ChannelMessage(CampPlayerSenderTbl[uProcess], Character_Id, charName, uChannel_Id,sMsg,0)
		return
	end
	assert(true, "有频道添加没有转发处理")
end

function CChatChannelMgr.SendNpcChannelMsg(Conn, NpcName, uChannel_Id, sMsg, uProcess)
	if uChannel_Id == 1 then
		Gas2Gac:SendNpcRpcMessage(g_AllPlayerSender,NpcName,uChannel_Id,sMsg)
		return
	elseif uChannel_Id == 3 then
		Gas2Gac:SendNpcRpcMessage(CampPlayerSenderTbl[uProcess],NpcName,uChannel_Id,sMsg)
		return
	end
	assert(true, "有频道添加没有转发处理")
end
