gac_gas_require "item/item_info_mgr/ItemInfoMgr"

local StmtContainer = class()

g_ItemInfoMgr = g_ItemInfoMgr or CItemInfoMgr:new()
local g_ItemInfoMgr = g_ItemInfoMgr
local g_MoneyMgr = CMoney:new()
local event_type_tbl = event_type_tbl
local Stone_Common 				= Stone_Common
local HoleMaterial_Common = HoleMaterial_Common
local GetProcessTime 			= GetProcessTime
local LogErr 							= LogErr
local lPutItem 						= RequireDbBox("CPutItemToBagDB")
local StoneSqr 						= CreateDbBox(...)

local StmtDef=
{
	"_GetItemInfo",
	"select is_uType, is_sName from tbl_item_static where is_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_OpenHole",             --打孔
	"insert into tbl_stone_hole(cs_uId,sh_uPanel,sh_uDepart,sh_uSlot) values(?,?,?,?)"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_CheckHoleOpen",
	"select sh_uId from tbl_stone_hole where cs_uId=? and sh_uPanel=? and sh_uDepart=? and sh_uSlot=?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_AllOpenedHole",        --查询某面板，某部位已经打开的孔
 	[[ select sh_uId,sh_uSlot from tbl_stone_hole where cs_uId=? and sh_uPanel=? and sh_uDepart=?]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"_MaxOpenedNum",
	[[
		select 
			count(sh_uSlot) 
		from 
			tbl_stone_hole 
		where 
			cs_uId=? and sh_uPanel=? and sh_uDepart=?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
    "_CheckAllHoleOpen",
    [[
        select 
            sh_uId, sh_uPanel, sh_uDepart, sh_uSlot
        from 
            tbl_stone_hole 
        where 
            cs_uId=?
    ]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetHoleStone",
	[[
		select
			s.siu_uType, s.siu_sName, s.siu_bIsbind, s.sh_uId
		from 
			tbl_stone_hole as h, tbl_stone_in_use as s
		where 
			h.cs_uId = s.cs_uId and s.sh_uId=h.sh_uId and h.cs_uId=? and h.sh_uPanel=? 
			and h.sh_uDepart=? and sh_uSlot=?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
    "_AllHolesInlayed",
    [[
        select 
            s.sh_uId, h.sh_uPanel, h.sh_uDepart, sh_uSlot , s.siu_uType, s.siu_sName
        from
            tbl_stone_hole as h, tbl_stone_in_use as s
        where 
          h.cs_uId = s.cs_uId and s.sh_uId=h.sh_uId and h.cs_uId=?
    ]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_FindHoleId",
	"select sh_uId from tbl_stone_hole where cs_uId=? and sh_uPanel=? and sh_uDepart=? and sh_uSlot=?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{ 
	"_InlayStone",
	[[ insert into tbl_stone_in_use(cs_uId,sh_uId,siu_uType,siu_sName,siu_bIsbind) values(?,?,?,?,?) ]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = {
	"_GetUseFrame",      --获取材料或者物品的信息
	[[ select us_uPanel from tbl_stone_frame_use where cs_uId=? ]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = {
	"_UpdateUseFrame",
	[[ replace into tbl_stone_frame_use values (?, ?) ]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = {
	"_RemovalStone",
	[[ delete from tbl_stone_in_use where cs_uId = ? and sh_uId=? ]]
}
DefineSql(StmtDef,StmtContainer)
---------------------------------------------------------------------

function StoneSqr.GetAllHoleOpened(PlayerId)
	local OpenedHolesList = {}
    local opened_holes=StmtContainer._CheckAllHoleOpen:ExecSql('nnnn', PlayerId)

	local opened_holes_num = opened_holes:GetRowNum()

    for i=1, opened_holes_num do
        table.insert(OpenedHolesList, {opened_holes:GetNumber(i-1, 0), opened_holes:GetNumber(i-1, 1), opened_holes:GetNumber(i-1,
2), opened_holes:GetNumber(i-1, 3)})
    end
	
	opened_holes:Release()
	
	return OpenedHolesList
end

function StoneSqr.GetAllInlayedHolesInfo(PlayerId)
	local inlayed=StmtContainer._AllHolesInlayed:ExecSql('nnnnns[32]', PlayerId)
	return inlayed
end

function StoneSqr.GetNowPanel(player_id)
	local res = StmtContainer._GetUseFrame:ExecSql('n',player_id)
	local nowpanel = 1
	if(res:GetRowNum()~=0) then
		nowpanel= res:GetNumber(0,0)
	end
	res:Release()
	return nowpanel
end

function StoneSqr.ChangeFrame(data)
	local player_id = data["player_id"]
	local new_panel = data["new_panel"]
	local old_panel = data["old_panal"]
	if old_panel ~= StoneSqr.GetNowPanel(player_id) then
		return {false}
	end
	
	StmtContainer._UpdateUseFrame:ExecSql("", player_id, new_panel)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		return {false}
	end
	local ret = {true, old_panel, new_panel}
	ret["OpenedHolesList"] = StoneSqr.GetAllHoleOpened(player_id)
	ret["AllInlayedHolesInfo"] = StoneSqr.GetAllInlayedHolesInfo(player_id)
	return ret
end

local function MaxOpenedNum(player_id, panel, depart)
	local ret = StmtContainer._MaxOpenedNum:ExecSql("n", player_id, panel, depart)
	local MaxNum = ret:GetNumber(0,0)
	ret:Release()
	return MaxNum
end

local function CheckHoleOpened(player_id, panel, depart, slot)
	local ret = StmtContainer._CheckHoleOpen:ExecSql("n", player_id, panel, depart, slot)
	local num = ret:GetRowNum()
	ret:Release()
	return num > 0
end

local function OpenHole(player_id, panel, depart, slot)
	StmtContainer._OpenHole:ExecSql('',player_id,panel,depart,slot)
	return g_DbChannelMgr:LastAffectedRowNum()>0
end

function StoneSqr.OpenHole(data)
	local player_id = data["player_id"]
	local player_level = data["player_level"]
	local panel = data["panel"]
	local depart = data["depart"]
	local slot = data["slot"]
	local item_id = data["item_id"]
	if CheckHoleOpened(player_id, panel, depart, slot) then
		return {false, 7001}
	end
	local max_num = MaxOpenedNum(player_id, panel, depart)
	if (slot ~= max_num + 1) then
		return {false, 7002}
	end
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local stype,name=lRoomMgr.GetItemType(item_id)
	
	if((slot >= 2 and slot < 8) and (player_level < 20)) 
		or ((slot >= 4 and slot < 8) and player_level < 40)
		or ((slot >= 6 and slot < 8) and player_level < 60)
		or (slot == 8 and player_level < 70) then
		return {false, 7003}
	end
	
	if HoleMaterial_Common(name,"Level") ~= slot then
		return {false}
	end
	
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local res = lRoomMgr.GetRoomIndexAndPosByItemId(player_id, item_id)
	if not res then
		return {false}
	end
	
	if not OpenHole(player_id, panel, depart, slot) then
		CancelTran()
		return {false}
	end
	
	local bRet,uMsgId = lRoomMgr.DelItemByID(player_id, item_id,event_type_tbl["物品使用"])
	if not bRet then
		CancelTran()
		return {false,uMsgId}
	end

	local ret = {true, res(1,1), res(1,2)}
	ret["OpenedHolesList"] = StoneSqr.GetAllHoleOpened(player_id)
	ret["AllInlayedHolesInfo"] = StoneSqr.GetAllInlayedHolesInfo(player_id)
	ret["nowpanel"] = StoneSqr.GetNowPanel(player_id)
	return ret
end

--合成高等级的打孔材料或宝石
function StoneSqr.SynthesisItem(data)
	local tbl_item_id = data["tbl_item_id"]
	local player_id = data["player_id"]
	local room_index= data["room_index"]
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local item_num = 0
	local item_type, item_name

	for i=1,# tbl_item_id do
		local type, name = lRoomMgr.GetItemType(tbl_item_id[i])
		if item_type ~= nil then
			assert( item_type == type )
		end
		item_type = type
		
		if item_name ~= nil and item_name ~= name then
			LogErr("合成符石符文时名字不一致 ", item_name .. ":" .. name)
			return {false }
		end
		item_name = name
		item_num = item_num + 1
	end

	if item_type == nil or item_name == nil then --有可能发上来的Itemid不存在
		return {false }
	end

	local item_info = g_ItemInfoMgr:GetItemFunInfo(item_type, item_name)
	local parameter = {}
	local NextLevelName = item_info("NextLevelName")
	if NextLevelName == "" then
		return {false, 7006 }
	end
	
	if item_num < 4 then
		return {false }
	end

	local new_item_num=math.floor(item_num/4)
	local del_item_num=4*new_item_num
	
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local res = {}
	local itemBindingType = lRoomMgr.GetItemBindingTypeByID(tbl_item_id[1])
	if not itemBindingType then
		return {false}
	end	
	
	local single_ExpendMoney = item_info("ExpendMoney")
	if not single_ExpendMoney then
		LogErr("合成符石或符文时ExpendMoney为nil", item_name)
		return {false}
	end
	local ExpendMoney = single_ExpendMoney*new_item_num
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	if not MoneyManagerDB.EnoughMoney(player_id, ExpendMoney) then
		return {false, 7008 }
  end
	
	local LogMgr = RequireDbBox("LogMgrDB")
	local uEventId = LogMgr.LogCompose(player_id)
	
	local fun_info = g_MoneyMgr:GetFuncInfo("Stone")
	local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["SynthesisExpend"],player_id, -ExpendMoney,uEventId)
	if not bFlag then
		CancelTran() 
		if IsNumber(uMsgID) then
			return {false,uMsgID}
		end
		return {false}
	end
	
	
	for i=1,# tbl_item_id do
		local temp = lRoomMgr.GetRoomIndexAndPosByItemId(player_id, tbl_item_id[i])
		if temp == nil then
			CancelTran()
			return {false}
		end
		table.insert(res, {temp(1,1),temp(1,2),tbl_item_id[i]})
		local bRet,uMsgId = lRoomMgr.DelItemByID(player_id, tbl_item_id[i],nil,uEventId)
		if not bRet then
			CancelTran()
			return {false,uMsgId}
		end
	end
	local new_itemid_tbl = {}
	local new_info = {}
		local params= {}
			params.m_nType = item_type
			params.m_sName = NextLevelName
			params.m_nBindingType = itemBindingType
			params.m_nCharID = player_id
			params.m_nEventID = uEventId
	for i = 1, new_item_num do
		local new_item_id=lRoomMgr.CreateItem(params)
		table.insert(new_itemid_tbl,{new_item_id})
		local new_item={new_item_id,item_type,NextLevelName}
		table.insert(new_info,new_item)
	end
--	if IsNumber(info) then
--		return {false}
--	end
	local item_type, item_name = lRoomMgr.GetItemType(new_itemid_tbl[1][1])
	local itemBindingType = lRoomMgr.GetItemBindingTypeByID(new_itemid_tbl[1][1])
	if not itemBindingType then
		CancelTran()
		return {false}
	end
	local param = {}
	param.m_nCharID = player_id
	param.m_nItemType = item_type
	param.m_sItemName = item_name
	param.m_tblItemID = new_itemid_tbl
	param.m_nBindType = itemBindingType
	local succ,info=lPutItem.AddCountItem(param)
	
	if not succ then
		CancelTran()
		return {false, 160000}
	end
	local ret = { true, res, new_info, {succ, info}, (0-ExpendMoney)}
	return ret
end

function StoneSqr.TakeItem(data)
	local player_id = data["player_id"]
	local tbl_new_items = data["tbl_new_items"]
	local type = data["type"]
	if(tbl_new_items == nil or table.getn(tbl_new_items)==0) then
		return {false}
	end
	
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local item_type, item_name = lRoomMgr.GetItemType(tbl_new_items[1][1])
	local itemBindingType = lRoomMgr.GetItemBindingTypeByID(tbl_new_items[1][1])
	if not itemBindingType then
		return {false}
	end
	local params = {}
	params.m_nCharID = player_id
	params.m_nItemType = item_type
	params.m_sItemName = item_name
	params.m_tblItemID = tbl_new_items
	params.m_nBindType = itemBindingType
	local succ,info=lPutItem.AddCountItem(params)
	if succ then
		return {succ, info, tbl_new_items}
	else
		local title = 1009
		local content = 1009 --“错误”
		if type == "Appraised" then
			title = 1010 --"白宝石鉴定"
			content = 1011 --"您鉴定白宝石时包裹空间已满，通过邮件发给您宝石"
		elseif type == "Synthesised" then
			title = 1012 --"合成物品"
			content = 1013--"您在合成材料市包裹空间已满，通过邮件发给您"
		end
		local lSysMailExecutor = RequireDbBox("SysMailDB")
		succ = lSysMailExecutor.SendSysMail(1014,player_id,title,content,tbl_new_items,0,event_type_tbl["背包满石头融合或鉴定邮件"])
		return {succ}
	end
end

local function GetRandomFirstTimeName(name)
	--2级		3级		4级		5级		6级
	--6		3-4		1-2		<=1		<=1
	---		-		6-7		5		<=1
	local tblName2IndexPercent = 
	{
		["白宝石"] = {{0},{6},{3,4},{1,2},{0,1},{0,1}},
		["高级白宝石"] = {{0},{0},{0},{6,7},{5},{0,1}},
	}
	local tbl_percent = tblName2IndexPercent[name]
	
	local resultFirstName = nil
	local tbl_result = {}
	local tbl_level = {}
	tbl_level[1]={}
	tbl_level[2]={}
	tbl_level[3]={}
	tbl_level[4]={}
	tbl_level[5]={}
	tbl_level[6]={}
	
	local StoneKeys = Stone_Common:GetKeys()
	for i=1, #StoneKeys do
		local StoneKey = StoneKeys[i]
		local StoneLevel = Stone_Common(StoneKey,"StoneLevel")
		if StoneLevel == 1 then
			table.insert(tbl_level[1],StoneKey)
		elseif StoneLevel == 2 then
			table.insert(tbl_level[2],StoneKey)
		elseif StoneLevel == 3 then
			table.insert(tbl_level[3],StoneKey)
		elseif StoneLevel == 4 then
			table.insert(tbl_level[4],StoneKey)
		elseif StoneLevel == 5 then
			table.insert(tbl_level[5],StoneKey)
		elseif StoneLevel == 6 then
			table.insert(tbl_level[6],StoneKey)
		end
	end
	for m = 1, # tbl_percent do
		if # tbl_percent[m] == 1 then --这一等级宝石固定有n个
			for i = 1, tbl_percent[m][1] do
				local r = math.random(#(tbl_level[m]))
				table.insert(tbl_result,tbl_level[m][r])
				if m == 2 or resultFirstName == nil then
					resultFirstName = tbl_level[m][r]
				end
				table.remove(tbl_level[m],r)
				if # tbl_result == 12 then
					return resultFirstName, tbl_result
				end
			end
		else		--这一等级宝石随机有n个
			local n = math.random(#(tbl_percent[m]))
			local count = tbl_percent[m][n]
			if m == 5 and # tbl_result < 11 then --特殊只能特殊写
				count = 1
			elseif m == 6 and # tbl_result < 12 then
				count = 1
			end
			for i = 1, count do
				local r = math.random(#(tbl_level[m]))
				table.insert(tbl_result,tbl_level[m][r])
				if m == 2 or resultFirstName == nil then
					resultFirstName = tbl_level[m][r]
				end
				table.remove(tbl_level[m],r)
				if # tbl_result == 12 then
					return resultFirstName, tbl_result
				end
			end
		end
	end
	return resultFirstName, tbl_result
end

local function GetRandomName(whitestone_Name,name, tbl_newstone)
	--根据权重来得到一个宝石... goon
	local tblName2Index = 
	{
		["白宝石"] = "AppearPower1",
		["高级白宝石"] = "AppearPower2",
	}
	local index_name = tblName2Index[whitestone_Name] --原白宝石等级

	local sum = 0
	local StoneKeys = Stone_Common:GetKeys()
	for i=1, #StoneKeys do
		local Name = Stone_Common(StoneKeys[i],"Name")
		for n = 1, # tbl_newstone do
			if Name == tbl_newstone[n] then
				sum = sum + Stone_Common(StoneKeys[i],index_name)
			end
		end
	end
	
	--local index = GetProcessTime() % sum
	local index = math.random(sum)
	local current_total = 0
	
	for n = 1, # tbl_newstone do--和下面这两个循环不能换位置, 要不去权重比例算得不对
		local StoneKeys = Stone_Common:GetKeys()
		for i=1, #StoneKeys do
			local Name = Stone_Common(StoneKeys[i],"Name")
			if Name == tbl_newstone[n] then
				current_total = current_total + Stone_Common(StoneKeys[i],index_name)
				if current_total >= index then
					return i
				end
			end
		end
	end
	local n = math.random(#(tbl_newstone))
	return tbl_newstone[n]
end

function StoneSqr.WhiteStoneAppraiseFirstTime(data)
	local player_id = data["player_id"]
	local item_id = data["item_id"]	
	local have_count = data["have_count"]
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local res = lRoomMgr.GetRoomIndexAndPosByItemId(player_id, item_id)
	local item_type, item_name = lRoomMgr.GetItemType(item_id)
	if (item_name ~= "白宝石") and (item_name ~= "高级白宝石") then
		return {false}
	end
	
	local new_name, tbl_new_name_result= GetRandomFirstTimeName(item_name)
	if new_name == nil then
		return {false}
	end
	
	local itemBindingType = lRoomMgr.GetItemBindingTypeByID(item_id)
	if not itemBindingType then
		return {false}
	end
	--包裹没空间则不允许鉴定，鉴定失败
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if not g_RoomMgr.HaveEnoughRoomPos(player_id,g_ItemInfoMgr:GetStoneBigID(),new_name,1) and have_count > 1 then
		return {false, 7500}
	end
	local bRet,uMsgId = g_RoomMgr.DelItemByID(player_id,item_id,event_type_tbl["物品使用"])
	if not bRet then
		return {false, uMsgId}
	end
	local params= {}
	params.m_nType = g_ItemInfoMgr:GetStoneBigID()
	params.m_sName = new_name
	params.m_nBindingType = itemBindingType
	params.m_nCharID = player_id
	params.m_sCreateType = event_type_tbl["白宝石鉴定"]
	local new_item_id=g_RoomMgr.CreateItem(params)
	return {true, {res(1,1),res(1,2)}, {new_name, new_item_id},tbl_new_name_result, item_name}
end

function StoneSqr.WhiteStoneAppraiseAgain(data)
	local player_id 		= data["player_id"]
	local item_id 		= data["item_id"]	
	local tbl_newstone	= data["tbl_new_stoneName"]
	local whitestone_Name	= data["whitestone_Name"]
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local res = lRoomMgr.GetRoomIndexAndPosByItemId(player_id, item_id)
	local item_type, item_name = lRoomMgr.GetItemType(item_id)
	if not res or not g_ItemInfoMgr:IsStone(item_type) then
		return {false}
	end
	
	local new_name = GetRandomName(whitestone_Name,item_name,tbl_newstone)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local itemBindingType = lRoomMgr.GetItemBindingTypeByID(item_id)
	if not itemBindingType then
		return {false}
	end
	
		local params= {}
			params.m_nType = g_ItemInfoMgr:GetStoneBigID()
			params.m_sName = new_name
			params.m_nBindingType = itemBindingType
			params.m_nCharID = player_id
			params.m_sCreateType = event_type_tbl["白宝石鉴定"]
	local new_item_id=g_RoomMgr.CreateItem(params)
	return {true, {res(1,1),res(1,2)}, {new_name, new_item_id}}
end

local function IsSlotInlay(player_id, panel, depart, slot)
	local ret = StmtContainer._GetHoleStone:ExecSql("ns[32]nn", player_id, panel, depart, slot)
	local num = ret:GetRowNum()
	ret:Release()
	return num > 0
end

function StoneSqr.RemovalStole(data)
	local player_id = data["player_id"]
	local panel = data["Panel"]
	local depart = data["Depart"]
	local slot = data["Slot"]
	if IsSlotInlay(player_id, panel, depart, slot) then
		local now_panel = StoneSqr.GetNowPanel(player_id)
		local ret = StmtContainer._GetHoleStone:ExecSql("ns[32]nn", player_id, panel, depart, slot)
		local rows = ret:GetRowNum()
		if (not rows) or (rows == 0) then
			return {false,7009}
		end
		--判断金钱是否够用
		local costMoney = 1000
		local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
		if not MoneyManagerDB.EnoughMoney(player_id, costMoney) then
			return {false,7010}
  	end
		local item_type, item_name, itemBindingType, sh_uId = ret:GetNumber(0,0),ret:GetString(0,1), ret:GetNumber(0,2), ret:GetNumber(0,3)
		ret:Release()
		StmtContainer._RemovalStone:ExecSql("", player_id,sh_uId)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			return {false,7009}
		end
		
		local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
		local fun_info = g_MoneyMgr:GetFuncInfo("Stone")
		local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["RemoveStone"],player_id, -costMoney,nil,event_type_tbl["宝石摘除"])
		if not bFlag then
			CancelTran() 
			if IsNumber(uMsgID) then
				return {false,uMsgID}
			end
			return {false,7009}
		end
		
		local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
		local new_name = item_name
--		local new_name = Stone_Common(item_name,"LastLevelName")
--		if not new_name or new_name == "" then
--			new_name = item_name
--		end
		
		local params= {}
		params.m_nType = g_ItemInfoMgr:GetStoneBigID()
		params.m_sName = new_name
		params.m_nBindingType = itemBindingType
		params.m_nCharID = player_id
		params.m_sCreateType = event_type_tbl["宝石摘除"]
		local new_item_id=g_RoomMgr.CreateItem(params)
		
		local param = {}
		param.m_nCharID = player_id
		param.m_nItemType = item_type
		param.m_sItemName = new_name
		param.m_tblItemID = {{new_item_id}}
		param.m_nBindType = itemBindingType
		local succ,info=lPutItem.AddCountItem(param)
		if not succ then
			CancelTran()
			return {false,7009}
		end
		local ret = {true}
		ret["add_info"] = info
		ret["OpenedHolesList"] = StoneSqr.GetAllHoleOpened(player_id)
		ret["AllInlayedHolesInfo"] = StoneSqr.GetAllInlayedHolesInfo(player_id)
		ret["extra_info"] = {}
		ret["extra_info"]["now_panel"] = now_panel
		ret["extra_info"]["old_panel"] = panel
		ret["extra_info"]["old_name"] = item_name
		ret["cost_money"] = 0-costMoney
		return ret
	end
	return {false,7009}
end

local function CheckType(Depart,item_type, item_name)
	local stone_info = Stone_Common(item_name)
	if (not stone_info) or (not stone_info("Depart"..Depart))  or (stone_info("Depart"..Depart) == 0) then
		return false
	end
	return true
end

local function GetHoleId(PlayerId,Panel,Depart,Slot)
	local ret=StmtContainer._FindHoleId:ExecSql("n",PlayerId,Panel,Depart,Slot)
	local num = 0
	if ret:GetRowNum() > 0 then
		num = ret:GetNumber(0,0)
	end
	ret:Release()
	return num
end

function StoneSqr.InlayStone(parameter)
	local player_id = parameter["player_id"]
	local panel = parameter["panel"]
	local depart = parameter["depart"]
	local slot = parameter["slot"]
	local stoneId = parameter["stoneId"]
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local item_type, item_name = lRoomMgr.GetItemType(stoneId)
	local res = lRoomMgr.GetRoomIndexAndPosByItemId(player_id, stoneId)
	local holeId=GetHoleId(player_id, panel, depart, slot)
	if (not res) or (holeId==0)
		or (CheckHoleOpened(player_id, panel, depart, slot)==false) --是否打开
		or (IsSlotInlay(player_id, panel, depart, slot)==true)  --是否已镶嵌
		or (slot>1 and IsSlotInlay(player_id, panel, depart, slot-1)==false) --查询其前面的孔是否为空
		or (CheckType(depart,item_type, item_name)==false) then	         --查询孔与宝石的属性是否符
		return {false}
	else 
		local ItemInfoRet = StmtContainer._GetItemInfo:ExecSql('ns[32]', stoneId)
		local stoneType,stoneName = ItemInfoRet:GetNumber(0,0),ItemInfoRet:GetString(0,1)
		local stoneIsbind = lRoomMgr.GetItemBindingTypeByID(stoneId)
		if not stoneIsbind then
			return {false}
		end
		StmtContainer._InlayStone:ExecSql('',player_id,holeId,stoneType,stoneName,stoneIsbind)
		if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
			CancelTran()
			return {false}
		end	
		
		local bRet,uMsgId = lRoomMgr.DelItemByID(player_id,stoneId,event_type_tbl["物品使用"])
		if not bRet then
			CancelTran()
			return {false,uMsgId}
		end
	end
	
	local ret = {true, {res(1,1),res(1,2)}}
	ret["OpenedHolesList"] = StoneSqr.GetAllHoleOpened(player_id)
	ret["AllInlayedHolesInfo"] = StoneSqr.GetAllInlayedHolesInfo(player_id)
	ret["extra"] = {}
	ret["extra"]["item_type"] = item_type
	ret["extra"]["item_name"] = item_name
	ret["extra"]["now_panel"] = StoneSqr.GetNowPanel(player_id)
	return ret	
end

SetDbLocalFuncType(StoneSqr.OpenHole)
SetDbLocalFuncType(StoneSqr.InlayStone)
SetDbLocalFuncType(StoneSqr.RemovalStole)
SetDbLocalFuncType(StoneSqr.SynthesisItem)
SetDbLocalFuncType(StoneSqr.WhiteStoneAppraiseAgain)
SetDbLocalFuncType(StoneSqr.TakeItem)
SetDbLocalFuncType(StoneSqr.ChangeFrame)
return StoneSqr
