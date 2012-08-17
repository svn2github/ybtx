gac_gas_require "scene_mgr/SceneCfg"

CQuestRecordWnd = class( SQRDialog )

local RecordCamp = nil

local Tree_QuestForcePlot = {}

local Tree_QuestMapPlot = {}

local function InitAreaQuestTree(PlayerCamp)
	--------初始化按地图划分的任务记录节点树-------------------
	RecordCamp = PlayerCamp
	Tree_QuestForcePlot = {}
	Tree_QuestMapPlot = {}
	
	local Keys = SceneArea_Common_Shell:GetKeys()
	for n=1, table.getn(Keys) do
		local SceneName = Keys[n]
		if Scene_Common[SceneName] then
			if PlayerCamp == Scene_Common[SceneName].Camp then
				local areatbl = {}
				local index = SceneArea_Common_Shell(SceneName, "PictureWndIndex")
				for AreaName,v in pairs( g_SceneAreaList[SceneName] ) do
					if g_AreaQuestMgr[AreaName] ~= nil then
						table.insert(areatbl,AreaName)
					end
				end
				if table.getn(areatbl) > 0 then
					Tree_QuestMapPlot[index] = {}
					Tree_QuestMapPlot[index].m_NodeName = SceneName
					Tree_QuestMapPlot[index].m_AreaTbl = {}
					for i = 1, table.getn(areatbl) do
						table.insert(Tree_QuestMapPlot[index].m_AreaTbl,areatbl[i])
					end
				end
			end
		end
	end
	--------初始化按势力划分的任务记录节点树-------------------
	local forceindex = 0
	for ForceName,tbl in pairs(g_AreaMgr.m_ForceAreaTbl) do
		local areatbl = {}
		for i = 1, table.getn(tbl) do
			local AreaName = tbl[i]
			if g_AreaQuestMgr[AreaName] ~= nil then
				table.insert(areatbl,AreaName)
			end
		end
		if table.getn(areatbl) > 0 then
			forceindex = forceindex+1
			Tree_QuestForcePlot[forceindex] = {}
			Tree_QuestForcePlot[forceindex].m_NodeName = ForceName
			Tree_QuestForcePlot[forceindex].m_AreaTbl = {}
			for i = 1, table.getn(areatbl) do
				table.insert(Tree_QuestForcePlot[forceindex].m_AreaTbl,areatbl[i])
			end
		end
	end
end

function CreateQuestRecordWnd( Parent )
	local Wnd = CQuestRecordWnd:new()
	Wnd:CreateFromRes( "QuestRecordWnd", Parent )
	Wnd.m_ForcePlotBtn = Wnd:GetDlgChild("ForcePlotBtn")
	Wnd.m_MapPlotBtn = Wnd:GetDlgChild("MapPlotBtn")
	Wnd.m_QuestPlotMapTreeWnd = Wnd:GetDlgChild("QuestPlotMapTree")
	Wnd.m_QuestPlotForceTreeWnd = Wnd:GetDlgChild("QuestPlotForceTree")
	Wnd.m_QuestDiscretionText = Wnd:GetDlgChild("QuestDiscretionText")
	Wnd.m_QuestTargetText = Wnd:GetDlgChild("QuestTargetText")
	Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
	Wnd.m_QuestGiveUpBtn = Wnd:GetDlgChild("QuestGiveUp")
	Wnd.m_QuestShareBtn = Wnd:GetDlgChild("QuestShare")
	Wnd.m_QuestTrackBtn = Wnd:GetDlgChild("QuestTrack")
	Wnd.m_ExperienceNum = Wnd:GetDlgChild("ExperienceNum")
	Wnd.m_MoneyNum = Wnd:GetDlgChild("MoneyNum")
	Wnd.m_BindMoneyNum = Wnd:GetDlgChild("BindMoneyNum")
	Wnd.m_TicketNum = Wnd:GetDlgChild("YuanBaoNum")

	for i = 1, 4 do
		Wnd["m_Good"..i] = Wnd:GetDlgChild("Good"..i)
		Wnd["m_Select"..i] = Wnd:GetDlgChild("Select"..i)
		Wnd["m_Good"..i]:ShowWnd(false)
		Wnd["m_Select"..i]:ShowWnd(false)
	end

	Wnd.m_ForceNodeTbl = {}
	Wnd.m_MapNodeTbl = {}

	--所有的任务表(包括以完成的任务，执行中的任务，失败的任务)
	Wnd.m_QuestStateTbl = {}					--QuestStateTbl[任务名] = state

	--所有任务的部分细节内容
	Wnd.m_DoingQuestInfo = {}					--DoingQuestInfo[任务名][任务变量名].DoNum (DoNum代表当前数目  或 NeedNum代表最大数)
	
	Wnd.m_QuestRecordType = nil
	Wnd.m_SelectQuestName = nil

	--隐藏任务
	Wnd.g_QuestHide = {}							--g_QuestHide[任务名称]
	
	--跑环任务
	Wnd.m_LoopTbl = {}
	
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, 1)
	Wnd:ShowWnd(false)
	return Wnd
