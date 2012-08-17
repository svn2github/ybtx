
local os = os
local _WriteShellLog = WriteShellLog
local tbl_cat = table.concat
local LogErr 	= LogErr
local event_type_tbl = event_type_tbl
local LogDBName = DeployConfig.MySqlDatabase .. "_log"

local function WriteLog(...)
	local arg = {...}
	
	_WriteShellLog(tbl_cat(arg, "\t"))
end

local CLogMgrDB = CreateDbBox(...)

local function LogStmtCall(Stmt,...)
	Stmt:ExecSql( "", ... )
end
------------------------------------------------------------------------------------
local StmtContainer = {}
local StmtDef=
{
	"_InsertEventStmt",
	"insert into "..  LogDBName .. ".tbl_log_event set le_dtDateTime=from_unixtime(?)"
}
DefineSql(StmtDef,StmtContainer)

--@brief 创建事件Id
function CLogMgrDB._InsertEvent()
	LogStmtCall(StmtContainer._InsertEventStmt, os.time())
	return g_DbChannelMgr:LastInsertId()
end
------------------------------------------------------------------------------------
--备份账号信息
function CLogMgrDB.CopyUserInfo(userId,userName)
	if userId then
		WriteLog("tbl_log_user_static",userId,userName,os.date("%y-%m-%d %H:%M:%S"))
	end
end
----------------------------------------------------------------------------------
--备份角色列表
function CLogMgrDB.CopyCharList(charId,charName)
	WriteLog("tbl_log_char",charId,charName)
end
----------------------------------------------------------------------------------
--删除角色列表
function CLogMgrDB.DelCharList(charId,charName)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_char_deleted",uEventId,charId,charName)
end
----------------------------------------------------------------------------------
--备份角色信息
function CLogMgrDB.CopyCharInfo(charId,userId,hair,hairColor,face,scale,gender,class,camp,inherence)
	if IsNumber(charId) then
		WriteLog("tbl_log_char_static",charId,userId,hair,hairColor,face,scale,gender,class,camp,os.date("%y-%m-%d %H:%M:%S"))
	end
end
----------------------------------------------------------------------------------
--备份物品信息
function CLogMgrDB.CopyItemInfo(itemId,itemType,ItemName,charId,code)
	WriteLog("tbl_log_item_static",itemId,itemType , ItemName,charId,code)
end
-------------------------------------------------[[登录登出相关]]--------------------------------------------
--@brief 登录
--@param uUserId:帐号Id
function CLogMgrDB.LogLogin(uUserId,ip,sn)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_user",uEventId , uUserId)
	WriteLog("tbl_log_login",uEventId,ip,sn)
end
-----------------------------------------------------------------
--@brief 登出
--@param uUserId:帐号Id
function CLogMgrDB.LogLogout(uUserId,sn)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_user",uEventId , uUserId)
	WriteLog("tbl_log_logout",uEventId, sn)
end
-----------------------------------------------------------------
--@brief 角色登录
function CLogMgrDB.LogCharLogin(uCharId,uLevel)
	local uEventId = CLogMgrDB._InsertEvent()
	
	WriteLog("tbl_log_char_login",uEventId,uCharId,uLevel)
end
------------------------------------------- [[加经验升级信息]]------------------------------------
--@brief 升级记录玩家信息
--@param uCharID:角色Id
--@param CurLevel:角色升级后的等级
function CLogMgrDB.PlayerLevelUpLog( uCharID, CurLevel)
	local uEventId = CLogMgrDB._InsertEvent() 
	WriteLog("tbl_log_player",uEventId , uCharID)
	WriteLog("tbl_log_level",uEventId , CurLevel)
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["角色升级"])		
end
--------------------------------------------------------------


local StmtDef=
{
	"_SelectExpCode",
	"select se_sCode from " .. LogDBName .. ".tbl_serial_experience where lcs_uId = ?"

}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_SaveExpCode",

	"replace into " .. LogDBName .. ".tbl_serial_experience set lcs_uId = ?, se_sCode = md5(?)"

}
DefineSql(StmtDef,StmtContainer)

--@brief 加经验记录玩家信息
--@param uCharID:角色Id
--@param CurLevel:角色升级后的等级
--@param nAddExp:角色所加经验值
function CLogMgrDB.LogPlayerAttribute( uCharID, CurLevel, nAddExp,addExpType,totalExp,eventId )
	if addExpType == nil and eventId == nil then
		LogErr("修改经验没有记录log类型")
	end
	local uEventId = eventId
	if not uEventId then
		uEventId = CLogMgrDB._InsertEvent() 
		WriteLog("tbl_log_player_taker",uEventId , uCharID)
		WriteLog("tbl_log_event_type",uEventId ,addExpType)
	end
	WriteLog("tbl_log_level",uEventId , CurLevel)
	local code_res = StmtContainer._SelectExpCode:ExecStat(uCharID)
	local code
	if code_res and code_res:GetRowNum() > 0 then
		code = code_res:GetData(0,0)
	else
		code = "tbl_log_experience"
	end
	
	WriteLog("tbl_log_experience",uEventId ,uCharID, nAddExp,totalExp,code)
  	LogStmtCall( StmtContainer._SaveExpCode, uCharID ,code)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
---------------------------------------[[物品操作]]-----------------------------------
--@brief 物品的产出
--@param uCharId：角色Id
--@param uItemId：物品Id
--@param sSceneName:产出的场景名称
function CLogMgrDB.LogCreateItem(uCharId,uItemId,createType,eventId)
	if createType == nil and eventId == nil then
		LogErr("物品创建没有记录log类型")
	end
	
	local uEventId = eventId
	if not uEventId then
		uEventId = CLogMgrDB._InsertEvent()
		WriteLog("tbl_log_player_taker",uEventId , uCharId)
		WriteLog("tbl_log_event_type",uEventId , createType)
	end
	WriteLog("tbl_log_item_taker",uEventId , uItemId)
end
----------------------------------------------------------------------------------------
--@brief 物品绑定
--@param uCharId：角色Id
--@param uItemId：物品Id
--@param uBindType:绑定类型
function CLogMgrDB.LogItemBindInfo(uCharId,uItemId,uBindType)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player",uEventId , uCharId)
	WriteLog("tbl_log_item_binding",uEventId , uItemId,uBindType)
end
----------------------------------------------------------------------------------------
local StmtDef=
{
	"_SelectDelItemCode",

	"select sid_sCode from " .. LogDBName .. ".tbl_serial_item_del where lcs_uId = ?"

}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_SaveDelItemCode",

	"replace into " .. LogDBName .. ".tbl_serial_item_del set lcs_uId = ?, sid_sCode = md5(?)"

}
DefineSql(StmtDef,StmtContainer)

--@brief 物品的删除(消耗或丢弃)
--@param uCharId：角色Id
--@param uItemId：物品Id
--@param delType：删除的类型 9--使用 10--销毁
function CLogMgrDB.LogDelAndUseItem(eventId,uCharId,uItemId,delType,item_type,item_name)
	local uEventId = eventId
	if not uEventId then
		uEventId = CLogMgrDB._InsertEvent()
		WriteLog("tbl_log_player_giver",uEventId , uCharId)
		WriteLog("tbl_log_event_type",uEventId , delType)
	end
	local code_res = StmtContainer._SelectDelItemCode:ExecStat(uCharId)
	local code
	if code_res:GetRowNum() > 0 then
		code = code_res:GetData(0,0)
	else
		code = "tbl_log_item_del"
	end
	CLogMgrDB.CopyItemInfo(uItemId,item_type,item_name,uCharId,"")
	WriteLog("tbl_log_item_del",uEventId , uItemId,code)
	LogStmtCall( StmtContainer._SaveDelItemCode, uCharId , code)
	return g_DbChannelMgr:LastAffectedRowNum() > 0,uEventId
end
----------------------------------------[[GM指令相关]]-----------------------------------
function CLogMgrDB.SaveGMLog(account, ip, content)
	WriteLog("tbl_log_gmcommand",account, ip,os.date("%y-%m-%d %H:%M:%S"), content)
end
--------------------------------------------[[金钱功能]]--------------------------------
local StmtDef=
{
	"_SelectMoneyCode",

	"select sm_sCode from " .. LogDBName .. ".tbl_serial_money where lcs_uId = ?"

}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_SaveMoneyCode",

	"replace into " .. LogDBName .. ".tbl_serial_money set lcs_uId = ?, sm_sCode = md5(?)"

}
DefineSql(StmtDef,StmtContainer)

