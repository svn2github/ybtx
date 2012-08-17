cfg_load "information/guide/GuideType_Client"
cfg_load "information/guide/GuideCondition_Client"
cfg_load "information/guide/GuideNeedFinishQuest_Client"
cfg_load "information/guide/GuideWeighting_Client"
cfg_load "information/guide/GuideAutoTrack_Client"
lan_load "information/guide/Lan_GuideCondition_Client"
lan_load "skill/Lan_NonFightSkill_Common"
lan_load "skill/Lan_SkillPart_Common"
lan_load "scene/Lan_Scene_Basic_Common"

CGuideData = class()

local tblCampIndex = {}
tblCampIndex["大剑士"]		= 1
tblCampIndex["魔剑士"]		= 2
tblCampIndex["法师"]		= 3
tblCampIndex["邪魔"]		= 4
tblCampIndex["牧师"]		= 5
tblCampIndex["兽人战士"]	= 6

local tblShowOrder = {"NeedMainLineNum", "NeedCharLevel", "NeedFinishQuest", "NeedNonFightSkill", "NeedItem",
	"FightSkill", "NeedMoney", "Description", "OpenPanel", "AutoTrack", "UseSkill", "NeedAction",
	"SuggestFightValue", "Link"}
	
local linkColor		= g_ColorMgr:GetColor("攻略超链接")
local textColor		= g_ColorMgr:GetColor("攻略文字")
local linkNpcColor	= "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标移动到NpcObj"), 3, -1) .. "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标单击"), 3, -1)
local linkSkillColor = "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标移动到Skill"), 3, -1) .. "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标单击"), 3, -1)

function CGuideData:Ctor()
	self.m_tblActionCount	= {}
	self.m_tblActionTimes	= {}
end

function CGuideData:RefreshData()
	self.m_tblBeDone			= {}
	self.m_tblDirectionClear	= {}
	
	for i, v in ipairs( GuideType_Client:GetKeys() ) do
		for j, w in ipairs( GuideType_Client(v):GetKeys() ) do
			local bDone, bClear = self:IsConditionBeDone(w)
			self.m_tblBeDone[w] = {bDone, bClear}
		end
	end
	
	self.m_tblStringIndex	= {}
	for i, v in ipairs( GuideType_Client:GetKeys() ) do
		local subNode = GuideType_Client(v)
		local a = 1
		for j, w in ipairs( subNode:GetKeys() ) do
			local ConditionSubNode		= GuideCondition_Client(w)
			local sPreConditionReplace	= ConditionSubNode("PreConditionReplace")
			if( ("" == sPreConditionReplace or self.m_tblBeDone[sPreConditionReplace][1]) and not self.m_tblBeDone[w][1]) then
				local sBigType				= ConditionSubNode("BigType")
				local sPreConditionClear	= ConditionSubNode("PreConditionClear")
				if("" ~= sPreConditionClear) then
					self.m_tblBeDone[w][2]	= self.m_tblBeDone[w][2] or self.m_tblBeDone[sPreConditionClear][1]
				end
				if("" ~= sBigType) then
					self.m_tblStringIndex[sBigType]	= {i, a}
				else
					self.m_tblStringIndex[w]		= {i, a}
				end
				a = a + 1
			end
			if("" ~= sPreConditionReplace and not self.m_tblBeDone[sPreConditionReplace][1]) then
				self.m_tblBeDone[w][2] = false
			end
			
			if(self.m_tblBeDone[w][2]) then
				self.m_tblDirectionClear[i] = true
			end
		end
	end
	self.m_tblCondition = {}
	g_GameMain.m_GuideWnd.m_sSmallTypeForHighLight = nil
	for i, v in ipairs( GuideType_Client:GetKeys() ) do
		self.m_tblCondition[i] = {}
		self.m_tblCondition[i].sDirection = v
		local subNode = GuideType_Client(v)
		local a = 1
		for j, w in ipairs( subNode:GetKeys() ) do
			local sPreConditionReplace	= GuideCondition_Client(w, "PreConditionReplace")
			if( ("" == sPreConditionReplace or self.m_tblBeDone[sPreConditionReplace][1]) and not self.m_tblBeDone[w][1]) then
				if( self.m_tblBeDone[w][2] ) then
					self.m_tblCondition[i][a] = self:GetConditionList(w)
				else
					self.m_tblCondition[i][a] = {}
				end
				self.m_tblCondition[i][a].sDirection = v
				self.m_tblCondition[i][a].sType = w
				local tblMainQuest = CQuest.GetAvaliableMasterStrokeQuests(0)
				if	(	(
							(w == "剧情任务" and (self:BeQuestEmpty() or next(tblMainQuest))) or
							(not self.m_tblBeDone[w][1] and self.m_tblBeDone[w][2] and 1 ~= GuideCondition_Client(w, "Last"))
						) and #self.m_tblCondition[i][a] > 0
					) then
					local sBigType = GuideCondition_Client(w, "BigType")
					if( GuideWeighting_Client(sBigType) ) then
						local sSaveSmallType	= g_GameMain.m_GuideWnd.m_sSmallTypeForHighLight
						local sSaveBigType		= GuideCondition_Client(sSaveSmallType, "BigType")
						local nSaveWeighting	= GuideWeighting_Client(sSaveBigType, "Weighting")
						local nWeighting		= GuideWeighting_Client(sBigType, "Weighting")
						if( not sSaveSmallType or nSaveWeighting < nWeighting ) then
							g_GameMain.m_GuideWnd.m_sSmallTypeForHighLight = w
						end
					end
				end
				
				a = a + 1
			end
		end
		
	end