end

function ShowQuestByKey()
	--g_GameMain.m_QuestRecordWnd:InitQuestRecordWnd(nil)
end

function CQuestRecordWnd:InitQuestRecordWnd(questname)
	local PlayerCamp = g_MainPlayer:CppGetBirthCamp()
	if RecordCamp ~= PlayerCamp then
		InitAreaQuestTree(PlayerCamp)
	end

	if not self:IsShow() then
		if questname == nil then
			if self.m_QuestRecordType == nil then
				self.m_QuestRecordType = "Map"
				self:InitQuestPlotTree(self.m_MapNodeTbl,self.m_QuestPlotMapTreeWnd,Tree_QuestMapPlot)
				self:InitQuestPlotTree(self.m_ForceNodeTbl,self.m_QuestPlotForceTreeWnd,Tree_QuestForcePlot)
				self.m_QuestPlotMapTreeWnd:ShowWnd(true)
				self.m_QuestPlotForceTreeWnd:ShowWnd(false)
				self.m_QuestPlotMapTreeWnd:SetFocus()
			elseif self.m_QuestRecordType == "Force" then
				self.m_QuestPlotForceTreeWnd:ShowWnd(true)
				self.m_QuestPlotMapTreeWnd:ShowWnd(false)
				self.m_QuestPlotForceTreeWnd:SetFocus()
			elseif self.m_QuestRecordType == "Map" then
				self.m_QuestPlotMapTreeWnd:ShowWnd(true)
				self.m_QuestPlotForceTreeWnd:ShowWnd(false)
				self.m_QuestPlotMapTreeWnd:SetFocus()
			end
		else
			if self.m_QuestRecordType == nil then
				self.m_QuestRecordType = "Map"
				self:InitQuestPlotTree(self.m_MapNodeTbl,self.m_QuestPlotMapTreeWnd,Tree_QuestMapPlot)
				self:InitQuestPlotTree(self.m_ForceNodeTbl,self.m_QuestPlotForceTreeWnd,Tree_QuestForcePlot)
				self.m_QuestPlotMapTreeWnd:ShowWnd(true)
				self.m_QuestPlotMapTreeWnd:SetFocus()
			end
			if self.m_SelectQuestName ~= questname then
				self:SetSelectNode(questname)
			end
		end
		self:ShowWnd(true)
		self:SetFocus()
	else
		if questname == nil then
			self:ShowWnd(false)
		else
			if self.m_SelectQuestName ~= questname then
				if not self:SetSelectNode(questname) then
					self:ShowWnd(false)
				end
			else
				self:ShowWnd(false)
			end
		end
	end
end

function CQuestRecordWnd:SetSelectNodeByType(NodeTbl,TreeWnd,NodeName)
	for ParentName, tbl in pairs(NodeTbl) do
		local node = tbl[NodeName]
		if node then
			TreeWnd:ExpandNode(tbl["m_Node"],true)
			self:InitQuestInfo(NodeName)
			TreeWnd:ShowWnd(true)
			TreeWnd:SetFocus()
			TreeWnd:SelectNode(node,true)
			return true
		end
	end
	return false
