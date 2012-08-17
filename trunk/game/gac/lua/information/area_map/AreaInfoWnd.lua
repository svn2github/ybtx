--区域系统窗口
--@@@@@@@@@@@这个文件不知道怎么写注释，后面接手的人千万别怪我啊
--gac_require "information/area_map/AreaSelectTypeListItemWnd"
--gac_require "information/area_map/NpcPointBtnWnd"
--gac_require "information/area_map/ScenePictureWnd"
--gac_require "information/area_map/TeammateBtnWnd"
--gac_require "information/area_map/TongmateBtnWnd"
gac_require "information/world_map/Camp1MapWnd"
gac_require "information/world_map/Camp2MapWnd"
gac_require "information/world_map/Camp3MapWnd"
gac_require "information/world_map/WarAreaMapWnd"
cfg_load "npc/NotFunNpcTypeMap"
lan_load "npc/Lan_NotFunNpcTypeMap"
cfg_load "npc/NpcClassify_Common"
cfg_load "fb_game/FbRobRes_Server"
lan_load "npc/Lan_NpcClassify_Common"
lan_load "npc/Lan_SubFunOnMiddleMap_Common"
gac_require "information/area_map/NpcOnMiddleMapExistTimeInc"
gac_require "world/npc/NpcCommFunc"

CAreaInfoWnd 	= class( SQRDialog )
local CTrackPosWnd = class(SQRDialog)

local function LoadSceneFile()
	for _, SceneName in pairs(Scene_Basic_Common_Shell:GetKeys()) do
		local sceneFile = Scene_Basic_Common_Shell(SceneName, "NpcSceneFile")
	--for _, v in pairs(Scene_Basic_Common_Shell) do
		if sceneFile ~= "" then
			cfg_require(sceneFile)
		end
	end
end

LoadSceneFile()

--45,145
--165,145，√，×，☆，★
--- @brief 创建小地图窗口
local MaxWndDir = 2*3.1415926
--NpcSelectTypeTbl = nil --{[0] = "标示所有", "任务Npc", "功能Npc", "玩法Npc", "佣兵团Npc", "商会Npc", "技能学习Npc", "邮箱"}
local ForceSelectTypeTbl = {[0] = "选择所有势力", "自然之友","碑林自治会"}
local bSetFocus = true
local NotSetFocus = false

local ForbidOpenAreaWnd = {
	[2] = true,
	[16] = true,
	[18] = true,
	[19] = true,
	[23] = true,
	[24] = true,
}

--升级需要修改pk开关
local function InitScenePictureWnd(SceneName)
	if not SceneArea_Common_Shell(SceneName) then
		LogErr("SceneArea_Common_Shell 表里该 SceneName不存在", SceneName)
	end
	
	local index = SceneArea_Common_Shell(SceneName, "PictureWndIndex")
	if index == nil or index == 0 then
		return
	end
	
	if not g_GameMain.m_AreaInfoWnd.m_SceneName or g_GameMain.m_AreaInfoWnd.m_SceneName ~= SceneName then
		g_GameMain.m_AreaInfoWnd.m_SceneName = SceneName
		g_GameMain.m_AreaInfoWnd:RemoveAllNpcPointBtn()
		if not g_GameMain.m_AreaInfoWnd.m_ScenePool[SceneName] then
			CScenePictureWnd:new(g_GameMain.m_AreaInfoWnd.m_SceneMapWnd, SceneName)
		else
			g_GameMain.m_AreaInfoWnd.m_SceneMapWnd["m_ScenePictureWnd"] = g_GameMain.m_AreaInfoWnd.m_ScenePool[SceneName]
		end
	end
	g_GameMain.m_AreaInfoWnd.m_SceneMapWnd["m_ScenePictureWnd"]:UpdateAreaQuestRecord(SceneName)

	local ScenePictureWnd = g_GameMain.m_AreaInfoWnd.m_SceneMapWnd["m_ScenePictureWnd"]
	ScenePictureWnd.m_FrogWnd.m_Frog:UpdateVisibleRegion()
	ScenePictureWnd:ShowWnd(true)
	ScenePictureWnd:SetFocus()
end

function CAreaInfoWnd:OpenAreaInfoWnd()
-- local Wnd = g_GameMain.m_AreaInfoWnd
	if ForbidOpenAreaWnd[g_GameMain.m_SceneType] then
		if not self:IsShow() then
			--这个类型的场景打不开区域面板
			MsgClient(3057)--佣兵在完成挑战任务时，应该不依赖地图就能独立完成探险
		else
			self.UseMkey = false
			self:CloseAreaInfoWnd()
		end
		return
	end
	if not self:IsShow() then
		self.UseMkey = true
		self:InitAreaInfoWnd(g_GameMain.m_SceneName)
		self:ShowWnd(true)
		if self.m_SceneMapWnd["m_ScenePictureWnd"] then
			self:CheckIsGetTeammatePos()
			self:CheckIsGetTongmatePos()
			self:CheckIsGetAllPlayerPos()
		end
--		self:SetFocus()
	else
		-- 如果已经用N键打开了世界地图
		if self.UseNkey then
			-- 如果用N打开区域面板，并返回到小地图，此时应该关掉区域面板
			if self.m_MapState == "Small" then
				self:CloseAreaInfoWnd()
				self.UseMkey = false
			else
				self:InitAreaInfoWnd(g_GameMain.m_SceneName)
				self.UseMkey = true
			end
			self.UseNkey = false

		else
			self.UseMkey = false
			self:CloseAreaInfoWnd()
		end
	end
end

-- 读取区域面板筛选选项
local function ReadAreaSelect()
	if not g_MainPlayer or not IsCppBound(g_MainPlayer) then
		return
	end
	local PlayerID = g_MainPlayer.m_uID
	local strpath = g_RootPath .. "var/gac/AreaSelect"..PlayerID..".txt"
	local f = CLuaIO_Open(strpath, "rb")
	local data = ""
	if f then
		data = f:ReadAll()
		f:Close()
	end
	--print("ReadAreaSelect>>>>data = "..data)
	if(data == "") then
		return
	end
	return data
end

local function GetMiddleMapBySceneName(SceneName)
	for MiddleMap, tbl in pairs(WorldMapInfo) do
		for _, scene in pairs( tbl ) do
			if SceneName == scene then
				return MiddleMap
			end
		end
	end
	local uCamp=g_MainPlayer:CppGetCamp()
	return "Camp"..uCamp
end

function CAreaInfoWnd:CheckIsGetTeammatePos()
	if IsCppBound(g_MainPlayer) then
		local TeamID = g_MainPlayer.m_Properties:GetTeamID()
		if TeamID == 0 then
			return
		end
		Gac2Gas:GetTeammatePos(g_Conn)
	end
end

function CAreaInfoWnd:CheckIsGetTongmatePos(tongId)
	if not IsCppBound(g_MainPlayer) then
		return
	end
  local TongID = 0
  if tongId then
   TongID = tongId
  else
		TongID = g_MainPlayer.m_Properties:GetTongID()
	end
	if TongID == 0 then
 		return
	end
	--print("向服务器查询")
	Gac2Gas:GetTongmatePos(g_Conn)
end

function CAreaInfoWnd:CheckIsGetAllPlayerPos()
	if not IsCppBound(g_MainPlayer) then
		return
	end
	if g_GameMain.m_SceneName == "鲜血试炼场"
		or g_GameMain.m_SceneName == "小pvp测试" then
		Gac2Gas:GetFbPlayerPos(g_Conn)
	end
end

function CAreaInfoWnd:ClickSceneBtnOpenAreaWnd(SceneName)
	self:UnDisplayTrackPosWnd()
	self:InitAreaInfoWnd(SceneName)
	self:ShowWnd(true)
	self:CheckIsGetTeammatePos()
	self:CheckIsGetTongmatePos()
	self:CheckIsGetAllPlayerPos()
	-- 某些刷怪器刷出来的NPC需要在中地图上显示
	Gac2Gas:GetBornNpcOnMiddleMap(g_Conn)
	Gac2Gas:GetFbBossOnMiddleMap(g_Conn)
--	self:SetFocus()
end

function CAreaInfoWnd:ClickAreaBtn(AreaName)
	self.m_LastOpenArea[self.m_SceneName] = AreaName
	self.m_LastShowQuest[self.m_SceneName] = nil
	self:InitAreaWndByArea(AreaName)
end

function CAreaInfoWnd:InitSelectTypeListItem(SceneName)
	local NpcSignBtnTbl = self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl
	if NpcSignBtnTbl then
		for PointName, WndTbl in pairs(NpcSignBtnTbl) do
			for i,Wnd in pairs(WndTbl) do
				Wnd:ShowWnd(false)
			end
		end
	end
	self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl = {}
end

local function CreateTrackPosWnd(Parent)
	local Wnd = CTrackPosWnd:new()
	Wnd:CreateFromRes("AreaTrackWnd", Parent )
	Wnd.m_TrackPic = Wnd:GetDlgChild("TrackPic")
	Wnd:SetStyle(0x60000000)
	Wnd:ShowWnd(false)
	return Wnd
end

--function CreateAreaInfoWnd(Parent)
function CAreaInfoWnd:Ctor()
	self:CreateFromRes("AreaInfoWnd", g_GameMain)

	self.m_ProcessState = self:GetDlgChild("ProcessState")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_ShowSceneInfoBtn = self:GetDlgChild("ShowSceneInfoBtn")
	self.m_BackBtn = self:GetDlgChild("BackBtn")
	self.m_ReturnWorldMapBtn = self:GetDlgChild("ReturnWorldMapBtn") --返回大地图按钮
	self.m_SceneMapWnd = self:GetDlgChild("AreaMap")
	CreateWorldMapBG(self.m_SceneMapWnd)
	--self.m_Camp1Map = CreateCamp1MapBG(self.m_SceneMapWnd)
	CreateCamp1MapBG(self.m_SceneMapWnd)
	CreateCamp2MapBG(self.m_SceneMapWnd)
	CreateCamp3MapBG(self.m_SceneMapWnd)
	CreateWarAreaMapBG(self.m_SceneMapWnd)

	self.m_SceneMapWnd:SetMouseOverDescAfter("")

	self.m_SelectNpcSignBtn = self:GetDlgChild("SelectNpcSignBtn")
	self.m_CloseSelectNpcBtn = self:GetDlgChild("CloseSelectNpcBtn")
	self.m_SelectNpcSignListCtrl = self:GetDlgChild("SelectNpcSignListCtrl")
	self.m_SelectNpcSignListCtrl:ShowWnd(false)
	self.m_NpcSignTypeSelectTbl = nil
	self.m_ForceSignTypeSelectTbl = nil
	self.m_TeammateWndTbl = {}
	self.m_TongmateWndTbl = {}
	self.m_BornNpcWndTbl = {}
	self.m_RobResWndTbl = {}
	self.m_FbPlayerWndTbl = {}

--	self.m_SelectForceSignBtn = self:GetDlgChild("SelectForceSignBtn")
--	self.m_CloseSelectForceBtn = self:GetDlgChild("CloseSelectForceBtn")
--	self.m_SelectForceSignListCtrl = self:GetDlgChild("SelectForceSignListCtrl")
--	self.m_SelectForceSignListCtrl:ShowWnd(false)

	self.m_ShowTrackWnd = self:GetDlgChild("A001")
	self.m_SelectShowTrackBtn = self:GetDlgChild("SelectShowTrackBtn")
	self.m_SelectShowTrackBtn:SetCheck(true)
	self.m_bSelectShowTrack = true

	self.m_SelectQuestName = nil
	self.m_SceneName = nil
	self.m_SelectAreaName = nil
	self.m_LastOpenArea = {}  --记录上一次打开面板的状态
	self.m_LastShowQuest = {}
	self.m_AreaLastState = {}  -- 记录上一次打开面板各个区域的PK状态
	self.m_PlayerAreaInfoTbl = {}
	self.m_TrackWndTbl = {}
	self.m_ClickPos = {}
	self.m_ScenePool = {}
	self.m_NpcFuncRecord = {}
	self.m_QuestNpcIcon = {}
	self.m_TrackBeginPos = 1
	self.m_TrackTotalLen = 0
	self.m_DeletQuestIcon = {}
	self.m_AllNpcIcon = {}
	--CreateScenePictureWnd(Wnd.m_SceneMapWnd)
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
--	self:SetFocus()
	self:InitTable()
end

function CAreaInfoWnd:InitTable()
	if not self.NpcSelectTypeTbl then
		self.NpcSelectTypeTbl = {}
		self.NpcSubMenu = {}
		
		for _, Index in pairs(Lan_NpcClassify_Common:GetKeys()) do
			self.NpcSelectTypeTbl[Index] = Lan_NpcClassify_Common(Index, "TypeName")
			self.NpcSubMenu[Index] = loadstring("return {" .. ( Lan_NpcClassify_Common(Index, "SubMenu") or "") .. "}")()	
		end
		
--		for Index, tbl in pairs(Lan_NpcClassify_Common) do
--			self.NpcSelectTypeTbl[Index] = tbl.TypeName
--			self.NpcSubMenu[Index] = loadstring("return {" .. (tbl.SubMenu or "") .. "}")()	
--		end
	end

	if not self.NpcTypeFunc then

		self.NpcTypeFunc = {}
		self.JobNpcFilter = {}
		
		for _, Index in pairs(NpcClassify_Common:GetKeys()) do
			-- 每个筛选选项包含的功能
			self.NpcTypeFunc[Index] = loadstring("return {" .. (NpcClassify_Common(Index, "NpcType") or "") .. "}")()	
			-- 每个职业中地图NPC筛选选项初始化
			self.JobNpcFilter[Index] = loadstring("return{" .. (NpcClassify_Common(Index, "InitJobSelect") or "") .. "}")()
		end		
		
		
--		for Index, tbl in pairs(NpcClassify_Common)do
--			-- 每个筛选选项包含的功能
--			self.NpcTypeFunc[Index] = loadstring("return {" .. (tbl.NpcType or "") .. "}")()	
--			-- 每个职业中地图NPC筛选选项初始化
--			self.JobNpcFilter[Index] = loadstring("return{" .. (tbl.InitJobSelect or "") .. "}")()		
--		end
	end
	
	self.m_NpcSignTypeSelectTbl = {}

end

local function CreateAreaSelectTypeListItem(Parent,str,itemwidth,WndType,Index)
	local Wnd = CAreaSelectTypeListItemWnd:new(Parent,str,itemwidth,WndType)
	Wnd:ShowWnd(true)
	
	-- 创建子菜单
	Wnd.m_SubMenuWnd = CAreaSubMenuWnd:new(Wnd, itemwidth, Index)
	return Wnd
end

function CAreaInfoWnd:InitSelectWnd()
	if not IsCppBound(g_MainPlayer) then
		return
	end
	local ZoomSize = self:GetRootZoomSize()
--	self.m_SelectNpcSignListCtrl:DeleteAllItem()
	local rt = CFRect:new()
	self.m_SceneMapWnd:GetLogicRect(rt)
--	local WndHeight = self.m_SelectNpcSignBtn:GetWndOrgHeight()
	local itemwidth = self.m_SelectNpcSignBtn:GetWndOrgWidth()+self.m_CloseSelectNpcBtn:GetWndOrgWidth()
	local width = self.m_SelectNpcSignListCtrl:GetWndOrgWidth()
	local height = self.m_SelectNpcSignListCtrl:GetWndOrgHeight()
