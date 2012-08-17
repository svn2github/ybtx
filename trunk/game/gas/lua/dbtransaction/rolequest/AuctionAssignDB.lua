gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "framework/common/CMoney"
local g_ItemInfoMgr = CItemInfoMgr:new()
local g_MoneyMgr = CMoney:new()
local StmtContainer = class()
local event_type_tbl = event_type_tbl
local AuctionAssignDB = CreateDbBox(...)

local StmtDef = {
			"_ValidateAuctionMoney",
			[[ 
				insert into tbl_char_auction_money values(?,?,?,now())
			]]
}
DefineSql (StmtDef,StmtContainer)

local StmtDef = {
			"_UpdateAuctionMoney",
			[[ 
				update tbl_char_auction_money set cam_uMoney = ? where cs_uId = ? and cam_uObjId = ?
			]]
}
DefineSql (StmtDef,StmtContainer)

local StmtDef = {
			"_DeleteAuctionMoney",
			[[ 
				delete from tbl_char_auction_money where cs_uId = ? and cam_uObjId = ?
			]]
}
DefineSql (StmtDef,StmtContainer)

local StmtDef = {
			"_QueryAuctionMoney",
			[[ 
				select cam_uMoney from tbl_char_auction_money where cs_uId = ? and cam_uObjId = ?
			]]
}
DefineSql (StmtDef,StmtContainer)

local StmtDef = {
			"_QueryAllAuctionMoney",
			[[ 
				select cs_uId, cam_uObjId from tbl_char_auction_money where cam_dtAddDate < (now() - 1800);
			]]
}
DefineSql (StmtDef,StmtContainer)

local function AddMoney(PlayerId, Money, eventId, AddType)
--print(PlayerId, Money)
	local fun_info = g_MoneyMgr:GetFuncInfo("Quest")
	local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
--print(AddType, eventId)
	local bFlag, uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["MoneyAward"],PlayerId, Money, eventId, AddType)
	if not bFlag then
		return false,uMsgID
	end
	return true,Money
end

function AuctionAssignDB.AcutionAssignGiveItem(data)
	local TeamID = data["TeamID"]
	local CharId = data["char_id"]
	local EventId = data["EventId"]
	local teammgr = RequireDbBox("GasTeamDB")
	local TeamHeadId = teammgr.GetCaptain(TeamID)
	local SuccTbl = {}
	
	data["nCount"]	= 1
	data["uEventId"] = EventId
--print(48, EventId)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local res1 = CharacterMediatorDB.AddItem(data)
--	if IsNumber(res1) then
--		return {false,res1}
--	end
	local result = StmtContainer._QueryAuctionMoney:ExecStat(CharId,data["ObjID"])
	if result:GetRowNum() <= 0 then
		CancelTran()
		return {false, nil, MsgId}
	end
	
	local decMoney = result:GetData(0,0)
	local LogMgr = RequireDbBox("LogMgrDB")
	local TempEventId = LogMgr.DropItemVendueEndLog()
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local totalMoney = MoneyManagerDB.GetMoneyByType(CharId,1)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local player_level = CharacterMediatorDB.GetPlayerLevel(CharId)
	LogMgr.PlayerMoneyModify( TempEventId, 1, -1*decMoney,CharId,totalMoney,nil,player_level)
	
	StmtContainer._DeleteAuctionMoney:ExecStat(CharId,data["ObjID"])
	for _, char_id in pairs(data["AssignPlayerTbl"]) do 
		local addMoney = data["addmoney"]
--		if TeamHeadId == char_id then
--			addMoney = addMoney + data["SurplusMoney"]
--		end
		local res = AddMoney(char_id, addMoney, TempEventId, event_type_tbl["ÅÄÂôÆ½¾ù·ÖÇ®"])
		SuccTbl[char_id] = {res, addMoney}
		if data["sysFee"] > 0 then
			LogMgr.SaveTaxLog(data["sysFee"], char_id, event_type_tbl["ÅÄÂô·ÖÇ®¿ÛË°"])
		end
	end
	
	return {true, res1, SuccTbl}
