CChallengeCompass = class()

function CChallengeCompass:ChallengeFlagOutDis()
	self:CancelCompassState()
	Gac2Gas:JudgeDistance(g_Conn)
end

function CChallengeCompass:OutDistance()
	return "chaqijiantou/create1"
end

local OutDisFun = {
	[1] = CChallengeCompass.ChallengeFlagOutDis,
	[2] = CChallengeCompass.OutDistance
}

function CChallengeCompass:GetArrowDistStr(Dis)
	local strState = "chaqijiantou/create1"
	if Dis <= 5 then
		strState = "chaqijiantou/create5"
	elseif Dis <= 11 then
		strState = "chaqijiantou/create4"
	elseif Dis <= 17 then
		strState = "chaqijiantou/create3"
	elseif Dis <= 23 then
		strState = "chaqijiantou/create2"
	elseif Dis <= 30 then
		strState = "chaqijiantou/create1"
	else
		strState = OutDisFun[self.m_Type](self)
	end
	return strState
end

function CChallengeCompass:CancelCompassState()
	if self.m_CancelTick then
		UnRegisterTick(self.m_CancelTick)
		self.m_CancelTick = nil
	end
	self.m_CurrScene = nil
	self.m_ObjPos = nil
	self.m_DisPos = nil
	self.m_DistanceStr = nil
--	print("CTargetDirectionFX_CppCancelTargetDirFX()")
	CTargetDirectionFX_CppCancelTargetDirFX()
end

function CChallengeCompass:GetArrowDist()
	return g_GetDistanceByPos(g_MainPlayer, self.m_DisPos)
end

--创建小箭头特效
function CChallengeCompass:SmallArrowHeadDir()
	if IsCppBound(g_MainPlayer) and self.m_CurrScene == g_CoreScene then
		--这个是离目标的距离
		local DistanceNum = self:GetArrowDist()
		local strState = self:GetArrowDistStr(DistanceNum)
		if not strState then
			return
		end
		if (self.m_DistanceStr == nil) or self.m_DistanceStr ~= strState then
			self.m_DistanceStr = strState
--			print(strState,self.m_ObjPos.x,self.m_ObjPos.y)
			CTargetDirectionFX_CppPlayTargetDirFX("fx/setting/other/other/"..strState..".efx", strState, self.m_ObjPos)
		end
	end
end

function CChallengeCompass:SetCompassHeadDir(Type,PosX,PosY)
	if not OutDisFun[Type] then
		return
	end
	local ObjPos = CFPos:new()
	ObjPos.x = PosX
	ObjPos.y = PosY
	local DisPos = CPos:new()
	DisPos.x = PosX/EUnits.eGridSpanForObj
	DisPos.y = PosY/EUnits.eGridSpanForObj
	self.m_CurrScene = g_CoreScene
	self.m_ObjPos = ObjPos
	self.m_DisPos = DisPos
	self.m_Type = Type
	self:SmallArrowHeadDir()
end

function CChallengeCompass:SetTickCompassHeadDir(Type,PosX,PosY,Time)
	if not self.m_CurrScene then
		local function TickFun()
			self:CancelCompassState()
		end
		self.m_CancelTick = RegisterTick("CancelCompassTick", TickFun, Time)
		self:SetCompassHeadDir(Type,PosX,PosY)
	end
end