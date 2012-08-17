CDirectionsMake = class()

local function ProduceByProductNameFaield(data)
	local Conn = data[1]
	Gas2Gac:ReturnProduceByProductName(Conn, false)
end

--先判断是否可以制造
local function CanProduceByProductName(Conn, sDirectType,sSkillName,DirectionName,nNum)
	local function CallBack(result)
		if (not result) or IsNumber(result) then
			if IsNumber(result) then
				MsgToConn(Conn,result)
			end
			ProduceByProductNameFaield({Conn,sSkillName})
			return
		end
		ProduceProgress(Conn, sDirectType,sSkillName,DirectionName,nNum)
	end
	
	if not(Conn.m_Account and IsCppBound(Conn.m_Account)) then return end

	local params = {}
	params.uPlayerID = Conn.m_Player.m_uID
	params.DirectionName = DirectionName
	params.nNum = 1
	params.sSkillName = sSkillName
	params.sDirectType = sDirectType
	CallAccountManualTrans(Conn.m_Account, "DirectionsMakeDB","CanProduceByProductName", CallBack, params)
end

local function ProduceByProductName(data)
			local Conn,sSkillName,sDirectType, DirectionName,nNum = unpack(data)
			local Player = Conn.m_Player
			if Player == nil then 
				return
			end
			local uPlayerID = Player.m_uID
			local function CallBack(bFlag,tblRetRes,sMsg)
			
					if not bFlag then
						if IsNumber(tblRetRes) then
							if sMsg then
								MsgToConn(Conn,tblRetRes,sMsg)
							else
								MsgToConn(Conn,tblRetRes)
							end
						end
						MsgToConn(Conn,9544)
						ProduceByProductNameFaield({Conn,sSkillName})
						return
					end
					
					MsgToConn(Conn,9557)
					
					local res,info,new_direction = tblRetRes.m_tblDelRes,tblRetRes.m_tblAddRes,tblRetRes.new_direction
					local ExpAddNum,nTotalExp = tblRetRes.m_AddExp,tblRetRes.m_nTotalExp
					local AddSpecial = tblRetRes.m_AddSpecial
					
					if new_direction then
						Gas2Gac:ReturnLearnNewDirection(Conn,sSkillName,new_direction[1],new_direction[2])
						Gas2Gac:ReturnGetProductsCanMake(Conn,new_direction[2])
						Gas2Gac:ReturnGetProductsCanMakeEnd(Conn)
					end
					if IsNumber(ExpAddNum) and ExpAddNum > 0 then
						MsgToConn(Conn,9541,ExpAddNum)
						Gas2Gac:ReturnLiveSkillExp(Conn, nTotalExp, sSkillName)
					end
					if IsNumber(AddSpecial) and AddSpecial > 0 then
							MsgToConn(Conn,9542,AddSpecial)
					end
					nNum = nNum - 1
					if nNum > 0 then
						CanProduceByProductName(Conn, sDirectType,sSkillName,DirectionName,nNum)
					else
						Gas2Gac:ReturnProduceByProductName(Conn, true)
					end
			end
			if not(Player and IsCppBound(Player)) then return end
			if not(Conn.m_Account and IsCppBound(Conn.m_Account)) then return end
			local params = {}
			params.uPlayerID	= uPlayerID
			params.DirectionName = DirectionName
			params.nNum = 1
			params.sceneName = Player.m_Scene.m_SceneName
			params.sSkillName = sSkillName
			params.sDirectType = sDirectType
			CallAccountManualTrans(Conn.m_Account, "DirectionsMakeDB", "ProduceByProductName", CallBack, params)
end

function ProduceProgress(Conn,sDirectType,sSkillName,DirectionName,nNum)
	local Player = Conn.m_Player
	if Player == nil then return end
	local load_time = 1000*5
	if "烹饪" == sSkillName then
		load_time = 1000*2
	end
	local bFlag,nIndex = CommLoadProgress(Player,load_time, ProduceByProductName, ProduceByProductNameFaield, {Conn,sSkillName,sDirectType,DirectionName,nNum,["ActionName"] = sSkillName})	
	if not bFlag then
		ProduceByProductNameFaield({Conn,sSkillName})
	end
end

