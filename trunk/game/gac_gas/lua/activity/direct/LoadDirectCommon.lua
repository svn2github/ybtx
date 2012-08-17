gac_gas_require "activity/direct/LoadDirectCommonInc"
cfg_load "player/DirectInfo_Common"

function CDirectMgr:Ctor()
	self.m_tblActionTypeTrans = {}
	self.m_DirectInfoTbl = self:LoadDirectInfo_Common()
	self.m_DirectTypeInfo = self:LoadDirectTypeInfo()
	AddCheckLeakFilterObj(self.m_DirectInfoTbl)
	AddCheckLeakFilterObj(self.m_DirectTypeInfo)
end

function CDirectMgr:LoadDirectInfo_Common()
	local DirectInitTbl = {}
	local Keys = DirectInfo_Common:GetKeys()
	for _, ActionType in pairs(Keys) do
		local DirectList	= DirectInfo_Common(ActionType, "DirectList")
		DirectList			= loadstring("return {" .. (DirectList or "") .. "}")()
		local BigType		= DirectInfo_Common(ActionType, "BigType")
		local AwardItem		= DirectInfo_Common(ActionType, "AwardItem")
		AwardItem			= loadstring("return {" .. (AwardItem or "") .. "}")()
		local Order			= DirectInfo_Common(ActionType, "Order")
		self.m_tblActionTypeTrans[ActionType] = Order
		if not DirectInitTbl[BigType] then
			DirectInitTbl[BigType] = {}
		end
		DirectInitTbl[BigType][Order] = {
			["BigType"]		= BigType,
			["ActionType"]	= ActionType,
			["DirectList"]	= DirectList,
			["AwardItem"]	= AwardItem
			}
	end
	return DirectInitTbl
end

function CDirectMgr:LoadDirectTypeInfo()

	local DirectTypeTbl = {}
	local Keys = DirectInfo_Common:GetKeys()
	for _, ActionType in pairs(Keys) do
		local DirectList = DirectInfo_Common(ActionType, "DirectList")
		DirectList = loadstring("return {" .. (DirectList or "") .. "}")()
		local BigType = DirectInfo_Common(ActionType, "BigType")
		for i, v in pairs(DirectList) do
			DirectTypeTbl[v] = {
				["BigType"] = BigType,
				["ActionType"] = ActionType,
			}
		end
	end
	return DirectTypeTbl
end

function CDirectMgr:GetDirectType(DirectName)
	if not self.m_DirectTypeInfo[DirectName] then
		return
	end
	return self.m_DirectTypeInfo[DirectName]["BigType"], self.m_DirectTypeInfo[DirectName]["ActionType"]
end

g_DirectMgr = g_DirectMgr or CDirectMgr:new()