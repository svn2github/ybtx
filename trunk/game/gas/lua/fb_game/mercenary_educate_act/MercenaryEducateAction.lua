local m_FbName = "佣兵训练活动"--佣兵训练活动
local m_PlayerLife = 5  --每个人进入游戏后,有5条命
--local RewardFloorNumTbl = {4,8} -- 在4、8层后刷出奖励关
local RewardRoomNum = 1 -- 奖励关的房间号

CMercenaryEducateAct = class()
CMercenaryEducateAct.RewardFloorNumTbl = {4}
--g_MercenaryEduAction.RewardFloorNumTbl = {4,8} -- 在4、8层后刷出奖励关

function CMercenaryEducateAct.CreateFbTrap(Scene)
  if Scene.m_MercenaryActionInfo then
    local SelRoomNum = Scene.m_MercenaryActionInfo.m_SelMercenaryRoomNum
    if Scene and SelRoomNum and IsNumber(SelRoomNum) then
      local TrapPos = CPos:new()
      if g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum] then
        TrapPos.x = g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].SwitchPoint[1]--YbEducateActionTransport_Server[SelRoomNum]["SwitchPoint"][1]
        TrapPos.y = g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].SwitchPoint[2]--YbEducateActionTransport_Server[SelRoomNum]["SwitchPoint"][2]
        --local Trap = CreateServerTrap(Scene, TrapPos, "佣兵训练活动传送点")
        --print("刷出======修行塔用拉杆")
        Scene.m_CreateAwardNpcSwitch = CreateIntObj(Scene, TrapPos, "修行塔用拉杆")
      end
    end
  end
end

--将所有活动的信息,挂玩家身上
function CMercenaryEducateAct.SetPlayerActInfoOnPlayer(Player)
  -- 玩家从别的场景进入修行塔
  Player.m_MercenaryEduActTbl = {}
  for i, ActionMgr in pairs(g_MercenaryEduActMgr) do
    Player.m_MercenaryEduActTbl[i] = {}
    Player.m_MercenaryEduActTbl[i].m_GameID = ActionMgr.m_GameID
    Player.m_MercenaryEduActTbl[i].m_GameType = ActionMgr.m_GameType
    Player.m_MercenaryEduActTbl[i].m_MinLevel = ActionMgr.m_MinLevel
    Player.m_MercenaryEduActTbl[i].m_MinFloor = ActionMgr.m_MinFloor
  end   
end

--将所有活动的信息,挂场景身上
function CMercenaryEducateAct.SetPlayerActInfo(Player)
  local Scene = Player.m_Scene
  if not Scene.m_MercenaryActionInfo then
    Scene.m_MercenaryActionInfo = {}
  end
  local MercInfo = Scene.m_MercenaryActionInfo
  
  MercInfo.m_PlayerID = Player.m_uID
  if not MercInfo.m_MercenaryEduActTbl then
    MercInfo.m_MercenaryEduActTbl = {}
    for i, ActionMgr in pairs(g_MercenaryEduActMgr) do
      MercInfo.m_MercenaryEduActTbl[i] = {}
      MercInfo.m_MercenaryEduActTbl[i].m_GameID = ActionMgr.m_GameID
      MercInfo.m_MercenaryEduActTbl[i].m_GameType = ActionMgr.m_GameType
      MercInfo.m_MercenaryEduActTbl[i].m_MinLevel = ActionMgr.m_MinLevel
      MercInfo.m_MercenaryEduActTbl[i].m_MinFloor = ActionMgr.m_MinFloor
    end
  end
    
  -- 初始化关卡列表
  if not MercInfo.m_TempMercCardInfo then
    MercInfo.m_TempMercCardInfo = {}
    for i=1, #(Player.m_MercCardInfo) do
      MercInfo.m_TempMercCardInfo[i] = Player.m_MercCardInfo[i]
    end
  end
  -- 去掉在进入副本时随机到的游戏
  if Player.m_TempYbGameID then
    table.remove(MercInfo.m_TempMercCardInfo, Player.m_TempYbGameID)
  end 
end