--@brief 四种钱的变化,通过钱的种类和数目来判断到底应该记录在哪张表里
--@param uEventId:这次金钱的事件Id
--@param MoneyType:金钱类型
--@param MoneyCount:金钱的数量
function CLogMgrDB.PlayerMoneyModify( eventId, MoneyType, MoneyCount,uCharId,totalMoney,addType ,curlevel)
	if eventId == nil and addType == nil then
		LogErr("角色金钱修改没有加log类型!")
	end
	local uEventId
	if not eventId and addType > 1000 then
		uEventId = addType
	else
		uEventId = eventId
	end
	
	if not uEventId then
		uEventId = CLogMgrDB._InsertEvent()
		if MoneyCount >= 0 then
			WriteLog("tbl_log_player_taker",uEventId , uCharId)
		else
			WriteLog("tbl_log_player_giver",uEventId , uCharId)
		end
		WriteLog("tbl_log_event_type",uEventId ,addType)
	end

	local code_res = StmtContainer._SelectMoneyCode:ExecStat(uCharId)
	local code
	if code_res:GetRowNum() > 0 then
		code = code_res:GetData(0,0)
	else
		code = "tbl_log_money"		
	end
	WriteLog("tbl_log_level",uEventId , curlevel)
	WriteLog("tbl_log_money",uEventId,uCharId, MoneyCount,MoneyType,totalMoney,code)
	LogStmtCall(StmtContainer._SaveMoneyCode,uCharId,code)
	return g_DbChannelMgr:LastAffectedRowNum() > 0,uEventId
end
---------------------------------------------------------------------------------------
--@brief 记录税收log
function CLogMgrDB.SaveTaxLog(uMoneyCount,uCharId,addType)
	if addType == nil then
		LogErr("记录税收没有加log类型!")
	end
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type",uEventId ,addType)
	local code_res = StmtContainer._SelectMoneyCode:ExecStat(uCharId)
	local code
	if code_res:GetRowNum() > 0 then
		code = code_res:GetData(0,0)
	else
		code = "tbl_log_money"		
	end
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local totalMoney = MoneyManagerDB.GetMoneyByType(uCharId,1)
	WriteLog("tbl_log_money",uEventId,uCharId, -uMoneyCount,1,totalMoney,code)
end
---------------------------------------------------------------------------------------
--@brief 提升装备可强化上限
function CLogMgrDB.SetupEquipIntensifyLevelLog()
	local uEventId = CLogMgrDB._InsertEvent()
	
	WriteLog("tbl_log_equip_intensify_level_up",uEventId) 
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["提升装备可强化上限"])
	return uEventId
end
---------------------------------------------------------------------------------------
local StmtDef=
{
	"_SelectDepotMoneyCode",

	"select sdm_sCode from " .. LogDBName .. ".tbl_serial_depot_money where lcs_uId = ?"

}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_SaveDepotMoneyCode",

	"replace into " .. LogDBName .. ".tbl_serial_depot_money set lcs_uId = ?, sdm_sCode = md5(?)"

}
DefineSql(StmtDef,StmtContainer)

--@brief 记录仓库里钱的流通情况
function CLogMgrDB.SaveMoneyChangedFromDepot(eventId,uMoneyCount,uCharId,uTotalMoney)
	if eventId == nil then
		LogErr("修改仓库金钱没有记录log类型")
	end
	local uEventId = eventId
	local code_res = StmtContainer._SelectDepotMoneyCode:ExecStat(uCharId)
	local code
	if code_res and code_res:GetRowNum() > 0 then
		code = code_res:GetData(0,0)
	else
		code = "tbl_log_depot_money"
	end
	WriteLog("tbl_log_depot_money",uEventId , uMoneyCount,uTotalMoney,code)
	LogStmtCall(StmtContainer._SaveDepotMoneyCode, uCharId,code)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end 
---------------------------------------------------------------------------------------
--@brief 保存物品信息
--@param tblItemData：物品Id的table
function CLogMgrDB.SaveItemInfoToLog(uEventId, tblItemData,table_name)
	if not tblItemData or #tblItemData == 0 then
		return
	end
	local item_list_len = #tblItemData
	for i = 1, item_list_len do
		WriteLog(table_name,uEventId, tblItemData[i])
	end	
end
-----------------------------------------[[任务相关]]--------------------------------
--@brief 任务完成记录
--@param PlayerId：角色Id
--@param sQuestName：任务名称
function CLogMgrDB.FinishQuest(PlayerId, sQuestName)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_quest_finish",uEventId, sQuestName)
	WriteLog("tbl_log_player_taker",uEventId, PlayerId)
	--WriteLog("tbl_log_npc_giver",uEventId , "任务npc")
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["任务奖励"])
	return uEventId
end
-------------------------------------------------------------------------------------
--@brief 加荣誉记录
--@param uEventId：事件Id
--@param exp_info：荣誉奖励
function CLogMgrDB.AddHonorByEventId(uEventId,honor_count)
	if honor_count and honor_count > 0 then
		WriteLog("tbl_log_reputation",uEventId, honor_count)
	end
end
-------------------------------------------------------------------------------------
local StmtDef=
{
	"_SelectSoulCode",

	"select ss_sCode from " .. LogDBName .. ".tbl_serial_soul where lcs_uId = ?"

}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_SaveSoulCode",

	"replace into " .. LogDBName .. ".tbl_serial_soul set lcs_uId = ?, ss_sCode = md5(?)"

}
DefineSql(StmtDef,StmtContainer)


--@brief 加魂记录
--@param uEventId：事件Id
--@param SoulRet：魂奖励
function CLogMgrDB.AddSoulByEventId(eventId,SoulRet,addSoulType,char_id,totalSoul)
	if eventId == nil and addSoulType == nil then
		LogErr("修改魂没有记录log类型")
	end
	local uEventId = eventId
	local soulValue = tonumber(SoulRet)
	if not uEventId then
		uEventId = CLogMgrDB._InsertEvent()
		if soulValue < 0 then --消耗魂值
			WriteLog("tbl_log_player_giver", uEventId , char_id)
		else 		-- 增加魂值
			WriteLog("tbl_log_player_taker",uEventId , char_id)
		end
		WriteLog("tbl_log_event_type",uEventId , addSoulType)
	end
	local code_res = StmtContainer._SelectSoulCode:ExecStat(char_id)
	local code
	if code_res and code_res:GetRowNum() > 0 then
		code = code_res:GetData(0,0)
	else
		code = "tbl_log_soul"
	end
	WriteLog("tbl_log_soul",uEventId,char_id, soulValue,totalSoul,code)
	LogStmtCall( StmtContainer._SaveSoulCode, char_id , code)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
------------------------------------------------------------------------------
--@brief 任务失败记录
--@param uCharId：角色Id
--@param sQuestName：任务名称
function CLogMgrDB.QuestFail(uCharId,sQuestName)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_quest_failure",uEventId, uCharId, sQuestName)
end
------------------------------------------------------------------------------
--@brief 任务放弃记录
--@param uCharId：角色Id
--@param sQuestName：任务名称
function CLogMgrDB.GiveUpQuest(uCharId,sQuestName)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_quest_giveup",uEventId, uCharId, sQuestName)
end
---------------------------------------------------------------------------------------------
--玩家发送邮件事件
--邮件系统发件箱，发送邮件的记录
--记录的内容包括：单纯的邮资费用，发送物品附件时增加的邮资，寄送的金钱以及物品
--假设现在邮件系统中，基本邮资为10，每增加一个附件邮资增加1
function CLogMgrDB.PlayerSendEmail( uSenderId, reciever_id,ItemData,eventId,uSendMoney)
	local uEventId = eventId
	if not uEventId then
		uEventId = CLogMgrDB._InsertEvent()
		WriteLog("tbl_log_event_type",uEventId , event_type_tbl["邮寄"])
		WriteLog("tbl_log_player_giver",uEventId , uSenderId )
	end
	WriteLog("tbl_log_player_taker",uEventId , reciever_id )
	
	if #ItemData > 0 then
		for i = 1,#ItemData do
			WriteLog("tbl_log_item",uEventId , ItemData[i] )
		end
	end
	if uSendMoney and uSendMoney ~= 0 then
		local code_res = StmtContainer._SelectMoneyCode:ExecStat(reciever_id)
		local code
		if code_res:GetRowNum() > 0 then
			code = code_res:GetData(0,0)
		else
			code = "tbl_log_money"		
		end
		local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
		local totalMoney = MoneyManagerDB.GetMoneyByType(reciever_id,1)
		WriteLog("tbl_log_money",uEventId,reciever_id, uSendMoney,1,totalMoney,code)
	end
end
-----------------------------------------------------------------------------
--@brief 从邮件提取物品写log
--@param uCharId：角色Id
--@param ItemData：提取的物品
--@param logType：物品来源
function CLogMgrDB.TakerItemFromMailLog( uCharId,ItemData,logType)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player_taker",uEventId , uCharId )
	WriteLog("tbl_log_event_type",uEventId , logType)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_taker")