--	self.m_SelectNpcSignListCtrl:ShowWnd(false)
--	local itemwidth = self.m_SelectNpcSignListCtrl:GetWndOrgWidth()
	if not self.m_isFirstTime then
		self.m_SelectNpcSignListCtrl:InsertColumn(0,itemwidth)
	end
	if not self.m_SelectNpcSignListCtrl.m_ItemTbl then
		self.m_SelectNpcSignListCtrl.m_ItemTbl = {}
	end

	if self.m_NpcSignTypeSelectTbl == nil then
		self.m_NpcSignTypeSelectTbl = {}
	end

	if not self.finalwndheight then
		self.finalwndheight = 0
	end
	for i = 0, #(self.NpcSelectTypeTbl) do
		if not self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[i]] then
			self.m_SelectNpcSignListCtrl:InsertItem(i, 22)
			local item = self.m_SelectNpcSignListCtrl:GetSubItem(i, 0 )
			self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[i]] = CreateAreaSelectTypeListItem(item, self.NpcSelectTypeTbl[i], itemwidth, "NpcSelect", i)
	
	--		if self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[i]] then
	--			self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[i]].m_SelectBtn:SetCheck(true)
	--		end
			local itemheight = self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[i]]:GetWndOrgHeight()
			self.m_SelectNpcSignListCtrl:SetItemHeight(itemheight, i)
			self.finalwndheight = self.finalwndheight+itemheight
		else
			self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[i]]:ShowWnd(true)
		end
	end
	self.m_SelectNpcSignListCtrl:SetWndHeight(self.finalwndheight * ZoomSize + 20)

	-- 读取上一次哪些被选中
	
	local AreaSelectRecord = ReadAreaSelect()
	
	local WordPos = 1
	if self.m_AreaSelectRecord and self.m_AreaSelectRecord == AreaSelectRecord then
		return
	end
	if AreaSelectRecord ~= nil then
		-- "大剑士"=1, "魔剑士"=2 ,"骑士"=3, "法师"=4, "邪魔"=5, "牧师"=6, "矮人骑士"=7, "精灵弓箭手"=8, "兽人战士"=9
		local JobSelType = {0,1,2,4,5,6,9}
		if AreaSelectRecord == "all" then
			for i = 0, #(self.NpcSelectTypeTbl) do
				if i ~= 0 and i ~= 1 then
				 	self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[i]] = true
				 	self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[i]].m_SelectBtn:SetCheck(true)
				end
				-- 选中子菜单
				for j, MenuName in pairs(self.NpcSubMenu[i]) do
				 	self.m_NpcSignTypeSelectTbl[MenuName] = true
				 	local ParentMenu = self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[i]]
				 	local IsShow = true
				 	if i == 1 then
				 		local JobNum = g_MainPlayer:CppGetClass()
					 	if JobSelType[j] == JobNum then
					 		IsShow = true
					 	else
					 		IsShow = false
					 	end
					 	ParentMenu.m_SubMenuWnd.m_SubSignListCtrl.m_SubItemTbl[self.NpcSubMenu[1][1]].m_SelectBtn:SetCheck(true)
					end
					
				 	ParentMenu.m_SubMenuWnd.m_SubSignListCtrl.m_SubItemTbl[MenuName].m_SelectBtn:SetCheck(IsShow)
				end
			end
			self.m_isFirstTime = true
			self.m_AreaSelectRecord = AreaSelectRecord
		else
			for i = 0, #(self.NpcSelectTypeTbl) do
				local Record = string.sub( AreaSelectRecord, WordPos, WordPos )
				WordPos = WordPos + 1
				if Record == "1" then
				 	self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[i]] = true
				 	self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[i]].m_SelectBtn:SetCheck(true)
				end

				-- 选中子菜单
				for _, MenuName in pairs(self.NpcSubMenu[i]) do
					local Record = string.sub( AreaSelectRecord, WordPos, WordPos )
					WordPos = WordPos + 1
					if Record == "y" then
					 	self.m_NpcSignTypeSelectTbl[MenuName] = true
					 	local ParentMenu = self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[i]]
					 	ParentMenu.m_SubMenuWnd.m_SubSignListCtrl.m_SubItemTbl[MenuName].m_SelectBtn:SetCheck(true)
					end
				end
			end
			self.m_isFirstTime = true
			self.m_AreaSelectRecord = AreaSelectRecord
		end
	-- 第一次打开中地图筛选 从配置表里读取初始值
	else
--		local SelectAll = Lan_NpcClassify_Common[0].TypeName			-- 标示所有
--		self.m_NpcSignTypeSelectTbl[SelectAll] = true
--		self.m_SelectNpcSignListCtrl.m_ItemTbl[SelectAll].m_SelectBtn:SetCheck(true)
--	InitJobSeeType_Common
		local Job = g_MainPlayer:CppGetClass()
		for Index, tbl in pairs(self.JobNpcFilter) do

			-- 如果InitJobSelect列没有填，则是false
			local IsAllSelect = nil
			if #(tbl) > 0 then
			  IsAllSelect = true
			else
				IsAllSelect = false
			end

			for num, JobTypeTbl in pairs(tbl) do

				local IsSelect = false
				for i, JobType in pairs(JobTypeTbl) do
					if JobType == Job then
						local MenuName = self.NpcSubMenu[Index][num]
						if MenuName then
							self.m_NpcSignTypeSelectTbl[MenuName] = true
							local ParentMenu = self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[Index]]
					 		ParentMenu.m_SubMenuWnd.m_SubSignListCtrl.m_SubItemTbl[MenuName].m_SelectBtn:SetCheck(true)
						end
						IsSelect = true
						break
					end
				end

				if not IsSelect then
					IsAllSelect = false
				end
			end

			-- 如果子菜单全部选中，主菜单也选中
			if IsAllSelect then
				self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[Index]] = true
			 	self.m_SelectNpcSignListCtrl.m_ItemTbl[self.NpcSelectTypeTbl[Index]].m_SelectBtn:SetCheck(true)
			end

		end
		self.m_isFirstTime = true
		self.m_AreaSelectRecord = AreaSelectRecord
	end

end

function CAreaInfoWnd:InitOneNpcWndPos(SceneName, PictureWnd, wndres, childwndres, PointName, NpcPosX, NpcPosY, strTexture, IsQuestNpc)
	local Wnd
	local NpcIndex = SceneName..PointName..NpcPosX..NpcPosY
	
	-- 任务图标是否需要重新创建
	local IsNeedRecreate
	if IsQuestNpc then
		if self.m_NpcFuncRecord[NpcIndex] ~= childwndres then
			self.m_NpcFuncRecord[NpcIndex] = childwndres
			IsNeedRecreate = true
		end
	end
	
	
	if not self.m_AllNpcIcon[NpcIndex] or IsNeedRecreate then
		Wnd = CNpcPointBtnWnd:new(PictureWnd, wndres, childwndres, strTexture)
		
		-- 隐藏旧的任务图标
		if IsQuestNpc and self.m_AllNpcIcon[NpcIndex] then
			self.m_AllNpcIcon[NpcIndex]:ShowWnd(false)
			self.m_AllNpcIcon[NpcIndex] = nil
		end

		self.m_AllNpcIcon[NpcIndex] = Wnd
	else
		Wnd = self.m_AllNpcIcon[NpcIndex]
		--Wnd:ShowWnd(true)
	end
	
	-- 如果是任务Wnd
	if IsQuestNpc then
	  self.m_QuestNpcIcon[NpcIndex] = true
	  self.m_DeletQuestIcon[NpcIndex] = false
--	  Wnd:ShowWnd(true)
	end
	if not Wnd.m_NpcSignBtn then
		LogErr("中的图错误", "SceneName:" .. SceneName .. ",PointName:" .. PointName)
	end
	--local Wnd = CNpcPointBtnWnd:new(g_GameMain.m_AreaInfoWnd, wndres, childwndres, strTexture)	--CreateNpcPointBtn(PictureWnd,wndres, childwndres, strTexture)
	Wnd.m_NpcSignBtn.m_PointName = PointName
	Wnd.m_NpcSignBtn.m_SceneName = SceneName
	Wnd.m_NpcSignBtn.m_PosTbl = {NpcPosX,NpcPosY}
	self:CalculateCharWndPos(Wnd,SceneName,PictureWnd,NpcPosX,NpcPosY)
	Wnd:ShowWnd(true)
--	Wnd:SetFocus()
	Wnd:SetStyle(0x40000000)
	return Wnd
end

function CAreaInfoWnd:InitAllTeammateWndPos()
	for MemberId, tbl in pairs(self.m_TeammateWndTbl) do
		self:InitTeammateWndPos(MemberId)
	end
end

function CAreaInfoWnd:InitAllTongmateWndPos()
	for MemberId, tbl in pairs(self.m_TongmateWndTbl) do
		self:InitTongmateWndPos(MemberId, tbl.m_MemberName)
	end
end

function CAreaInfoWnd:InitTeammateWndPos(TeammateId)
	local tbl = self.m_TeammateWndTbl[TeammateId]
	local tblTeamIcons = g_GameMain.m_tblTeamIcons
	if tbl.m_Wnd then
		tbl.m_Wnd:ShowWnd(false)
		tbl.m_Wnd = nil
	end
	if tbl.m_Pos[1] == self.m_SceneName then
		local PictureWnd = self.m_SceneMapWnd["m_ScenePictureWnd"]
		if not PictureWnd then
			return
			--LogErr("PictureWnd为空！", self.m_SceneName)
		end
		local Wnd = CTeammateBtnWnd:new(PictureWnd)--CreateTeammateBtnWnd(PictureWnd)
		Wnd.m_TeammateBtn.m_uCharID = TeammateId
		self:CalculateCharWndPos(Wnd,self.m_SceneName,PictureWnd,tbl.m_Pos[2],tbl.m_Pos[3])
		--Wnd:SetFocus()
		Wnd:SetStyle(0x40000000)
		tbl.m_Wnd = Wnd
		for i=1, #tblTeamIcons do
			if tblTeamIcons[i].m_uCharID == TeammateId then
				local sName = tblTeamIcons[i].m_sName
				local Tips = GetStaticTextClient(19011)..":#r"..sName.."#r("..tbl.m_Pos[2]..","..tbl.m_Pos[3]..")"
				Wnd.m_TeammateBtn:SetMouseOverDescAfter(Tips)
				break
			end
		end
	end
end

function CAreaInfoWnd:InitTongmateWndPos(TongmateId, MemberName)
	local tbl = self.m_TongmateWndTbl[TongmateId]
	tbl.m_MemberName = MemberName
	--local tblTeamIcons = g_GameMain.m_tblTeamIcons
	if tbl.m_Wnd then
		tbl.m_Wnd:ShowWnd(false)
		tbl.m_Wnd = nil
	end
	if tbl.m_Pos[1] == self.m_SceneName then
		local PictureWnd = self.m_SceneMapWnd["m_ScenePictureWnd"]
		if not PictureWnd then
			return
		end
		local Wnd = CTongmateBtnWnd:new()
		Wnd.m_TongmateBtn.m_uCharID = TongmateId
		self:CalculateCharWndPos(Wnd,self.m_SceneName,PictureWnd,tbl.m_Pos[2],tbl.m_Pos[3])
		--Wnd:SetFocus()
		Wnd:SetStyle(0x40000000)
		tbl.m_Wnd = Wnd
		--print("InitTongmateWndPos")
		if MemberName then
			local Tips = MemberName.."#r("..tbl.m_Pos[2]..","..tbl.m_Pos[3]..")"
			Wnd.m_TongmateBtn:SetMouseOverDescAfter(Tips)
		end
	end
end

function CAreaInfoWnd:InitFbPlayerWndPos(MemberId, MemberName, CampID, isLive)
	local tbl = self.m_FbPlayerWndTbl[MemberId]
	tbl.m_MemberName = MemberName
	--local tblTeamIcons = g_GameMain.m_tblTeamIcons
	if tbl.m_Wnd then
		tbl.m_Wnd:ShowWnd(false)
		tbl.m_Wnd = nil
	end
	if tbl.m_Pos[1] == self.m_SceneName then
		local PictureWnd = self.m_SceneMapWnd["m_ScenePictureWnd"]
		if not PictureWnd then
			return
		end
		local Wnd = CFbPlayerBtnWnd:new(PictureWnd, CampID, MemberId, isLive)
		Wnd.m_uCharID = MemberId
		self:CalculateCharWndPos(Wnd,self.m_SceneName,PictureWnd,tbl.m_Pos[2],tbl.m_Pos[3])
		--Wnd:SetFocus()
		Wnd:SetStyle(0x40000000)
		tbl.m_Wnd = Wnd
		--print("InitFbPlayerWndPos")
		if MemberName then
			local Tips = MemberName.."#r("..tbl.m_Pos[2]..","..tbl.m_Pos[3]..")"
			Wnd:SetMouseOverDescAfter(Tips)
		end
	end
end

function CAreaInfoWnd:AutoTrackToTeammate(TeammateId)
	local tbl = self.m_TeammateWndTbl[TeammateId]
	if not tbl then
		return
	end
	local GridX,GridY = tbl.m_Pos[2],tbl.m_Pos[3]
	PlayerAutoTrack("",self.m_SceneName,GridX,GridY)
end

function CAreaInfoWnd:AutoTrackToTongmate(TongmateId)
	local tbl = self.m_TongmateWndTbl[TongmateId]
	if not tbl then
		return
	end
	local GridX,GridY = tbl.m_Pos[2],tbl.m_Pos[3]
	PlayerAutoTrack("",self.m_SceneName,GridX,GridY)
end

function CAreaInfoWnd:AutoTrackToFbPlayer(MemberId)
	local tbl = self.m_FbPlayerWndTbl[MemberId]
	if not tbl then
		return
	end
	local GridX,GridY = tbl.m_Pos[2],tbl.m_Pos[3]
	PlayerAutoTrack("",self.m_SceneName,GridX,GridY)
end

function CAreaInfoWnd:TeammateOffline(MemberID)
	local tbl = self.m_TeammateWndTbl[MemberID]
	if not tbl then
		return
	end
	if tbl.m_Wnd then
		tbl.m_Wnd:ShowWnd(false)
		tbl.m_Wnd = nil
	end
	self.m_TeammateWndTbl[MemberID] = nil
end

function CAreaInfoWnd:TongmateOffline(MemberID)
	if self:IsShow() then
		local tbl = self.m_TongmateWndTbl[MemberID]
		if not tbl then
			return
		end
		if tbl.m_Wnd then
			tbl.m_Wnd:ShowWnd(false)
			tbl.m_Wnd = nil
		end
		self.m_TongmateWndTbl[MemberID] = nil
	end
end

function CAreaInfoWnd:DeleteTeammatePosWnd()
	for id, tbl in pairs(self.m_TeammateWndTbl) do
		if tbl.m_Wnd then
			tbl.m_Wnd:ShowWnd(false)
		end
	end
	self.m_TeammateWndTbl = {}
end

function CAreaInfoWnd:DeleteTongmatePosWnd()
	for id, tbl in pairs(self.m_TongmateWndTbl) do
		if tbl.m_Wnd then
			tbl.m_Wnd:ShowWnd(false)
		end
	end
	self.m_TongmateWndTbl = {}
end

function CAreaInfoWnd:DeleteFbPlayerPos()
	for id, tbl in pairs(self.m_FbPlayerWndTbl) do
		if tbl.m_Wnd then
			tbl.m_Wnd:ShowWnd(false)
		end
	end
	self.m_FbPlayerWndTbl = {}	
end

--function CAreaInfoWnd:AutoTrackToPoint(PointName)
--	local linktbl = nil
--	if self.m_CanFinishQuestNpcTbl[PointName] then
--		linktbl = self.m_CanFinishQuestNpcTbl[PointName].m_HypeLinkTbl
--	elseif self.m_CanTakeQuestNpcTbl[PointName] then
--		linktbl = self.m_CanTakeQuestNpcTbl[PointName].m_HypeLinkTbl
--	elseif self.m_DoingQuestPointTbl[PointName] then
--		linktbl = self.m_DoingQuestPointTbl[PointName].m_HypeLinkTbl
--	elseif self.m_FunNpcHyperLinkTbl[PointName] then
--		linktbl = self.m_FunNpcHyperLinkTbl[PointName]
--	-- 邮箱寻路
--	elseif self.m_MailBoxPointTbl[PointName] then
--		linktbl = self.m_MailBoxPointTbl[PointName]
--	end
--	if linktbl == nil then
--		return
--	end
----	if(g_GameMain.m_SceneName ~= linktbl[1]) then
----		MessageBox(g_App:GetRootWnd(), MsgBoxMsg(401), MB_BtnOK)
----	else
--		PlayerAutoTrack(PointName,linktbl[1],linktbl[2],linktbl[3])
----	end
--end

function CAreaInfoWnd:RemoveAllNpcPointBtn()
--	local SceneName = self.m_SceneName
	if not self.m_SceneMapWnd["m_ScenePictureWnd"] then
		return
	end

	local NpcSignBtnTbl = self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl
	if NpcSignBtnTbl then
		for PointName, WndTbl in pairs(NpcSignBtnTbl) do
			for i, Wnd in pairs(WndTbl) do
				Wnd:ShowWnd(false)
			end
		end
	end
	self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl = {}
