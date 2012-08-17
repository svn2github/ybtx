gac_gas_require "relation/tong/TongPurviewMgrInc"
cfg_load "tong/TongPurview_Common"

function CTongPurviewInfoMgr:Ctor()
	self.m_tblItemInfo = TongPurview_Common
end

function CTongPurviewInfoMgr:GetPosNameTableOrderByLevel()
	local tbl =	{	{GetStaticTextClient(10021)}, --"团员"
								{GetStaticTextClient(10020)}, --"队长"
								{GetStaticTextClient(10019)}, --"营长"
								{GetStaticTextClient(10018)}, --"副团长"
								{GetStaticTextClient(10017)}, --"团长"	
							}
	return tbl
end

function CTongPurviewInfoMgr:GetPosName(nPos)
	local sPosTbl = self:GetPosNameTableOrderByLevel()
	local tbl = sPosTbl[nPos]
	return tbl and tbl[1] or nil
end

function CTongPurviewInfoMgr:GetInviteMemberValue(posLevel)
	local tblInfo = self.m_tblItemInfo("InviteMember")
	local tbl = {tblInfo("Level1"), tblInfo("Level2"), tblInfo("Level3"), tblInfo("Level4"), tblInfo("Level5")}
	return tbl[posLevel] or 0
end

function CTongPurviewInfoMgr:GetUpdatePurposeValue(posLevel)
	local tblInfo = self.m_tblItemInfo("UpdatePurpose")
	local tbl = {tblInfo("Level1"), tblInfo("Level2"), tblInfo("Level3"), tblInfo("Level4"), tblInfo("Level5")}
	return tbl[posLevel] or 0
end

function CTongPurviewInfoMgr:JudgePurview(posLevel,purview_name)
	local tblInfo = self.m_tblItemInfo(purview_name)
	local tbl = {tblInfo("Level1"), tblInfo("Level2"), tblInfo("Level3"), tblInfo("Level4"), tblInfo("Level5")}
	local nFlag =  tbl[posLevel]
	return 1 == nFlag
end

g_TongPurviewMgr = g_TongPurviewMgr or CTongPurviewInfoMgr:new()