end
-------------------------------【交易所相关】----------------------------------
--@brief 寄售交易所购买物品列表
--@param uCharId：角色Id
--@param ItemData：出售的物品Id table
--@param MoneyType：金钱类型
--@param order_id：订单Id
--@param itemcount：物品数量
--@param price：物品单价
function CLogMgrDB.LogConsignmentBuyItem( uCharId,ItemData,order_id)
	local uEventId = CLogMgrDB._InsertEvent()
	--WriteLog("tbl_log_npc_giver",uEventId , "寄售交易所购买物品列表窗口")
	WriteLog("tbl_log_player_taker",uEventId , uCharId )
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["寄售交易所购买"])
	WriteLog("tbl_log_market_item",uEventId ,order_id)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_taker")
	return uEventId
end
------------------------------------------------------------------
--@brief 添加出售订单
--@param uCharId：角色Id
--@param ItemData：出售的物品Id table
--@param order_id：订单Id
--@param itemcount：物品数量
--@param price：物品单价
--@param needFee：手续费
function CLogMgrDB.LogConsignmentAddSellOrder( uCharId,ItemData,order_id,itemcount,price,uLevel )
	local uEventId = CLogMgrDB._InsertEvent()
	--WriteLog("tbl_log_npc_taker",uEventId , "寄售交易所出售物品窗口")
	WriteLog("tbl_log_player_giver",uEventId , uCharId)
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["添加出售订单"])
	WriteLog("tbl_log_market",uEventId ,order_id,itemcount,price)
	WriteLog("tbl_log_level",uEventId , uLevel)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_giver")
end
------------------------------------------------------------------
--@brief 取消出售订单
--@param uCharId：角色Id
--@param ItemData：出售的物品Id table
--@param order_id：订单Id
--@param itemcount：物品数量
--@param price：物品单价
function CLogMgrDB.LogConsignmentCancelSellOrder( uCharId,ItemData,order_id,delOrderType)
	local uEventId = CLogMgrDB._InsertEvent()
	--WriteLog("tbl_log_npc_giver",uEventId , "寄售交易所出售物品窗口")
	WriteLog("tbl_log_player_taker",uEventId , uCharId)
	WriteLog("tbl_log_event_type",uEventId , delOrderType)
	WriteLog("tbl_log_market_item",uEventId ,order_id)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_taker")
end
------------------------------------------------------------------
--@brief 添加求购订单
--@param uCharId：角色Id
--@param order_id：订单Id
--@param sItemName：求购的物品名称
--@param itemcount：物品数量
--@param price：物品单价
function CLogMgrDB.LogConsignmentAddBuyOrder( uCharId,order_id,sItemName,itemcount,price)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player_giver",uEventId , uCharId)
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["添加求购订单"])
	WriteLog("tbl_log_market_buy_order_static",uEventId ,order_id,sItemName,itemcount,price)
	local levelRet = StmtContainer._GetCharLevel:ExecStat(uCharId)
	WriteLog("tbl_log_level",uEventId , levelRet(1,1))
	return uEventId
end
------------------------------------------------------------------
--@brief 删除求购订单
--@param uCharId：角色Id
--@param order_id：订单Id
--@param delOrderType:删除方式(超时系统删除订单、手动取消订单、求购订单成功)
--@param ItemData:求购到的物品
function CLogMgrDB.LogConsignmentCancelBuyOrder( uCharId,order_id,delOrderType,ItemData)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player_taker",uEventId , uCharId)
	WriteLog("tbl_log_event_type",uEventId , delOrderType)
	WriteLog("tbl_log_market_item",uEventId ,order_id)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_taker")
	return uEventId
end
-----------------------------------------------------------------
--@brief 出售收购订单物品
--@param uCharId：角色Id
--@param ItemData：出售的物品Id table
--@param order_id：订单Id
function CLogMgrDB.LogConsignmentSellBuyOrderItem( uCharId,ItemData,order_id)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player_giver",uEventId , uCharId )
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["出售收购订单"])
	WriteLog("tbl_log_market_item",uEventId ,order_id)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_giver")
	return uEventId
end
--------------------------------【代工相关】---------------------------------
--@brief 代工订单制作
--@param uCharId：角色Id
--@param order_id：订单Id
function CLogMgrDB.LogMakeContractManuOrder( uCharId,ItemData,order_id)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player",uEventId , uCharId )
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["代工订单制作"])
	WriteLog("tbl_log_contract_manufacture_order",uEventId ,order_id)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item")
	return uEventId
end
------------------------------------------------------------------
--@brief 添加代工订单
--@param uCharId：角色Id
--@param order_id：订单Id
function CLogMgrDB.LogAddContractManuOrder( uCharId,ItemData,order_id,sSkillName,sDirection,sPrescripName,uCMMoney)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player_giver",uEventId , uCharId)
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["添加代工订单"])
	WriteLog("tbl_log_contract_manufacture_order_static",uEventId ,order_id,sSkillName,sDirection,sPrescripName,uCMMoney)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_giver")
	return uEventId
end
------------------------------------------------------------------
--@brief 取消代工订单
--@param uCharId：角色Id
--@param ItemData：出售的物品Id table
--@param order_id：订单Id
--@param delOrderType：取消的类型
function CLogMgrDB.LogCancelContractManuOrder( uCharId,ItemData,order_id,delOrderType)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player_taker",uEventId , uCharId)
	WriteLog("tbl_log_event_type",uEventId , delOrderType)
	WriteLog("tbl_log_contract_manufacture_order",uEventId ,order_id)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_taker")
end
------------------------------------[[与功能NPC交互事件]]-------------------------------------
--@brief 玩家从商店Npc处领取物品
--@param uCharId：角色Id
--@param npcName：对应功能npc名称
--@param Money：涉及到的钱
function CLogMgrDB.LogPlayerTakerFromNpc(uCharId, npcName,ItemData,event_type )
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_npc_giver",uEventId , npcName)
	WriteLog("tbl_log_player_taker", uEventId , uCharId)
	WriteLog("tbl_log_event_type",  uEventId , event_type)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_taker")
	return uEventId
end
--------------------------------------------------------------------------
--@brief 玩家在商店Npc处消耗物品
--@param uCharId：角色Id
--@param npcName：对应功能npc名称
--@param ItemData：涉及到的物品Id Table
--@param Money：涉及到的钱
function CLogMgrDB.LogPlayerGiverToNpc(uCharId, npcName, ItemData, event_type)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_npc_taker",  uEventId , npcName)
	WriteLog("tbl_log_player_giver",  uEventId , uCharId)
	WriteLog("tbl_log_event_type",  uEventId , event_type)
	CLogMgrDB.SaveItemInfoToLog(uEventId, ItemData,"tbl_log_item_giver")
	return uEventId
end
----------------------------------------------[[玩家交易事件]]---------------------------------------
--@brief 玩家交易事件
--@param TakerCharId: 交易的接收方
--@param GiverCharId：交易的给予方
--@param ItemData：被交易的物品Id table
--@param Money：交易用到的钱
function CLogMgrDB.LogPlayerTrade(invitorId,inviteeId, oItemData_R2S, oR2S_Money,eItemData_S2R, eS2R_Money)
	local uEventId = CLogMgrDB._InsertEvent()
	if #eItemData_S2R == 0 and oR2S_Money ~= 0 then
		WriteLog("tbl_log_player_trade",uEventId , invitorId,inviteeId,1, oR2S_Money,0)
	else
		local item_list_len = #eItemData_S2R
		for i = 1, item_list_len do
			WriteLog("tbl_log_player_trade",uEventId , invitorId,inviteeId,1, oR2S_Money,eItemData_S2R[i])
		end	
	end
	
	if #oItemData_R2S == 0 and eS2R_Money ~= 0 then
		WriteLog("tbl_log_player_trade",uEventId ,inviteeId,invitorId,1, eS2R_Money,0)
	else
		local item_list_len = #oItemData_R2S
		for i = 1, item_list_len do
			WriteLog("tbl_log_player_trade",uEventId ,inviteeId,invitorId,1, eS2R_Money,oItemData_R2S[i])
		end	
	end
end
-------------------------------[[装备相关]]----------------------------
--@brief 装备耐久信息
--@param charId:角色Id
--@param equipId:装备Id
--@param uMaxDuraValue:耐久上限值
--@param uCurDuraValue:当前耐久值
function CLogMgrDB.SaveEquipDuraInfo( charId, equipId,uMaxDuraValue,uCurDuraValue )
	local uEventId = CLogMgrDB._InsertEvent()
	--保存装备耐久信息
	WriteLog("tbl_log_item_equip_durability",  uEventId, equipId,uMaxDuraValue or 0,uCurDuraValue or 0)
	--保存角色Id
	WriteLog("tbl_log_player",  uEventId , charId)
