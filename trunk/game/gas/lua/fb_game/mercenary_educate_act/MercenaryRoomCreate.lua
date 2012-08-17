
local GameProbability = {
			[1] = {1,15},--15%
			[2] = {16,55},--40%
			[3] = {56,80},--25%
			[4] = {81,95},--15%
			[5] = {96,100},--5%
			}

local GameTypeDeadTimes = {100, 100, 100, 100, 0}--每一个类型会出现的次数
local m_ToTalFloor = 4
local m_CloseFbTime = 20 --结束后,退出副本的时间
local RewardFloorType = 5 --类型5是奖励关
local DividedNum = 10
local ExtraExpPara = 2
CMercenaryRoomCreate = class()
--g_MercenaryRoomCreate = CMercenaryRoomCreate:new()
CMercenaryRoomCreate.GameState = 
{
	BeforeGame = "BeforeGame", 
	StartGame = "StartGame", 
	LoseGame = "GameEnd", 
	PrepareNextGame = "PrepareNextGame",
}
--g_MercenaryRoomCreate.m_DownTimeTick = {}

local function GetXiShu(FloorNum)
	local XS = (1+(0.1*(FloorNum-1))) * 100
	return XS
end

-- function CMercenaryRoomCreate:GetRandomGameID(Player,FloorNum)
function CMercenaryRoomCreate.GetRandomGameID(Player,FloorNum)
	local Level = Player:CppGetLevel()
--	local RandNum = math.random(1,100)
--	local GameType = 1
--	for i=1, #(GameProbability) do
--		if RandNum >= GameProbability[i][1]
--			and RandNum <= GameProbability[i][2] then
--			GameType = i
--			break
--		end
--	end
	local Scene = Player.m_Scene
	local MercInfo = nil
	
	if FloorNum == 1 then
		MercInfo = Player
		MercInfo.m_TempMercCardInfo = Player.m_MercCardInfo
	else
		MercInfo = Scene.m_MercenaryActionInfo
	end
	
	local GameTypeTbl = {}
	if MercInfo.m_IsRewardFloor then	-- 刷奖励关
		--for i=1, #(Player.m_MercenaryEduActTbl) do
		--	local ActionMgr = Player.m_MercenaryEduActTbl[i]
		for _, ActionMgr in pairs(MercInfo.m_MercenaryEduActTbl) do
			if ActionMgr.m_GameType == RewardFloorType then -- RewardFloorType是奖励关的类型
				if ActionMgr.m_MinLevel <= Level
					and ActionMgr.m_MinFloor <= FloorNum then
					--print("插入奖励关"..ActionMgr.m_GameID)
					table.insert(GameTypeTbl,ActionMgr.m_GameID)
				end			
			end
		end	
	else	-- 刷普通关
		for i=1, #(MercInfo.m_TempMercCardInfo) do
			local ActionMgr = MercInfo.m_MercenaryEduActTbl[MercInfo.m_TempMercCardInfo[i]]
			if ActionMgr.m_GameType ~= RewardFloorType then	-- 普通关不随出奖励关
				--local SumSelTimes = Player.m_MercenaryEduActTbl.m_GameTypeTimes[ActionMgr.m_GameType]
				--if not SumSelTimes or (SumSelTimes < GameTypeDeadTimes[ActionMgr.m_GameType]) then
					if ActionMgr.m_MinLevel <= Level
						and ActionMgr.m_MinFloor <= FloorNum then
						
						--print("插入普通"..ActionMgr.m_GameID)
						table.insert(GameTypeTbl,ActionMgr.m_GameID)
					end
				--end
			end
		end	
	end
	
	local GameID = 1
	if next(GameTypeTbl) then
		local RandNum = nil
		-- GM指令
		if Player.GMGotoMercLevel then
			GameID = Player.GMGotoMercLevel 
			-- 超出范围
			if not g_MercenaryEduActMgr[GameID] then
				GameID = 1
			end
			return GameID
		else
			RandNum = math.random(1,#(GameTypeTbl))
			GameID = GameTypeTbl[RandNum]
			--print("选出GameID = " .. GameID)
		end
		if MercInfo.m_IsRewardFloor then	-- 刷奖励关
			MercInfo.m_MercenaryEduActTbl[GameID] = nil
		elseif FloorNum ~= 1 then
			for i=1, #(MercInfo.m_TempMercCardInfo) do
				--将选出来的活动,从列表中去掉
				if MercInfo.m_TempMercCardInfo[i] == GameID then
					table.remove(MercInfo.m_TempMercCardInfo,i)
					--print("去掉第"..i)
					break
				end
			end
		end
	end
	--print("GameID "..GameID)
	return GameID
end

--加技能,和去掉技能
-- function CMercenaryRoomCreate:YbEduActAddOrResetSkill(Player, GameID, IsAdd)
function CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, GameID, IsAdd)
	if GameID and IsCppBound(Player) then
		local SkillTbl = nil
		if IsAdd then
			SkillTbl = g_MercenaryEduActMgr[GameID].m_LoadSkillTbl
			--清除临时技能
			Player:RemoveAllTempSkill()
			Player:CancelCastingProcess()
		else
			SkillTbl = g_MercenaryEduActMgr[GameID].m_ResetSkillTbl
			--清除临时技能
			Player:RemoveAllTempSkill()
			Player:CancelCastingProcess()
			--清除手上拿着的东西
			Player:ClearPlayerActionState()
		end
		if SkillTbl then
			for _, sillName in ipairs(SkillTbl) do
				Player:PlayerDoFightSkillWithoutLevel(sillName)
			end
		end
	end
end

-- function CMercenaryRoomCreate:ClearDownTimeTick(Scene)
function CMercenaryRoomCreate.ClearDownTimeTick(Scene)
	if Scene.m_DownTimeTick then
		--print("delete Tick PlayerID:",PlayerId)
		UnRegisterTick(Scene.m_DownTimeTick)
		Scene.m_DownTimeTick = nil
	end
end

-- function CMercenaryRoomCreate:ClearAllTick(Scene)
function CMercenaryRoomCreate.ClearAllTick(Scene)
	if Scene.m_DownTimeTick then
		--print("delete Tick PlayerID:",PlayerId)
		UnRegisterTick(Scene.m_DownTimeTick)
		Scene.m_DownTimeTick = nil
	end

	if Scene.m_TimeOverTick then
		UnRegisterTick(Scene.m_TimeOverTick)
		Scene.m_TimeOverTick = nil
	end
	
	if Scene.m_DividuallyRewardTick then
		UnRegisterTick(Scene.m_DividuallyRewardTick)
		Scene.m_DividuallyRewardTick = nil
	end
end

-- function CMercenaryRoomCreate:GetPlayerBornPos(BornPoint, SelRoomNum)
function CMercenaryRoomCreate.GetPlayerBornPos(BornPoint, SelRoomNum)
	if g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum] then
		local Pos = CPos:new()
		Pos.x = g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].DynamicPoint[1]--YbEducateActionTransport_Server[SelRoomNum]["DynamicPoint"][1]
		Pos.y = g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].DynamicPoint[2]--YbEducateActionTransport_Server[SelRoomNum]["DynamicPoint"][2]
		Pos.x = Pos.x + BornPoint[1]
		Pos.y = Pos.y + BornPoint[2]
		return Pos
	else
		return BornPoint
	end
end

