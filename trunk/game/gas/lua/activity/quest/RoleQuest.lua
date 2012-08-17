cfg_load "player/LevelExp_Common"
gas_require "world/trigger_action/item/item_trigger/CreateAreaScene"
gac_gas_require "activity/quest/QuestBase"

cfg_load "item/Mercenary_Quest_Server"
cfg_load "skill/SkillPart_Common"

CRoleQuest = class()

--local function CountReturn(Player,str,LoopNum)
--	local playerlevel = Player:CppGetLevel()
--	local PlayerYbLevel = Player.m_uMercenaryLevel or 0
--	str = string.gsub( str, "yblevel", PlayerYbLevel)
--	str = string.gsub( str, "level", playerlevel)
--	str = string.gsub( str, "loop", LoopNum)
--	local count = assert(loadstring( "return " .. str))()
--	count = math.floor(count)
--	return count
--end

local function ClearQuestState( Player ,sQuestName)
--	local Player = g_GetPlayerInfo(PlayerId)
--	if not Player then
--		return
--	end
	if Quest_Common(sQuestName, "触发技能") then
		local BuffName = Quest_Common(sQuestName, "触发技能", "Arg")
		Player:ClearState(BuffName)
	end
	if Quest_Common(sQuestName, "交任务状态需求") then
		local Keys = Quest_Common:GetKeys(sQuestName, "交任务状态需求")
		for k = 1, table.getn(Keys) do
			local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "交任务状态需求", Keys[k], "Function")
			local needbuff = tbl[1]		--需求的buff名
			Player:ClearState(needbuff)
		end
	end
	if Quest_Common(sQuestName, "曾经达到状态需求") then
		local Keys = Quest_Common:GetKeys(sQuestName, "曾经达到状态需求")
		for k = 1, table.getn(Keys) do
			local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "曾经达到状态需求", Keys[k], "Function")
			local needstate = tbl[1]		--需求的buff名
			Player:ClearState(needstate)
		end
	end
end

local function SendPlayerFirstInfo_Add(Player,QuestName)
	if QuestName == "战斗，佣兵的天职"
		or QuestName == "与邪恶作战"
		or QuestName == "老兵的教导" then
		Player:IsFirstTimeAndSendMsg("任务1", 1001, sMsg)
		
	elseif QuestName == "成为一个佣兵"		--任务面板的使用
		or QuestName == "光明指引你的到来"
		or QuestName == "欢迎新佣兵" then
		Player:IsFirstTimeAndSendMsg("任务2", 1002, sMsg)
		
--	elseif QuestName == "佣兵日志的作用"		--区域面板系统的使用
--		or QuestName == "学会使用你的日志"
--		or QuestName == "活用你的佣兵日志" then
--		Player:IsFirstTimeAndSendMsg("任务3", 1003, sMsg)
		
--	elseif QuestName == "装备强化与升级"		--装备升级与强化的使用
--		or QuestName == "装备提升：魂根与注魂"
--		or QuestName == "装备成长：魂根与注魂" then
--		Player:IsFirstTimeAndSendMsg("任务4", 1004, sMsg)
	end
end

local function SendPlayerFirstInfo_Finish(Player,QuestName)
	if QuestName == "初学战斗（艾米）"		--区域面板系统的使用
		or QuestName == "初学战斗（神圣）"
		or QuestName == "初学战斗（修斯）" then
		Player:IsFirstTimeAndSendMsg("任务5", 2009, sMsg)
		
	elseif QuestName == "新佣兵的成长之路(艾米)"		--区域面板系统的使用
		or QuestName == "新佣兵的成长之路(神圣)"
		or QuestName == "新佣兵的成长之路(修斯)" then
		Player:IsFirstTimeAndSendMsg("任务6", 1005, sMsg)
		
	end
end

local function AddPlayerExp_DB_Ret(Player,AddExpNumTbl)
	if not IsCppBound(Player) then
		return
	end
	
	local Conn = Player.m_Conn
	local DBLevel = AddExpNumTbl["Level"]
	local DBExp = AddExpNumTbl["Exp"]
	local AddExp = AddExpNumTbl["AddExp"]
	local TeamAwardExp = AddExpNumTbl["TeamAwardExp"] or 0
	local uInspirationExp = AddExpNumTbl["uInspirationExp"]
	local DirectTbl = AddExpNumTbl["DirectTbl"]
	
	local OldLevel = Player:CppGetLevel()
	--print(DBLevel,OldLevel)
	if DBLevel ~= OldLevel then
		CLevelUpGas.SetLevel(Player,DBLevel)
		CGasTeam.SetTeamMemberLevel(Player.m_uID)
		Player:SetSkillToPlayer()
	end
	Player.m_uLevelExp = DBExp
	
	if DBLevel==3 then
		Player:IsFirstTimeAndSendMsg("有技能可学", 2006)
	elseif DBLevel==60 then 
		Gas2Gac:NotifyOpenPanel(Conn,2)
	elseif DBLevel==120 then
		Gas2Gac:NotifyOpenPanel(Conn,2)
		Gas2Gac:NotifyOpenPanel(Conn,3)
	end
	--print(DBLevel,DBExp,AddExpNumTbl[3], AddExpNumTbl[4])
	--print(Player.m_KillNpcExp)
	Gas2Gac:SetExp( Conn,AddExp, uInspirationExp, TeamAwardExp, DBExp,LevelExp_Common(DBLevel, "ExpOfCurLevelUp") )
	if DirectTbl then
		SendPlayerFinishDirect(Player, DirectTbl)
	end
end

local function IncreQuestItemVar_DB_Ret(Player,ItemType, ItemName, nCount, QuestItemTbl)
	if QuestItemTbl then
		local num = table.getn(QuestItemTbl)
		for i=1,num do
			local res = QuestItemTbl[i]
			Gas2Gac:RetQuestVar(Player.m_Conn, res["QuestName"], res["ItemName"],res["DoNum"])			
		end
	end
	return
end

local function add_item_DB_Ret(Player, nBigId, sItemName,nCount,res)--加了一个绑定类型参数
--	local Player = g_GetPlayerInfo(uCharId)
--	if not IsCppBound(Player) then
--		return
--	end
	--IncreQuestItemVar_DB_Ret(Player,nBigId, sItemName, nCount,IncreQuestItemVarTbl)
end

