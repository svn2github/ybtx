CEssayQuestion = class()

local function AnswerQuestionFaild(Conn, QuestName)
	Gac2Gas:GiveUpQuest(Conn,QuestName)
end

function CEssayQuestion.CreateAnswerTbl(Conn, I)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return 
	end
	Conn.m_Player.m_AnswerQuestionResultTbl = nil
	Conn.m_Player.m_AnswerQuestionResultTbl = {}
end

function CEssayQuestion.AnswerQuestion(Conn,QuestionStoreName,QuestionId, Answer, QuestName, ID, EntityId)
	if g_QuestionStoreTbl[QuestionStoreName] == nil then
		return 
	end
	local questionTbl = g_QuestionStoreTbl[QuestionStoreName](QuestionId)
	local count = table.maxn(g_QuestionStoreTbl[QuestionStoreName])
	if questionTbl then
		local QuestionAnswer = questionTbl("Answer")
		local Result = false
		if QuestionAnswer then
			if Answer == QuestionAnswer then
				--Conn.m_Player.m_rightNum = Conn.m_Player.m_rightNum + 1
				--MsgToConn(Conn, 3000, "回答正确!!!!")
				Result = true
				
			else
				--Conn.m_Player.m_errorNum = Conn.m_Player.m_errorNum + 1
				--MsgToConn(Conn, 3000, "回答错误000000000000000000!!!!")
				--AnswerQuestionFaild(Conn, QuestName)
				Result = false
			end
		end
		if not IsCppBound(Conn.m_Player) or not Conn.m_Player.m_AnswerQuestionResultTbl then
			return 
		end
		
		Conn.m_Player.m_AnswerQuestionResultTbl[ID] = Result
		Gas2Gac:RetAnswerResult(Conn, QuestionId, Result,QuestionStoreName)
	end	
end


local function StatisticRightAndWrong (Conn)
	local right = 0
	local wrong = 0
	if not IsCppBound(Conn.m_Player) or Conn.m_Player.m_AnswerQuestionResultTbl == nil then
		return 
	end
	for i, v in pairs( Conn.m_Player.m_AnswerQuestionResultTbl) do
		if v == true then
			right = right + 1
		else
			wrong = wrong + 1
		end		
	end
	return right, wrong
end

local function ShowFinishQuestWnd(Conn, sQuestName)

	---Gas2Gac:RetShowFinishQuest(Conn,sQuestName)

end

--完成答题给予奖励
function CEssayQuestion.AnswerQuestionFinish(Conn, QuestionCount, QuestName, QuestionMode, AnswerRightNum, ID)
	if not IsCppBound(Conn) then
		return 
	end
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	if Player.m_AnswerQuestionResultTbl == nil then
		return 
	end
	local Count = table.maxn(Player.m_AnswerQuestionResultTbl)
	if Count == QuestionCount then
		
	end
	if AnswerRightNum == 0 then
		return 
	end
	local rightCount, wrongCount = StatisticRightAndWrong( Conn)
	if QuestionMode ~= 2 and QuestionMode ~= 3 then
		local Result = nil 
		if rightCount >= AnswerRightNum then
			Result = true
		end
		if QuestName ~= nil and QuestName ~= "" and Result then 
			--如果答对了,并且是一个任务
			if g_QuestNeedMgr[QuestName] and g_QuestNeedMgr[QuestName]["答题"] then
				local PlayerId = Player.m_uID
				local data = 
				{
					["iNum"] = 1,
					["sQuestName"] = QuestName,
					["sVarName"] = "答题",
					["char_id"] = PlayerId
				}
				local function CallBack(isSuccess)
					if isSuccess and IsCppBound(Player) then
						Gas2Gac:RetAddQuestVar(Player.m_Conn, QuestName, "答题", 1)
						Gas2Gac:ReturnFinishInfo(Player.m_Conn, QuestName, ID)
						CRoleQuest.ShowFinishQuestWnd(Player.m_Conn, QuestName, ID)
					end
				end
				CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "AddQuestVarNum", CallBack, data)
			end
		end
		--ShowFinishQuestWnd(Conn, QuestName)
	else
		if QuestionMode == 2 then
			if Count == QuestionCount then
				local Result = false
				if math.floor((rightCount/ QuestionCount)*10) >= 6 then
					Result = true
				end
				Gas2Gac:ShowBoxItemWnd( Conn, Result)
			end
		else
			
		end
	end
	Player.m_AnswerQuestionResultTbl = nil 
end

