cfg_load "item/VipItem_Common"

local VipItem_Common = VipItem_Common
local event_type_tbl = event_type_tbl
local GetCfgTransformValue = GetCfgTransformValue
local g_StoreRoomIndex = g_StoreRoomIndex

local UseVIPItemDB = CreateDbBox(...)

local StmtContainer = {}	
-----------------------------------------------------------
local StmtDef=
{
	"_SaveUseVIPItemTime",
	"replace into tbl_char_vip_effect values(?,now(),?)"
}
DefineSql(StmtDef,StmtContainer)

--@brief 使用VIP道具
function UseVIPItemDB.UseVIPItem(data)
	local uCharId = data["PlayerID"]
	local ItemName = data["ItemName"]
	local uRoomIndex = data["RoomIndex"]
	local uPos = data["Pos"]
	local uLevel = data["uLevel"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local param = {["char_id"] = uCharId}
	local uLeftTime = UseVIPItemDB.GetVIPEffect(param)
	if uLeftTime ~=  0 then
		return false,410029
	end
	
	local sItemType, sItemName, uItemId = g_RoomMgr.GetOneItemByPosition(uCharId, uRoomIndex, uPos)
	if sItemType  == nil or sItemName == nil or uItemId == nil or sItemType ~= 51  then
		return false
	end
	
	local DelRet = g_RoomMgr.DelItemByPos(uCharId, uRoomIndex, uPos,1,event_type_tbl["物品使用"])
	if IsNumber(DelRet) then
		return false
	end
	
	local ItemInfoTbl = GetCfgTransformValue(true, "VipItem_Common", sItemName, "ItemInfo")     --选择物品
	for i,p in pairs(ItemInfoTbl) do 
		if uLevel >= p[1] and uLevel <= p[2] then
			local params = {}
			params.nCharID = uCharId
			params.nStoreMain = g_StoreRoomIndex.PlayerBag
			params.nBigID = p[3]
			params.nIndex = p[4]
			params.nCount = p[5]
			params.createType = event_type_tbl["使用VIP道具获得物品"]
			local res = g_RoomMgr.AddItem(params)
			if IsNumber(res) then
				CancelTran()
				return false,res
			end
		end
	end
	StmtContainer._SaveUseVIPItemTime:ExecStat(uCharId,sItemName)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		CancelTran()
		return false
	end
	return true
end
------------------------------------------------------------
local StmtDef=
{
	"_GetUseVIPItemTime",
	"select 14*24*60 - (unix_timestamp(now()) - unix_timestamp(cve_dtUseTime))/60   from tbl_char_vip_effect where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_DelUseVIPItemTime",
	"delete from tbl_char_vip_effect where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

--@brief 查询VIP特效状态剩余时间
function UseVIPItemDB.GetVIPEffect(data)
	local uCharId = data["char_id"]
	local res = StmtContainer._GetUseVIPItemTime:ExecStat(uCharId)
	if res:GetRowNum() == 0 then
		return 0
	else
		local leftTime = res:GetData(0,0)
		if leftTime <= 0 then
			StmtContainer._DelUseVIPItemTime:ExecStat(uCharId)
			return 0
		else
			return leftTime
		end 
	end
end
-----------------------------------------------------------
local StmtDef=
{
	"_GetAllVIPRole",
	[[
		select 
			cve.cs_uId, 14*24*60 - (unix_timestamp(now()) - unix_timestamp(cve_dtUseTime))/60 as left_time,cb_uLevel,cve_sItemName 
		from 
			tbl_char_vip_effect cve,tbl_char_basic cb 
		where 
			cve.cs_uId = cb.cs_uId
	]]
}
DefineSql(StmtDef,StmtContainer)



local StmtDef=
{
	"_DelOutTimeVIPRole",
	[[
		delete from  tbl_char_vip_effect where 14*24*60 - (unix_timestamp(now()) - unix_timestamp(cve_dtUseTime))/60 <=0
	]]
}
DefineSql(StmtDef,StmtContainer)


function UseVIPItemDB.SendMailItemToVip()
	StmtContainer._DelOutTimeVIPRole:ExecStat()
	local res = StmtContainer._GetAllVIPRole:ExecStat()
	if res:GetRowNum() > 0 then
		local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
		local sysMailExecutor = RequireDbBox("SysMailDB")
		for m = 1,res:GetRowNum() do
			local left_time = res(m,2)
			if left_time > 0 then
				local uLevel = res(m,3)
				local sItemName = res(m,4)
				local ItemInfoTbl = GetCfgTransformValue(true, "VipItem_Common", sItemName, "MailItemInfo")     --选择物品
				local AddItemsTbl = {}
				for i,p in pairs(ItemInfoTbl) do 
					if uLevel >= p[1] and uLevel <= p[2] then
						local params= {}
						params.m_nType = p[3]
						params.m_sName = p[4]
						params.m_nBindingType = p[6]
						params.m_nCharID = res(m,1)
						params.m_sCreateType = event_type_tbl["VIP邮件物品"]
						for j = 1, p[5] do
							local itemID = g_RoomMgr.CreateItem(params)
							table.insert(AddItemsTbl,{itemID})
						end
					end
				end
				if next(AddItemsTbl) then
					sysMailExecutor.SendSysMail(1014,res(m,1), 1029, 1030, AddItemsTbl, 0,event_type_tbl["VIP邮件"],5009)
				end
			end
		end
	end
end
-----------------------------------------------------------

local StmtDef=
{
	"_SaveWelfareRole",
	"replace into tbl_char_welfare values(?,?)"
}
DefineSql(StmtDef,StmtContainer)

--@brief 保存有福利buff的玩家
function UseVIPItemDB.SaveWelfareRole(uCharId,buffName)
	StmtContainer._SaveWelfareRole:ExecStat(uCharId,buffName)
	return g_DbChannelMgr:LastAffectedRowNum() > 0	
end
-----------------------------------------------------------
local vipBuffTbl = {
		[1] = "G级佣兵福利",
		[2] = "F级佣兵福利",
		[3] = "E级佣兵福利",
		[4] = "D级佣兵福利",
}

local StmtDef=
{
	"_GetWelfareRole",
	[[
		select 
			cw.cs_uId,cb_uLevel,cw_sBuffName 
		from 
			tbl_char_welfare cw,tbl_char_basic cb 
		where 
			cw.cs_uId = cb.cs_uId and cw_sBuffName = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

function UseVIPItemDB.SendWelfareMailItem(data)
	local WelfareLevel = data["WelfareLevel"]
	local res = StmtContainer._GetWelfareRole:ExecStat(vipBuffTbl[WelfareLevel])
	if res:GetRowNum() > 0 then
		local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
		local sysMailExecutor = RequireDbBox("SysMailDB")
		for m = 1,res:GetRowNum() do
			local uLevel = res(m,2)
			local sItemName = res(m,3)
			local ItemInfoTbl = GetCfgTransformValue(true, "VipItem_Common", sItemName, "MailItemInfo")     --选择物品
			local AddItemsTbl = {}
			for i,p in pairs(ItemInfoTbl) do 
				if uLevel >= p[1] and uLevel <= p[2] then
					local params= {}
					params.m_nType = p[3]
					params.m_sName = p[4]
					params.m_nBindingType = p[6]
					params.m_nCharID = res(m,1)
					params.m_sCreateType = event_type_tbl[vipBuffTbl[WelfareLevel]]
					for j = 1, p[5] do
						local itemID = g_RoomMgr.CreateItem(params)
						table.insert(AddItemsTbl,{itemID})
					end
				end
			end
			if next(AddItemsTbl) then
				sysMailExecutor.SendSysMail(1014,res(m,1), "1033_fuli:" ..  vipBuffTbl[WelfareLevel], "1034_fuli:" ..  vipBuffTbl[WelfareLevel], AddItemsTbl, 0,event_type_tbl[vipBuffTbl[WelfareLevel]],5010)
			end
		end
	end
end

return UseVIPItemDB


