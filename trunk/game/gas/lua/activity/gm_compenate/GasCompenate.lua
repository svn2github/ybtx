
CGasCompenate = class()

function CGasCompenate.GetCompensateItemInfo(Conn,ActivityID)
		local Player = Conn.m_Player
		if not (Player and IsCppBound(Player)) then return end
		local role_id = Player.m_uID
		local function _GetActivityItemInfo_CallBack(ItemInfo)
				local tbl_item = {}
				Gas2GacById:ReturnGetCompensateItemInfoBegin(role_id,ActivityID)
				for i =1,#ItemInfo do
					local activity_id,H64ItemNameI,item_left_num,begin_time,end_time = unpack(ItemInfo[i])
					table.insert(tbl_item,{activity_id,H64ItemNameI,item_left_num,begin_time})
					Gas2GacById:ReturnGetCompensateItemInfo(role_id,activity_id,H64ItemNameI,item_left_num,begin_time,end_time) 
				end
				Gas2GacById:ReturnGetCompensateItemInfoEnd(role_id,ActivityID) 
				Conn.m_tblAllCompenateItem = tbl_item
		end
		local data = {Conn.m_Account:GetID(),role_id,ActivityID,Player:CppGetLevel()}
		g_EProtocol:ActivityItemQuery(data, _GetActivityItemInfo_CallBack)
end

function CGasCompenate.TakeCompensateItem(Conn,ActivityID,H64ItemName,ItemNum)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	local tbl_item = Conn.m_tblAllCompenateItem or {}
	local have_item = false
	for i =#tbl_item,1,-1 do
		if ActivityID == tbl_item[i][1] and H64ItemName == tbl_item[i][2] and ItemNum <= tbl_item[i][3] then
			have_item = true
			Conn.m_CompenateTime = tbl_item[i][4]
			table.remove(tbl_item,i)
			break
		end
	end
	if not have_item then return end
		
	local role_id = Player.m_uID
	local SendName = H64ItemName
	local ItemInfo = {0,SendName}
	local user_id = Conn.m_Account:GetID()
	local begin_time = Conn.m_CompenateTime
	local binding_type
	if not string.find(SendName,"money") then
		local str_pos = string.find(H64ItemName,"&")
		if str_pos then
			binding_type = tonumber(string.sub(H64ItemName,str_pos+1,str_pos+1))
			H64ItemName = string.sub(H64ItemName,1,str_pos-1)
		end
		local H64RealName,SoulCount = g_ItemInfoMgr:GetSoulpearlInfo(H64ItemName)
		local ItemName = MemH64ToMbcs(H64RealName)
		ItemInfo = g_ItemInfoMgr:GetItemInfoByIndex(ItemName)
		if not ItemInfo or #ItemInfo == 0 then return end
		if SoulCount > 0 then
			ItemInfo[2] = ItemInfo[2] .. ":" .. SoulCount
		end
	end
	
	local function CallBack(bFlag,sMsg)
			if not bFlag and sMsg ~= "got" then
				if sMsg then
					MsgToConnById(sMsg,52)
				else
					MsgToConnById(role_id,52)
				end
				return
			end
			if sMsg == "got" then
				MsgToConnById(role_id,61)
			else
				MsgToConnById(role_id,53)
			end
			
			local function _UserItemMinus_CallBack_Succ()
					local function DelCallBack()
						CGasCompenate.GetCompensateItemInfo(Conn,ActivityID)
					end
					CallAccountManualTrans(Conn.m_Account,"GMCompenate", "DelCompenateItem", DelCallBack, {["item_name"] = ItemInfo[2],["user_id"] = user_id,["activity_id"] = ActivityID,["begin_time"] = begin_time})	
			end
			local function _UserItemMinus_CallBack_Fail(ReturnCode)
			end
			local data = {user_id,role_id,ActivityID,SendName,ItemNum}
			g_EProtocol:UserItemMinus(data, {_UserItemMinus_CallBack_Succ,_UserItemMinus_CallBack_Fail})
	end
	local data = {}
	data["item_type"] = ItemInfo[1]
	data["item_name"] = ItemInfo[2]
	data["item_num"] = ItemNum
	data["char_id"] = role_id
	data["binding_type"] = binding_type
	data["user_id"] = user_id
	data["activity_id"] = ActivityID
	data["begin_time"] = begin_time
	CallAccountManualTrans(Conn.m_Account,"GMCompenate", "AddCompenateItem", CallBack, data)	
end


-----------------------下面是游戏内账号奖励功能，不经过erating的-------------------------------
local function SendItemInfo(role_id,ItemInfo)
	local ActivityID = 1000000000
	Gas2GacById:ReturnGetCompensateItemInfoBegin(role_id,ActivityID)
	for i =1,ItemInfo:GetRowNum() do
		Gas2GacById:ReturnActivationCodeCompensateItem(role_id,ItemInfo(i,1),ItemInfo(i,2),ItemInfo(i,3),ItemInfo(i,4)) 
	end
	Gas2GacById:ReturnGetCompensateItemInfoEnd(role_id,ActivityID) 
end

function CGasCompenate.GetActivationCodeCompenateItem(Conn)
		local Player = Conn.m_Player
		local Account = Conn.m_Account
		if not (Player and IsCppBound(Player) and IsCppBound(Account)) then return end
		local role_id = Player.m_uID
		
		local function CallBack(ItemInfo)
			SendItemInfo(role_id,ItemInfo)
		end
		CallDbTrans("GMCompenate", "GetUserComenpate", CallBack, {["user_id"] = Account:GetID()})	
end

function CGasCompenate.TakeActivationCodeCompensate(Conn,CompenateID)
	local Player = Conn.m_Player
	local Account = Conn.m_Account
	if not (Player and IsCppBound(Player) and IsCppBound(Account)) then return end
	local role_id = Player.m_uID
	
	local function CallBack(Flag,ItemInfo)
		if not Flag then
			MsgToConnById(role_id,52)
			return
		end
		MsgToConnById(role_id,53)
		SendItemInfo(role_id,ItemInfo)
	end
	CallAccountManualTrans(Conn.m_Account,"GMCompenate", "TakeActivationCodeCompensate", CallBack, {["user_id"] = Account:GetID(),["role_id"] = role_id,["compensate_id"] = CompenateID})	
end


function CGasCompenate.AssignUserSoulAndExp()
	if g_CurServerId ~= 1 then
		return
	end	
	local function CallBack()
		if not g_App:CallDbHalted() then
			CallDbTrans("GMCompenate","AssignUserSoulAndExp", nil,{})
		end
	end
	g_AlarmClock:AddTask("AssignUserSoulAndExp", {wday = {1,2,3,4,5,6,7},time = {"05:00"}}, CallBack)
end

function CGasCompenate.AssignUserSoulAndExpGM()
	CallDbTrans("GMCompenate","AssignUserSoulAndExp", nil,{})
end