end
---------------------------------------------------------------------
--@brief 装备的强化信息
--@param charId:角色Id
--@param res:装备强化信息
--#装备强化	时间+角色ID+装备ID+强化后装备信息+场景ID
function CLogMgrDB.SaveEquipIntensifyInfo( charId, res,eventId)
	local uEventId = eventId
	if not uEventId then
		uEventId = CLogMgrDB._InsertEvent()
		--保存角色Id
		WriteLog("tbl_log_player",  uEventId , charId)
	end
	if not IsNumber(res) then
		WriteLog("tbl_log_item_equip_intensify",  uEventId, res(1,1),res(1,2) or 0 ,res(1,3) or "",res(1,4) or 0 ,res(1,5) or "",res(1,6) or 0,res(1,7) or "",res(1,8) or 0,res(1,9) or "",res(1,10) or 0,res(1,11) or 0,res(1,12) or 0,res(1,13) or "",res(1,14) or 0,res(1,15) or 0,res(1,16) or 0,res(1,17) or 0)
	else
		WriteLog("tbl_log_item_equip_intensify",  uEventId, res)
	end
end
---------------------------------------------------------------------
--@brief 装备的强化4-9阶段属性信息
--@param charId:角色Id
--@param res:装备强化信息
--#装备强化	时间+角色ID+装备ID+强化后装备信息+场景ID
function CLogMgrDB.SaveEquipIntensifyInfoAddAttr( charId, res,eventId)
	local uEventId = eventId
	if not uEventId then
		uEventId = CLogMgrDB._InsertEvent()
		--保存角色Id
		WriteLog("tbl_log_player",  uEventId , charId)
	end
	if not IsNumber(res) then
		WriteLog("tbl_log_item_equip_intensifyAddAttr",  uEventId, res(1,1),res(1,2) or "",res(1,3) or 0,res(1,4) or "",res(1,5) or 0,res(1,6) or "",res(1,7) or 0,res(1,8) or "",res(1,9) or 0,res(1,10) or "",res(1,11) or 0,res(1,12) or "",res(1,13) or 0)
	else
		WriteLog("tbl_log_item_equip_intensifyAddAttr",  uEventId, res)
	end
end
---------------------------------------------------------------------
--@brief 装备的升级信息
--@param charId:角色Id
--@param equipId:装备Id
--@param sceneName:场景名称
--#装备升级	时间+角色ID+装备ID+升级后装备信息+场景ID
function CLogMgrDB.SaveEquipUpgradeInfo( charId, equipId,sceneName,uAfterLevel )
	local uEventId = CLogMgrDB._InsertEvent()
	--保存装备Id
	WriteLog("tbl_log_item",  uEventId , equipId)
	--保存装备升级信息
	WriteLog("tbl_log_equip_upgrade",   uEventId, uAfterLevel)
	--保存角色Id
	WriteLog("tbl_log_player",   uEventId , charId)
end
---------------------------------------------------------------------
--@brief 装备的洗白信息
--@param charId:角色Id
--#装备洗白	时间+角色ID+装备ID+洗白后装备信息+场景ID
function CLogMgrDB.SaveEquipIntensifyBackInfo( charId )
	local uEventId = CLogMgrDB._InsertEvent()
	--保存装备洗白信息
	WriteLog("tbl_log_equip_intensifyback",   uEventId)
	--保存角色Id
	WriteLog("tbl_log_player",    uEventId , charId)
	WriteLog("tbl_log_event_type", uEventId ,event_type_tbl["装备洗白"] )	
	return uEventId
end
---------------------------------------------------------------------
--@brief 穿装备
--@param charId:角色Id
--@param equipId:装备Id
--@param sceneName:场景名称
function CLogMgrDB.SaveEquipPutOnInfo( charId, equipId,equipPart )
	local uEventId = CLogMgrDB._InsertEvent()
	--保存装备Id
	WriteLog("tbl_log_item",    uEventId , equipId)
	--保存穿装备信息
	WriteLog("tbl_log_equip_puton",    uEventId,equipPart)
	--保存角色Id
	WriteLog("tbl_log_player",    uEventId, charId)
	return uEventId
end
---------------------------------------------------------------------
--@brief 摘除装备
--@param charId:角色Id
--@param equipId:装备Id
--@param sceneName:场景名称
function CLogMgrDB.SaveEquipPutOffInfo( charId, equipId,sceneName)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存装备Id
	WriteLog("tbl_log_item",    uEventId , equipId)
	--保存摘除装备信息
	WriteLog("tbl_log_equip_putoff",    uEventId)
	--保存角色Id
	WriteLog("tbl_log_player",   uEventId, charId)
	return uEventId
end
---------------------------------------------------------------------
--@brief 装备进阶
--@param charId:角色Id
--@param equipId:装备Id
--@param newAdvanceInfoTbl:最新的装备进阶信息
function CLogMgrDB.SaveEquipAdvanceInfo( charId,equipId,newAdvanceInfoRet)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存装备进阶信息
	WriteLog("tbl_log_item_equip_advance",   uEventId,equipId, 
	                            newAdvanceInfoRet(1,1),newAdvanceInfoRet(1,2) or 0,newAdvanceInfoRet(1,3) or 0,
                                newAdvanceInfoRet(1,4) or 0,newAdvanceInfoRet(1,5) or 0,newAdvanceInfoRet(1,6) or 0,
                                newAdvanceInfoRet(1,7)  or 0,newAdvanceInfoRet(1,8) or "",newAdvanceInfoRet(1,9) or 0,
                                newAdvanceInfoRet(1,10) or 0,newAdvanceInfoRet(1,11) or 0,newAdvanceInfoRet(1,12) or 0,
                                newAdvanceInfoRet(1,13) or 0,newAdvanceInfoRet(1,14) or 0,newAdvanceInfoRet(1,15) or "",
                                newAdvanceInfoRet(1,16) or "",newAdvanceInfoRet(1,17) or "",newAdvanceInfoRet(1,18) or "",
                                newAdvanceInfoRet(1,19) or "")
	--保存角色Id
	WriteLog("tbl_log_player",   uEventId, charId)
end
---------------------------------------------------------------------
--@brief 装备技能激活
--@param charId:角色Id
--@param equipId:装备Id
--@param sceneName:场景名称
function CLogMgrDB.SaveEquipAdvancedActiveSkill( charId,index)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存装备技能激活信息
	WriteLog("tbl_log_equip_advancedActiveSkill", uEventId,charId,index or 0)
end
---------------------------------------------------------------------
--@brief 记录防具信息
--@param charId:角色Id
--@param equipId:装备Id
function CLogMgrDB.SaveEquipArmorInfo( charId,equipId,uBaseLevel,uCurLevel,uUpgradeAtrrValue1,uUpgradeAtrrValue2,uUpgradeAtrrValue3,uStaticProValue,uIntensifyQuality,sIntenSoul)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存防具信息
	WriteLog("tbl_log_item_armor", uEventId,equipId,uBaseLevel,uCurLevel,uUpgradeAtrrValue1,uUpgradeAtrrValue2,uUpgradeAtrrValue3,uStaticProValue,uIntensifyQuality,sIntenSoul)
	WriteLog("tbl_log_player",   uEventId, charId)
end
---------------------------------------------------------------------
--@brief 记录武器信息
--@param charId:角色Id
--@param equipId:装备Id
function CLogMgrDB.SaveEquipWeaponInfo( charId,equipId,uBaseLevel,uCurLevel,uDPS,uAttackSpeed,uDPSFloorRate,uIntensifyQuality,sIntenSoul)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存武器信息
	WriteLog("tbl_log_item_weapon", uEventId,equipId,uBaseLevel,uCurLevel,uDPS,uAttackSpeed,uDPSFloorRate,uIntensifyQuality,sIntenSoul)
	WriteLog("tbl_log_player",   uEventId, charId)
end
---------------------------------------------------------------------
--@brief 记录戒指信息
--@param charId:角色Id
--@param equipId:装备Id
function CLogMgrDB.SaveEquipRingInfo( charId,equipId,uBaseLevel,uCurLevel,uDPS,uStaticProValue,uIntensifyQuality,sIntenSoul)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存戒指信息
	WriteLog("tbl_log_item_ring", uEventId,equipId,uBaseLevel,uCurLevel,uDPS,uStaticProValue,uIntensifyQuality,sIntenSoul)
	WriteLog("tbl_log_player",   uEventId, charId)
end
---------------------------------------------------------------------
--@brief 记录盾牌或者法器产出信息
--@param charId:角色Id
--@param equipId:装备Id
function CLogMgrDB.SaveEquipShieldInfo( charId,equipId,uBaseLevel,uCurLevel,uIntensifyQuality,sIntenSoul)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存盾牌信息
	WriteLog("tbl_log_item_shield", uEventId,equipId,uBaseLevel,uCurLevel,uIntensifyQuality,sIntenSoul)
	WriteLog("tbl_log_player",   uEventId, charId)
end
---------------------------------------------------------------------
--@brief 记录盾牌信息
--@param charId:角色Id
--@param equipId:装备Id
function CLogMgrDB.SaveShieldInfo( charId,equipId,uAttrValue1,uAttrValue2,uAttrValue3,uAttrValue4)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存盾牌信息
	WriteLog("tbl_log_item_shield_Attr", uEventId,equipId,uAttrValue1 or 0,uAttrValue2 or 0,uAttrValue3 or 0,uAttrValue4 or 0)
	WriteLog("tbl_log_player",   uEventId, charId)
