cfg_load "fb_game/DrinkShoot_Common"
CShootProgressWnd = class(SQRDialog)

local Time = 50
local GridHeight = 50
local FrameTbl = {
	["start01"] = 85,
	["disturbed01"] = 70,
	["disturbed02"] = 130,
	["disturbed03"] = 100,
	["bite01"] = 40,
	["succeed01"] = 75,
	["succeed02"] = 75,
}

function CShootProgressWnd:Ctor()
	self:CreateFromRes("ShootProgressWnd",g_GameMain)
	self.m_Time = self:GetDlgChild( "Time" )
	self.m_Progress = self:GetDlgChild( "Progress" )
	self.m_Progress:SetProgressMode(3)
	self.m_Progress:SetRange(1000)
	self.m_Tick = nil
	self.m_DrinkLevel = 1
	self.m_AccumulatePower = nil
	self.m_Dir = CDir:new()
	
	local tbl = GetCfgTransformValue(true, "DrinkShoot_Common", "喝酒射箭", "Skills")
	self.m_DrinkSkill = tbl[1]
	self.m_ShootSkill = {tbl[2], tbl[3], tbl[4]}
	self.m_AngleRange = GetCfgTransformValue(true, "DrinkShoot_Common", "喝酒射箭", "AngleRange")
	self.m_AngleSpeed = GetCfgTransformValue(true, "DrinkShoot_Common", "喝酒射箭", "AngleSpeed")
	self.m_Award = GetCfgTransformValue(true, "DrinkShoot_Common", "喝酒射箭", "Award")
	self.m_Frequency = DrinkShoot_Common("喝酒射箭", "Frequency")
	self.m_PowerSpeed = DrinkShoot_Common("喝酒射箭", "PowerSpeed")
	
	self:UpdateHeight()
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CShootProgressWnd:UpdateHeight()
	local n = self.m_DrinkLevel
	local rt = CFRect:new()
	local rt_1 = CFRect:new()
	local rt_2 = CFRect:new()
	local middleLine = 0
	self:GetLogicRect(rt)
	self.m_Progress:GetLogicRect(rt_1)
	self.m_Time:GetLogicRect(rt_2)
	middleLine = (rt_1.bottom + rt_1.top) / 2
	
	rt.top = middleLine - GridHeight * n - (rt_2.bottom - rt_2.top)
	rt.bottom = middleLine + GridHeight * n
	self:SetLogicRect(rt)
	
	self.m_Progress:GetLogicRect(rt_1) 
	rt_1.bottom = rt.bottom
	self.m_Progress:SetLogicRect(rt_1)
end

--加箭头特效
local function AddRender(RenderDir)
	local pos = CFPos:new()
	g_MainPlayer:GetPixelPos(pos)
	g_MainPlayer.m_DrinkShootArrowDirFx = g_CoreScene:CreateObjectPerformer(pos, nil)
	local RenderObj = g_MainPlayer.m_DrinkShootArrowDirFx:GetRenderObject()
	local Dir = CDir:new()
	Dir.uDir = RenderDir
	RenderObj:SetDirection(Dir)
	RenderObj:AddEffect( "fx/setting/other/other/chaqijiantou/create1.efx", "chaqijiantou/create1", -1 , NULL)
end

--设置箭头方向
local function SetDir(Dir)
	local pos = CFPos:new()
	local RenderObj = g_MainPlayer.m_DrinkShootArrowDirFx:GetRenderObject()
	g_MainPlayer:GetPixelPos(pos)
	g_MainPlayer.m_DrinkShootArrowDirFx:SetPixelPos(pos)
	RenderObj:SetDirection(Dir)
end

local function GetAngle(TargetPos)
	local Pos = CPos:new()
	g_MainPlayer:GetGridPos(Pos)

	local RangeX = TargetPos.x-Pos.x
	local RangeY = TargetPos.y-Pos.y
	local RangeXBool = RangeX >= -0.00001 and RangeX <= 0.00001
	local RangeYBool = RangeY >= -0.00001 and RangeY <= 0.00001
	
	local iAngle = nil
	if RangeXBool and RangeYBool then
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
		return GetAngle(mousePos)
	end
end

local function GetTargetPos()
	if not (g_MainPlayer and g_MainPlayer.m_DrinkShootArrowDirFx) then
		return
	end
	
	local dir = CDir:new()
	local RenderObj = g_MainPlayer.m_DrinkShootArrowDirFx:GetRenderObject()
	RenderObj:GetDirection(dir)
	local angle = dir.uDir * (360/256)
	local vecPos = CVector3f:new()
	g_CoreScene:GetRenderScene():GetSelectedPos(vecPos)
	local mousePos = CFPos:new()
	mousePos.x = vecPos.x/64
	mousePos.y = vecPos.z/64
	local dis = g_GetDistanceByPos(g_MainPlayer, mousePos)
	local pos = CFPos:new()
	local rootPos = CFPos:new()
	g_MainPlayer:GetPixelPos(rootPos)
	pos.x = dis * math.sin(math.rad(angle)) * 64 + rootPos.x
	pos.y = dis * math.cos(math.rad(angle)) * 64 + rootPos.y
	return pos
end

function CShootProgressWnd:UpdateTurnArrow(PosX, PosY)
	if self.m_TurnArrowFx then
		g_CoreScene:DestroyObjectPerformer( self.m_TurnArrowFx )
		self.m_TurnArrowFx = nil
	end
	if PosX~=0 and PosY~=0 then
		local pos = CFPos:new()
		pos.x, pos.y = PosX, PosY
		self.m_TurnArrowFx = g_CoreScene:CreateObjectPerformer(pos, nil)
		self.m_TurnArrowFx:SetHeight(100)
		local RenderObj = self.m_TurnArrowFx:GetRenderObject()
		RenderObj:AddEffect( "fx/setting/other/other/tiaoxin/explode.efx", "tiaoxin/explode", -1 , NULL)
	end