end

function CAreaInfoWnd:PlayerMoveChangePlayerPosWnd()
	if not self:IsShow() then
		return
	end
	if g_GameMain.m_SceneName ~= self.m_SceneName then
		return
	end
	if SceneArea_Common_Shell(g_GameMain.m_SceneName) == nil then
		return
	end
	local PictureWnd = self.m_SceneMapWnd["m_ScenePictureWnd"]
	if not PictureWnd then
--		print("if not PictureWnd then")
		return
	end
	self:DisplayPlayerPosWnd(g_GameMain.m_SceneName,PictureWnd,NotSetFocus)
end

local function AddImage(wnd, strTexture)
	if strTexture then
		local Flag = IMAGE_PARAM:new()
		Flag.CtrlMask = SM_BS_BK
		Flag.StateMask = IP_ENABLE
		local DefaultImage = WND_IMAGE_LIST:new()
		DefaultImage:AddImage(wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(0,0), "0xffffffff", 0 )
		wnd:SetWndBkImage( Flag, DefaultImage )
	end
end

function CAreaInfoWnd:CalculateCharWndPos(Wnd,SceneName,PictureWnd,PosX,PosY)
	--场景的最大坐标
	local SceneMaxPos = GetCfgTransformValue(false, "SceneArea_Common_Shell", SceneName, "SceneMaxPos")
	local SceneWidth = SceneMaxPos[1]
	local SceneHeight = SceneMaxPos[2]
	--有效的地图图片在窗口中的位置
	local PictureMinPos = GetCfgTransformValue(false, "SceneArea_Common_Shell", SceneName, "PictureMinPos")
	local PictureMaxPos = GetCfgTransformValue(false, "SceneArea_Common_Shell", SceneName, "PictureMaxPos")
	--有效的地图图片在裁剪前的大小（正方形）
	local PictureWidthBeforeCut = SceneArea_Common_Shell(SceneName, "PictureWidthBeforeCut")
	--四个方向上裁减掉的数值
	local CutTop= SceneArea_Common_Shell(SceneName, "CutTop")
	local CutBottom = SceneArea_Common_Shell(SceneName, "CutBottom")
	local CutLeft = SceneArea_Common_Shell(SceneName, "CutLeft")
	local CutRight = SceneArea_Common_Shell(SceneName, "CutRight")

	local EffectivePictureHeight = (math.sqrt(2)*PictureWidthBeforeCut*SceneHeight)/(SceneWidth+SceneHeight)
	local EffectivePictureWidth = math.sqrt(2)*PictureWidthBeforeCut-EffectivePictureHeight
	local EffectivePosHeight = EffectivePictureHeight*(PosY/SceneHeight)
	local EffectivePosWidth = EffectivePictureWidth*(PosX/SceneWidth)

	local TransferPosHeight = nil
	local TransferPosWidth = nil
	if PosX > PosY then
		TransferPosHeight=math.sqrt(2)*EffectivePosHeight+(EffectivePosWidth-EffectivePosHeight)/math.sqrt(2)
		TransferPosWidth=EffectivePictureHeight/math.sqrt(2)+(EffectivePosWidth-EffectivePosHeight)/math.sqrt(2)
	else
		TransferPosHeight=math.sqrt(2)*EffectivePosWidth+(EffectivePosHeight-EffectivePosWidth)/math.sqrt(2)
		TransferPosWidth=EffectivePictureHeight/math.sqrt(2)-(EffectivePosHeight-EffectivePosWidth)/math.sqrt(2)
	end
	local FinalPosHeight = TransferPosHeight-CutBottom-PictureMinPos[2]
	local FinalPosWidth = TransferPosWidth-CutLeft+PictureMinPos[1]

	local WndWidth = Wnd:GetWndOrgWidth()
	local WndHeight = Wnd:GetWndOrgHeight()
	local rt = CFRect:new()

	if not PictureWnd then
		LogErr("PictureWnd为空！", SceneName)
	end

	PictureWnd:GetLogicRect(rt)
	local rt_1 = CFRect:new()
	rt_1.top = rt.bottom-FinalPosHeight-WndHeight/2
--	if rt_1.top%1 >= 0.5 then
--		rt_1.top = math.ceil(rt_1.top)
--	else
--		rt_1.top = math.floor(rt_1.top)
--	end
	rt_1.bottom = rt.bottom-FinalPosHeight+WndHeight/2
--	if rt_1.top%1 >= 0.5 then
--		rt_1.bottom = math.ceil(rt_1.bottom)
--	else
--		rt_1.bottom = math.floor(rt_1.bottom)
--	end
	rt_1.right = rt.left+FinalPosWidth+WndWidth/2
--	if rt_1.right%1 >= 0.5 then
--		rt_1.right = math.ceil(rt_1.right)
--	else
--		rt_1.right = math.floor(rt_1.right)
--	end
	rt_1.left = rt.left+FinalPosWidth-WndWidth/2
--	if rt_1.left%1 >= 0.5 then
--		rt_1.left = math.ceil(rt_1.left)
--	else
--		rt_1.left = math.floor(rt_1.left)
--	end
	Wnd:SetLogicRect(rt_1)
end

local function CreatePlayerPosWnd(Parent)
	local Wnd = CNpcPointBtnWnd:new(Parent, "AreaPlayerPosWnd")
	Wnd.m_PlayerSign = Wnd:GetDlgChild("PlayerPosWnd")
	Wnd:SetStyle(0x40000000)
	Wnd:ShowWnd(true)
	return Wnd
end

function CAreaInfoWnd:SetTrackWndFocus()
	for i = 1,table.getn(self.m_TrackWndTbl) do
		self.m_TrackWndTbl[i][1]:SetFocus()
	end
end

function CAreaInfoWnd:DisplayPlayerPosWnd(SceneName,PictureWnd,IsSetFocus)
	local posSelf = CFPos:new()
	g_MainPlayer:GetPixelPos( posSelf )

	local GridX = math.floor(posSelf.x/EUnits.eGridSpanForObj)
	local GridY = math.floor(posSelf.y/EUnits.eGridSpanForObj)
	for i = self.m_TrackBeginPos, self.m_TrackTotalLen do
		
		local TrackPos = self.m_TrackWndTbl[i][2]
		local X = math.floor(TrackPos.x/EUnits.eGridSpanForObj)
		local Y = math.floor(TrackPos.y/EUnits.eGridSpanForObj)
		if (X <= GridX+1 and X >= GridX-1) and (Y <= GridY+1 and Y >= GridY-1) then
			for j = 1, i do
				if self.m_TrackWndTbl[j][1] then
					self.m_TrackWndTbl[j][1]:ShowWnd(false)
				end
			end
--			for j = 1, i do
--				table.remove(self.m_TrackWndTbl,1)
--			end
			self.m_TrackBeginPos = i + 1
			break
		end
	end

	local PlayerPos = CPos:new()
	g_MainPlayer:GetGridPos(PlayerPos)

	local Wnd = nil
	if PictureWnd.m_PlayerPosWnd then
--		PictureWnd.m_PlayerPosWnd:ShowWnd(false)
		Wnd = PictureWnd.m_PlayerPosWnd
	else
		Wnd = CreatePlayerPosWnd(PictureWnd)
	end

	self:CalculateCharWndPos(Wnd,SceneName,PictureWnd,PlayerPos.x,PlayerPos.y)

	Wnd:ShowWnd(true)
	if IsSetFocus then
		--Wnd:SetFocus()
	end
	PictureWnd.m_PlayerPosWnd = Wnd
end


function CAreaInfoWnd:DelNpcFunTbl()
	if self.m_isFlag then
		self.m_isFlag = nil
	end
end

function CAreaInfoWnd:DelNpcFunLinkTbl(Name)
	if not self.m_isFlag then
		self.m_isFlag = {}
	end
	self.m_isFlag[Name] = true
end

function CAreaInfoWnd:ShowMiddleMapNpcSign(SceneName, PointTipsTbl, npcTypeTbl)
	for PointName, HyperLinkTbl in pairs(self.m_FunNpcHyperLinkTbl) do
		--print("PointName -- "..PointName)
		local _,strTexture = GetFuncTypeByNpcName(PointName)
		local FuncNameTbl = Npc_Common(PointName,"FuncName")
		local FuncName = "无"
		if FuncNameTbl ~= "" then
			FuncNameTbl = loadstring("return {" .. FuncNameTbl.."}")()

			if( type(FuncNameTbl[1]) == "table" ) then
				FuncNameTbl = FuncNameTbl[1]
			end

			FuncName = FuncNameTbl[1]
		end
		
		local npcType = tonumber(NotFunNpcTypeMap(FuncName, "NpcTypeInMiddleMapShow") )
		-- 如果贴图不存在，则不创建图标
		if strTexture and strTexture ~= "" then
			-- 24 为特殊NPC，当strTexture不为空时，特殊NPC为传送用NPC
			if (SceneName == HyperLinkTbl.m_SceneName and npcTypeTbl[npcType]) or npcType == 24 then
				local functbl = {}
				local Tips = ""
				--for i = 1,#(HyperLinkTbl[4]) do

				local IsBreak = nil
				for i = 1,#(HyperLinkTbl.m_FuncNameTbl) do
					local funcname = HyperLinkTbl.m_FuncNameTbl[i]
					local NotFunNpcType = NotFunNpcTypeMap(funcname)
					if NotFunNpcType and not functbl[funcname] then

--						if NotFunNpcType.JobCanSee and NotFunNpcType.JobCanSee ~= "" then
--							local JobCanSee = tonumber(NotFunNpcType.JobCanSee)
--							local Job = g_MainPlayer:CppGetClass()
--							if Job ~= JobCanSee then
--								-- 跳过这个NPC,这个NPC的图标对这个职业隐藏
--								IsBreak = true
--								break
--							end
--						end

						functbl[funcname] = true

						local NpcFuncName = nil
						local IconType = Npc_Common(PointName,"IconType")
						if IconType ~= "" and Lan_SubFunOnMiddleMap_Common(MemH64(IconType)) then
							NpcFuncName = Lan_SubFunOnMiddleMap_Common(MemH64(IconType), "DisplayName").."#r"							
						end
						if not NpcFuncName then
							NpcFuncName = Lan_NotFunNpcTypeMap(MemH64(funcname), "NpcShowDlg").."#r"
						end
						Tips = Tips..NpcFuncName

						-- ToolTips上具体显示出这个NPC有哪些活动
						if funcname == "日常活动报名处" then
							Gac2Gas:GetMatchGameNpcTips(g_Conn, PointName)
						end

					end
				end

				--if not IsBreak then
				if self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] == nil then
					PointTipsTbl[PointName] = {}
					PointTipsTbl[PointName].m_PosTbl = {}
					PointTipsTbl[PointName].m_Str = Tips
					--PointTipsTbl[PointName].m_Pos = {math.floor(HyperLinkTbl[2]),math.floor(HyperLinkTbl[3])}
					self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] = {}
					--self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] = Wnd
				else
					PointTipsTbl[PointName].m_Str = PointTipsTbl[PointName].m_Str..Tips
				end

				for i = 1,#(HyperLinkTbl.m_PosTbl) do
					local posx,posy = HyperLinkTbl.m_PosTbl[i][1],HyperLinkTbl.m_PosTbl[i][2]
					--local npcType = GetFuncTypeByNpcName(PointName)
					local Wnd = nil
					Wnd = self:InitOneNpcWndPos(SceneName,self.m_SceneMapWnd["m_ScenePictureWnd"],"NpcMiddleMapSign", "MiddleMapNpcBtn",PointName,posx,posy,strTexture)
					
					if Wnd then
						if self.m_isFlag and self.m_isFlag[PointName] then
							Wnd:ShowWnd(false)
						end
						table.insert(PointTipsTbl[PointName].m_PosTbl,{posx,posy})
						table.insert(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName],Wnd)
					end
				end				
				--end
			end
		end
	end
end

function CAreaInfoWnd:DisplayAllCharPointBtn(SceneName)
	local PictureWnd = self.m_SceneMapWnd["m_ScenePictureWnd"]
--	local width = PictureWnd:GetWndOrgWidth()
--	local height = PictureWnd:GetWndOrgHeight()
--	local SceneMaxPos = GetCfgTransformValue(false, "SceneArea_Common_Shell", SceneName, "SceneMaxPos")
--	local SceneWidth = SceneMaxPos[1]
--	local SceneHeight = SceneMaxPos[2]
	self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl = {}
	self.PointTipsTbl = {}
	local PointTipsTbl = self.PointTipsTbl

	-- 筛选任务
	local QuestFuncNum = NotFunNpcTypeMap("任务Npc", "NpcTypeInMiddleMapShow")
	local QuestType = nil	 -- 任务属于哪个种类
	local QuestWndText = nil -- 任务按钮上的字
	for Index, tbl in pairs(self.NpcTypeFunc) do
		for ItemNum, FuncNumTbl in pairs(tbl) do
			for _, FuncNum in pairs(FuncNumTbl) do
				if QuestFuncNum == FuncNum then
					QuestType = Index
					QuestWndText = self.NpcSubMenu[Index][ItemNum]
				end
			end
		end
		if QuestType then
			break
		end
	end
	if self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[QuestType]]
		or self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[0]]
		or self.m_NpcSignTypeSelectTbl[QuestWndText]
		then
--	if self.m_NpcSignTypeSelectTbl["任务Npc"] or self.m_NpcSignTypeSelectTbl["标示所有"] then
--	if SelectType == nil or SelectType == "任务Npc" or SelectType == "标示所有" then
		for PointName, Tbl in pairs(self.m_CanFinishQuestNpcTbl) do
			local HyperLinkTbl = Tbl.m_HypeLinkTbl
			if SceneName == HyperLinkTbl[1] then
				local ShowQuestName = nil
				local index = 0
				local Tips = GetStaticTextClient(19012)..":#r"
				for questname, _ in pairs(Tbl.m_QuestTbl) do
					ShowQuestName = questname
					Tips = Tips.. g_GetLanQuestName(questname) .."#r"
					index = index + 1
					if index > 5 then
						Tips = Tips.."...#r"
						break
					end
				end
				local strParentWnd = "NpcSubmitQuestSign"
				local strChildWnd = "SubmitQuestNpcBtn"
