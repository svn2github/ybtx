
local GasPetInfoDB = "GasPetInfoDB"

--------------------------------------------------------------------------------
local function ReturnCharPetInfo(Conn,charPetInfoSet)
	if charPetInfoSet:GetRowNum() > 0 then
		local charPetInfoTbl = charPetInfoSet:GetTableSet()
		for i = 1,charPetInfoSet:GetRowNum() do
			Gas2Gac:ReturnCharPetInfo(Conn,charPetInfoTbl(i,1),charPetInfoTbl(i,2) or charPetInfoTbl(i,3),
			charPetInfoTbl(i,4),charPetInfoTbl(i,5),charPetInfoTbl(i,6),charPetInfoTbl(i,7),charPetInfoTbl(i,8),
			charPetInfoTbl(i,9),charPetInfoTbl(i,10))
		end
	end
end
--------------------------------------------------------------------------------
local function ReturnCharPetEggInfo(Conn,charpetEggTbl)
	if #charpetEggTbl > 0 then
		Gas2Gac:ReturnCharPetEggInfo(Conn,charpetEggTbl[1][1],charpetEggTbl[1][2])
	end
end
--------------------------------------------------------------------------------
local function RetPreparePet(Conn,preparePet)
	if preparePet:GetRowNum() > 0 then
		local preparePetTbl = preparePet:GetTableSet()
		Gas2Gac:ReturnPreparePet(Conn,preparePetTbl(1,1))
	end
end
--------------------------------------------------------------------------------
--上线后向客户端发送幻兽技能信息
local function GetPetSkill(Conn,petSkill)
	if petSkill:GetRowNum() > 0 then
		local petSkillTbl = petSkill:GetTableSet()
		for i=1,petSkill:GetRowNum() do
			Gas2Gac:ReturnUsePetSkillStone(Conn,petSkillTbl(i,1),petSkillTbl(i,2),petSkillTbl(i,3))
		end
	end
end
--------------------------------------------------------------------------------
function Gac2Gas:InitPetWndInfo(Conn)
	local data = {["charId"]		= Conn.m_Player.m_uID}
	
	local function CallBack(result)    
		Gas2Gac:RetInitPetWndInfoBegin(Conn)
		local charPetInfoTbl = result["charPetInfoTbl"]
		local charpetEggTbl = result["charpetEggTbl"]
		local preparePet	= result["preparePet"]
		local petSkill = result["petSkill"]
		ReturnCharPetInfo(Conn,charPetInfoTbl)
		GetPetSkill(Conn,petSkill)
		ReturnCharPetEggInfo(Conn,charpetEggTbl)
		RetPreparePet(Conn,preparePet)
		Gas2Gac:ReturnCharPetInfoEnd(Conn)
	end
	
	CallAccountManualTrans(Conn.m_Account, GasPetInfoDB, "InitPetWndInfo", CallBack, data)
end
--------------------------------------------------------------------------------
--@brief 使用幻兽晶核
--@param ItemName:ItemName晶核名称
--@param nRoomIndex:包裹索引
--@param nPos:包裹格子
function Gac2Gas:IncubatePetEgg(Conn,ItemName,nRoomIndex, nPos)
	local data = {
					["charId"]		= Conn.m_Player.m_uID,
					["ItemName"]	= ItemName,
					["nRoomIndex"]	= nRoomIndex,
					["nPos"]		= nPos
				}
	local function CallBack(succ,petEggId,petType)
		if succ then
			Gas2Gac:ReturnIncubatePetEgg(Conn,succ,ItemName,petType)
		else
			if IsNumber(petEggId) then
				MsgToConn(Conn, petEggId)
			end
		end
	end
	
	CallAccountManualTrans(Conn.m_Account, GasPetInfoDB, "IncubatePetEgg", CallBack, data)