-- 玩家上线恢复游戏
-- function CMercenaryRoomCreate:ResumeLoadYbActionInfo(Player)
function CMercenaryRoomCreate.ResumeLoadYbActionInfo(Player)
	local Scene = Player.m_Scene
	if IsTable(Scene.m_MercenaryActionInfo) then
		Scene.m_MercenaryActionInfo.m_IsGameEnd = nil
		local FloorNum = Scene.m_MercenaryActionInfo.m_FloorNum
		
		local MercenaryEduActMgr = nil
		local NowGameID = Scene.m_MercenaryActionInfo.m_GameID
		if NowGameID then
			MercenaryEduActMgr = g_MercenaryEduActMgr[NowGameID]
		end
		local SelRoomNum = Scene.m_MercenaryActionInfo.m_SelMercenaryRoomNum
		local Pos = CMercenaryRoomCreate.GetPlayerBornPos(MercenaryEduActMgr.m_BornPoint, SelRoomNum)
		Player:SetGridPosByTransport( Pos )
		--local DownTime = MercenaryEduActMgr.m_FitoutTime
		--Gas2Gac:RetShowYbEducateActInfoWnd(Player.m_Conn, MercenaryEduActMgr.m_RulesText, DownTime)
		local LifeNum = Scene.m_MercenaryActionInfo.m_LifeNum
		local MercMoney = Scene.m_MercenaryActionInfo.m_MercMoney
	
		local XS = GetXiShu(FloorNum)
		Gas2Gac:RetUpdateYbEduActInfo(Player.m_Conn, MercMoney, FloorNum, XS, m_ToTalFloor)
		
		-- 判断游戏的阶段	
		if CMercenaryRoomCreate.IsInState(Scene, CMercenaryRoomCreate.GameState.BeforeGame) then   				-- 游戏开始前
			CMercenaryRoomCreate.SendMusicType(Scene, CMercenaryRoomCreate.GameState.BeforeGame)
			local DownTime = MercenaryEduActMgr.m_FitoutTime
			Gas2Gac:RetShowYbEducateActInfoWnd(Player.m_Conn, MercenaryEduActMgr.m_RulesText, DownTime, NowGameID)
			
			--加载所有场景中的信息
			local function TickBack(Tick, param)
				local Player = param[1]
				CMercenaryRoomCreate.ClearDownTimeTick(Scene)
				
				if IsCppBound(Player) and Player.m_Scene.m_MercenaryActionInfo then
					CMercenaryRoomCreate.LoadYbEduActNpcOrObj(Player, NowGameID)
				end
			end
			
			CMercenaryRoomCreate.ClearDownTimeTick(Scene)
			local data = {Player}
			Scene.m_DownTimeTick = RegisterTick("XiuXingTaInfoTick",TickBack,DownTime*1000,data)
				
		elseif CMercenaryRoomCreate.IsInState(Scene, CMercenaryRoomCreate.GameState.StartGame) then				-- 游戏中
			CMercenaryRoomCreate.SendMusicType(Scene, CMercenaryRoomCreate.GameState.StartGame)
			Gas2Gac:RetRecordGameInfo(Player.m_Conn, MercenaryEduActMgr.m_RulesText, NowGameID)
			-- 右边活动说明
			Gas2Gac:RetShowSmallActInfoWnd(Player.m_Conn, NowGameID)
			
			-- 恢复数字
			local OveTypeTempTbl = Scene.m_MercenaryActionInfo.m_OverTypeTempTbl
			for NpcName, Score in pairs(Scene.m_MercenaryActionInfo.m_OverTypeTbl) do
				if NpcName ~= "生存时间" and NpcName ~= "定时" then
					local NameLocal = OveTypeTempTbl[NpcName].Local
					local CountNum = OveTypeTempTbl[NpcName].Num
					if Score == true then
						Gas2Gac:SendResumeNum(Player.m_Conn, true, NameLocal, CountNum, NowGameID)
					else
						Gas2Gac:SendResumeNum(Player.m_Conn, false, NameLocal, CountNum - Score, NowGameID)
					end
				end
			end
			Gas2Gac:RetUpDateSmallInfo(Player.m_Conn)
			CMercenaryRoomCreate.ResumeYbActGame(Player, MercenaryEduActMgr)
			
			-- 加技能
			CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, NowGameID, true)
		elseif CMercenaryRoomCreate.IsInState(Scene, CMercenaryRoomCreate.GameState.GameEnd) then				-- 每层游戏结束后
			CMercenaryRoomCreate.SendMusicType(Scene, CMercenaryRoomCreate.GameState.GameEnd)
			-- 头上加特效
			local playerInfo = Scene.m_MercenaryActionInfo
			local RoomNum = playerInfo.m_SelMercenaryRoomNum
			local SwitchPos = {}
			SwitchPos.x = g_MercenaryEduActMgr.m_RoomPoints[RoomNum].SwitchPoint[1]
			SwitchPos.y = g_MercenaryEduActMgr.m_RoomPoints[RoomNum].SwitchPoint[2]
--			print("上线：SwitchPos.x:"..SwitchPos.x.."  SwitchPos.y"..SwitchPos.y)
			Gas2Gac:SetCompassHeadDir(Player.m_Conn, 2, SwitchPos.x*EUnits.eGridSpanForObj, SwitchPos.y*EUnits.eGridSpanForObj)
			Gas2Gac:RetRecordGameInfo(Player.m_Conn, MercenaryEduActMgr.m_RulesText, NowGameID)
			--如果还有命,就继续
			if (LifeNum > 0 and FloorNum <= m_ToTalFloor) then
				--弹出一个提示面板,看是否还要继续
				local Is15Floor = false
				-- 右边活动说明
--				Gas2Gac:RetShowSmallActInfoWnd(Player.m_Conn, NowGameID)
				-- 活动信息
				Gas2Gac:RetIsContinueYbXlAction(Player.m_Conn,LifeNum,Is15Floor)
				
--				-- 恢复数字
--				local OveTypeTempTbl = Scene.m_MercenaryActionInfo.m_OverTypeTempTbl
--				for NpcName, Score in pairs(Scene.m_MercenaryActionInfo.m_OverTypeTbl) do
--					if NpcName ~= "生存时间" then
--						local NameLocal = OveTypeTempTbl[NpcName].Local
--						local CountNum = OveTypeTempTbl[NpcName].Num
--						if Score == true then
--							Gas2Gac:SendResumeNum(Player.m_Conn, true, NameLocal, CountNum, NowGameID)
--						else
--							-- 判断是否是保护NPC， 是否成功
--							if Scene.m_MercenaryActionInfo.m_IsProtectNpc then
--								if Scene.m_MercenaryActionInfo.m_GameIsSucc then
--									Gas2Gac:SendResumeNum(Player.m_Conn, false, NameLocal, 1, NowGameID)
--								end
--							else
--								Gas2Gac:SendResumeNum(Player.m_Conn, false, NameLocal, CountNum - Score, NowGameID)	
--							end
--						end
--					end
--				end
--				Gas2Gac:RetUpDateSmallInfo(Player.m_Conn)
				
				-- 去掉禁用的技能
				CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, NowGameID, false)
			--else
				-- 如果>=m_ToTalFloor层退出游戏
				--CMercenaryRoomCreate.ClearAllTick(Scene)
				--CMercenaryRoomCreate.GameEndGiveAward(Player, nil)
			end
		elseif CMercenaryRoomCreate.IsInState(Scene, CMercenaryRoomCreate.GameState.PrepareNextGame) then	-- 点击机关后，准备进入下一关
			CMercenaryRoomCreate.SendMusicType(Scene, CMercenaryRoomCreate.GameState.PrepareNextGame)
			Gas2Gac:RetRecordGameInfo(Player.m_Conn, MercenaryEduActMgr.m_RulesText, NowGameID)
			if LifeNum > 0 and FloorNum < m_ToTalFloor then
				-- 右边活动说明
--				Gas2Gac:RetShowSmallActInfoWnd(Player.m_Conn, NowGameID)
				-- 活动信息
				Gas2Gac:RetIsContinueYbXlAction(Player.m_Conn,LifeNum,Is15Floor)
				-- 去掉禁用的技能
				CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, NowGameID, false)
			else
				-- 如果下一层不是奖励层而且 >=m_ToTalFloor层,就退出游戏
				local IsRewardFloor = CMercenaryEducateAct.IsNextRewardFloor(Scene)
				if not IsRewardFloor then
					CMercenaryRoomCreate.ClearAllTick(Scene)
					--AwardItemTbl = GetEndGiveItem(Level)
					CMercenaryRoomCreate.GameEndGiveAward(Player, nil)
				end
			end
		end
		
		if IsCppBound(Player) then
			Player:SetGameCamp(MercenaryEduActMgr.m_GameCamp)
		else
			return
		end
	end