end

function CQuestRecordWnd:SetSelectNode(NodeName)
	if self.m_QuestRecordType == "Map" then
		if self:SetSelectNodeByType(self.m_MapNodeTbl,self.m_QuestPlotMapTreeWnd,NodeName) then
			return true
		end
		if self:SetSelectNodeByType(self.m_ForceNodeTbl,self.m_QuestPlotForceTreeWnd,NodeName) then
			self.m_QuestRecordType = "Force"
			return true
		end
	elseif self.m_QuestRecordType == "Force" then
		if self:SetSelectNodeByType(self.m_ForceNodeTbl,self.m_QuestPlotForceTreeWnd,NodeName) then
			return true
		end
		if self:SetSelectNodeByType(self.m_MapNodeTbl,self.m_QuestPlotMapTreeWnd,NodeName) then
			self.m_QuestRecordType = "Map"
			return true
		end
	end
	return false
end

function CQuestRecordWnd:InsertToDoingNode(NodeTbl,TreeWnd,NodeName,AreaName,QuestName)
	if NodeTbl[NodeName] == nil then
		return
	end
	if NodeTbl[NodeName][QuestName] == nil then
		local parent_node = NodeTbl[NodeName]["m_Node"]
		local node = TreeWnd:InsertNode(parent_node,QuestName,nil,nil,nil,nil)
		TreeWnd:ExpandNode(node,true)
		NodeTbl[NodeName][QuestName] = node
	end
end

function CQuestRecordWnd:DeleteFromDoingNode(NodeTbl,TreeWnd,NodeName,AreaName,QuestName)
	if NodeTbl[NodeName] == nil then
		return
	end
	if NodeTbl[NodeName][QuestName] then
		TreeWnd:DeleteNode(NodeTbl[NodeName][QuestName])
		NodeTbl[NodeName][QuestName] = nil
	end
end

function CQuestRecordWnd:DeleteDoingQuestNode(NodeTbl,TreeWnd,QuestName,PlotTreeTbl)
	if not Quest_Common(QuestName, "所属区域") then
		return
	end
	local Keys = Quest_Common:GetKeys(QuestName, "所属区域")
	for n = 1, table.getn(Keys) do
		local AreaName = Quest_Common(QuestName, "所属区域", Keys[n])
		if AreaName and AreaName ~= "" then
			for i = 1, table.maxn(PlotTreeTbl) do
				if PlotTreeTbl[i] then
					local NodeName = PlotTreeTbl[i].m_NodeName
					local areatbl = PlotTreeTbl[i].m_AreaTbl
					for i = 1,table.getn(areatbl) do
						if areatbl[i] == AreaName then
							if self.m_SelectQuestName == QuestName then
								self:ClearAllText()
							end
							self:DeleteFromDoingNode(NodeTbl,TreeWnd,NodeName,AreaName,QuestName)
						end
					end
				end
			end
		end
	end
end

function CQuestRecordWnd:AddDoingQuestNode(NodeTbl,TreeWnd,QuestName,PlotTreeTbl)
	if not Quest_Common(QuestName, "所属区域") then
		return
	end
	local Keys = Quest_Common:GetKeys(QuestName, "所属区域")
	for n = 1, table.getn(Keys) do
		local AreaName = Quest_Common(QuestName, "所属区域", Keys[n])
		if AreaName and AreaName ~= "" then
			for i = 1, table.maxn(PlotTreeTbl) do
				if PlotTreeTbl[i] then
					local NodeName = PlotTreeTbl[i].m_NodeName
					local areatbl = PlotTreeTbl[i].m_AreaTbl
					for i = 1,table.getn(areatbl) do
						if areatbl[i] == AreaName then
							if self.m_SelectQuestName == QuestName then
								self:InitQuestInfo(QuestName)
							end
							self:InsertToDoingNode(NodeTbl,TreeWnd,NodeName,AreaName,QuestName)
						end
					end
				end
			end
		end
	end
end

