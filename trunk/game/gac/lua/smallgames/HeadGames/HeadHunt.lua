cfg_load "smallgame/HeadHuntData_Common"

local time = 20
local dTbl = {}

local function CheckHeadHuntDataCfg()
	for _, i in pairs(HeadHuntData_Common:GetKeys()) do
		local data = HeadHuntData_Common(i)
		dTbl[data("Index")] = {}
		dTbl[data("Index")].att = data("Attention")
		dTbl[data("Index")].pos = data("Position")
		dTbl[data("Index")].rag = data("Range")
		dTbl[data("Index")].spd = data("Speed")
		if data("Index") == 1 then
			dTbl.freq = tonumber(data("Frequency"))
		end
	end
end

--加箭头特效
local function HeadHuntAddRender(uRenderDir)
	local pos = CFPos:new()
	g_MainPlayer:GetPixelPos(pos)
	g_MainPlayer.m_HeadHuntArrowDirFx = g_CoreScene:CreateObjectPerformer(pos, nil)
	local RenderObj = g_MainPlayer.m_HeadHuntArrowDirFx:GetRenderObject()
	local Dir = CDir:new()
	Dir.uDir = uRenderDir
	RenderObj:SetDirection(Dir)
	RenderObj:AddEffect( "fx/setting/other/other/fanyao7/create.efx", "fanyao7/create", -1, nil )
end

--设置箭头方向
local function HeadHuntSetDir(Dir)
	local pos = CFPos:new()
	local RenderObj = g_MainPlayer.m_HeadHuntArrowDirFx:GetRenderObject()
	g_MainPlayer:GetPixelPos(pos)
	g_MainPlayer.m_HeadHuntArrowDirFx:SetPixelPos(pos)
	RenderObj:SetDirection(Dir)
end

local function HeadHuntGetAngle(TargetPos)
	local Pos = CPos:new()
	g_MainPlayer:GetGridPos(Pos)
	
	local RangeX = TargetPos.x-Pos.x
	local RangeY = TargetPos.y-Pos.y
	local RangeXBool = RangeX >= -0.00001 and RangeX <= 0.00001
	local RangeYBool = RangeY >= -0.00001 and RangeY <= 0.00001
	
	local iAngle = nil
	if RangeXBool and RangeYBool then
		--print("离 ",NpcName," 太近了!")
		return false
	elseif RangeXBool then
		if RangeY <= -0.00001 then
			iAngle = 180
		else
			iAngle = 0
		end
	elseif RangeYBool then
		if RangeX <= -0.00001 then
			iAngle = 270
		else
			iAngle = 90
		end
	else
		iAngle = math.atan(RangeX / RangeY)*(180/3.1415926)
		if RangeX <= -0.00001 then
			if iAngle <= -0.00001 then
				iAngle = 360 + iAngle
			else
				iAngle = 180 + iAngle
			end
		else
			if iAngle <= -0.00001 then
				iAngle = 180 + iAngle
			end
		end
	end
	iAngle = 256/360 * iAngle
	
	return iAngle
end

--获得鼠标方向
local function GetBaseAngle()
	if g_CoreScene then
		local vecPos = CVector3f:new()
		local mousePos = CFPos:new()
		g_CoreScene:GetRenderScene():GetSelectedPos(vecPos)
		mousePos.x = vecPos.x/64
		mousePos.y = vecPos.z/64
		return HeadHuntGetAngle(mousePos)
	end
end	