local function AddQuest_DB_Ret(Player,sQuestName,give_items,YbItemIndex,QuestVarTbl,DirectTbl,SkillQuestTbl,LoopQuestTbl)
--	Player.m_tblDoingQuests[sQuestName] = {}
--	Player.m_DoingQuest[sQuestName] = true
	
	local LoopName, LoopNum = "", 0
	if LoopQuestTbl then
		LoopName, LoopNum = LoopQuestTbl[1], LoopQuestTbl[2]
	end
	Gas2Gac:RetAcceptQuest(Player.m_Conn, sQuestName, YbItemIndex, LoopName, LoopNum)
	
	--增加任务变量
	if Quest_Common(sQuestName) then
		local BeginEssayQuestion = Quest_Common(sQuestName, "开启答题")
		if BeginEssayQuestion and BeginEssayQuestion~="" then
			--Gas2Gac:RetShowEssayQuestionWnd(Player.m_Conn,  BeginEssayQuestion, true, sQuestName)
	--		local Num = table.getn(BeginEssayQuestion[1].Arg)
			local QuestionTypeName = BeginEssayQuestion
			--Gas2Gac:RetInitEssayQuestion(Player.m_Conn,QuestionTypeName)	
			Gas2Gac:RetShowEssayQuestionWnd(Player.m_Conn, QuestionTypeName, sQuestName, 1)	
	--		if Num >= 1 then
	--			local randnum = math.random(1,Num)
	--			local selnum = BeginEssayQuestion[1].Arg[randnum]
	-- 			Gas2Gac:RetShowEssayQuestionWnd(Player.m_Conn, QuestionStorAndCount, selnum, sQuestName)
	--		end
	--		local question = g_QuestStore[QuestionStoreName][Num]
		end
		
		if Quest_Common(sQuestName, "触发技能") then
			local skillsName = Quest_Common(sQuestName, "触发技能", "Subject")
			local pos = CFPos:new()
			Player:GetPixelPos(pos)
			Player:PlayerDoPosSkill(skillsName,1,pos)
		end
		
		local questtheater = Quest_Common(sQuestName, "开启剧情")
		if questtheater and questtheater~="" then
	 		Player.m_Scene.m_TheaterMgr:RunTriggerTheater(questtheater, Player:GetEntityID())
		end
		
		--Renton,如果成功的话+技数
		if SkillQuestTbl ~= nil then
			for i= 1,#SkillQuestTbl do 
				if SkillQuestTbl[i].IsTrue then
					local quest_playId = SkillQuestTbl[i]["char_id"]
					local quest_questname = SkillQuestTbl[i]["sQuestName"]
					local quest_questvarname = SkillQuestTbl[i]["sVarName"]
					local quest_inum = SkillQuestTbl[i]["iNum"]
					local player = g_GetPlayerInfo(quest_playId)
					if player then
						Gas2Gac:RetAddQuestVar(player.m_Conn, quest_questname, quest_questvarname,quest_inum)
					end
				end
			end
		end
		
		--若任务有特殊的事件需求，加任务计数
		if QuestVarTbl then
			for i=1, #QuestVarTbl do
				if QuestVarTbl[i] then
					local res = QuestVarTbl[i][Player.m_uID]
					for j=1, #res do
						Gas2Gac:RetAddQuestVar(Player.m_Conn, res[j][1], res[j][2], res[j][3])
					end
				end
			end
		end
		
		CRoleQuest.NpcFromQuest_Add(Player, sQuestName)
	end
	
--	if QuestNeedVarTbl then
--		local num = table.getn(QuestNeedVarTbl)
--		for k = 1, num do
--			local res = QuestNeedVarTbl[k]
--			Gas2Gac:RetQuestVar(Player.m_Conn, res["QuestName"],res["ItemName"],res["DoNum"])
--		end
--	end
	for i=1, table.getn(give_items) do
		local give_type,give_name,num,res=give_items[i][1],give_items[i][2],give_items[i][3],give_items[i][4]
		--add_item_DB_Ret(Player, give_type, give_name,num,res,IncreQuestItemVarTbl[i])
		add_item_DB_Ret(Player, give_type, give_name,num,res)
	end
	
	SendPlayerFirstInfo_Add(Player,sQuestName)
	SendPlayerFinishDirect(Player, DirectTbl)
	return true
end

--By PlayerID
local function add_item_DB_Ret_By_Id(PlayerId, nBigId, sItemName,nCount,res)--加了一个绑定类型参数
--	local Player = g_GetPlayerInfo(uCharId)
--	if not IsCppBound(Player) then
--		return
--	end
	
	local nSoulNum = 0
	if g_ItemInfoMgr:IsSoulPearl(nBigId) then
			sItemName, nSoulNum = g_ItemInfoMgr:GetSoulpearlInfo(sItemName)
	end
	--IncreQuestItemVar_DB_Ret(Player,nBigId, sItemName, nCount,IncreQuestItemVarTbl)
end

--最后一个参数,是true的话,那么客户端会显示需求个数
local function delete_item_DB_Ret(Player, nBigId, sItemName, nCount, res, IsShowMsg)
	--local Player = g_GetPlayerInfo(uCharId)
	--if IsCppBound(Player) and IsCppBound(Player.m_Conn) then
--		for n=1,#res do
--			if IsShowMsg then
--			else
--				Gas2Gac:RetQuestDelItemEnd(Player.m_Conn,res[n].m_nRoomIndex,res[n].m_nPos)
--			end
--		end
	--end
end

--任务失败(服务端触发的任务失败)
local function QuestFailure_DB_Ret(Player,sQuestName)
	Player.m_DoingQuest[sQuestName] = nil
	ClearQuestState(Player, sQuestName)  --计时任务 失败. 清除玩家状态.
	Gas2Gac:NotifyQuestFailure(Player.m_Conn,sQuestName)
end

local function GiveQuestAward_DB_Ret(Player,sQuestName,AddExpTbl,LoopNum)
	--声望奖励
	--local countstr = Quest_Common(sQuestName, "声望奖励")
	--if countstr and countstr~="" then
	--	local count = CountReturn(Player,countstr,LoopNum)
	--	player:AddPrestige(count)	
	--end
	--经验奖励
	if AddExpTbl then
		AddPlayerExp_DB_Ret(Player,AddExpTbl)
	end
end

local function GiveQuestIntegral_DB_Ret(Player,MercenaryLevelTbl, sQuestName)
	if MercenaryLevelTbl then
		if g_AllMercenaryQuestMgr[sQuestName] then
			local GetQuestLevel = Quest_Common(sQuestName, "接任务最小佣兵等级")
			if GetQuestLevel and GetQuestLevel~="" then
				Player.m_uAddIntegral = MercenaryLevelTbl[3]
				local PlayAddIntegral = Player.m_uAddIntegral
				MsgToConn(Player.m_Conn, 3047, GetQuestLevel, PlayAddIntegral)
			end
		end
		
		Player.m_uMercenaryIntegral = MercenaryLevelTbl[2]
		Gas2Gac:RetSetMercenaryIntegral(Player.m_Conn, Player.m_uMercenaryIntegral )
	end
end

--任务完成播放一个特效
local function QuestEffect(Player, AddExpTbl, MercenaryLevelTbl)
	local IsPlayer = true
	if AddExpTbl then
		local DBLevel = AddExpTbl["Level"]
		local OldLevel = Player:CppGetLevel()
		if DBLevel ~= OldLevel then
			IsPlayer = false
		end
	end
	
	if IsPlayer and MercenaryLevelTbl then
		local DBYbLevel = MercenaryLevelTbl[1]
		if Player.m_uMercenaryLevel ~= DBYbLevel then
			IsPlayer = false
		end
	end
	
	if IsPlayer then
		Gas2Gac:UseItemPlayerEffect(Player.m_Conn,"fx/setting/other/other/finishquest/create.efx","finishquest/create")
	end
end

--完成任务(发放任务奖励，清空任务物品，设置任务状态)
local function FinishQuest_DB_Ret(Player,sQuestName,Del_ItemTbl,Add_ItemTbl,AddExpTbl,SoulRet,MercenaryLevelTbl,DirectTbl,LoopQuestTbl,uAddSoulNum)
	ClearQuestState( Player, sQuestName)  --清除任务状态
