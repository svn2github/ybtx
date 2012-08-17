gac_require "fight/skill_loading/SkillLoadingInc"
cfg_load "skill/SkillPart_Common"
lan_load "skill/Lan_SkillPart_Common"
cfg_load "skill/NonFightSkill_Common"
lan_load "skill/Lan_NonFightSkill_Common"
cfg_load "skill/SkillName_Client"
lan_load "skill/Lan_SkillName_Client"

local mRange = 100
function CBaseSkillLoading:InitSkillLoading( ProgressLoad ) --参数: Process
	self.m_ProgressLoad = ProgressLoad
	self.m_ProgressLoad:SetProgressMode(0)
	self.m_ProgressLoad:SetRange(mRange)
	self.m_LoadTime = nil
	self.Pos = 0
	self.m_Time = 0
	self.m_RunedTime = 0
end

function CBaseSkillLoading:SetPos( nNum )
	self.m_ProgressLoad:SetPos(nNum)
end

function CBaseSkillLoading:GetPos()
	return self.m_ProgressLoad:GetPos()
end

function CBaseSkillLoading:UnRegisterTick()
	if self.m_LoadTime then
		if self.m_FolObj and IsCppBound(self.m_FolObj) then
			UnRegisterDistortedTickByCoreObj( self.m_FolObj, self.m_LoadTime )
		else 
			UnRegisterDistortedTick(self.m_LoadTime)
		end
		self.m_LoadTime = nil
	end
end

--吟唱中断
function CBaseSkillLoading:OnCastEnd()
	self.Pos = 99
	self:SetPos( 0 )
	self:UnRegisterTick()
	self:ShowWndFalse()
end

--暂停
function CBaseSkillLoading:OnPause()
	self:UnRegisterTick()
end

function CBaseSkillLoading:ShowWndFalse()
	
end

function CBaseSkillLoading:GetSkillDisPlayName(skillname)
	local Lan_skill = Lan_SkillPart_Common(MemH64(skillname))
	local DisplayName = Lan_skill and Lan_skill("DisplayName")
	if DisplayName == nil then
		Lan_skill = Lan_NonFightSkill_Common(MemH64(skillname))
		DisplayName = Lan_skill and Lan_skill("DisplayName")
	end
	if DisplayName == nil then
		Lan_skill = Lan_SkillName_Client(MemH64(skillname))
		DisplayName = Lan_skill and Lan_skill("DisplayName")
	end
	if DisplayName == nil then
		DisplayName = ""
	end
	return DisplayName
end

function CBaseSkillLoading:GetSkillDisIcon(skillname)
	local SmallIcon = nil
	local skill = SkillPart_Common(skillname)
	if skill then
		SmallIcon =	skill("SkillSmallIcon")
		return SmallIcon
	end
	skill = NonFightSkill_Common(skillname)
	if skill then
		SmallIcon = skill("SkillSmallIcon")
		return SmallIcon
	end
	skill = SkillName_Client(skillname)
	if skill then
		SmallIcon = skill("SkillSmallIcon")
		return tonumber(SmallIcon)
	end
	return SmallIcon
end

function CBaseSkillLoading:OnCastBegin( FolObj, skillname, flag ,time ,runedtime) --参数:flag: 1 吟唱, 2 引导, 3, 蓄力吟唱   time:技能的吟唱时间 ,runedtime:运行了的时间
	if time <= 0 then
		return
	end
	self.m_FolObj = FolObj
	self.m_Time = time
	self.m_LeftTime = runedtime
	self:UnRegisterTick()
	self:ShowWnd(true)
	self.EndPos = mRange
	if(flag == 1 or flag ==3)then
		if(runedtime == 0) then  --runedtime为0 表示正常施放,否则从runedtime处开始发放
			self.Pos = 0
		else
			self.Pos = runedtime/time*100
		end
		self:SetPos( self.Pos )
		local Loading = function()
		--db()
			self.Pos = self.Pos + 1
			--print(self.Pos)
			if not self.SetPos then
				LogErr("流程条对象已经删除,Tick竟然还在")
				self:UnRegisterTick()
			end
			self:SetPos( self.Pos )
			self.m_RunedTime = self.Pos/100*self.m_Time
			if(self.Pos >= self.EndPos) then
				self:SetPos( self.EndPos )
				self:UnRegisterTick()
				return
			end
		end
		--print("CastTime :",10*time%1000)
		self.m_LoadTime = RegisterDistortedTickByCoreObj( FolObj, "Loading", Loading , 10*time%1000, self )  --注册时钟
	else
		if(runedtime == 0) then  --runedtime为0 表示正常施放,否则从runedtime处开始发放
			self.Pos = mRange
		elseif(runedtime == time)then
			self:SetPos( 0 )
			self:UnRegisterTick()
			return
		else
			self.Pos = mRange - runedtime/time*100
		end
		self:SetPos( self.Pos )
		local desLoading = function()
			self.Pos = self.Pos - 1
			self:SetPos( self.Pos )
			self.m_RunedTime = (100 - self.Pos)/100*self.m_Time
			if(self.Pos <= 0) then
				self:SetPos( 0 )
				self:UnRegisterTick()
				return
			end
		end
		self.m_LoadTime = RegisterDistortedTickByCoreObj( FolObj,  "desLoading", desLoading, 10*time%1000, self )  --注册时钟
	end
end