local function ProduceByProductNameFor(Conn, sDirectType, DirectionName, nNum, sSkillName, trap_name, nMsgID)
	local Player = Conn.m_Player
	if Player == nil then return end
	if nNum <= 0 then
		return
	end
	local f = Player.m_Properties.m_InTrapTblName[trap_name]
	if not (f ~= nil and table.maxn(f) ~= 0) then
		MsgToConn(Conn, nMsgID)
		ProduceByProductNameFaield({Conn, sSkillName})
		return 
	end
	CanProduceByProductName(Conn, sDirectType, sSkillName, DirectionName,nNum)
end

local function BreakDoing(Conn, sSkillName)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then
		return
	end
	if Player.m_CommLoadProTick then
		CommStopLoadProgress(Player)
	end
	ProduceByProductNameFaield({Conn, sSkillName})
end

local tblMsgID = {}
tblMsgID["锻造"] = 9546
tblMsgID["铸甲"] = 9547
tblMsgID["烹饪"] = 9548
tblMsgID["制药"] = 9549
tblMsgID["工艺"] = 9550

--【制造】
function CDirectionsMake.ProduceByProductName(Conn,SkillName,sDirectType, DirectionName, nNum)
	ProduceByProductNameFor(Conn, sDirectType, DirectionName, nNum, SkillName,SkillName, tblMsgID[SkillName])
end

--全部制造
function  CDirectionsMake.ProduceByProductNameAll(Conn,SkillName, sDirectType, DirectionName)
	ProduceByProductNameFor(Conn, sDirectType, DirectionName, 1000, SkillName,SkillName, tblMsgID[SkillName])
end

--取消制造
function  CDirectionsMake.LiveSkillBreakProducing(Conn,sSkillName)
	BreakDoing(Conn,sSkillName)
end

--【获得已经学会的配方】
function  CDirectionsMake.GetProductsCanMake(Conn,sSkillName)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	local function CallBack(result,tbl_expert)
		local expert_name,expert_level = "",0
		if tbl_expert:GetRowNum() > 0 then
			expert_name,expert_level = tbl_expert(1,1),tbl_expert(1,2)
		end
		local row = result:GetRowNum()
		Gas2Gac:ReturnGetProductsCanMakeBegin(Conn,expert_name,expert_level)
		for i =1,row do
			Gas2Gac:ReturnGetProductsCanMake(Conn, result(i,1))
		end
		Gas2Gac:ReturnGetProductsCanMakeEnd(Conn)
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.sSkillName	= sSkillName
	
	CallAccountManualTrans(Conn.m_Account, "DirectionsMakeDB","GetProductsCanMake", CallBack, parameters)
end

function  CDirectionsMake.GetLiveSkillProdRemainCDTime(Conn,sSkillName,DirectType,DirectionName)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	local function CallBack(cool_time)
		Gas2Gac:ReturnGetLiveSkillProdRemainCDTime(Conn,DirectionName,cool_time)
	end
	
	local parameters = {}
	parameters.uPlayerID = uPlayerID
	parameters.sSkillName = sSkillName
	parameters.DirectType = DirectType
	parameters.DirectionName = DirectionName
	CallAccountManualTrans(Conn.m_Account, "DirectionsMakeDB","GetCoolDownTime", CallBack, parameters)
end

function  CDirectionsMake.ReturnMakeDirectionForCM(nCharID,tblRetRes)
	local new_direction = tblRetRes.new_direction
	local ExpAddNum,nTotalExp = tblRetRes.m_AddExp,tblRetRes.m_nTotalExp
	local AddSpecial = tblRetRes.m_AddSpecial
	local sSkillName = tblRetRes.sSkillName
	local target_id = tblRetRes.target_id	
	if new_direction then
		Gas2GacById:ReturnLearnNewDirection(nCharID,sSkillName,new_direction[1],new_direction[2])
	end
	if IsNumber(ExpAddNum) and ExpAddNum > 0 then
		MsgToConnById(nCharID,9541,ExpAddNum)
		Gas2GacById:ReturnLiveSkillExp(nCharID, nTotalExp, sSkillName)
	end
	if IsNumber(AddSpecial) and AddSpecial > 0 then
		MsgToConnById(nCharID,9542,AddSpecial)
	end
end

function CDirectionsMake.LiveSkillSendMail(nCharID)
	MsgToConnById(nCharID,9625)
end