end

--随机刷副本(第一次进入,和踩Trap)
-- function CMercenaryRoomCreate:RandomLoadYbActionInfo(Player, SelRoomNum)
function CMercenaryRoomCreate.RandomLoadYbActionInfo(Player, SelRoomNum)
	local Scene = Player.m_Scene
	if IsTable(Scene.m_MercenaryActionInfo) then
		
		-- 判断是否是奖励层
		if Scene.m_MercenaryActionInfo.m_IsRewardFloor then
			--Scene.m_MercenaryActionInfo.m_FloorNum = "奖励层"
		else
			Scene.m_MercenaryActionInfo.m_FloorNum = Player.m_Scene.m_MercenaryActionInfo.m_FloorNum + 1
			--print("+1后为 "..Scene.m_MercenaryActionInfo.m_FloorNum )
		end
		
		Scene.m_MercenaryActionInfo.m_IsGameEnd = nil

		local FloorNum = Scene.m_MercenaryActionInfo.m_FloorNum
		
		-- 得到GameID
		local GameID = nil
		local MercenaryEduActMgr = nil
		if Player.m_TempYbGameID then		-- 第一次进入
			GameID = Player.m_TempYbGameID
			MercenaryEduActMgr = g_MercenaryEduActMgr[GameID]
			Player.m_TempYbGameID = nil
		else
--			if Scene.m_MercenaryActionInfo.IsCurrFloor then 	-- 玩家重复挑战这一关
--				GameID = Scene.m_MercenaryActionInfo.m_GameID
--				Scene.m_MercenaryActionInfo.IsCurrFloor = false
--			else	-- 新的一关
				GameID = CMercenaryRoomCreate.GetRandomGameID(Player, FloorNum)	
				Scene.m_MercenaryActionInfo.m_GameID = GameID	
--			end

			MercenaryEduActMgr = g_MercenaryEduActMgr[GameID]
			local Pos = CMercenaryRoomCreate.GetPlayerBornPos(MercenaryEduActMgr.m_BornPoint, SelRoomNum)
			--print("---开始传送---  x: "..Pos.x.."  y: "..Pos.y)
			Player:SetGridPosByTransport( Pos )
		end
		
		Scene.m_MercenaryActionInfo.m_SelMercenaryRoomNum = SelRoomNum
		Scene.m_MercenaryActionInfo.m_GameID = GameID
		
		-- 界面
		local DownTime = MercenaryEduActMgr.m_FitoutTime
		Gas2Gac:RetShowYbEducateActInfoWnd(Player.m_Conn, MercenaryEduActMgr.m_RulesText, DownTime, GameID)					
		local LifeNum = Player.m_Scene.m_MercenaryActionInfo.m_LifeNum
		local SuccTimes = Scene.m_MercenaryActionInfo.m_SuccTimes
		--Scene.m_MercenaryActionInfo.m_MercMoney = Scene.m_MercenaryActionInfo.m_MercMoney + Player:CppGetLevel()
		local MercMoney = Scene.m_MercenaryActionInfo.m_MercMoney
		local XS = GetXiShu(FloorNum)
		if Scene.m_MercenaryActionInfo.m_IsRewardFloor then
			Gas2Gac:RetUpdateYbEduActInfo(Player.m_Conn, MercMoney, "奖励层", XS, m_ToTalFloor)
		else
			Gas2Gac:RetUpdateYbEduActInfo(Player.m_Conn, MercMoney, FloorNum, XS, m_ToTalFloor)
		end
		if IsCppBound(Player) then
			Player:SetGameCamp(MercenaryEduActMgr.m_GameCamp)
		else
			return
		end
		
		--加载所有场景中的信息
		local function TickBack(Tick, param)
			local Player = param[1]
			CMercenaryRoomCreate.ClearDownTimeTick(Scene)
			
			if IsCppBound(Player) and Player.m_Scene.m_MercenaryActionInfo then
				CMercenaryRoomCreate.LoadYbEduActNpcOrObj(Player, GameID)
			end
		end
		
		CMercenaryRoomCreate.ClearDownTimeTick(Scene)
		local data = {Player}
		Scene.m_DownTimeTick = RegisterTick("XiuXingTaInfoTick",TickBack,DownTime*1000,data)
	end
end

-- function CMercenaryRoomCreate:BeginYbEducateAction(Conn)
function CMercenaryRoomCreate.BeginYbEducateAction(Conn)
	if IsCppBound(Conn) and IsCppBound(Conn.m_Player) then
		local Player = Conn.m_Player
		local Scene = Player.m_Scene
		if Scene:GetSceneType() == 14 and Scene.m_MercenaryActionInfo then
			-- 如果此时修行塔不在BeforeGame状态则忽略此消息
			if not CMercenaryRoomCreate.IsInState(Player.m_Scene, CMercenaryRoomCreate.GameState.BeforeGame) then 
				return
			end
				
			CMercenaryRoomCreate.ClearDownTimeTick(Player.m_Scene)
			
			local GameID = Player.m_Scene.m_MercenaryActionInfo.m_GameID
			
			--加载所有场景中的信息
			CMercenaryRoomCreate.LoadYbEduActNpcOrObj(Player, GameID)
		end
	end
end

-- function CMercenaryRoomCreate:GetDynamicPoint(SelRoomNum)
function CMercenaryRoomCreate.GetDynamicPoint(SelRoomNum)
	if g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum] then
		local Pos = {}
		Pos[1] = g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].DynamicPoint[1]
		Pos[2] = g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].DynamicPoint[2]
		return Pos
	end
end

