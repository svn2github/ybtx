
local time = 20
local fishObj = {}
local startPos = CFPos:new()
local frameTbl = {
	["start01"] = 85,
	["disturbed01"] = 70,
	["disturbed02"] = 130,
	["disturbed03"] = 100,
	["bite01"] = 40,
	["succeed01"] = 75,
	["succeed02"] = 75,
}

function EndHeadFishing()
	if g_GameMain and g_GameMain.HeadFishingTick then
		UnRegisterTick(g_GameMain.HeadFishingTick)
		g_GameMain.HeadFishingTick = nil
		g_GameMain.m_HeadProgressWnd:StopHeadProgress()
	end
	if g_MainPlayer then 
		if g_MainPlayer.m_HeadFishingFx then
			g_CoreScene:DestroyObjectPerformer( g_MainPlayer.m_HeadFishingFx )
			g_MainPlayer.m_HeadFishingFx = nil
		end
		g_MainPlayer.m_HeadGameName = nil
	end
	if fishObj then
		fishObj = {}
	end
	Gac2Gas:HeadGameChangeAction(g_Conn, "end01")
	Gac2Gas:HeadGameChangeActionBag(g_Conn, "")
end

local function MoveToPosition()
	local angle = GetActionDir(g_MainPlayer, fishObj) * 360/256
	local targetPos = CFPos:new()
	fishObj:GetPixelPos(targetPos)
	targetPos.x = 6 * math.sin(math.rad(angle)) * 64 + targetPos.x
	targetPos.y = 6 * math.cos(math.rad(angle)) * 64 + targetPos.y
	g_MainPlayer:MoveToInAStarPath(targetPos, EFindPathType.eFPT_AStar, EBarrierType.eBT_NoBarrier, 0, 100)
end

