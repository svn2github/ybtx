gac_gas_require "relation/army_corps/ArmyCorpsMgrInc"

function CArmyCorpsMgr:Ctor()
	self:Init()
end

function CArmyCorpsMgr:Init()
	self.m_nRegistLevel = 40			--注册佣兵团需要等级
	self.m_nRegistMoney = 200000	--佣兵团创建需要资金
	self.m_uAssistantColonelCountLimit = 8 --佣兵团副团长数量限制
	self.m_uInitTeamCountLimit = 8	--佣兵团小队初始数量限制
	
	self.m_tblPosInfo =
		{
			["团长"]		= 1,
			["副团长"]	= 2,
			["团员"]		=	3,
		}
end

function CArmyCorpsMgr:GetRegistLevel()
	return self.m_nRegistLevel 
end

function CArmyCorpsMgr:GetRegistMoney()
	return self.m_nRegistMoney 
end

function CArmyCorpsMgr:GetAssistantColonelCountLimit()
	return self.m_uAssistantColonelCountLimit
end

function CArmyCorpsMgr:GetTongCountLimit(uLevel)
	return self.m_uInitTeamCountLimit
end

function CArmyCorpsMgr:GetPosIndexByName(sPosName)
	return self.m_tblPosInfo[sPosName]
end

function CArmyCorpsMgr:JudgePurview(pos_index,purview_name)
	if purview_name == "UpdatePurpose" or purview_name == "InviteJoin" or purview_name == "KickOut" then
		return pos_index == 1 or pos_index == 2
	end
	if purview_name == "ChangePosition" or purview_name =="UpdateArmyCorpsName" then
		return pos_index == 1
	end
	return false
end


g_ArmyCorpsMgr = g_ArmyCorpsMgr or CArmyCorpsMgr:new()
----------------------
