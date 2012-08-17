gac_require "relation/association/AssociationChatBaseInc"

local fl_canSendMaxLength	= 1024
local fl_nCutStringNum		= 20
local fl_nMaxStringNum		= 40

function CAssociationChatBase:InitEx()
	self.m_tblTextRecord	= {}
	self.m_tblTextRecord.n	= 0
	self.m_SelfName			= g_MainPlayer.m_Properties:GetCharName()
end

function CAssociationChatBase:ResetText()
	if(g_GameMain.m_AssociationBase.m_IsRecChatMsg) then
		local f = CLuaIO_Open(self.m_RecordPath, "ab")
		for i = 1, fl_nCutStringNum do
			f:WriteString(self.m_tblTextRecord[1] .. "\n")
			table.remove(self.m_tblTextRecord, 1)
		end
		f:Close()
	else
		for i = 1, fl_nCutStringNum do
			table.remove(self.m_tblTextRecord, 1)
		end
	end
	
	self.m_tblTextRecord.n = self.m_tblTextRecord.n - fl_nCutStringNum
	local sText = table.concat(self.m_tblTextRecord, "#r")
	self.m_OutPutText:SetWndText(sText)
end

function CAssociationChatBase:RecordRestOfText()
	if(g_GameMain.m_AssociationBase.m_IsRecChatMsg) then
		local f = CLuaIO_Open(self.m_RecordPath, "ab")
		for i, v in ipairs(self.m_tblTextRecord) do
			f:WriteString(v .. "\n")
		end
		f:Close()
		self.m_tblTextRecord = {}
	end
end

function CAssociationChatBase:SendMsgCommon(str)
	local text = ""
	if(str) then
		text = str
	else
		text = self.m_InputText:GetWndText()
	end
	text = self.m_TextFilter:ReplaceInvalidChar(text)
	if("" == text) then
		MsgClient(5003)
		return
	elseif(string.len(text) > fl_canSendMaxLength) then
		self.m_OutPutText:AddNewLineText(g_ColorMgr:GetColor("好友聊天提示") .. GetStaticTextClient(2001, fl_canSendMaxLength))
		return
	end
	self.m_InputText:SetWndText("")
	self:AddNewLineInfoWithTime(self.m_SelfName, text)
	return text
end

function CAssociationChatBase:AddNewLineInfoWithTime(fromName, text, time)
	local sTime = time and time or os.date("%X")
	local colorNameAndTime = g_ColorMgr:GetColor("好友聊天名") .. fromName .. "(".. sTime .. "):#r"
	local colorText        = g_ColorMgr:GetColor("好友聊天内容") .. text
	self:AddNewLineInfo(colorNameAndTime .. colorText)
end

function CAssociationChatBase:AddNewLineInfo(text)
	if(fl_nMaxStringNum <= self.m_tblTextRecord.n) then
		self:ResetText()
	end
	table.insert(self.m_tblTextRecord, text)
	self.m_tblTextRecord.n = self.m_tblTextRecord.n + 1
	self.m_OutPutText:AddNewLineText(text)
end

function CAssociationChatBase:VirtualExcludeWndClosed(bFlag)
	if(bFlag) then return end
	self:DestroyWnd()
	self:CloseWnd()
end

----------------------------------------------------------------------------------
function CAssociationPublicChatListItemBase:Init(parent)
	self:CreateFromRes("AssociationPublicChatListItem", parent)
	self:ShowWnd( true )
end

function CAssociationPublicChatListItemBase:OnChildCreated()
	self.m_Icon	= self:GetDlgChild("Icon")
	self.m_Name	= self:GetDlgChild("Name")
end

function CAssociationPublicChatListItemBase:SetShowTextColor(color)
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
	self.m_Icon:SetWndTextColor(Flag, color)
	self.m_Name:SetWndTextColor(Flag, color)
end

function CAssociationPublicChatListItemBase:SetName(name, beOnline)
	if(1 == beOnline) then
		self:SetShowTextColor("0xffffffff")
	else
		self:SetShowTextColor("0xff777777")
	end
	self.m_Name:SetWndText(name)
end
