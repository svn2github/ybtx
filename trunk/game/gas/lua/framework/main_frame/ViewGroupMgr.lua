gas_require "framework/main_frame/ViewGroupMgrInc"




function CViewGroupMgr:Ctor()
	self.m_uCurViewGroup = 0
end



function CViewGroupMgr:NewViewGroup()
	self.m_uCurViewGroup = self.m_uCurViewGroup + 1
	return self.m_uCurViewGroup
end