--				if ShowQuestName then
--					if g_AllMercenaryQuestMgr[ShowQuestName] then
--						strParentWnd = "NpcSubmitQuestSign_Yb"
--						strChildWnd = "SubmitQuestNpcBtn"
--					elseif g_HideQuestMgr[ShowQuestName] then
--						strParentWnd = "NpcSubmitQuestSign_Hide"
--						strChildWnd = "SubmitQuestNpcBtn"
--					end
--				end
				local posx,posy = HyperLinkTbl[2],HyperLinkTbl[3]
				local Wnd = self:InitOneNpcWndPos(SceneName,PictureWnd,strParentWnd,strChildWnd,PointName,posx,posy, nil, true)
  			PointTipsTbl[PointName] = {}
  			PointTipsTbl[PointName].m_Str = Tips
  			PointTipsTbl[PointName].m_PosTbl = {}
  			Wnd.m_bQuestPoint = true
  			--self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] = Wnd

  			table.insert(PointTipsTbl[PointName].m_PosTbl,{posx,posy})
  			self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] = {}
				table.insert(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName],Wnd)
  		end
  	end
  	for PointName, Tbl in pairs(self.m_CanTakeQuestNpcTbl) do
  		if not string.find(PointName, "日常任务发布榜") then
  			-- 如果这个NPC身上有玩家完成的任务
  			if not self.m_CanFinishQuestNpcTbl[PointName] then
  			  local HyperLinkTbl = Tbl.m_HypeLinkTbl
		  		if SceneName == HyperLinkTbl[1] then
		  			local index = 0
		  			local ShowQuestName = nil
		  			local Tips = GetStaticTextClient(19013)..":#r"
		  			for questname, _ in pairs(Tbl.m_QuestTbl) do
		  				ShowQuestName = questname
		  				Tips = Tips.. g_GetLanQuestName(questname) .."#r"
		  				index = index + 1
							if index > 5 then
								Tips = Tips.."...#r"
								break
							end
		  			end
		  			local strParentWnd = "NpcTakeQuestSign"
						local strChildWnd = "TakeQuestNpcBtn"
						if ShowQuestName then
							if g_AllMercenaryQuestMgr[ShowQuestName] then
							--	strParentWnd = "NpcTakeQuestSign_Yb"
							--	strChildWnd = "TakeQuestNpcBtn"
								Tips = ""--佣兵任务因为是随机的,所以不用显示任务列表
							--elseif g_HideQuestMgr[ShowQuestName] then
							--	strParentWnd = "NpcTakeQuestSign_Hide"
							--	strChildWnd = "TakeQuestNpcBtn"
							end
						end
		  			local posx,posy = HyperLinkTbl[2],HyperLinkTbl[3]
						local Wnd = self:InitOneNpcWndPos(SceneName,PictureWnd,strParentWnd,strChildWnd,PointName,posx,posy,nil,true)
		  			if self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] == nil then
		  				PointTipsTbl[PointName] = {}
		  				PointTipsTbl[PointName].m_Str = Tips
		  				PointTipsTbl[PointName].m_PosTbl = {}
		  				Wnd.m_bQuestPoint = true
		  				self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] = {}
		  				--self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] = Wnd
		  			else
		  				PointTipsTbl[PointName].m_Str = PointTipsTbl[PointName].m_Str..Tips
		  			end
		  			table.insert(PointTipsTbl[PointName].m_PosTbl,{posx,posy})
						table.insert(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName],Wnd)
		  		end
  			end
	  	end
  	end
  	for PointName, Tbl in pairs(self.m_DoingQuestPointTbl) do
  		local HyperLinkTbl = Tbl.m_HypeLinkTbl
  		if SceneName == HyperLinkTbl[1] then
  			local Tips = GetStaticTextClient(19014)..":#r"
  			for questname, _ in pairs(Tbl.m_QuestTbl) do
  				Tips = Tips.. g_GetLanQuestName(questname) .."#r"
  			end
  			local posx,posy = HyperLinkTbl[2],HyperLinkTbl[3]
  			local Wnd = self:InitOneNpcWndPos(SceneName,PictureWnd,"NpcDoingQuestSign", "DoingQuestNpcBtn",PointName,posx,posy,nil,true)
  			if self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] == nil then
  				PointTipsTbl[PointName] = {}
  				PointTipsTbl[PointName].m_Str = Tips
  				PointTipsTbl[PointName].m_PosTbl = {}
  				self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] = {}
  				--self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] = Wnd
  			else
  				PointTipsTbl[PointName].m_Str = PointTipsTbl[PointName].m_Str..Tips
  			end
  			table.insert(PointTipsTbl[PointName].m_PosTbl,{posx,posy})
				table.insert(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName],Wnd)
  		end
		end

		-- 日常任务发布榜 一直显示
		local MercenaryQuestPoolWnd = CQuestPoolWnd.GetWnd()
		if self.m_MercQuestPointTbl ~= nil and next(self.m_MercQuestPointTbl)
			and not g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn
			and MercenaryQuestPoolWnd.m_IsShowMercQuest then

			-- 中地图显示日常任务发布榜
			local MercQuestStr = nil
			if self.m_SceneName == "艾米帝国树屋酒吧" then
				MercQuestStr = "艾米佣兵日常任务发布榜"
			elseif self.m_SceneName == "修斯帝国树屋酒吧" then
				MercQuestStr = "修斯佣兵日常任务发布榜"
			elseif self.m_SceneName == "神圣教廷树屋酒吧" then
				MercQuestStr = "神圣佣兵日常任务发布榜"
			end

			if MercQuestStr then
				PointTipsTbl[MercQuestStr] = {}
				PointTipsTbl[MercQuestStr].m_PosTbl = {}
				PointTipsTbl[MercQuestStr].m_Str = ""
				--PointTipsTbl["日常任务发布榜"].m_Pos = {math.floor(HyperLinkTbl[2]),math.floor(HyperLinkTbl[3])}
				self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[MercQuestStr] = {}
				--self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[MercQuestStr] = Wnd
				for i=1,#(self.m_MercQuestPointTbl.m_PosTbl) do
					local posx,posy = self.m_MercQuestPointTbl.m_PosTbl[i][1],self.m_MercQuestPointTbl.m_PosTbl[i][2]
					local Wnd = self:InitOneNpcWndPos(SceneName,PictureWnd,"NpcTakeQuestSign", "TakeQuestNpcBtn",MercQuestStr,posx,posy,nil,true)
					table.insert(PointTipsTbl[MercQuestStr].m_PosTbl,{posx,posy})
					table.insert(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[MercQuestStr],Wnd)
				end
			end
		end
	end

	-- 功能NPC
	local npcTypeTbl = {}
	for Index, tbl in pairs(self.NpcTypeFunc) do
		if not self.m_SelectAreaName and (self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[Index]]) then  -- 主菜单选中
			for _, NumTbl in pairs(tbl) do
				for _, num in pairs(NumTbl) do
					npcTypeTbl[num] = true
				end
			end
		else 			-- 判断子菜单是否选中
			for i, SubMenuName in pairs(self.NpcSubMenu[Index]) do
				if not self.m_SelectAreaName and (self.m_NpcSignTypeSelectTbl[SubMenuName]) then
					if tbl[i] then
						for _, num in pairs(tbl[i]) do
							npcTypeTbl[num] = true
						end
					end
				end
			end
		end
	end
	self:ShowMiddleMapNpcSign(SceneName, PointTipsTbl, npcTypeTbl)

	-- 筛选邮箱
	local MailBoxFuncNum = NotFunNpcTypeMap("邮箱", "NpcTypeInMiddleMapShow")
	local MailBoxType = nil	 -- 邮箱属于哪个种类
	local MailBoxWndText = nil  -- 邮箱按钮上的字
	for Index, tbl in pairs(self.NpcTypeFunc) do
		for ItemNum, FuncNumTbl in pairs(tbl) do
			for _, FuncNum in pairs(FuncNumTbl) do
				if MailBoxFuncNum == FuncNum then
					MailBoxType = Index
					MailBoxWndText = self.NpcSubMenu[Index][ItemNum]
				end
			end
		end
		if MailBoxType then
			break
		end
	end
	
	if self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[MailBoxType]]-- 主菜单， TypeName[NpcSelectTypeTbl]
		or self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[0]]
		or self.m_NpcSignTypeSelectTbl[MailBoxWndText] -- 子菜单
		then
	--if self.m_NpcSignTypeSelectTbl["邮箱"] or self.m_NpcSignTypeSelectTbl["标示所有"] then
		if self.m_MailBoxPointTbl ~= nil and next(self.m_MailBoxPointTbl) then

			-- 中地图显示邮箱
			PointTipsTbl["邮箱"] = {}
			PointTipsTbl["邮箱"].m_PosTbl = {}
			PointTipsTbl["邮箱"].m_Str = ""
			--PointTipsTbl["邮箱"].m_Pos = {math.floor(HyperLinkTbl[2]),math.floor(HyperLinkTbl[3])}
			self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl["邮箱"] = {}
			--self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl["邮箱"] = Wnd
			for i=1,#(self.m_MailBoxPointTbl.m_PosTbl) do
				local posx,posy = self.m_MailBoxPointTbl.m_PosTbl[i][1],self.m_MailBoxPointTbl.m_PosTbl[i][2]
				local Wnd = self:InitOneNpcWndPos(SceneName,PictureWnd,"ObjMailBoxSign", "MailBoxObjBtn","邮箱",posx,posy,nil,false)
				table.insert(PointTipsTbl["邮箱"].m_PosTbl,{posx,posy})
				table.insert(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl["邮箱"],Wnd)
			end
		end
	end -- end if
	
	--抢夺资源点
	local RobResFuncNum = NotFunNpcTypeMap("资源点", "NpcTypeInMiddleMapShow")
	local RobResType = nil	 -- 邮箱属于哪个种类
	local RobResText = nil  -- 邮箱按钮上的字
	for Index, tbl in pairs(self.NpcTypeFunc) do

		for ItemNum, FuncNumTbl in pairs(tbl) do
			for _, FuncNum in pairs(FuncNumTbl) do
				if RobResFuncNum == FuncNum then
					RobResType = Index
					RobResText = self.NpcSubMenu[Index][ItemNum]
				end
			end
		end
		if RobResType then
			break
		end
	end

--	if self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[RobResType]]-- 主菜单， TypeName[NpcSelectTypeTbl]
--		or self.m_NpcSignTypeSelectTbl[self.NpcSelectTypeTbl[0]]
--		or self.m_NpcSignTypeSelectTbl[RobResText] -- 子菜单
--		then
	if self.m_RobResPointTbl ~= nil and next(self.m_RobResPointTbl) then
		for objName, posInfo in pairs(self.m_RobResPointTbl.m_PosTbl) do
			
			-- 中地图显示资源点
			PointTipsTbl[objName] = {}
			PointTipsTbl[objName].m_PosTbl = {}
			PointTipsTbl[objName].m_Str = ""
			self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[objName] = {}
			for i=1,#(posInfo) do
				local posx,posy = posInfo[i][1],posInfo[i][2]
				local Wnd = self:InitOneNpcWndPos(SceneName,PictureWnd,"RobResSign", "RobResObjBtn","资源点",posx,posy,nil,false)
				local TexSrc = g_ImageMgr:GetImagePath(40000)
				AddImage(Wnd, TexSrc)
				table.insert(PointTipsTbl[objName].m_PosTbl,{posx,posy})
				table.insert(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[objName],Wnd)
			end
		end
	end
	
	-- 某些刷怪器刷出来的NPC需要在中地图上显示
	Gac2Gas:GetBornNpcOnMiddleMap(g_Conn)
	Gac2Gas:GetFbBossOnMiddleMap(g_Conn)
	-- 传送Trap坐标
	local EndScenePosX = nil
	local EndScenePosY = nil
	if g_BetweenSceneTransportTrapPos[SceneName] then
		local BeginPosX = nil
		local BeginPosY = nil 
		local Texture = g_ImageMgr:GetImagePath(1356)
		for EndScene, TrapPosTbl in pairs(g_BetweenSceneTransportTrapPos[SceneName]) do
			for i, Tbl in pairs(TrapPosTbl) do
				BeginPosX = Tbl[1][1]
				BeginPosY = Tbl[1][2]
				EndScenePosX = Tbl[2][1]
				EndScenePosY = Tbl[2][2]
				local QuestName = Tbl[1]["ConditionTbl"]["Quest"]
				if QuestName == "" then
					--print("PosX:"..PosX.."--PosY:"..PosY)
					local PointName = GetStaticTextClient(19015)..GetSceneDispalyName(EndScene)
					if not PointTipsTbl[PointName] then
						PointTipsTbl[PointName] = {}
						PointTipsTbl[PointName].m_PosTbl = {}
						PointTipsTbl[PointName].m_EndPosTbl = {}
						PointTipsTbl[PointName].m_Str = ""
						self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName] = {}
					end
					table.insert(PointTipsTbl[PointName].m_PosTbl, {BeginPosX, BeginPosY})
					table.insert(PointTipsTbl[PointName].m_EndPosTbl, {EndScenePosX, EndScenePosY})	-- 记录Trap传送的位置
					PointTipsTbl[PointName].m_EndSceneName = EndScene
					
					local Wnd = self:InitOneNpcWndPos(SceneName, PictureWnd, "NpcMiddleMapSign", "MiddleMapNpcBtn", PointName, BeginPosX, BeginPosY, Texture, false)
					Wnd.m_IsTrap = true
					Wnd.m_EndSceneName = EndScene
					Wnd.m_EndScenePosX = EndScenePosX
					Wnd.m_EndScenePosY = EndScenePosY
		
					table.insert(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName], Wnd)
				end
			end
		end
	end

	-- 显示ToolTips
	self:ShowWndToolTips()

	if SceneName == g_GameMain.m_SceneName then
		self:DisplayPlayerPosWnd(SceneName,PictureWnd,NotSetFocus)
	elseif PictureWnd.m_PlayerPosWnd then
		PictureWnd.m_PlayerPosWnd:ShowWnd(false)
	end
	
	-- 删除没用的任务图标
	for Index, _ in pairs(self.m_QuestNpcIcon) do
		if self.m_DeletQuestIcon[Index] then
			self.m_QuestNpcIcon[Index] = nil
			self.m_AllNpcIcon[Index] = nil
		end
	end

end

function CAreaInfoWnd:ShowWndToolTips()
	for PointName, PointTbl in pairs(self.PointTipsTbl) do
		--local ShowName =  GetNpcTitleName(PointName)
		local ShowName = nil
		if IntObj_Common(PointName) then
			ShowName = GetIntObjDispalyName(PointName)
		elseif Npc_Common(PointName) then
			ShowName = GetNpcDisplayName(PointName)
		else
			ShowName = PointName
		end

		for i = 1, #(PointTbl.m_PosTbl) do
			local Wnd = self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName][i]
			local Tips = PointTbl.m_Str..ShowName.."("..PointTbl.m_PosTbl[i][1]..","..PointTbl.m_PosTbl[i][2]..")"
			Wnd.m_Tips = Tips
			Wnd.m_ShowName = ShowName
			--Wnd.m_NpcSignBtn:SetMouseOverDescAfter(Tips)
		end
	end

	for PointName, PointTbl in pairs(self.PointTipsTbl) do
		for i = 1, #(PointTbl.m_PosTbl) do
			local Wnd = self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName][i]
			-- 获取周围NPC的Tips
			Wnd.m_AroundTips = self:GetAroundNpcTips(Wnd)
			Wnd.m_NpcSignBtn:SetMouseOverDescAfter(Wnd.m_AroundTips)
			Wnd:SetMouseOverDescAfter(Wnd.m_AroundTips)
		end
	end
end



local function RectCollisionTest(rect1, rect2)
	local top1 		= rect1.top + 8
	local bottom1 = rect1.bottom - 8
	local left1 	= rect1.left + 8
	local right1  = rect1.right - 8
	local top2 		= rect2.top + 8
	local bottom2	= rect2.bottom - 8
	local left2   = rect2.left + 8
	local right2  = rect2.right - 8
	if top1 <= bottom2
		and bottom1 >= top2
		and left1 <= right2
		and right1 >= left2 then
		return true
	end
end

local function UpdataNearWndTips(TheWnd, Wnd)
	if not Wnd.m_AroundTips then
		return
	end
	-- RPC消息过来了，更新周围NPC的TIPS
	local NewTips = ""
	local FncName = Lan_NotFunNpcTypeMap(MemH64("日常活动报名处"), "NpcShowDlg")
	local StartPos, EndPos
	local Pos = 1
	StartPos, EndPos = string.find(Wnd.m_AroundTips, FncName, Pos)
	while EndPos do
		local ParaBegin, ParaEnd = string.find(Wnd.m_AroundTips, "%b()", EndPos)
		local Line = string.sub(Wnd.m_AroundTips, Pos, ParaEnd) -- "商店#r奇兵奖章兑换人%(454,354%)#r玩法报名#r佣兵波尔多(11,11)"
		local BeginPos =  string.find(Line, TheWnd.m_ShowName)
		if BeginPos then			-- 找到TheWnd的NPC名字
			local FucBegin, FucEnd = string.find(Line, FncName)
			local NoNameLine = string.sub(Line, 1, FucEnd)		-- 去掉名字 "商店#r奇兵奖章兑换人%(454,354%)#r玩法报名#r"
			local NewLine = string.gsub(NoNameLine, FncName, TheWnd.m_Tips)
			Line = string.gsub(Line, "%b()", "%%b%(%)")
			Line = string.gsub(Line, "%b[]", "%%b%[%]")
			NewTips = string.gsub(Wnd.m_AroundTips, Line, NewLine)
			break
		end
		Pos = EndPos + 1
		StartPos, EndPos = string.find(Wnd.m_AroundTips, FncName, Pos)
	end

	if NewTips ~= "" then
		Wnd.m_AroundTips = NewTips
		Wnd.m_NpcSignBtn:SetMouseOverDescAfter(Wnd.m_AroundTips)
		Wnd:SetMouseOverDescAfter(Wnd.m_AroundTips)
	end
end

