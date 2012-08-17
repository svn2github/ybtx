CBreakItem = class()

function  CBreakItem.BreakItem(Connection,nRoomIndex,nPos)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end
		
	if GasConfig.GasType == 1 then
		Gas2Gac:OpenMessageBox(Connection, 1400)
		Gas2Gac:ReturnBreakItemEnd(Connection, false, nRoomIndex,nPos)
		return
	end
	
	local nCharID = Player.m_uID
	if Player.m_ItemBagLock then
		MsgToConn(Connection,160010)
		Gas2GacById:ReturnBreakItemEnd(nCharID, false,nRoomIndex,nPos) 
		return 
	end
	
	local function BreakItemFailed(data)
		local bFlag = data[1]
		Gas2GacById:ReturnBreakItemEnd(nCharID, bFlag,nRoomIndex,nPos) 
	end
	local function BreakItem()
			local function CallBack(tblRes,Msg)
				if not tblRes then
					if IsNumber(Msg) then
						MsgToConn(Connection,Msg)
					elseif IsString(Msg) then
						MsgToConn(Connection,25,Msg)
					end
					BreakItemFailed({false})
					return
				end
				Gas2GacById:ReturnBreakItemBegin(nCharID)
				local tbl_add = tblRes.tbl_add
				MsgToConn(Connection,27)
				local AddExp = tblRes.AddExp
				if AddExp and AddExp > 0 then
					MsgToConn(Connection,35,AddExp)
					Gas2GacById:AddBreakItemExp(nCharID,AddExp)
				else
					MsgToConn(Connection,36)
				end
				for i=1,#tbl_add do
					local item_info = tbl_add[i][1]
					local item_num = tbl_add[i][2]
					local item_type,item_name = item_info[1].m_nBigID,item_info[1].m_nIndex
					MsgToConn(Connection,28,item_num,item_name)
					Gas2GacById:ReturnBreakProducts(nCharID, item_type,item_name,item_num) 
				end
				Gas2GacById:ReturnBreakItemEnd(nCharID, true,nRoomIndex,nPos) 
			end
			
			if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end
			if not Connection.m_Account then return end
			local parameters = {}
			parameters.nCharID = nCharID
			parameters.nRoomIndex = nRoomIndex
			parameters.nPos = nPos
			parameters.sceneName = Player.m_Scene.m_SceneName
			local nRes = CallAccountManualTrans(Connection.m_Account, "BreakItemDB","BreakItem", CallBack, parameters)
			if 1 == nRes then
				BreakItemFailed({true})
			end
	end
	
	local function CanBreakItem()
		local function RetCanBreakItem(result,sMsg)
			if not result or IsNumber(result) then
				if IsNumber(result) then
					if sMsg then
						MsgToConn(Connection,result,sMsg)
					else
						MsgToConn(Connection,result)
					end
				end
				BreakItemFailed({true})
				return
			end
			local bFlag,nIndex = CommLoadProgress(Player, 1000*2, BreakItem, BreakItemFailed, {false,["ActionName"] = "·Ö½â"})	
			if not bFlag then
				BreakItemFailed({false})
			end
		end
		
		local parameters = {}
		parameters.nCharID = nCharID
		parameters.nRoomIndex = nRoomIndex
		parameters.nPos = nPos
		CallAccountManualTrans(Connection.m_Account, "BreakItemDB","CanBreakItem", RetCanBreakItem, parameters)
	end
	CanBreakItem()
end