end
---------------------------------------------------------------------
--@brief 记录装备护甲信息
--@param charId:角色Id
--@param equipId:装备Id
function CLogMgrDB.SaveArmorIncrustationInfo( charId,equipId,sAttr,uAddTimes,uIndex)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_item_equip_armor", uEventId,equipId,sAttr,uAddTimes,uIndex)
	WriteLog("tbl_log_player",   uEventId, charId)
end
---------------------------------------------------------------------
--@brief 记录装备追加信息
--@param charId:角色Id
--@param equipId:装备Id
function CLogMgrDB.SaveEquipSuperAddationInfo( charId,equipId,uCurSuperaddPhase)
	local uEventId = CLogMgrDB._InsertEvent()
	
	WriteLog("tbl_log_item_equip_superaddation", uEventId,equipId,uCurSuperaddPhase)
	WriteLog("tbl_log_player",   uEventId, charId)
end
---------------------------------------------------------------------
--@brief 记录装备制造者信息
--@param uMakerId:制造者Id
--@param equipId:装备Id
function CLogMgrDB.SaveEquipMakerInfo(equipId,uMakerId)
	local uEventId = CLogMgrDB._InsertEvent()
	
	WriteLog("tbl_log_item_maker", uEventId,equipId,uMakerId)
end
-------------------------------------------[[死亡事件相关]]--------------------------------
--@brief 保存角色战斗信息
--@param AttackerId:击杀者角色Id
--@param BeAttackerId:被击杀者Id
--@param sceneName:场景名称
function CLogMgrDB.SavePlayerFightInfo( AttackerId, BeAttackerId,SceneName,attackerLevel,beAttackerLevel)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存击杀者
	WriteLog("tbl_log_player_kill",  uEventId, AttackerId, attackerLevel)
	--保存被击杀者
	WriteLog("tbl_log_player_dead", uEventId , BeAttackerId,beAttackerLevel)
	--保存角色死亡所在场景
	WriteLog("tbl_log_scene", uEventId , SceneName)
end
---------------------------------------------------------------------
local StmtDef = {
    	"_GetTheSameSceneRole",
    	[[ 
    		select cfp.cs_uId,ifnull(t_uId,0) from tbl_char_fb_position cfp left join tbl_member_tong mt on  cfp.cs_uId = mt.cs_uId
    		where sc_uId = (select sc_uId from tbl_char_fb_position where cs_uId = ?)
    	]]
}    
DefineSql ( StmtDef, StmtContainer )


--@brief 保存怪物死亡信息
--@param AttackerId:击杀者角色Id
--@param NpcName:Npc名称
--@param sceneName:场景名称
function CLogMgrDB.SaveNpcDeadInfo( AttackerId, NpcName,uCharLevel)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存击杀者
	WriteLog("tbl_log_player_kill", uEventId, AttackerId,uCharLevel)
	--保存Npc信息
	WriteLog("tbl_log_npc_name", uEventId , NpcName)
	local theSameSceneRole = StmtContainer._GetTheSameSceneRole:ExecStat(AttackerId)
	for i = 1,theSameSceneRole:GetRowNum() do
		WriteLog("tbl_log_join_activity", uEventId, theSameSceneRole(i,1) ,theSameSceneRole(i,2))
	end
end
---------------------------------------------------------------------
--@brief 保存角色被怪物杀死事件信息
--@param NpcName:npc名称
--@param uCharId:死亡角色Id
--@param SceneName:场景名称
function CLogMgrDB.SavePlayerKilledByNpc(NpcName, uCharId, SceneName,uCharLevel)
	local uEventId = CLogMgrDB._InsertEvent()
	--保存NPC
	WriteLog("tbl_log_npc_name", uEventId, NpcName)
	--保存被击杀者
	WriteLog("tbl_log_player_dead", uEventId , uCharId,uCharLevel)
	--保存角色死亡所在场景
	WriteLog("tbl_log_scene", uEventId , SceneName)
