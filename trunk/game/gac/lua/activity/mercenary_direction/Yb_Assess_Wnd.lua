--lan_load "quest/Lan_MercenaryLevelAward_Client"
--cfg_load "quest/pos_MercenaryLevelAward_Client"
--cfg_load_multi "quest/MercenaryLevelAward_Client"

CYbAssessWnd = class(SQRDialog)
local CYbAssessProgressWnd = class(SQRDialog)
local CYbAssessAwardWnd = class(SQRDialog)

--g_MercenaryLevelAwardMgr = {}
--g_MercenaryLevelAwardMgr.m_MercLevAward = {}
--g_MercenaryLevelAwardMgr.m_MercLevAwardSort = {}

--function MercenaryLevelAwardClient()
--	for i=1, #(MercenaryLevelAward_Client) do
--		local tbl = MercenaryLevelAward_Client[i]
--		local LevelName = tbl.LevelName
--		local FieldName = tbl.FieldName
--		local Type = tbl.Type
--		local TexPath = tbl.TexPath
--		
--		--1.排序用
--		if not g_MercenaryLevelAwardMgr.m_MercLevAwardSort[Type] then
--			g_MercenaryLevelAwardMgr.m_MercLevAwardSort[Type] = {}
--		end
--		if not g_MercenaryLevelAwardMgr.m_MercLevAwardSort[Type][LevelName] then
--			g_MercenaryLevelAwardMgr.m_MercLevAwardSort[Type][LevelName] = {}
--		end
--		table.insert(g_MercenaryLevelAwardMgr.m_MercLevAwardSort[Type][LevelName],FieldName)
--		
--		--2.内容信息
--		if not g_MercenaryLevelAwardMgr.m_MercLevAward[LevelName] then
--			g_MercenaryLevelAwardMgr.m_MercLevAward[LevelName] = {}
--		end
--		if not g_MercenaryLevelAwardMgr.m_MercLevAward[LevelName][FieldName] then
--			g_MercenaryLevelAwardMgr.m_MercLevAward[LevelName][FieldName] = {}
--		end
--		g_MercenaryLevelAwardMgr.m_MercLevAward[LevelName][FieldName].m_TexPath = TexPath
--	end
--end

--MercenaryLevelAwardClient()

local function GetMercenaryLevelTxt(Level, Field, Subject)
	local index = MemH64(Level..Field..Subject)
	if Lan_MercenaryLevelDirect_Common(index) then
		return Lan_MercenaryLevelDirect_Common(index,"Describe")
	end
end

function CYbAssessWnd:Ctor()
	self:CreateFromRes("yb_pd_wnd", g_GameMain) 
	self.m_CloseBtn = self:GetDlgChild("Close")
	self.m_YbLevelPic = self:GetDlgChild("Yb_Level")
	self.m_LevelInfoText = self:GetDlgChild("LevelInfo")
	
	self.m_InfoListCtrl = self:GetDlgChild("ListCtrl_Info")
	self.m_AwardListCtrl = self:GetDlgChild("ListCtrl_Award")
	self.m_ActionListCtrl = self:GetDlgChild("ListCtrl_Action")
	
	self.m_CheckTab = {}
	for i=1, 10 do
		self.m_CheckTab[i] = self:GetDlgChild("check"..i)
	end
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end


