gac_gas_require "item/item_info_mgr/ItemInfoMgr"
cfg_load "skill/NonFightSkill_Common"
gac_gas_require "skill/SkillCommon"

local NonFightSkill_Common = NonFightSkill_Common
local FightSkillKind = FightSkillKind
local CheckAllObjLeak = CheckAllObjLeak
local StmtContainer = class()
gac_gas_require "framework/common/CMoney"
local event_type_tbl = event_type_tbl
local g_MoneyMgr = CMoney:new()

local StmtDef=
{
	"DbSetCamp",
	[[
	update tbl_char_static set cs_uCamp = ? where cs_uId = ?;
	]]
}
DefineSql( StmtDef , StmtContainer )

local StmtDef=
{
	"DelTblShortcut",
	[[
	delete from tbl_shortcut where cs_uId = ?;
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetGmLevel",
	"select gv_uLevel from tbl_gm_validate where username = ? and us_uGameID = ?"
}
DefineSql( StmtDef, StmtContainer )


local StmtDef=
{
	"CheckClosedAction",
	"SELECT ac_sActionName FROM tbl_action_closed"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"SearchClosedAction",
	"SELECT ac_sActionName FROM tbl_action_closed WHERE ac_sActionName = ?"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"InsertClosedAction",
	"INSERT INTO tbl_action_closed (ac_sActionName) VALUES(?)"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"DeleteClosedAction",
	"DELETE FROM tbl_action_closed WHERE ac_sActionName = ?"
}
DefineSql( StmtDef, StmtContainer )


g_ItemInfoMgr = g_ItemInfoMgr or CItemInfoMgr:new()
local g_ItemInfoMgr = g_ItemInfoMgr
local GMDBExecutor = CreateDbBox(...)

function GMDBExecutor.CheckClosedAction()
	local res = StmtContainer.CheckClosedAction:ExecSql("")
	local rowNum = res:GetRowNum()
	if rowNum == 0 then
		res:Release()
		return {}
	else 
	  local tbl = {}
		for i=0,rowNum-1 do
			tbl[i+1] = res:GetData(i,0)
		end
		res:Release()
	  return tbl
	end	
end

function GMDBExecutor.SearchClosedAction(data)
	local actionName = data["actionName"]
	local res = StmtContainer.SearchClosedAction:ExecSql("", actionName)
	if res:GetRowNum() == 0 then 
		res:Release()
		return false
	else
		res:Release()
		return true
	end
end

function GMDBExecutor.InsertClosedAction(data)
	local isFind = GMDBExecutor.SearchClosedAction(data)
	if isFind == false then
		local actionName = data["actionName"]
		StmtContainer.InsertClosedAction:ExecSql("", actionName)
	end
	return isFind
end

function GMDBExecutor.DeleteClosedAction(data)
	local isFind = GMDBExecutor.SearchClosedAction(data)
	if isFind then
		local actionName = data["actionName"]
		StmtContainer.DeleteClosedAction:ExecSql("", actionName)
	end
	return isFind
end

function GMDBExecutor.GetGmLevel(data)
	local user_name = data["user_name"]
	local game_id = data["game_id"]
	local res = StmtContainer._GetGmLevel:ExecStat(user_name,game_id)
	if 0 == res:GetRowNum() then
		return
	end
	return res:GetNumber(0,0)
end

function GMDBExecutor.SetCamp(data)
	local player_id, camp = data["player_id"], data["camp"]
	StmtContainer.DbSetCamp:ExecSql("",camp,player_id)
end

function GMDBExecutor.DeleteShortCut(data)
	local player_id = data["player_id"]
	StmtContainer.DelTblShortcut:ExecSql("",player_id)
end

function GMDBExecutor.AddBindingTicket(data)
	local ticket, player_id = data["ticket"], data["player_id"]
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("GMcmd")
	local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["AddBindingTicket"],player_id, 3, ticket,nil,event_type_tbl["GM指令加钱"])
	return bFlag
end

function GMDBExecutor.StartGmAction(data)
end

function GMDBExecutor.EndGmAction(data)
end

function GMDBExecutor.AddBindingMoney(data)
	local bindingmoney, player_id = data["bindingmoney"], data["player_id"]
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("GMcmd")
	local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["AddBindingMoney"],player_id, 2, bindingmoney,nil,event_type_tbl["GM指令加钱"])
	return bFlag
end

function GMDBExecutor.AddTicket(data)
	local Ticket, user_id = data["ticket"], data["user_id"]
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local bFlag,uMsgID = MoneyManagerDB.AddTicket(user_id,Ticket)
	return bFlag
end

function GMDBExecutor.DelItem(data)
	local Player_id,nBigID, nIndex,nCount = data['player_id'], data['item_type'], data['item_name'], data['count']
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local res = g_RoomMgr.DelItem(Player_id,nBigID,nIndex,nCount,nil,event_type_tbl["GM删除物品"])
	local sError = ""
	if IsNumber(res) then
		if res == 1 then
			sError = "删除的物品数量<= 0。"
		elseif res == 2  then
			sError = "删除类型id错误。"
		elseif res == 3 then
			sError = "没有那么多该物品可以删除。"
		end
		return {false, sError}
	end

	return {true, res}
end