end

function CGuideData:BeQuestEmpty()
	local IsShow = true
	for QuestName,_ in pairs(g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo) do
		if g_AllMercenaryQuestMgr[QuestName] then
			IsShow = false
		elseif g_DareQuestMgr[QuestName] then
		elseif g_ActionQuestMgr[QuestName] then
		else
			if not g_HideQuestMgr[QuestName] then
				IsShow = false
			end
		end
	end
	return IsShow
end

--------------------------------------------------------------------------------------------------------
local function QuestLink(tblInfo, sMsgIndex)
	local sDisplayName = ""
	if(tblInfo[1] and tblInfo[2] and tblInfo[3] and tblInfo[4]) then
		local value = string.format( "%s,%s,%s,%d,%d", "寻路", tblInfo[1], tblInfo[2], tblInfo[3], tblInfo[4])
		sDisplayName = linkColor .. "#u#l" .. linkNpcColor .. tblInfo[6] .. "#i[" .. MemH64(value) .. "]#l#u" .. textColor
	else
		sDisplayName = tblInfo[6]
	end
	return GetStaticTextClient(sMsgIndex, sDisplayName, tblInfo[5])
end

--记得每添加或者删除一个条件的时候，tblShowOrder也要修改
function CGuideData:GetConditionList(sType)
	local subNode				= GuideCondition_Client(sType)
	local nNeedMainLineNum		= subNode("NeedMainLineNum")
	local nNeedCharLevel		= subNode("NeedCharLevel")
	local nNeedFinishQuest		= subNode("NeedFinishQuest")
	local sNeedNonFightSkill	= subNode("NeedNonFightSkill")
	local sNeedItem				= subNode("NeedItem")
	local nSuggestFightValue	= subNode("SuggestFightValue")
	local nNeedMoney			= subNode("NeedMoney")
	local nAutoTrack			= subNode("AutoTrack")
	local sHide					= subNode("Hide")
	local sAction				= subNode("Action")
	local sOpenPanel			= subNode("OpenPanel")
	local sFightSkill			= subNode("FightSkill")
	local sUseSkill				= subNode("UseSkill")
	local sLink					= subNode("Link")
	local sNeedAction			= subNode("NeedAction")
	local sDescription			= Lan_GuideCondition_Client(MemH64(sType), "Description")
	
	local nLevel		= g_GameMain.m_PlayerInfoTbl.m_PlayerLevel
	local nBirthCamp	= g_GameMain.m_PlayerInfoTbl.m_PlayerBirthCamp
	local nClass		= g_GameMain.m_PlayerInfoTbl.m_PlayerClassID
	local sClass		= g_GetPlayerClassNameByID(nClass)
	
	if("剧情任务" == sType) then
		local tblQuest = {}
		local Info = CQuest.GetAvaliableMasterStrokeQuests(0)--LevelDiff
		if( next(Info) or nLevel < 5) then
			for i, v in ipairs(Info) do
				local sDisplayName = QuestLink(v, 28010)
				table.insert(tblQuest, sDisplayName)
			end
			return tblQuest
		end
	end
	
	local tblHideInfo = {}
	SplitStringEx(sHide, ";", tblHideInfo)
	
	local tbl = {}
	local sDone = CTextureFontMgr_Inst():GetEmtStrByIdx("#1109")
	
	if(nNeedMainLineNum > 0 and not tblHideInfo["NeedMainLineNum"]) then
		local value				= string.format("%s,%s", "跳转", "剧情任务")
		local sDisplayName		= linkColor .. "#u#l" .. GetStaticTextClient(28002) .. "#i[" .. MemH64(value) .. "]#l#u" .. textColor
		local nFinishNum		= CountMasterStrokeQuest()
		local str				= GetStaticTextClient(28001, nNeedMainLineNum, sDisplayName, nFinishNum, nNeedMainLineNum)
		tbl["NeedMainLineNum"]	= nFinishNum < nNeedMainLineNum and str or str .. sDone
	end
	
	if(nNeedCharLevel > 0 and not tblHideInfo["NeedCharLevel"]) then
		local str = GetStaticTextClient(28003, nNeedCharLevel, nLevel, nNeedCharLevel)
		tbl["NeedCharLevel"] = nLevel < nNeedCharLevel and str or str .. sDone
	end
	
	if(nNeedFinishQuest > 0 and not tblHideInfo["NeedFinishQuest"]) then
		local QuestName	= GuideNeedFinishQuest_Client(tostring(nNeedFinishQuest), tostring(nBirthCamp), sClass, "Quest")
		local tblInfo	= CQuest.GetAvaliableMasterStrokeQuestByName(QuestName)
		local str		= QuestLink(tblInfo, 28012)
		local QuestInfo	= g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName]
		local bFlag		= QuestInfo and 3 == QuestInfo.m_State
		tbl["NeedFinishQuest"] = bFlag and str .. sDone or str
	end
	
	if("" ~= sNeedNonFightSkill and not tblHideInfo["NeedNonFightSkill"]) then
		local sDisplayName	= Lan_NonFightSkill_Common(MemH64(sNeedNonFightSkill), "DisplayName")
		local tblIndex		= self.m_tblStringIndex[sNeedNonFightSkill]
		if(tblIndex) then
			local value	= string.format("%s,%s", "跳转", sNeedNonFightSkill)
			sDisplayName = linkColor .. "#u#l" .. linkSkillColor .. sDisplayName .. "#i[" .. MemH64(value) .. "]#l#u" .. textColor
		end
		local str = GetStaticTextClient(28004, sDisplayName)
		tbl["NeedNonFightSkill"] = self:BeLearnTheSkill(sNeedNonFightSkill) and str .. sDone or str
	end
	
	if("" ~= sNeedItem and not tblHideInfo["NeedItem"]) then
		local tblItemInfo = {}
		SplitString(sNeedItem, ",", tblItemInfo)
		local nNowNum = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace( g_StoreRoomIndex.PlayerBag, tonumber(tblItemInfo[1]), tblItemInfo[2] )
		local sDisplayName = g_ItemInfoMgr:GetItemLanInfo(tonumber(tblItemInfo[1]), tblItemInfo[2], "DisplayName")
		local str = GetStaticTextClient(28005, sDisplayName, nNowNum)
		tbl["NeedItem"] = nNowNum < 1 and str or str .. sDone 
	end
	
	if("" ~= sFightSkill and  not tblHideInfo["FightSkill"]) then
		local tblInfo = {}
		SplitString(sFightSkill, ";", tblInfo)
		local sSkill		= tblInfo[tblCampIndex[sClass]]
		local sNpc			= linkColor .. "#u#l" .. linkSkillColor .. linkNpcColor .. GetStaticTextClient(28009) .. "#i[" .. MemH64("技能学习寻路") .. "]#l#u" .. textColor
		local LanInfo		= Lan_SkillPart_Common( MemH64(sSkill) )
		local sDisplaySkill	= LanInfo and LanInfo("DisplayName") or ""
		local tblIndex		= self.m_tblStringIndex[sSkill]
		if(tblIndex) then
			local value		= string.format("%s,%s", "跳转", sSkill)
			sDisplaySkill	= linkColor .. "#u#l" .. sDisplaySkill .. "#i[" .. MemH64(value) .. "]#l#u" .. textColor
		end
		local str = GetStaticTextClient(28006, sNpc, sDisplaySkill)
		tbl["FightSkill"] = self:BeLearnTheSkill(sSkill) and str .. sDone or str
	end
	
	if("" ~= sNeedAction and not tblHideInfo["NeedAction"]) then
		local tblInfo = {}
		SplitString(sNeedAction, ",", tblInfo)
		local tblIndex		= self.m_tblStringIndex[tblInfo[3]]
		local sDisplayName	= Lan_Scene_Basic_Common(MemH64(tblInfo[1]), "DisplayName")
		if(tblIndex) then
			local value		= string.format("%s,%s", "跳转", tblInfo[3])
			sDisplayName	= linkColor .. "#u#l" .. sDisplayName .. "#i[" .. MemH64(value) .. "]#l#u" .. textColor
		end
		local bFlag			= not self.m_tblActionTimes[tblInfo[1]] or self.m_tblActionTimes[tblInfo[1]] < tonumber(tblInfo[2])
		local str			= GetStaticTextClient(28011, sDisplayName, tblInfo[2], self.m_tblActionTimes[tblInfo[1]] or 0, tblInfo[2])
		tbl["NeedAction"]	= bFlag and str or str .. sDone
	end
	
	if(nNeedMoney > 0 and not tblHideInfo["NeedMoney"]) then
		local nNowMoney		= g_GameMain.m_PlayerInfoTbl.m_PlayerMoney
		local sNeedMoney	= g_MoneyMgr:ChangeMoneyToString(nNeedMoney)
		local str			= GetStaticTextClient(28007, sNeedMoney)
		tbl["NeedMoney"]	= nNowMoney < nNeedMoneystr and str or str . sDone
	end
	
	if("" == sAction) then
		if("" ~= sDescription) then
			tbl["Description"] = sDescription
		end
	else
		tbl["Description"] = { sDescription, {"副本", sAction} }
	end
	
	if("" ~= sOpenPanel) then
		local DisplayOpenPanel = Lan_GuideCondition_Client(MemH64(sType), "DisplayOpenPanel")
		tbl["OpenPanel"] = {"开启面板", sOpenPanel, DisplayOpenPanel}
	end
	
	if(0 ~= nAutoTrack and not tblHideInfo["AutoTrack"]) then
		local DisplayAutoTrack =  Lan_GuideCondition_Client(MemH64(sType), "DisplayAutoTrack")
		local tblInfo = {}
		SplitString(DisplayAutoTrack, ";", tblInfo)
		local sAutoTrack = GuideAutoTrack_Client(tostring(nAutoTrack), tostring(nBirthCamp), "AutoTrack")
		local value = ""
		if("技能学习寻路" == sAutoTrack) then
			value = "技能学习寻路"
		else
			local Info = {}
			SplitString(sAutoTrack, ",", Info)
			value = string.format( "%s,%s,%s,%s,%s", "寻路", unpack(Info) )
		end
		local sDisplayName = linkColor .. "#u#l" .. linkNpcColor .. tblInfo[2] .. "#i[" .. MemH64(value) .. "]#l#u" .. textColor
		local str = GetStrAccordWildcard(tblInfo[1], sDisplayName)
		tbl["AutoTrack"] = str
	end
	
	if("" ~= sUseSkill) then
		local sDisplayUseSkill	= Lan_GuideCondition_Client(MemH64(sType), "DisplayUseSkill")
		local sDisplayName		= Lan_NonFightSkill_Common(MemH64(sUseSkill), "DisplayName")
		local value				= string.format("%s,%s", "技能", sUseSkill)
		sDisplayName			= linkColor .. "#u#l" .. linkSkillColor .. sDisplayName .. "#i[" .. MemH64(value) .. "]#l#u" .. textColor
		local str = GetStrAccordWildcard(sDisplayUseSkill, sDisplayName)
		tbl["UseSkill"] = str
	end
	
	if(nSuggestFightValue > 0 and not tblHideInfo["SuggestFightValue"]) then
		local nFightValue = self:GetPlayerFightingEvalution()
		local str = GetStaticTextClient(28008, nSuggestFightValue, nFightValue, nSuggestFightValue)
		tbl["SuggestFightValue"] = str
	end
	
	if("" ~= sLink) then
		local tblInfo = {}
		SplitString(sLink, ",", tblInfo)
		local strLink = ""
		for i, v in ipairs(tblInfo) do
			strLink	= strLink .. tbl[v]
			tbl[v]	= nil
		end
		tbl["Link"] = strLink
	end
	
	local tblResult = {}
	for i, v in ipairs(tblShowOrder) do
		if(tbl[v]) then
			table.insert(tblResult, tbl[v])
		end
	end
	
	return tblResult