function CAreaInfoWnd:GetAroundNpcTips(TheWnd)
	if not TheWnd then
		return
	end

	local AllTips = TheWnd.m_Tips.."#r"
	local TheRect = CFRect:new()
	TheWnd:GetLogicRect(TheRect)
	
	for name, WndTbl in pairs(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl) do
		for i, Wnd in pairs(WndTbl) do
			if Wnd ~= TheWnd then
				local TempRect = CFRect:new()
				Wnd:GetLogicRect(TempRect)
				--检测 TheRect 和 TempRect 是否重合
				local IsCollision = RectCollisionTest(TheRect, TempRect)
				if IsCollision and Wnd.m_Tips then
					AllTips = AllTips..Wnd.m_Tips.."#r"
					-- RPC消息过来了，更新周围NPC的TIPS
					if TheWnd.m_IsMatchGameNpc then
						UpdataNearWndTips(TheWnd, Wnd)
					end
				end
			end
		end
	end
	return AllTips
end


local function GetStringInfo(QuestName)
	local str = g_GetLanQuestInfo(QuestName.."任务目标描述")
	local needinfo = g_QuestNeedMgr[QuestName]
	if needinfo then
		for varname,p in pairs(needinfo) do
			str = str .. p.TrackInfo
		end
	end
	return str
end

function CAreaInfoWnd:InitSceneCharPoint(SceneName)

	for AreaName, _ in pairs(g_SceneAreaList[SceneName]) do
		local AreaQuestMgr = g_AreaQuestMgr[AreaName]
		if AreaQuestMgr then
			for i = 1, table.getn(AreaQuestMgr) do
				local questname = AreaQuestMgr[i]
				if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[questname] then
					
					if CQuest.CheckCanFinishQuest(questname) then
						local objname = Quest_Common(questname, "结束任务方式", "Arg")
						if self.m_CanFinishQuestNpcTbl[objname] == nil then
							local HyperLink = Quest_Common(questname, "结束任务方式", "HyperLink")
							if HyperLink ~= "" then
								local LinkTbl = {}
								LinkTbl[1] = QuestPos_Common(HyperLink, "SceneName")
								LinkTbl[2] = QuestPos_Common(HyperLink, "PosX")
								LinkTbl[3] = QuestPos_Common(HyperLink, "PosY")
								LinkTbl[4] = QuestPos_Common(HyperLink, "Mark")
								self.m_CanFinishQuestNpcTbl[objname] = {}
								self.m_CanFinishQuestNpcTbl[objname].m_QuestTbl = {}
--    					table.insert(self.m_CanFinishQuestNpcTbl[objname].m_QuestTbl,questname)
								self.m_CanFinishQuestNpcTbl[objname].m_QuestTbl[questname] = true
								self.m_CanFinishQuestNpcTbl[objname].m_HypeLinkTbl = LinkTbl
							end
						else
--    			table.insert(self.m_CanFinishQuestNpcTbl[objname].m_QuestTbl,questname)
						self.m_CanFinishQuestNpcTbl[objname].m_QuestTbl[questname] = true
						end
						
					else
						if Quest_Common(questname, "超链接") then
							local objname = Quest_Common(questname, "结束任务方式", "Arg")
							local Keys = Quest_Common:GetKeys(questname, "超链接")
							for k=table.getn(Keys),1,-1 do
								local Subject = Quest_Common(questname, "超链接", Keys[k], "Subject")
								local HyperLink = Quest_Common(questname, "超链接", Keys[k], "HyperLink")
								if Subject~="" and HyperLink~=0 and QuestPos_Common(HyperLink) then
									local SceneName = QuestPos_Common(HyperLink, "SceneName")
									local PosX = QuestPos_Common(HyperLink, "PosX")
									local PosY = QuestPos_Common(HyperLink, "PosY")
									local FindStr = "%b{"..Subject.."}"
									local sTblInfo = string.match(GetStringInfo(questname),FindStr)
									if sTblInfo then
										local num = string.find(sTblInfo,",")
										local PointName = string.sub(sTblInfo,2,num-1)
										--if objname ~= PointName then
											if self.m_DoingQuestPointTbl[PointName] == nil then
												self.m_DoingQuestPointTbl[PointName] = {}
												self.m_DoingQuestPointTbl[PointName].m_QuestTbl = {}
												self.m_DoingQuestPointTbl[PointName].m_QuestTbl[questname] = true
												self.m_DoingQuestPointTbl[PointName].m_HypeLinkTbl = {SceneName, PosX, PosY}
											else
												self.m_DoingQuestPointTbl[PointName].m_QuestTbl[questname] = true
											end
										--end
									end
								end
							end
						end
					end

				elseif CQuest.CheckQuestAvavilable(questname) then
					local objname = Quest_Common(questname, "触发任务方式", "Arg")
					
					local IsShow = true
					if g_MasterStrokeQuestMgr[questname] then -- 主线任务
						local QuestLevel = g_RetNoneMercSentenceParse(Quest_Common(questname, "任务等级"))
						QuestLevel = g_ReturnSentenceParse(QuestLevel, questname)

						local PlayerLevel = g_MainPlayer:CppGetLevel()
						if PlayerLevel - QuestLevel >= 2 then
							IsShow = false
						end
					end
					if IsShow then
						if self.m_CanTakeQuestNpcTbl[objname] == nil then
							local HyperLink = Quest_Common(questname, "触发任务方式", "HyperLink")
							if HyperLink ~= "" then
								local LinkTbl = {}
								LinkTbl[1] = QuestPos_Common(HyperLink, "SceneName")
								LinkTbl[2] = QuestPos_Common(HyperLink, "PosX")
								LinkTbl[3] = QuestPos_Common(HyperLink, "PosY")
								LinkTbl[4] = QuestPos_Common(HyperLink, "Mark")
								self.m_CanTakeQuestNpcTbl[objname] = {}
								self.m_CanTakeQuestNpcTbl[objname].m_QuestTbl = {}
								self.m_CanTakeQuestNpcTbl[objname].m_QuestTbl[questname] = true
								self.m_CanTakeQuestNpcTbl[objname].m_HypeLinkTbl = LinkTbl
							end
						else
							self.m_CanTakeQuestNpcTbl[objname].m_QuestTbl[questname] = true
						end
					end
				end
			end
		end
	end

	if Scene_Common[SceneName] then
		local npcfile = Scene_Common[SceneName].NpcSceneFile
		if npcfile ~= "" and g_NpcBornMgr.m_FunNpcBornPos[npcfile] then
			for NpcName, postbl in pairs(g_NpcBornMgr.m_FunNpcBornPos[npcfile]) do
				local NpcCommon = Npc_Common(NpcName)
				local FuncNameTbl = NpcCommon("FuncName")
				if FuncNameTbl ~= "" then
					FuncNameTbl = loadstring("return {" .. FuncNameTbl.."}")()
					if ( type(FuncNameTbl[1]) == "table" ) then
						FuncNameTbl = FuncNameTbl[1]
					end
					for i = 1, #(FuncNameTbl) do
						local FuncName = FuncNameTbl[i]
						local NpcCamp = NpcCommon("Camp")
						if IsPasserbyCamp(NpcCamp, g_MainPlayer:CppGetBirthCamp(), 0, g_MainPlayer:CppGetGameCamp())
							and FuncName ~= "无" and FuncName ~= "任务NPC" then
							self.m_FunNpcHyperLinkTbl[NpcName] = {}
							self.m_FunNpcHyperLinkTbl[NpcName].m_SceneName = SceneName
							self.m_FunNpcHyperLinkTbl[NpcName].m_FuncNameTbl = FuncNameTbl
							self.m_FunNpcHyperLinkTbl[NpcName].m_PosTbl = postbl
							--self.m_FunNpcHyperLinkTbl[NpcName] = {SceneName,postbl[1],postbl[2],FuncNameTbl}
							break
						end
					end
				end
			end
		end
		-- 邮箱坐标
		if g_NpcBornMgr.m_MailBoxBornPos[npcfile] then
			self.m_MailBoxPointTbl = {}-- 保存邮箱坐标
			self.m_MailBoxPointTbl.m_SceneName = SceneName
			self.m_MailBoxPointTbl.m_PosTbl = g_NpcBornMgr.m_MailBoxBornPos[npcfile]
--			for i, postbl in pairs(g_NpcBornMgr.m_MailBoxBornPos[npcfile]) do
--				self.m_MailBoxPointTbl["邮箱"..i] = {}
--				self.m_MailBoxPointTbl["邮箱"..i] = {SceneName, postbl[1], postbl[2]}
--			end -- end for
		end -- end if

		-- 日常任务发布榜
		if g_NpcBornMgr.m_MercQuestBornPos[npcfile] then
			self.m_MercQuestPointTbl = {}
			self.m_MercQuestPointTbl.m_SceneName = SceneName
			self.m_MercQuestPointTbl.m_PosTbl = g_NpcBornMgr.m_MercQuestBornPos[npcfile]
		end
		if g_NpcBornMgr.m_RobResBornPos[npcfile] then
			self.m_RobResPointTbl = {}
			for objName, info in pairs(g_NpcBornMgr.m_RobResBornPos[npcfile]) do
				self.m_RobResPointTbl.m_SceneName = SceneName
				if not self.m_RobResPointTbl.m_PosTbl then
					self.m_RobResPointTbl.m_PosTbl = {}
				end
				self.m_RobResPointTbl.m_PosTbl[objName] = info
			end
		end
	end
end



function CAreaInfoWnd:ShowAreaPKSteate(SceneName)
	for AreaName, v in pairs(g_SceneAreaList[SceneName]) do
		if v[1] and self.m_SceneMapWnd.m_ScenePictureWnd then
			local Wnd = self.m_SceneMapWnd.m_ScenePictureWnd["m_AreaBtn"..v[1]]
			if Wnd and Wnd.m_AreaStateWnd then
				if self.m_AreaLastState then
					if not self.m_AreaLastState[AreaName] then	-- 战争 
						local TexSrc = g_ImageMgr:GetImagePath(1357)
						AddImage(Wnd.m_AreaStateWnd.m_StateWnd, TexSrc)
					else
						local TexSrc = g_ImageMgr:GetImagePath(1358)
						AddImage(Wnd.m_AreaStateWnd.m_StateWnd, TexSrc)
					end
				end
			end
		end
	end
	Gac2Gas:SendAreaPkOnMiddleMap(g_Conn, SceneName)
end

function CAreaInfoWnd:SendRobResOnMiddleMap(SceneName)
	Gac2Gas:SendRobResOnMiddleMap(g_Conn, SceneName)
end

function CAreaInfoWnd:InitAreaWndByScene(SceneName)
--	CreateScenePictureWnd( self.m_SceneMapWnd,SceneName )
	self:ShowAreaPKSteate(SceneName) -- 显示区域PK状态
	self:ShowWarZoneState(SceneName)
	self.AreaStateList = {}	-- 需要更新的区域列表
	self:InitSelectWnd()
	self.m_LastOpenArea[SceneName] = ""
	self.m_LastShowQuest[SceneName] = nil
	self.m_SelectAreaName = nil
--	self:RemoveAllNpcPointBtn()
	self.m_CanFinishQuestNpcTbl = {}
	self.m_CanTakeQuestNpcTbl = {}
	self.m_DoingQuestPointTbl = {}
	self.m_FunNpcHyperLinkTbl = {}
	self.m_MailBoxPointTbl = {}
	self.m_MercQuestBornPos = {}
	self.m_RobResPointTbl = {}
	self:InitAllTeammateWndPos()
	self:InitAllTongmateWndPos()
	self:InitSceneCharPoint(SceneName)
	self:DisplayAllCharPointBtn(SceneName)
	self:DisplayAllTrackPosWnd()
	self:UpdateAreaStateTime(SceneName) -- 注册区域状态的Tick
	self:SendRobResOnMiddleMap(SceneName)
end

--升级需要修改pk开关
-- 初始化小地图
function CAreaInfoWnd:InitAreaInfoWnd(SceneName)
-- 保存当前区域面板的显示状态
-- Small 小地图
-- Middle 中地图
-- World 世界地图
	self.m_MapState = "Small"
	-- 隐藏中地图
	if self.m_SceneMapWnd["m_MiddleMapWnd"] then
		self.m_SceneMapWnd["m_MiddleMapWnd"]:ShowWnd(false)
	end

	-- 隐藏世界地图
	if self.m_SceneMapWnd["m_WorldMap"] then
		self.m_SceneMapWnd["m_WorldMap"]:ShowWnd(false)
	end

	-- 返回阵营地图
	self.m_BackBtn:SetWndText(GetStaticTextClient(1122))
	self.m_BackBtn:ShowWnd(true)
	-- 显示筛选
	self.m_SelectNpcSignBtn:ShowWnd(true)
	self.m_CloseSelectNpcBtn:ShowWnd(true)

	-- 显示寻路
	self.m_ShowTrackWnd:ShowWnd(true)
	self.m_SelectShowTrackBtn:ShowWnd(true)

--	local Keys = SceneArea_Common_Shell:GetKeys()
--	for i=1, table.getn(Keys) do
--		if Keys[i] ~= SceneName then
			if self.m_SceneMapWnd["m_ScenePictureWnd"] then
				self.m_SceneMapWnd["m_ScenePictureWnd"]:ShowWnd(false)
--				self.m_SceneMapWnd["m_ScenePictureWnd"] = nil
			end
--		end
--	end

	if SceneArea_Common_Shell(SceneName) == nil
		or SceneArea_Common_Shell(SceneName, "PictureWndIndex") == 0 then
		self.m_SceneName = nil
		self.m_ProcessState:SetWndText(GetStaticTextClient(19020))
		return
	end
	self.m_ProcessState:SetWndText(GetStaticTextClient(19019))
	if self.m_LastOpenArea[SceneName] == nil then
		--self.m_SceneName = SceneName
		Gac2Gas:GetPlayerSceneAreaInfo(g_Conn,SceneName)
		return
	end

	--self.m_SceneName = SceneName

	-- 显示地图
	InitScenePictureWnd(SceneName)

--	if self.m_LastShowQuest[SceneName] then
--		self:InitAreaWndByQuest(self.m_LastShowQuest[SceneName])
--	elseif self.m_LastOpenArea[SceneName] ~= "" then
--		self:InitAreaWndByArea(self.m_LastOpenArea[SceneName])
--	else
	self:InitAreaWndByScene(SceneName)

--	end
end

function CAreaInfoWnd:ClickSelectShowTrackBtn()
	if not self.m_bSelectShowTrack then
		self.m_bSelectShowTrack = true
		self.m_SelectShowTrackBtn:SetCheck(true)
		self:DisplayAllTrackPosWnd()
	else
		self.m_bSelectShowTrack = nil
		self:UnDisplayTrackPosWnd()
		self.m_SelectShowTrackBtn:SetCheck(false)
	end
end

function CAreaInfoWnd:ClickMapAutoTrack()
--	if self.m_SceneName ~= g_GameMain.m_SceneName then
--		return
--	end
	local GridX,GridY = self:CalculateMapPosByCursorPos()
	if GridX then
		PlayerAutoTrack("",self.m_SceneName,GridX,GridY)
	else
		self.m_SceneMapWnd:SetMouseOverDescAfter(ToolTipsText(510))
	end
end

function CAreaInfoWnd:ClearTrackPosWnd()
	for i = 1, table.getn(self.m_TrackWndTbl) do
		if self.m_TrackWndTbl[i][1] then
			self.m_TrackWndTbl[i][1]:ShowWnd(false)
		end
	end
	self.m_TrackTotalLen = 0
	--self.m_TrackWndTbl = {}
end

function CAreaInfoWnd:UnDisplayTrackPosWnd()
	for i = 1, table.getn(self.m_TrackWndTbl) do
		if self.m_TrackWndTbl[i][1] then
			self.m_TrackWndTbl[i][1]:ShowWnd(false)
		end
	end
end

function CAreaInfoWnd:DisplayAllTrackPosWnd()
	if self.m_SceneName == g_GameMain.m_SceneName
		and self.m_bSelectShowTrack then
		local PictureWnd = self.m_SceneMapWnd["m_ScenePictureWnd"]
		if self.m_TrackTotalLen and PictureWnd then
			for i = self.m_TrackBeginPos, self.m_TrackTotalLen do
--				if self.m_TrackWndTbl[i][1] then
--					self.m_TrackWndTbl[i][1]:ShowWnd(false)
--					self.m_TrackWndTbl[i][1] = nil
--				end
				local FPos = self.m_TrackWndTbl[i][2]