--	local Player = g_GetPlayerInfo(self.m_PlayerId)
--	if not IsCppBound(Player) then
--		return
--	end

	--任务完成播放一个特效
	QuestEffect(Player, AddExpTbl, MercenaryLevelTbl)
	
	local questtheater = Quest_Common(sQuestName, "任务完成剧情")
	if questtheater and questtheater ~= "" then
		Player.m_Scene.m_TheaterMgr:RunTriggerTheater(questtheater, Player:GetEntityID())
	end
	
	local QuestAppellation = Quest_Common(sQuestName, "获得称谓")
	if QuestAppellation and QuestAppellation ~= "" then
		--调用加称谓的接口
		CGasAppellationAndMoral.AddNewAppellation(Player, QuestAppellation)
	end
	
	--金钱奖励，经验奖励，物品奖励
	GiveQuestAward_DB_Ret(Player,sQuestName,AddExpTbl,LoopQuestTbl[2])
	--佣兵积分奖励
	GiveQuestIntegral_DB_Ret(Player,MercenaryLevelTbl, sQuestName)
	
	if SoulRet then
		Player.m_uSoulValue = SoulRet + Player.m_uKillDropSoulCount
		if SoulRet ~= 0 then
			Gas2Gac:ReturnModifyPlayerSoulNum(Player.m_Conn,Player.m_uSoulValue)
			if uAddSoulNum > 0 then
				Gas2Gac:ReturnCharAddSoulMsg(Player.m_Conn,uAddSoulNum)
			end
		end
	end
	
	--客户端清空任务道具返回客户端
	for i=1,table.getn(Del_ItemTbl) do
		local type,name,num,res = Del_ItemTbl[i]["nType"],Del_ItemTbl[i]["sName"],Del_ItemTbl[i]["nCount"],Del_ItemTbl[i]["res"]
		delete_item_DB_Ret(Player,type,name,num,res)
	end
	--物品奖励返回客户端
	for i=1,table.getn(Add_ItemTbl) do
		local itemtype,name,num,res,msgType = Add_ItemTbl[i]["nType"],Add_ItemTbl[i]["sName"],Add_ItemTbl[i]["nCount"],Add_ItemTbl[i]["res"],Add_ItemTbl[i]["msgType"]
		--add_item_DB_Ret(Player,itemtype,name,num,res,Add_ItemTbl[i]["QuestItemVar"])
		add_item_DB_Ret(Player,itemtype,name,num,res)
		if itemtype >= 5 and itemtype <= 9 then
			Gas2Gac:AddItemMsgToConn(Player.m_Conn, res[1][1])
		end
		if msgType then
			--加一个消息提示,说给你物品了
			--(1表示宝石,2表示挑战任务卷轴,3表示领域卷轴,4表示佣兵卡牌)
			MsgToConn(Player.m_Conn, 300100 + msgType)
		end
	end
	--step7：写log
--	Player.m_tblDoingQuests[sQuestName] = nil
--	Player.m_DoingQuest[sQuestName] = nil
	CRoleQuest.NpcFromQuest_Del(Player, sQuestName)
	SendPlayerFirstInfo_Finish(Player,sQuestName)
	SendPlayerFinishDirect(Player, DirectTbl)
	Gas2Gac:RetFinishQuest(Player.m_Conn, sQuestName, LoopQuestTbl[1])
	return true
end

local function DeleteQuest_DB_Ret(Player,Del_Item_Tbl,sQuestName, IsShowMsg)
	for i=1,table.getn(Del_Item_Tbl) do
		local type,name,num,res = Del_Item_Tbl[i]["nType"],Del_Item_Tbl[i]["sName"],Del_Item_Tbl[i]["nCount"],Del_Item_Tbl[i]["res"]
		delete_item_DB_Ret(Player,type,name,num,res,IsShowMsg)
	end
	
	--local Player = g_GetPlayerInfo(self.m_PlayerId)
--	Player.m_tblDoingQuests[sQuestName] = nil
--	Player.m_DoingQuest[sQuestName] = nil
	local bNeedSaveMagic = false
	if Quest_Common(sQuestName) then
		if Quest_Common(sQuestName, "触发技能") then
			local BuffName = Quest_Common(sQuestName, "触发技能", "Arg")
			Player:ClearState(BuffName)
			bNeedSaveMagic = true
		end
		if Quest_Common(sQuestName, "交任务状态需求") then
			local Keys = Quest_Common:GetKeys(sQuestName, "交任务状态需求")
			for k = 1, table.getn(Keys) do
				local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "交任务状态需求", Keys[k], "Function")
				local needbuff = tbl[1]		--需求的buff名
				Player:ClearState(needbuff)
				bNeedSaveMagic = true
			end
		end
		if Quest_Common(sQuestName, "曾经达到状态需求") then
			local Keys = Quest_Common:GetKeys(sQuestName, "曾经达到状态需求")
			for k = 1, table.getn(Keys) do
				local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "曾经达到状态需求", Keys[k], "Function")
				local needstate = tbl[1]		--需求的buff名
				Player:ClearState(needstate)
				bNeedSaveMagic = true
			end
		end
		
		if bNeedSaveMagic then
			Player:OnSaveFightPropertyTick(nil,Player)
		end
		
		CRoleQuest.NpcFromQuest_Del(Player, sQuestName)
	end
end

local function DecreQuestItem_DB_Ret(Player,ItemName,DeleteQuestName,DelItemTbl)
--	if ChangeQuestItemVarTbl then
--		local num = table.getn(ChangeQuestItemVarTbl)
--		for i = 1, num do
--			local res = ChangeQuestItemVarTbl[i]
--			Gas2Gac:RetQuestVar(Player.m_Conn, res["QuestName"],res["ItemName"],res["DoNum"])
--		end
--	end
	
	if DelItemTbl then
		DeleteQuest_DB_Ret(Player,DelItemTbl,DeleteQuestName,true)
	end
	
	if DeleteQuestName then
		Gas2Gac:RetGiveUpQuest(Player.m_Conn,DeleteQuestName,true)
	end
--	if DelGiveQuestItemTbl then
--		if g_WhereGiveQuestMgr["Goods"][ItemName] then
--			local sQuestName = g_WhereGiveQuestMgr["Goods"][ItemName][1]
--			DeleteQuest_DB_Ret(Player,DelGiveQuestItemTbl,sQuestName)
--			Gas2Gac:RetGiveUpQuest(Player.m_Conn,sQuestName,true)
--			return
--		end
--	end
--	
--	if DelQuestPropItemTbl then
--		if g_QuestPropMgr[ItemName] then
--			local sQuestName = g_QuestPropMgr[ItemName][2]
--			DeleteQuest_DB_Ret(Player,DelQuestPropItemTbl,sQuestName)
--			Gas2Gac:RetGiveUpQuest(Player.m_Conn,sQuestName,true)
--			return
--		end
--	end
end

local function ShareQuest(Conn, QuestName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) or Quest_Common(QuestName) == nil then
		return
	end
	if Quest_Common(QuestName, "能否共享") ~= 1 then
		MsgToConn(Conn,3004)
		return
	end
	if Quest_Common(QuestName, "任务类型") == 10 then
		MsgToConn(Conn,3004)
		return
	end
	
	local TeamID = Player.m_Properties:GetTeamID()
	local PlayerName = Player.m_Properties:GetCharName()
	if TeamID == 0 then
		MsgToConn(Conn,3007)
		return
	end
	
	local PlayerId = Player.m_uID
	local ShareTime = os.time()
	local function CallBack(result)
		if IsCppBound(Player) then
			if not result[1] then
				MsgToConn(Player.m_Conn, result[2])
			else
				local TeamPlayerInfoTbl = result[2]
				if table.getn(TeamPlayerInfoTbl) == 0 then
					return
				end
				
				for i=1, table.getn(TeamPlayerInfoTbl) do
					local teammateid = TeamPlayerInfoTbl[i]["PlayerId"]
					if PlayerId ~= teammateid then
						
						if TeamPlayerInfoTbl[i]["IsCanShare"] then
			 				Gas2GacById:NotifyTeammateShareQuest(teammateid, TeamID, PlayerName, QuestName)
			 			else
			 				if TeamPlayerInfoTbl[i]["MsgID"] then
			 					MsgToConnById(teammateid, TeamPlayerInfoTbl[i]["MsgID"], unpack(TeamPlayerInfoTbl[i]["Arg"]))
			 				end
			 			end
						
				 	end
				end
				
			end
		end
	end
	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["PlayerId"] = PlayerId
	parameter["TeamID"] = TeamID
	parameter["PlayerCamp"] = Player:CppGetBirthCamp()
	parameter["GMTimeCountLimit"] = g_GMTimeCountLimit
	
	CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "ShareQuest", CallBack, parameter)