end

function CGuideData:IsConditionBeDone(sType)
	local subNode				= GuideCondition_Client(sType)
	local sPreConditionReplace	= subNode("PreConditionReplace")
	
	if("" ~= sPreConditionReplace) then
		if( not self:IsConditionBeDone(sPreConditionReplace) ) then
			return false, false
		end
	end
	
	local nClearLevel	= subNode("ClearLevel")
	local nLast			= subNode("Last")
	local bClear		= g_GameMain.m_PlayerInfoTbl.m_PlayerLevel >= nClearLevel
	if(1 == nLast) then
		return false, bClear
	end
	
	local nNeedMainLineNum		= subNode("NeedMainLineNum")
	local nNeedCharLevel		= subNode("NeedCharLevel")
	local nNeedFinishQuest		= subNode("NeedFinishQuest")
	local sNeedNonFightSkill	= subNode("NeedNonFightSkill")
	local sNeedItem				= subNode("NeedItem")
	local nNeedMoney			= subNode("NeedMoney")
	local sAction				= subNode("Action")
	local sFightSkill			= subNode("FightSkill")
	local sNeedAction			= subNode("NeedAction")
	
	local nBirthCamp	= g_GameMain.m_PlayerInfoTbl.m_PlayerBirthCamp
	local nClass		= g_GameMain.m_PlayerInfoTbl.m_PlayerClassID
	local sClass		= g_GetPlayerClassNameByID(nClass)
	
	if(nNeedMainLineNum > 0) then
		local nFinishNum = CountMasterStrokeQuest()
		if(nFinishNum < nNeedMainLineNum ) then
			return false, bClear
		end
	end
	
	if(nNeedCharLevel > 0) then
		local nLevel = g_GameMain.m_PlayerInfoTbl.m_PlayerLevel
		if(nLevel < nNeedCharLevel ) then
			return false, bClear
		end
	end
	
	if(nNeedFinishQuest > 0) then
		local QuestName	= GuideNeedFinishQuest_Client(tostring(nNeedFinishQuest), tostring(nBirthCamp), sClass, "Quest")
		local QuestInfo	= g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName]
		local bFlag		= QuestInfo and 3 == QuestInfo.m_State
		if(not bFlag) then
			return false, bClear
		end
	end
	
	if("" ~= sNeedNonFightSkill) then
		local bFlag = self:BeLearnTheSkill(sNeedNonFightSkill)
		if(not bFlag) then
			return false, bClear
		end
	end
	
	if("" ~= sNeedItem) then
		local tblItemInfo = {}
		SplitString(sNeedItem, ",", tblItemInfo)
		local nNowNum = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace( g_StoreRoomIndex.PlayerBag, tonumber(tblItemInfo[1]), tblItemInfo[2] )
		if(nNowNum < 1 ) then
			return false, bClear
		end
	end
	
	if("" ~= sFightSkill) then
		local tbl = {}
		SplitString(sFightSkill, ";", tbl)
		local nClass	= g_GameMain.m_PlayerInfoTbl.m_PlayerClassID
		local sClass	= g_GetPlayerClassNameByID(nClass)
		local sSkill	= tbl[tblCampIndex[sClass]]
		local bFlag		= self:BeLearnTheSkill(sSkill)
		if(not bFlag) then
			return false, bClear
		end
	end
	
	if("" ~= sNeedAction) then
		local tblInfo = {}
		SplitString(sNeedAction, ",", tblInfo)
		if( not self.m_tblActionTimes[tblInfo[1]] or self.m_tblActionTimes[tblInfo[1]] < tonumber(tblInfo[2]) ) then
			return false, bClear
		end
	end
	
	if(nNeedMoney > 0) then
		local nNowMoney		= g_GameMain.m_PlayerInfoTbl.m_PlayerMoney
		local sNeedMoney	= g_MoneyMgr:ChangeMoneyToString(nNeedMoney)
		if(nNowMoney < nNeedMoney ) then
			return false, bClear
		end
	end
	
	if( "" ~= sAction and (not self.m_tblActionTimes[sAction] or 0 == self.m_tblActionTimes[sAction]) ) then
		return false, bClear
	end
	
	return true, bClear
