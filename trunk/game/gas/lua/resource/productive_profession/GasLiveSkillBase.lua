CLiveskillBase = class()
local LiveSkillBaseDB = "LiveSkillBaseDB"

function  CLiveskillBase.GetLiveSkillHasLearned(Conn)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	local uPlayerID = Player.m_uID
	
	local function CallBack(result)
		CLiveskillBase.RetLiveskillInfo(Conn,result)
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, LiveSkillBaseDB,"GetLiveSkillInfosDB", CallBack, parameters)
end


function CLiveskillBase.RetLiveskillInfo(Conn,result)
	Gas2Gac:ReturnGetLiveSkillHasLearnedBegin(Conn)
	local row = result:GetRowNum()
	for i =1,row do 
			Gas2Gac:ReturnGetLiveSkillHasLearned(Conn,result(i,1),result(i,2),result(i,3))
	end
			
	Gas2Gac:ReturnGetLiveSkillHasLearnedEnd(Conn)
end

function  CLiveskillBase.LearnLiveSkill(Conn, skillName)
	local Player = Conn.m_Player
	if Player == nil then return end
	local uPlayerID = Player.m_uID
	local uPlayerLevel = Player:CppGetLevel()
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160035)
		return 
	end
	local function CallBack(bFlag,result)--,MLRes
			
			if not bFlag then
				if IsNumber(result) then
					MsgToConn(Conn,result)
				end
				return
			end
			MsgToConn(Conn,9529)
			CLiveskillBase.RetLiveskillInfo(Conn,result.SkillsInfo)
			--更新佣兵等级追踪窗口
			--UpdateMercenaryLevelTraceWnd(Conn, MLRes)
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["uPlayerLevel"]	= uPlayerLevel
	parameters["skillName"]	= skillName
	CallAccountManualTrans(Conn.m_Account, LiveSkillBaseDB,"LearnLiveSkillDB", CallBack, parameters)
end

function CLiveskillBase.WashLiveSkill(Conn,sSkillName)
	local Player = Conn.m_Player
	if Player == nil then return end
	local uPlayerID = Player.m_uID
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160037)
		return 
	end
	local function CallBack(result)
			if IsNumber(result) then
				MsgToConn(Conn,result)
				return
			end
			MsgToConn(Conn,9533)
			CLiveskillBase.RetLiveskillInfo(Conn,result)
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["sSkillName"]	= sSkillName
	CallAccountManualTrans(Conn.m_Account, LiveSkillBaseDB,"WashLiveSkill", CallBack, parameters)
end