end

local function ResponseShareQuest(Conn, QuestTeamID, QuestName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	local PlayerId = Player.m_uID
	local TeamID = Player.m_Properties:GetTeamID()
	if TeamID == 0 or QuestTeamID ~= TeamID then
		return
	end
	
	local function CallBack(result)
		if IsCppBound(Player) then
			if not result[1] then
				if IsString(result[2]) then
					MsgToConn(Conn, 3001, result[2])
				elseif IsNumber(result[2]) then
					if IsTable(result[3]) then
						MsgToConn(Conn, result[2], unpack(result[3]))
					else
						MsgToConn(Conn, result[2])
					end
				end
			else
				--AddQuest_DB_Ret(Player,QuestName,result[2],result[3],result[4])
				AddQuest_DB_Ret(Player,QuestName,result[2],result[3],nil,result[5])
			end
		end
	end
	
	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["PlayerId"] = PlayerId
	parameter["PlayerCamp"] = Player:CppGetBirthCamp()
	parameter["PlayerLevel"] = Player:CppGetLevel()
	parameter["PlayerYbLevel"] = Player.m_uMercenaryLevel
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	parameter["GMTimeCountLimit"] = g_GMTimeCountLimit
	CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "AcceptQuest", CallBack, parameter)
end

local function GiveUpQuest(Conn,QuestName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	if Quest_Common(QuestName) == nil then
		return
	end
	
	local function CallBack(result)
		if IsCppBound(Player) then
			if not result[1] then
				if IsString(result[2]) then
					MsgToConn(Conn, 3002, result[2])
				elseif IsNumber(result[2]) then
					if IsTable(result[3]) then
						MsgToConn(Conn, result[2], result[3])
					else
						MsgToConn(Conn, result[2])
					end
				end
			else
				DeleteQuest_DB_Ret(Player,result[2],QuestName)
			end
			Gas2Gac:RetGiveUpQuest(Conn,QuestName,result[1])
		end
	end

	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["PlayerId"] = Player.m_uID
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "GiveUpQuest", CallBack, parameter)
end

local function AcceptQuestFun(Conn, QuestName, IsGM)
	local Player = Conn.m_Player
	local function CallBack(result)
		if IsCppBound(Player) then
			if not result[1] then
				if IsString(result[2]) then
					MsgToConn(Conn, 3001, result[2])
				elseif IsNumber(result[2]) then
					if IsTable(result[3]) then
						MsgToConn(Conn, result[2], unpack(result[3]))
					else
						MsgToConn(Conn, result[2])
					end
				end
			else
				AddQuest_DB_Ret(Player,QuestName,result[2],result[3],result[4],result[5],result[6],result[7])
				
				--若需要传送区域副本，则传送
				if Quest_Common(QuestName, "区域副本传送") then
					local areaFb = GetCfgTransformValue(true, "Quest_Common", QuestName, "区域副本传送")
					NewChangeScene(Conn, areaFb[1], areaFb[2], areaFb[3])
				end
			end
		end
	end
	
	--Renton传如果任务的需求是技能需求，则把任务需求表的那一行传进去
	local skillNeedQuestInfoTbl = nil
	local questInfoTbl = g_QuestNeedMgr[QuestName]
	if questInfoTbl ~= nil then
		for k,v in pairs(questInfoTbl) do
			local temp = {}
			if "技能需求" == v.NeedType then
				temp = v
				temp.skilllearntype = SkillPart_Common(temp.SkillName,"SkillLearnType")
				if not skillNeedQuestInfoTbl then
					skillNeedQuestInfoTbl = {}
				end
				--table.insert(skillNeedQuestInfoTbl, temp)
				skillNeedQuestInfoTbl[k] = temp
			end
		end
	end
	
	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["PlayerId"] = Player.m_uID
	parameter["PlayerCamp"] = Player:CppGetBirthCamp()
	parameter["PlayerLevel"] = Player:CppGetLevel()
	parameter["PlayerYbLevel"] = Player.m_uMercenaryLevel
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	parameter["GMTimeCountLimit"] = g_GMTimeCountLimit
	--Renton可能有问题,有可能表为{},但是不为nil,不为空而且有next(有值)
	if skillNeedQuestInfoTbl and next(skillNeedQuestInfoTbl) then
		parameter["skillNeedQuestInfoTbl"] = skillNeedQuestInfoTbl
	end
	
	if not IsGM then
		local res = CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "AcceptQuest", CallBack, parameter)
	else
		local res = CallAccountAutoTrans(Conn.m_Account, "RoleQuestDB", "AcceptQuest", CallBack, parameter)
	end
end

local function AcceptQuest(Conn, QuestName, ObjEntityID)
	if not IsCppBound(Conn) then
		return
	end
	
	local Player = Conn.m_Player
	local DictatorObj = CCharacterDictator_GetCharacterByID(ObjEntityID)
	local IntObj = CIntObjServer_GetIntObjServerByID(ObjEntityID)
	local QuestObj = DictatorObj or IntObj
	if not QuestObj or not Player then
		return
	end
	
	local Type = QuestObj.m_Properties:GetType()
	if Type == ECharacterType.Npc
		and not IsPasserbyCamp(QuestObj:CppGetCamp(), Player:CppGetBirthCamp(), 0, Player:CppGetGameCamp()) then
		return
	end
	
	if not Quest_Common(QuestName) then
		return
	end
	
	if g_HideQuestMgr[QuestName] then
		if g_HideQuestMgr[QuestName][Player.m_uID] ~= true then
			return
		end
	end
	--废弃任务
	if Quest_Common(QuestName, "任务类型") == 9 then
		return
	end
		
	if IfAcceptQuestMap(Player.m_Scene.m_SceneName,QuestName) == false then
		return
	end
	
	local QuestObjName = QuestObj.m_Properties:GetCharName()
	if Quest_Common(QuestName, "触发任务方式", "Arg") ~= QuestObjName then
		return --所接任务和npc身上的任务不符
	end
	
	local pos = CPos:new()
	QuestObj:GetGridPos( pos )	
	if not IfAcceptQuestArea(Player,pos) then --玩家位置判断
		return
	end
	
	local TheaterName = Quest_Common(QuestName, "开启剧情")
	if TheaterName and TheaterName~="" then
 		if Player.m_Scene.m_TheaterMgr:BeTheaterLocked(TheaterName) then
			MsgToConn(Conn,150000,TheaterName)
			return
		end
	end
	
	local PlayerTongId = Player.m_Properties:GetTongID()
	if Quest_Common(QuestName, "佣兵小队限制") and Quest_Common(QuestName, "佣兵小队限制") ~= 0 then
		if PlayerTongId ~= DictatorObj.m_TongID then
			MsgToConn(Conn,300030)
			return
		end
	end
	AcceptQuestFun(Conn, QuestName)
end

