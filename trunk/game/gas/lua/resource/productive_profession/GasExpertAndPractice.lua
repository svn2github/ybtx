
CExpertAndPractice = class()
local ExpertAndPracticeDB = "ExpertAndPracticeDB"

--【获得锻造熟练度信息】
function  CExpertAndPractice.GetPracticedInfo(Conn,nSkillType)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	local uPlayerID = Player.m_uID
	
	local function CallBack(result)
			
			Gas2Gac:ReturnGetPracticedInfoBegin(Conn)
			
			for i = 1,#result do
				Gas2Gac:ReturnGetPracticedInfo(Conn,result[i][1],result[i][2])
			end
			local tblExpert = result.tblExpert
			Gas2Gac:ReturnGetPracticedInfoEnd(Conn,tblExpert[1] or "", tblExpert[2] or 0)
	end
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["nSkillType"]	= nSkillType
	CallAccountManualTrans(Conn.m_Account, ExpertAndPracticeDB,"GetPracticedInfoDB", CallBack, parameters)
end

function  CExpertAndPractice.GetPracticedInfoForNpcPanel(Conn,nSkillType)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	local uPlayerID = Player.m_uID
	
	local function CallBack(result)
			
			Gas2Gac:ReturnGetPracticedInfoForNpcPanelBegin(Conn)
			for i = 1,#result do
				Gas2Gac:ReturnGetPracticedInfoForNpcPanel(Conn,result[i][1],result[i][2])
			end
			local tblExpert = result.tblExpert
			Gas2Gac:ReturnGetPracticedInfoForNpcPanelEnd(Conn,tblExpert[1] or "", tblExpert[2] or 0)
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["nSkillType"]	= nSkillType
	CallAccountManualTrans(Conn.m_Account, ExpertAndPracticeDB,"GetPracticedInfoDB", CallBack, parameters)
end



--【专精学习】
function  CExpertAndPractice.LearnLiveSkillExpert(Conn,sSkillName,sEquipType)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160036)
		return 
	end
	local uPlayerID = Player.m_uID
	
	local function CallBack(bFlag,Arg1,Arg2)
			if not bFlag then
				if IsNumber(Arg1) then
					if Arg2 then
						MsgToConn(Conn,Arg1,Arg2)
					else
						MsgToConn(Conn,Arg1)
					end
				end
			else
				MsgToConn(Conn,9556)
				Gas2Gac:ReturnGetLiveSkillExpert(Conn, Arg1.sEquipType, Arg1.uLevel)
			end
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["sSkillName"] = sSkillName 
	parameters["sEquipType"] = sEquipType
	CallAccountManualTrans(Conn.m_Account, ExpertAndPracticeDB,"StudyExpertDB", CallBack, parameters)
end

--【获得专精信息】
function CExpertAndPractice.GetLiveSkillExpert(Conn,sSkillName)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	local uPlayerID = Player.m_uID
	
	local function CallBack(result)
		if #result > 0 then
			Gas2Gac:ReturnGetLiveSkillExpert(Conn, result[1], result[2])
		else
			Gas2Gac:ReturnGetLiveSkillExpert(Conn, "", 0)
		end
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["sSkillName"] = sSkillName
	CallAccountManualTrans(Conn.m_Account, ExpertAndPracticeDB,"GetExpertDB", CallBack, parameters)
end

--【遗忘专精】
function CExpertAndPractice.ForgetLiveSkillExpert(Conn)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160038)
		return 
	end
	local uPlayerID = Player.m_uID
	
	local function CallBack(result)
		if not result then return end
		if IsNumber(result) then
			MsgToConn(Conn,result)
			return
		end
		MsgToConn(Conn,9569)
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, ExpertAndPracticeDB,"WashExpert", CallBack, parameters)
end
