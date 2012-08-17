gac_gas_require "scene_mgr/SceneCfg"
cfg_load "scene/PkRuleInfo_Common"

CScenePictureWnd = class( SQRDialog )
local CAreaFrogWnd = class( SQRDialog )
local CAreaStateWnd = class(SQRDialog)

function CAreaFrogWnd:Ctor(Parent)	
	self:CreateFromRes("AreaFrogWnd" , Parent )
	self:ShowWnd( true )
	self.m_Frog = self:GetDlgChild("Frog")
	self:SetStyle(0x60000000)
end

-- DesPosWnd 为区域按钮
function CAreaStateWnd:Ctor(Parent, DesPosWnd, AreaName, SceneName)
	-- 判断中地图是否显示状态标记
	local IsShow = false
	local pkRuleKeys = PkRuleInfo_Common:GetKeys()
	for i, pkRuleAreaTemp in pairs(pkRuleKeys) do
		if pkRuleAreaTemp == AreaName then
			local pkRuleTemp = PkRuleInfo_Common(pkRuleAreaTemp)
			local tempKeys = pkRuleTemp:GetKeys()
			for _, temp in pairs(tempKeys) do
				if temp == SceneName then
					IsShow = true
					break
				end
			end
		end
	end

	if not IsShow then
		return
	end
	
	self:CreateFromRes("AreaStateWnd", Parent )
	self.m_StateWnd = self:GetDlgChild("StatePic")
	self.m_StateWnd:SetMouseOverDescAfter("#cffeaab"..GetStaticTextClient(19021))
	self.m_StateWnd:ShowWnd(true)
	
	local DesWndWidth = DesPosWnd:GetWndOrgWidth()
	local DesWndHeight = DesPosWnd:GetWndOrgHeight()
	local WndWidth = self:GetWndOrgWidth()
	local WndHeight = self:GetWndOrgHeight()
	
	local rt = CFRect:new()
	DesPosWnd:GetLogicRect(rt)
	local rt_1 	= CFRect:new()
	rt_1.top 		= rt.top + (DesWndHeight - WndHeight) / 2 - 20
	rt_1.bottom = rt_1.top + WndHeight
	rt_1.left 	= rt.left + (DesWndWidth - WndWidth) / 2
	rt_1.right 	= rt_1.left + WndWidth
	
	self:SetLogicRect(rt_1)
	self:SetFocus()
	self:ShowWnd(true)
	DesPosWnd.m_AreaStateWnd = self
end

--local function CreateScenePictureWnd( Parent,SceneName )
function CScenePictureWnd:Ctor(Parent,SceneName)
	local index = SceneArea_Common_Shell(SceneName, "PictureWndIndex")
	local MapFile = Scene_Common[SceneName].MapFile
	
--	local Wnd = CScenePictureWnd:new()
	if index ~= 0 then
		self:CreateFromRes("SceneMap"..index , Parent )
	else
		self:CreateFromRes("SceneMap" , Parent )
	end
	Parent["m_ScenePictureWnd"] = self
	
	self.m_FrogWnd = CAreaFrogWnd:new(self)
	self.m_FrogWnd.m_Frog:InitMapData(MapFile)
	for AreaName, v in pairs(g_SceneAreaList[SceneName]) do
		local areainfo = g_GameMain.m_AreaInfoWnd.m_PlayerAreaInfoTbl[AreaName]
		if v[1] then
			self["m_AreaBtn"..v[1]] = self:GetDlgChild("AreaBtn"..v[1])
			---------------两行临时代码---------------
			self["m_AreaBtn"..v[1]]:EnableWnd(false)
			self["m_AreaBtn"..v[1]]:SetStyle(0x65400000)
			----------------------------------
			self["m_AreaName"..v[1]] = AreaName
			
			-- 创建区域状态窗口
			if not self["m_AreaBtn"..v[1]].m_AreaStateWnd then
				CAreaStateWnd:new(self, self["m_AreaBtn"..v[1]], AreaName, SceneName)
			end
			
			local ShowAreaName = AreaName
			ShowAreaName = string.gsub(ShowAreaName,"艾米帝国","")
			ShowAreaName = string.gsub(ShowAreaName,"神圣教廷","")
			ShowAreaName = string.gsub(ShowAreaName,"修斯帝国","")	
			if areainfo == nil or areainfo["IsDiscovery"] == 0 then
				self.m_FrogWnd.m_Frog:AddAreaFrog(AreaName)
				self["m_AreaBtn"..v[1]]:SetWndText(GetAreaDispalyName(ShowAreaName))
				self["m_AreaBtn"..v[1]]:SetFocus()
				self["m_AreaBtn"..v[1]]:ShowWnd(true)
			else
				local areaquestnum = 0
				local finishnum = 0
				if g_AreaQuestMgr[AreaName] then
					for i = 1, table.getn(g_AreaQuestMgr[AreaName]) do
						local questname = g_AreaQuestMgr[AreaName][i]
						if g_MasterStrokeQuestMgr[questname] then
							areaquestnum = areaquestnum + 1
							if g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname]
								and g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname].m_State == QuestState.finish then
								finishnum = finishnum+1
							end
						end
					end
				end
				
				local str = GetAreaDispalyName(ShowAreaName)
				if areaquestnum ~= 0 then
					str = str.."("..finishnum.."/"..areaquestnum..")"
				end
				self["m_AreaBtn"..v[1]]:SetWndText(str)
				self["m_AreaBtn"..v[1]]:ShowWnd(true)
				self["m_AreaBtn"..v[1]]:SetFocus()
			end
		else
			if areainfo == nil or areainfo["IsDiscovery"] == 0 then
				if string.find(AreaName,"主路") then
					self.m_FrogWnd.m_Frog:AddAreaFrog("主路")
				else
					self.m_FrogWnd.m_Frog:AddAreaFrog(AreaName)
				end
			end
		end
	end
	self.m_FrogWnd.m_Frog:UpdateVisibleRegion()
	self.m_SceneName = SceneName
	g_GameMain.m_AreaInfoWnd.m_ScenePool[SceneName] = self