end

function AuctionAssignDB.ValidateAuctionMoney(data)
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local Money = MoneyManagerDB.GetMoney(data["player_id"])
	local result3 = nil
	local PayMoney = data["Price"]
	local EventId = data["EventId"]
	
	if data["PreCharID"] then
		local result = StmtContainer._QueryAuctionMoney:ExecStat(data["PreCharID"],data["ObjID"])
		if result and result:GetRowNum() > 0 then
			local PrePrice = result:GetData(0,0)
			if data["PreCharID"] == data["player_id"] then
				PayMoney = PayMoney - PrePrice
				if Money < PayMoney then
					return false,{3997}
				end
				StmtContainer._UpdateAuctionMoney:ExecStat(data["Price"],data["player_id"],data["ObjID"])
				local res, MsgID = AddMoney(data["player_id"], -PayMoney, EventId, event_type_tbl["ÅÄÂô¾º¼Û²¹Ç®"])
				if not res then
					CancelTran()
					return false,{MsgID}
				end
				return true,PayMoney
			else
				StmtContainer._DeleteAuctionMoney:ExecStat(data["PreCharID"],data["ObjID"])
				if g_DbChannelMgr:LastAffectedRowNum() > 0 then
					local res, MsgID = AddMoney(data["PreCharID"], PrePrice, EventId, event_type_tbl["ÅÄÂô¾º¼Û·µÇ®"])
					if not res then
						CancelTran()
						result3 = {false, data["PreCharID"], MsgID}
					else
						result3 = {true, data["PreCharID"], PrePrice}
					end
				end
			end
		end
	end
	if not EventId then
		local LogMgr = RequireDbBox("LogMgrDB")
		EventId = LogMgr.DropItemVendueLog()
	end
	local result = StmtContainer._QueryAuctionMoney:ExecStat(data["player_id"],data["ObjID"])
	if result and result:GetRowNum() > 0 then
		CancelTran()
		return false, {3995}
	end
	if Money < PayMoney then
		CancelTran()
		return false, {3997}
	end
	local res, MsgID = AddMoney(data["player_id"], -PayMoney, EventId, event_type_tbl["ÅÄÂô¾º¼Û¿ÛÇ®"])
	if not res then
		CancelTran()
		return false, {MsgID}
	end
	StmtContainer._ValidateAuctionMoney:ExecStat(data["player_id"],data["ObjID"],PayMoney)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false, {3995}
	end
	return true, PayMoney, result3, EventId
end

function AuctionAssignDB.ReturnAuctionMoney(data)
	local result = StmtContainer._QueryAuctionMoney:ExecStat(data["CharID"],data["ObjID"])
	if result and result:GetRowNum() > 0 then
		local PrePrice = result:GetData(0,0)
		StmtContainer._DeleteAuctionMoney:ExecStat(data["CharID"],data["ObjID"])
		if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
			return
		end
		local res, MsgID = AddMoney(data["CharID"], PrePrice, data["EventId"], data["AddType"] or event_type_tbl["ÅÄÂôOBJÏûÊ§·µÇ®"])
		if not res then
			CancelTran()
			return
		end
		return data["CharID"], PrePrice
	end
end

function AuctionAssignDB.ReturnAllAuctionMoney()
	local result = StmtContainer._QueryAllAuctionMoney:ExecStat()
	local resultTbl = {}
	if result and result:GetRowNum() > 0 then
		local data = {}
		data["CharID"] = result:GetData(0,0)
		data["ObjID"] = result:GetData(0,1)
		data["AddType"] = 151
		local id,price = AuctionAssignDB.ReturnAuctionMoney(data)
		if id then
			table.insert(resultTbl,{id,price})
		end
	end
	return resultTbl
end

SetDbLocalFuncType(AuctionAssignDB.ReturnAuctionMoney)

return AuctionAssignDB