--加载所有场景中的信息(开始比赛)
--function CMercenaryRoomCreate:LoadYbEduActNpcOrObj(Player, GameID)
function CMercenaryRoomCreate.LoadYbEduActNpcOrObj(Player, GameID)
	local Scene = Player.m_Scene
	if not CMercenaryRoomCreate.IsInState(Scene, CMercenaryRoomCreate.GameState.BeforeGame) then   				-- 不是游戏开始前，不刷怪
		LogErr("修行塔在游戏进行时刷另一关的怪！", Player.m_Conn)
		return
	end
	
	--CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, GameID, true)
	local MercenaryEduActMgr = g_MercenaryEduActMgr[GameID]
	local NpcFilePath = MercenaryEduActMgr.m_NpcFilePath
	local centerPos = CMercenaryRoomCreate.GetDynamicPoint(Scene.m_MercenaryActionInfo.m_SelMercenaryRoomNum)
	Scene.m_CenterPos = centerPos
	--将本场中的结束条件加玩家身上
	Scene.m_MercenaryActionInfo.m_OverTypeTbl = {}
	Scene.m_MercenaryActionInfo.m_OverTypeTempTbl = {}
	for i, tbl in pairs(MercenaryEduActMgr.OverTypeTbl) do
		Scene.m_MercenaryActionInfo.m_OverTypeTbl[tbl[1]] = tbl[2]
		Scene.m_MercenaryActionInfo.m_OverTypeTempTbl[tbl[1]] = {}
		Scene.m_MercenaryActionInfo.m_OverTypeTempTbl[tbl[1]].Num = tbl[2]
		Scene.m_MercenaryActionInfo.m_OverTypeTempTbl[tbl[1]].Local = i
	end
	
	-- 将本场中的失败条件加玩家身上
	if MercenaryEduActMgr.LoseGameTypeTbl then	
		Scene.m_MercenaryActionInfo.m_LoseGameTypeTbl = {}
		Scene.m_MercenaryActionInfo.m_LoseGameTypeTempTbl = {}
		for i, tbl in pairs(MercenaryEduActMgr.LoseGameTypeTbl) do
			if tbl[1] == "NPC死亡" then
				tbl[2] = 0
			end
			Scene.m_MercenaryActionInfo.m_LoseGameTypeTbl[tbl[1]] = tbl[2]
			Scene.m_MercenaryActionInfo.m_LoseGameTypeTempTbl[tbl[1]] = {}
			Scene.m_MercenaryActionInfo.m_LoseGameTypeTempTbl[tbl[1]].Num = tbl[2]
			Scene.m_MercenaryActionInfo.m_LoseGameTypeTempTbl[tbl[1]].Local = i
		end	
	end
	
	--为了给要保护的身上加一个玩家对象,要不无法知道,要保护的人是谁
	local OverTbl = Scene.m_MercenaryActionInfo.m_OverTypeTbl
	Scene.m_MercenaryActionInfo.m_IsProtectNpc = false
	if OverTbl["生存时间"] then
		if not Scene.m_MercenaryActSaveNpcTbl then
			Scene.m_MercenaryActSaveNpcTbl = {}
		end
		for NpcName,_ in pairs(OverTbl) do
			if NpcName ~= "生存时间" then
				table.insert(Scene.m_MercenaryActSaveNpcTbl,NpcName)				
			end
		end
		Scene.m_MercenaryActionInfo.m_IsProtectNpc = true
	end
	
	local LoseGameType = Scene.m_MercenaryActionInfo.m_LoseGameTypeTbl
	if LoseGameType then
		if LoseGameType["NPC死亡"] then
			if not Scene.m_MercenaryActSaveNpcTbl  then
				Scene.m_MercenaryActSaveNpcTbl = {}
			end
			for NpcName,_ in pairs(LoseGameType) do
				if NpcName ~= "NPC死亡" then
					table.insert(Scene.m_MercenaryActSaveNpcTbl,NpcName)				
				end
			end
			Scene.m_MercenaryActionInfo.m_IsProtectNpc = true
		end
	end
	
	
	g_DynamicCreateMgr:Create(Scene, NpcFilePath, centerPos)
	
	CMercenaryRoomCreate.BeginYbActGame(Player, MercenaryEduActMgr)
	
	local Pos = CMercenaryRoomCreate.GetPlayerBornPos(MercenaryEduActMgr.m_BornPoint, Scene.m_MercenaryActionInfo.m_SelMercenaryRoomNum)
--	print("---开始传送---  x: "..Pos.x.."  y: "..Pos.y)
	Player:SetGridPosByTransport( Pos )
end

-- function CMercenaryRoomCreate:BeginYbActGame(Player, MercenaryEduActMgr)
function CMercenaryRoomCreate.BeginYbActGame(Player, MercenaryEduActMgr)
	local Scene = Player.m_Scene
	CMercenaryRoomCreate.SetGameState(Scene, CMercenaryRoomCreate.GameState.StartGame)
	CMercenaryRoomCreate.ClearDownTimeTick(Scene)
	local OverTbl = Player.m_Scene.m_MercenaryActionInfo.m_OverTypeTbl
	local DownTime = OverTbl["定时"] or OverTbl["生存时间"]
	
	-- 记录游戏进入时间
	Scene.m_MercenaryActionInfo.m_BeginGameTime = math.floor(GetProcessTime() / 1000)
	Scene.m_MercenaryActionInfo.m_CurGameLastTime = nil
	if IsNumber(DownTime) and DownTime > 0 then
--		local nowdate = os.date("*t")
--		local nowmin = nowdate.min
--		local nowsec = nowdate.sec
--	
--		Scene.m_MercenaryActionInfo.m_BeginGameTime = nowmin*60+nowsec
		--Scene.m_MercenaryActionInfo.m_BeginGameTime2 = GetProcessTime()/1000
		Gas2Gac:RetBeginYbEduActGame(Player.m_Conn, DownTime)
		local data = {Scene, MercenaryEduActMgr, Player.m_Conn}

		if Scene.m_TimeOverTick then
			UnRegisterTick(Scene.m_TimeOverTick)
			Scene.m_TimeOverTick = nil
		end

		Scene.m_TimeOverTick = RegClassTick("XiuXingTaBeginTick",CMercenaryRoomCreate.TimeOver,DownTime*1000,self,data)
	end
--	Scene.m_Decreased = false
end

-- function CMercenaryRoomCreate:ResumeYbActGame(Player, MercenaryEduActMgr)
function CMercenaryRoomCreate.ResumeYbActGame(Player, MercenaryEduActMgr)
	local Scene = Player.m_Scene
	local OverTbl = Player.m_Scene.m_MercenaryActionInfo.m_OverTypeTbl
	local DownTime = OverTbl["定时"] or OverTbl["生存时间"]
	if not DownTime or IsBoolean(DownTime) then
		return
	end
	
	-- 计算剩余的时间
--	local Scene = Player.m_Scene	
--	local PlayerId = Player.m_uID
--	local nowdate = os.date("*t")
--	local nowmin = nowdate.min
--	local nowsec = nowdate.sec
--	
--	local Interval = DownTime - (nowmin*60+nowsec) + Scene.m_MercenaryActionInfo.m_BeginGameTime
	local Interval = DownTime - math.floor(GetProcessTime() / 1000) + Scene.m_MercenaryActionInfo.m_BeginGameTime
--	print("Interval  "..Interval.."  DownTime"..DownTime)
	if Interval > 0 then
		Gas2Gac:RetBeginYbEduActGame(Player.m_Conn, Interval)	--开始倒计时
		if Scene.m_TimeOverTick then
			UnRegisterTick(Scene.m_TimeOverTick)
			Scene.m_TimeOverTick = nil
		end
		local data = {Scene, MercenaryEduActMgr}
		--CMercenaryRoomCreate.m_DownTimeTick[PlayerId] = RegClassTick("YbActionBeginTick",CMercenaryRoomCreate.TimeOver,Interval*1000,self,data)
		Scene.m_TimeOverTick = RegClassTick("XiuXingTaBeginTick",CMercenaryRoomCreate.TimeOver,Interval*1000,self,data)
	else
		local data = {Scene, MercenaryEduActMgr}
		CMercenaryRoomCreate.TimeOver(nil, data)
	end
end

--时间到了,结束
function CMercenaryRoomCreate:TimeOver(Tick, data)
	local Scene = data[1]
	local MercenaryEduActMgr = data[2]
	local Conn = data[3]
	if Scene and Scene.m_MercenaryActionInfo then
		
		CMercenaryRoomCreate.ClearAllTick(Scene)
		
		if Scene then
			
			local OverTbl = Scene.m_MercenaryActionInfo.m_OverTypeTbl
			if OverTbl["定时"] then
				OverTbl["定时"] = true
				if MercenaryEduActMgr.IsOverFun(OverTbl) then
					--print("CMercenaryRoomCreate.GameEnd(Scene, false)")
					CMercenaryRoomCreate.GameEnd(Scene, false, true)				
				end
			else
				OverTbl["生存时间"] = true
				CMercenaryRoomCreate.GameEnd(Scene, true)
				
				if IsCppBound(Conn) then
					local GameID = Scene.m_MercenaryActionInfo.m_GameID
					Gas2Gac:RetYbEducateActShowMsg(Conn, GameID, 2, 1, 1)--NPC(1/1)个
				end
				
			end
			
		end
		
	end
end