function CQuestRecordWnd:GiveUpQuestDeleteNode(NodeTbl,TreeWnd,NodeName,AreaName,QuestName)
	if not Quest_Common(QuestName, "所属区域") then
		return
	end
	local Keys = Quest_Common:GetKeys(QuestName, "所属区域")
	for n = 1, table.getn(Keys) do
		local AreaName = Quest_Common(QuestName, "所属区域", Keys[n])
		if AreaName and AreaName ~= "" then
			for i = 1, table.maxn(PlotTreeTbl) do
				if PlotTreeTbl[i] then
					local NodeName = PlotTreeTbl[i].m_NodeName
					local areatbl = PlotTreeTbl[i].m_AreaTbl
					for i = 1,table.getn(areatbl) do
						if areatbl[i] == AreaName then
							if self.m_SelectQuestName == QuestName then
								self:ClearAllText()
							end
							self:DeleteFromDoingNode(NodeTbl,TreeWnd,NodeName,AreaName,QuestName)
						end
					end
				end
			end
		end
	end
end

function CQuestRecordWnd:DeleteQuestNode(Num,QuestName)
	if Num == 3 then
		self:DeleteDoingQuestNode(self.m_ForceNodeTbl,self.m_QuestPlotForceTreeWnd,QuestName,Tree_QuestForcePlot)
		self:DeleteDoingQuestNode(self.m_MapNodeTbl,self.m_QuestPlotMapTreeWnd,QuestName,Tree_QuestMapPlot)
	elseif Num == 2 then
		self:AddDoingQuestNode(self.m_ForceNodeTbl,self.m_QuestPlotForceTreeWnd,QuestName,Tree_QuestForcePlot)
		self:AddDoingQuestNode(self.m_MapNodeTbl,self.m_QuestPlotMapTreeWnd,QuestName,Tree_QuestMapPlot)
	else
		self:DeleteDoingQuestNode(self.m_ForceNodeTbl,self.m_QuestPlotForceTreeWnd,QuestName,Tree_QuestForcePlot)
		self:DeleteDoingQuestNode(self.m_MapNodeTbl,self.m_QuestPlotMapTreeWnd,QuestName,Tree_QuestMapPlot)
	end
end

function CQuestRecordWnd:GiveUpQuest(QuestName)
	if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
		GacGiveDoingQuest(QuestName)
	end
end

function CQuestRecordWnd:ShareQuest(QuestName)
	local TeamID = g_MainPlayer.m_Properties:GetTeamID()
	if TeamID == 0 then
		MsgClient(3007)
		return
	end

	if Quest_Common(QuestName, "能否共享") ~= 1 then
		MsgToConn(Conn,3004)
		return
	end

	if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
		Gac2Gas:ShareQuest(g_Conn, QuestName)
	else
		MsgClient(3006)
	end
end

function CQuestRecordWnd:ClearQuestPlotTree(NodeTbl)
	for i = 1,table.maxn(NodeTbl) do
		if NodeTbl[i] then
			self:ClearQuestPlotTree(NodeTbl[i])
		end
	end
	if NodeTbl["m_Node"] then
		self.m_QuestPlotTreeWnd:DeleteNode(NodeTbl["m_Node"])
	end
end

function CQuestRecordWnd:ClearAllText()
--print("CQuestRecordWnd:ClearAllText()")
	self.m_SelectQuestName = nil
	self.m_QuestDiscretionText:SetWndText("")
	self.m_QuestTargetText:SetWndText("")
	self.m_ExperienceNum:SetWndText("")
	self.m_MoneyNum:SetWndText("")
	self.m_BindMoneyNum:SetWndText("")
	self.m_TicketNum:SetWndText("")
	for i = 1, 4 do
		self["m_Good"..i]:SetWndText("")
		self["m_Good"..i]:ShowWnd(false)
		self["m_Select"..i]:SetWndText("")
		self["m_Select"..i]:ShowWnd(false)
	end
end

