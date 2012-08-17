--gac_gas_require "activity/quest/MercenaryLevelCfg"
--lan_load "quest/Lan_MercenaryLevelAward_Client"
--cfg_load "quest/pos_MercenaryLevelAward_Client"

CMercenaryLevelAward = class(SQRDialog)
local CMercenaryLevelAwardItem = class(SQRDialog)

local AwardInfoTbl = {
	["开放等级"] = 	{type="other", textnum=190107},
	["称号"] = 			{type="other", textnum=190105},
	["经验提升"] = 	{type="other", textnum=190107},
	["列表传送"] = 	{type="other", textnum=190107},
	["行为"] = 			{type="other", textnum=190107},
	["敬礼"] = 			{type="other", textnum=190107},
	["去掉CD"] = 		{type="other", textnum=190107},
	["技能"] = 			{type="skill", textnum=190106},
	["物品"] = 			{type="item", textnum=190105},
}

function CMercenaryLevelAward:Ctor()
	self:CreateFromRes("MercenaryLevelAward", g_GameMain)
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_AwardListCtrl = self:GetDlgChild("AwardListCtrl")
	self.FinishAwards = {}
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CreateMercenaryLevelAwardItem(parent, MercenaryLevelTbl, Award)
	local Wnd = CMercenaryLevelAwardItem:new()
	Wnd:CreateFromRes("MercenaryLevelAwardItem", parent)
	Wnd.m_GetAwardBtn = Wnd:GetDlgChild("GetAwardBtn")
	Wnd.m_AwardImage = Wnd:GetDlgChild("AwardImage")
	Wnd.m_Text = Wnd:GetDlgChild("Text")
	
	if AwardInfoTbl[Award].type == "item" then
		local ItemType = MercenaryLevelTbl["特权"][Award].Arg[1]
		local ItemName = MercenaryLevelTbl["特权"][Award].Arg[2]
		Wnd:SetTip(AwardInfoTbl[Award].type, ItemType, ItemName)
	elseif AwardInfoTbl[Award].type == "skill" then
		local SkillName = MercenaryLevelTbl["特权"][Award].Arg
		Wnd:SetTip(AwardInfoTbl[Award].type, SkillName)
	else
		Wnd:SetTip(AwardInfoTbl[Award].type, MercenaryLevelTbl.LevelName, Award)
	end
	return Wnd
end

function CMercenaryLevelAward:AddAwardItem(ItemNum, MercenaryLevelTbl, Award)
	self.m_AwardListCtrl:InsertItem(ItemNum, 50)
	local Item = self.m_AwardListCtrl:GetSubItem(ItemNum, 0)
	local ItemWnd = CreateMercenaryLevelAwardItem(Item, MercenaryLevelTbl, Award)
	self.m_AwardListCtrl:SetItemHeight(ItemWnd:GetWndOrgHeight(), ItemNum)
	Item:SetMouseWheel(true)
	Item.m_Wnd = ItemWnd
end