-- function CMercenaryRoomCreate:SendKillNpcNumMsgByName(Player, OverTypeTbl, Name)
function CMercenaryRoomCreate.SendKillNpcNumMsgByName(Player, OverTypeTbl, Name)
	if not IsCppBound(Player) then
		return
	end
	
	local Conn = Player.m_Conn
	local Scene = Player.m_Scene
	local OveTypeTempTbl = Scene.m_MercenaryActionInfo.m_OverTypeTempTbl
	local LoseGameTempTbl = Scene.m_MercenaryActionInfo.m_LoseGameTypeTempTbl
	local GameID = Scene.m_MercenaryActionInfo.m_GameID
	local Num = OverTypeTbl[Name]
	if Name ~= "定时" and Name ~= "生存时间" then
		if OveTypeTempTbl[Name] then
			local CountNum = OveTypeTempTbl[Name].Num
			local NameLocal = OveTypeTempTbl[Name].Local
			if Num == true then
				Num = 0
			end
			Gas2Gac:RetYbEducateActShowMsg(Conn, GameID, NameLocal, CountNum-Num, CountNum)--NPC(2/5)个
		elseif LoseGameTempTbl[Name] then 
			local CountNum = LoseGameTempTbl[Name].Num
			local NameLocal = LoseGameTempTbl[Name].Local
			if Num == true then
				Num = 0
			end
			Gas2Gac:RetYbEducateActShowLoseGameMsg(Conn, GameID, NameLocal, CountNum-Num, CountNum)--NPC(2/5)个
		end
		--MsgToConn(Player.m_Conn, 3604,Name,CountNum-Num,CountNum)
	end
end

-- function CMercenaryRoomCreate:SendKillNpcNumMsg(Player, OverTypeTbl)
function CMercenaryRoomCreate.SendKillNpcNumMsg(Player, OverTypeTbl)
	if not IsCppBound(Player) then
		return
	end
	
	local Conn = Player.m_Conn
	local Scene = Player.m_Scene
	local OveTypeTempTbl = Scene.m_MercenaryActionInfo.m_OverTypeTempTbl
	local LoseGameTempTbl = Scene.m_MercenaryActionInfo.m_LoseGameTypeTempTbl
	local GameID = Scene.m_MercenaryActionInfo.m_GameID

	for Name,Num in pairs(OverTypeTbl) do
		if Name ~= "定时" and Name ~= "生存时间" and Name ~= "NPC死亡" then
			if OveTypeTempTbl[Name] then
				local CountNum = OveTypeTempTbl[Name].Num
				local NameLocal = OveTypeTempTbl[Name].Local
				if Num == true then
					Num = 0
				end
				Gas2Gac:RetYbEducateActShowMsg(Conn, GameID, NameLocal, CountNum-Num, CountNum)--NPC(2/5)个
			elseif LoseGameTempTbl[Name] then 
				local CountNum = LoseGameTempTbl[Name].Num
				local NameLocal = LoseGameTempTbl[Name].Local
				if Num == true then
					Num = 0
				end
				Gas2Gac:RetYbEducateActShowLoseGameMsg(Conn, GameID, NameLocal, CountNum-Num, CountNum)--NPC(2/5)个
			end
			--MsgToConn(Player.m_Conn, 3604,Name,CountNum-Num,CountNum)
		end
	end
end

--每次杀怪后调用
--有NPC或者是玩家踩Trap后,加任务计数
-- function CMercenaryRoomCreate:KillTargetAddNum(Scene, NpcName, Score)
function CMercenaryRoomCreate.KillTargetAddNum(Scene, NpcName, Score)
	if Scene and Scene.m_MercenaryActionInfo then
		if not Score then
			Score = 1
		end
		local GameID = Scene.m_MercenaryActionInfo.m_GameID
		local OverTbl = Scene.m_MercenaryActionInfo.m_OverTypeTbl
		local LoseGameTbl = Scene.m_MercenaryActionInfo.m_LoseGameTypeTbl
		
		-- 查询玩家，修行塔是单人
		local Player = nil		
		for playerID, player in pairs(Scene.m_tbl_Player) do
			Player = player
		end
		
		local OverTypeMgr = g_MercenaryEduActMgr[GameID].m_OverType -- MercenaryEducateAction_Server(GameID, "OverType")
		local HaveAnd = string.find(OverTypeMgr, "and")
		if OverTbl[NpcName] then
			if OverTbl[NpcName] ~= true then
				OverTbl[NpcName] = OverTbl[NpcName] - Score

				if OverTbl[NpcName] > 0 then	
					if HaveAnd then			
						CMercenaryRoomCreate.SendKillNpcNumMsg(Player, OverTbl)
					else
						CMercenaryRoomCreate.SendKillNpcNumMsgByName(Player, OverTbl, NpcName)
					end
				else
					if OverTbl[NpcName] == 0 then
						if HaveAnd then			
							CMercenaryRoomCreate.SendKillNpcNumMsg(Player, OverTbl)
						else
							CMercenaryRoomCreate.SendKillNpcNumMsgByName(Player, OverTbl, NpcName)
						end
						OverTbl[NpcName] = true
						if g_MercenaryEduActMgr[GameID].IsOverFun(OverTbl) then
							CMercenaryRoomCreate.GameEnd(Scene, true)
						end
					elseif OverTbl[NpcName] < 0 then
						OverTbl[NpcName] = true
						--小于0的,只有保护刷怪器的会进,其它的不会进,也不允许进入(而且只能保护一个刷怪器)
						CMercenaryRoomCreate.GameEnd(Scene, false)--刷怪器死了,就失败
					end
				end
				
			end
		end
		
		if LoseGameTbl and LoseGameTbl[NpcName] then
			if LoseGameTbl[NpcName] ~= true then
				LoseGameTbl[NpcName] = LoseGameTbl[NpcName] - Score
				
				if LoseGameTbl[NpcName] > 0 then
					CMercenaryRoomCreate.SendKillNpcNumMsg(Player, LoseGameTbl)
				else
					if LoseGameTbl[NpcName] == 0 then
						CMercenaryRoomCreate.SendKillNpcNumMsg(Player, LoseGameTbl)
						LoseGameTbl[NpcName] = true
						CMercenaryRoomCreate.GameEnd(Scene, false)
					end
				end
				
			end
		end	
		
	end
end

--完美采集,加任务计数
-- function CMercenaryRoomCreate:CollAddNum(Player, NpcName, Num)
function CMercenaryRoomCreate.CollAddNum(Player, NpcName, Num)
	if Player and Player.m_Scene.m_MercenaryActionInfo then
		local Scene = Player.m_Scene
		local GameID = Scene.m_MercenaryActionInfo.m_GameID
		local OverTbl = Scene.m_MercenaryActionInfo.m_OverTypeTbl
		if OverTbl[NpcName] then
			if OverTbl[NpcName] ~= true then
				OverTbl[NpcName] = OverTbl[NpcName] - Num
				
				if OverTbl[NpcName] > 0 then
					if Num == 3 then
						Gas2Gac:UseItemPlayerEffect(Player.m_Conn,"fx/setting/other/other/xiaoyouxi/create.efx","xiaoyouxi/create")
					end
					CMercenaryRoomCreate.SendKillNpcNumMsg(Player, OverTbl)
				else
					OverTbl[NpcName] = true
					CMercenaryRoomCreate.SendKillNpcNumMsg(Player, OverTbl)
					if g_MercenaryEduActMgr[GameID].IsOverFun(OverTbl) then
						CMercenaryRoomCreate.GameEnd(Scene, true)
					end
				end
				
			end
		end
	end
end

local function XiuXingTaReborn(Player)
	if IsCppBound(Player) and not Player:CppIsLive() then
		Player:Reborn(0.1)
	end
end

--玩家在副本中死掉
-- function CMercenaryRoomCreate:PlayerDeadInYbActFb(Attacker, Player)
function CMercenaryRoomCreate.PlayerDeadInYbActFb(Attacker, Player)
	local Scene = Player.m_Scene
	if Scene.m_MercenaryActionInfo then
		
		if not Scene.m_MercenaryActionInfo.m_IsGameEnd then
			CMercenaryRoomCreate.GameEnd(Scene, false)
		else
			if IsCppBound(Player) and not Player:CppIsLive() then
				RegisterOnceTick(Scene, "m_XiuXingTaRebornTick", XiuXingTaReborn, 2000, Player) -- 三秒后复活		
			end
		end
		
	end
end

