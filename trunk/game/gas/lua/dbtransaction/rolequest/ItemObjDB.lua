--数据库操作.只进行了.数据的查询.主要是查询 玩家位置 在线与否
local EAssignMode =
{
	eAM_FreePickup 		   = 0, --自由拾取
	eAM_AverageAssign 	 = 1, --平均分配
	eAM_RespectivePickup = 2, --各自拾取
	eAM_AuctionAssign 	 = 3, --拍卖模式
	eAM_NeedAssign			 = 4, --需求分配
}
local CPos = CPos
local ShareArea = 38
local event_type_tbl = event_type_tbl
local ItemObjDB = CreateDbBox(...)

local function Get2PosDistance(pos1, pos2)
	return math.max( math.abs( pos1.x - pos2.x ), math.abs( pos1.y - pos2.y ) )
end

local function ClickItemObjAddItem(data)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local res1 = CharacterMediatorDB.AddItem(data)
	if IsNumber(res1) then
		CancelTran()
		return {false,res1}
	end
	local uCharId = data["char_id"]
	local uItemType = data["nType"]
	local uItemId = nil
	for j = 1,#(res1) do
		for n = 1,#(res1[j]) do
			uItemId = res1[j][n]
			break
		end
		if uItemId then
			break
		end
	end
	local DirectDB = RequireDbBox("DirectDB")
	local DirectTbl = DirectDB.AddPlayerItemDirect(uCharId, "拾取获得物品", data["nType"], data["sName"])
	return {true,res1,DirectTbl}
end
--平均分配
local function AverageAssignOneItemObj(data)
	local PlayerId = data["char_id"]
	local AverageAssignOrder = data["AverageAssignOrder"]
	local CanSharePlayer = data["CanSharePlayer"]
	local LoginServerSql = RequireDbBox("LoginServerDB")
	local AreaDB = RequireDbBox("AreaDB")
	
	local itemtype = data["nType"]
	local itemname = data["sName"]
	local itemnum = 1
	
	--更改一下给物品的顺序
	if CanSharePlayer and AverageAssignOrder then
		for i=1, #(AverageAssignOrder) do
			if not CanSharePlayer[i] then
				break
			elseif AverageAssignOrder[i] ~= CanSharePlayer[i] then
				local Temp = CanSharePlayer[i]
				for k=i+1, #(CanSharePlayer) do
					if AverageAssignOrder[i] == CanSharePlayer[k] then
						CanSharePlayer[i] = CanSharePlayer[k]
						CanSharePlayer[k] = Temp
						break
					end
				end
			end
		end
	end
	
	local ordertbl = CanSharePlayer or AverageAssignOrder
	
	local function GiveItem(GainerId)
		local Param = {}
		Param["char_id"]	= GainerId
		Param["nType"]	= itemtype
		Param["sName"]	= itemname
		Param["nCount"]	= itemnum
		Param["createType"]	= event_type_tbl["平均分配拾取物品"]
		local ResTbl = ClickItemObjAddItem(Param)
		if ResTbl[1] then
			return {true,GainerId,ResTbl}
		elseif ResTbl[2] == 3 then --3代表背包满，背包满也算分配成功
			return {true,GainerId}  --背包满时，没有添加物品的返回结果集ResTbl
		end
		return {false}
	end
	
	for i = 1, table.getn(ordertbl) do
		---从顺序表中找一个符合条件的物品获得者
		local res = nil
		local teammateid = ordertbl[i]
		if teammateid ~= PlayerId then
			data["char_id"] = teammateid
			if AreaDB.Check2PlayerInSameArea(PlayerId,teammateid) 
				and LoginServerSql.IsPlayerOnLine(teammateid) then
			--if InSameAreaPlayer[teammateid] then  --判断是否符合共享范围
				res = GiveItem(teammateid)
			end
		else  --当分配顺序到自己时，不再检测是否在线和范围判断
			res = GiveItem(PlayerId)
		end
		
		if res and res[1] then  --如果分配成功直接Return
			return res
		end
	end
	return {false}  --达到这一步表示分配物品时所有人都不符合要求
end


--返回  符合参加拍卖条件的玩有的ID  char_id
local function AuctionAssignOneItemObj(data)
	local PlayerId = data["char_id"]
	local AverageAssignOrder = data["AverageAssignOrder"]
	local CanSharePlayer = data["CanSharePlayer"]
	--local InSameAreaPlayer = data["InSameAreaPlayer"]
	local LoginServerSql = RequireDbBox("LoginServerDB")
	local AreaDB = RequireDbBox("AreaDB")
	
	local itemtype = data["nType"]
	local itemname = data["sName"]
	local itemnum = 1	
	
	local ordertbl = AverageAssignOrder or CanSharePlayer
	local res = {}
	
	for i = 1, table.getn(ordertbl) do
		local teammateid = ordertbl[i]
		data["char_id"] = teammateid
		if AreaDB.Check2PlayerInSameArea(PlayerId,teammateid) 
			and LoginServerSql.IsPlayerOnLine(teammateid) then
		--if InSameAreaPlayer[teammateid] then  --判断是否在同一区域
			table.insert( res, teammateid)
		end
	end
	return res
end

local function NeedAssignGiveItem(data)
	data["nCount"] = 1
	data["createType"]	= event_type_tbl["需求分配"]
	local ResTbl = ClickItemObjAddItem(data)
	if not ResTbl[1] then
		return {false,ResTbl[2]}
	end
	return {true,ResTbl[2],ResTbl[3]}
end

function ItemObjDB.PickUpOneItemObj(data)
	--物品尚未分配出去，而且当前队伍是平均分配模式或者拍卖模式
--	print("PickUpOneItemObj",data["AssignMode"])
	if data["AssignMode"] == EAssignMode.eAM_AverageAssign then  --平均分配
		return AverageAssignOneItemObj(data)
	elseif data["AssignMode"] == EAssignMode.eAM_AuctionAssign then	 --拍卖分配
		return AuctionAssignOneItemObj(data)
	end
	local PlayerId = data["char_id"]
	data["nCount"] = 1
	data["createType"]	= event_type_tbl["自由拾取物品"]
	local ResTbl = ClickItemObjAddItem(data)
	if not ResTbl[1] then
		return {false,ResTbl[2]}
	end
	return {true,ResTbl[2],ResTbl[3]}
end

function ItemObjDB.NeedAssignGiveItem(data)
	return NeedAssignGiveItem(data)
end

function ItemObjDB.CollectOreObj(data)

	local GatherLiveSkillDB = RequireDbBox("GatherLiveSkillDB")
	local AddExp,SkillInfo = GatherLiveSkillDB.AddExperience(data["char_id"], "采矿", data["AddOreExp"])

	data["createType"]	= event_type_tbl["自由拾取物品"]
	local res = ClickItemObjAddItem(data)
	if not res[1] then
		CancelTran()
		return {false,res[2]}
	else
		return {true,res[2],AddExp,SkillInfo,res[3]}
	end
end

return ItemObjDB