function CQuestRecordWnd:InitQuestPlotTree(NodeTbl,TreeWnd,TreeTbl)
	self:ClearAllText()
	for i = 1,table.maxn(TreeTbl) do
		if TreeTbl[i] then
			local NodeName = TreeTbl[i].m_NodeName
			local areatbl = TreeTbl[i].m_AreaTbl
			NodeTbl[NodeName] = {}
			local parent_node = TreeWnd:InsertNode(nil,NodeName,nil,nil,nil,nil)
			TreeWnd:ExpandNode(parent_node,true)
			NodeTbl[NodeName]["m_Node"] = parent_node
			for i = 1, table.getn(areatbl) do
				local AreaName = areatbl[i]
				for j = 1, table.getn(g_AreaQuestMgr[AreaName]) do
					local questname = g_AreaQuestMgr[AreaName][j]
					if NodeTbl[NodeName][questname] == nil
						and g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname]
					 	and( g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname].m_State == QuestState.init
						or g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname].m_State == QuestState.failure) then
						local quest_node = TreeWnd:InsertNode(parent_node,questname,nil,nil,nil,nil)
						TreeWnd:ExpandNode(quest_node,true)
--						print(NodeName,"进行中",AreaName,questname)
						NodeTbl[NodeName][questname] = quest_node
					end
				end
			end
		end
	end
end

--function RecordHavedHypeLink(str)
--	local tempstr = ""
--	for i = 1, string.len(str) do
--		local strchar = string.sub(str,i,i).."hypelink"
--		tempstr = tempstr..strchar
--	end
--	return tempstr
--end