-- function CMercenaryRoomCreate:SendPlayerDeadMsg(Player)
function CMercenaryRoomCreate.SendPlayerDeadMsg(Player)
	if not IsCppBound(Player) then
		return
	end
	local Scene = Player.m_Scene
	if Scene.m_MercenaryActionInfo then
		local LifeNum = Scene.m_MercenaryActionInfo.m_LifeNum
		local FloorNum = Scene.m_MercenaryActionInfo.m_FloorNum
		--如果还有命,就继续
		if LifeNum > 0 and FloorNum < m_ToTalFloor then
			--弹出一个提示面板,看是否还要继续
			local Is15Floor = false
			--if FloorNum >= m_ToTalFloor then
			--	Is15Floor = true
			--end
			--CMercenaryRoomCreate.FinishGameGiveAward(Player,true)
			
			-- 改成直接3秒后复活，不询问玩家是否重玩
			--Gas2Gac:RetIsContinueYbXlAction(Player.m_Conn,LifeNum,Is15Floor)
			--Scene.m_MercenaryActionInfo.m_LifeNum = LifeNum - 1  -- 去掉生命限制，玩家有无数条命
			CMercenaryRoomCreate.SetGameState(Scene, CMercenaryRoomCreate.GameState.PrepareNextGame)
		else
			--弹出一个提示面板,然后让其选择退出副本
			--加功能,弹出显示所有物品的面板,给了什么显示什么
			
			if LifeNum <=0 then
				Scene.m_IsLoseGame = true
			end
			Scene.m_MercenaryActionInfo.m_FloorNum = m_ToTalFloor--改为m_ToTalFloor,表示活动结束
		end
		
--	RegisterOnceTick(Scene, "m_XiuXingTaRebornTick", XiuXingTaReborn, 3000, Player) -- 三秒后复活
	end
end

local function DestroyAllFromScene(SceneID)
	g_NpcBornMgr:DeleteAllNpcExceptServant(SceneID)
	DestroyOneSceneIntObj(SceneID)
	DestroyOneSceneTrap(SceneID)
	local Scene = g_SceneMgr:GetScene(SceneID)
	if Scene then
		UnRegisterObjOnceTick(Scene)
	end
end

-- 删除场景内所有东西后，创建机关
local function DelAllAndCreateSwichTick(data)
	local Scene = data.m_Scene
	local Player = data.m_Player
	local SceneID = Scene.m_MercenaryActionInfo.m_FbSceneID
	DestroyAllFromScene(SceneID)

	--去掉禁用的技能-------
	if Scene.m_MercenaryActionInfo.m_GameID then
		CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, Scene.m_MercenaryActionInfo.m_GameID, false)
	end
	-- 刷出机关
	CMercenaryEducateAct.CreateFbTrap(Scene)
	if IsCppBound(Player) and not Player:CppIsLive() then
		--Player:Reborn(0.1)
		RegisterOnceTick(Scene, "m_XiuXingTaRebornTick", XiuXingTaReborn, 2000, Player) -- 三秒后复活		
	end
end

-- function CMercenaryRoomCreate:GameEnd(Scene, IsSucc, IsTimeOver)
function CMercenaryRoomCreate.GameEnd(Scene, IsSucc, IsTimeOver)
	local Player = nil
	for playerID, player in pairs(Scene.m_tbl_Player) do
		Player = player
	end
	CMercenaryRoomCreate.ClearAllTick(Scene)
	if not Scene.m_MercenaryActionInfo.m_IsGameEnd then
		Scene.m_MercenaryActionInfo.m_GameIsSucc = IsSucc
		Scene.m_MercenaryActionInfo.m_IsGameEnd = true
		
		--删除场景内的东西-------------
		local SceneID = Scene.m_MercenaryActionInfo.m_FbSceneID
		DestroyAllFromScene(SceneID)
		
		-- 一秒后再删一次
		local data = {}
		data.m_Player = Player
		data.m_Scene = Scene
		RegisterOnceTick(Scene, "m_DelAllAndCreateSwichTick", DelAllAndCreateSwichTick, 1000, data) 
		--------------------------------
		
		--去掉禁用的技能-------
		if Scene.m_MercenaryActionInfo.m_GameID then
			CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, Scene.m_MercenaryActionInfo.m_GameID, false)
		end
		-----------------------
		
--		--给玩家加满血
--		if IsCppBound(Player) and Player:CppIsLive() then
--			local HP = Player:CppGetMaxHP()
--			local MP = Player:CppGetMaxMP()
--			Player:CppInitHPMP(HP,MP)
--		end
		-------------------------------
		CMercenaryRoomCreate.SetGameState(Scene, CMercenaryRoomCreate.GameState.GameEnd)
		-- 头上加特效
		local playerInfo = Scene.m_MercenaryActionInfo
		local RoomNum = playerInfo.m_SelMercenaryRoomNum
		local SwitchPos = {}
		SwitchPos.x = g_MercenaryEduActMgr.m_RoomPoints[RoomNum].SwitchPoint[1]
		SwitchPos.y = g_MercenaryEduActMgr.m_RoomPoints[RoomNum].SwitchPoint[2]
		
		if IsCppBound(Player) and IsCppBound(Player.m_Conn) then
			Gas2Gac:SetCompassHeadDir(Player.m_Conn,2, SwitchPos.x*EUnits.eGridSpanForObj, SwitchPos.y*EUnits.eGridSpanForObj)
		end
		
		local FloorNum = Scene.m_MercenaryActionInfo.m_FloorNum
		local IsRewardFloor = Scene.m_MercenaryActionInfo.m_IsRewardFloor
		if FloorNum == m_ToTalFloor and IsRewardFloor then
			Scene.m_MercenaryActionInfo.m_XiuXingTaWin = true 	-- 通关
		end
		
		-- 修行塔LOG
		local PlayerID = Scene.m_MercenaryActionInfo.m_PlayerID
		local LastTime = math.floor(GetProcessTime() / 1000) - Scene.m_MercenaryActionInfo.m_BeginGameTime
		Scene.m_MercenaryActionInfo.m_CurGameLastTime = LastTime
		local SaveData = {}
		SaveData.PlayerID = PlayerID
		SaveData.GameID = Scene.m_MercenaryActionInfo.m_GameID
		
		if Scene.m_MercenaryActionInfo.m_IsRewardFloor then
			for i, num in pairs(CMercenaryEducateAct.RewardFloorNumTbl) do
				if num == Scene.m_MercenaryActionInfo.m_FloorNum then
					SaveData.LevelNum = "J"..i
				end
			end
		else
			SaveData.LevelNum = Scene.m_MercenaryActionInfo.m_FloorNum
		end
		
		SaveData.SceneID = Scene.m_SceneId
		SaveData.PlayerID = PlayerID
		SaveData.uSpendTimes = LastTime

		if IsSucc then
			if IsCppBound(Player) and IsCppBound(Player.m_Conn) then
				SaveData.uSucceedFlag = 1
			end
			
			--CMercenaryRoomCreate.SetGameState(Scene, CMercenaryRoomCreate.GameState.PrepareNextGame)
			local OverTbl = Scene.m_MercenaryActionInfo.m_OverTypeTbl
