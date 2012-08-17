gac_gas_require "setting/SettingMgrInc"
cfg_load "setting/MouseSetting_Common"

function CSettingMgr:Ctor()
	self.m_tblMouseCtrl		= {}
	for i, v in pairs( MouseSetting_Common:GetKeys() ) do
		self.m_tblMouseCtrl[v]	= {}
		self.m_tblMouseCtrl[v].lockTarget	= math.abs(MouseSetting_Common(v, "LockTarget") - 1)
		self.m_tblMouseCtrl[v].movekey		= MouseSetting_Common(v, "Movekey")
		self.m_tblMouseCtrl[v].selectLeft	= MouseSetting_Common(v, "SelectLeft")
		self.m_tblMouseCtrl[v].selectRight	= MouseSetting_Common(v, "SelectRight")
		self.m_tblMouseCtrl[v].attrackLeft	= MouseSetting_Common(v, "AttrackLeft")
		self.m_tblMouseCtrl[v].attrackRight	= MouseSetting_Common(v, "AttrackRight")
	end
end

function CSettingMgr:GetMouseOptionByType(nType)
	return self.m_tblMouseCtrl[nType]
end

function CSettingMgr:MatchMouseSettingDefault(lockTarget, movekey, selectLeft, selectRight, attrackLeft, attrackRight)
	for i, v in ipairs(self.m_tblMouseCtrl) do
		if	(	v.lockTarget	== lockTarget	and
				v.movekey		== movekey		and
				v.selectLeft	== selectLeft	and
				v.selectRight	== selectRight	and
				v.attrackLeft	== attrackLeft	and
				v.attrackRight	== attrackRight
			) then
			return i
		end
	end
end

g_SettingMgr = g_SettingMgr or CSettingMgr:new()