function CMercenaryEducateAct.GetPlayerToPos(Player)
  local MaxRoomNum = g_MercenaryEduActMgr.m_RoomPoints.MaxRoomNum
  local Scene = Player.m_Scene
  local SelRoomNum = CMercenaryEducateAct.GetNextRoomNum(Player)
  Player.m_TempRoomNum = SelRoomNum--玩家一进入副本后,得到的当前房间号

  CMercenaryEducateAct.SetPlayerActInfoOnPlayer(Player)

  local GameID = CMercenaryRoomCreate.GetRandomGameID(Player,1)
  Player.m_TempYbGameID = GameID
  local MercenaryEduActMgr = g_MercenaryEduActMgr[GameID]
  return CMercenaryRoomCreate.GetPlayerBornPos(MercenaryEduActMgr.m_BornPoint, SelRoomNum)
end

function CMercenaryEducateAct.PlayerIntoScene(Player)
  local PlayerId = Player.m_uID
  local playerInfo = Player.m_Scene.m_MercenaryActionInfo
  playerInfo.m_ExpModulus = 1
  playerInfo.m_MoneyModulus = 1
  local nowTime = os.time()
  
  local index = GetExtraRewardActionIndex(m_FbName, nowTime)
  --print("当前活动index " .. (index or "空"))
  if index then
    local extraRewardData, criticalTime = GetTodayExtraRewardInfo(nowTime)
    local data = {}
    data["PlayerId"] = PlayerId
    data["ActionName"] = m_FbName
    data["CriticalTime"] = criticalTime
    data["MaxCount"] = extraRewardData.ActionCountTbl[index]
    data["ActionTbl"] = extraRewardData.ActionTbl
    local function CallBack(curCount, AllCount)
      if curCount < extraRewardData.ActionCountTbl[index]  
        and AllCount < extraRewardData.AllRewardCount  then
        --player.m_FbActionExpModulus = extraRewardData.ExpModulusTbl[index]
        --player.m_FbActionMoneyModulus = extraRewardData.MoneyModulusTbl[index]
        playerInfo.m_ExpModulus = extraRewardData.ExpModulusTbl[index]
        playerInfo.m_MoneyModulus = extraRewardData.MoneyModulusTbl[index]
      end
    end
    CallAccountAutoTrans(Player.m_Conn.m_Account, "ExtraRewardDB", "AddExtraRewardCount", CallBack, data)
  end
  
  local parameters = {}
  if Player.m_Scene.m_IsCheckTimes == 1 then
	  parameters["PlayerId"] = PlayerId
	  parameters["ActivityName"] = m_FbName
	  parameters["ActionName"] = m_FbName
	  parameters["SceneName"] = Player.m_Scene.m_SceneName
	  CallDbTrans("ActivityCountDB", "AddXiuXingTaCount", nil, parameters, PlayerId)
	else
		parameters["charId"] = PlayerId
		parameters["SceneName"] = Player.m_Scene.m_SceneName
		CallDbTrans("ActivityCountDB", "RecordActionTimes", nil, parameters, PlayerId)
	end
end

function CMercenaryEducateAct.ResumeOpenPlayerInfoWnd(Player)
  Gas2Gac:RetInitFbActionQueueWnd(Player.m_Conn,m_FbName)
  Gas2Gac:RetOpenFbActionWnd(Player.m_Conn,m_FbName,0)
  CMercenaryRoomCreate.ResumeLoadYbActionInfo(Player)  -- 这里和OpenPlayerInfoWnd不一样
  Player.m_TempRoomNum = nil
end

function CMercenaryEducateAct.OpenPlayerInfoWnd(Player)
  if Player.m_TempRoomNum then
    Gas2Gac:RetInitFbActionQueueWnd(Player.m_Conn,m_FbName)
    Gas2Gac:RetOpenFbActionWnd(Player.m_Conn,m_FbName,0)
    CMercenaryRoomCreate.RandomLoadYbActionInfo(Player, Player.m_TempRoomNum)
    Player.m_TempRoomNum = nil
  end
end