local function FinishQuestFun(Player, QuestName, SelectIndex)
	local PlayerCamp = Player:CppGetBirthCamp()
	local PlayerOldLevel = Player:CppGetLevel()
	Player:OnSavePlayerSoulTickFunc(nil,Player)
	local function CallBack(result)
		if IsCppBound(Player) then
			if not result[1] then
				if IsString(result[2]) then
					MsgToConn(Player.m_Conn, 3003, result[2])
				elseif IsNumber(result[2]) then
					if IsTable(result[3]) then
						MsgToConn(Player.m_Conn, result[2], result[3])
					else
						MsgToConn(Player.m_Conn, result[2])
					end
				end
			else
				if result[2]["AddLoopQuestResult"] and not result[2]["AddLoopQuestResult"][1] then
					local addsname = result[2]["LoopQuestTbl"][1]
					local addres  = result[2]["AddLoopQuestResult"]
					if IsString(addres[2]) then
						MsgToConn(Player.m_Conn, 3001, addres[2])
					elseif IsNumber(addres[2]) then
						if IsTable(addres[3]) then
							MsgToConn(Player.m_Conn, addres[2], unpack(addres[3]))
						else
							MsgToConn(Player.m_Conn, addres[2])
						end
					end
				else
					FinishQuest_DB_Ret(Player,QuestName,result[2]["delItemTbl"],result[2]["addItemTbl"],result[2]["AddExpTbl"],result[2]["SoulRet"],result[2]["MercenaryLevelTbl"],result[2]["DirectTbl"],result[2]["LoopQuestTbl"],result[2]["uAddSoul"])
					
					if result[2]["FinishTimesTbl"] then
						if result[2]["FinishTimesTbl"][1] then
							MsgToConn(Player.m_Conn, 300015, result[2]["FinishTimesTbl"][1])
						elseif result[2]["FinishTimesTbl"][2] then
							MsgToConn(Player.m_Conn, 300016, result[2]["FinishTimesTbl"][2])
						end
					end
					
					--若完成的是佣兵任务，且其他任务有完成佣兵任务的事件需求，加佣兵任务计数
					if g_AllMercenaryQuestMgr[QuestName] then
						local sVarName = "完成"..QuestName
						if g_VarQuestMgr[sVarName] then
							AddVarNumForTeamQuest(Player, sVarName, 1)
						end
						sVarName = "完成任意佣兵任务"
						if g_VarQuestMgr[sVarName] then
							AddVarNumForTeamQuest(Player, sVarName, 1)
						end
					end
					
					--奖励buff
					local buff = Quest_Common(QuestName, "buff奖励")
					if buff and buff ~= "" then
						Player:PlayerDoFightSkillWithoutLevel( buff )
					end
					
					--更新佣兵等级追踪窗口
					--UpdateMercenaryLevelTraceWnd(Player.m_Conn, result[2]["MLRes"])
					
					if result[2]["AddLoopQuestResult"] then
						local addsname = result[2]["LoopQuestTbl"][1]
						local addres  = result[2]["AddLoopQuestResult"]
						AddQuest_DB_Ret(Player,addsname,addres[2],addres[3],addres[4],addres[5],addres[6],addres[7])
					end
				end
			end
		end
		
		--给其它人经验
		if result[1] and result[2]["TeamPlayerExp"] and next(result[2]["TeamPlayerExp"]) then
			local TeamPlayerExp = result[2]["TeamPlayerExp"]
			for i=1, #(TeamPlayerExp) do
				local ID = TeamPlayerExp[i][1]
				local ExpTbl = TeamPlayerExp[i][2]
				--经验奖励
				local Target = g_GetPlayerInfo(ID)
				if IsServerObjValid(Target) and ExpTbl then
					MsgToConn(Target.m_Conn,3045,QuestName,ExpTbl["AddExp"])
					AddPlayerExp_DB_Ret(Target,ExpTbl)
				end
			end
		end
	end
	
	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["PlayerId"] = Player.m_uID
	parameter["SelectIndex"] = SelectIndex
	parameter["PlayerCamp"] = Player:CppGetBirthCamp()
	parameter["PlayerTongID"] = Player.m_Properties:GetTongID()
	parameter["PlayerLevel"] = Player:CppGetLevel()
	parameter["GMTimeCountLimit"] = g_GMTimeCountLimit
	parameter["TeamPlayerID"] = {}
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	local TeamID = Player.m_Properties:GetTeamID()
	local Member = {Player}
	local ChannelIDList = {}
	if TeamID and TeamID ~= 0 then
		local tblMembers = g_TeamMgr:GetMembers(TeamID)
		local MemberNum = #(tblMembers)
		for i = 1, MemberNum do
			local MemberId = tblMembers[i]
			if MemberId ~= Player.m_uID then
				local playerTemp = g_GetPlayerInfo(MemberId)
				if playerTemp and playerTemp.m_Scene 
					and Player.m_Scene.m_SceneId == playerTemp.m_Scene.m_SceneId then
					table.insert(ChannelIDList, playerTemp.m_AccountID)
					table.insert(parameter["TeamPlayerID"], MemberId)
					table.insert(Member, playerTemp)
				end
			end
		end
	end
	
	OnSavePlayerExpFunc(Member)
	CallAccountManualTrans(Player.m_Conn.m_Account, "RoleQuestDB", "FinishQuest", CallBack, parameter, unpack(ChannelIDList))
end

local function FinishQuest(Conn, QuestName, SelectIndex, ObjEntityID)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	
	--local data = {Conn, QuestName, SelectIndex, ObjEntityID}
	local DictatorObj = CCharacterDictator_GetCharacterByID(ObjEntityID)
	local IntObj = CIntObjServer_GetIntObjServerByID(ObjEntityID)
	local QuestObj = DictatorObj or IntObj
	if QuestObj == nil then
		return
	end	
	local Type = QuestObj.m_Properties:GetType()
	if Type == ECharacterType.Npc 
		and not IsPasserbyCamp(QuestObj:CppGetCamp(), Conn.m_Player:CppGetBirthCamp(), 0, Conn.m_Player:CppGetGameCamp()) then
		return
	end
	
	if not Quest_Common(QuestName) then
		return
	end
	
	if Quest_Common(QuestName, "佣兵团限制") and Quest_Common(QuestName, "佣兵团限制") ~= 0 then
		local TongId = Conn.m_Player.m_Properties:GetTongID()
		if TongId == 0 then
			MsgToConn(Conn, 300022)
			return false
		end
	end
	
	if Quest_Common(QuestName, "交任务状态需求") then
		local Keys = Quest_Common:GetKeys(QuestName, "交任务状态需求")
		for k = 1, table.getn(Keys) do
			local tbl = GetCfgTransformValue(true, "Quest_Common", QuestName, "交任务状态需求", Keys[k], "Function")
			local needbuff = tbl[1]		--需求的buff名
			if not Conn.m_Player:ExistState(needbuff) then
				return {false,"你身上没有状态:"..needbuff}
			end
		end
	end
	
	--判断角色所在位置是否符合提交条件
	local QuestObjName = QuestObj.m_Properties:GetCharName()
	if(Quest_Common(QuestName, "结束任务方式", "Arg") ~= QuestObjName) then
		return
	end
	if IfFinishQuestMap(Conn.m_Player.m_Scene.m_SceneName,QuestName) == false then
		return
	end
	
	local pos = CPos:new()
	QuestObj:GetGridPos( pos )
	local Player = Conn.m_Player
	if not IfCanFinishArea(Player,pos) then
		return
	end
	
	FinishQuestFun(Player, QuestName, SelectIndex)
end

--打开任务接取面板
local function ShowTakeQuestWnd(Conn,QuestName, EntityID)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	
	local DictatorObj = CCharacterDictator_GetCharacterByID(EntityID)
	local IntObj = CIntObjServer_GetIntObjServerByID(EntityID)
	local QuestObj = DictatorObj or IntObj
	if QuestObj == nil then
		return
	end	
	local Type = QuestObj.m_Properties:GetType()
	if Type == ECharacterType.Npc 
		and not IsPasserbyCamp(QuestObj:CppGetCamp(), Conn.m_Player:CppGetBirthCamp(),0,Conn.m_Player:CppGetGameCamp()) then
		return
	end
	local NpcName = QuestObj.m_Properties:GetCharName()
	if(not Quest_Common(QuestName) or Quest_Common(QuestName, "触发任务方式", "Arg") ~= NpcName) then