local function AddImage(Wnd,strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

--设置佣兵的等级
function CYbAssessWnd:InitLevelWnd(MercenaryLevel)
	if MercenaryLevel then
		local LevelName = "新兵入伍"
		if MercenaryLevel-1 >= 0 then
			LevelName = g_MercenaryLevelTbl[MercenaryLevel-1].LevelName
		end
		
		local strTex = g_MercenaryLevelAwardMgr.m_MercLevAward[LevelName]["佣兵描述"].m_TexPath
		AddImage(self.m_YbLevelPic,strTex)
		
		local str = Lan_MercenaryLevelAward_Client(MemH64(LevelName.."佣兵描述"), "Text")
		str = loadstring("return "..str)()
		self.m_LevelInfoText:SetWndText(str)
	end
end

--设置佣兵的Tab标签
function CYbAssessWnd:InitTabWnd()
	local MercenaryLevel = g_MainPlayer.m_uMercenaryLevel
	if MercenaryLevel then
		local SelNum = MercenaryLevel + 1
		if SelNum > 10 then SelNum = 10 end
			
		for i=1, 10 do
			if i <= SelNum then
				self.m_CheckTab[i]:EnableWnd(true)
				self.m_CheckTab[i]:ShowWnd(true)
			else
				self.m_CheckTab[i]:EnableWnd(false)
				self.m_CheckTab[i]:ShowWnd(false)
			end
		end
		
		self.m_CheckTab[SelNum]:SetCheck(true)
	end
end

--==========================================
--设置佣兵的显示信息
function CYbAssessWnd:InitListCtrlWnd()
	self.m_InfoListCtrl:DeleteAllItem()
	self.m_InfoListCtrl:InsertColumn(0, self.m_InfoListCtrl:GetWndOrgWidth())
	
	self.m_AwardListCtrl:DeleteAllItem()
	self.m_AwardListCtrl:InsertColumn(0, self.m_AwardListCtrl:GetWndOrgWidth())
	
	self.m_ActionListCtrl:DeleteAllItem()
	self.m_ActionListCtrl:InsertColumn(0, self.m_ActionListCtrl:GetWndOrgWidth())
end

function CYbAssessWnd:CreateProgressWnd(Parent)
	local Wnd = CYbAssessProgressWnd:new()
	Wnd:CreateFromRes("yb_pd_progress_wnd",Parent) 
	Wnd.m_Name = Wnd:GetDlgChild("Name")
	Wnd.m_Progress = Wnd:GetDlgChild("Progress")
	
	Wnd.m_Progress:SetProgressMode(0)
	Wnd.m_Progress:SetRange(100)
	Wnd.m_Progress:SetPos(0)
	
	Wnd:ShowWnd(true)
	return Wnd
end

--改变进度条的颜色
function CYbAssessProgressWnd:SetProgressWnd()
	local FildPath = g_ImageMgr:GetImagePath(1337)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_FILL

	local DefaultImage = self.m_Progress:GetWndBkImage(Flag)
	if not DefaultImage then
		DefaultImage = WND_IMAGE_LIST:new()
	end
	
	DefaultImage:AddImage( self.m_Progress:GetGraphic(), -1,  FildPath, nil, CFPos:new(), "0xffffffff", 16 ) 	
	self.m_Progress:SetWndBkImage( Flag, DefaultImage )
end

function CYbAssessWnd:InsertListInfo(RowName, Percent)
	local ItemCount = self.m_InfoListCtrl:GetItemCount()
	self.m_InfoListCtrl:InsertItem(ItemCount, 50)
	local Item = self.m_InfoListCtrl:GetSubItem(ItemCount, 0)
	Item:SetMouseWheel(true)
	
	local itemWnd = self:CreateProgressWnd(Item)
	itemWnd.m_Name:SetWndText(RowName)
	itemWnd.m_Progress:SetPos(Percent*100)
	if Percent >= 1 then
		itemWnd:SetProgressWnd()
	end
	Item.m_Wnd = itemWnd
end
--==============================================

--=============================================
local function GetLinkPos(TextInfo)
	local LinkTbl = {}
	local FindStr = "%b{}"
	local _,num = string.gsub(TextInfo,FindStr,"")--得到有多少个,
	
	for i = 1, num do
		local sTblInfo = string.match(TextInfo,FindStr)
		if sTblInfo then
			local num = string.find(sTblInfo,",")
			local strHipeLink = string.sub(sTblInfo,2,num-1)
			local LinkNum = tonumber(string.sub(sTblInfo,num+1,string.len(sTblInfo)-1))
			
			local linkstr = AutoTrackColorStr.."#u#l"..strHipeLink.."#i["..MemH64(strHipeLink).."]#l#u#W"
			TextInfo = string.gsub(TextInfo, "%b{,"..LinkNum.."}", linkstr)
			if not LinkTbl[strHipeLink] then
				local tbl = {
					pos_MercenaryLevelAward_Client(LinkNum, "SceneName"),
					pos_MercenaryLevelAward_Client(LinkNum, "PosX"),
					pos_MercenaryLevelAward_Client(LinkNum, "PosY")
				}
				LinkTbl[strHipeLink] = tbl
			end
		end
	end
	
	return TextInfo,LinkTbl
end

function CYbAssessAwardWnd:AddPic(LevelName, FieldName)
	local PicPath = g_MercenaryLevelAwardMgr.m_MercLevAward[LevelName][FieldName].m_TexPath
	AddImage(self.m_Pic,PicPath)
	
	local TextInfo = Lan_MercenaryLevelAward_Client(MemH64(LevelName..FieldName), "Text")
	TextInfo = loadstring("return {"..TextInfo.."}")()
	
	local camp = g_MainPlayer:CppGetBirthCamp()
	if TextInfo[camp] then
		local str = ""
		str, self.m_HypeLink = GetLinkPos(TextInfo[camp])
		self.m_Text:SetWndText(str)
	else
		self.m_Text:SetWndText("")
	end
end

function CYbAssessWnd:CreateAwardWnd(parent)
	local Wnd = CYbAssessAwardWnd:new()
	Wnd:CreateFromRes("yb_pd_award_wnd",parent) 
	--Wnd.m_Button = Wnd:GetDlgChild("Btn")
	Wnd.m_Pic = Wnd:GetDlgChild("Pic")
	Wnd.m_Text = Wnd:GetDlgChild("Text")
--	local rt = CFRect:new()
--	Wnd.m_Pic:GetLogicRect(rt)
--	Wnd.m_Button:SetLogicRect(rt)
--	Wnd.m_Button:SetFocus()
	
	Wnd:ShowWnd(true)
	return Wnd
end

function CYbAssessAwardWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == RICH_CLICK then
		if self.m_IsEnable then
	  	local value = self.m_Text:GetChooseStrAttr()
	  	local linktbl = self.m_HypeLink[value]
			if linktbl then
				PlayerAutoTrack(value,linktbl[1],linktbl[2],linktbl[3])
			end
		else
			MsgClient(299910)--"等级不够,不能寻路"
		end
	end
end

function CYbAssessWnd:InsertAwardListInfo(MercenaryLevelTbl, AwardTypeName, IsEnable)
	local ItemCount = self.m_AwardListCtrl:GetItemCount()
	self.m_AwardListCtrl:InsertItem(ItemCount, 91)
	local Item = self.m_AwardListCtrl:GetSubItem(ItemCount, 0)
	Item:SetMouseWheel(true)
	
	local itemWnd = self:CreateAwardWnd(Item)
	itemWnd:AddPic(MercenaryLevelTbl.LevelName, AwardTypeName)
	itemWnd.m_IsEnable = IsEnable
	
	Item.m_Wnd = itemWnd
end

function CYbAssessWnd:InsertActionListInfo(LevelName, FieldName)
	local ItemCount = self.m_ActionListCtrl:GetItemCount()
	self.m_ActionListCtrl:InsertItem(ItemCount, 91)
	local Item = self.m_ActionListCtrl:GetSubItem(ItemCount, 0)
	Item:SetMouseWheel(true)
	
	local itemWnd = self:CreateAwardWnd(Item)
	itemWnd:AddPic(LevelName, FieldName)
	itemWnd.m_IsEnable = true
	
	Item.m_Wnd = itemWnd
end
--===========================================

function CYbAssessWnd:ShowOldMercLevInfo(MercLevNum)
	local MercenaryLevelTbl = g_MercenaryLevelTbl[MercLevNum]
	local MercenaryLevelSortTbl = g_MercenaryLevelSortTbl[MercLevNum]
	for i=1,#(MercenaryLevelSortTbl["达成条件"]) do
		local showName = GetMercenaryLevelTxt(MercenaryLevelTbl.LevelName, "达成条件", MercenaryLevelSortTbl["达成条件"][i])
		self:InsertListInfo(showName.."("..GetStaticTextClient(190100)..")", 1)
	end
	for Filed,Tbl in pairs(MercenaryLevelTbl) do
		if Filed == "挑战条件" then
			self:InsertListInfo(GetStaticTextClient(1183).."("..GetStaticTextClient(190100)..")", 1)
		end
	end
	
	--显示特权的图标
	for i=1,#(MercenaryLevelSortTbl["特权"]) do
		self:InsertAwardListInfo(MercenaryLevelTbl,MercenaryLevelSortTbl["特权"][i],true)
	end
	
	--显示行为的图标
	local LevelName = "新兵入伍"
	if MercLevNum-1 >= 0 then
		LevelName = g_MercenaryLevelTbl[MercLevNum-1].LevelName
	end
	local MercLevAwardSortTbl = g_MercenaryLevelAwardMgr.m_MercLevAwardSort[2][LevelName]
	for i=1, #(MercLevAwardSortTbl) do
		self:InsertActionListInfo(LevelName, MercLevAwardSortTbl[i])
	end
end

function CYbAssessWnd:InitCurMercLevInfo()
	local MercenaryLevel = g_MainPlayer.m_uMercenaryLevel
	
	if MercenaryLevel >= g_MercenaryLevelTbl.MaxLevel then
		self:ShowOldMercLevInfo(MercenaryLevel-1)
		return
	end
	
	local Wnd = g_GameMain.m_MercenaryLevelTrace
	local State = Wnd.m_MercenLevState.m_State
	local MercenaryLevelTbl = g_MercenaryLevelTbl[MercenaryLevel]
	local MercenaryLevelSortTbl = g_MercenaryLevelSortTbl[MercenaryLevel]
	
	if State >= 1 then
		for i=1,#(MercenaryLevelSortTbl["达成条件"]) do
			local Subject = MercenaryLevelSortTbl["达成条件"][i]
			local NumTbl = Wnd.m_MercenLevState["达成条件"][Subject]
			local showName = GetMercenaryLevelTxt(MercenaryLevelTbl.LevelName, "达成条件", Subject)
			local str = ""
			if State == 1 then
				str = showName..": ("..NumTbl[1].."/"..NumTbl[2]..")"
				self:InsertListInfo(str, NumTbl[1]/NumTbl[2])
			else
				str = showName..": ("..GetStaticTextClient(190100)..")"
				self:InsertListInfo(str, 1)
			end
		end
	end
	
	if State >= 2 then
		for Filed,Tbl in pairs(MercenaryLevelTbl) do
			if Filed == "挑战条件" then
				if State == 2 then
					self:InsertListInfo(GetStaticTextClient(1183), 0.05)
				else
					self:InsertListInfo(GetStaticTextClient(1183).."("..GetStaticTextClient(190100)..")", 1)
				end
			end
		end
	end
	
	if State >= 4 then
		for i=1,#(MercenaryLevelSortTbl["特权"]) do
			self:InsertAwardListInfo(MercenaryLevelTbl,MercenaryLevelSortTbl["特权"][i],true)
		end
	else
		for i=1,#(MercenaryLevelSortTbl["特权"]) do
			self:InsertAwardListInfo(MercenaryLevelTbl,MercenaryLevelSortTbl["特权"][i],false)
		end
	end
	
	--显示行为的图标
	local LevelName = "新兵入伍"
	if MercenaryLevel-1 >= 0 then
		LevelName = g_MercenaryLevelTbl[MercenaryLevel-1].LevelName
	end
	local MercLevAwardSortTbl = g_MercenaryLevelAwardMgr.m_MercLevAwardSort[2][LevelName]
	for i=1, #(MercLevAwardSortTbl) do
		self:InsertActionListInfo(LevelName, MercLevAwardSortTbl[i])
	end
end

function CYbAssessWnd:OpenYbAssessWnd()
	if IsFunction(self.IsShow)	and self:IsShow() then
		self:ShowWnd(false)
	else
		local Wnd = g_GameMain.m_MercenaryLevelTrace
		local State = Wnd.m_MercenLevState.m_State
		if State then
			self:InitLevelWnd(g_MainPlayer.m_uMercenaryLevel)
			self:InitTabWnd()
			self:InitListCtrlWnd()
			self:InitCurMercLevInfo()
			
			self:ShowWnd(true)
			--self:ShowWnd(true)
		end
	end
end

function CYbAssessWnd:ShowAllInfo(SelTabNum)
	local MercenaryLevel = g_MainPlayer.m_uMercenaryLevel
	if SelTabNum <= MercenaryLevel then
		self:ShowOldMercLevInfo(SelTabNum-1)
	else
		self:InitCurMercLevInfo()
	end
end

function CYbAssessWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			--关闭窗口(打开寻路窗口)
			self:ShowWnd(false)
			--self:ShowWnd(false)
		else
			for i=1, 10 do
				if Child == self.m_CheckTab[i] then
					self:InitLevelWnd(i-1)
					self:InitListCtrlWnd()
					self:ShowAllInfo(i)
					return
				end
			end
		end
	end
end

function CYbAssessWnd:VirtualExcludeWndClosed()
	--窗口关闭后(打开寻路窗口)
	if g_GameMain.m_MercenaryDirWnd.m_ClickBtn then
		g_GameMain.m_MercenaryDirWnd.m_ClickBtn = nil
		g_GameMain.m_MercenaryLevelTrace:ShowWnd(true)
	end
end

function CYbAssessWnd:RetShowOpenYbAssessWnd()
	if not (IsCppBound(g_Conn) and g_MainPlayer) then
		return
	end
	self:OpenYbAssessWnd()
end
