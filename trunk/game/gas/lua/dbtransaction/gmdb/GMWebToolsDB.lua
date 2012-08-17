gac_gas_require "framework/common/CMoney"
cfg_load "player/LevelExp_Common"
local g_StoreRoomIndex = g_StoreRoomIndex
local g_ItemInfoMgr = g_ItemInfoMgr
local g_MoneyMgr = CMoney:new()
local LevelExp_Common = LevelExp_Common
local g_TestRevisionMaxLevel = g_TestRevisionMaxLevel
local event_type_tbl = event_type_tbl
local MemH64ToMbcs = MemH64ToMbcs
local g_GetRoomRange = g_GetRoomRange
local g_StoreRoomIndex = g_StoreRoomIndex
local LogDBName = DeployConfig.MySqlDatabase .. "_log"

local StmtContainer = class()
--GM聊天记录
local StmtDef=
{
	"_AddGmMsg",
	"insert into tbl_gm_msg values(0,?,now(),?);"
}
DefineSql(StmtDef,StmtContainer)

--热更新执行结果记录
local StmtDef=
{
	"_AddHotUpdateResult",
	"insert ignore into tbl_hot_update_result set hur_type=?, hur_filename=?, hur_code=?, hur_result=?;"
}
DefineSql(StmtDef,StmtContainer)


--gas
local StmtDef=
{
	"_AddHotUpdateGas",
	"insert into tbl_hot_update_gas set hug_revision=?, hug_filename=?, hug_code=?;"
}
DefineSql(StmtDef,StmtContainer)

--gac
local StmtDef=
{
	"_AddHotUpdateGac",
	"insert into tbl_hot_update_gac set hug_revision=?, hug_filename=?, hug_code=?;"
}
DefineSql(StmtDef,StmtContainer)

--db
local StmtDef=
{
	"_AddHotUpdateDb",
	"insert into tbl_hot_update_db set hud_revision=?, hud_filename=?, hud_code=?;"
}
DefineSql(StmtDef,StmtContainer)

--玩家名称
local StmtDef=
{
	"_GetUserName",
	"select us_sName,us_uGameID from tbl_user_static where us_uId=?;"
}
DefineSql(StmtDef,StmtContainer)
--角色编号
local StmtDef=
{
	"_GetRoleId",
	"select cs_uId from tbl_char where c_sName=?;"
}
DefineSql(StmtDef,StmtContainer)
--发送邮件离线提示
local StmtDef=
{
	"_SendMailNotice",
	"insert into tbl_system_message set sm_uMsgId='5001', cs_uId=?;"
}
DefineSql(StmtDef,StmtContainer)
--冻结帐号
local StmtDef=
{
	"FreezeAccount",
	"insert ignore into tbl_freeze_account(username,us_uGameID) values(?,?)"
}
DefineSql(StmtDef,StmtContainer)
--取消冻结
local StmtDef=
{
	"CancelFreezeAccount",
	"delete from tbl_freeze_account where username = ? and us_uGameID = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"MoveRole",
	"update tbl_char_position set cp_uPosX = '?', cp_uPosY='?', sc_uId='?' where cs_uId = '?'"
}
DefineSql(StmtDef,StmtContainer)