--		print("该对象身上没有任务")
		return --所接任务和npc身上的任务不符
	end
	if IfAcceptQuestMap(Conn.m_Player.m_Scene.m_SceneName,QuestName) == false then
		return --当前地图和任务所在地图不符
	end
	local pos=CPos:new()
	QuestObj:GetGridPos( pos ) 
	
	if IfAcceptQuestArea(Conn.m_Player,pos) then
		Gas2Gac:RetShowTakeQuest(Conn,QuestName)
		return
	end
end

--打开任务完成面板
local function ShowFinishQuestWnd(Conn,sQuestName, EntityID)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	
	local DictatorObj = CCharacterDictator_GetCharacterByID(EntityID)
	local IntObj = CIntObjServer_GetIntObjServerByID(EntityID)
	local QuestObj = DictatorObj or IntObj
	if QuestObj == nil then
		return
	end	
	local Type = QuestObj.m_Properties:GetType()
	if Type == ECharacterType.Npc 
		and not IsPasserbyCamp(QuestObj:CppGetCamp(), Conn.m_Player:CppGetBirthCamp(),0,Conn.m_Player:CppGetGameCamp()) then
		return
	end
	if IfFinishQuestMap(Conn.m_Player.m_Scene.m_SceneName,sQuestName) == false then
		return --当前地图和任务所在地图不符
	end
	local NpcName = QuestObj.m_Properties:GetCharName()
	if(not Quest_Common(sQuestName) or Quest_Common(sQuestName, "结束任务方式", "Arg") ~= NpcName) then
--		print("该对象身上不能完成任务")
		return
	end	
	local pos=CPos:new()
	QuestObj:GetGridPos( pos )
	
	local function ShowFinishWnd(ItemIndex)
		if IsCppBound(Conn) then
			Gas2Gac:RetShowFinishQuest(Conn,sQuestName,ItemIndex or 0)
		end
	end


	if IfCanFinishArea(Conn.m_Player,pos) then
		if g_AllMercenaryQuestMgr[sQuestName] then
			local parameter = {}
			parameter["char_Id"] = Conn.m_Player.m_uID
			parameter["QuestName"] = sQuestName
			CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "GetAwardItemFromQuest", ShowFinishWnd, parameter)
		else
			ShowFinishWnd()
		end
		return
	end
	return
end

local function RequestQuestByItem(Conn,QuestName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	if not Quest_Common(QuestName) then
		return
	end
	if Quest_Common(QuestName, "触发任务方式", "Subject") ~= "Goods" then
		MsgToConn(Conn, 300004)
		return
	end
	
	local ItemName = Quest_Common(QuestName, "触发任务方式", "Arg")
	local FbSceneName = nil
	if Mercenary_Quest_Server(ItemName) then
		FbSceneName = Mercenary_Quest_Server(ItemName,"SceneName")
	end
	
	local function CallBack(PlayerQuest,delitem,TargetQuetTbl)
		if not PlayerQuest[1] then
			if IsString(PlayerQuest[2]) then
				MsgToConn(Conn, 3001, PlayerQuest[2])
			elseif IsNumber(PlayerQuest[2]) then
				if IsTable(PlayerQuest[3]) then
					MsgToConn(Conn, PlayerQuest[2], unpack(PlayerQuest[3]))
				else
					MsgToConn(Conn, PlayerQuest[2])
				end
			end
			return
		end
		
		if IsCppBound(Player) then
			for i=1,table.getn(delitem) do
				local type,name,num,res = delitem[i][1],delitem[i][2],delitem[i][3],delitem[i][4]
				delete_item_DB_Ret(Player,type,name,num,res)
			end
			AddQuest_DB_Ret(Player,QuestName,PlayerQuest[2],PlayerQuest[3],nil,PlayerQuest[5])
		end
		
--		for Id,Tbl in pairs(TargetQuetTbl) do
--			local Target = g_GetPlayerInfo(Id)
--			if IsCppBound(Target) then
--				AddQuest_DB_Ret(Target,QuestName,Tbl[2])
--			end
--		end
		
		if FbSceneName then
			local DareQuestFb = CDareQuestFb:new()
			DareQuestFb:UseItemJoinFb(Player, FbSceneName, QuestName)
		end
	end
	
	local ChannelIDList = {}	
	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["ItemName"] = ItemName
	parameter["PlayerId"] = Player.m_uID
	parameter["PlayerCamp"] = Player:CppGetBirthCamp()
	parameter["PlayerLevel"] = Player:CppGetLevel()
	parameter["PlayerYbLevel"] = Player.m_uMercenaryLevel
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	parameter["GMTimeCountLimit"] = g_GMTimeCountLimit
	
	if FbSceneName then
		parameter["IsYbQuestItem"] = true
	end
	
	CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "ItemRequestQuest", CallBack, parameter, unpack(ChannelIDList))
end

local function AcceptQuestJoinFb(Conn, QuestName, SceneName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	if not Quest_Common(QuestName) then
		return
	end
	if Quest_Common(QuestName, "触发任务方式", "Subject") ~= "Goods" then
		MsgToConn(Conn, 300004)
		return
	end
	local ItemName = quest_mgr[1].Arg
	
	local function CallBack(result)
		if IsCppBound(Player) then
			if not result[1] then
				if IsString(result[2]) then
					MsgToConn(Conn, 3001, result[2])
				elseif IsNumber(result[2]) then
					if IsTable(result[3]) then
						MsgToConn(Conn, result[2], unpack(result[3]))
					else
						MsgToConn(Conn, result[2])
					end
				end
			else
				AddQuest_DB_Ret(Player,QuestName,result[2],result[3],nil,result[5])
				Gac2Gas:AgreedJoinDareFb(Player.m_Conn, SceneName, QuestName)
			end
		end
	end
	
	local parameter = {}
	parameter["QuestName"] = QuestName
	parameter["PlayerId"] = Player.m_uID
	parameter["PlayerCamp"] = Player:CppGetBirthCamp()
	parameter["PlayerLevel"] = Player:CppGetLevel()
	parameter["PlayerYbLevel"] = Player.m_uMercenaryLevel
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	parameter["GMTimeCountLimit"] = g_GMTimeCountLimit
	CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "AcceptQuest", CallBack, parameter)
end

local function SendMsgLeaveFb(Player, Time)
	local Scene = Player.m_Scene

	local function CallBack_Tick(Player,OldSceneId)
		if IsCppBound(Player) and Player.m_Scene.m_SceneId == OldSceneId then -- 玩家通过其他方式离开了副本,不需传送了
			
			local sceneName = Player.m_MasterSceneName
			local pos = Player.m_MasterScenePos
			ChangeSceneByName(Player.m_Conn, sceneName, pos.x, pos.y)
			
		end
	end
	--客户端打开一个倒计时的窗口,给个提示.让退出该副本
	RegisterOnceTick(Player,"QuestLeaveFbSceneTick", CallBack_Tick, Time * 1000, Player, Scene.m_SceneId)
	MsgToConn(Player.m_Conn, 3058, Time)
	Gas2Gac:RetShowDownTimeWnd(Player.m_Conn, Time)
	--NotifyPlayerLeaveFbScene(Player.m_uID, Time, MsgID)
end