local function HeadFishingTickFun()
	--计时变量
	local timeCount = 0
	local signalTime = 1000
	local msgTime = 0
	local biteTime = 0
	local actionTime = 3000
	local actionLastTime = 0
	local lowAttentionTime = 0
	--数据变量
	local pullPower = 0
	local state = "wait"
	local action = ""
	--信号变量
	local attention = 100
	local newAttention = 0
	local targetAttention = 0
	local perAttention = 0
	local quality = 0
	local newQuality = 0
	local targetQuality = 0
	local perQuality = 0
	
	g_MainPlayer:GetPixelPos(startPos)
	
	local function Update(Tick)
		if not (IsCppBound(g_Conn) and IsCppBound(g_MainPlayer)) then
			EndHeadFishing()
			return
		end
		--判断玩家是否移动，是则退出
		local playerPos = CFPos:new()
		g_MainPlayer:GetPixelPos(playerPos)
		if playerPos.x ~= startPos.x or playerPos.y ~= startPos.y then
			EndHeadFishing()
		end

		--脑电波信号相关处理
		if signalTime >= 1000 then
			newAttention = g_App:GetTGInst():GetThinkgearData(TG_DATA_ATTENTION)
			newQuality = g_App:GetTGInst():GetThinkgearData(TG_DATA_POOR_SIGNAL)
			--newAttention = math.random(30,60)
			--newQuality = math.random(0,200)
			if newAttention >= 1 then
				if state == "wait" and timeCount > 10000 then
					targetAttention = newAttention - newAttention * ((timeCount-10000)/10000) * (0.75+math.random(-5,2)/20)
				else
					targetAttention = newAttention
				end
				targetQuality = newQuality
				g_GameMain.m_HeadProgressWnd.m_GoodSignal:ShowWnd(true)
				g_GameMain.m_HeadProgressWnd.m_BadSignal:ShowWnd(false)
			else
				targetAttention = 0
				targetQuality = 0
				g_GameMain.m_HeadProgressWnd.m_GoodSignal:ShowWnd(false)
				g_GameMain.m_HeadProgressWnd.m_BadSignal:ShowWnd(true)
			end
			perAttention = (targetAttention - attention)/1000*time
			perQuality = (targetQuality - quality)/1000*time
			signalTime = 0
		end
		if attention and targetAttention and attention ~= targetAttention then
			attention = attention + perAttention
			quality = quality + perQuality
			g_GameMain.m_HeadProgressWnd:SetPerPos(attention, 100 - quality/2)
		end
		
		--钓鱼玩法流程，动作的播放和控制
		if attention > 0 then
			--算法
			--等待鱼咬钩状态
			if state == "wait" then
					if attention > 40 then
							if actionTime >= 3000 then 
								local num = math.random(1,3)
								action = "disturbed0"..num
								Gac2Gas:HeadGameChangeAction(g_Conn, action)
								actionTime = 0
								actionLastTime = 0
							end
							if msgTime >= 5000 then
								MsgClient(3280)
								msgTime = 0
							end
							msgTime = msgTime + time
							actionTime = actionTime + time
							lowAttentionTime = 0
					else
							if action ~= "stand01" then
								action = "stand01"
								Gac2Gas:HeadGameChangeAction(g_Conn, action)
							end
							if lowAttentionTime >= 4000 then
								if biteTime >= 1000 then
									if math.random(1,10) <= 5 then
										state = "pull"
										action = "bite01"
										Gac2Gas:HeadGameChangeAction(g_Conn, action)
										g_MainPlayer.m_HeadFishingFx = g_CoreScene:CreateObjectPerformer(startPos, nil)
										g_MainPlayer.m_HeadFishingFx:SetHeight(100)
										local RenderObj = g_MainPlayer.m_HeadFishingFx:GetRenderObject()								
										RenderObj:AddEffect( "fx/setting/other/other/tiaoxin/explode.efx", "tiaoxin/explode", -1, NULL )
										MsgClient(3286)
										msgTime = 0
										actionLastTime = 0
										timeCount = 0
									end
									biteTime = 0
								end
								biteTime = biteTime + time
							end
							actionTime = 3000
							msgTime = 0
							lowAttentionTime = lowAttentionTime + time
					end
					if action ~= "stand01" and action ~= "stand02"then
						if actionLastTime >= frameTbl[action]*33 then
							action = "stand01"
							Gac2Gas:HeadGameChangeAction(g_Conn, action)
						else
							actionLastTime = actionLastTime + time
						end
					end
					if timeCount < 20000 then
						timeCount = timeCount + time
					end
					
			--鱼咬钩后拉杆状态
			elseif state == "pull" then
					if action == "bite01" or action == "succeed01" or action == "succeed02" then
						if actionLastTime >= frameTbl[action]*33 then
							if g_MainPlayer.m_HeadFishingFx then
								g_CoreScene:DestroyObjectPerformer( g_MainPlayer.m_HeadFishingFx )
								g_MainPlayer.m_HeadFishingFx = nil
							end
							if action == "bite01" then
								action = "stand02"
								Gac2Gas:HeadGameChangeAction(g_Conn, action)
								actionLastTime = 0
							else
								action = "end01"
								EndHeadFishing()
							end
						else
							actionLastTime = actionLastTime + time
						end
						
					else
						if pullPower < 0 then
							MsgClient(3282)
							--Gac2Gas:HeadGameChangeAction(g_Conn, "lose01")
							EndHeadFishing()
						elseif pullPower >= 0 and pullPower <= 6000 then
							if action ~= "stand02" then
								action = "stand02"
								Gac2Gas:HeadGameChangeAction(g_Conn, action)
							end
						elseif pullPower > 6000 and pullPower <= 16000 then
							if action ~= "pull01" then
								action = "pull01"
								Gac2Gas:HeadGameChangeAction(g_Conn, action)
							end
						elseif pullPower > 16000 and pullPower <= 26000 then
							if action ~= "pull02" then
								action = "pull02"
								Gac2Gas:HeadGameChangeAction(g_Conn, action)
							end
						elseif pullPower > 26000 and pullPower < 40000 then
							if action ~= "pull03" then
								action = "pull03"
								Gac2Gas:HeadGameChangeAction(g_Conn, action)
							end
						else
							if math.random(1,5) ~= 5 then
								MsgClient(3283)
								action = "succeed01"
								Gac2Gas:HeadGameChangeAction(g_Conn, action)
							else
								MsgClient(3284)
								action = "succeed02"
								Gac2Gas:HeadGameChangeAction(g_Conn, action)
							end
							g_MainPlayer.m_HeadFishingFx = g_CoreScene:CreateObjectPerformer(startPos, nil)
							local RenderObj = g_MainPlayer.m_HeadFishingFx:GetRenderObject()								
							RenderObj:AddEffect( "fx/setting/other/other/xiaoyouxi/create.efx", "xiaoyouxi/create", -1, NULL)
						end
						
						if timeCount > 30000 then
							MsgClient(3282)
							EndHeadFishing()
						end
						if msgTime >= 5000 then
							MsgClient(3281)
							msgTime = 0
						end
						pullPower = pullPower + attention
						msgTime = msgTime + time
						timeCount = timeCount + time
					end
			else
			end
		end
		signalTime = signalTime + time
	end
	g_GameMain.HeadFishingTick = RegisterTick("HeadFishingTick", Update, time)