--			local DownTime = OverTbl["定时"] or OverTbl["生存时间"]
--			if DownTime and Player and IsCppBound(Player) then
--				Gas2Gac:RetBeginYbEduActGame(Player.m_Conn, 0)--让计时停止
--			end
			-- 
			if OverTbl["定时"] then
				OverTbl["定时"] = true
			end
			--打印提示信息(奖励的信息)
			--调用奖励接口
			if IsCppBound(Player) then
				MsgToConn(Player.m_Conn,3605)
				Gas2Gac:UseItemPlayerEffect(Player.m_Conn,"fx/setting/other/other/brith/create.efx","brith/create")
				--CMercenaryRoomCreate.FinishGameGiveAward(Player,false)
			end
			-- 加成功计数
			local FloorNum = Scene.m_MercenaryActionInfo.m_FloorNum
			local XS = GetXiShu(FloorNum)
			Scene.m_MercenaryActionInfo.m_SuccTimes= Scene.m_MercenaryActionInfo.m_SuccTimes + 1
			local SuccTimes= Scene.m_MercenaryActionInfo.m_SuccTimes
			Scene.m_MercenaryActionInfo.m_MercMoney = Scene.m_MercenaryActionInfo.m_MercMoney + 1
			local MercMoney = Scene.m_MercenaryActionInfo.m_MercMoney
			if IsCppBound(Player) then
				Gas2Gac:RetUpdateYbEduActInfo(Player.m_Conn, MercMoney, FloorNum, XS, m_ToTalFloor)
			end
		else
			if IsTimeOver then	
				SaveData.uSucceedFlag = 3		-- 时间到结束
			else
				SaveData.uSucceedFlag = 2
			end
			--CMercenaryRoomCreate.SetGameState(Scene, CMercenaryRoomCreate.GameState.LoseGame)
			if IsCppBound(Player) then
				MsgToConn(Player.m_Conn,3606)
				--Gas2Gac:UseItemPlayerEffect(Player.m_Conn,"fx/setting/other/other/xunlian/shibai.efx","xunlian/create")
				CMercenaryRoomCreate.SendPlayerDeadMsg(Player)
			end
		end
		
		if IsCppBound(Player) and IsCppBound(Player.m_Conn) then
			CallAccountAutoTrans(Player.m_Conn.m_Account, "LogMgrDB", "SaveXiuXingTaLogInfo", nil, SaveData)
		end		
		
		local OverTbl = Scene.m_MercenaryActionInfo.m_OverTypeTbl
		local DownTime = OverTbl["定时"] or OverTbl["生存时间"]
		if DownTime and Player and IsCppBound(Player) then
			Gas2Gac:RetBeginYbEduActGame(Player.m_Conn, 0)--让计时停止
		end
		-- 刷出机关
		--g_MercenaryEduAction:CreateFbTrap(Scene)
	end
end

-- function CMercenaryRoomCreate:IsCanClose(Player, IsSucc)
function CMercenaryRoomCreate.IsCanClose(Player, IsSucc)
	if not CMercenaryRoomCreate.m_IsExitGame then
		return false
	end
	return true
end

local function StringToNumber(Level,FloorNum,strInfo)
	if strInfo then
		strInfo = string.gsub( strInfo, "L", Level)
		local count = assert(loadstring( "return " .. strInfo))()
		return count * GetXiShu(FloorNum) / 100--奖励和层数也有关系
	else
		return 0
	end
end

local function GetEndGiveItem(PlayerLevel)
	local FbName = "佣兵训练活动"--佣兵训练活动
	if g_MercenaryAwardItemMgr[FbName] then
		for i=1, #(g_MercenaryAwardItemMgr[FbName]) do
			local ItemMgr = g_MercenaryAwardItemMgr[FbName][i]
			if ItemMgr[1] and ItemMgr[2] then
				local LevelTbl = ItemMgr[1]
				local ItemTbl = ItemMgr[2]
				if LevelTbl[1] and LevelTbl[2] then
					if PlayerLevel >= LevelTbl[1] and PlayerLevel <= LevelTbl[2] then
						return ItemTbl
					end
				elseif LevelTbl[1] and PlayerLevel == LevelTbl[1] then
					return ItemTbl
				end
			end
		end
	end
	return
end

local function CreateXXTTrap(Scene)
	-- 奖励给完后刷出去下关的Trap
	local playerInfo = Scene.m_MercenaryActionInfo
	local SelRoomNum = playerInfo.m_SelMercenaryRoomNum
	local TrapPos = CPos:new()
	TrapPos.x = g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].TrapPoint[1]--YbEducateActionTransport_Server[SelRoomNum]["TrapPoint"][1]
	TrapPos.y = g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].TrapPoint[2]--YbEducateActionTransport_Server[SelRoomNum]["TrapPoint"][2]
	local Trap = CreateServerTrap(Scene, TrapPos, "佣兵训练活动传送点")
end

-- IsFailed 是否失败
--function CMercenaryRoomCreate:FinishGameGiveAward(Player, IsFailed, Scene)
function CMercenaryRoomCreate.FinishGameGiveAward(Player, IsFailed, Scene)
	local playerInfo = Scene.m_MercenaryActionInfo
	if not playerInfo then
		return
	end
	local FloorNum = playerInfo.m_FloorNum
	local IsRewardFloor = CMercenaryEducateAct.IsNextRewardFloor(Scene)
	if FloorNum >= m_ToTalFloor and not IsRewardFloor then  -- 大于等于m_ToTalFloor，并且下一关不是奖励关
		CMercenaryRoomCreate.GameEndGiveAward(Player, AwardItemTbl)
	else
		-- 奖励给完后刷出去下关的Trap
		CreateXXTTrap(Scene)
	end
	
	if not IsCppBound(Player) then
		return
	end
	
	local PlayerId = Player.m_uID
	local GameID = playerInfo.m_GameID
	local MercenaryEduActMgr = g_MercenaryEduActMgr[GameID]
	local Level = Player:CppGetLevel()
	local FloorNum = playerInfo.m_FloorNum
	local dbData = {}
	dbData[PlayerId] = {}
	dbData[PlayerId].isInGame = true
	--dbData[PlayerId].itemTbl = MercenaryEduActMgr.m_AwardItemTbl
	dbData[PlayerId].exp = playerInfo.m_ExpModulus * StringToNumber(Level,FloorNum,MercenaryEduActMgr.m_AwardExp)
	dbData[PlayerId].money = playerInfo.m_MoneyModulus * StringToNumber(Level,FloorNum,MercenaryEduActMgr.m_AwardMoney)
	dbData[PlayerId].fetch = playerInfo.m_ExpModulus * StringToNumber(Level,FloorNum,MercenaryEduActMgr.m_AwardFetch)
	
	if IsFailed then
	--	dbData[PlayerId].itemTbl = nil
		if Level > 15 then
			dbData[PlayerId].exp = dbData[PlayerId].exp / 2
			dbData[PlayerId].money = 0 --dbData[PlayerId].money / 2
			dbData[PlayerId].fetch = dbData[PlayerId].fetch / 2
		else
			dbData[PlayerId].exp = dbData[PlayerId].exp * 0.9
			dbData[PlayerId].money = dbData[PlayerId].money * 0.9
			dbData[PlayerId].fetch = dbData[PlayerId].fetch * 0.9
		end
	end
	
	if Level < 7 then
		dbData[PlayerId].exp = dbData[PlayerId].exp * 1.5
	end
	local AwardItemTbl = nil
	if FloorNum >= m_ToTalFloor then
		-- 去掉奖励
		--AwardItemTbl = GetEndGiveItem(Level)
		if IsFailed and Player.m_Scene.m_IsLoseGame then
			AwardItemTbl = nil
		end
		if AwardItemTbl and next(AwardItemTbl) then
			if dbData[PlayerId].itemTbl then
				for i=1, #(AwardItemTbl)do
					table.insert(dbData[PlayerId].itemTbl,AwardItemTbl[i])
				end
			else
				dbData[PlayerId].itemTbl = AwardItemTbl
			end
		end
		local ExtraExp = Player.m_Scene.m_MercenaryActionInfo.m_MercMoney * ExtraExpPara * Player:CppGetLevel()
		--print("dbData[PlayerId].exp"..dbData[PlayerId].exp.."    额外奖励 "..ExtraExp)
		dbData[PlayerId].exp = dbData[PlayerId].exp + ExtraExp
		--print("dbData[PlayerId].exp  = "..dbData[PlayerId].exp)
	end
	
	dbData[PlayerId].exp = math.floor(dbData[PlayerId].exp)
	dbData[PlayerId].money = math.floor(dbData[PlayerId].money)
	dbData[PlayerId].fetch = math.floor(dbData[PlayerId].money)
	dbData["MailTitle"] = 1022
	dbData["MailContent"] = 1023
	
	Player:OnSavePlayerSoulTickFunc(nil,Player)
	local function CallBack(IsSucc, result)
		if IsSucc and Player and IsCppBound(Player) then