function GMDBExecutor.AddSoulPearl(data)
	local player_id, item_name, index, soulRoot, soulcount,SceneName = data["player_id"], data["item_name"], data["index"], data["soultype"], data["soulcount"],data["SceneName"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local nBigId =  g_ItemInfoMgr:GetSoulPearlBigID()
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	--在静态物品表item_static中插入魂珠物品信息
			local params= {}
			params.m_nType = nBigId
			params.m_sName = soulRoot .. ":" .. soulcount
			params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(nBigId, item_name,"BindingStyle") or 0
			params.m_nCharID = player_id
			params.m_sCreateType =25
	local create_soulpearl_id = g_RoomMgr.CreateItem(params)
	if  create_soulpearl_id == false then
		local errorMsg = "表中插入魂珠信息失败！"
		return false, errorMsg
	end
	--将创建的魂珠放进背包
	local param = {}
	param.m_nCharID = player_id
	param.m_nItemType = nBigId
	param.m_sItemName = item_name
	param.m_tblItemID = {{create_soulpearl_id}}
	local bFlag, res = item_bag_box.AddCountItem(param)
	if  bFlag == false then
		CancelTran()
		return false,"添加物品失败"
	end
	return {true, res, create_soulpearl_id}
end

local JiFenKeyStrTbl = {}
JiFenKeyStrTbl[1] = "uNaturePoint"
JiFenKeyStrTbl[2] = "uEvilPoint"
JiFenKeyStrTbl[3] = "uDestroyPoint"
JiFenKeyStrTbl[4] = "uDaTaoShaPoint"
JiFenKeyStrTbl[5] = "uJiFenSaiPoint"
JiFenKeyStrTbl[6] = "uArmorPoint"

function GMDBExecutor.AddJiFen(data)
  local jiFenType = data["JiFenType"]
  local jiFenCount= data["JiFenCount"]
  local charID    = data["CharID"]
  
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local bFlag,uMsgID = AreaFbPointDB.AddAreaFbPointByType(charID,jiFenCount,jiFenType,data["SceneName"],nil,event_type_tbl["GM指令加积分"])
	if bFlag then
		return true,jiFenCount
	else
		 return false, 16
	end
end

function GMDBExecutor.AddItem(data)
	local nType, sName, nCount, index, uCharId,bindingStyle = data["itemtype"], data["itemname"], data["count"], data["index"], data["player_id"],data["bindingStyle"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local params= {}
	params.nCharID = uCharId
	params.nStoreMain = index
	params.nBigID = nType
	params.nIndex = sName
	params.nCount = nCount
	params.BindingType = g_ItemInfoMgr:GetItemInfo(nType,sName,"BindingStyle")
	params.createType = event_type_tbl["GM添加物品"]
	local res = g_RoomMgr.AddItem(params)
	
	if IsNumber(res) then
		CancelTran()
		return {false,res}
	end
	local result = {}
	return {true, result}
end

local StmtDef=
{ 
	"Dbs_SaveFightSkill",
	"replace into tbl_fight_skill( cs_uId, fs_sName, fs_uKind, fs_uLevel) values(?,?,?,?);"
}
DefineSql(StmtDef,StmtContainer)

function GMDBExecutor.AddNonFightSkill(data)
	local SkillName = data["SkillName"]
	local PlayerId = data["PlayerId"]
	local skill = NonFightSkill_Common(SkillName)
	if skill == nil then
		return false,2038
	end
	StmtContainer.Dbs_SaveFightSkill:ExecStat(PlayerId, SkillName, FightSkillKind.NonFightSkill,1)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,2038
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	return true
end

local StmtDef=
{
	"UpdateJfsPoint",
	[[
	update
		tbl_char_jifensaipoint
	set
		cs_uPoint = cs_uPoint + ?
	where
		cs_uId = ? and cs_uType = ? and cs_uPoint + ? >= 20
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"GetJfsPoint",
	[[
	select
		cs_uPoint
	from
		tbl_char_jifensaipoint
	where
		cs_uId = ? and cs_uType = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

function GMDBExecutor.AddJfsPoint(data)
	local CharId = data["charId"]
	local Type = data["Type"]
	local Point = data["Point"]
	
	if Point ~= 0 then
		StmtContainer.UpdateJfsPoint:ExecSql('',Point,CharId,Type,Point)
	end
	
	local CurPoint = nil
	local res = StmtContainer.GetJfsPoint:ExecSql('',CharId,Type)
	if res:GetRowNum() > 0 then
		CurPoint = res:GetData(0,0)
	end
	
	return CurPoint
end

function GMDBExecutor.GetDbTransInfo(data)
	local WaitingLocalChannel 	= g_DbChannelMgr:GetWaitingLocalChannel()
	local WaitingConnection 	= g_DbChannelMgr:GetWaitingConnection()
	local WaitingGlobalChannel 	= g_DbChannelMgr:GetWaitingGlobalChannel()
	local ExecutingSql 			= g_DbChannelMgr:GetExecutingSql()
	return WaitingLocalChannel, WaitingConnection, WaitingGlobalChannel, ExecutingSql
end

function GMDBExecutor.EnableSqlProfiler(data)
	local open = data["Open"]
	g_DbChannelMgr:EnableSqlProfiler(open)
end

function GMDBExecutor.GetDbSqlProfilerData(data)
	return g_DbChannelMgr:GetDbSqlProfilerData()
end

function GMDBExecutor.PrintMemoryData(data)
	CheckAllObjLeak(data["fileName"],data["maxLevel"])
end

return GMDBExecutor