function CQuestRecordWnd:InitQuestInfo(QuestName)
	self:ClearAllText()
	self.m_SelectQuestName = QuestName
	self.m_QuestHypeLink = {}
	local QuestNode = Quest_Common(QuestName)
	local descriptionstr = g_GetLanQuestInfo(QuestName.."任务描述")
	--*********************************************
	descriptionstr = g_QuestInfoTextTransfer(descriptionstr)
	--***********************************************
	self.m_QuestDiscretionText:SetWndText(descriptionstr)
	
	local HyperLink = QuestNode("触发任务方式", "HyperLink")
	local startername = QuestNode("触发任务方式", "Arg")
	local starterstr = GetStaticTextClient(8328)
	if HyperLink ~= "" then
		local LinkTbl = {}
		LinkTbl[1] = QuestPos_Common(HyperLink, "SceneName")
		LinkTbl[2] = QuestPos_Common(HyperLink, "PosX")
		LinkTbl[3] = QuestPos_Common(HyperLink, "PosY")
		LinkTbl[4] = QuestPos_Common(HyperLink, "Mark")
		if self.m_QuestHypeLink[startername] == nil then
			self.m_QuestHypeLink[startername] = LinkTbl
		end
		starterstr = starterstr..AutoTrackColorStr.."#l"..startername.."#l#W#r"
	else
		starterstr = starterstr..startername.."#r"
	end
	HyperLink = QuestNode("结束任务方式", "HyperLink")
	local endername = QuestNode("结束任务方式", "Arg")
	local enderstr = GetStaticTextClient(8329)
	if HyperLink ~= "" then
		local LinkTbl = {}
		LinkTbl[1] = QuestPos_Common(HyperLink, "SceneName")
		LinkTbl[2] = QuestPos_Common(HyperLink, "PosX")
		LinkTbl[3] = QuestPos_Common(HyperLink, "PosY")
		LinkTbl[4] = QuestPos_Common(HyperLink, "Mark")
		if self.m_QuestHypeLink[endername] == nil then
			self.m_QuestHypeLink[endername] = LinkTbl
		end
		enderstr = enderstr..AutoTrackColorStr.."#l"..endername.."#l#W#r"
	else
		enderstr = enderstr..endername.."#r"
	end
	local targetdecription = g_GetLanQuestInfo(QuestName.."任务目标描述")
	targetdecription = g_QuestInfoTextTransfer(targetdecription)
	local descrstr = GetStaticTextClient(8330)..targetdecription.."#r"

	local TempStr, TempQuetLink = g_SetKeyHypeLink(QuestName, descrstr, "")
	descrstr = TempStr
	for KeyName, Tbl in pairs(TempQuetLink) do
		if not self.m_QuestHypeLink[KeyName] then
			self.m_QuestHypeLink[KeyName] = Tbl
		end
	end

	local showstr = starterstr..enderstr..descrstr
	if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
		local str = ""
		if g_QuestNeedMgr[QuestName] then
			for varname ,infotbl in pairs(g_QuestNeedMgr[QuestName]) do
				local num = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName][varname].DoNum
				local TrackStr = infotbl.TrackInfo
				--if IsTable(TrackStr) then
				--	TrackStr = infotbl.TrackInfo[1]
				--end
				str = str.."--"..TrackStr .." "..num.."/"..infotbl.Num.."#r"
			end
		end
		local BuffQuestMgr = g_BuffQuestMgr[QuestName]
		if BuffQuestMgr then
			for buffname,Tbl in pairs(BuffQuestMgr) do
				local TrackInfo = Tbl.TrackInfo
				local NeedNum = Tbl.Num
				local num = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName][buffname].DoNum
				str = str .. "--" .. TrackInfo .." " .. num .."/" .. NeedNum .."#r"
			end
		end
		showstr = showstr..str
	end
	self.m_QuestTargetText:SetWndText(showstr)
	
	local ExperienceNum = QuestNode("经验奖励", "Subject")
	if ExperienceNum and ExperienceNum ~= "" then
		ExperienceNum = g_ReturnSentenceParse(ExperienceNum, QuestName)
		self.m_ExperienceNum:SetWndText(ExperienceNum)
	else
		self.m_ExperienceNum:SetWndText(0)
	end
	
	local MoneyStr = QuestNode("金钱奖励")
	if MoneyStr and MoneyStr ~= "" then
		local MoneyNum = g_ReturnSentenceParse(MoneyStr, QuestName)
		self.m_MoneyNum:SetWndText(MoneyNum)
	else
		self.m_MoneyNum:SetWndText(0)
	end
	
	local BindMoneyNum = QuestNode("绑定的金钱奖励")
	if BindMoneyNum and BindMoneyNum ~= "" then
		BindMoneyNum = g_ReturnSentenceParse(BindMoneyNum, QuestName)
		self.m_BindMoneyNum:SetWndText(BindMoneyNum)
	else
		self.m_BindMoneyNum:SetWndText(0)
	end
	
	local TicketNum = QuestNode("绑定的元宝奖励")
	if TicketNum and TicketNum ~= "" then
		TicketNum = g_ReturnSentenceParse(TicketNum, QuestName)
		self.m_TicketNum:SetWndText(TicketNum)
	else
		self.m_TicketNum:SetWndText(0)
	end
	
	if QuestNode("物品奖励") then
		local mustaward = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Subject")
		local selectaward = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Arg")
		for i = 1, 4 do
			if mustaward ~= nil and mustaward[i] then
				self["m_Good"..i]:ShowWnd(true)
				local itemtype = mustaward[i][1]
				local itemname = mustaward[i][2]
			  local sName = itemname
				if g_ItemInfoMgr:IsSoulPearl(itemtype) then
					sName = g_SoulPearlSentenceParse(sName, QuestName)
					itemname = g_ItemInfoMgr:GetSoulpearlInfo(sName)
				end
			 	local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"SmallIcon")
				g_LoadIconFromRes(SmallIcon, self["m_Good"..i], -1, IP_ENABLE, IP_ENABLE)
				self["m_Good"..i]:SetWndText(mustaward[i][3])
				--tooltips 显示 正常显示
				g_Tooltips:LoadIconFromRes(self["m_Good"..i],SmallIcon)
				g_SetItemRichToolTips(self["m_Good"..i],itemtype,sName,0)
			end
			if selectaward ~= nil and selectaward[i] then
				self["m_Select"..i]:ShowWnd(true)
				local itemtype = selectaward[i][1]
				local itemname = selectaward[i][2]
			    local sName = itemname
				if g_ItemInfoMgr:IsSoulPearl(itemtype) then
					itemname = g_ItemInfoMgr:GetSoulpearlInfo(itemname)
				end
			 	local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"SmallIcon")
				g_LoadIconFromRes(SmallIcon, self["m_Select"..i], -1, IP_ENABLE, IP_ENABLE)
				self["m_Select"..i]:SetWndText(selectaward[i][3])
				--tooltips 显示 正常显示
				g_Tooltips:LoadIconFromRes(self["m_Select"..i],SmallIcon)
				g_SetItemRichToolTips(self["m_Select"..i],itemtype,sName,0)
			end
		end
	end
