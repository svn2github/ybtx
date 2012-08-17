lan_load "player/Lan_NewRoleProperty_Common"
gac_require "framework/display_common/DisplayCommonInc"

function CDisplayCommon:Ctor()
	self.m_tblClassCanUse =
		{
			EClass.eCL_MagicWarrior,	--"魔剑士"
			EClass.eCL_NatureElf,		--"法师"
			EClass.eCL_EvilElf,			--"邪魔"
			EClass.eCL_Priest,			--"牧师"
--			EClass.eCL_Paladin,			--"人类骑士"
--			EClass.eCL_ElfHunter,		--"精灵弓箭手"
			EClass.eCL_Warrior,			--"大剑士"
			EClass.eCL_OrcWarrior,		--"兽人战士"
--			EClass.eCL_DwarfPaladin		--"矮人骑士"
		}
			
	self.m_tblPlayerClassForShow =
		{
			[EClass.eCL_MagicWarrior]	= ToolTipsText(232),	--"魔剑士"
			[EClass.eCL_NatureElf]		= ToolTipsText(233),	--"法师"
			[EClass.eCL_EvilElf]		= ToolTipsText(234),	--"邪魔"
			[EClass.eCL_Priest]			= ToolTipsText(235),	--"牧师"
			[EClass.eCL_Paladin]		= ToolTipsText(236),	--"人类骑士"
			[EClass.eCL_ElfHunter]		= ToolTipsText(237),	--"精灵弓箭手"
			[EClass.eCL_Warrior]		= ToolTipsText(238),	--"大剑士"
			[EClass.eCL_OrcWarrior]		= ToolTipsText(239),	--"兽人战士"
			[EClass.eCL_DwarfPaladin]	= ToolTipsText(240)		--"矮人骑士"
		}
	
	self.m_tblPlayerShortClassForShow =
		{
			[EClass.eCL_MagicWarrior]	= ToolTipsText(284),	--"魔剑"
			[EClass.eCL_NatureElf]		= ToolTipsText(285),	--"法师"
			[EClass.eCL_EvilElf]		= ToolTipsText(286),	--"邪魔"
			[EClass.eCL_Priest]			= ToolTipsText(287),	--"牧师"
			[EClass.eCL_Paladin]		= ToolTipsText(288),	--"人骑"
			[EClass.eCL_ElfHunter]		= ToolTipsText(289),	--"精灵"
			[EClass.eCL_Warrior]		= ToolTipsText(290),	--"大剑"
			[EClass.eCL_OrcWarrior]		= ToolTipsText(291),	--"兽人"
			[EClass.eCL_DwarfPaladin]	= ToolTipsText(292)		--"矮人"
		}
		
	self.m_tblDisplayLevel = {"G", "F", "E", "D", "C", "B", "A", "S"}
	
	self.m_tblAreaFbPoinDisplay =
		{
			[1] = ToolTipsText(251),
			[2] = ToolTipsText(252),
			[3] = ToolTipsText(253),
			[4] = ToolTipsText(254),
			[5] = ToolTipsText(255),
			[6] = ToolTipsText(256),
			[7] = ToolTipsText(298),
			[8] = ToolTipsText(299),
			[9] = ToolTipsText(300),
			[10] = ToolTipsText(1200),
			[11] = ToolTipsText(1199)
		}
end

function CDisplayCommon:GetPlayerCampForShow(nCamp)
	return Lan_NewRoleProperty_Common(nCamp, "DisplayName")
end

function CDisplayCommon:GetPlayerClassForShow(ClassID)
	return self.m_tblPlayerClassForShow[ClassID]
end

function CDisplayCommon:GetPlayerShortClassForShow(ClassID)
	return self.m_tblPlayerShortClassForShow[ClassID]
end

function CDisplayCommon:GetPlayerClassCanUseTable()
	local tblResult = {}
	for i, v in pairs(self.m_tblClassCanUse) do
		local tbl = {}
		tbl.classID				= v
		tbl.classDisplayName	= self.m_tblPlayerClassForShow[v]
		table.insert(tblResult, tbl)
	end
	return tblResult
end

function CDisplayCommon:GetDisplayLevel(nLevel)
	return self.m_tblDisplayLevel[nLevel]
end

function CDisplayCommon:GetTongTypeName(type)
	local sShowType = ""
	if type == g_TongMgr:GetTongTypeByName("普通") then
		sShowType = GetStaticTextClient(10045)
	elseif type == g_TongMgr:GetTongTypeByName("战斗") then
		sShowType = GetStaticTextClient(10046)
	elseif type == g_TongMgr:GetTongTypeByName("生产") then
		sShowType = GetStaticTextClient(10053)
	elseif type == g_TongMgr:GetTongTypeByName("精英战斗") then
		sShowType = GetStaticTextClient(10054)
	elseif type == g_TongMgr:GetTongTypeByName("精英生产") then
		sShowType = GetStaticTextClient(10055)
	end
	return sShowType
end