--				local Wnd = CreateTrackPosWnd(PictureWnd)
--				self.m_TrackWndTbl[i][1] = Wnd
				if self.m_TrackWndTbl[i][1] then
					self.m_TrackWndTbl[i][1]:ShowWnd(true)
					self.m_TrackWndTbl[i][1]:SetFocus()
				end
				self:CalculateCharWndPos(self.m_TrackWndTbl[i][1],self.m_SceneName,PictureWnd,FPos.x/EUnits.eGridSpanForObj,FPos.y/EUnits.eGridSpanForObj)
			end
		end
	end
end

function CAreaInfoWnd:CalculateTrackPosTbl(BeginPos,EndPos,TrackPosDis)
	local dx = EndPos.x-BeginPos.x
	local dy = EndPos.y-BeginPos.y
	local fDist = math.sqrt(dx * dx + dy * dy)
	if math.abs(fDist) > (TrackPosDis+TrackPosDis/2) then
		local PointNum = math.floor(fDist/TrackPosDis)
		local mindx = dx/(fDist/TrackPosDis)
		local mindy = dy/(fDist/TrackPosDis)
		local PosTbl = {}
		for i = 1, PointNum do
			local WayPos = CFPos:new()
			WayPos.x = BeginPos.x+mindx*i
			WayPos.y = BeginPos.y+mindy*i
			table.insert(PosTbl, WayPos)
		end
		if math.abs(fDist%TrackPosDis) > (TrackPosDis/2) then
			table.insert(PosTbl, EndPos)
		end
		return PosTbl
	else
		return {EndPos}
	end
end

local ICoefficient = 1/70

function CAreaInfoWnd:CreateAutoTrackByPath(SceneName,path)
	self:ClearTrackPosWnd()
	if not SceneArea_Common_Shell(SceneName) then
		return
	end
	local PictureWnd = nil
	if self.m_SceneName == SceneName then
		PictureWnd = self.m_SceneMapWnd["m_ScenePictureWnd"]
	end
	
	local SceneMaxPos = GetCfgTransformValue(false, "SceneArea_Common_Shell", SceneName, "SceneMaxPos")
	local ScenePixelX = SceneMaxPos[1]*EUnits.eGridSpanForObj
	local ScenePixelY = SceneMaxPos[2]*EUnits.eGridSpanForObj
	local TrackPosDis = ICoefficient*math.sqrt(ScenePixelX*ScenePixelX+ScenePixelY*ScenePixelY)
	
	local PlayerPos = CFPos:new()
	g_MainPlayer:GetPixelPos(PlayerPos)
	local len = path:GetPathSize()
	local BeginPos = PlayerPos
	local TotalNum = 0
	local TrackWndLen = table.getn(self.m_TrackWndTbl)
	for i = 1, len do
		local EndPos = CFPos:new()
		path:GetWayPointPixelPosOfPath(EndPos,i)
		local PosTbl = self:CalculateTrackPosTbl(BeginPos,EndPos,TrackPosDis)
		for j = 1, table.getn(PosTbl) do
			TotalNum = TotalNum + 1
			if TotalNum <= TrackWndLen then
				self.m_TrackWndTbl[TotalNum][2] = PosTbl[j]
				if self.m_SceneName == SceneName then
--					self.m_TrackWndTbl[TotalNum][1]:ShowWnd(true)
--					self.m_TrackWndTbl[TotalNum][1]:SetFocus()
--					--print("true")
				else
					self.m_TrackWndTbl[TotalNum][1]:ShowWnd(false)
				end
			else
				local Wnd = CreateTrackPosWnd(self)
				self.m_TrackWndTbl[TotalNum] = {Wnd, PosTbl[j]}
			end
			--如果开启了显示寻路轨迹而且当前打开的是当前地图的区域面板，则创建寻路轨迹	
			if self.m_bSelectShowTrack and self.m_SceneName == SceneName and PictureWnd then
				self:CalculateCharWndPos(self.m_TrackWndTbl[TotalNum][1],self.m_SceneName,PictureWnd,PosTbl[j].x/EUnits.eGridSpanForObj,PosTbl[j].y/EUnits.eGridSpanForObj)
				self.m_TrackWndTbl[TotalNum][1]:ShowWnd(true)
			end
		end
		BeginPos = EndPos
	end
	self.m_TrackTotalLen = TotalNum
	self.m_TrackBeginPos = 1
end

function CAreaInfoWnd:CloseAreaInfoWnd()
	self:ShowWnd(false)
	--self.m_SceneName = nil
end

--重写VirtualExcludeWndOnClose，在CloseExcludeWnd时会调用
function CAreaInfoWnd:VirtualExcludeWndOnClose()
  -- 清除AreaTick
	self:ClearAllTick()
	self.m_SelectNpcSignListCtrl:ShowWnd(false)
	-- 隐藏任务图标，下次打开时再显示
	for Index, _ in pairs(self.m_QuestNpcIcon) do
		self.m_DeletQuestIcon[Index] = true
		self.m_AllNpcIcon[Index]:ShowWnd(false)
	end
	Gac2Gas:StopGetTeammatePos(g_Conn)
	Gac2Gas:StopGetTongmatePos(g_Conn)
	if g_GameMain.m_SceneName == "鲜血试炼场"
		 or g_GameMain.m_SceneName == "小pvp测试" then
		Gac2Gas:StopGetFbPlayerPos(g_Conn)
	end
end

function CAreaInfoWnd:RefreshQuestInfo(QuestName)
	if not self:IsShow() then
		return
	end
	
	if not Quest_Common(QuestName, "所属区域") then
		return
	end
	local SceneNameKeys = SceneArea_Common_Shell:GetKeys()
	for i=1, table.getn(SceneNameKeys) do
		local SceneName = SceneNameKeys[i]
		local Keys = Quest_Common:GetKeys(QuestName, "所属区域")
		for n=1, table.getn(Keys) do
			local AreaName = Quest_Common(QuestName, "所属区域", Keys[n])
			if AreaName and AreaName ~= "" and g_SceneAreaList[SceneName][AreaName] then
				if self.m_SceneName == SceneName then
					self:InitAreaInfoWnd(SceneName)
					return
				end
			end
		end
	end
end

function CAreaInfoWnd:OpenAreaWndByQuest(QuestName)
	local SceneName = ""
	if Quest_Common(QuestName, "所属区域") then
		local SceneNameKeys = SceneArea_Common_Shell:GetKeys()
		for i=1, table.getn(SceneNameKeys) do
			local scenename = SceneNameKeys[i]
			local AreaName = Quest_Common(QuestName, "所属区域", "1")
			if AreaName and AreaName ~= "" and g_SceneAreaList[scenename][AreaName] then
				SceneName = scenename
				break
			end
		end
	end

	if not self:IsShow() then
		if SceneName ~= "" then
			self.m_LastShowQuest[SceneName] = QuestName
		else
			SceneName = g_GameMain.m_SceneName
		end
		self:InitAreaInfoWnd(SceneName)
		self:ShowWnd(true)
		self:CheckIsGetTeammatePos()
		self:CheckIsGetTongmatePos()
		self:CheckIsGetAllPlayerPos()
		-- 某些刷怪器刷出来的NPC需要在中地图上显示
		Gac2Gas:GetBornNpcOnMiddleMap(g_Conn)
		Gac2Gas:GetFbBossOnMiddleMap(g_Conn)
		self:SetFocus()
	else
--		if SceneName ~= "" and (self.m_SceneName ~= SceneName or self.m_LastShowQuest[SceneName] ~= QuestName) then
--			self.m_LastShowQuest[SceneName] = QuestName
--			self:InitAreaInfoWnd(SceneName)
--			self:ShowWnd(true)
--			self:SetFocus()
--		else
			self:CloseAreaInfoWnd()
--		end
	end
end

function CAreaInfoWnd:InitOneAreaCharPoint(AreaName)
--	if self.m_NpcSignTypeSelectTbl == nil then
--		self.m_NpcSignTypeSelectTbl = {}
--		self.m_SelectNpcSignListCtrl.m_ItemTbl["标示所有"].m_SelectBtn:SetCheck(true)
--		self.m_NpcSignTypeSelectTbl["标示所有"] = true
--	end
	local AreaQuestMgr = g_AreaQuestMgr[AreaName]
	
	if AreaQuestMgr then
		for i = 1, table.getn(AreaQuestMgr) do
			local questname = AreaQuestMgr[i]
			local QuestNode = Quest_Common(questname)
			if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[questname] then

				if CQuest.CheckCanFinishQuest(questname) then
					local objname = QuestNode("结束任务方式", "Arg")
					if self.m_CanFinishQuestNpcTbl[objname] == nil then
						local HyperLink = QuestNode("结束任务方式", "HyperLink")
						if HyperLink ~= "" then
							local LinkTbl = {}
							LinkTbl[1] = QuestPos_Common(HyperLink, "SceneName")
							LinkTbl[2] = QuestPos_Common(HyperLink, "PosX")
							LinkTbl[3] = QuestPos_Common(HyperLink, "PosY")
							LinkTbl[4] = QuestPos_Common(HyperLink, "Mark")
							self.m_CanFinishQuestNpcTbl[objname] = {}
							self.m_CanFinishQuestNpcTbl[objname].m_QuestTbl = {}
							self.m_CanFinishQuestNpcTbl[objname].m_QuestTbl[questname] = true
							self.m_CanFinishQuestNpcTbl[objname].m_HypeLinkTbl = LinkTbl
						end
					else
						self.m_CanFinishQuestNpcTbl[objname].m_QuestTbl[questname] = true
					end
				else
					if QuestNode("超链接") then
						local Keys = QuestNode:GetKeys("超链接")
						for k=1, table.getn(Keys) do
							local Subject = QuestNode("超链接", Keys[k], "Subject")
							local HyperLink = QuestNode("超链接", Keys[k], "HyperLink")
							if Subject~="" and HyperLink~=0 and QuestPos_Common(HyperLink) then
								local SceneName = QuestPos_Common(HyperLink, "SceneName")
								local PosX = QuestPos_Common(HyperLink, "PosX")
								local PosY = QuestPos_Common(HyperLink, "PosY")
								local FindStr = "%b{"..Subject.."}"
								local sTblInfo = string.match(GetStringInfo(questname),FindStr)
								if sTblInfo then
									local num = string.find(sTblInfo,",")
									local PointName = string.sub(sTblInfo,2,num-1)
									local objname = QuestNode("结束任务方式", "Arg")
									--if PointName ~= objname then
										if self.m_DoingQuestPointTbl[PointName] == nil then
											self.m_DoingQuestPointTbl[PointName] = {}
											self.m_DoingQuestPointTbl[PointName].m_QuestTbl = {}
											self.m_DoingQuestPointTbl[PointName].m_QuestTbl[questname] = true
											self.m_DoingQuestPointTbl[PointName].m_HypeLinkTbl = {SceneName, PosX, PosY}
										else
											self.m_DoingQuestPointTbl[PointName].m_QuestTbl[questname] = true
										end
									--end
								end
							end
						end
					end
				end

			elseif CQuest.CheckQuestAvavilable(questname) then

				local objname = QuestNode("触发任务方式", "Arg")
				if self.m_CanTakeQuestNpcTbl[objname] == nil then
					local HyperLink = QuestNode("触发任务方式", "HyperLink")
					if HyperLink ~= "" then
						local LinkTbl = {}
						LinkTbl[1] = QuestPos_Common(HyperLink, "SceneName")
						LinkTbl[2] = QuestPos_Common(HyperLink, "PosX")
						LinkTbl[3] = QuestPos_Common(HyperLink, "PosY")
						LinkTbl[4] = QuestPos_Common(HyperLink, "Mark")
						self.m_CanTakeQuestNpcTbl[objname] = {}
						self.m_CanTakeQuestNpcTbl[objname].m_QuestTbl = {}
						self.m_CanTakeQuestNpcTbl[objname].m_QuestTbl[questname] = true
						self.m_CanTakeQuestNpcTbl[objname].m_HypeLinkTbl = LinkTbl
					end
				else
					self.m_CanTakeQuestNpcTbl[objname].m_QuestTbl[questname] = true
				end
			end
		end
	end
end

function CAreaInfoWnd:InitAreaWndByArea(AreaName)
	self:InitSelectWnd()
	self.m_SelectAreaName = AreaName
--	self:RemoveAllNpcPointBtn()
	self.m_CanFinishQuestNpcTbl = {}
	self.m_MailBoxPointTbl = {}
	self.m_MercQuestBornPos = {}
	self.m_RobResPointTbl = {}
	self.m_CanTakeQuestNpcTbl = {}
	self.m_DoingQuestPointTbl = {}
	self.m_FunNpcHyperLinkTbl = {}
	self:InitOneAreaCharPoint(AreaName)
	self:DisplayAllTrackPosWnd()
	self:InitAllTeammateWndPos()
	self:InitAllTongmateWndPos()
	self:DisplayAllCharPointBtn(self.m_SceneName)
end

local function TempPlayWndFxFunc(Wnd)
	if Wnd.m_FxTick then
		UnRegisterTick(Wnd.m_FxTick)
	end
	local DoNum = 0
	local function DoFx()
		DoNum = DoNum+1
		if DoNum%2 == 1 then
			Wnd.m_NpcSignBtn:SetTransparent(0.1)
		else
			Wnd.m_NpcSignBtn:SetTransparent(1)
		end
--		Wnd.m_NpcSignBtn:SetTransparentObj(400,true)
		if DoNum == 20 then
			if Wnd.m_FxTick then
				UnRegisterTick(Wnd.m_FxTick)
				Wnd.m_FxTick = nil
			end
		end
	end
	Wnd.m_FxTick = RegisterTick("Wnd.m_FxTick",DoFx,1000)
end

function CAreaInfoWnd:PlayDoingQuestPointFx(QuestName)
	for pointname ,v in pairs(self.m_CanFinishQuestNpcTbl) do
		if v.m_QuestTbl[QuestName] then
			local Wnd = self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[pointname]
			--Wnd:PlayFx()
--			TempPlayWndFxFunc(Wnd)
--			print("任务点"..pointname.."播放特效1")
		end
	end
	for pointname ,v in pairs(self.m_DoingQuestPointTbl) do
		if v.m_QuestTbl[QuestName] then
			local Wnd = self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[pointname]
			--Wnd:PlayFx()
--			TempPlayWndFxFunc(Wnd)
--			print("任务点"..pointname.."播放特效2")
		end
	end
end

function CAreaInfoWnd:InitAreaWndByQuest(QuestName)
	self:InitSelectWnd()
--	self:RemoveAllNpcPointBtn()
	self.m_CanFinishQuestNpcTbl = {}
	self.m_CanTakeQuestNpcTbl = {}
	self.m_DoingQuestPointTbl = {}
	self.m_FunNpcHyperLinkTbl = {}
	self.m_MailBoxPointTbl = {}
	self.m_MercQuestBornPos = {}
	self.m_RobResPointTbl = {}
	local bLastOpenArea = false
	if Quest_Common(QuestName, "所属区域") then
		local Keys = Quest_Common:GetKeys(QuestName, "所属区域")
		for n = 1, table.getn(Keys) do
			local AreaName = Quest_Common(QuestName, "所属区域", Keys[n])
			if AreaName and AreaName ~= "" then
				self:InitOneAreaCharPoint(AreaName)
				local sArea = self.m_LastOpenArea[self.m_SceneName]
				bLastOpenArea = (sArea == AreaName)
			end
		end
	end
	self:InitAllTeammateWndPos()
	self:InitAllTongmateWndPos()
	self:DisplayAllCharPointBtn(self.m_SceneName)
	--self:PlayDoingQuestPointFx(QuestName)
	self:DisplayAllTrackPosWnd()

	if not bLastOpenArea then
		local PictureWnd = self.m_SceneMapWnd["m_ScenePictureWnd"]
		PictureWnd:ActiveAreaBtn()
		self.m_LastOpenArea[self.m_SceneName] = ""
	end
end