end
---------------------------------------【战斗信息相关】---------------------------------------
local StmtDef=
{
	"_GetCharLevel",
	[[
		select cb_uLevel from tbl_char_basic where cs_uId = ? 
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetCharSeries",
	[[
		select ss_uSeries from tbl_skill_Series where cs_uId = ? 
	]]
}
DefineSql(StmtDef,StmtContainer)
	
local StmtDef=
{
	"_GetCharFightPoint",
	[[
		select cfe_uPoint from tbl_char_fighting_evaluation where cs_uId = ? 
	]]
}
DefineSql(StmtDef,StmtContainer)

--@brief 根据玩家id获得写log需要的等级、天赋系、战斗力评分
function CLogMgrDB.GetCharSaveLogInfo(charId)
	local levelRet = StmtContainer._GetCharLevel:ExecStat(charId)
	local charSeries = StmtContainer._GetCharSeries:ExecStat(charId)
	local fightPoint = StmtContainer._GetCharFightPoint:ExecStat(charId)
	local uSeries = 0
	if charSeries:GetRowNum() > 0 then
		uSeries = charSeries(1,1)
	end
	return {levelRet(1,1),uSeries,fightPoint(1,1)}
end
---------------------------------------------------------------------
--@brief 记录角色pk事件
--@param uFlagId:旗子id
--@param uResponsesId:插旗者id
--@param uChallengeId:挑战者id
--@param uCostTime：战斗时长
--@param uIsSucc：PK是否成功 0-失败，1-成功
function CLogMgrDB.SaveCharPKInfoLog(uFlagId,uResponsesId,uChallengeId,uCostTime,uIsSucc,fightInfoTbl1,fightInfoTbl2)
	local uEventId = CLogMgrDB._InsertEvent()
	
	WriteLog("tbl_log_char_pk",uEventId, uFlagId,uResponsesId,uChallengeId,uCostTime,uIsSucc)
	WriteLog("tbl_log_char_fight_info",uEventId, unpack(fightInfoTbl1))
	WriteLog("tbl_log_char_fight_info",uEventId, unpack(fightInfoTbl2))
end
---------------------------------------------------------------------
--@brief 记录3V3比赛\角斗场\鲜血试炼场\剧情本\地下城等队伍活动相关信息
--@param uSuccTeamId：成功队伍id
--@param uFailTeamId:失败队伍id
--@param uCostTime:活动、比赛总耗时
--@param uActivityType:活动类型 1-3V3比赛\2-角斗场\3-鲜血试炼场\4-剧情本\5-地下城....
function CLogMgrDB.SaveTeamActivityLogInfo(uSuccTeamId,uFailTeamId,uCostTime,uActivityType)
	local uEventId = CLogMgrDB._InsertEvent()
	
	WriteLog("tbl_log_team_activity",uEventId,uSuccTeamId,uFailTeamId,uCostTime,uActivityType)
end
-----------------------------------------[[天赋相关]]---------------------------------
--@brief 保存天赋学习信息
--@param charId:玩家id
--@param talentName:天赋名称
--@param uType:1-学天赋；2洗天赋
function CLogMgrDB.SavePlayerTelentInfo( charId,talentName,uType )
	local uEventId = CLogMgrDB._InsertEvent()
	--保存天赋学习角色Id
	WriteLog("tbl_log_player", uEventId, charId )
	--保存Npc信息
	WriteLog("tbl_log_talent", uEventId , talentName,uType)
end
-----------------------------------------[[技能相关]]---------------------------------
--@brief 技能学习和升级信息
--@param uType:0代表学习，1代表升级 
--@IsNonSkill:0 代表是战斗技能 、1 代表非战斗技能
function CLogMgrDB.SavePlayerSkillInfo( charId,skillName,skillLevel,uType,uIsNonSkill)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player_giver", uEventId, charId )
	WriteLog("tbl_log_skill",uEventId , skillName,skillLevel,uType,uIsNonSkill )
	return uEventId
end
------------------------------------------[生活技能]----------------------------------------
--@brief 生活技能信息记录
--@param charId:玩家Id
--@param skillName:技能名称
--@param uSkillLevel:技能等级
--@param uExperience:技能经验
--@param uUpdateType:操作类型
function CLogMgrDB.SaveLiveskillInfo(charId,skillName,uSkillLevel,uExperience,uUpdateType)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type", uEventId ,uUpdateType )
	WriteLog("tbl_log_commerce_skill", uEventId, charId,skillName,uSkillLevel,uExperience)
	return uEventId
end
----------------------------------------------------------------------------------
--@brief 锻造熟练度信息记录
--@param charId:玩家Id
--@param skillType:技能类型
--@param uType:武器熟练度类型
--@param uSpecialize:熟练度大小
--@param uUpdateType:操作类型
function CLogMgrDB.SaveLiveskillPractice(charId,skillType,uType,uSpecialize,uUpdateType,uEventId)
	local uEventId = uEventId or CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type", uEventId ,uUpdateType )
	WriteLog("tbl_log_specialize_smithing", uEventId, charId,skillType,uType,uSpecialize)
	return uEventId
end
----------------------------------------------------------------------------------
--@brief 锻造专精信息记录
--@param charId:玩家Id
--@param skillType:技能类型
--@param uType:武器专精类型
--@param uLevel:专精等级
--@param uUpdateType:操作类型
function CLogMgrDB.SaveLiveskillExpert(charId,nSkillType,sExpertType,uLevel,uUpdateType)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type", uEventId ,uUpdateType )
	WriteLog("tbl_log_expert_smithing",uEventId, charId,nSkillType,sExpertType,uLevel)
	return uEventId
end
--------------------------------------[[重生]]------------------------------------
--@brief 复活
--@param 相关角色
--@param 复活方式：1,复活石复活; 2,新手复活; 3,回复活点复活;4,进入观察模式; 5,在副本的进入点复活; 6,立即复活;
function CLogMgrDB.LogReBorn(data)
	local charId = data["PlayerId"]
	local uRebornMethod = data["RebornMethod"]
	local sceneName = data["sceneName"]
	local res = data["res"]
	
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player", uEventId, charId)
	WriteLog("tbl_log_reborn_info", uEventId , uRebornMethod )
end
--------------------------------------[[合成]]----------------------------------------
--@brief 合成
--@brief 相关角色
--@param 新物品名称
function CLogMgrDB.LogCompose(charId)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player", uEventId, charId)
	WriteLog("tbl_log_compose_info", uEventId )
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["合成"])
	return uEventId
end
-----------------------------------[[副本相关]]-------------------------------------
local SceneTypeTbl = {
			[5] = true,
			[8] = true,
			[10] = true,
			[13] = true,
			[14] = true,
			[15] = true,
			[16] = true,
			[18] = true,
			[19] = true,
			[20] = true,
			[21] = true,
			[22] = true,
			[23] = true,
			[24] = true,
			[26] = true,
			[27] = true,
			}
			
function CLogMgrDB.EnterScene(data)
	local charId = data["PlayerId"]
	local SceneName = data["SceneName"]
	local curlevel = data["curlevel"]
	local SceneType = data["SceneType"]
	local SceneID = data["SceneID"]
	local uEventId = CLogMgrDB._InsertEvent()

	if SceneTypeTbl[SceneType] then
		local ActionName = SceneName
		if SceneType == 13 then
			ActionName = data["MatchGameName"]
		end
		CLogMgrDB.EnterActivity(uEventId,SceneID, ActionName)  --玩家参加副本活动
	end
	WriteLog("tbl_log_enter_scene",uEventId)
	WriteLog("tbl_log_level",uEventId , curlevel)
	WriteLog("tbl_log_scene",uEventId , SceneName)
	WriteLog("tbl_log_player",uEventId, charId)
end

--退出副本事件
function CLogMgrDB.LeaveScene(data)
	local PlayerId = data["PlayerId"]
	local SceneName = data["SceneName"]
	local SceneType = data["SceneType"]
	local uEventId = CLogMgrDB._InsertEvent()
	if SceneTypeTbl[SceneType] then
		local sActivityName = SceneName
		if SceneType == 13 then
			sActivityName = data["MatchGameName"]
		end
		CLogMgrDB.LeaveActivity(uEventId,data["SceneID"], sActivityName)
	end
	WriteLog("tbl_log_leave_scene",uEventId)
	WriteLog("tbl_log_scene",uEventId , SceneName)
	WriteLog("tbl_log_player",uEventId, PlayerId)
end

function CLogMgrDB.CreateScene(SceneId, SceneName)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_create_scene", uEventId, SceneId)
	WriteLog("tbl_log_scene", uEventId, SceneName)
end

-----------------------------------[[matchgame相关]]-----------------------------------
function CLogMgrDB.SaveMatchGameServerLog(data)
	local RoomId = data["RoomId"]
	local ActionName = data["ActionName"]
	local SceneId = data["SceneId"]
	local LogStr = data["LogStr"]
	local lenth = string.len(LogStr)
	
	if lenth <= 1024 then
		WriteLog("tbl_log_matchgame_server", RoomId, ActionName, SceneId, 1, os.date("%y-%m-%d %H:%M:%S"), LogStr)
	else	--信息过长，拆分成多条保存
		local index = 1
		for i=1, lenth, 1024 do
			local str = string.sub(LogStr, i, i+1023)
			WriteLog("tbl_log_matchgame_server", RoomId, ActionName, SceneId, index, os.date("%y-%m-%d %H:%M:%S"), str)
			index = index + 1
		end
	end
end

function CLogMgrDB.SaveMatchGameRoomLog(ActionName, RoomId, SceneId, ServerId)
	WriteLog("tbl_log_matchgame_room", RoomId, ActionName, SceneId, ServerId, os.date("%y-%m-%d %H:%M:%S"))
end

function CLogMgrDB.SaveMatchGameMemberLog(CharId, ActionName, TeamId, RoomId, FuncName, State)
	WriteLog("tbl_log_matchgame_member", CharId, ActionName, TeamId, RoomId, FuncName, State, os.date("%y-%m-%d %H:%M:%S"))
end

function CLogMgrDB.SaveXianxieLog(data)
	local uEventId = CLogMgrDB._InsertEvent()
	for id, v in pairs(data) do
		local isWin, score, killNum, deadNum = unpack(v)
		WriteLog("tbl_log_xianxie", uEventId, id, isWin and 1 or 0, score, killNum, deadNum)
	end
end

-----------------------------------[[修行塔相关]]-----------------------------------
--@brief 记录玩家进修行塔的log
--@param uCharId：角色id
--@param uLevelNum：修行塔层数 
--@param uGateId：关卡id
--@param uSceneId：场景id
--@param uSucceedFlag：成功与否的标识  1--成功；2--失败；3--时间到； 4--放弃；5--传送出去
--@param uSpendTimes:每一关所花费的时间
		
function CLogMgrDB.SaveXiuXingTaLogInfo(data)
	local uCharId = data.PlayerID
	local uLevelNum = data.LevelNum
	local uGateId = data.GameID
	local uSceneId = data.SceneID
	local uSucceedFlag = data.uSucceedFlag
	local uSpendTimes = data.uSpendTimes
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_xiuxingta",uEventId,uGateId,uSceneId,uSucceedFlag,uSpendTimes,tostring(uLevelNum))
	WriteLog("tbl_log_player_taker", uEventId , uCharId)
end
--------------------------------[[组队相关]]-----------------------------------------
function CLogMgrDB.EnterTeam(charId, uTeamId,CurLevel)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player",uEventId, charId)
	WriteLog("tbl_log_enter_team",uEventId , uTeamId)
	WriteLog("tbl_log_level",uEventId , CurLevel)
end
----------------------------------------------------------------
function CLogMgrDB.LeaveTeam(charId, uTeamId)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player",uEventId, charId)
	WriteLog("tbl_log_leave_team",uEventId , uTeamId)
end
-------------------------------服务器活动相关--------------------------------
function CLogMgrDB.EnterActivity(uEventId, sActivityID, sActivityName)
	WriteLog("tbl_log_enter_activity",uEventId, sActivityID, sActivityName)
end

function CLogMgrDB.LeaveActivity(uEventId,sActivityID, sActivityName)
	WriteLog("tbl_log_leave_activity",uEventId,sActivityID, sActivityName)
end
----------------------------------------[[社会关系事件]]--------------------------------------
--@brief 添加好友事件记录
--@param uCharId:角色Id
--@param CurLevel：角色自己的当前等级
--@param friendId：好友Id
function CLogMgrDB.AddFriend(uCharId,CurLevel,friendId)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_level",uEventId , CurLevel)
	WriteLog("tbl_log_player",uEventId, uCharId)
	WriteLog("tbl_log_add_friend",uEventId, friendId)
end
---------------------------------------------------------------------------------------------
--@brief 删除好友事件记录
--@param uCharId:角色Id
--@param friendId：好友Id
function CLogMgrDB.DeleteFriend(uCharId,friendId)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player",uEventId, uCharId)
	WriteLog("tbl_log_delete_friend",uEventId, friendId)
end

----------------------------------------[[积分信息]]----------------------------------------
local StmtDef=
{
	"_SelectJiFenCode",

	"select sj_sCode from " .. LogDBName .. ".tbl_serial_jifen where lcs_uId = ?"

}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_SaveJiFenCode",

	"replace into " .. LogDBName .. ".tbl_serial_jifen set lcs_uId = ?, sj_sCode = md5(?)"

}
DefineSql(StmtDef,StmtContainer)

