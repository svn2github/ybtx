gac_require "smallgames/ClickPicClient/ClickPicWnd/CClickPicWndInc"

function CreateClickPicWnd(partent)
	local Wnd = CClickPicWnd:new()
	Wnd:CreateFromRes("WndClickPic",partent)
	Wnd.m_ClickPic1 = Wnd:GetDlgChild("btn1")
	Wnd.m_ClickPic2 = Wnd:GetDlgChild("btn2")
	Wnd.m_ClickPic3 = Wnd:GetDlgChild("btn3")
	Wnd.m_ClickPic4 = Wnd:GetDlgChild("btn4")

	return Wnd
end

--窗口消息响应函数
function CClickPicWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ClickPic1) then
			g_GameMain.m_ClickPicGame:RunSmallGameStep(1)
		elseif(Child == self.m_ClickPic2) then
			g_GameMain.m_ClickPicGame:RunSmallGameStep(2)
		elseif(Child == self.m_ClickPic3) then
			g_GameMain.m_ClickPicGame:RunSmallGameStep(3)
		elseif(Child == self.m_ClickPic4) then
			g_GameMain.m_ClickPicGame:RunSmallGameStep(4)
		end
	end
end