local StmtContainer = class()
local g_MoneyMgr = g_MoneyMgr
local event_type_tbl = event_type_tbl
local LogDBName = DeployConfig.MySqlDatabase .. "_log"

local ChannelDB = CreateDbBox(...)
------------------------------------------------------------
local StmtDef = 
{
	"CancelForbidChannel",
	"delete from tbl_forbid_talking where cs_uID = ? and ft_uChannelId = ?"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = 
{
	"_ForbidChannel",
	"insert ignore into tbl_forbid_talking(cs_uID, ft_uChannelId) values (?, ?)"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = 
{
	"SelectAllForbidChannel",
	"select distinct ft_uChannelId from tbl_forbid_talking where cs_uId = ?"
}
DefineSql( StmtDef, StmtContainer )

local chat_cost_tbl = 
										{
										[2] = - 2000,
										[10] = - 50000
										}

function ChannelDB.CancelForbid(data)
	local uID = data["char_id"]
	local uChannel_id = data["channel_id"]
	StmtContainer.CancelForbidChannel:ExecSql("", uID, uChannel_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function ChannelDB.ForbidChannel(data)
	local uID = data["char_id"]
	local uChannel_id = data["channel_id"]
	StmtContainer._ForbidChannel:ExecSql("", uID, uChannel_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function ChannelDB.SelectAllForbidChannel(uID)
	--local uID = data["char_id"]
	local Ret =  StmtContainer.SelectAllForbidChannel:ExecStat(uID)
	return Ret
end

--------------------------公共聊天频道-----------------------------
--小队频道
function ChannelDB.GetTeamPlayer(data)
	local Character_Id = data["Character_Id"]
	local tbl_team_member = {}
	local team = RequireDbBox("GasTeamDB")
	local tblMembers
	if(team ~= nil) then
		local TeamID = team.GetTeamID(Character_Id)
		if TeamID == 0 then
			return false,109
		end
		tblMembers = team.GetTeamMembers(TeamID)
	end
	return true,tblMembers
end

--帮会频道
function ChannelDB.GetPartyOnlinePlayer(data)
	local uTongID = data["uTongID"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tblMembers = tong_box.GetTongOnlineCharId(uTongID)                                
	return true, tblMembers
end
--------------------------------聊天面板-----------------------------------
--频道枚举
local ChannelPanelAction = 
	{
		SUBSCRIBE = 1,
		UN_SUBSCRIBE = 0
	}
--面板枚举
local PanelAction = 
	{
		SUBSCRIBE = 1,
		UN_SUBSCRIBE = 0
	}

--设置面板基本属性
local StmtDef = {
	"SetPanelInfoSql",
	[[
		replace into tbl_msgpanel (cs_uId, panel_name, panel_position, action) values (?, ?, ?, ?)
	]]
}
DefineSql( StmtDef, StmtContainer )

--查询面板信息
local StmtDef = {
	"SelectPanelInfoSql",
	[[
		select action from tbl_msgpanel where cs_uId = ? and panel_name = ? and panel_position = ?
	]]
}
DefineSql( StmtDef, StmtContainer ) 

--查询面板信息
local StmtDef = {
	"UpdatePanelInfoSql",
	[[
		update tbl_msgpanel set action = ? where cs_uId = ? and panel_position = ?
	]]
}
DefineSql( StmtDef, StmtContainer )


function ChannelDB.DeletePanelInfo(data)
	local Panel_Name = data["Panel_Name"]
 	local Panel_Pos = data["Panel_Pos"]
	local player_id = data["player_id"]
	local Action = data["Action"]
	StmtContainer.UpdatePanelInfoSql:ExecStat(Action, player_id, Panel_Pos)
	if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
		return false
	end
	return true
end

--设置面板
function ChannelDB.SetPanelInfo(data)
	local Panel_Name = data["Panel_Name"]
 	local Panel_Pos = data["Panel_Pos"]
	local player_id = data["player_id"]
	local Action = data["Action"]
	if (string.len(Panel_Name) > 12) then
		return false,201
	end
	if (Action ~= PanelAction.SUBSCRIBE and Action ~= PanelAction.UN_SUBSCRIBE) then
		return false,201
	end	
	
	local res = StmtContainer.SelectPanelInfoSql:ExecStat(player_id, Panel_Name, Panel_Pos)
	if res:GetRowNum() == 0 then
		StmtContainer.SetPanelInfoSql:ExecSql("", player_id, Panel_Name, Panel_Pos, Action)
		if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
			return false
		end
	end
	return true
end

--设置默认信息
local StmtDef = {
	"SetDefaultPanelInfoSql",
	[[
		delete from tbl_msgpanel where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

--设置默认面板信息
function ChannelDB.SetDefault(data)
	local player_id = data["player_id"]
	StmtContainer.SetDefaultPanelInfoSql:ExecSql("", player_id)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return true
	end
	return false
end

--获取所有面板信息
local StmtDef = {
	"GetAllPanelSql",
	[[
		select panel_name, panel_position, action from tbl_msgpanel where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

function ChannelDB.GetAllPanel(data)
	local char_id	= data["char_id"]
	local query_result = StmtContainer.GetAllPanelSql:ExecSql("s[12]nn",char_id)
	return query_result
end

--聊天页移动
local StmtDef = {
	"DeletePosA",
	[[
		delete from tbl_msgpanel where panel_position = ? and cs_uId = ?  
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"MovePosBToPosASql",
	[[
		update tbl_msgpanel set panel_position = ?, action = 1 where cs_uId = ? and panel_position = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
"DeletePosB",
	[[
		replace into tbl_msgpanel (cs_uId, panel_name, panel_position, action) values (?, "", ?, 0)
	]]
}
DefineSql( StmtDef, StmtContainer )

--面板赋值从B位置到A位置
--参数为A位置，B位置
function ChannelDB.SetPosB2PosA(player_id,nPosA, nPosB)
	StmtContainer.DeletePosA:ExecSql("", nPosA, player_id)
	StmtContainer.MovePosBToPosASql:ExecSql("", nPosA, player_id, nPosB )
	StmtContainer.DeletePosB:ExecSql("", player_id, nPosB)
	if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
		CancelTran()
	end
end

function ChannelDB.move_posB_2_posA(data)
	local pos_a = data["nPosA"]
	local pos_b = data["nPosB"]
	local player_id = data["player_id"]
	ChannelDB.SetPosB2PosA(player_id,pos_a, pos_b)
end

--设置聊天面板的频道
local StmtDef = {
	"GetChannelPanelSql",
	[[
		select count(*) from tbl_msgpanel where  cs_uId = ? and  panel_position = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"SetChannelPanelSql",
	[[
		replace into tbl_msgchannel (cs_uId, panel_position, mc_uChannelId, mc_sAction )values (?, ?, ?, ?)
	]]
}
DefineSql( StmtDef, StmtContainer )

--[[
	参数定义如下：
		Channel_Name：	频道名称
		Action：		枚举类型，订阅or取消订阅
		Panel_Position：	面板位置
--]]
function ChannelDB.SetChannelPanelInfo(paramater)
	local bFlag1,bFlag2 = false,true
	if	paramater[2] ~= nil then
		local bFlag,uMsgID = ChannelDB.SetPanelInfo(paramater[2])
		if not bFlag then
			return false,false,uMsgID
		end
	end
	if	paramater[3] ~= nil then
		bFlag1 = true
		local bFlag,uMsgID = ChannelDB.SetPanelInfo(paramater[3])
		if not bFlag then
			return bFlag1,false,uMsgID
		end
	end
	for i = 1,#paramater[1] do
		local data = paramater[1][i]
		local Channel_ID = data["Channel_ID"]
		local Action = data["Action"]
		local Panel_Position = data["Panel_Position"]
		local player_id = data["player_id"]
	
		if (Action == ChannelPanelAction.SUBSCRIBE	or Action == ChannelPanelAction.UN_SUBSCRIBE) then
			local tblPanels = StmtContainer.GetChannelPanelSql:ExecSql('n',player_id, Panel_Position)
			if tblPanels:GetData(0,0) > 0 then
				StmtContainer.SetChannelPanelSql:ExecSql("", player_id, Panel_Position, Channel_ID, Action)
			end
		end
	end
	return bFlag1,bFlag2
end

--获取数据库里面存放的所有关于聊天面板频道信息
local StmtDef = {
	"GetAllChanelPanelSql",
	[[
		select mc_uChannelId, mc_sAction, panel_position 
		from tbl_msgchannel  
		where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

function ChannelDB.GetAllChanelPanelInfo(data)
	local char_id	= data["char_id"]
	local query_result = StmtContainer.GetAllChanelPanelSql:ExecSql("nnn", char_id)
	return query_result
end

------------------------------------------------------
local StmtDef=
{
	"_SaveChannelChatInfo",

	"insert into " .. LogDBName .. ".tbl_log_channel_chat values(?,?,now(),?)"

}
DefineSql(StmtDef,StmtContainer)

--@brief 记录公聊信息
--@param charId：角色id
--@param sMsg：聊天内容
--@param sChannelName:聊天频道
function ChannelDB.SaveChannelChatInfo(data)
	local charId = data["charId"]
	local sMsg = data["sMsg"]
	local sChannelName = data["sChannelName"]
	local uChannel_Id = data["uChannel_Id"]
	local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("Chat")
	
	if uChannel_Id and uChannel_Id == 2 then --世界频道聊天扣钱
		local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["WorldChat"],charId, 1, chat_cost_tbl[uChannel_Id],nil,event_type_tbl["世界频道聊天扣钱"])
		if not bFlag then
			return bFlag,uMsgID
		else
			StmtContainer._SaveChannelChatInfo:ExecStat(charId,sMsg,sChannelName)
			return true,chat_cost_tbl[uChannel_Id]
		end
	elseif uChannel_Id and uChannel_Id == 10 then  --传声频道说话消耗传声石
		local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
	  	--检测帐户是否锁定
	  	if ItemBagLockDB.HaveItemBagLock(charId) then
	  		return false,160029
	  	end
		local g_RoomMgr		= RequireDbBox("GasRoomMgrDB")	
		local res = g_RoomMgr.DelItem(charId, 1, "传声石", 1, nil,event_type_tbl["物品使用"])
		if IsNumber(res) then
			local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["ChuanSheng"],charId, 1, chat_cost_tbl[uChannel_Id],nil,event_type_tbl["传声频道发消息扣钱"])
			if not bFlag then
				return bFlag,uMsgID
			else
				StmtContainer._SaveChannelChatInfo:ExecStat(charId,sMsg,sChannelName)
				return true,chat_cost_tbl[uChannel_Id]
			end
		end
		StmtContainer._SaveChannelChatInfo:ExecStat(charId,sMsg,sChannelName)
		return true,res
	else
		StmtContainer._SaveChannelChatInfo:ExecStat(charId,sMsg,sChannelName)
		return g_DbChannelMgr:LastAffectedRowNum() == 0
	end
end

---------------------------------------------------------------

SetDbLocalFuncType(ChannelDB.SaveChannelChatInfo)
SetDbLocalFuncType(ChannelDB.DeletePanelInfo) 
SetDbLocalFuncType(ChannelDB.GetPartyOnlinePlayer) 
SetDbLocalFuncType(ChannelDB.move_posB_2_posA)
SetDbLocalFuncType(ChannelDB.SetChannelPanelInfo)
SetDbLocalFuncType(ChannelDB.SetDefault)
SetDbLocalFuncType(ChannelDB.SetPanelInfo)
return ChannelDB