function CAreaInfoWnd:CalculateMapPosByCursorPos()
	local PictureWnd = self.m_SceneMapWnd["m_ScenePictureWnd"]
	local SceneName = self.m_SceneName
	if PictureWnd == nil or SceneName == nil or SceneArea_Common_Shell(SceneName) == nil then
		return nil,nil
	end
	local posCursor = CFPos:new()
	g_App:GetRootWnd():GetLogicCursorPos(posCursor)
	local rt = CFRect:new()
	PictureWnd:GetLogicRect(rt)

	--场景的最大坐标
	local SceneMaxPos = GetCfgTransformValue(false, "SceneArea_Common_Shell", SceneName, "SceneMaxPos")
	local SceneWidth = SceneMaxPos[1]
	local SceneHeight = SceneMaxPos[2]
	--有效的地图图片在窗口中的位置
	local PictureMinPos = GetCfgTransformValue(false, "SceneArea_Common_Shell", SceneName, "PictureMinPos")
	local PictureMaxPos = GetCfgTransformValue(false, "SceneArea_Common_Shell", SceneName, "PictureMaxPos")
	--有效的地图图片在裁剪前的大小（正方形）
	local PictureWidthBeforeCut = SceneArea_Common_Shell(SceneName, "PictureWidthBeforeCut")
	--四个方向上裁减掉的数值
	local CutTop= SceneArea_Common_Shell(SceneName, "CutTop")
	local CutBottom = SceneArea_Common_Shell(SceneName, "CutBottom")
	local CutLeft = SceneArea_Common_Shell(SceneName, "CutLeft")
	local CutRight = SceneArea_Common_Shell(SceneName, "CutRight")

	local EffectivePictureHeight = (math.sqrt(2)*PictureWidthBeforeCut*SceneHeight)/(SceneWidth+SceneHeight)
	local EffectivePictureWidth = math.sqrt(2)*PictureWidthBeforeCut-EffectivePictureHeight

	local ZoomSize = self:GetRootZoomSize()
	local TransferPosX = (posCursor.x-rt.left)+CutLeft-PictureMinPos[1]
	local TransferPosY = (rt.bottom-posCursor.y)+CutBottom-PictureMinPos[2]

	local EffiveWndW = ((TransferPosX-EffectivePictureHeight/math.sqrt(2))/math.sqrt(2)+TransferPosY/math.sqrt(2))
	local EffiveWndH = (-(TransferPosX-EffectivePictureHeight/math.sqrt(2))/math.sqrt(2)+TransferPosY/math.sqrt(2))

	if EffiveWndH < 0 or EffiveWndW < 0 or EffiveWndH > EffectivePictureHeight or EffiveWndW > EffectivePictureWidth then
		return nil,nil
	end

	local GridX = SceneWidth*EffiveWndW/EffectivePictureWidth
	local GridY = SceneHeight*EffiveWndH/EffectivePictureHeight
	return math.floor(GridX),math.floor(GridY)
end

function CAreaInfoWnd:ShowCursorPosToolTips(Wnd)
	local GridX,GridY = self:CalculateMapPosByCursorPos()
	local Tips = ""
	if GridX then
		Tips = GetStaticTextClient(19009).."("..GridX..","..GridY..")"
	else
		Tips = GetStaticTextClient(19010)
	end
	Wnd:SetMouseOverDescAfter(Tips)
end

-- 初始化中地图
function CAreaInfoWnd:InitMiddleMap(MiddleMap)

	self.m_MapState = "Middle"

	-- 返回世界地图
	self.m_BackBtn:SetWndText(GetStaticTextClient(1123))
	self.m_BackBtn:ShowWnd(true)

	-- 隐藏鼠标旁边的Tips
	self.m_SceneMapWnd:SetMouseOverDescAfter("")

	-- 隐藏筛选
	self.m_SelectNpcSignBtn:ShowWnd(false)
	self.m_SelectNpcSignListCtrl:ShowWnd(false)
	self.m_CloseSelectNpcBtn:ShowWnd(false)

	-- 隐藏寻路
	self.m_ShowTrackWnd:ShowWnd(false)
	self.m_SelectShowTrackBtn:ShowWnd(false)

	-- 隐藏小地图
	if self.m_SceneMapWnd["m_ScenePictureWnd"] then
		self.m_SceneMapWnd["m_ScenePictureWnd"]:ShowWnd(false)
	end

	-- 隐藏世界地图
	if self.m_SceneMapWnd["m_WorldMap"] then
		self.m_SceneMapWnd["m_WorldMap"]:ShowWnd(false)
	end

	-- 设置位置
	local rt = CFRect:new()
	self.m_SceneMapWnd:GetLogicRect(rt)

	-- 显示中地图
	local MapInfo = "m_"..MiddleMap.."Map"

	if self.m_SceneMapWnd[MapInfo] then
		self.m_SceneMapWnd[MapInfo]:SetLogicRect(rt)
		self.m_SceneMapWnd[MapInfo]:ShowWnd(true)
		self.m_SceneMapWnd[MapInfo]:SetFocus()
		self.m_SceneMapWnd["m_MiddleMapWnd"] = self.m_SceneMapWnd[MapInfo]
	end
end

function CAreaInfoWnd:InitWorldMap()

	self.m_MapState = "World"

	-- 隐藏返回按钮
	self.m_BackBtn:ShowWnd(false)

	-- 隐藏鼠标旁边的Tips
	self.m_SceneMapWnd:SetMouseOverDescAfter("")

	-- 隐藏筛选
	self.m_SelectNpcSignBtn:ShowWnd(false)
	self.m_SelectNpcSignListCtrl:ShowWnd(false)
	self.m_CloseSelectNpcBtn:ShowWnd(false)

	-- 隐藏寻路
	self.m_ShowTrackWnd:ShowWnd(false)
	self.m_SelectShowTrackBtn:ShowWnd(false)

	-- 隐藏小地图
	if self.m_SceneMapWnd["m_ScenePictureWnd"] then
		self.m_SceneMapWnd["m_ScenePictureWnd"]:ShowWnd(false)
	end

	-- 隐藏中地图
	if self.m_SceneMapWnd["m_MiddleMapWnd"] then
			self.m_SceneMapWnd["m_MiddleMapWnd"]:ShowWnd(false)
	end

	-- 设置位置
	local rt = CFRect:new()
	self.m_SceneMapWnd:GetLogicRect(rt)
	self.m_SceneMapWnd["m_WorldMap"]:SetLogicRect(rt)

	self.m_SceneMapWnd["m_WorldMap"]:ShowWnd(true)
	self.m_SceneMapWnd["m_WorldMap"]:SetFocus()
end

--子窗口消息
function CAreaInfoWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK  then
		if Child == self.m_SelectNpcSignBtn or Child == self.m_CloseSelectNpcBtn then
			--self:InitSelectWnd()
			if self.m_SelectNpcSignListCtrl:IsShow() then
				self.m_SelectNpcSignListCtrl:ShowWnd(false)
			else
				self.m_SelectNpcSignListCtrl:ShowWnd(true)
				self.m_SelectNpcSignListCtrl:SetFocus()
			end
		elseif Child == self.m_SelectForceSignBtn then
--			self.m_SelectForceSignListCtrl:ShowWnd(true)
--			self.m_SelectForceSignListCtrl:SetFocus()
		elseif Child == self.m_CloseSelectForceBtn then
			self.m_SelectForceSignListCtrl:ShowWnd(false)
--			elseif Child == self.m_ReturnWorldMapBtn then
--				g_GameMain.m_WorldMap:ShowWnd(true)
		elseif Child == self.m_ShowSceneInfoBtn then
			if g_GameMain.m_SceneName and SceneArea_Common_Shell(g_GameMain.m_SceneName) then
				--self.m_SceneMapWnd["m_ScenePictureWnd"]:ActiveAreaBtn()
				self:InitAreaInfoWnd(g_GameMain.m_SceneName)
			end
		elseif Child == self.m_SelectShowTrackBtn then
			self:ClickSelectShowTrackBtn()
		elseif Child == self.m_CloseBtn then
			self:CloseAreaInfoWnd()
		elseif Child == self.m_SceneMapWnd then	-- 自动寻路
			if self.m_MapState == "Small" then
				self.m_SelectNpcSignListCtrl:ShowWnd(false)
				self:ClickMapAutoTrack()
				self:SetTrackWndFocus()
			end

		--点击按钮 跳到上一层地图
		elseif Child == self.m_BackBtn then
			if self.m_MapState == "Small" then
				if self.m_SceneName and Scene_Common[self.m_SceneName] ~= "" then
					local MiddleMap = GetMiddleMapBySceneName(self.m_SceneName)
					self:InitMiddleMap(MiddleMap)
				end
			elseif self.m_MapState == "Middle" then -- 显示世界地图
				self:InitWorldMap()
			end
		else
			return
		end
	elseif uMsgID == BUTTON_RCLICK and Child == self.m_SceneMapWnd then
		if self.m_MapState == "Small" then
			if self.m_SceneName and Scene_Common[self.m_SceneName] ~= "" then
				-- 清除Tick
				self:ClearAllTick()
				local MiddleMap = GetMiddleMapBySceneName(self.m_SceneName)
				self:InitMiddleMap(MiddleMap)
			end
		elseif self.m_MapState == "Middle" then -- 显示世界地图
			self:InitWorldMap()
		end
	elseif uMsgID == BUTTON_MOUSEMOVE and Child == self.m_SceneMapWnd then
		if self.m_MapState == "Small" then
			self:ShowCursorPosToolTips(self.m_SceneMapWnd)
		end
	elseif uMsgID == ICON_LBUTTONDBLCLICK and Child == self.m_SceneMapWnd then
		self:SetTrackWndFocus()
	end
end

function CAreaInfoWnd:RetPlayerAreaInfo(Conn, AreaName, IsDiscovery, QuestCount)
	if self.m_PlayerAreaInfoTbl[AreaName] == nil then
		self.m_PlayerAreaInfoTbl[AreaName] = {}
	end
	self.m_PlayerAreaInfoTbl[AreaName]["IsDiscovery"] = IsDiscovery
	self.m_PlayerAreaInfoTbl[AreaName]["QuestCount"] = QuestCount
	self.m_AreaName = AreaName
--	if AreaName == self.m_SelectAreaName then
--		self:InitAreaWndByArea(AreaName)
--	end
end

function CAreaInfoWnd:RetFirstTimeIntoArea(Conn, AreaName, OldState, QuestCount)
	if self.m_PlayerAreaInfoTbl[AreaName] == nil then
		self.m_PlayerAreaInfoTbl[AreaName] = {}
	end
	if OldState == 0 then
		g_GameMain.m_AreaIntroductionWnd:ShowAreaIntroduction(AreaName)
	end
	self.m_PlayerAreaInfoTbl[AreaName]["IsDiscovery"] = 1
	self.m_PlayerAreaInfoTbl[AreaName]["QuestCount"] = QuestCount
	if not self:IsShow() then
		return
	end
	local SceneName = self.m_SceneName
	if AreaName == self.m_SelectAreaName then
		InitScenePictureWnd(SceneName)
		self:InitAreaWndByArea(AreaName)
	elseif SceneName then
		if g_SceneAreaList[SceneName][AreaName] then
			if self.m_SceneMapWnd["m_ScenePictureWnd"] then
				self.m_SceneMapWnd["m_ScenePictureWnd"]:ShowWnd(false)
				InitScenePictureWnd(SceneName)
				self:InitAreaWndByScene(SceneName)
			end
		end
	end
end

function CAreaInfoWnd:RetPlayerSceneAreaInfo(Conn,SceneName)
	--self.m_SceneName = SceneName
	InitScenePictureWnd(SceneName)
	local QuestName = self.m_LastShowQuest[SceneName]
	if self:IsShow()
		and self.m_SceneName
		and self.m_SceneName ~= SceneName then
		return
	end
	if QuestName then
		self:InitAreaWndByQuest(QuestName)
	else
		self:InitAreaWndByScene(SceneName)
	end
  -- 显示帮会成员和队友
	self:CheckIsGetTongmatePos()
	self:CheckIsGetTeammatePos()
	self:CheckIsGetAllPlayerPos()
end

function CAreaInfoWnd:SendTeammatePos(Conn,MemberId,SceneName,Gridx,Gridy)
	if not self.m_TeammateWndTbl[MemberId] then
		self.m_TeammateWndTbl[MemberId] = {}
	end
	if self.m_TeammateWndTbl[MemberId].m_Pos then
		local sName = self.m_TeammateWndTbl[MemberId].m_Pos[1]
		local x = self.m_TeammateWndTbl[MemberId].m_Pos[2]
		local y = self.m_TeammateWndTbl[MemberId].m_Pos[3]
		if sName == SceneName and Gridx == x and Gridy == y then
			return
		end
	end
	self.m_TeammateWndTbl[MemberId].m_Pos = {SceneName,Gridx,Gridy}
	if self:IsShow() then
		self:InitTeammateWndPos(MemberId)
	else
		Gac2Gas:StopGetTeammatePos(g_Conn)
	end
end

function CAreaInfoWnd:SendTongmatePos(Conn, MemberId, SceneName, Gridx, Gridy, MemberName)
	if not self.m_TongmateWndTbl[MemberId] then
		self.m_TongmateWndTbl[MemberId] = {}
	end
	if self.m_TongmateWndTbl[MemberId].m_Pos then
		local sName = self.m_TongmateWndTbl[MemberId].m_Pos[1]
		local x = self.m_TongmateWndTbl[MemberId].m_Pos[2]
		local y = self.m_TongmateWndTbl[MemberId].m_Pos[3]
		if sName == SceneName and Gridx == x and Gridy == y then
			--print("相同============")
			return
		end
	end
	self.m_TongmateWndTbl[MemberId].m_Pos = {SceneName,Gridx,Gridy}
	if self:IsShow() then
		self:InitTongmateWndPos(MemberId, MemberName)
	else
		Gac2Gas:StopGetTongmatePos(g_Conn)
	end
end

function CAreaInfoWnd:CheckIsNeedSendTeammatePos(Conn)
	if self:IsShow() then
		Gac2Gas:GetTeammatePos(g_Conn)
	end
end

-- 加入帮会后是否有必要显示帮会成员在中地图上的位置
function CAreaInfoWnd:CheckIsNeedSendTongmatePos(Conn, TongID)
	if self:IsShow() then
		self:CheckIsGetTongmatePos(TongID)
	end
end

function CAreaInfoWnd:HideLeavedTeammatePos(Conn, PlayerID)
	if self:IsShow() then
		tbl = self.m_TeammateWndTbl[PlayerID]
		if tbl and tbl.m_Wnd then
			tbl.m_Wnd:ShowWnd(false)
		end
	end
	self.m_TeammateWndTbl[PlayerID] = nil -- 坐标信息清零
end

function CAreaInfoWnd:HideLeavedTongmatePos(Conn, PlayerID)
	if self:IsShow() then
		tbl = self.m_TongmateWndTbl[PlayerID]
		if tbl and tbl.m_Wnd then
			tbl.m_Wnd:ShowWnd(false)
		end
	end
	self.m_TongmateWndTbl[PlayerID] = nil -- 坐标信息清零
end

function CAreaInfoWnd:HideLeftFbPlayerPos(Conn, PlayerID)
	if self:IsShow() then
		tbl = self.m_FbPlayerWndTbl[PlayerID]
		if tbl and tbl.m_Wnd then
			tbl.m_Wnd:ShowWnd(false)
		end
	end
	self.m_FbPlayerWndTbl[PlayerID] = nil -- 坐标信息清零	
end

-- 中地图MatchGame的NPC显示具体可以参加哪些活动
function CAreaInfoWnd:SetMatchGameNpcTips(Conn, PointName, GameNames)
	local Tips = ""
	local m_GameTbl = loadstring("return {" .. GameNames .. "}")()
	if not m_GameTbl then
		return
	end
	for _, Name in pairs(m_GameTbl) do
		local ShowName = g_GetFbActionNameLanStr(Name)
		Tips = Tips..ShowName.."#r"
	end

	if Tips and Tips ~= "" then
		local WndTbl = self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName]
		if WndTbl then
			for i, Wnd in pairs(WndTbl) do
				local FncName = Lan_NotFunNpcTypeMap(MemH64("日常活动报名处"), "NpcShowDlg")
				Wnd.m_Tips = string.gsub(Wnd.m_Tips, FncName.."#r", Tips)
				-- 获取周围NPC的Tips
				Wnd.m_IsMatchGameNpc = true
				local AroundTips = self:GetAroundNpcTips(Wnd)
				Wnd.m_NpcSignBtn:SetMouseOverDescAfter(AroundTips)
				Wnd:SetMouseOverDescAfter(AroundTips)
			end
		end
	end
end