-- 判断下一关是否是奖励关
function CMercenaryEducateAct.IsNextRewardFloor(Scene)
  -- 奖励关不能连续
  if Scene.m_MercenaryActionInfo.m_IsRewardFloor then
    return false
  end
    
  -- 判断是否刷出奖励关
  local IsRewardFloor = false
  local FloorNum = Scene.m_MercenaryActionInfo.m_FloorNum
  for _, Num in pairs(CMercenaryEducateAct.RewardFloorNumTbl) do
    if Num == FloorNum then
      IsRewardFloor = true
      break
    end
  end
  return IsRewardFloor
end

-- 确保每层进入的房间不同,奖励关为专门的房间1
function CMercenaryEducateAct.GetNextRoomNum(Player)
  local Scene = Player.m_Scene
  local SelRoomNum = nil
  
  if Player.GMGotoMercLevel and Player.GMGotoMercLevel >= 1000 then -- 如果用GM指令跳进奖励关
    SelRoomNum = RewardRoomNum
    return SelRoomNum
  end
  
  if Scene.m_MercenaryActionInfo and Scene.m_MercenaryActionInfo.m_IsRewardFloor then -- 是奖励关
    SelRoomNum = RewardRoomNum
  else
    local CurRoomNum = nil
    if Scene.m_MercenaryActionInfo then
      CurRoomNum = Scene.m_MercenaryActionInfo.m_SelMercenaryRoomNum
    else
      CurRoomNum = nil
    end
    local MaxRoomNum = g_MercenaryEduActMgr.m_RoomPoints.MaxRoomNum
    local RoomNumTbl = {}
    for i=1, MaxRoomNum do
      if i ~= CurRoomNum and i ~= RewardRoomNum then  -- 不等于当前房间号和奖励关房间号
        table.insert(RoomNumTbl, i)
      end
    end
    local RandomNum = math.random(1,table.getn(RoomNumTbl))
    SelRoomNum = RoomNumTbl[RandomNum]
  end
  return SelRoomNum
end

local function ChangeSceneSucc(Player, Scene)
  if Player.m_TempRoomNum then
    if not Scene.m_MercenaryActionInfo then
      Scene.m_MercenaryActionInfo = {}
    end
    --加动态等级
--    Scene.m_SceneBaseLv = Player:CppGetLevel() - 10
    -- 初始化
    Scene.m_MercenaryActionInfo.m_FbSceneID = Scene.m_SceneId
    Scene.m_MercenaryActionInfo.m_LifeNum = m_PlayerLife - 1
    Scene.m_MercenaryActionInfo.m_FloorNum = 0
    Scene.m_MercenaryActionInfo.m_SuccTimes = 0
    Scene.m_MercenaryActionInfo.m_TotalExp = 0
    Scene.m_MercenaryActionInfo.m_TotalMoney = 0
    Scene.m_MercenaryActionInfo.m_TotalSoul = 0
    Scene.m_MercenaryActionInfo.m_MercMoney = 0
    --Scene.m_MercenaryActionInfo.m_Player = Player
    --得到当前能给的经验倍数
    CMercenaryEducateAct.PlayerIntoScene(Player)
  end
end

function CMercenaryEducateAct:JoinYbEducate(Player, SceneName, IsWnd)
	local SceneCommon = Scene_Common[SceneName]
	if not SceneCommon or SceneCommon.SceneType ~= 14 then
    return
  end
  
  local ToScenePos = CMercenaryEducateAct.GetPlayerToPos(Player)
  local function CallBack(isSucceed, errMsg, sceneId, serverId)
    if not IsCppBound(Player) then
      return
    end
    if isSucceed then
      if not ToScenePos then
        return
      end
      MultiServerChangeScene(Player.m_Conn, sceneId, serverId, ToScenePos.x , ToScenePos.y)
    else
      if errMsg == 1 then -- 表示为20级以上限制次数不够
        local msgId = 191019
        MsgToConn(Player.m_Conn, msgId, "", m_FbName)
      elseif errMsg == 2 then
        local msgId = 191019 -- 表示为20级以下限制次数不够, 只能参加一次
        MsgToConn(Player.m_Conn, msgId, "", m_FbName)
      end
    end
  end
  
  local data = {}
  data["charId"] = Player.m_uID
  data["SceneName"] = SceneName
  data["PlayerLevel"] = Player:CppGetLevel()
  --data["MercenaryEduActTbl"] = Player.m_MercenaryEduActTbl
  --data["TempMercCardInfo"] = Player.TempMercCardInfo
 
  data["OtherArg"] = {}
	data["OtherArg"]["m_SceneBaseLv"] = Player:CppGetLevel() - 10
	data["OtherArg"]["m_TempRoomNum"] = Player.m_TempRoomNum
	data["OtherArg"]["m_TempYbGameID"] = Player.m_TempYbGameID
	data["OtherArg"]["m_IsCheckTimes"] = not IsWnd and 1 or 0
	
	if Player.GMGotoMercLevel then
		data["OtherArg"]["m_GMMercLevel"] = Player.GMGotoMercLevel
	end
	
  CallAccountManualTrans(Player.m_Conn.m_Account, "SceneMgrDB", "GetYbActFbScene", CallBack, data)