--@brief 记录积分信息
--@param charId：角色id
--@param uPoint：积分数
--@param uType：积分类型
--@param sSceneName：场景名称
function CLogMgrDB.SaveJiFenPointInfo(charId,uPoint,uType,sSceneName,uTotalPoint,eventId,modifyType,uLevel)
	if eventId == nil and modifyType == nil then
		LogErr("修改积分没有记录log类型")
	end
	local uEventId = eventId
	if not uEventId then
		uEventId = CLogMgrDB._InsertEvent()
		if uPoint > 0 then
			WriteLog("tbl_log_player_taker",uEventId, charId )
		else
			WriteLog("tbl_log_player_giver",uEventId, charId )
		end
		WriteLog("tbl_log_event_type",uEventId , modifyType)
	end
	WriteLog("tbl_log_level",uEventId , uLevel)
	local code_res = StmtContainer._SelectJiFenCode:ExecStat(charId)
	local code
	if code_res:GetRowNum() > 0 then
		code = code_res:GetData(0,0)
	else
		code = "tbl_log_jifenpoint"		
	end
	WriteLog("tbl_log_jifenpoint",uEventId ,charId, uPoint,uType,uTotalPoint,code)
	LogStmtCall(StmtContainer._SaveJiFenCode, charId,code)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
------------------------------------------------------------------------------------
--@brief 记录大逃杀积分信息
--@param charId：角色id
--@param uPoint：积分数
function CLogMgrDB.SaveDaTaoShaPointInfo(charId,uPoint)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player_taker",uEventId, charId )
	WriteLog("tbl_log_event_type",uEventId ,event_type_tbl["大逃杀加积分"])
	WriteLog("tbl_log_dataosha_point",uEventId, uPoint)
	return uEventId
end
-----------------------------------------------------------------------------------------------
local StmtDef = 
{
	"_GetALLServerOnlineUserInfo",
	[[
		select 
			tbl_server.s_uId, count(uo.us_uId) as userNum
		from 
			tbl_server left join tbl_user_online as uo on(tbl_server.s_uId = uo.uo_uOnServerId)
			group by tbl_server.s_uId 
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_GetALLSceneOnlineUserInfo",
	[[
		select 
			s.sc_uServerId,count(*) ,s.sc_sSceneName,s.sc_uType
		from 
			tbl_char_fb_position cfp,tbl_scene s,tbl_char_online co
		where 
			cfp.sc_uId = s.sc_uId  and cfp.cs_uId = co.cs_uId 
			group by cfp.sc_uId,s.sc_uServerId
		union
		select 
			s.sc_uServerId,count(*),s.sc_sSceneName,s.sc_uType
		from 
			tbl_char_position cp,tbl_scene s,tbl_char_online co
		where 
			cp.cs_uId not in (select cs_uId from tbl_char_fb_position) 
			and cp.sc_uId = s.sc_uId  and cp.cs_uId = co.cs_uId
			group by cp.sc_uId,s.sc_uServerId
	]]
}
DefineSql(StmtDef, StmtContainer)

--@brief 每个五分钟记录服务器在线人数情况
function CLogMgrDB.SaveServiceOnlineUserInfo()
	local uEventId = CLogMgrDB._InsertEvent()
	local res1 = StmtContainer._GetALLServerOnlineUserInfo:ExecStat()
	if res1:GetRowNum() > 0 then
		for i = 1,res1:GetRowNum() do
			WriteLog("tbl_log_service_online_num",uEventId,res1:GetData(i-1,0),res1:GetData(i-1,1) )
		end
	end
	local res2 = StmtContainer._GetALLSceneOnlineUserInfo:ExecStat()
		if res2:GetRowNum() > 0 then
		for i = 1,res2:GetRowNum() do
			WriteLog("tbl_log_service_scene_online_num",uEventId,res2:GetData(i-1,0),res2:GetData(i-1,1),res2:GetData(i-1,2),res2:GetData(i-1,3))
		end
	end
end
------------------------------------------【佣兵小队相关】-----------------------------------------
--@brief 记录佣兵小队静态信息
function CLogMgrDB.SaveTongStaticInfo(tongId,tongName,uCamp,uCharId)
	assert(tongId)
	assert(tongName)
	WriteLog("tbl_log_tong_static",tongId,tongName,os.date("%y-%m-%d %H:%M:%S"),uCamp,uCharId)
end
------------------------------------------------------
--@brief 记录佣兵小队荣誉信息
function CLogMgrDB.SaveTongHonorInfo(tongId,uHonor,event_type)
	assert(event_type,"小队荣誉修改没写log")
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type",  uEventId , event_type)
	if uHonor > 0 then
		WriteLog("tbl_log_tong_taker",uEventId, tongId )
		WriteLog("tbl_log_tong_honor",uEventId, uHonor )
	end
end
------------------------------------------------------
--@brief 记录佣兵小队等级信息
function CLogMgrDB.SaveTongLevelInfo(tongId,uLevel, event_type)
	assert(event_type,"小队等级修改没写log")
	local uEventId = CLogMgrDB._InsertEvent()
	if uLevel > 0 then
		WriteLog("tbl_log_event_type",  uEventId , event_type)
		WriteLog("tbl_log_tong_taker",uEventId, tongId )
		WriteLog("tbl_log_tong_level",uEventId, uLevel )
	end
end
------------------------------------------------------
--@brief 记录佣兵小队资金信息
function CLogMgrDB.SaveTongMoneyInfo(tongId,uMoney,event_type)
	assert(event_type,"小队金钱修改没写log")
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type",  uEventId , event_type)
	if uMoney >= 0 then
		WriteLog("tbl_log_tong_taker",uEventId, tongId )
	else
		WriteLog("tbl_log_tong_giver",uEventId, tongId )
	end
	WriteLog("tbl_log_tong_money",uEventId, uMoney)
end
------------------------------------------------------
--@brief 记录佣兵小队资源信息
function CLogMgrDB.SaveTongResourceInfo(tongId,uResource,event_type)
	assert(event_type,"小队资源修改没写log")
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type",  uEventId , event_type)
	if uResource >= 0 then
		WriteLog("tbl_log_tong_taker",uEventId, tongId)
	else
		WriteLog("tbl_log_tong_giver",uEventId, tongId)
	end
	WriteLog("tbl_log_tong_resource",uEventId, uResource)
end
------------------------------------------------------
--@brief 记录佣兵小队发展度信息
--@param tongId:发展度改变的佣兵小队id
--@param uPoint:改变的发展度
--@param event_type：改变类型
function CLogMgrDB.SaveTongDevelopDegreeInfo(tongId,uPoint,event_type)
	assert(event_type,"小队发展度修改没写log")
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type",  uEventId , event_type)
	if uPoint >= 0 then
		WriteLog("tbl_log_tong_taker",uEventId, tongId)
	else
		WriteLog("tbl_log_tong_giver",uEventId, tongId)
	end
	WriteLog("tbl_log_tong_develop_degree",uEventId, uPoint)
end
------------------------------------------------------
--@brief 佣兵小队人事事件
--@param tongId:佣兵小队id
--@param uEventType:事件类型 1-加入;2-踢出;3-退出;4-更改职位
--@param uExecutorId:执行者
--@param uObjectId:被执行者
--@param uJobType:职位
function CLogMgrDB.TongMemberEventLog(tongId,uEventType,uExecutorId,uObjectId,uJobType)
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_tong_member_event",  uEventId , tongId,uEventType,uExecutorId,uObjectId,uJobType)
end
------------------------------------------------------
--@brief 佣兵小队科技事件
--@param tongId:佣兵小队id
--@param uEventType:事件类型 1-开始研发;2-停止研发;3-完成研发;4-科技重置(学院被打爆或者拆除)；5--科技学习
--@param uExecutorId:执行者
--@param sName:科技名称
--@param uLevel:科技当前等级
function CLogMgrDB.TongTechnologyEventLog(tongId,uEventType,uExecutorId,sName,uLevel)
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_tong_technology_event",  uEventId , tongId,uEventType,uExecutorId,sName,uLevel)
end
------------------------------------------------------
--@brief 佣兵小队建筑事件
--@param tongId:建筑所属佣兵小队id
--@param sName:建筑名称
--@param uLevel:建筑当前等级
--@param uEventType:事件类型 1-放置建筑;2-拆除摧毁建筑
--@param uExecutorId:执行者
--@param uExecutorTongId:执行者佣兵小队id
--@param uBuildingId:建筑id
--@param uAddParam:附加参数(血量百分比/目标等级/摧毁时掉落资源)
function CLogMgrDB.TongBuildingEventLog(tongId,sName,uLevel,uEventType,uExecutorId,uExecutorTongId,uBuildingId,uAddParam)
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_tong_building_event",  uEventId,tongId,sName,uLevel,uEventType,uExecutorId,uExecutorTongId,uBuildingId,uAddParam)
end
------------------------------------------------------
--@brief 佣兵小队物品制造事件
--@param tongId:佣兵小队id
--@param sName:物品名称
--@param uEventType:事件类型 1-开始;2-取消 3-完成制造
--@param uExecutorId:执行者
function CLogMgrDB.TongItemProduceEventLog(tongId,sName,uEventType,uExecutorId)
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_tong_item_produce",  uEventId,tongId,sName,uEventType,uExecutorId)
end
------------------------------------------------------
--@brief 佣兵小队转类型事件
--@param tongId:佣兵小队id
--@param uNewType:新类型
--@param uOldType:原类型
--@param uExecutorId:执行者
function CLogMgrDB.TongChangeTypeLog(tongId,uNewType,uOldType,uExecutorId)
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_tong_change_type",  uEventId,tongId,uNewType,uOldType,uExecutorId)
end
------------------------------------------------------
--@brief 佣兵小队仓库存取事件
--@param tongId:佣兵小队id
--@param uType:仓库物品操作类型 1-领取;2-捐献
--@param itemId:物品id
--@param uExecutorId:执行者
--@param uExecutorPosition:执行者职位
function CLogMgrDB.DepotEventLog(tongId,uType,itemId,uDepotId,uExecutorId,uExecutorPosition)
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_tong_depot",  uEventId,tongId,uType,itemId,uDepotId,uExecutorId,uExecutorPosition)
end
------------------------------------------------------
--@brief 记录佣兵小队解散信息
--@param tongId:佣兵小队id
--@param uCharId:执行者
function CLogMgrDB.BreakTongLog(tongId,uCharId)
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_tong_break",  uEventId , tongId,uCharId)
end
------------------------------------------------------
--@brief 记录佣兵小队驻地迁移log信息
--@param tongId:佣兵小队id
--@param uCharId:执行者
--@param uEventType:迁移类型(1-挑战占领，2-挑战被占领，3-国战占领，4-国战被占领，5-主动撤退，6-帮会解散撤退，7-挑战放弃)
--@param uOldWarzoneId:老战区id
--@param uOldIndex:老驻地id
--@param uNewWarzoneId:新战区id
--@param uNewIndex:新驻地id
function CLogMgrDB.TongStationMoveLog(tongId,uCharId,uEventType,uOldWarzoneId,uOldIndex,uNewWarzoneId,uNewIndex)
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_tong_station_move",  uEventId , tongId,uCharId,uEventType,uOldWarzoneId,uOldIndex,uNewWarzoneId,uNewIndex)
end
------------------------------------------------------
--@brief 记录佣兵小队挑战开始Log信息
--@param uCharId:发起挑战者id
--@param uChallengeTongId:挑战佣兵小队id
--@param uRecoveryTongId:防御佣兵小队id
--@param uWarzoneId:战区id
--@param uIndex:驻地id
function CLogMgrDB.TongChallengeStartLog(uCharId,uChallengeTongId,uRecoveryTongId,uWarzoneId,uIndex)
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_tong_challenge_start",  uEventId,uCharId,uChallengeTongId,uRecoveryTongId,uWarzoneId,uIndex)
end
------------------------------------------【佣兵团相关】-----------------------------------------
--@brief 记录佣兵团静态Log信息
--@param uArmyCorpsId:团id
--@param sName:团名称
--@param uCharId:创建者id
--@param uCamp:团阵营
function CLogMgrDB.ArmyCorpsStaticInfoLog(uArmyCorpsId,sName,uCharId,uCamp)
	WriteLog("tbl_log_army_corps", uArmyCorpsId,sName,uCharId,uCamp,os.date("%y-%m-%d %H:%M:%S"))
end
------------------------------------------------------
--@brief 记录佣兵团解散log信息
--@param uArmyCorpsId:团id
--@param uCharId:团长id
function CLogMgrDB.ArmyCorpsBreakInfoLog(uArmyCorpsId,uCharId)
	local uEventId = CLogMgrDB._InsertEvent()
	
	WriteLog("tbl_log_army_corps_break", uEventId,uArmyCorpsId,uCharId,os.date("%y-%m-%d %H:%M:%S"))
end

------------------------------------------------------
--@brief 记录佣兵团人事事件
--@param uArmyCorpsId:团id
--@param uEventType:1-加入;2-踢出;3-更改
--@param uExecutorId:执行者id
--@param uObjectId:被执行者id
--@param uJobType:职位
function CLogMgrDB.ArmyCorpsMemberEventLog(uArmyCorpsId,uEventType,uExecutorId,uObjectId,uJobType)
	local uEventId = CLogMgrDB._InsertEvent()
	
	WriteLog("tbl_log_army_corps_member_event", uEventId,uArmyCorpsId,uEventType,uExecutorId,uObjectId,uJobType)
end
------------------------------------------------------
--@brief 记录玩家主动下线场景、区域(坐标)
--@param charId：角色id
--@param sSceneName：下线所在场景
--@param sArgNameName:下线所在区域
--@param uPosX,uPosY:下线的坐标
function CLogMgrDB.SaveCharLogoutPosition(charId,sSceneName,sArgNameName,uPosX,uPosY)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_player_logout_position",uEventId, charId,sSceneName,sArgNameName,uPosX,uPosY) 
end
----------------------------------------------------
function CLogMgrDB.SetLatencyToDB(data)
	local player_id, latency = data["char_id"], data["latency"]
	WriteLog("tbl_log_player_latency",player_id, latency,os.date("%y-%m-%d %H:%M:%S")) 
end
-----------------------------------------------------
--@brief 种花
function CLogMgrDB.CultivateFlowersLog()
	local uEventId = CLogMgrDB._InsertEvent()
	
	WriteLog("tbl_log_cultivate_flowers",uEventId) 
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["种花"])
	return uEventId
end
-----------------------------------------------------
--@brief 掉落物品拍卖过程
function CLogMgrDB.DropItemVendueLog()
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_item_vendue",uEventId) 
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["掉落物品拍卖过程"])
	return uEventId