function CAreaInfoWnd:DeadNpcOnMiddleMap(Conn, SceneName, NpcName)
	if self.m_BornNpcWndTbl then
		if self.m_BornNpcWndTbl[NpcName] then
			local Wnd = self.m_BornNpcWndTbl[NpcName][1]
			Wnd:ShowWnd(false)
		end
	end
end

-- 某些刷怪器刷的NPC需要在中地图上显示
function CAreaInfoWnd:ShowBornNpcOnMiddleMap(SceneName, PointName, PosX, PosY)
--	if not self.PointTipsTbl[PointName] then
--		self.PointTipsTbl[PointName] = {}
--		self.PointTipsTbl[PointName].m_PosTbl = {}
--		self.PointTipsTbl[PointName].m_Str = ""
--	end
	if not self.m_BornNpcWndTbl[PointName] then
		self.m_BornNpcWndTbl[PointName] = {}
	end

	local _,strTexture = GetFuncTypeByNpcName(PointName)
	if not self.m_PointName then
		 self.m_PointName = {}
	end
	
	if self.m_BornNpcWndTbl[PointName] then
		if self.m_BornNpcWndTbl[PointName][1] then
			if not self.m_BornNpcWndTbl[PointName][1]:IsShow() then
				self.m_BornNpcWndTbl[PointName][1]:ShowWnd(true)
			end
		end
	end
	
	if self.m_PointName[PointName] then
		return 
	end
	local Wnd = self:InitOneNpcWndPos(SceneName,self.m_SceneMapWnd["m_ScenePictureWnd"],"NpcMiddleMapSign", "MiddleMapNpcBtn", PointName, PosX, PosY, strTexture, false)
	self.m_PointName[PointName] = true
	--table.insert(self.PointTipsTbl[PointName].m_PosTbl,{PosX,PosY})
	--table.insert(self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[PointName],Wnd)
	table.insert(self.m_BornNpcWndTbl[PointName],Wnd)
	-- 显示ToolTips
	--self:ShowWndToolTips()
	local ShowName = nil
	if IntObj_Common(PointName) then
		ShowName = GetIntObjDispalyName(PointName)
	elseif Npc_Common(PointName) then
		ShowName = GetNpcDisplayName(PointName)
	else
		ShowName = PointName
	end
	
	local ShowTips = ""
	if NpcOnMiddleMapExistTime[PointName] then
		 ShowTips = ToolTipsText(NpcOnMiddleMapExistTime[PointName]) -- 占领中
	elseif NpcOnMiddleMapResetTime[PointName] then
		local nowtime = os.time()
		local nowdate = os.date("*t")
		local nowhour = nowdate.hour
		local NextHour = NpcOnMiddleMapResetTime.time[1]
		for i = 1, #(NpcOnMiddleMapResetTime.time) - 1 do
			if nowhour >= NpcOnMiddleMapResetTime.time[i] and nowhour < NpcOnMiddleMapResetTime.time[i+1] then
				NextHour = NpcOnMiddleMapResetTime.time[i+1]
			end
		end
		
		ShowTips = ToolTipsText(903)..NextHour..":00" --"Next time -- "..NextHour	-- 下次可被占领时间为：10:00
		Wnd.m_NpcSignBtn:SetMouseOverDescAfter(ShowTips)
		Wnd:SetMouseOverDescAfter(ShowTips)
	else
		ShowTips = ShowName.."("..math.floor(PosX)..","..math.floor(PosY)..")"
	end
	Wnd.m_NpcSignBtn:SetMouseOverDescAfter(ShowTips)
	Wnd:SetMouseOverDescAfter(ShowTips)
end

function CAreaInfoWnd:ClearAllTick()
	-- 清除Tick
	if self.m_AreaStateCountDownTick then
		UnRegisterTick(self.m_AreaStateCountDownTick)
		self.m_AreaStateCountDownTick = nil
		--print("+++++++++++++++++++++++++++++注销Tick")
	end
end

local function FormatTime(Time)
	local Hour = math.floor(Time / 1000 / 60 / 60)
  local Min  = math.floor(Time / 1000 / 60 % 60)
  local Sec  = math.floor(Time /1000 % 60)

  if Hour < 10 then
  	Hour = "0"..Hour
  end
  if Min < 10 then
  	Min = "0"..Min
  end
  if Sec < 10 then
  	Sec = "0"..Sec
  end
  local ShowTime = Hour..":"..Min..":"..Sec
  return ShowTime
end

function CAreaInfoWnd:UpdateAreaStateTime(SceneName)
	local function CountDownTick()
		--print("----tick")
		for AreaName, wnd in pairs(self.AreaStateList) do
			if wnd.m_AreaStateTime and wnd.m_StateWnd then
				local ShowTime = FormatTime(wnd.m_AreaStateTime)
				
				local MinLevel = 0
				local MaxLevel = 0
				
				local pkRuleKeys = PkRuleInfo_Common:GetKeys()
				for i, pkRuleAreaTemp in pairs(pkRuleKeys) do
					local pkRuleTemp = PkRuleInfo_Common(pkRuleAreaTemp)
					local tempKeys = pkRuleTemp:GetKeys()
					for _, temp in pairs(tempKeys) do
						if temp == SceneName then
							MinLevel = pkRuleTemp(temp, "MinLevel")
							MaxLevel = pkRuleTemp(temp, "MaxLevel")
						end
					end
				end
				
				if wnd.m_AreaState then
					wnd.m_StateWnd:SetMouseOverDescAfter("#cffeaab"..GetStaticTextClient(19016)..ShowTime.."#r"..GetStaticTextClient(19022)..":"..MinLevel.."-"..MaxLevel)
				else
					wnd.m_StateWnd:SetMouseOverDescAfter("#cffeaab"..GetStaticTextClient(19017)..ShowTime.."#r"..GetStaticTextClient(19022)..":"..MinLevel.."-"..MaxLevel)
				end
			end
			if wnd.m_AreaStateTime > 0 then
				wnd.m_AreaStateTime = wnd.m_AreaStateTime - 1000
			end
		end
	end

	-- 注册每秒钟更新ToolTips的Tick
	if self.m_AreaStateCountDownTick then
		UnRegisterTick(self.m_AreaStateCountDownTick)
		self.m_AreaStateCountDownTick = nil
	end

	-- 如果在表 PkRuleInfo_Common 里填了才注册tick
	if self.m_AreaName then
		local pkRuleKeys = PkRuleInfo_Common:GetKeys()
		for i, pkRuleAreaTemp in pairs(pkRuleKeys) do
			local pkRuleTemp = PkRuleInfo_Common(pkRuleAreaTemp)
			local tempKeys = pkRuleTemp:GetKeys()
			for _, temp in pairs(tempKeys) do
				if temp == SceneName then
					--print("------------------------注册Tick")
					self.m_AreaStateCountDownTick = RegisterTick("m_AreaStateCountDownTick", CountDownTick, 1000)
					break
				end
			end
			if self.m_AreaStateCountDownTick then
				break
			end
		end
	end
end


local function UpdateAreaState(Wnd, AreaName, AreaState, Time)
	-- 显示状态图标,状态改变了才重新显示
	if g_GameMain.m_AreaInfoWnd.m_AreaLastState[AreaName] ~= AreaState then
		if AreaState then 
			local TexSrc = g_ImageMgr:GetImagePath(1358)
			AddImage(Wnd.m_StateWnd, TexSrc)
		else
			local TexSrc = g_ImageMgr:GetImagePath(1357)
			AddImage(Wnd.m_StateWnd, TexSrc)
		end
	end
	-- 记录时间
	Wnd.m_AreaState = AreaState
	Wnd.m_AreaStateTime = Time
	-- 增加进更新列表
	--table.insert(g_GameMain.m_AreaInfoWnd.AreaStateList, Wnd)
	if not g_GameMain.m_AreaInfoWnd.AreaStateList[AreaName] then
		g_GameMain.m_AreaInfoWnd.AreaStateList[AreaName] = Wnd
	end

	--Wnd:SetFocus()
end

-- 在中地图上显示区域Pk状态
function CAreaInfoWnd:ShowAreaPkOnMiddleMap(SceneName, AreaName, AreaState, Time, EternalState)
	v = g_SceneAreaList[SceneName][AreaName]
	if v[1] and self.m_SceneMapWnd.m_ScenePictureWnd then
		local Wnd = self.m_SceneMapWnd.m_ScenePictureWnd["m_AreaBtn"..v[1]]
		if Wnd and Wnd.m_AreaStateWnd then
			if EternalState == 0 then
				UpdateAreaState(Wnd.m_AreaStateWnd, AreaName, AreaState, Time)
			elseif EternalState == 1 then
				
				local MinLevel = 0
				local MaxLevel = 0
				local pkRuleKeys = PkRuleInfo_Common:GetKeys()
				for i, pkRuleAreaTemp in pairs(pkRuleKeys) do
					local pkRuleTemp = PkRuleInfo_Common(pkRuleAreaTemp)
					local tempKeys = pkRuleTemp:GetKeys()
					for _, temp in pairs(tempKeys) do
						if temp == SceneName then
							MinLevel = pkRuleTemp(temp, "MinLevel")
							MaxLevel = pkRuleTemp(temp, "MaxLevel")
						end
					end
				end
				Wnd.m_AreaStateWnd.m_StateWnd:SetMouseOverDescAfter("#cffeaab"..GetStaticTextClient(19018).."#r"..GetStaticTextClient(19022)..":"..MinLevel.."-"..MaxLevel)
			end
		end
	end
end


function CAreaInfoWnd:ShowRobResWinOnMiddleMap(sceneName, objName, tongName, tongCamp)

	local wnd = self.m_SceneMapWnd["m_ScenePictureWnd"].m_NpcSignBtnTbl[objName]
	local TexSrc = g_ImageMgr:GetImagePath(40000)
	if tongCamp == 1 then
		TexSrc = g_ImageMgr:GetImagePath(40001)
	elseif tongCamp == 2 then
		TexSrc = g_ImageMgr:GetImagePath(40002)
	elseif tongCamp == 3 then
		TexSrc = g_ImageMgr:GetImagePath(40003)	
	end
	local proffer = FbRobRes_Server(objName, "Modulus") *100* 0.1 *60 /20
	local money = FbRobRes_Server(objName, "AwardMoney")
	money = g_MoneyMgr:ChangeMoneyToString(money, EGoldType.GoldCoin)
	if wnd and wnd[1] then
		AddImage(wnd[1], TexSrc)
		local ShowTips = nil
		local lanObjName = GetIntObjDispalyName(objName)
		if tongName ~= "" then
			ShowTips = ToolTipsText(1220, lanObjName, tongName, proffer, money)
			wnd[1].m_NpcSignBtn:SetMouseOverDescAfter(ShowTips)
			wnd[1]:SetMouseOverDescAfter(ShowTips)
			return
		end
		ShowTips = ToolTipsText(1221, lanObjName, proffer, money)
		wnd[1].m_NpcSignBtn:SetMouseOverDescAfter(ShowTips)
		wnd[1]:SetMouseOverDescAfter(ShowTips)
	end
end

function CAreaInfoWnd:BornNpcOnMiddleMap(Conn, SceneName, NpcName, PosX, PosY)
	self:ShowBornNpcOnMiddleMap(SceneName, NpcName, PosX, PosY)
end

function CAreaInfoWnd:AreaPkOnMiddleMap(Conn, SceneName, AreaName, AreaState, Time, EternalState)
	if self:IsShow() then
		self:ShowAreaPkOnMiddleMap(SceneName, AreaName, not AreaState, Time, EternalState)
	end
	self.m_AreaLastState[AreaName] = not AreaState
end

function CAreaInfoWnd:RobResWinOnMiddleMap(Conn, sceneName, objName, tongName, tongCamp)
	if self:IsShow() then
		self:ShowRobResWinOnMiddleMap(sceneName, objName, tongName, tongCamp)
	end
end

-- 中地图上显示帮会驻地状态
function CAreaInfoWnd:ShowWarZoneState(SceneName)
	local SceneType = Scene_Common[SceneName].SceneType
	if SceneType == 7 then
		if not self.m_IsGetWarZoneState then
			Gac2Gas:GetWarZoneState(g_Conn, SceneName)
		else
			self:CreateAllWarZoneWnd()
		end
	end
end

function CAreaInfoWnd:SaveWarZoneState(stationID, campID, tongName)
	if not self.m_WarZoneState then
		self.m_WarZoneState = {}
	end
	
	if not self.m_WarZoneState[stationID] then
		self.m_WarZoneState[stationID] = {}
	end
	
	self.m_WarZoneState[stationID].m_CampID = campID
	self.m_WarZoneState[stationID].m_TongName = tongName
end

function CAreaInfoWnd:CreateAllWarZoneWnd()
	for stationID, info in pairs(self.m_WarZoneState) do
		if not self.m_WarZoneState[stationID].m_Wnd 
			or self.m_WarZoneState[stationID].m_OccupyedName ~= info.m_TongName 
			or self.m_WarZoneState[stationID].m_OccupyedCamp ~= info.m_CampID then
	
			local Wnd = self.m_WarZoneState[stationID].m_Wnd
			if Wnd then
				Wnd:DestroyWnd()
			end
			
			self:CreateNewWnd(stationID, info.m_CampID, info.m_TongName)
		end
	end
end

function CAreaInfoWnd:EndSendWarZoneState()
	self.m_IsGetWarZoneState = true
	self:CreateAllWarZoneWnd()
end

-- 创建
function CAreaInfoWnd:CreateNewWnd(stationID, campID, tongName)			
	local Parent = self.m_SceneMapWnd["m_ScenePictureWnd"]
	local NewWnd = CWarZoneStateBtn:new(Parent, stationID, campID, tongName)
	
	self.m_WarZoneState[stationID].m_Wnd = NewWnd
	self.m_WarZoneState[stationID].m_OccupyedName = tongName
	self.m_WarZoneState[stationID].m_OccupyedCamp = campID

	if self:IsShow() then
		NewWnd:ShowWnd(true)
	end
end

-- 被占领后 重新显示驻地图标
function CAreaInfoWnd:UpdateWarZoneState(Conn, stationID, campID, tongName)
	if self.m_IsGetWarZoneState and self.m_WarZoneState and self.m_WarZoneState[stationID] then
		local Wnd = self.m_WarZoneState[stationID].m_Wnd
		if Wnd and Wnd:IsShow() then
			Wnd:DestroyWnd()
			self:CreateNewWnd(stationID, campID, tongName)
		end
	end
	self:SaveWarZoneState(stationID, campID, tongName)
end

-- 选中中地图上所有菜单 
function CAreaInfoWnd:SelectAllMiddleMapMenu()
	local JobNum = g_MainPlayer:CppGetClass()
	local PlayerID = g_MainPlayer.m_uID
	local strpath = g_RootPath .. "var/gac/AreaSelect"..PlayerID..".txt"
	--print("strpath"..strpath)
	local fo = assert(CLuaIO_Open(strpath, "w+"))
	fo:WriteString("")
	fo:Close()
	local f = assert(CLuaIO_Open(strpath, "a"))
	f:WriteString("all")
	--print("SaveAreaSelect>>>>AreaSelect"..AreaSelect)
	f:Close()
end

function CAreaInfoWnd:SendFbPlayerPos(Conn, MemberId, SceneName, Gridx, Gridy, MemberName, CampID, isLive)
	if not self.m_FbPlayerWndTbl[MemberId] then
		self.m_FbPlayerWndTbl[MemberId] = {}
	end
	if self.m_FbPlayerWndTbl[MemberId].m_Pos then
		local sName = self.m_FbPlayerWndTbl[MemberId].m_Pos[1]
		local x = self.m_FbPlayerWndTbl[MemberId].m_Pos[2]
		local y = self.m_FbPlayerWndTbl[MemberId].m_Pos[3]
		if sName == SceneName and Gridx == x and Gridy == y 
			and (self.m_IsAliveFlag and self.m_IsAliveFlag == isLive) then
			return
		end
	end
	self.m_FbPlayerWndTbl[MemberId].m_Pos = {SceneName,Gridx,Gridy}
	if self:IsShow() then
		self:InitFbPlayerWndPos(MemberId, MemberName, CampID, isLive)
	else
		Gac2Gas:StopGetFbPlayerPos(g_Conn)
	end
	self.m_IsAliveFlag = isLive
end