end

function CMercenaryEducateAct.JoinYbEducateAction(Conn, SceneName)
	local Player = Conn.m_Player
	if(not Player and not IsCppBound(Player) ) then return end
	if( Player:IsInBattleState() ) then
		MsgToConn(Conn, 410023)
		return
	end
	if Player.m_Scene.m_SceneAttr.SceneType == 14 then
		MsgToConn(Conn, 410024)
		return
	end
	if SceneName == "佣兵训练营" then
		CMercenaryEducateAct.JoinYbEducateActionFromNpc(Conn, SceneName)
	else
		CMercenaryEducateAct.JoinYbEducateActionFromWnd(Conn, SceneName)
	end
end

function CMercenaryEducateAct.JoinYbEducateActionFromWnd(Conn, SceneName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	CMercenaryEducateAct:JoinYbEducate(Player, SceneName, true)
end

function CMercenaryEducateAct.JoinYbEducateActionFromNpc(Conn, SceneName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	--比较等级
	local minLevel = FbActionDirect_Common(m_FbName, "MinLevel")
	if Player:CppGetLevel() < minLevel then
		MsgToConn(Conn,3607,minLevel)
		return
	end
	-----------
	CMercenaryEducateAct:JoinYbEducate(Player, SceneName, false)
end

function CMercenaryEducateAct.ShowYbEducateActionWnd(Conn, NpcID)
  if not CheckAllNpcFunc(Conn.m_Player,NpcID,"修行塔报名处") then
    return
  end
  Gas2Gac:RetShowYbEducateActionWnd(Conn)
end

--玩家掉线
function CMercenaryEducateAct.PlayerOffLineYbActFb(Player)
  local Scene = Player.m_Scene
  --Scene.m_MercenaryActionInfo.m_Player = nil
  local function Init()
    if Scene.m_MercenaryActionInfo then
      CMercenaryRoomCreate.ClearDownTimeTick(Scene)
      --去掉禁用的技能-------
      if Scene.m_MercenaryActionInfo.m_GameID then
      	--如果手上拿着的东西,就先放下
				CClaspBehavior.DroptBehaviorActualize(Player)
        CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, Scene.m_MercenaryActionInfo.m_GameID, false)
      end
      -----------------------
      -- 清除GM指令
      if Player.GMGotoMercLevel then
        Player.GMGotoMercLevel = nil
      end
      
      -- 记录掉线时间
      local nowdate = os.date("*t")
      local nowmin = nowdate.min
      local nowsec = nowdate.sec
      Scene.m_MercenaryActionInfo.m_OffLineTime = nowmin*60+nowsec
      Scene.m_MercenaryActionInfo.PlayerOffLine = true
    end
  end
  apcall(Init)
end

-- 玩家上线
function CMercenaryEducateAct.PlayerLoginYbEducationAction(Player)  
  local Scene = Player.m_Scene
  if Scene.m_MercenaryActionInfo.PlayerOffLine then
    Scene.m_MercenaryActionInfo.PlayerOffLine = nil
  end 
  -- 清除上次掉线清除场景TICK
  if Scene.m_CloseYbActionTick then
    UnRegisterTick(Scene.m_CloseYbActionTick)
    Scene.m_CloseYbActionTick = nil
  end
  CMercenaryEducateAct.SetPlayerActInfo(Player)

  if Scene.m_MercenaryActionInfo then
    -- 恢复信息 
    --Scene.m_MercenaryActionInfo.m_Player = Player
    -- 恢复界面
    CMercenaryEducateAct.ResumeOpenPlayerInfoWnd(Player)
  end

end

function CMercenaryEducateAct.IntoYbActFbScene(Player)
  local Scene = Player.m_Scene
  Player.m_TempRoomNum = Scene["m_TempRoomNum"]
  Player.m_TempYbGameID = Scene["m_TempYbGameID"]
  if Scene["m_GMMercLevel"] then
    Player.GMGotoMercLevel = Scene["m_GMMercLevel"]
  end
  
  CMercenaryEducateAct.SetPlayerActInfo(Player)
  ChangeSceneSucc(Player, Player.m_Scene)
  -- 如果是跳到活动中的话,就打开相应的窗口
  CMercenaryEducateAct.OpenPlayerInfoWnd(Player)
  -- 游戏开始前
  CMercenaryRoomCreate.SetGameState(Player.m_Scene, CMercenaryRoomCreate.GameState.BeforeGame)  
	local GameID = Scene.m_MercenaryActionInfo.m_GameID
	CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, GameID, true)