end
-----------------------------------------------------
--@brief 掉落物品拍卖结果
function CLogMgrDB.DropItemVendueEndLog()
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_item_vendue_end",uEventId) 
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["掉落物品拍卖结果"])
	return uEventId
end
-----------------------------------------------------
--@brief 离线经验兑换
function CLogMgrDB.ExpChangeLog()
	local uEventId = CLogMgrDB._InsertEvent()

	WriteLog("tbl_log_exp_change",uEventId) 
	WriteLog("tbl_log_event_type",uEventId , event_type_tbl["离线经验兑换"])
	return uEventId
end	
-----------------------------------------------------	
--@brief 保存战斗力总评分
function CLogMgrDB.SaveFightingEvaluation(uCharId,uPoint,uLevel)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_char_fighting_evaluation",uEventId,uCharId,uPoint) 
	WriteLog("tbl_log_level",uEventId , uLevel)
end
------------------------------------------------------
--@brief 佣兵小队抢夺资源点报名
--@param tongId:参加报名的小队id
--@param uCharId：报名的队长或者副队长id
--@param uExploit：报名时小队的声望
--@param sObjName：报名的资源点名称
function CLogMgrDB.SaveTongResourceSigUp(tongId,uCharId,uExploit,sObjName)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_tong_resource_sig_up",uEventId,tongId,uCharId,uExploit,sObjName) 
end
-------------------------------------------------------
--@brief 佣兵小队抢夺资源点获胜
--@param tongId:获胜小队id
function CLogMgrDB.SaveTongRobResourceSucc(tongId)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type",  uEventId , event_type_tbl["抢夺资源点获胜"])
	WriteLog("tbl_log_tong_taker",uEventId, tongId ) 
end
-------------------------------------------------------
--@brief 点券信息记录
--@param couponsId:点券id
--@param sName:点券名称
--@param uSmallIcon：点券图标
--@param uPrice：价钱
--@param sDesc：描述
function CLogMgrDB.SaveCouponsInfoLog(couponsId,sName,uSmallIcon,uPrice,sDesc,sUrl)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_coupons_info",  uEventId , couponsId,sName,uSmallIcon,uPrice,sDesc,sUrl)
end
-------------------------------------------------------
--@brief 记录玩家购买点券信息
--@param uCharId:玩家id
--@param couponsId:点券id
--@param sSequenceId：点券对应的序列号
function CLogMgrDB.SaveCharBoughtCouponsLog(uEventId,uCharId,couponsId,sSequenceId)
	WriteLog("tbl_log_char_bought_coupons",uEventId, uCharId,couponsId,sSequenceId ) 
end
------------------------------------------------------
--@brief 记录玩家领取军资log
--@param uCharId:领取军资的角色id
--@param uResource:领取军资的数量
function CLogMgrDB.SavePlayerDrawResourceInfoLog(uCharId,uResource)
	local uEventId = CLogMgrDB._InsertEvent()
	WriteLog("tbl_log_event_type",  uEventId , event_type_tbl["领取军资"])
	WriteLog("tbl_log_player_taker",uEventId, uCharId)
	WriteLog("tbl_log_tong_resource",uEventId, uResource)
end

SetDbLocalFuncType(CLogMgrDB.LeaveScene)
SetDbLocalFuncType(CLogMgrDB.EnterScene)
SetDbLocalFuncType(CLogMgrDB.LogReBorn)
return CLogMgrDB