--客户端触发的任务失败,一般是任务限时的时间到了后,触发的任务失败
local function SetQuestFailure(Conn, QuestName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local PlayerID = Player.m_uID
	local parameter = {}
	parameter["PlayerId"] = PlayerID
	parameter["QuestName"] = QuestName
	
	local function CallBack (result)
		if result and IsCppBound(Player) then
			QuestFailure_DB_Ret(Player,QuestName)
			if Player.m_Scene.m_SceneAttr.SceneType == 16 then
				if Player.m_Scene.m_QuestName == QuestName then
					SendMsgLeaveFb(Player, 20)--失败
				end
			end
		end
	end
	CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "QuestFailure", CallBack, parameter)		
end

local function SetQuestIsLimitTime(Conn, QuestName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local PlayerID = Player.m_uID
	--print(QuestName)
	local function CallBack (result)
		if result and IsCppBound(Player) then
			if Player.m_Scene.m_SceneAttr.SceneType == 16 then
				if Player.m_Scene.m_QuestName == QuestName then
					SendMsgLeaveFb(Player, 30)--成功
				end
			end
		end
	end
	
	local LimitTime = Quest_Common(QuestName, "限时")
	if LimitTime ~= nil and LimitTime ~= 0 then
		local parameter = {}
		parameter["PlayerId"] = PlayerID
		parameter["QuestName"] = QuestName
		CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "QuestIsLimitTime", CallBack, parameter)
		return
	end
	if Quest_Common(QuestName, "区域副本传送") then
		SendMsgLeaveFb(Player, 10)--成功
	end
end

--离开场景,让任务失败
local function LeaveScene_SetQuestFailure(Scene, Player)
	local function CallBack (result)
		if result and next(result) and IsCppBound(Player) then
			for i=1, #(result) do
				if result[i]["DelSucc"] then
					QuestFailure_DB_Ret(Player,result[i]["QuestName"])
				end
			end
		end
	end
	
	local parameter = {}
	parameter["char_id"] = Player.m_uID
	parameter["scene_name"] = Scene.m_SceneName
	parameter["scene_type"] = Scene.m_SceneAttr.SceneType
	CallDbTrans("RoleQuestDB", "LeaveSceneSetQuestFailure", CallBack, parameter, Player.m_AccountID)
end

--设置对话npc的服务端全局id
local function SetLockingNPCID(Conn,NpcID)
	Conn.LockingNpcID = NpcID
end

local function SetQuestLevelVarNum(Conn, QuestName, varName)
	if g_QuestNeedMgr[QuestName] then
		for i,v in pairs(g_QuestNeedMgr[QuestName]) do
			
			if v.NeedLevel and i == varName then
				local Level = Conn.m_Player:CppGetLevel()
				if v.NeedLevel <= Level then
					AddVarNumForTeamQuest(Conn.m_Player, varName, 1)
				end
				break
			end
			
		end
	end
end

local function SetCheckInMercenaryInfo(Conn)
	local Player = Conn.m_Player
	if IsCppBound(Player) then
		Player:IsFirstTimeAndSendMsg("登记佣兵")
	end
end

local NameMercLevel = {
	["见习佣兵"]= 1,
	["小佣兵"]	= 2,
	["G"] = 3,
	["F"] = 4,
	["E"] = 5,
	["D"] = 6,
	["C"] = 7,
	["B"] = 8,
	["A"] = 9,
}

local function ReturnSentenceParse(Level,str)
	str = string.gsub( str, "level", Level)
	local count = assert(loadstring( "return " .. str))()
	count = math.floor(count)
	return count
end