end

--玩家在副本中切场景
function CMercenaryEducateAct.ChangeSceneYbActFb(Player)
  local Scene = Player.m_Scene
  if Scene.m_MercenaryActionInfo then
    -- 清除倒计时的Tick
    CMercenaryRoomCreate.ClearAllTick(Scene)
    
    if Scene.m_MercenaryActionInfo.m_GameID then
      CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, Player.m_Scene.m_MercenaryActionInfo.m_GameID, false)
    end
    Player:SetGameCamp(0)
    Gas2Gac:RetDelQueueFbAction(Player.m_Conn,m_FbName, true)
    Gas2Gac:RetCloseFbActionWnd(Player.m_Conn,m_FbName)
    Gas2Gac:RetCloseYbEducateActInfoWnd(Player.m_Conn)
    -- 停止播放音乐
    Gas2Gac:RetStopXiuXingTaBgm(Player.m_Conn)
    -- 清除GM指令
    if Player.GMGotoMercLevel then
      Player.GMGotoMercLevel = nil
    end

    -- 修行塔还没有通关就退出,记录修行塔LOG 
    if not Scene.m_MercenaryActionInfo.m_XiuXingTaWin then
      local PlayerID = Scene.m_MercenaryActionInfo.m_PlayerID
      local LastTime = 0
      if Scene.m_MercenaryActionInfo.m_CurGameLastTime then
        LastTime = Scene.m_MercenaryActionInfo.m_CurGameLastTime
      else
        if Scene.m_MercenaryActionInfo.m_BeginGameTime then
          LastTime = math.floor(GetProcessTime() / 1000) - Scene.m_MercenaryActionInfo.m_BeginGameTime
        else
          LastTime = 0
        end
      end
      
      if IsCppBound(Player) and IsCppBound(Player.m_Conn) then
        local data = {}
        data.PlayerID = PlayerID
        
        if Scene.m_MercenaryActionInfo.m_IsRewardFloor then
          for i, num in pairs(CMercenaryEducateAct.RewardFloorNumTbl) do
            if num == Scene.m_MercenaryActionInfo.m_FloorNum then
              data.LevelNum = "J"..i
            end
          end
        else
          data.LevelNum = Scene.m_MercenaryActionInfo.m_FloorNum
        end
  
        data.GameID = Scene.m_MercenaryActionInfo.m_GameID
        data.SceneID = Scene.m_SceneId
        if Scene.m_ExitYbEducateAction then
          data.uSucceedFlag = 4
        else
          data.uSucceedFlag = 5
        end
        data.PlayerID = PlayerID
        data.uSpendTimes = LastTime
        CallAccountAutoTrans(Player.m_Conn.m_Account, "LogMgrDB", "SaveXiuXingTaLogInfo", nil, data)
      end
    end
  end
