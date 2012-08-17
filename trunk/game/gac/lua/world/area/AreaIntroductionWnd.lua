lan_load "area/Lan_AreaInfo_Client"

local IntervalTime = 4
local WndStayTime = 2
local CloseWndTime = 10
CAreaIntroductionWnd = class(SQRDialog)

function CreateAreaIntroductionWnd(Parent)
	local Wnd = CAreaIntroductionWnd:new()
	Wnd:CreateFromRes("AreaIntroductionWnd",Parent)
	Wnd.m_RichText = Wnd:GetDlgChild("RichText")
	local WndWidth = Wnd.m_RichText:GetWndWidth()
	Wnd.m_RichText:SetStyle(0x60000008)
	Wnd.m_RichText:SetAutoWidth(WndWidth,WndWidth)
	return Wnd
end

function CAreaIntroductionWnd:ShowAreaIntroduction(AreaName)
	if Lan_AreaInfo_Client(MemH64(AreaName)) then
		local AreaDescription = Lan_AreaInfo_Client(MemH64(AreaName), "AreaDescription")
		if AreaDescription ~= "" then
			local sText = "    "..AreaDescription
			local fontsize = self.m_RichText:GetFontSize()
			self.m_RichText:SetWndText(sText)
			local wndheight = self.m_RichText:GetWndHeight()
			self:ShowWnd(true)
			
			local function CloseWnd()
				self:ShowWnd(false)
				UnRegisterTick(self.m_CloseWndTick)
				self.m_CloseWndTick = nil
			end
			if self.m_CloseWndTick then
				UnRegisterTick(self.m_CloseWndTick)
				self.m_CloseWndTick = nil
			end
			self.m_CloseWndTick = RegisterTick("CloseWndTick",CloseWnd,CloseWndTime*1000)
		end
	end
end