end
--------------------------------------------------------------------------------
--@brief 孵化幻兽晶核
--@param petEggName:晶核名称
--@param costSoulValue:需要消耗的魂值
function Gac2Gas:PetEggPourSoul(Conn,petEggName,costSoulValue)
	local data = {
					["charId"]		= Conn.m_Player.m_uID,
					["petEggName"]	= petEggName,
					["costSoulValue"]	= costSoulValue,
					["petHealthPoint"] = CStaticAttribMgr_CalcBasicMaxHealthPoint(5,1 ),
					["petDamageValue"] = CStaticAttribMgr_CalcBasicMaxManaPoint(5,1 )
		}
		
	local function CallBack(succ,msgId,total_soul,petId,petName,petEggName,aptitudeValue,petType,petHealthPoint,petDamageValue,uExpValue,petVimValue,skillName1,skillName2)    
		if succ then
			Gas2Gac:ReturnPetEggPourSoul(Conn,petId,petName,petEggName,aptitudeValue,petType,petHealthPoint,petDamageValue,uExpValue,petVimValue,skillName1,skillName2 or "")
		end
		
		if msgId == 194106 then
			MsgToConn(Conn, msgId,total_soul,petId)
		else
			MsgToConn(Conn, msgId)
		end
		if msgId == 194102 or msgId == 194103 then
			Gas2Gac:ReturnModifyPlayerSoulNum(Conn,total_soul)
		end
	end
	
	CallAccountManualTrans(Conn.m_Account, GasPetInfoDB, "PetEggPourSoul", CallBack, data)
end
-----------------------------------------------------------------------------------------
--@brief 晶核卵放生
--@param petEggName:晶核名称
function Gac2Gas:ThrowAwayPetEgg(Conn,petEggName)
	local data = {
								["charId"]			= Conn.m_Player.m_uID,
								["petEggName"]	= petEggName
								}
		local function CallBack(succ)    
			if succ then
				Gas2Gac:ReturnThrowAwayPetEgg(Conn,petEggName)
			else
				MsgToConn(Conn, 194108)
			end
		end
	CallAccountManualTrans(Conn.m_Account, GasPetInfoDB, "ThrowAwayPetEgg", CallBack, data)
end
-----------------------------------------------------------------------------------------
--幻兽放生
function Gac2Gas:ThrowAwayPet(Conn,petId)
	local data = {
					["charId"]		= Conn.m_Player.m_uID,
					["petId"]	= petId
		}
		
	local function CallBack(succ,msgId,bIsPreparePet)    
		if succ then
			Gas2Gac:ReturnThrowAwayPet(Conn,petId,bIsPreparePet)
		end
		MsgToConn(Conn, msgId)
	end
	
	CallAccountManualTrans(Conn.m_Account, GasPetInfoDB, "ThrowAwayPet", CallBack, data)
end
-----------------------------------------------------------------------------------------
--@brief 幻兽重命名
function Gac2Gas:ChangePetName(Conn,petId,newName)
	local data = {
					["charId"]	= Conn.m_Player.m_uID,
					["petId"]	= petId,
					["newName"]	= newName
		}
		
	local function CallBack(succ,msgId)    
		if succ then
			Gas2Gac:ReturnChangePetName(Conn,petId,newName)
		end
		MsgToConn(Conn, msgId)
	end
	
	CallAccountManualTrans(Conn.m_Account, GasPetInfoDB, "ChangePetName", CallBack, data)
end
-----------------------------------------------------------------------------------------
--@brief 准备幻兽
function Gac2Gas:PreparePet(Conn,petId)
	local data = {
							["charId"]		= Conn.m_Player.m_uID,
							["petId"]	= petId
							}
		
	local function CallBack(succ)    
		if succ then
			Gas2Gac:ReturnPreparePet(Conn,petId)
		end
	end
	
	CallAccountManualTrans(Conn.m_Account, GasPetInfoDB, "PreparePet", CallBack, data)
end
-----------------------------------------------------------------------------------------
--@brief 使用幻兽灵石学技能
function Gac2Gas:UsePetSkillStone(Conn,ItemName,nRoomIndex, nPos)
	local data = {
							["charId"]		= Conn.m_Player.m_uID,
							["ItemName"]	= ItemName,
							["nRoomIndex"]	= nRoomIndex,
							["nPos"]		= nPos
							}
		
	local function CallBack(succ,msgId,itemId,skillName,posIndex,petId)    
		if succ then
			Gas2Gac:ReturnUsePetSkillStone(Conn,petId,skillName,posIndex)
		end
		MsgToConn(Conn, msgId)
	end
	
	CallAccountManualTrans(Conn.m_Account, GasPetInfoDB, "UsePetSkillStone", CallBack, data)
end
