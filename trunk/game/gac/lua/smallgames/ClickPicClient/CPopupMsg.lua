gac_require "smallgames/ClickPicClient/CPopupMsgInc"

function CreatePopupMsg(partent)
	local PopupMsg = CPopupMsg:new()
	return PopupMsg
end

--窗口消息响应函数time表示ms数
function CPopupMsg:SetLoopTimePopupMsg(time)
	if self.m_LoadTime then
		UnRegisterTick( self.m_LoadTime )
	end
	local Loading = function()
		local Target = g_MainPlayer.m_LockCenter.m_LockingObj or g_MainPlayer.m_LockCenter.m_LockingIntObj
		if Target then
			if not Target.m_NpcHeadUpDialog then
				local bIsCreate = NpcHeadUpDialog(Target)
				if not bIsCreate then
					return
				end
			end
			local Wnd = Target.m_NpcHeadUpDialog
			Wnd.m_NpcHeadUpDialogWnd:SetAutoWidth(0, 100)
			local strMsg = ""

			if g_GameMain.m_ClickPicGame.m_RandPicNum and g_GameMain.m_ClickPicGame.m_state then
				strMsg = string.sub(g_GameMain.m_ClickPicGame.m_RandPicNum, 1, g_GameMain.m_ClickPicGame.m_state)
			end
			NpcHeadUpDialogShow(Wnd, "请选择：" .. strMsg)
		end
	end
	self.m_LoadTime = RegisterTick( "Loading", Loading , time)  --注册时钟
end

function CPopupMsg:StartPopup(strMsg)
	local Target = g_MainPlayer.m_LockCenter.m_LockingObj or g_MainPlayer.m_LockCenter.m_LockingIntObj
	if Target then
		if not Target.m_NpcHeadUpDialog then
			local bIsCreate = NpcHeadUpDialog(Target)
			if not bIsCreate then
				return
			end
		end
		local Wnd = Target.m_NpcHeadUpDialog
		Wnd.m_NpcHeadUpDialogWnd:SetAutoWidth(0, 100)
		NpcHeadUpDialogShow(Wnd, strMsg)
	end
end

function CPopupMsg:StopPopup()
	if self.m_LoadTime then
		UnRegisterTick( self.m_LoadTime )
		self.m_LoadTime = nil
	end
end