end

local function BeginHeadFishing()
	Gac2Gas:HeadGameChangeActionBag(g_Conn, "diao")
	Gac2Gas:HeadGameChangeAction(g_Conn, "start01")
	MsgClient(3285)
	local function fun()
		Gac2Gas:HeadGameChangeAction(g_Conn, "stand01")
		HeadFishingTickFun()
		g_GameMain.m_HeadProgressWnd:BeginHeadProgress()
		g_GameMain.m_HeadProgressWnd.m_GoodSignal:ShowWnd(true)
		g_GameMain.m_HeadProgressWnd.m_BadSignal:ShowWnd(false)
		local rect = CFRect:new()
		g_GameMain.m_HeadProgressWnd:GetLogicRect(rect)
		if rect.left ~= 1200 then
			local width = rect.right - rect.left
			rect.left = 1200
			rect.right = rect.left + width
			g_GameMain.m_HeadProgressWnd:SetLogicRect(rect)
		end
	end
	RegisterOnceTick(g_GameMain, "BeginHeadFishingTick", fun, frameTbl["start01"]*33)
end

function Gas2Gac:CheckHeadFishing(Conn, GameName, EntityID)
	if not (IsCppBound(Conn) and IsCppBound(g_MainPlayer)) then
		return
	end
	local function IsBeginHeadFishing(wnd, btnIndex)
		if btnIndex == 1 then
			fishObj = CIntObjClient_GetIntObjByID(EntityID)
			local playerPos = CFPos:new()
			g_MainPlayer:GetPixelPos(playerPos)
			--若玩家的位置与上次开始游戏的位置相同，则不用移动，直接开启游戏
			if playerPos.x == startPos.x and playerPos.y == startPos.y then
				BeginHeadFishing()
			else
				g_MainPlayer.m_HeadGameName = "脑电波钓鱼"
				MoveToPosition()
			end
		end
		wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	local msg = MsgBoxMsg(6000, GameName)
	msg = msg.." (%d)"
	local OkStr = GetStaticTextClient(1124)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, msg, 30, IsBeginHeadFishing, {OkStr, "取消"})
end
--玩家移动到指定位置结束后的回调
function BeginHeadGame()
	if not (IsCppBound(g_Conn) and IsCppBound(g_MainPlayer)) then
		return
	end
	if g_MainPlayer.m_HeadGameName == "脑电波钓鱼" then
		--移动后，面对obj
		local dir = GetActionDir(fishObj, g_MainPlayer)
		g_MainPlayer:TurnAndSyncDir(dir)
		g_MainPlayer.m_HeadGameName = nil
		BeginHeadFishing()
	end
end
