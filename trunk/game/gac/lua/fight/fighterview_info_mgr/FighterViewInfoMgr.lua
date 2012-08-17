CFighterViewInfoMgr = class()

function CFighterViewInfoMgr:Ctor()
	self.m_pViewInfoTbl = {}
end

function CFighterViewInfoMgr.GetWnd()
	local mgr = g_GameMain.m_FighterViewInfoMgr
	if not mgr then
		mgr = CFighterViewInfoMgr:new()
		g_GameMain.m_FighterViewInfoMgr = mgr
	end
	return mgr
end

function CFighterViewInfoMgr:AddViewInfoByID(uEntityID)
	if self.m_pViewInfoTbl[uEntityID] then
		self.m_pViewInfoTbl[uEntityID][2] = self.m_pViewInfoTbl[uEntityID][2] + 1
	else
		local pViewInfo = CFighterViewInfo:new()
		pViewInfo:CppInit(uEntityID)
		self.m_pViewInfoTbl[uEntityID] = {pViewInfo,1}
	end
	g_MainPlayer:OpenObjPanel(uEntityID, self.m_pViewInfoTbl[uEntityID][1], false, true)
end

function CFighterViewInfoMgr:DelViewInfoByID(uEntityID)
	if self.m_pViewInfoTbl[uEntityID] then
		self.m_pViewInfoTbl[uEntityID][2] = self.m_pViewInfoTbl[uEntityID][2] - 1
		if self.m_pViewInfoTbl[uEntityID][2] == 0 then
			g_MainPlayer:OpenObjPanel(uEntityID, nil, false, false)
			self.m_pViewInfoTbl[uEntityID] = nil
		end
	end
end

function CFighterViewInfoMgr:GetFightViewInfoByID(uEntityID)
	if self.m_pViewInfoTbl[uEntityID] and self.m_pViewInfoTbl[uEntityID][1] then
		return self.m_pViewInfoTbl[uEntityID][1]
	end
	return nil
end

