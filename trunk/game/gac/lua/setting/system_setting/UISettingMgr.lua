CUISettingMgr = class()

local fl_tblIndex = {}
fl_tblIndex["玩家名称"]				= 1
fl_tblIndex["玩家称号"]				= 2
fl_tblIndex["玩家佣兵小队"]			= 3
fl_tblIndex["玩家佣兵团"]			= 4
fl_tblIndex["其他玩家名称"]			= 5
fl_tblIndex["其他玩家称号"]			= 6
fl_tblIndex["其他玩家佣兵小队"]		= 7
fl_tblIndex["其他玩家佣兵团"]		= 8
fl_tblIndex["NPC名称"]				= 9
fl_tblIndex["怪物名称"]				= 10
fl_tblIndex["显示头盔"]				= 11
fl_tblIndex["锁定快捷栏"]			= 12
fl_tblIndex["鼠标悬停显示怪物名称"]	= 13
fl_tblIndex["友方不可选中"]			= 14

function CUISettingMgr:Ctor()
	self.m_tblDefault = {true,false,false,false,true,false,false,false,true,true,true,false,true,false}
	self.m_tblInfo = {}
end

function CUISettingMgr:ReturnGetUISetting(b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14)
	self.m_tblInfo = {b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14}
	g_GameMain.m_CharacterInSyncMgr:UpdateCharacterInSync()
	g_GameMain.m_CharacterInSyncMgr:PlayerHeadInfoInit()
	g_GameMain.m_CharacterInSyncMgr:UpdateFriendCanSelectedInSync()
end

--得到某个属性被设置的状态，返回结果是true或者false
function CUISettingMgr:GetOnePropState(sIndex)
	if not self.m_tblInfo then return false end
	return self.m_tblInfo[fl_tblIndex[sIndex]]
end

function Gas2Gac:ReturnGetUISetting(Conn,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14)
	g_UISettingMgr:ReturnGetUISetting(b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14)
end