end

--玩家放弃比赛
function CMercenaryEducateAct.ExitYbEducateAction(Conn)
  local Player = Conn.m_Player
  if not IsCppBound(Player) then
    return
  end
  
  local Scene = Player.m_Scene
  -- 标记为玩家放弃
  Scene.m_ExitYbEducateAction = true
  if Player and Scene.m_MercenaryActionInfo then
    CMercenaryRoomCreate.ClearAllTick(Scene)
    
    if Scene.m_MercenaryActionInfo.m_GameID then
      CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, Player.m_Scene.m_MercenaryActionInfo.m_GameID, false)
    end
    Player:SetGameCamp(0)
--    Gas2Gac:RetDelQueueFbAction(Conn,m_FbName)
--    Gas2Gac:RetCloseFbActionWnd(Conn,m_FbName)
--    Gas2Gac:RetCloseYbEducateActInfoWnd(Conn)
    local SceneName = Player.m_MasterSceneName
    local Pos = Player.m_MasterScenePos
    ChangeSceneByName(Conn,SceneName,Pos.x,Pos.y)
  
    -- 清除GM指令
    if Player.GMGotoMercLevel then
      Player.GMGotoMercLevel = nil
    end
    
    -- 停止播放音乐
    --Gas2Gac:RetStopXiuXingTaBgm(Conn)
  end
end

-- 玩家选择是重新挑战还是进入下一关
--function Gac2Gas:ContinueYbXlAction(Conn,IsCurrFloor)
--  local Player = Conn.m_Player
--  if not Player then
--    return
--  end
--  local Scene = Player.m_Scene
--  if Player and Scene.m_MercenaryActionInfo then
--    local Scene = Player.m_Scene
--    local LifeNum = Scene.m_MercenaryActionInfo.m_LifeNum
--    --如果还有命,就继续
--    if LifeNum > 0 then
----      if not Scene.m_Decreased then
--        Scene.m_MercenaryActionInfo.m_LifeNum = LifeNum - 1
----        Scene.m_Decreased = true
----      end
--
----      if IsCurrFloor then
----        if not Player:CppIsLive() then
----          Player:Reborn(1)
----        end
----        local GameID = Scene.m_MercenaryActionInfo.m_GameID
----        --加载所有场景中的信息
----        
----        local MercenaryEduActMgr = g_MercenaryEduActMgr[GameID]
----        local SelRoomNum = Scene.m_MercenaryActionInfo.m_SelMercenaryRoomNum
----        local Pos = CMercenaryRoomCreate.GetPlayerBornPos(MercenaryEduActMgr.m_BornPoint, SelRoomNum)
----        Player:SetGridPosByTransport( Pos )
----        Scene.m_MercenaryActionInfo.m_FloorNum = Scene.m_MercenaryActionInfo.m_FloorNum + 1--重新挑战也加一层
----        Scene.m_MercenaryActionInfo.m_IsGameEnd = nil
----        CMercenaryRoomCreate.LoadYbEduActNpcOrObj(Player, GameID)
----        
----        -- 重新挑战本关，立刻给奖励
----        CMercenaryRoomCreate.FinishGameGiveAward(Player,true, false)
----      else
--
----      end
--      
--      -- 记录下来在Trap里判断
--      Scene.m_MercenaryActionInfo.IsCurrFloor = IsCurrFloor
--      if not Player:CppIsLive() then
--        Player:Reborn(0.1)
--      end
--      g_MercenaryEduAction:CreateFbTrap(Player.m_Scene)
--      CMercenaryRoomCreate.SetGameState(Scene, CMercenaryRoomCreate.GameState.PrepareNextGame)
--      local LifeNum = Scene.m_MercenaryActionInfo.m_LifeNum
--      local FloorNum = Scene.m_MercenaryActionInfo.m_FloorNum
--      local XS = (1+(0.05*(FloorNum-1)))*100
--      Gas2Gac:RetUpdateYbEduActInfo(Player.m_Conn,LifeNum,FloorNum,XS)
--    else
--      ExitYbEducateAction(Conn)
--    end
--  end
--end
