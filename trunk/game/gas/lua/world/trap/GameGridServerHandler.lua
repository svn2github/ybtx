gas_require "world/trap/trap_event/TrapRegisterEvent"
cfg_load "scene/Trap_Common"

CGameGridServerHandler = class()
function CGameGridServerHandler:Init(time, Trap)
	self.m_OnStepInByCharacter = CDelegate:new()
	self.m_OnStepOutByCharacter = CDelegate:new()
--	self.m_OnTrapCharIDTbl = {} --目前陷阱上的玩家和Npc的ID
	self.m_TriggerTime = time	--如果是地刺类陷阱 time=间隔时间  如果不是time = nil
	self.m_Exist = true
	self.m_Owner = Trap
	if time ~= nil then
		self.m_StateTick = RegClassTick( "GridServerStateTick", self.OnStateTick, time, self )
	end
end

function CGameGridServerHandler:OnStateTick()
	local EntityId = self.m_Owner:GetEntityID()
	local Name = self.m_Owner.m_Properties:GetCharName()
	local EffectName = Trap_Common(Name, "ObjEffectName")
	self.m_Exist = not self.m_Exist
	if self.m_Exist then
		if EffectName ~= nil and EffectName ~= "" then
			Gas2Gac:NotifyIntObjEffect(self.m_Owner:GetIS(0), EntityId, EffectName)
		end
		self:OnStepAutoTrip()
	else
		self:OutStepAutoTrip()
	end
end
local tblTriggerTrap = {}

function CGameGridServerHandler:OnStepAutoTrip()
--	self.m_Owner:GetEntityID()
	local name = self.m_Owner.m_Properties:GetCharName()
	if tblTriggerTrap[name] == nil then
		tblTriggerTrap[name] = {}
	end
	local OnTrapCharIDTbl = self.m_Owner.m_OnTrapCharIDTbl
	for i, v in pairs(OnTrapCharIDTbl) do
		local Char = CCharacterDictator_GetCharacterByID(OnTrapCharIDTbl[i]) or CIntObjServer_GetIntObjServerByID(OnTrapCharIDTbl[i])
		if Char == nil then 
			print ("Char为nil")
			return 
		end
		if table.maxn(tblTriggerTrap[name]) == 0 then
			self:TriggerOnTrap(Char)
		end
		tblTriggerTrap[name][self.m_Owner:GetEntityID()] = self.m_Owner:GetEntityID()
	end
end

function CGameGridServerHandler:OutStepAutoTrip()
	local name = self.m_Owner.m_Properties:GetCharName()
	if tblTriggerTrap[name] == nil then
		return 
	end
	local OnTrapCharIDTbl = self.m_Owner.m_OnTrapCharIDTbl
	for i, v in pairs(OnTrapCharIDTbl) do
		local Char = CCharacterDictator_GetCharacterByID(OnTrapCharIDTbl[i]) or CIntObjServer_GetIntObjServerByID(OnTrapCharIDTbl[i])
		if Char == nil then 
			print ("Char为nil")
			return 
		end
		tblTriggerTrap[name][self.m_Owner:GetEntityID()] = nil 
		if table.maxn(tblTriggerTrap[name]) == 0 then
			self:TriggerOutTrap(Char)
		end
	end
end

function CGameGridServerHandler:ClearTriggertblData( )
	local name = self.m_Owner.m_Properties:GetCharName()
	if tblTriggerTrap[name] == nil then
		return 
	end
	tblTriggerTrap[name][self.m_Owner:GetEntityID()] = nil
end

function CGameGridServerHandler:IsEmpty()
	if self.m_OnStepInByCharacter:IsEmpty() and
		self.m_OnStepOutByCharacter:IsEmpty() then
		return true
	end
	return false
end

function CGameGridServerHandler:AddCallback(callback)
	self.m_OnStepInByCharacter:Add(callback)
	self.m_OnStepOutByCharacter:Add(callback)
end

function CGameGridServerHandler:RemoveCallback()
	self.m_OnStepInByCharacter:RemoveAll()
	self.m_OnStepOutByCharacter:RemoveAll()
end
--进入trap
function CGameGridServerHandler:OnStepInByCharacter(pCharacter)
	if self.m_Exist == true then
		self:TriggerOnTrap(pCharacter)
	end
end
--离开trap
function CGameGridServerHandler:OnStepOutByCharacter(pCharacter)
	self:TriggerOutTrap(pCharacter)
end

function CGameGridServerHandler:TriggerOnTrap(pCharacter)
	if pCharacter ~= nil then
		local objtype = pCharacter.m_Properties:GetType()
		if objtype == ECharacterType.Player or objtype == ECharacterType.Npc then
			if not pCharacter:CppIsLive() then
				return 
			end
		end
		if self.m_OnStepInByCharacter.m_List then
			for _,v in pairs(self.m_OnStepInByCharacter.m_List) do
				v[1].OnStepInTrap(self, pCharacter, v[2])
			end
		end
	end
end

function CGameGridServerHandler:TriggerOutTrap(pCharacter)
	if pCharacter ~= nil then
		local objtype = pCharacter.m_Properties:GetType()
		if objtype == ECharacterType.Player or objtype == ECharacterType.Npc then
			if not pCharacter:CppIsLive() then
				return 
			end
		end
		if self.m_OnStepOutByCharacter.m_List then
			for _,v in pairs(self.m_OnStepOutByCharacter.m_List) do
				v[1].OnStepOutTrap(self, pCharacter, v[2])
			end
		end
	end
end