end

function CQuestRecordWnd:InitQuestForcePlotTreeWnd()
	if self.m_QuestRecordType == "Force" then
		return
	end
	self:ClearAllText()
	self.m_QuestRecordType = "Force"
	self.m_QuestPlotForceTreeWnd:ShowWnd(true)
	self.m_QuestPlotMapTreeWnd:ShowWnd(false)
	self.m_QuestPlotForceTreeWnd:SetFocus()
end

function CQuestRecordWnd:InitQuestMapPlotTreeWnd()
	if self.m_QuestRecordType == "Map" then
		return
	end
	self:ClearAllText()
	self.m_QuestRecordType = "Map"
	self.m_QuestPlotMapTreeWnd:ShowWnd(true)
	self.m_QuestPlotForceTreeWnd:ShowWnd(false)
	self.m_QuestPlotMapTreeWnd:SetFocus()
end

--子窗口消息
function CQuestRecordWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK  then
		if Child == self.m_ForcePlotBtn then
			self:InitQuestForcePlotTreeWnd()
		elseif Child == self.m_MapPlotBtn then
			self:InitQuestMapPlotTreeWnd()
		elseif Child == self.m_QuestPlotForceTreeWnd then
			self.m_SelectQuestName = nil
			self.m_ChooseNode = self.m_QuestPlotForceTreeWnd:GetCurrentNode()
			if self.m_ChooseNode == 0 then
				return
			end
			local NodeName = self.m_QuestPlotForceTreeWnd:GetNodeText(self.m_ChooseNode)
			local Parent = self.m_QuestPlotForceTreeWnd:GetNodeParent(self.m_ChooseNode)
			if Parent == 0 then
				return
			end
			self.m_SelectQuestName = NodeName
			self:InitQuestInfo(NodeName)
		elseif Child == self.m_QuestPlotMapTreeWnd then
			self.m_SelectQuestName = nil
			self.m_ChooseNode = self.m_QuestPlotMapTreeWnd:GetCurrentNode()
			if self.m_ChooseNode == 0 then
				return
			end
			local NodeName = self.m_QuestPlotMapTreeWnd:GetNodeText(self.m_ChooseNode)
			local Parent = self.m_QuestPlotMapTreeWnd:GetNodeParent(self.m_ChooseNode)
			if Parent == 0 then
				return
			end
			self.m_SelectQuestName = NodeName
			self:InitQuestInfo(NodeName)
		elseif Child == self.m_QuestGiveUpBtn then
			if self.m_SelectQuestName ~= nil then
				self:GiveUpQuest(self.m_SelectQuestName)
			end
		elseif Child == self.m_QuestShareBtn then
			if self.m_SelectQuestName ~= nil then
				self:ShareQuest(self.m_SelectQuestName)
			end
		elseif Child == self.m_QuestTrackBtn then
			if self.m_SelectQuestName ~= nil
				and g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[self.m_SelectQuestName] then
				g_GameMain.m_QuestTraceBack:ClickTrackBtn(self.m_SelectQuestName)
			end
		elseif Child == self.m_CloseBtn then
			self:ShowWnd(false)
			--self:ShowWnd(false)
		else
			return
		end
	elseif uMsgID == RICH_CLICK then
		local value = self.m_QuestTargetText:GetChooseStr()
		local linktbl = self.m_QuestHypeLink[value]
		if linktbl then
--			if(g_GameMain.m_SceneName ~= linktbl[1]) then
--				MessageBox(g_App:GetRootWnd(),MsgBoxMsg(401), MB_BtnOK)
--			else
				PlayerAutoTrack(value,linktbl[1],linktbl[2],linktbl[3])
--			end
		end
	end
end
