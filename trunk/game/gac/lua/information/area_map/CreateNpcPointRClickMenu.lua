
local function GetPointQuestTbl(PointName)
	local tbl = {}
	local CanFinishTbl = g_GameMain.m_AreaInfoWnd.m_CanFinishQuestNpcTbl[PointName]
	local DoingTbl = g_GameMain.m_AreaInfoWnd.m_DoingQuestPointTbl[PointName]
	if CanFinishTbl then
		for QuestName , v in pairs(CanFinishTbl.m_QuestTbl) do
			table.insert(tbl,QuestName)
		end
	end
	
	if DoingTbl then
		for QuestName , v in pairs(DoingTbl.m_QuestTbl) do
			table.insert(tbl,QuestName)
		end
	end
	return tbl
end

local function MenuAutoTrack(Wnd)
	local PointName = Wnd.m_PointName
	local SceneName = Wnd.m_SceneName
	local PosTbl = Wnd.m_PosTbl
	PlayerAutoTrack(PointName,SceneName,PosTbl[1],PosTbl[2])
	--g_GameMain.m_AreaInfoWnd:AutoTrackToPoint(PointName,SceneName,PosTbl[1],PosTbl[2])
end

local function MenuCancel()
end

local function OnClickMenuMsg(func)
	func(g_GameMain.m_AreaInfoWnd.m_Menu.m_ParentWnd)
	g_GameMain.m_AreaInfoWnd.m_Menu:Destroy()
	g_GameMain.m_AreaInfoWnd.m_Menu = nil
end

local function QueryQuest(QuestName)
--	g_GameMain.m_QuestRecordWnd:InitQuestRecordWnd(QuestName)
	g_GameMain.m_HandBookWnd:InitHandBookWndByQuest(QuestName)
end

local function ShareQuest(QuestName)
	g_GameMain.m_QuestRecordWnd:ShareQuest(QuestName)
end

local function GiveUpQuest(QuestName)
	g_GameMain.m_QuestRecordWnd:GiveUpQuest(QuestName)
end

local function TrackQuest(QuestName)
	g_GameMain.m_QuestTraceBack:ClickTrackBtn(QuestName)
end

local function OnClickChildMenuMsg(func)
	func(g_GameMain.m_AreaInfoWnd.m_ChildMenu.m_QuestName)
	g_GameMain.m_AreaInfoWnd.m_ChildMenu:Destroy()
	g_GameMain.m_AreaInfoWnd.m_ChildMenu = nil	
end
	
local function OnMouseMoveToMsg(QuestName)
	if g_GameMain.m_AreaInfoWnd.m_ChildMenu then
		g_GameMain.m_AreaInfoWnd.m_ChildMenu:Destroy()
		g_GameMain.m_AreaInfoWnd.m_ChildMenu = nil
	end
	local ChildMenu = CMenu:new("TargetMenu",g_GameMain.m_AreaInfoWnd.m_Menu)
--	ChildMenu:InsertItem(GacMenuText(1901), OnClickChildMenuMsg, nil, false, false, nil, MenuAutoTrack )	--"自动寻路"
	ChildMenu:InsertItem(GacMenuText(2001), OnClickChildMenuMsg, nil, false, false, nil, QueryQuest )		--"查看任务"
	if not g_AllMercenaryQuestMgr[QuestName] then
		ChildMenu:InsertItem(GacMenuText(2002), OnClickChildMenuMsg, nil, false, false, nil, ShareQuest )		--"任务共享"
	end
	ChildMenu:InsertItem(GacMenuText(2003), OnClickChildMenuMsg, nil, false, false, nil, GiveUpQuest )		--"任务放弃"
	ChildMenu:InsertItem(GacMenuText(2004), OnClickChildMenuMsg, nil, false, false, nil, TrackQuest )		--"任务追踪"
	ChildMenu:InsertItem(GacMenuText(1), OnClickChildMenuMsg, nil, false, false, nil, MenuCancel )			--"取消"
	
	local Rect = g_GameMain.m_AreaInfoWnd.m_Menu:GetItemRect(1)
	ChildMenu:SetPos(Rect.right, Rect.top)
	ChildMenu.m_QuestName = QuestName
	g_GameMain.m_AreaInfoWnd.m_ChildMenu = ChildMenu
end

function CreateNpcPointRClickMenu(BtnWnd)
	local PointName = BtnWnd.m_PointName
	local QuestTbl = GetPointQuestTbl(PointName)
	if g_GameMain.m_AreaInfoWnd.m_Menu then
		g_GameMain.m_AreaInfoWnd.m_Menu:Destroy()
		g_GameMain.m_AreaInfoWnd.m_Menu = nil
	end
	local Menu = nil
	if table.getn(QuestTbl) > 0 then
		Menu = CMenu:new("TargetMenu",g_GameMain.m_AreaInfoWnd)
		--"自动寻路"
		Menu:InsertItem(GacMenuText(1901), OnClickMenuMsg, nil, false, false, nil, MenuAutoTrack )
		for i = 1, table.getn(QuestTbl) do
		--"任务:".. QuestTbl[i] .. " >>"
			Menu:InsertItem(GacMenuText(1902, g_GetLanQuestName(QuestTbl[i])), nil, nil, false, true, OnMouseMoveToMsg, QuestTbl[i] )
		end
		--"取消"
		Menu:InsertItem(GacMenuText(1), OnClickMenuMsg, nil, false, false, nil, MenuCancel )
	elseif g_GameMain.m_AreaInfoWnd.m_FunNpcHyperLinkTbl[PointName] 
		or g_GameMain.m_AreaInfoWnd.m_CanTakeQuestNpcTbl[PointName]
		or PointName == "邮箱" then
		Menu = CMenu:new("TargetMenu",g_GameMain.m_AreaInfoWnd)
		--"自动寻路"
		Menu:InsertItem(GacMenuText(1901),OnClickMenuMsg, nil, false, false, nil, MenuAutoTrack )
		--"取消"
		Menu:InsertItem(GacMenuText(1),OnClickMenuMsg, nil, false, false, nil, MenuCancel )
	end
	
	if Menu then
		local Rect = CFRect:new()
		BtnWnd:GetLogicRect(Rect)
		Menu:SetPos( Rect.left+20 , Rect.top )
		
		Menu.m_ParentWnd = BtnWnd
		g_GameMain.m_AreaInfoWnd.m_Menu = Menu
	end
	
end