end

function CShootProgressWnd:DoDrink()
	if self.m_DrinkLevel < 3 then
		self.m_DrinkLevel = self.m_DrinkLevel + 1
		self:UpdateHeight()
	end
end

function CShootProgressWnd:BeginAccumulatePower()
	self.m_AccumulatePower = 0
end

function CShootProgressWnd:BeginDrinkShoot(Conn)
	if self.m_Tick then
		UnRegisterTick( self.m_Tick )
		self.m_Tick = nil
	end
	self:ShowWnd(true)
	math.randomseed(os.time())
	
	--计时变量
	local timeCount = 0
	--数据变量
	local angle = 0
	local newAngle = 0
	local perAngle = 0
	local turnDir = 1
	
	local PlayerPos = CFPos:new()
	g_MainPlayer:GetPixelPos(PlayerPos)
	Gac2Gas:DrinkShootDoAction(g_Conn, "bite01")
	
	local function Update()
		if not g_MainPlayer.m_DrinkShootArrowDirFx then
			if timeCount >= FrameTbl["bite01"]*33 then
				Gac2Gas:DrinkShootDoAction(g_Conn, "stand02")
				GroundSelector:CancelSelectGround()
				--GroundSelector:SetSkill( self.m_ShootSkill[1], 1, CFPos:new(), ESelectGroundType.eSGT_MouseSelector)
				GroundSelector:BeginSelectMouseSelector()
				--g_GameMain.m_TempSkill:AddTempSkill(self.m_DrinkSkill, 1, 0, 1)
				AddRender(GetBaseAngle())
			end
			
		else
			--箭头方向的控制和更新
			if not self.m_AccumulatePower then
				if timeCount%self.m_Frequency == 0 then
					local range = self.m_AngleRange[self.m_DrinkLevel]
					newAngle = math.random(-range, range)
					if angle < newAngle then
						turnDir = 1
					else
						turnDir = -1
					end
				end
				perAngle = self.m_AngleSpeed[self.m_DrinkLevel] * Time/1000
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
				local RenderObj = g_MainPlayer.m_DrinkShootArrowDirFx:GetRenderObject()
				self.m_Dir.uDir = angle * 256/360 + baseAngle
				if self.m_Dir.uDir > 256 then
					self.m_Dir.uDir = self.m_Dir.uDir - 256
				end
				if self.m_Dir.uDir < 0 then
					self.m_Dir.uDir = self.m_Dir.uDir + 256
				end
				SetDir(self.m_Dir)
			elseif self.m_AccumulatePower < 1000 then
				self.m_AccumulatePower = self.m_AccumulatePower + Time*self.m_PowerSpeed
				self.m_Progress:SetPos(self.m_AccumulatePower)
			else
				self:DoShoot()
			end
		end
		
		self.m_Time:SetWndText( math.ceil(15 - timeCount/1000) )
		if timeCount > 15000 then
			self:EndShoot()
		else
			timeCount = timeCount + Time
		end
	end
	
	self.m_Tick = RegisterTick("DrinkShootTick", Update, Time)
end

function CShootProgressWnd:DoShoot()
	local perpos = 500 / self.m_DrinkLevel
	local power = self.m_AccumulatePower
	if power > 500 then
		power = 1000 - power
	end
	
	for i=1, self.m_DrinkLevel do
		if power >= perpos*(i-1) and power < perpos*i then
			local targetPos = GetTargetPos()
			g_MainPlayer:SetTargetPos(targetPos)
			g_MainPlayer:TurnAndSyncDir(self.m_Dir.uDir)
			Gac2Gas:DrinkShootDoSkill(g_Conn, self.m_ShootSkill[i], targetPos.x, targetPos.y)
			--GroundSelector:SetSkill( self.m_ShootSkill[i], 1, targetPos, ESelectGroundType.eSGT_MouseSelector)
		end
	end
	self:EndShoot()
end

function CShootProgressWnd:EndShoot()
	if self.m_Tick then
		UnRegisterTick( self.m_Tick )
		self.m_Tick = nil
	end
	self.m_AccumulatePower = nil
	self.m_DrinkLevel = 1
	self.m_Progress:SetPos(0)
	self:UpdateHeight()
	self:ShowWnd(false)
	GroundSelector:CancelSelectGround()
	
	if not IsCppBound(g_MainPlayer) then
		return
	end
	
	g_CoreScene:DestroyObjectPerformer( g_MainPlayer.m_DrinkShootArrowDirFx )
	g_MainPlayer.m_DrinkShootArrowDirFx = nil
	--g_GameMain.m_TempSkill:DeleteTempSkill(1)
	
	local function fun2()
		Gac2Gas:DrinkShootDoAction(g_Conn, "stand01")
		Gac2Gas:DrinkShootNextPlayer(g_Conn)
	end
	local function fun1()
		Gac2Gas:DrinkShootDoAction(g_Conn, "start01")
		RegisterOnceTick(g_GameMain, "EndDrinkShootTick2", fun2, FrameTbl["start01"]*33)
	end
	RegisterOnceTick(g_GameMain, "EndDrinkShootTick1", fun1, 3000)
end

--[[
高攻枪
标准枪
霰弹枪
减速枪
冰冻枪

--]]