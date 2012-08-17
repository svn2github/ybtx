cfg_load "skill/SkillPart_Common"

local function AddQuestFun(Conn, QuestName, LoopNum)
	local Player = Conn.m_Player
	local PlayerId = Player.m_uID
	local function CallBack(result)
		if IsCppBound(Player) then
			if result[1] then
				if result[2] == QuestState.init then
					Gas2Gac:RetGiveUpQuest(Player.m_Conn,QuestName,true) 
				end
				CRoleQuest.AddQuest_DB_Ret(Player,QuestName,result[3],result[4],result[5],result[6],result[7],result[8])	
				
			else
				if IsString(result[2]) then
					MsgToConn(Conn, 3001, result[2])
				elseif IsNumber(result[2]) then
					if IsTable(result[3]) then
						MsgToConn(Conn, result[2], unpack(result[3]))
					else
						MsgToConn(Conn, result[2])
					end
				end
			end
		end
	end
	
	--Renton传如果任务的需求是技能需求，则把任务需求表的那一行传进去
	local questInfoTbl = g_QuestNeedMgr[QuestName]
	local skillNeedQuestInfoTbl = {}
	if questInfoTbl ~= nil then
		for k,v in pairs(questInfoTbl) do
			local temp = {}
			if "技能需求" == v.NeedType then
				temp = v
				temp.skilllearntype = SkillPart_Common(temp.SkillName,"SkillLearnType")
				skillNeedQuestInfoTbl[k] = temp
			end
		end
	end
	
	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["PlayerId"] = PlayerId
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	parameter["skillNeedQuestInfoTbl"] = skillNeedQuestInfoTbl
	parameter["LoopNum"] = LoopNum
	
	CallAccountAutoTrans(Conn.m_Account, "RoleQuestDB", "GMAddQuest", CallBack, parameter)
end

local function FinishQuestFun(Conn, QuestName)
	local Player = Conn.m_Player
	local PlayerId = Player.m_uID
	Player:OnSavePlayerSoulTickFunc(nil,Player)
	local function CallBack(result)
		local Player = g_GetPlayerInfo(PlayerId)
		if Player then
			if not result[1] then
				if IsString(result[2]) then
					MsgToConn(Conn, 3003, result[2])
				elseif IsNumber(result[2]) then
					if IsTable(result[3]) then
						MsgToConn(Conn, result[2], unpack(result[3]))
					else
						MsgToConn(Conn, result[2])
					end
				end
			else
				CRoleQuest.FinishQuest_DB_Ret(Player,QuestName,result[2],result[3],result[4],result[5],result[6],result[7],result[8],result[9])
				if result[10] then
					local addsname = result[8][1]
					local addres = result[10]
					CRoleQuest.AddQuest_DB_Ret(Player,addsname,addres[2],addres[3],addres[4],addres[5],addres[6],addres[7])
				end
			end
		end
	end
	
	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["PlayerId"] = PlayerId
	parameter["PlayerLevel"] = Player:CppGetLevel()
	parameter["sceneName"] = Player.m_Scene.m_SceneName

	OnSavePlayerExpFunc({Conn.m_Player})
	CallAccountAutoTrans(Conn.m_Account, "RoleQuestDB", "GMFinishQuest", CallBack, parameter)
end


function GMCommand.Executor.AddQuest(Conn, QuestName)
	if not Quest_Common(QuestName) then
		MsgToConn(Conn, 3086)
		return
	end
	if Quest_Common(QuestName, "任务类型") == 10 then
		GMCommand:print(Conn, "This task is a loop task. Please use command \"addloopquest\".")
		return
	end
	AddQuestFun(Conn, QuestName)
end

function GMCommand.Executor.TargetAddQuest(Conn, TargetName, QuestName)
	if not Quest_Common(QuestName) then
		MsgToConn(Conn, 3086)
		return
	end
	if Quest_Common(QuestName, "任务类型") == 10 then
		GMCommand:print(Conn, "This task is a loop task. Please use command \"addloopquest\".")
		return
	end
	local Player = g_GetPlayerInfoByName(TargetName)
	if not IsCppBound(Player) then
		MsgToConn(Conn,19)
		return
	end
	AddQuestFun(Player.m_Conn, QuestName)
end

function GMCommand.Executor.AddLoopQuest(Conn, QuestName, LoopNum)
	if not Quest_Common(QuestName) then
		MsgToConn(Conn, 3086)
		return
	end
	if Quest_Common(QuestName, "任务类型") ~= 10 then
		GMCommand:print(Conn, "This task is not a loop task. Please use command \"addquest\".")
		return
	end
	AddQuestFun(Conn, QuestName, LoopNum)
	local tbl = GetCfgTransformValue(true, "Quest_Common", QuestName, "环属性", "1")
	Gas2Gac:RetGetQuestLoop(Conn, tbl[1], LoopNum)
end

function GMCommand.Executor.FinishQuest(Conn, QuestName)
	FinishQuestFun(Conn, QuestName)
end

function GMCommand.Executor.TargetFinishQuest(Conn, TargetName, QuestName)
	local Player = g_GetPlayerInfoByName(TargetName)
	if Player == nil then
		MsgToConn(Conn,19)
		return
	end
	FinishQuestFun(Player.m_Conn, QuestName)
end

--1代表时间限次有效
function GMCommand.Executor.QuestTimeLimit(Conn, Limit)
	if Limit == 1 then
		g_GMTimeCountLimit = 1
	else
		g_GMTimeCountLimit = 0
	end
end