end

function CScenePictureWnd:UpdateAreaQuestRecord(SceneName)
	for AreaName, v in pairs(g_SceneAreaList[SceneName]) do
		local areainfo = g_GameMain.m_AreaInfoWnd.m_PlayerAreaInfoTbl[AreaName]
		local ShowAreaName = AreaName
		ShowAreaName = string.gsub(ShowAreaName,"艾米帝国","")
		ShowAreaName = string.gsub(ShowAreaName,"神圣教廷","")
		ShowAreaName = string.gsub(ShowAreaName,"修斯帝国","")	
		if v[1] then
			if areainfo == nil or areainfo["IsDiscovery"] == 0 then
				self.m_FrogWnd.m_Frog:AddAreaFrog(AreaName)
				self["m_AreaBtn"..v[1]]:SetWndText(GetAreaDispalyName(ShowAreaName))
				self["m_AreaBtn"..v[1]]:SetFocus()
				self["m_AreaBtn"..v[1]]:ShowWnd(true)
			else
				
				local areaquestnum = 0
				local finishnum = 0
				
				if g_AreaQuestMgr[AreaName] then
					for i = 1, table.getn(g_AreaQuestMgr[AreaName]) do
						local questname = g_AreaQuestMgr[AreaName][i]
						if g_MasterStrokeQuestMgr[questname] then
							areaquestnum = areaquestnum + 1
							if g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname]
								and g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname].m_State == QuestState.finish then
								finishnum = finishnum+1
							end
						end
					end
				end
				local str = GetAreaDispalyName(ShowAreaName)
				if areaquestnum ~= 0 then
					str = str.."("..finishnum.."/"..areaquestnum..")"
				end
				self["m_AreaBtn"..v[1]]:SetWndText(str)
				self["m_AreaBtn"..v[1]]:ShowWnd(true)
				self["m_AreaBtn"..v[1]]:SetFocus()
			end
		end
	end
end

function CScenePictureWnd:ActiveAreaBtn()
	local AreaName = g_GameMain.m_AreaInfoWnd.m_LastOpenArea[self.m_SceneName]
	if AreaName and AreaName ~= "" then
		for sName,v in pairs(g_SceneAreaList[self.m_SceneName]) do
			if sName == AreaName then
--				self["m_AreaBtn"..v[1]]:EnableWnd(true)
--				self["m_AreaBtn"..v[1]]:SetStyle(0x45400000)
				return
			end
		end
	end
end

--子窗口消息
function CScenePictureWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK  then
		for AreaName,v in pairs(g_SceneAreaList[self.m_SceneName]) do
			if v[1] and Child == self["m_AreaBtn"..v[1]] then
				self:ActiveAreaBtn()
				return
			end
		end
	elseif uMsgID == BUTTON_MOUSEMOVE then
		if SceneArea_Common_Shell(self.m_SceneName) == nil then
			return
		end
		for AreaName,v in pairs(g_SceneAreaList[self.m_SceneName]) do
			if v[1] and Child == self["m_AreaBtn"..v[1]] then
				if g_GameMain.m_AreaInfoWnd.m_MapState == "Small" then
					g_GameMain.m_AreaInfoWnd:ShowCursorPosToolTips(self["m_AreaBtn"..v[1]])
					return
				end
			end
		end
	end
end

-- 区域状态按钮消息
function CAreaStateWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == RICHWND_LCLICKDOWN  then
		if Child == self.m_StateWnd then
			print("------------")
			local Wnd = g_GameMain.m_AreaInfoWnd
			Wnd:ClickMapAutoTrack()
			g_GameMain.m_AreaInfoWnd:SetTrackWndFocus()
		end
	end
end