function CMercenaryLevelAwardItem:SetImage(strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(self.m_AwardImage:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	self.m_AwardImage:SetWndBkImage( Flag, DefaultImage )
	self.m_AwardImage:SetWndText("")
end

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

function CMercenaryLevelAwardItem:SetTip(Type, Arg1, Arg2)
	--物品第一个参数是类型,第二个参数是名字
	--技能第一个参数是技能名
	--其它的第一个参数和第二个参数是另一个配置表中的关键字
	if Type == "item" then
		g_SetItemRichToolTips(self.m_AwardImage, Arg1, Arg2, 0)
	elseif Type == "skill" then
		local tips = g_Tooltips:GetWndSkillTips(Arg1,0)
		self.m_AwardImage:SetMouseOverDescAfter(tips)
	elseif Type == "other" then
		local image = WND_IMAGE_LIST:new()
		local PicPath = g_MercenaryLevelAwardMgr.m_MercLevAward[Arg1][Arg2].m_TexPath
		local TextInfo = Lan_MercenaryLevelAward_Client(MemH64(Arg1..Arg2), "Text")
		TextInfo = loadstring("return {"..TextInfo.."}")()
		local str = ""
		local camp = g_MainPlayer:CppGetBirthCamp()
		if TextInfo[camp] then
			str,_ = GetLinkPos(TextInfo[camp])
		end
		image:AddImageWin( self.m_AwardImage:GetGraphic(), -1,  PicPath, nil, CFPos:new(), "0xffffffff", 0 )
		self.m_AwardImage:SetMouseOverRichTextImageForAllWnd(1, image)
		self.m_AwardImage:SetMouseOverDescAfter("#1"..str)
	end
end

function CMercenaryLevelAwardItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if Child == self.m_GetAwardBtn then
			Gac2Gas:SetMercenaryLevelAward(g_Conn, self.m_Award)
		end
	end
end

function CMercenaryLevelAward:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if Child == self.m_CancelBtn then
			Gac2Gas:MercenaryLevelUp(g_Conn)
			self:ShowWnd(false)
		end
	end
end

function ShowMercenaryLevelAward()
	if not (IsCppBound(g_Conn) and g_MainPlayer) then
		return
	end
	Gac2Gas:GetMercenaryLevelAward(g_Conn, true)
end

function CMercenaryLevelAward:SetMercenaryLevelFinishAwards(Award)
	self.FinishAwards[Award] = true
end

function CMercenaryLevelAward:ClearMercenaryLevelFinishAwards()
	self.FinishAwards = {}
end

function CMercenaryLevelAward:RetShowMercenaryLevelAward()
	if not (IsCppBound(g_Conn) and g_MainPlayer) then
		return
	end
	if g_MainPlayer.m_uMercenaryLevel == g_MercenaryLevelTbl.MaxLevel then
		return
	end
	
	local Wnd = g_GameMain.m_MercenaryLevelAward
	Wnd.m_AwardListCtrl:DeleteAllItem()
	Wnd.m_AwardListCtrl:InsertColumn(0, Wnd.m_AwardListCtrl:GetWndOrgWidth())
	
	local ItemNum = 0
	local LevelTbl = g_MercenaryLevelTbl[g_MainPlayer.m_uMercenaryLevel]
	for Award,_ in pairs(LevelTbl["特权"]) do
		local PicStr = g_MercenaryLevelAwardMgr.m_MercLevAward[LevelTbl.LevelName][Award].m_TexPath
		local AwardType = Award
		if string.sub(Award, 1, 4)=="技能" or string.sub(Award, 1, 4)=="物品" or string.sub(Award, 1, 4)=="行为" then
			AwardType = string.sub(Award, 1, 4)
		end
		local AwardText = Lan_MercenaryLevelAward_Client(MemH64(LevelTbl.LevelName..Award), "Text")
		AwardText = loadstring("return {"..AwardText.."}")()
		local Camp = g_MainPlayer:CppGetBirthCamp()
		Wnd:AddAwardItem(ItemNum, LevelTbl, AwardType)
		local Item = Wnd.m_AwardListCtrl:GetSubItem(ItemNum, 0)
		Item.m_Wnd.m_Text:SetWndText(AwardText[Camp])
		Item.m_Wnd.m_GetAwardBtn:SetWndText( GetStaticTextClient(AwardInfoTbl[AwardType].textnum) )
		if g_GameMain.m_MercenaryLevelAward.FinishAwards[Award] then
			Item.m_Wnd.m_GetAwardBtn:EnableWnd(false)
		end
		Item.m_Wnd.m_Award = Award
		Item.m_Wnd:SetImage(PicStr)
		Item.m_Wnd:ShowWnd(true)
		ItemNum = ItemNum+1
	end
	Wnd:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(Wnd)
end

function CMercenaryLevelAward:RetUpdateMercenaryLevelAwardItem(Award)
	if not (IsCppBound(g_Conn) and g_MainPlayer) then
		return
	end
	local Wnd = g_GameMain.m_MercenaryLevelAward
	local Count = Wnd.m_AwardListCtrl:GetItemCount()
	for i=0,Count-1 do
		local Item = Wnd.m_AwardListCtrl:GetSubItem(i, 0)
		if Item.m_Wnd.m_Award == Award then
			g_GameMain.m_MercenaryLevelAward.FinishAwards[Award] = true
			Item.m_Wnd.m_GetAwardBtn:EnableWnd(false)
			break
		end
	end
end