local function GetPlayerTempQuestList(Player)
	local Camp = Player:CppGetBirthCamp()
	local Level = Player:CppGetLevel()
	local MercLev = Player.m_uMercenaryLevel
	
	local LevelRange = 4			--等级段(要显示的任务等级段)大于20级后会用到
	local DownLevel = 2
	local UpLevel = 0
	
	if Level <= 11 then
		DownLevel = 0
	elseif Level <= 80 then
		DownLevel = Level - 10
	end
	
	local QuestList = {}
	QuestList[1] = {}
	QuestList[2] = {}
	QuestList[3] = {}
	
	local CurTime = os.time()
	--得到日常佣兵任务
	for QuestName,Tbl in pairs(g_GeneralMercenaryQuestMgr) do
		if Tbl.m_CampType == Camp then
			if Tbl.m_QuestIsRepeat and Tbl.m_QuestIsRepeat == 1 then--可重复的日常佣兵任务
				
				local QuestLevel = ReturnSentenceParse(Level,Tbl.m_QuestLevel)
				if QuestLevel >= Level-DownLevel and QuestLevel <= Level+UpLevel then
					if not Tbl.m_MercLev or Tbl.m_MercLev=="" or (MercLev >= NameMercLevel[Tbl.m_MercLev]) then
						local Index = Level - QuestLevel + 1
						if Level <= 11 then--11级以下的在一个列表中
							Index = 1
						end
						if Index > LevelRange then
							Index = LevelRange
						end
						if not QuestList[1][Index] then
							QuestList[1][Index] = {}
						end
						table.insert(QuestList[1][Index],{QuestName,1})
					end
					
				end
				
			end
		end
	end
	
	--得到小队佣兵任务
	for QuestName,Tbl in pairs(g_TeamMercenaryQuestMgr) do
		if Tbl.m_CampType == Camp then
			if Tbl.m_QuestIsRepeat and Tbl.m_QuestIsRepeat == 1 then--可重复的小队任务
				
				local QuestLevel = ReturnSentenceParse(Level,Tbl.m_QuestLevel)
				local TempDownLevel = Level-DownLevel
				if Level > 25 and Tbl.m_QuestType == 6 then
					TempDownLevel = 20
				end
				if QuestLevel >= TempDownLevel and QuestLevel <= Level+UpLevel then
					
					if not Tbl.m_MercLev or Tbl.m_MercLev=="" or (MercLev >= NameMercLevel[Tbl.m_MercLev]) then
						local Index = Level-QuestLevel+1
						if Level <= 11 then
							Index = 1
						end
						if Index > LevelRange then
							Index = LevelRange
						end
						
						--剧情本任务,就25级后,全部随机选任务
						if Level > 25 and Tbl.m_QuestType == 6 then
							Index = 1
						end
						
						if not QuestList[Tbl.m_QuestType-4][Index] then
							QuestList[Tbl.m_QuestType-4][Index] = {}
						end
						table.insert(QuestList[Tbl.m_QuestType-4][Index],{QuestName,Tbl.m_QuestType-4})
					end
					
				end
				
			end
		end
	end
	for index=1, 3 do
		
		if next(QuestList[index]) then
			for iLevel=1, LevelRange do
				
				if QuestList[index][iLevel] then
					for i=1, #(QuestList[index][iLevel]) do
						local num1 = math.random(1,#(QuestList[index][iLevel]))
						local num2 = math.random(1,#(QuestList[index][iLevel]))
						local Temp = QuestList[index][iLevel][num1]
						QuestList[index][iLevel][num1] = QuestList[index][iLevel][num2]
						QuestList[index][iLevel][num2] = Temp
					end
				end
				
			end
		end
	end
	
	--这样做是为了让先能接到同级的任务
	local resQuestList = {}
	for i=1, 3 do
		if not resQuestList[i] then
			resQuestList[i] = {}
		end
		for j=1, LevelRange do--分LevelRange段(按等级分的)
			
			if QuestList[i][j] then
				for k=1, 3 do--每段取任务的个数
					if QuestList[i][j][k] then
						table.insert(resQuestList[i],QuestList[i][j][k])
					end
				end
			end
			
		end
	end
	
	return resQuestList
end

local function PlayerSendTempQuestList(Player)
	local PlayerId = Player.m_uID
	--存储下次要刷出来的任务
	local Level = Player:CppGetLevel()
	local MercLev = Player.m_uMercenaryLevel
	Player.m_PlayerTempQuestList = {}
	Player.m_PlayerTempQuestList.m_Level = Level
	Player.m_PlayerTempQuestList.m_MercLev = MercLev

	local QuestList = GetPlayerTempQuestList(Player)
	for index=1, 3 do
		Player.m_PlayerTempQuestList[index] = {}
		for i=1, 3 do
			if QuestList[index][i] then
				table.insert(Player.m_PlayerTempQuestList[index], QuestList[index][i])
				Gas2Gac:RetSendMercTempQuestPool(Player.m_Conn,QuestList[index][i][1],QuestList[index][i][2])
			end
		end
		if not next(Player.m_PlayerTempQuestList[index]) then
			Player.m_PlayerTempQuestList[index] = nil
		end
	end
	if not next(Player.m_PlayerTempQuestList) then
		Player.m_PlayerTempQuestList = nil
	else
		Player.m_PlayerTempQuestList.m_Level = Level
		Player.m_PlayerTempQuestList.m_MercLev = MercLev
	end
	
	Gas2Gac:RetSendMercTempQuestPoolEnd(Player.m_Conn, true)
end

function CRoleQuest.GetPlayerTempQuestList(Player)
	return GetPlayerTempQuestList(Player)
end

function CRoleQuest.PlayerSendTempQuestList(Player)
	return PlayerSendTempQuestList(Player)
end

function CRoleQuest.AddPlayerExp_DB_Ret(Player,AddExpNumTbl)
	return AddPlayerExp_DB_Ret(Player,AddExpNumTbl)
end

function CRoleQuest.AddQuest_DB_Ret(Player,sQuestName,give_items,YbItemIndex,QuestVarTbl,DirectTbl)
	return AddQuest_DB_Ret(Player,sQuestName,give_items,YbItemIndex,QuestVarTbl,DirectTbl)
end

function CRoleQuest.add_item_DB_Ret(Player, nBigId, sItemName,nCount,res)
	return add_item_DB_Ret(Player, nBigId, sItemName,nCount,res)
end

function CRoleQuest.add_item_DB_Ret_By_Id(PlayerId, nBigId, sItemName,nCount,res)
	return add_item_DB_Ret_By_Id(PlayerId, nBigId, sItemName,nCount,res)
end

function CRoleQuest.delete_item_DB_Ret(Player, nBigId, sItemName, nCount, res, IsShowMsg)
	return delete_item_DB_Ret(Player, nBigId, sItemName, nCount, res, IsShowMsg)
end

function CRoleQuest.QuestFailure_DB_Ret(Player,sQuestName)
	return QuestFailure_DB_Ret(Player,sQuestName)
end

function CRoleQuest.FinishQuest_DB_Ret(Player,sQuestName,Del_ItemTbl,Add_ItemTbl,AddExpTbl,SoulRet,MercenaryLevelTbl,DirectTbl,LoopQuestTbl,uAddSoulNum)
	return FinishQuest_DB_Ret(Player,sQuestName,Del_ItemTbl,Add_ItemTbl,AddExpTbl,SoulRet,MercenaryLevelTbl,DirectTbl,LoopQuestTbl,uAddSoulNum)
end

function CRoleQuest.DeleteQuest_DB_Ret(Player,Del_Item_Tbl,sQuestName, IsShowMsg)
	return DeleteQuest_DB_Ret(Player,Del_Item_Tbl,sQuestName, IsShowMsg)
end

function CRoleQuest.DecreQuestItem_DB_Ret(Player,ItemName,DeleteQuestName,DelItemTbl)
	return DecreQuestItem_DB_Ret(Player,ItemName,DeleteQuestName,DelItemTbl)
end

function CRoleQuest.ShareQuest(Conn, QuestName)
	return ShareQuest(Conn, QuestName)
end

function CRoleQuest.ResponseShareQuest(Conn, QuestTeamID, QuestName)
	return ResponseShareQuest(Conn, QuestTeamID, QuestName)
end

function CRoleQuest.GiveUpQuest(Conn,QuestName)
	return GiveUpQuest(Conn,QuestName)
end

function CRoleQuest.AcceptQuest(Conn, QuestName, ObjEntityID)
	return AcceptQuest(Conn, QuestName, ObjEntityID)
end

function CRoleQuest.FinishQuest(Conn, sQuestName, SelectIndex, ObjEntityID)
	return FinishQuest(Conn, sQuestName, SelectIndex, ObjEntityID)
end

function CRoleQuest.ShowTakeQuestWnd(Conn,QuestName, EntityID)
	return ShowTakeQuestWnd(Conn,QuestName, EntityID)
end

function CRoleQuest.ShowFinishQuestWnd(Conn,sQuestName, EntityID)
	return ShowFinishQuestWnd(Conn,sQuestName, EntityID)
end

function CRoleQuest.RequestQuestByItem(Conn,QuestName)
	return RequestQuestByItem(Conn,QuestName)
end

function CRoleQuest.AcceptQuestJoinFb(Conn, QuestName, SceneName)
	return AcceptQuestJoinFb(Conn, QuestName, SceneName)
end

function CRoleQuest.SetQuestFailure(Conn, QuestName)
	return SetQuestFailure(Conn, QuestName)
end

function CRoleQuest.SetQuestIsLimitTime(Conn, QuestName)
	return SetQuestIsLimitTime(Conn, QuestName)
end

function CRoleQuest.LeaveScene_SetQuestFailure(Scene, Player)
	return LeaveScene_SetQuestFailure(Scene, Player)
end

function CRoleQuest.SetLockingNPCID(Conn,NpcID)
	return SetLockingNPCID(Conn,NpcID)
end

function CRoleQuest.SetQuestLevelVarNum(Conn, QuestName, varName)
	return SetQuestLevelVarNum(Conn, QuestName, varName)
end

function CRoleQuest.SetCheckInMercenaryInfo(Conn)
	return SetCheckInMercenaryInfo(Conn)
end

function CRoleQuest.IncreQuestItemVar_DB_Ret(Player,ItemType, ItemName, nCount, QuestItemTbl)
	return IncreQuestItemVar_DB_Ret(Player,ItemType, ItemName, nCount, QuestItemTbl)
end

function CRoleQuest.AcceptQuestFun(Conn, QuestName, IsGM)
	return AcceptQuestFun(Conn, QuestName, IsGM)
end

function CRoleQuest.NpcFromQuest_Add(Player, QuestName)
	if not Player.m_QuestToNpcMgr then
		Player.m_QuestToNpcMgr = {}
	end
	
	local QuestToNpc = g_QuestToNpcMgr[QuestName]
	if QuestToNpc then
		if not Player.m_QuestToNpcMgr[QuestName] then
			Player.m_QuestToNpcMgr[QuestName] = {}
		end
	
		for i=1, #(QuestToNpc) do
			table.insert(Player.m_QuestToNpcMgr[QuestName], QuestToNpc[i])
		end
	end
	
	if not Player.m_DoingQuest then
		Player.m_DoingQuest = {}
	end
	Player.m_DoingQuest[QuestName] = true
end

function CRoleQuest.NpcFromQuest_Del(Player, QuestName)
	if Player.m_QuestToNpcMgr and Player.m_QuestToNpcMgr[QuestName] then
		Player.m_QuestToNpcMgr[QuestName] = nil
	end
	
	if Player.m_DoingQuest and Player.m_DoingQuest[QuestName] then
		Player.m_DoingQuest[QuestName] = nil
	end
end

function CRoleQuest.NpcFromQuest_Get(Player)
	return Player.m_QuestToNpcMgr
end

function CRoleQuest.DoingQuest_Check(Player, QuestName)
	if Player.m_DoingQuest and Player.m_DoingQuest[QuestName] then
		return true
	else
		return false
	end
end