end

function CGuideData:CheckConditionBeDone()
	local tbl			= {}
	local bDoneFlag		= false
	local bClearFlag	= false
	for i, v in ipairs( GuideType_Client:GetKeys() ) do
		for j, w in ipairs( GuideType_Client(v):GetKeys() ) do
			local bDone, bClear = self:IsConditionBeDone(w)
			if(bDone and not self.m_tblBeDone[w][1]) then
				self.m_tblBeDone[w][1] = true
				table.insert(tbl, w)
				bDoneFlag = true
			end
			if(bClear and not self.m_tblBeDone[w][2]) then
				self.m_tblBeDone[w][2] = true
				table.insert(tbl, w)
				bClearFlag = true
			end
		end
	end
	
	if(bDoneFlag or bClearFlag) then
		if( g_GameMain.m_GuideWnd:IsShow() ) then
			g_GameMain.m_GuideWnd:OpenPanel(true)
		else
			g_GameMain.m_GuideBtn:BtnFlash()
		end
	end
end

function CGuideData:GetPlayerFightingEvalution()
	local text = g_GameMain.m_FightingEvaluationWnd.m_RatingValueInfoTbl[7]:GetWndText()
	local evalution = tonumber(text) or 0
	return evalution
end

function CGuideData:BeLearnTheSkill(skillName)
	for i, v in pairs (g_GameMain.m_LearnSkillTbl) do
		local skillInfo = v
		if v[1] == skillName then
			return true
		end
	end
	return false