--			local AwardTbl = {}
			-- 经验
			local expResult = result[PlayerId][1]
			if expResult and expResult[1] then
				MsgToConn(Player.m_Conn,3600,dbData[PlayerId].exp)
				--CRoleQuest.AddPlayerExp_DB_Ret(Player, {expResult[1],expResult[2],dbData[PlayerId].exp, 0})
				local AddExpTbl = {}
				AddExpTbl["Level"] = expResult[1]
				AddExpTbl["Exp"] = expResult[2] -- -(DividedNum-Tick.AwardTimes)*AwardTbl.m_AddExp
				AddExpTbl["AddExp"] = dbData[PlayerId].exp
				AddExpTbl["uInspirationExp"] = 0
				CRoleQuest.AddPlayerExp_DB_Ret(Player, AddExpTbl)		
			end

			-- 金钱
			if result[PlayerId][2][1] then
				MsgToConn(Player.m_Conn,3602,dbData[PlayerId].money)
			else
				if IsNumber(result[PlayerId][2][2]) then
					MsgToConn(Player.m_Conn,result[PlayerId][2][2])
				end
			end
			
			-- 魂
			if result[PlayerId][4] then
				MsgToConn(Player.m_Conn,3601,dbData[PlayerId].fetch)
				Player.m_uSoulValue = result[PlayerId][4] + Player.m_uKillDropSoulCount
				Gas2Gac:ReturnModifyPlayerSoulNum(Player.m_Conn, Player.m_uSoulValue)
			end
			
			-- 记录累计经验、金钱、魂
			Scene.m_MercenaryActionInfo.m_TotalExp = Scene.m_MercenaryActionInfo.m_TotalExp + dbData[PlayerId].exp
			Scene.m_MercenaryActionInfo.m_TotalMoney = Scene.m_MercenaryActionInfo.m_TotalMoney + dbData[PlayerId].money
			Scene.m_MercenaryActionInfo.m_TotalSoul = Scene.m_MercenaryActionInfo.m_TotalSoul + dbData[PlayerId].fetch
			
			local itemResult = result[PlayerId][3]
			if next(itemResult) then
				for i=1,#(itemResult) do
					if IsNumber(itemResult[i][4]) then
						if itemResult[i][4] == 3 then
							--,'包裹空间不足'
							MsgToConn(Player.m_Conn,160000)
						elseif itemResult[i][4] == 0 then
							MsgToConn(Player.m_Conn,8003,'CreateFightItem error')
						end
					else
						MsgToConn(Player.m_Conn,3603,itemResult[i][3],itemResult[i][2])
						CRoleQuest.add_item_DB_Ret(Player, itemResult[i][1], itemResult[i][2], itemResult[i][3],itemResult[i][4])
					end
				end
			end
			
			--创建下一层的Trap
			if FloorNum >= m_ToTalFloor then
				--CMercenaryRoomCreate.GameEndGiveAward(Player, AwardItemTbl)
				if not Player.m_Scene.m_IsLoseGame then
					-- 数据库中增加成功计数
					local parameters = {}
					parameters["PlayerId"] = PlayerId
					parameters["ActivityName"] = "佣兵训练活动"
					CallDbTrans("ActivityCountDB", "AddSuccessRecordCount", nil, parameters, PlayerId)
				end
			elseif not IsFailed then
				--g_MercenaryEduAction:CreateFbTrap(Player.m_Scene)
			end
		
		end
	end
	
	dbData["sceneName"] = Player.m_Scene.m_SceneName
	dbData["sceneType"] = Player.m_Scene.m_SceneAttr.SceneType
	OnSavePlayerExpFunc({Player})
	CallDbTrans("RoleQuestDB", "FbActionAward", CallBack, dbData, Player.m_AccountID)
end

-- function CMercenaryRoomCreate:GameEndGiveAward(Player, ItemTbl)
function CMercenaryRoomCreate.GameEndGiveAward(Player, ItemTbl)
	if not IsCppBound(Player) or not IsCppBound(Player.m_Conn) then
		return
	end
	
	local Scene = Player.m_Scene
	Gas2Gac:RetOverYbXlAction(Player.m_Conn,1,"",1)
--	if ItemTbl and next(ItemTbl) then
--		for i=1, #(ItemTbl) do
--			Gas2Gac:RetOverYbXlAction(Player.m_Conn,2,ItemTbl[i][2],ItemTbl[i][3])
--		end
--	end 
	Gas2Gac:RetOverYbXlAction(Player.m_Conn,2,"总经验",Scene.m_MercenaryActionInfo.m_TotalExp)
	Gas2Gac:RetOverYbXlAction(Player.m_Conn,2,"总金钱",Scene.m_MercenaryActionInfo.m_TotalMoney)
	Gas2Gac:RetOverYbXlAction(Player.m_Conn,2,"总魂值",Scene.m_MercenaryActionInfo.m_TotalSoul)
	Gas2Gac:RetOverYbXlAction(Player.m_Conn,2,"额外经验",Scene.m_MercenaryActionInfo.m_MercMoney * ExtraExpPara * Player:CppGetLevel())
	Gas2Gac:RetOverYbXlAction(Player.m_Conn,3,"",m_CloseFbTime)
	
	-- 加灵感值
	local uAfflatusValue = FbAfflatus_Common("佣兵训练活动", "AfflatusValue")
	if uAfflatusValue and IsNumber(uAfflatusValue)then
		Player:AddPlayerAfflatusValue(uAfflatusValue)
	end
	
	local function OverGameFb(Tick, param)
		local Player = param[1]
		local Scene = Player.m_Scene
		CMercenaryRoomCreate.ClearAllTick(Scene)
		if IsCppBound(Player) then
			CMercenaryEducateAct.ExitYbEducateAction(Player.m_Conn)
		end
	end
	local data = {Player}
--	CMercenaryRoomCreate.m_DownTimeTick[Player.m_uID] = RegisterTick("YbActionEndTick",OverGameFb,m_CloseFbTime*1000,data)
	CMercenaryRoomCreate.ClearDownTimeTick(Scene)
	Scene.m_DownTimeTick = RegisterTick("XiuXingTaEndTick",OverGameFb,m_CloseFbTime*1000,data)
end

-- function CMercenaryRoomCreate:SendMusicType(Scene, State)
function CMercenaryRoomCreate.SendMusicType(Scene, State)
	local MusicType = nil
	if State == CMercenaryRoomCreate.GameState.StartGame then
		if Scene.m_MercenaryActionInfo.m_IsRewardFloor then
			MusicType = 1	-- 奖励关的音乐
		else
			MusicType = 2	-- 普通关音乐
		end
	else
		MusicType = 3	-- 等待时音乐
	end
	
	--if Scene.m_MercenaryActionInfo.LastMusicType ~= MusicType then		
		for id,Player in pairs(Scene.m_tbl_Player) do
			Gas2Gac:SendPlayXiuXingTaBgm(Player.m_Conn, MusicType)
		end
		Scene.m_MercenaryActionInfo.LastMusicType = MusicType
	--end
end

-- function CMercenaryRoomCreate:SetGameState(Scene, State)
function CMercenaryRoomCreate.SetGameState(Scene, State)
	--print("游戏状态为："..State)
	Scene.m_MercenaryActionInfo.GameState = State
	CMercenaryRoomCreate.SendMusicType(Scene, State)
end

-- function CMercenaryRoomCreate:IsInState(Scene, State)
function CMercenaryRoomCreate.IsInState(Scene, State)
	if Scene.m_MercenaryActionInfo.GameState == State then
		--print("上线后游戏状态为: "..State)
		--CMercenaryRoomCreate.SendMusicType(Scene, State)
		return true
	else
		return false
	end
end
