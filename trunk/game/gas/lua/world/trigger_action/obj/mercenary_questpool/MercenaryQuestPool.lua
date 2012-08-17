local GetDistance = g_GetDistance
local g_FewMercenaryQuestMgr = g_FewMercenaryQuestMgr
local CallAccountManualTrans = CallAccountManualTrans
local MsgToConn = MsgToConn
local QuestState = QuestState
local IsServerObjValid = IsServerObjValid
local os = os
local loadstring = loadstring
local g_GetPlayerInfo = g_GetPlayerInfo
local GetPlayerTempQuestList = CRoleQuest.GetPlayerTempQuestList
local PlayerSendTempQuestList = CRoleQuest.PlayerSendTempQuestList

local QuestUpYbLevelToG = {--完成指定任务后,直接升入G级,成为G级佣兵
			[1] = "定级考核",
			[2] = "G级佣兵考核",
			[3] = "G级佣兵考验",
			}



local Entry = CreateSandBox(...)

local function IsSameTime(CurTime, DBTime)
	local nowDate = os.date("*t",CurTime)
	local dbDate = os.date("*t",DBTime)
	if (nowDate.year == dbDate.year)
		and (nowDate.month == dbDate.month)
		and (nowDate.day == dbDate.day) then
		return true
	else
		return false
	end
end

function Entry.SendQuestTbl(Player, QuestTbl)
	local PlayerId = Player.m_uID
	local Level = Player:CppGetLevel()
	local MercLev = Player.m_uMercenaryLevel
	local QuestList = nil
	
	if Player.m_PlayerTempQuestList then
		if Level == Player.m_PlayerTempQuestList.m_Level
			and MercLev == Player.m_PlayerTempQuestList.m_MercLev then
				
				QuestList = Player.m_PlayerTempQuestList
				
		end
	end
	if not QuestList then
		QuestList = GetPlayerTempQuestList(Player)
	end
	--发送筛选出来的任务
	for index=1, 3 do
		if QuestList[index] then
			for i=1, 3 do
				if QuestList[index][i] then
					Gas2Gac:RetSendMercQuestPool(Player.m_Conn,QuestList[index][i][1],QuestList[index][i][2])
				end
			end
		end
	end
end


function Entry.Exec(Conn, QuestObj, ObjName, GlobalID)
	local Player = Conn.m_Player
	if not IsServerObjValid(Player) then
		return
	end
	
	local PlayerId = Player.m_uID
	
	if GetDistance(Player,QuestObj) > 6 then
		return 
	end
	
	local function CallBack(QuestTbl)
		if IsServerObjValid(Player) then
			local XYQuestName = ""
			--得到玩家等级段的所有任务并发送
			Entry.SendQuestTbl(Player, QuestTbl)
			Gas2Gac:RetShowMercQuestPoolWndEnd(Player.m_Conn,GlobalID,XYQuestName)
			
			--存储下一次将会出现的任务
			PlayerSendTempQuestList(Player)
		end
	end
	
	local function QuestCheck_CallBackFun(resQuestState)
		if IsServerObjValid(Player) then
			if resQuestState == QuestState.finish then
				local data= {}
				data["PlayerId"] = PlayerId
				CallAccountManualTrans(Player.m_Conn.m_Account, "RoleQuestDB", "GetAllYbFinishQuest", CallBack, data)
			else
				MsgToConn(Player.m_Conn, 3061)
			end
		end
	end
	
	QuestCheck_CallBackFun(QuestState.finish)
	--CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "GetQuestState", QuestCheck_CallBackFun, {PlayerId, QuestUpYbLevelToG[Camp]})
end

return Entry