end

function CGuideData:GetJoinCountTimes(ActionName, Count1, Count2)
	local LeagueName		= Lan_FbActionDirect_Common(MemH64(ActionName), "LeagueName")
	local limitType			= ""
	local PlayerLev			= g_GameMain.m_PlayerInfoTbl.m_PlayerLevel
	local JoinCountTimes	= 0
	
	if LeagueName and LeagueName ~= "" then
		limitType = FbActionDirect_Common(ActionName, "LeagueName") or ""
	else
		limitType = FbActionDirect_Common(ActionName, "MaxTimes") or ""
	end
	
	if limitType == "佣兵训练活动" then
		if PlayerLev >= 10 and PlayerLev < 20 then
			JoinCountTimes = 1
		end
	end
	
	-- 特定的活动可以累计次数
	if JoinCountLimit_Server(limitType, "Cycle") == "store" then
		JoinCountTimes = Count2
	else
		JoinCountTimes = JoinCountLimit_Server(limitType, "Count")
	end
	
	if Lan_FbActionDirect_Common(MemH64(ActionName), "LeagueName") ~= "" then
		--Times = self.m_LeagueJointTims[LeagueName]
		return GetStaticTextClient(8005)
	elseif JoinCountTimes ~= nil then
		-- 检查RPC是否更新了次数
		local nTimes = Count1
		if nTimes then
			return nTimes.."/"..JoinCountTimes
		else
			return "0/"..JoinCountTimes
		end
	else
		return GetStaticTextClient(8001)
	end
end

function CGuideData:ReturnGetActionCountExBegin()
	self.m_tblActionCount	= {}
	self.m_tblActionTimes	= {}
end

function CGuideData:ReturnGetActionCountEx(ActionName, Count1, Count2)
	self.m_tblActionCount[ActionName] = self:GetJoinCountTimes(ActionName, Count1, Count2)
end

function CGuideData:ReturnGetActionAllTimes(sName, nTime)
	self.m_tblActionTimes[sName] = nTime
end

function CGuideData:ReturnGetActionCountExEnd()
	self:RefreshData()
	local wnd = g_GameMain.m_GuideWnd
	wnd:RefrashRadio()
	if(wnd.m_sSmallTypeForHighLight) then
		wnd:OpenForSaveHighLight(wnd.m_sSmallTypeForHighLight)
	else
		wnd.m_tblRadio[1]:SetCheck(true)
		wnd:SetList(1)
	end
end