--加经验
local StmtDef=
{
	"_GetExp",
	"select cs_uLevelExp from  tbl_char_experience where cs_uId = ?"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"_AddGMMgr",
	"replace into tbl_gm_validate set username = ?,gv_uLevel = ?,us_uGameID = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_AddCurrencyLimit",
	"replace into tbl_money_corrency_limit set mcl_sFunName = ?,mcl_sModule = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_AddJifenUseLimit",
	"replace into tbl_jifen_use_limit set jul_uType = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_AddItemUseLimit",
	"replace into tbl_item_use_limit set iul_sItemName = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_AddQuestFinishLimit",
	"replace into tbl_quest_finish_limit set qfl_sQuestName = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_AddObjDropLimit",
	"replace into tbl_obj_drop_limit set odl_sObjName = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_DelCurrencyLimit",
	"delete from tbl_money_corrency_limit where mcl_sFunName = ? and mcl_sModule = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_DelJifenUseLimit",
	"delete from tbl_jifen_use_limit where jul_uType = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_DelItemUseLimit",
	"delete from tbl_item_use_limit where iul_sItemName = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_DelQuestFinishLimit",
	"delete from tbl_quest_finish_limit where qfl_sQuestName = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_DelObjDropLimit",
	"delete from tbl_obj_drop_limit where odl_sObjName = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_SelectItemOnBodyByName",
	[[
		select ce_uEquipPart from tbl_item_equip as equip,tbl_item_static as tis
		where tis.is_uId = equip.is_uId and tis.is_sName = ? and equip.cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_DelGMMgr",
	"delete from tbl_gm_validate where  username = ? and us_uGameID = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_RoleRename",
	"update tbl_char set c_sName = ? where c_sName = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_CheckNewName",
	"(select cs_uId from tbl_char where c_sName = ?) union (select cs_uId from tbl_char_deleted where cd_sName = ?)"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_CheckTongName",
	"select t_uId from tbl_tong where t_sName = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_TongRename",
	"update tbl_tong set t_sName = ? where t_uId = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_TongChangePosition",
	"update tbl_member_tong set mt_sPosition = ? where cs_uId = ? and t_uId = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_TongChangelevel",
	"update tbl_tong set t_uLevel = ? where t_uId = ?"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_GetTongID",
	"select t_uId from tbl_tong where t_sName = ?"
}
DefineSql( StmtDef, StmtContainer )

local GMWebToolsDB = CreateDbBox(...)
function GMWebToolsDB.TextExecute(data)
	local qs = data["qs"]
	local fm = data["fm"]
	local report, result = g_DbChannelMgr:TextExecute(qs)
	if report=="" then
		return result
	else
		return report
	end
end

function GMWebToolsDB.ShutUp(data)
	local tbl_role_name = data["role_name"]
	local channel_id = data["channel_id"]
	local ex = RequireDbBox("Exchanger")
	local channel_db = RequireDbBox("ChannelDB")
	local charIdTbl = {}
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local char_id = ex.getPlayerIdByName(role_name)
		if 0 ~= char_id then
			local data = {}
			data["char_id"] = char_id
			data["channel_id"] = channel_id
			channel_db.ForbidChannel(data)
			table.insert(charIdTbl,char_id)
		end
	end
	return charIdTbl
end

function GMWebToolsDB.ChannelShutUp(data)
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	local channel_id = data["channel_id"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false, 0}
		end
	end
	
	local channel_db = RequireDbBox("ChannelDB")
	local data = {}
	data["char_id"] = char_id
	data["channel_id"] = channel_id
	return {channel_db.CancelForbid(data), char_id}
end

function GMWebToolsDB.FreezeAccountById(data)
	local user_id = data['user_id']
	
	local tbl_user_name = StmtContainer._GetUserName:ExecStat(user_id)
	if tbl_user_name:GetRowNum() == 0 then
		return {-1201}
	end
	local username = tbl_user_name:GetData(0,0)
	local game_id = tbl_user_name:GetData(0,1)
	StmtContainer.FreezeAccount:ExecSql("", username,game_id)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return {1,username}
	end
	return {-1}
end

function GMWebToolsDB.FreezeAccountDB(data)
	local username = data['username']
	local game_id = data["game_id"]
	StmtContainer.FreezeAccount:ExecSql("", username,game_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function GMWebToolsDB.CancelFreezeAccountById(data)
	local user_id = data['user_id']
	local tbl_user_name = StmtContainer._GetUserName:ExecStat(user_id)
	if tbl_user_name:GetRowNum() == 0 then
		return -1201
	end
	local username = tbl_user_name:GetData(0,0)
	local game_id = tbl_user_name:GetData(0,1)
	StmtContainer.CancelFreezeAccount:ExecSql("", username,game_id)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return 1
	end
	return -1
end

function GMWebToolsDB.CancelFreezeAccountDB(data)
	local username = data['username']
	local game_id = data['game_id']
	StmtContainer.CancelFreezeAccount:ExecSql("", username,game_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function GMWebToolsDB.MoveRole(data)
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	local x_pos = data["x_pos"]
	local y_pos = data["y_pos"]
	local scene_id = data["scene_id"]
	if scene_id==0 then
		return false
	end
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return false
		end
	end
	StmtContainer.MoveRole:ExecSql("", x_pos, y_pos, scene_id, char_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function GMWebToolsDB.AddExp(data)
	local num = data["num"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false,0}
		end
	end
	local tblLevelExp = StmtContainer._GetExp:ExecStat(char_id)
	local nLevelExp = tblLevelExp:GetData(0,0) + num
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local nCurlevel = CharacterMediatorDB.GetPlayerLevel(char_id)
	if nCurlevel == LevelExp_Common.GetHeight() then
		return {false,char_id}
	end
	if nCurlevel == g_TestRevisionMaxLevel then
		return {false,char_id}
	end
	while nLevelExp >= LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") do
		--如果不能升级，当前经验为0
		if LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") == 0 then
			break
		end
		--减去升级经验，并且提升一级
		nLevelExp = nLevelExp - LevelExp_Common(nCurlevel, "ExpOfCurLevelUp")

		nCurlevel=nCurlevel+1
		if nCurlevel == LevelExp_Common.GetHeight() then
			nLevelExp = 0
		end
		if nCurlevel == g_TestRevisionMaxLevel then
			nLevelExp = 0
		end	
	end
	local params = {}
	params.char_id = char_id
	params.char_level = nCurlevel
	params.char_exp = nLevelExp
	params.nAddExp = num 
	params.addExpType = event_type_tbl["GM指令加经验"]
	local result = CharacterMediatorDB.AddLevel(params)
	return {true,result} 
end

function GMWebToolsDB.AddMoney(data)
	local num = data["num"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false,0}
		end
	end
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("GmTools")
	local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["AddMoney"],char_id, num,nil,event_type_tbl["GM指令加钱"])
	if not bFlag then
		CancelTran()
		return {false,0}
	end
	
	return {true,char_id}
end

function GMWebToolsDB.AddBindingMoney(data)
	local num = data["num"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false,0}
		end
	end
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("GmTools")
	local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["AddMoney"],char_id, 2, num,nil,event_type_tbl["GM指令加钱"])
	if not bFlag then
		CancelTran()
		return {false,0}
	end
	
	return {true,char_id}
end

function GMWebToolsDB.AddTicket(data)
	local num = data["num"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false,0}
		end
	end
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("GmTools")
		local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["AddTicket"],char_id, 3, num,nil,event_type_tbl["GM指令加钱"])
		if not bFlag then
			CancelTran()
			return {false,0}
		end

	return {true,char_id}
end

function GMWebToolsDB.AddQuest(data)
	local quest_name = data["quest_name"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false,0}
		end
	end
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	local data = {["PlayerId"] = char_id,["QuestName"]= quest_name}
	local tRet = RoleQuestDB.GMAddQuest(data)
	
	return {true,char_id,tRet}
end

function GMWebToolsDB.FinishQuest(data)
	local quest_name = data["quest_name"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false,0}
		end
	end
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	local data = {["PlayerId"] = char_id,["QuestName"]= quest_name}
	local tRet = RoleQuestDB.GMFinishQuest(data)
	
	return {true,char_id,tRet}
end

function GMWebToolsDB.DelQuest(data)
	local quest_name = data["quest_name"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false,0}
		end
	end
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	local data = {["PlayerId"] = char_id,["QuestName"]= quest_name}
	local tRet = RoleQuestDB.GiveUpQuest(data)
	
	return {true,char_id,tRet}
end

function GMWebToolsDB.AddItem(data)
	local num = data["num"]
	local item_name = data["item_name"]
	local item_type = data["item_type"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false,0}
		end
	end
	local data = { 
		["itemtype"] = item_type, 
		["itemname"] = item_name, 
		["count"] = num, 
		["index"] = g_StoreRoomIndex.PlayerBag, 
		["player_id"] = char_id
	}
	local lGMDBExecutor = RequireDbBox("GMDB")	
	local ret = lGMDBExecutor.AddItem(data)
	if not ret[1] then
		return {false}
	end
	return {true,char_id,ret[2]}
end

function GMWebToolsDB.AddSoul(data)
	local num = data["num"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false}
		end
	end
	
	local soulRoot,nBigId,item_name,soulcount = data["soulRoot"],data["nBigId"],data["item_name"],num
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local nBindStyle = 0
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local params= {}
			params.m_nType = nBigId
			params.m_sName = soulRoot .. ":" .. soulcount
			params.m_nBindingType = nBindStyle
			params.m_nCharID = char_id
			params.m_sCreateType =25
	local soulpearl_id = g_RoomMgr.CreateItem(params)
	if  soulpearl_id == false then
		return {false}
	end
	--将创建的魂珠放进背包
	local param = {}
	param.m_nCharID = char_id
	param.m_nItemType = nBigId
	param.m_sItemName = item_name
	param.m_tblItemID = {{soulpearl_id}}
	local bFlag, res = item_bag_box.AddCountItem(param)
	if  bFlag == false then
		return {false}
	end
	return {true, char_id,res, soulpearl_id,nBindStyle}
end

function GMWebToolsDB.AddBindingSoul(data)
	local num = data["num"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false}
		end
	end
	
	local soulRoot,nBigId,item_name,soulcount = data["soulRoot"],data["nBigId"],data["item_name"],num
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local nBindStyle = 2
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local params= {}
			params.m_nType = nBigId
			params.m_sName = soulRoot .. ":" .. soulcount
			params.m_nBindingType = nBindStyle
			params.m_nCharID = char_id
			params.m_sCreateType =25
	local soulpearl_id = g_RoomMgr.CreateItem(params)
	if  soulpearl_id == false then
		return {false}
	end
	--将创建的魂珠放进背包
	local param = {}
	param.m_nCharID = char_id
	param.m_nItemType = nBigId
	param.m_sItemName = item_name
	param.m_tblItemID = {{soulpearl_id}}
	local bFlag, res = item_bag_box.AddCountItem(param)
	if  bFlag == false then
		return {false}
	end
	return {true, char_id,res, soulpearl_id,nBindStyle}
end

function GMWebToolsDB.AddGMMgr(data)
	local user_name = data["user_name"]
	local gm_level = data["gm_level"]
	local game_id = data["game_id"]
	if gm_level > 127 then
		return false
	end
	StmtContainer._AddGMMgr:ExecSql("",user_name,gm_level,game_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function GMWebToolsDB.DelGMMmr(data)
	local user_name = data["user_name"]
	StmtContainer._DelGMMgr:ExecSql("",user_name,data["game_id"])
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function GMWebToolsDB.AddCurrencyLimit(data)
	local mode_name = data["mode_name"]
	local func_name = data["func_name"]
	StmtContainer._AddCurrencyLimit:ExecSql("",func_name,mode_name)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
function GMWebToolsDB.AddJifenUseLimit(data)
	local jifen_type = data["jifen_type"]
	StmtContainer._AddJifenUseLimit:ExecSql("",jifen_type)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
function GMWebToolsDB.AddItemUseLimit(data)
	local item_name = data["item_name"]
	if item_name then
		StmtContainer._AddItemUseLimit:ExecSql("",item_name)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
end
function GMWebToolsDB.AddQuestFinishLimit(data)
	local quest_name = data["quest_name"]
	StmtContainer._AddQuestFinishLimit:ExecStat(quest_name)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
function GMWebToolsDB.AddObjDropLimit(data)
	local obj_name = data["obj_name"]
	StmtContainer._AddObjDropLimit:ExecStat(obj_name)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
function GMWebToolsDB.DelCurrencyLimit(data)
	local mode_name = data["mode_name"]
	local func_name = data["func_name"]
	StmtContainer._DelCurrencyLimit:ExecSql("",func_name,mode_name)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
function GMWebToolsDB.DelJifenUseLimit(data)
	local jifen_type = data["jifen_type"]
	StmtContainer._DelJifenUseLimit:ExecSql("",jifen_type)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
function GMWebToolsDB.DelItemUseLimit(data)
	local item_name = data["item_name"]
	StmtContainer._DelItemUseLimit:ExecSql("",item_name)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
function GMWebToolsDB.DelQuestFinishLimit(data)
	local quest_name = data["quest_name"]
	StmtContainer._DelQuestFinishLimit:ExecStat(quest_name)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
function GMWebToolsDB.DelObjDropLimit(data)
	local obj_name = data["obj_name"]
	StmtContainer._DelObjDropLimit:ExecStat(obj_name)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function GMWebToolsDB.DelItemInBag(data)
	local item_type = data["item_type"]
	local item_name = data["item_name"]
	local item_num = data["item_num"]
	local char_id = data["role_id"]
	local role_name = data["role_name"]
	
	if not char_id then
		--说明条件角色name
		local ex = RequireDbBox("Exchanger")
		char_id = ex.getPlayerIdByName(role_name)
		if 0 == char_id then
			return {false}
		end
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local res = g_RoomMgr.DelItem(char_id,item_type,item_name,item_num,nil,event_type_tbl["GM删除物品"])	
	if IsNumber(res) then
		return {false}
	end
	return {true,char_id,res}
end


function GMWebToolsDB.DelItemOnBody(data)
	local item_type = data["item_type"]
	local item_name = data["item_name"]
	local role_name = data["role_name"]
	--说明条件角色name
	local ex = RequireDbBox("Exchanger")
	local char_id = ex.getPlayerIdByName(role_name)
	if 0 == char_id then
		return {false}
	end
	
	local res = StmtContainer._SelectItemOnBodyByName:ExecSql("n",item_name,char_id)
	local row = res:GetRowNum()
	if row == 0 then
		return {false}
	end
	local FightSkillDB = RequireDbBox("FightSkillDB")
	local ClassID = FightSkillDB.Dbs_SelectPlayerClass(char_id)
	local param = {}
	param.PlayerId = char_id
	param.sceneName = "GMWeb"
	param.Class = ClassID
	local retTbl = {}
	local UseItemSql = RequireDbBox("GasUseItemDB")
	for i=1,row do
		param.EquipPart = res:GetData(i-1,0)
		local bFlag,tbl = UseItemSql.DelEquip(param)
		if bFlag then
			tbl.EquipPart = res:GetData(i-1,0)
			table.insert(retTbl,tbl)
		end
	end
	return {true,char_id,retTbl}
end

function GMWebToolsDB.DelItemInDepot(data)
	local item_name = data["item_name"]
	local item_type = data["item_type"]
	local role_name = data["role_name"]
	local item_num = data["item_num"]
	--说明条件角色name
	local ex = RequireDbBox("Exchanger")
	local char_id = ex.getPlayerIdByName(role_name)
	if 0 == char_id then
		return {false}
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local res = g_RoomMgr.DelItem(char_id,item_type,item_name,item_num,g_StoreRoomIndex.PlayerDepot,event_type_tbl["GM删除物品"])	
	if IsNumber(res) then
		return {false}
	end
	return {true,char_id,res}
end

function GMWebToolsDB.DelItemInTongDepot(data)
	local page = data["page"]
	local pos = data["pos"]
	local tong_name = data["tong_name"]
	local msg = data["msg"]
	--说明条件角色name
	local g_CDepotSQLMgr = RequireDbBox("GasCDepotSQLMgrDB")
	local nDepotID = g_CDepotSQLMgr.GetTongDepotIDByTongName(tong_name)
	if nDepotID == 0 then
		--说明还没有仓库
		return {false}
	end
	local depotMgr = RequireDbBox("GasCDepotMgrDB")
	local tblRet = depotMgr.DeleteItemByPos(nDepotID,page,pos)
	if IsNumber(tblRet) then	
		return {false}
	else
		local tong = StmtContainer._CheckTongName:ExecSql("",tong_name)
		local tong_id = tong:GetData(0,0)
		local CTongBasicBox = RequireDbBox("GasTongBasicDB")
		CTongBasicBox.AddTongLogs(tong_id, msg,6)
		return {true}
	end
end
function GMWebToolsDB.GetEquipItemOnBody(data)
	local item_id = data["item_id"]
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local Info = {}
	Info.m_tblEquipInfo= EquipMgrDB.GetEquipAllInfo(item_id) 
	Info.item_type,Info.item_name = g_RoomMgr.GetItemType(item_id)  
	return Info
end
function SplitString(str, splitChar, tblResult, plain)
	assert(string.len(splitChar) == 1 or (string.len(splitChar) == 2 and string.sub(splitChar,1,1)=='%'))
	local i = 0
	local j = 0
	while true do
		j = i
		i = string.find(str, splitChar, i+1, plain)
		if i == nil then  break end
		assert( i ~= 1, "被分割字符：【" .. str .. "】, 分割标识：【" .. splitChar .. "】")
		table.insert(tblResult, string.sub(str, j+1 ,i-1))
	end
	table.insert(tblResult, string.sub(str, j+1, -1))
end 
function GMWebToolsDB.SendMail(data)
	local role_name	 = data["role_name"]
	local content	 = (data["content"] or "")
	local money		 = (data["money"] or 0)
	local items		 = (data["items"] or "")
	local tbl_role_id = StmtContainer._GetRoleId:ExecSql("", role_name)
	if tbl_role_id:GetRowNum() == 0 then
		return 0
	end
	local role_id = tbl_role_id:GetData(0,0)
	
	local MailExecutor = RequireDbBox("MailDB")
	local mailId = MailExecutor.SendMail("7006", role_id, "1014", content, money, {}, nil, 3, event_type_tbl["gm发的邮件"])
	
	local RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	if items ~= nil and items ~= "" then
		local index = 0;
		local tbl_items_info = {}
		SplitString(items, ';', tbl_items_info)
		for i=1,#tbl_items_info-1 do
			local items_info = tbl_items_info[i]
			index = index+1
			local tbl_item = {}
			SplitString(items_info, '_', tbl_item)
			
			local params= {}
			params.m_nType = tonumber(tbl_item[1])
			params.m_sName = tbl_item[2]
			params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(tbl_item[1],tbl_item[2],"BindingStyle") or 0
			params.m_nCharID = role_id
			params.m_sCreateType = event_type_tbl["gm发的邮件"]
			for j=1,tbl_item[3] do
				local itemID = RoomMgr.CreateItem(params)
				MailExecutor.AddMailItem(mailId[1],itemID,index)
			end
		end
	end
	return mailId[1]
end
function GMWebToolsDB.SendMailNotice(data)
	local role_name	 = data["role_name"]
	local tbl_role_id = StmtContainer._GetRoleId:ExecSql("", role_name)
	if tbl_role_id:GetRowNum() == 0 then
		return false
	end
	local role_id = tbl_role_id:GetData(0,0)
	StmtContainer._SendMailNotice:ExecSql("", role_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0;
end

function GMWebToolsDB.AddHotUpdateCode(data)
	local type       = data["type"]
	local revision   = data["revision"]
	local filename   = data["filename"]
	local code       = data["code"]
	if type == 1 then
		StmtContainer._AddHotUpdateGas:ExecSql("", revision, filename, code)
	elseif type == 2 then
		StmtContainer._AddHotUpdateDb:ExecSql("", revision, filename, code)
	else
		StmtContainer._AddHotUpdateGac:ExecSql("", revision, filename, code)
	end
	--通知所有的gas从数据库中读取热更新
	Db2GasCall("NotifyGasUpdate", {})
end

function GMWebToolsDB.AddHotUpdateResult(data)
	local type     = data["type"]
	local filename = data["filename"]
	local code     = data["code"]
	local result   = data["result"]
	StmtContainer._AddHotUpdateResult:ExecSql("", type, filename, code, result)

end

function GMWebToolsDB.SendGMMsg(data)
	local role_id		 = data["role_id"]
	local content		 = data["content"]
	StmtContainer._AddGmMsg:ExecSql("", role_id, content)
	return g_DbChannelMgr:LastAffectedRowNum() > 0;
end
function GMWebToolsDB.RoleRename(data)
	local role_name = data["role_name"]
	local new_name = data["new_name"]
	
	local char = StmtContainer._CheckNewName:ExecSql("", new_name, new_name)
	if char:GetRowNum()>0 then
		return false
	end
	
	StmtContainer._RoleRename:ExecSql("", new_name, role_name)
	return g_DbChannelMgr:LastAffectedRowNum() > 0;
end
function GMWebToolsDB.TongRename(data)
	local tong_id = data["tong_id"]
	local new_name = data["new_name"]
	
	local tong = StmtContainer._CheckTongName:ExecSql("",new_name)
	if tong:GetRowNum()>0 then
		return false
	end
	
	StmtContainer._TongRename:ExecSql("",new_name,tong_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0;
end
function GMWebToolsDB.RemoveTongMember(data)
	local tong_id = data["tong_id"]
	local role_id = data["role_id"]
	local msg = data["msg"]
	
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local bRet, ret = CTongBasicBox.DelTongMember(role_id,tong_id,"GM删除帮会成员")
	if not bRet or ret == nil then
		 return false
	end
	CTongBasicBox.AddTongLogs(tong_id, msg,1)
	return true
end

function GMWebToolsDB.ChangeTongMember(data)
	local tong_id = data["tong_id"]
	local role_id = data["role_id"]
	local position = data["position"]
	local msg = data["msg"]
	
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	StmtContainer._TongChangePosition:ExecSql("",position,role_id,tong_id)
	CTongBasicBox.AddTongLogs(tong_id, msg,1)
	return true
end

function GMWebToolsDB.ChangeTongLevel(data)
	local tong_id = data["tong_id"]
	local level = data["level"]
	
	StmtContainer._TongChangelevel:ExecSql("",level,tong_id)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveTongLevelInfo(tong_id,tonumber(level),event_type_tbl["佣兵小队GM指令加等级"])
	return true
end

function GMWebToolsDB.GetPlayerIdByName(data)
	local role_name = data["role_name"]
	local ex = RequireDbBox("Exchanger")
	return ex.getPlayerIdByName(role_name)
end

function GMWebToolsDB.GetTongId(date)
	local tong_name = date["tong_name"]
	local tong = StmtContainer._CheckTongName:ExecSql("",tong_name)
	if tong:GetRowNum() == 0 then
		return 0
	else
	  return tong:GetData(0,0)
	end
end

function GMWebToolsDB.AddItemToTong(data)
	local tong_id = tonumber(data["tong_id"])
	local item_type = tonumber(data["item_type"])
	local item_name = data["item_name"]
	local page = tonumber(data["page"])
	local num = tonumber(data["num"])
	local log = data["log"]
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sIndex, soulpearlCount = item_name,0
	if g_ItemInfoMgr:IsSoulPearl(item_type) then
      sIndex, soulpearlCount = g_ItemInfoMgr:GetSoulpearlInfo(sIndex)
	end
	local sIndex = MemH64ToMbcs(sIndex)
	if g_ItemInfoMgr:IsSoulPearl(item_type) then
		sIndex = sIndex .. ":" .. soulpearlCount
	end
	item_name = sIndex
	local params= {}
	params.m_nType = item_type
	params.m_sName = item_name
	params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"BindingStyle") or 0
	params.m_nCharID = 0
	params.m_sCreateType = event_type_tbl["gm工具给佣兵小队仓库加物品"]
	local cdepotMgr = RequireDbBox("GasCDepotMgrDB")
	local tbl_item_id = {}
	local depot_id = CTongBasicBox.GetTongDepotID(tong_id)
	local g_LogMgr = RequireDbBox("LogMgrDB")	
	for i=1,num do
		local itemID = RoomMgr.CreateItem(params)
		table.insert(tbl_item_id,itemID)
		g_LogMgr.DepotEventLog(tong_id,3,itemID,depot_id,0,0)
	end
	local res = cdepotMgr.AutoAddItems(depot_id, item_type, item_name,tbl_item_id,page)
	if IsNumber(res) then
		return false
	end
	CTongBasicBox.AddTongLogs(tong_id, log,6)
	return true
end

--补偿资金
function GMWebToolsDB.AddMoneyToTong(data)
	local tong_id = tonumber(data["tong_id"])
	local add_money = tonumber(data["add_money"])
	local CTongBasicBox  = RequireDbBox("GasTongBasicDB")
  local para = {["uTongID"]= tong_id, ["uMoney"] = add_money,["nEventType"] = event_type_tbl["佣兵小队补偿资金"]}
  return CTongBasicBox.UpdateTongMoney(para)
end

--补偿军资
function GMWebToolsDB.AddResourceToTong(data)
	local tong_id = tonumber(data["tong_id"])
	local add_resource = tonumber(data["add_resource"])
	local build_box = RequireDbBox("GasTongbuildingDB")
	return build_box.UpdateTongResource(add_resource,tong_id,event_type_tbl["佣兵小队补偿军资"])
end

--补偿荣誉
function GMWebToolsDB.AddHonorToTong(data)
	local tong_id = tonumber(data["tong_id"])
	local add_honor = tonumber(data["add_honor"])
	local GasTongBasicDB  = RequireDbBox("GasTongBasicDB")
	return GasTongBasicDB.UpdateHonor({["uTongID"] = tong_id,["uHonor"] = add_honor,["nEventType"] = event_type_tbl["佣兵小队补偿荣誉"]})
end

local StmtDef=
{
	"_Insert_Weapon",
	[[
		replace into tbl_item_weapon values
		(?,?,?,?,?,?,?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )

local function _add_weapon(tbl_static_info)
	StmtContainer._Insert_Weapon:ExecSql("", unpack(tbl_static_info))
end

local StmtDef=
{
	"_Insert_Armor",
	[[
		replace into tbl_item_armor values
		(?,?,?,?,?,?,?,?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )
local function _add_armor(tbl_static_info)
	StmtContainer._Insert_Armor:ExecSql("", unpack(tbl_static_info))
end

local StmtDef=
{
	"_Insert_Equip_Armor",
	[[
		replace into tbl_item_equip_armor values
		(?,?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )
local function _add_equip_armor(tbl_equip_armor)
	StmtContainer._Insert_Equip_Armor:ExecSql("", unpack(tbl_equip_armor))
end

local StmtDef=
{
	"_Insert_Ring",
	[[
		replace into tbl_item_ring values
		(?,?,?,?,?,?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )
local function _add_ring(tbl_static_info)
	StmtContainer._Insert_Ring:ExecSql("", unpack(tbl_static_info))
end

local StmtDef=
{
	"_Insert_Shield",
	[[
		replace into tbl_item_shield values
		(?,?,?,?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"_Insert_Shield_attr",
	[[
		replace into tbl_item_shield_Attr values
		(?,?,?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )
local function _add_shield(tbl_static_info)
	StmtContainer._Insert_Shield:ExecSql("", unpack(tbl_static_info))
end
local function _add_shield_attr(tbl_attr_info)
	StmtContainer._Insert_Shield_attr:ExecSql("", unpack(tbl_attr_info))
end

local StmtDef=
{
	"_Insert_Advanced_Info",
	[[
		replace into tbl_item_equip_advance values
		(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )
local function _add_advanced(tbl_advanced_info)
	StmtContainer._Insert_Advanced_Info:ExecSql("", unpack(tbl_advanced_info))
end


local StmtDef=
{
	"_Insert_Intensify",
	[[
		replace into tbl_item_equip_intensify values
		(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"_Insert_Intensify_AddAttr",
	[[
		replace into tbl_item_equip_intensifyAddAttr values
		(?,?,?,?,?,?,?,?,?,?,?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )
local function _add_intensify(tbl_intensify_info)
	StmtContainer._Insert_Intensify:ExecSql("", unpack(tbl_intensify_info))
end
local function _add_intensify_attr(tbl_intensify_attr_info)
	StmtContainer._Insert_Intensify_AddAttr:ExecSql("", unpack(tbl_intensify_attr_info))
end

local StmtDef=
{
	"_Insert_Durability",
	[[
		replace into tbl_item_equip_durability values
		(?,?,?);
	]]
}
DefineSql( StmtDef, StmtContainer )
local function _add_durability(tbl_durability_info)
	StmtContainer._Insert_Durability:ExecSql("", unpack(tbl_durability_info))
end

local StmtDef=
{
	"_Insert_Superaddation",
	[[
		replace into tbl_item_equip_superaddation values
		(?,?)
	]]
}
DefineSql( StmtDef, StmtContainer )
local function _add_superaddation(tbl_superaddation_info)
	StmtContainer._Insert_Superaddation:ExecSql("", unpack(tbl_superaddation_info))
end

function GMWebToolsDB.GetBackWeapon(data)
	local role_name	 = data["role_name"]
	local content	 = data["content"]
	local money		 = data["money"]
	local tbl_role_id = StmtContainer._GetRoleId:ExecSql("", role_name)
	if tbl_role_id:GetRowNum() == 0 then
		return 0
	end
	local role_id = tbl_role_id:GetData(0,0)
	local MailExecutor = RequireDbBox("MailDB")
	local mailId = MailExecutor.SendMail("7006", role_id, "1014", content, money, {}, nil, 3, event_type_tbl["gm发的邮件"])
	local RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local params= {}
	params.m_nType = tonumber(data["item_type"])
	params.m_sName = data["item_name"]
	params.m_nBindingType = tonumber(data["Bd_2"])
	params.m_nCharID = role_id
	params.m_sCreateType = event_type_tbl["gm恢复装备"]
	local itemID = RoomMgr.CreateItem(params)
	MailExecutor.AddMailItem(mailId[1],itemID,1)
	
	if data["St_0"] ~= "null" then
		local st_i = {itemID, "", data["St_2"], data["St_3"], data["St_4"], data["St_5"], data["St_6"], data["St_7"], data["St_8"]}
		_add_weapon(st_i)
	end
	if data["In_0"] ~= "null" then
		local in_i = {itemID, data["In_2"], data["In_3"], data["In_4"], data["In_5"], data["In_6"], data["In_7"], data["In_8"], data["In_9"], data["In_10"], data["In_11"], data["In_12"], data["In_13"], data["In_14"], data["In_15"], data["In_16"], data["In_17"]}
		_add_intensify(in_i)
	end
	if data["IAA_0"] ~= "null" and data["In_0"] ~= "null" then
		local iaa_i = {itemID, data["IAA_2"], data["IAA_3"], data["IAA_4"], data["IAA_5"], data["IAA_6"], data["IAA_7"], data["IAA_8"], data["IAA_9"], data["IAA_10"], data["IAA_11"], data["IAA_12"], data["IAA_13"]}
		_add_intensify_attr(iaa_i)
	end
	if data["Ad_0"] ~= "null" then
		local ad_i = {itemID, data["Ad_2"], data["Ad_3"], data["Ad_4"], data["Ad_5"], data["Ad_6"], data["Ad_7"], data["Ad_8"], data["Ad_9"], data["Ad_10"], data["Ad_11"], data["Ad_12"], data["Ad_13"], data["Ad_14"], data["Ad_15"], data["Ad_16"], data["Ad_17"], data["Ad_18"], data["Ad_19"], data["Ad_20"]}
		_add_advanced(ad_i)
	end
	if data["Ar1_0"] ~= "null" then
		local ar_i = {itemID, data["Ar1_2"], data["Ar1_3"], data["Ar1_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Ar2_0"] ~= "null" then
		local ar_i = {itemID, data["Ar2_2"], data["Ar2_3"], data["Ar2_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Ar3_0"] ~= "null" then
		local ar_i = {itemID, data["Ar3_2"], data["Ar3_3"], data["Ar3_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Ar4_0"] ~= "null" then
		local ar_i = {itemID, data["Ar4_2"], data["Ar4_3"], data["Ar4_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Du_0"] ~= "null" then
		local du_i = {itemID, data["Du_2"], data["Du_3"]}
		_add_durability(du_i)
	end
	if data["Su_0"] ~= "null" then
		local su_i = {itemID, data["Su_2"]}
		_add_superaddation(su_i)
	end
	
	return mailId[1]
end


function GMWebToolsDB.GetBackArmor(data)
	local role_name	 = data["role_name"]
	local content	 = data["content"]
	local money		 = data["money"]
	local tbl_role_id = StmtContainer._GetRoleId:ExecSql("", role_name)
	if tbl_role_id:GetRowNum() == 0 then
		return 0
	end
	local role_id = tbl_role_id:GetData(0,0)
	local MailExecutor = RequireDbBox("MailDB")
	local mailId = MailExecutor.SendMail("7006", role_id, "1014", content, money, {}, nil, 3, event_type_tbl["gm发的邮件"])
	local RoomMgr = RequireDbBox("GasRoomMgrDB")

	local params= {}
	params.m_nType = tonumber(data["item_type"])
	params.m_sName = data["item_name"]
	params.m_nBindingType = tonumber(data["Bd_2"])
	params.m_nCharID = role_id
	params.m_sCreateType = event_type_tbl["gm恢复装备"]
	local itemID = RoomMgr.CreateItem(params)
	MailExecutor.AddMailItem(mailId[1],itemID,1)

	if data["St_0"] ~= "null" then
		local st_i = {itemID, "", data["St_2"], data["St_3"], data["St_4"], data["St_5"], data["St_6"], data["St_7"], data["St_8"], data["St_9"]}
		_add_armor(st_i)
	end
	if data["In_0"] ~= "null" then
		local in_i = {itemID, data["In_2"], data["In_3"], data["In_4"], data["In_5"], data["In_6"], data["In_7"], data["In_8"], data["In_9"], data["In_10"], data["In_11"], data["In_12"], data["In_13"], data["In_14"], data["In_15"], data["In_16"], data["In_17"]}
		_add_intensify(in_i)
	end
	if data["IAA_0"] ~= "null" and data["In_0"] ~= "null" then
		local iaa_i = {itemID, data["IAA_2"], data["IAA_3"], data["IAA_4"], data["IAA_5"], data["IAA_6"], data["IAA_7"], data["IAA_8"], data["IAA_9"], data["IAA_10"], data["IAA_11"], data["IAA_12"], data["IAA_13"]}
		_add_intensify_attr(iaa_i)
	end
	if data["Ad_0"] ~= "null" then
		local ad_i = {itemID, data["Ad_2"], data["Ad_3"], data["Ad_4"], data["Ad_5"], data["Ad_6"], data["Ad_7"], data["Ad_8"], data["Ad_9"], data["Ad_10"], data["Ad_11"], data["Ad_12"], data["Ad_13"], data["Ad_14"], data["Ad_15"], data["Ad_16"], data["Ad_17"], data["Ad_18"], data["Ad_19"], data["Ad_20"]}
		_add_advanced(ad_i)
	end
	if data["Ar1_0"] ~= "null" then
		local ar_i = {itemID, data["Ar1_2"], data["Ar1_3"], data["Ar1_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Ar2_0"] ~= "null" then
		local ar_i = {itemID, data["Ar2_2"], data["Ar2_3"], data["Ar2_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Du_0"] ~= "null" then
		local du_i = {itemID, data["Du_2"], data["Du_3"]}
		_add_durability(du_i)
	end
	if data["Su_0"] ~= "null" then
		local su_i = {itemID, data["Su_2"]}
		_add_superaddation(su_i)
	end

	return mailId[1]
end


function GMWebToolsDB.GetBackShield(data)
	local role_name	 = data["role_name"]
	local content	 = data["content"]
	local money		 = data["money"]
	local tbl_role_id = StmtContainer._GetRoleId:ExecSql("", role_name)
	if tbl_role_id:GetRowNum() == 0 then
		return 0
	end
	local role_id = tbl_role_id:GetData(0,0)
	local MailExecutor = RequireDbBox("MailDB")
	local mailId = MailExecutor.SendMail("7006", role_id, "1014", content, money, {}, nil, 3, event_type_tbl["gm发的邮件"])
	local RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local params= {}
	params.m_nType = tonumber(data["item_type"])
	params.m_sName = data["item_name"]
	params.m_nBindingType = tonumber(data["Bd_2"])
	params.m_nCharID = role_id
	params.m_sCreateType = event_type_tbl["gm恢复装备"]
	local itemID = RoomMgr.CreateItem(params)
	MailExecutor.AddMailItem(mailId[1],itemID,1)
	
	if data["St_0"] ~= "null" then
		local st_i = {itemID, "", data["St_2"], data["St_3"], data["St_4"], data["St_5"]}
		_add_shield(st_i)
	end
	if data["Sa_0"] ~= "null" and data["St_0"] ~= "null" then
		local sa_i = {itemID, data["Sa_2"],data["Sa_3"],data["Sa_4"],data["Sa_5"]} 
		_add_shield_attr(sa_i)
	end
	if data["In_0"] ~= "null" then
		local in_i = {itemID, data["In_2"], data["In_3"], data["In_4"], data["In_5"], data["In_6"], data["In_7"], data["In_8"], data["In_9"], data["In_10"], data["In_11"], data["In_12"], data["In_13"], data["In_14"], data["In_15"], data["In_16"], data["In_17"]}
		_add_intensify(in_i)
	end
	if data["IAA_0"] ~= "null" and data["In_0"] ~= "null" then
		local iaa_i = {itemID, data["IAA_2"], data["IAA_3"], data["IAA_4"], data["IAA_5"], data["IAA_6"], data["IAA_7"], data["IAA_8"], data["IAA_9"], data["IAA_10"], data["IAA_11"], data["IAA_12"], data["IAA_13"]}
		_add_intensify_attr(iaa_i)
	end
	if data["Ad_0"] ~= "null" then
		local ad_i = {itemID, data["Ad_2"], data["Ad_3"], data["Ad_4"], data["Ad_5"], data["Ad_6"], data["Ad_7"], data["Ad_8"], data["Ad_9"], data["Ad_10"], data["Ad_11"], data["Ad_12"], data["Ad_13"], data["Ad_14"], data["Ad_15"], data["Ad_16"], data["Ad_17"], data["Ad_18"], data["Ad_19"], data["Ad_20"]}
		_add_advanced(ad_i)
	end
	if data["Ar1_0"] ~= "null" then
		local ar_i = {itemID, data["Ar1_2"], data["Ar1_3"], data["Ar1_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Ar2_0"] ~= "null" then
		local ar_i = {itemID, data["Ar2_2"], data["Ar2_3"], data["Ar2_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Du_0"] ~= "null" then
		local du_i = {itemID, data["Du_2"], data["Du_3"]}
		_add_durability(du_i)
	end
	if data["Su_0"] ~= "null" then
		local su_i = {itemID, data["Su_2"]}
		_add_superaddation(su_i)
	end
	
	return mailId[1]
end


function GMWebToolsDB.GetBackRing(data)
	local role_name	 = data["role_name"]
	local content	 = data["content"]
	local money		 = data["money"]
	local tbl_role_id = StmtContainer._GetRoleId:ExecSql("", role_name)
	if tbl_role_id:GetRowNum() == 0 then
		return 0
	end
	local role_id = tbl_role_id:GetData(0,0)
	local MailExecutor = RequireDbBox("MailDB")
	local mailId = MailExecutor.SendMail("7006", role_id, "1014", content, money, {}, nil, 3, event_type_tbl["gm发的邮件"])
	local RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local params= {}
	params.m_nType = tonumber(data["item_type"])
	params.m_sName = data["item_name"]
	params.m_nBindingType = tonumber(data["Bd_2"])
	params.m_nCharID = role_id
	params.m_sCreateType = event_type_tbl["gm恢复装备"]
	local itemID = RoomMgr.CreateItem(params)
	MailExecutor.AddMailItem(mailId[1],itemID,1)

	if data["St_0"] ~= "null" then
		local st_i = {itemID, "", data["St_2"], data["St_3"], data["St_4"], data["St_5"], data["St_6"], data["St_7"]}
		_add_ring(st_i)
	end
	if data["In_0"] ~= "null" then
		local in_i = {itemID, data["In_2"], data["In_3"], data["In_4"], data["In_5"], data["In_6"], data["In_7"], data["In_8"], data["In_9"], data["In_10"], data["In_11"], data["In_12"], data["In_13"], data["In_14"], data["In_15"], data["In_16"], data["In_17"]}
		_add_intensify(in_i)
	end
	if data["IAA_0"] ~= "null" and data["In_0"] ~= "null" then
		local iaa_i = {itemID, data["IAA_2"], data["IAA_3"], data["IAA_4"], data["IAA_5"], data["IAA_6"], data["IAA_7"], data["IAA_8"], data["IAA_9"], data["IAA_10"], data["IAA_11"], data["IAA_12"], data["IAA_13"]}
		_add_intensify_attr(iaa_i)
	end
	if data["Ad_0"] ~= "null" then
		local ad_i = {itemID, data["Ad_2"], data["Ad_3"], data["Ad_4"], data["Ad_5"], data["Ad_6"], data["Ad_7"], data["Ad_8"], data["Ad_9"], data["Ad_10"], data["Ad_11"], data["Ad_12"], data["Ad_13"], data["Ad_14"], data["Ad_15"], data["Ad_16"], data["Ad_17"], data["Ad_18"], data["Ad_19"], data["Ad_20"]}
		_add_advanced(ad_i)
	end
	if data["Ar1_0"] ~= "null" then
		local ar_i = {itemID, data["Ar1_2"], data["Ar1_3"], data["Ar1_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Ar2_0"] ~= "null" then
		local ar_i = {itemID, data["Ar2_2"], data["Ar2_3"], data["Ar2_4"]}
		_add_equip_armor(ar_i)
	end
	if data["Du_0"] ~= "null" then
		local du_i = {itemID, data["Du_2"], data["Du_3"]}
		_add_durability(du_i)
	end
	if data["Su_0"] ~= "null" then
		local su_i = {itemID, data["Su_2"]}
		_add_superaddation(su_i)
	end
	
	return mailId[1]
end

function GMWebToolsDB.GetBackRole(data)
	local data = {["id"] = data["role_id"]}
	local LoginServerDB = RequireDbBox("LoginServerDB")
	if LoginServerDB.GetBackRole(data) == nil then
		return false;
	end
	return true;
end

function GMWebToolsDB.DelChar(data)
	local data = {["cs_uId"] = data["role_id"]}
	local LoginServerDB = RequireDbBox("LoginServerDB")
	if LoginServerDB.DelChar(data) == nil then
		return false;
	end
	return true;
end

local StmtDef=
{
	"_ClearItemInBag",
	[[
		select gir_uRoomIndex,gir_uPos,gi_uCount
		from tbl_grid_info iiig,tbl_grid_in_room igp
		where 
		igp.gir_uGridID = iiig.gir_uGridID
		and
		cs_uId = ?
		and 
		( 
          (gir_uRoomIndex >= ? and gir_uRoomIndex <= ?)
          or gir_uRoomIndex = ?
    ) 
	]]
}
DefineSql( StmtDef, StmtContainer )
function GMWebToolsDB.ClearItemInBag(data)
	local role_name = data["role_name"]
	local ex = RequireDbBox("Exchanger")
	local char_id = ex.getPlayerIdByName(role_name)
	local nStoreRoom = g_StoreRoomIndex.PlayerBag
	local nFirstRoom,nLastRoom = g_GetRoomRange(nStoreRoom)
	local tbl_items = StmtContainer._ClearItemInBag:ExecStat(char_id,nFirstRoom,nLastRoom,nStoreRoom)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	for i = 1, tbl_items:GetRowNum() do
		g_RoomMgr.DelItemByPos(char_id,tbl_items:GetData(i-1,0),tbl_items:GetData(i-1,1),tbl_items:GetData(i-1,2),event_type_tbl["GM删除物品"])
	end
	return true
end


function GMWebToolsDB.ClearItemInDepot(data)
	local role_name = data["role_name"]
	local ex = RequireDbBox("Exchanger")
	local char_id = ex.getPlayerIdByName(role_name)
	local nStoreRoom = g_StoreRoomIndex.PlayerDepot
	local nFirstRoom,nLastRoom = g_GetRoomRange(nStoreRoom)
	local tbl_items = StmtContainer._ClearItemInBag:ExecStat(char_id,nFirstRoom,nLastRoom,nStoreRoom)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	for i = 1, tbl_items:GetRowNum() do
		g_RoomMgr.DelItemByPos(char_id,tbl_items:GetData(i-1,0),tbl_items:GetData(i-1,1),tbl_items:GetData(i-1,2),event_type_tbl["GM删除物品"])
	end
	return true
end

local StmtDef=
{
	"_ClearChannelChat",
	"delete from " .. LogDBName .. ".tbl_log_channel_chat where lcc_dtChatTime < curdate()"
}
DefineSql( StmtDef, StmtContainer )
function GMWebToolsDB.ClearChannelChat(data)
	StmtContainer._ClearChannelChat:ExecSql("")
	return true;
end

local StmtDef=
{
	"_InsertConfServer",
	"insert into tbl_conf_server (sc_sVarName, sc_sVarValue) values (?,?)"
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_DeleteConfServer",
	"delete from tbl_conf_server where sc_sVarName=?"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"_GetWebshopDrawInfo",
	"select wdd_uRetCode from tbl_webshop_draw_detail where cs_uId = ? and unix_timestamp(now()) - unix_timestamp(wdd_dtTime) <= 300"
}
DefineSql( StmtDef, StmtContainer )

function GMWebToolsDB.SetConfServer(data)
	local name=data["name"]
	local value=data["value"]
	local flag=data["flag"]
	StmtContainer._DeleteConfServer:ExecSql("",name)
	if flag==nil or flag~="delete" then
		StmtContainer._InsertConfServer:ExecSql("",name,value)
	end
	return true
end

--webshop购买
function GMWebToolsDB.BuyItemFromWebShop(data)
	local item_name = data["item_name"]
	local char_id 	= tonumber(data["char_id"])
	local item_type = tonumber(data["item_type"])
	local count 		= tonumber(data["count"])
	local money 		= tonumber(data["money"])
	local money_type= tonumber(data["money_type"])
	if money and money > 0 then
		local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
		local enough_flag = MoneyManagerDB.EnoughMoneyByType(char_id, money_type,money)
	  if not enough_flag  then
	  	return {false, -1}
	  end
	  local fun_info = g_MoneyMgr:GetFuncInfo("WebShop")
	  local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["buy"],char_id, money_type, - money,nil, event_type_tbl["WebShop购买物品"])
		if not bFlag then
			return {false, -2}
		end
	end
	local sIndex, soulpearlCount = item_name,0
	if g_ItemInfoMgr:IsSoulPearl(item_type) then
      sIndex, soulpearlCount = g_ItemInfoMgr:GetSoulpearlInfo(sIndex)
	end
	local sIndex = MemH64ToMbcs(sIndex)
	if g_ItemInfoMgr:IsSoulPearl(item_type) then
		sIndex = sIndex .. ":" .. soulpearlCount
	end
	item_name = sIndex
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local params= {}
	params.nCharID 		= char_id
	params.nStoreMain = g_StoreRoomIndex.PlayerBag
	params.nBigID 		= item_type
	params.nIndex 		= item_name
	params.nCount	 		= count
	params.createType = event_type_tbl["GM添加物品"]
	local res = g_RoomMgr.AddItem(params)
	if not IsTable(res) then
		return {false, -3}
	end
	return {true, 1}
end

--webshop兑换金钱[添加兑换信息]
function GMWebToolsDB.AddDrawBalanceInfo(data)
	local char_id 	= tonumber(data["char_id"])
	local money = tonumber(data["money"])
	local AgiotageDB = RequireDbBox("AgiotageDB")
	local ParamsInfo = {}
	ParamsInfo.char_id 	= char_id
	ParamsInfo.amount		= money
	ParamsInfo.DrawType = 2
	local res = AgiotageDB.AddUserDrawBalanceInfo(ParamsInfo)
	return res == true
end

--webshop兑换金钱[是否兑换成功]
function GMWebToolsDB.IsDrawBalanceSucc(data)
	local char_id 	= tonumber(data["char_id"])
	local result = StmtContainer._GetWebshopDrawInfo:ExecStat(char_id)
	return (result:GetRowNum() > 0  and result(1,1) == 1)
end

local StmtDef=
{
	"_GetGMSLangs",
	"select distinct wsc_sValue from tbl_web_server_conf where wsc_sName = 'GMS_SUPPORT_LANG'"
}
DefineSql( StmtDef, StmtContainer )

--GMS预加载的语言文件
function GMWebToolsDB.GetGMSLangs(data)
	local result = StmtContainer._GetGMSLangs:ExecStat()
	local row = result:GetRowNum()
	local retTbl = {}
	for i=1,row do
		local Lang = result:GetData(i-1,0)
		table.insert(retTbl,Lang)
	end
	return retTbl
end

--阵营招募参数
function GMWebToolsDB.UpdateRecruitInfo(data)
	local GasRecruitSql = RequireDbBox("RecruitPlayerDB")
	GasRecruitSql.UpdateRecruitInfo(data)
	return true
end

local StmtDef=
{
	"_TempBackData_GetList",
	"select et_uOldId, cs_uId, et_uType, et_sName from tbl_equip_temp where et_uNewId is null"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"_TempBackData_SetNewId",
	"update tbl_equip_temp set et_uNewId = ? where et_uOldId = ?"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"_TempBackData_LastStep",
	[[
replace into tbl_item_armor 
select et_uNewId, ia_sName, ia_uBaseLevel, ia_uCurLevel, ia_uUpgradeAtrrValue1, ia_uUpgradeAtrrValue2, ia_uUpgradeAtrrValue3, ia_uStaticProValue, ia_uIntensifyQuality, ia_sIntenSoul 
from tbl_equip_temp et, tbl_item_armor_tmp it
where et.le_uOldId = it.is_uId;

replace into tbl_item_equip_advance 
select et_uNewId, iea_uCurAdvancePhase , iea_uTotalAdvancedTimes , iea_uSkillPieceIndex1 , iea_uSkillPieceIndex2 , iea_uSkillPieceIndex3 , iea_uSkillPieceIndex4 , iea_uChoosedSkillPieceIndex , iea_sJingLingType , iea_uAdvanceSoulNum , iea_uAdvancedTimes , iea_uAdvancedAttrValue1 , iea_uAdvancedAttrValue2 , iea_uAdvancedAttrValue3 , iea_uAdvancedAttrValue4 , iea_sAdvancedSoulRoot , iea_sAdvancedAttr1 , iea_sAdvancedAttr2 , iea_sAdvancedAttr3 , iea_sAdvancedAttr4 
from tbl_equip_temp et, tbl_item_equip_advance_tmp it
where et.le_uOldId = it.is_uId;

replace into tbl_item_equip_armor 
select et_uNewId, iea_sAttr , iea_uAddTimes , iea_uIndex 
from tbl_equip_temp et, tbl_item_equip_armor_tmp it
where et.le_uOldId = it.is_uId;

replace into tbl_item_equip_durability 
select et_uNewId, ied_uMaxDuraValue , ied_uCurDuraValue 
from tbl_equip_temp et, tbl_item_equip_durability_tmp it
where et.le_uOldId = it.is_uId;

replace into tbl_item_equip_intensifyAddAttr 
select et_uNewId, iei_sAddAttr4 , iei_uAddAttr4Value , iei_sAddAttr5 , iei_uAddAttr5Value , iei_sAddAttr6 , iei_uAddAttr6Value , iei_sAddAttr7 , iei_uAddAttr7Value , iei_sAddAttr8 , iei_uAddAttr8Value , iei_sAddAttr9 , iei_uAddAttr9Value
from tbl_equip_temp et, tbl_item_equip_intensifyAddAttr_tmp it
where et.le_uOldId = it.is_uId;

replace into tbl_item_equip_superaddation 
select et_uNewId, ies_uCurSuperaddPhase 
from tbl_equip_temp et, tbl_item_equip_superaddation_tmp it
where et.le_uOldId = it.is_uId;

replace into tbl_item_is_binding 
select et_uNewId, isb_bIsbind 
from tbl_equip_temp et, tbl_item_is_binding_tmp it
where et.le_uOldId = it.is_uId;

replace into tbl_item_ring 
select et_uNewId, ir_sName , ir_uBaseLevel , ir_uCurLevel , ir_uDPS , ir_uStaticProValue , ir_uIntensifyQuality , ir_sIntenSoul 
from tbl_equip_temp et, tbl_item_ring_tmp it
where et.le_uOldId = it.is_uId;

replace into tbl_item_shield 
select et_uNewId, is_sName , is_uBaseLevel , is_uCurLevel , is_uIntensifyQuality , is_sIntenSoul 
from tbl_equip_temp et, tbl_item_shield_tmp it
where et.le_uOldId = it.is_uId;

replace into tbl_item_weapon 
select et_uNewId, iw_sName , iw_uBaseLevel , iw_uCurLevel , iw_uDPS , iw_uAttackSpeed , iw_uDPSFloorRate , iw_uIntensifyQuality , iw_sIntenSoul 
from tbl_equip_temp et, tbl_item_weapon_tmp it
where et.le_uOldId = it.is_uId;
]]
}
DefineSql( StmtDef, StmtContainer )

function GMWebToolsDB.TempBackData(data)
	local res = StmtContainer._TempBackData_GetList:ExecSql("n")
	local row = res:GetRowNum()
	if row == 0 then
		return {false}
	end
	for i=1,row do
		local old_id = res:GetData(i-1,0)
		local role_id = res:GetData(i-1,1)
		local item_type = res:GetData(i-1,2)
		local item_name = res:GetData(i-1,3)
		local MailExecutor = RequireDbBox("MailDB")
		local mailId = MailExecutor.SendMail("7006", role_id, "1014", "", 0, {}, nil, 3, event_type_tbl["gm发的邮件"])
		local RoomMgr = RequireDbBox("GasRoomMgrDB")
	
		local params= {}
		params.m_nType = tonumber(item_type)
		params.m_sName = item_name
		params.m_nBindingType = 1
		params.m_nCharID = role_id
		params.m_sCreateType = event_type_tbl["gm恢复装备"]
		local itemID = RoomMgr.CreateItem(params)
		MailExecutor.AddMailItem(mailId[1],itemID,1)
		
		StmtContainer._TempBackData_SetNewId:ExecSql("n",itemID,old_id)
	end
	
	StmtContainer._TempBackData_LastStep:ExecSql("n")
end

return GMWebToolsDB