local function HeadHuntTick()
	--计时变量
	local timeCount = 5000
	local signalTime = 1000
	--数据变量
	local pos = 0
	local angle = 0
	local newAngle = 0
	local perAngle = dTbl[1].spd * (time / 1000)
	local range = dTbl[1].rag
	local turnDir = 1
	local frequency = dTbl.freq
	--信号变量
	local attention = 0
	local newAttention = 0
	local targetAttention = 0
	local perAttention = 0
	local quality = 0
	local newQuality = 0
	local targetQuality = 0
	local perQuality = 0
	
	g_GameMain.m_HeadProgressWnd:BeginHeadProgress()
	math.randomseed(os.time())
	
	local function Update(Tick)
		if not (IsCppBound(g_Conn) and IsCppBound(g_MainPlayer)) then
			EndHeadHunt()
			return
		end
		
		--脑电波信号相关处理
		if signalTime >= 1000 then
			newAttention = g_App:GetTGInst():GetThinkgearData(TG_DATA_ATTENTION)
			newQuality = g_App:GetTGInst():GetThinkgearData(TG_DATA_POOR_SIGNAL)
			--newAttention = math.random(10,40)
			--newQuality = math.random(0,200)
			if newAttention >= 1 then
				targetAttention = newAttention
				targetQuality = newQuality
				g_GameMain.m_HeadProgressWnd.m_GoodSignal:ShowWnd(true)
				g_GameMain.m_HeadProgressWnd.m_BadSignal:ShowWnd(false)
			else
				range = dTbl[1].rag
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
			for i=1, table.getn(dTbl) do
				if attention >= dTbl[i].att and attention <= dTbl[i+1].att then
					local k = (attention - dTbl[i].att) / (dTbl[i+1].att - dTbl[i].att)
					pos = dTbl[i].pos + (dTbl[i+1].pos - dTbl[i].pos) * k
					range = dTbl[i].rag - (dTbl[i].rag - dTbl[i+1].rag) * k
					perAngle = ( dTbl[i].spd - (dTbl[i].spd - dTbl[i+1].spd) * k ) * (time / 1000)
				end
			end
			g_GameMain.m_HeadProgressWnd:SetPerPos(pos, 100 - quality/2)
		end
		
		--箭头方向的控制和更新
		if timeCount/1000 >= frequency then
			newAngle = math.random(-range, range)
			if angle < newAngle then
				turnDir = 1
			else
				turnDir = -1
			end
			timeCount = 0
		end
		if angle ~= newAngle then
			if (turnDir>0 and (angle+perAngle)<newAngle) or (turnDir<0 and (angle-perAngle)>newAngle) then
				if turnDir > 0 then
					angle = angle + perAngle
				else
					angle = angle - perAngle
				end
			else
				angle = newAngle
			end
		end
		local baseAngle = GetBaseAngle()
		local dir = CDir:new()
		local RenderObj = g_MainPlayer.m_HeadHuntArrowDirFx:GetRenderObject()
		dir.uDir = angle * 256/360 + baseAngle
		if dir.uDir > 256 then
			dir.uDir = dir.uDir - 256
		end
		if dir.uDir < 0 then
			dir.uDir = dir.uDir + 256
		end
		HeadHuntSetDir(dir)
		timeCount = timeCount + time
		signalTime = signalTime + time
	end
	
	g_GameMain.HeadHuntTick = RegisterTick("HeadHuntTick", Update, time)
end

--获得目标位置
function GetHeadHuntTargetPos()
	if not (g_MainPlayer and g_MainPlayer.m_HeadHuntArrowDirFx) then
		return
	end
	
	local dir = CDir:new()
	local RenderObj = g_MainPlayer.m_HeadHuntArrowDirFx:GetRenderObject()
	RenderObj:GetDirection(dir)
	local angle = dir.uDir * (360/256)
	local vecPos = CVector3f:new()
	g_CoreScene:GetRenderScene():GetSelectedPos(vecPos)
	local mousePos = CFPos:new()
	mousePos.x = vecPos.x/64
	mousePos.y = vecPos.z/64
	local dis = g_GetDistanceByPos(g_MainPlayer, mousePos)
	local pos = CVector3f:new()
	local rootPos = CFPos:new()
	g_MainPlayer:GetPixelPos(rootPos)
	pos.x = dis * math.sin(math.rad(angle)) * 64 + rootPos.x
	pos.z = dis * math.cos(math.rad(angle)) * 64 + rootPos.y
	return pos
end

function BeginHeadHunt()
	if table.getn(dTbl) == 0 then
		CheckHeadHuntDataCfg()
	end
	local baseAngle = GetBaseAngle()
	HeadHuntAddRender(baseAngle)
	HeadHuntTick()
	g_GameMain.m_HeadProgressWnd:ShowWnd(true)
	g_GameMain.m_HeadProgressWnd.m_GoodSignal:ShowWnd(true)
	g_GameMain.m_HeadProgressWnd.m_BadSignal:ShowWnd(false)
	g_GameMain.m_HeadProgressWnd:BeginHeadProgress()
	local rect = CFRect:new()
	g_GameMain.m_HeadProgressWnd:GetLogicRect(rect)
	if rect.left ~= 1200 then
		local width = rect.right - rect.left
		rect.left = 1200
		rect.right = rect.left + width
		g_GameMain.m_HeadProgressWnd:SetLogicRect(rect)
	end
end

function EndHeadHunt()
	if g_GameMain.HeadHuntTick then
		UnRegisterTick(g_GameMain.HeadHuntTick)
		g_GameMain.HeadHuntTick = nil
	end
	if g_MainPlayer and g_MainPlayer.m_HeadHuntArrowDirFx then
		g_CoreScene:DestroyObjectPerformer( g_MainPlayer.m_HeadHuntArrowDirFx )
		g_MainPlayer.m_HeadHuntArrowDirFx = nil
	end
	g_GameMain.m_HeadProgressWnd:ShowWnd(false)
	g_GameMain.m_HeadProgressWnd:StopHeadProgress()
end