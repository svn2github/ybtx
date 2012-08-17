CAreaFbDeadMsgWnd = class(SQRDialog)

function CAreaFbDeadMsgWnd:Ctor()
	self:CreateFromRes("WndDeadMsg", g_GameMain)
	self:Init()
end

function CAreaFbDeadMsgWnd:Init()
	self.m_WndInfo			 		= self:GetDlgChild("MsgInfo")
	self.m_BtnOk						= self:GetDlgChild("BtnOK")
	self.m_BtnCancel 				= self:GetDlgChild("BtnCancel")	
	self.m_NoviceTime				= 0
	self.m_CountDownNum			= 0
	self.m_RebornCountTime	=	0
--	self.m_WndInfo:SetWndText(GetStaticTextClient(11003))
end

function CAreaFbDeadMsgWnd:SetOkBtnText(Num)
	self.m_BtnOk:SetWndText(GetStaticTextClient(11004, Num))
end

function CAreaFbDeadMsgWnd:SetNoviceTime()
	self.m_NoviceTime = 5
end

function CAreaFbDeadMsgWnd:GetNoviceTime()
--	local time = self.m_NoviceTime - (math.floor(GetProcessTime()/1000) - g_GameMain.m_DeadTime)
	local time = self.m_NoviceTime - self.m_CountTime
	if time < 0 then
		return 0
	end
	return time
end

function CAreaFbDeadMsgWnd:SetOriginRebornCountTime(time)
	self.m_RebornCountTime = time
end

function CAreaFbDeadMsgWnd:GetOriginRebornCountTime()
--	local time = self.m_RebornCountTime - (math.floor(GetProcessTime()/1000) - g_GameMain.m_DeadTime)
	local time = self.m_RebornCountTime - self.m_CountTime
	if time < 0 then
		return 0
	end
	return time
end

function CAreaFbDeadMsgWnd:SetBackRebornCountTime(time)
	self.m_CountDownNum = time
end

function CAreaFbDeadMsgWnd:GetBackRebornCountTime()
--	local time = self.m_CountDownNum - (math.floor(GetProcessTime()/1000) - g_GameMain.m_DeadTime)
	local time = self.m_CountDownNum - self.m_CountTime
	if time < 0 then
		return 0
	end
	return time
end

function CAreaFbDeadMsgWnd:ShowDeadWnd(time,RebornCountTime)

	self.m_CountTime = 0
	local LogTime = self:GetBackRebornCountTime()
	self:SetBackRebornCountTime(time) --初始化时间
	
	local sStr = nil
	local nItemCount = 0
	if not IsCppBound(g_MainPlayer) then
		return
	end
	if g_MainPlayer:CppGetLevel() < 15 then
		self.m_BtnCancel:EnableWnd(false)
		self:SetNoviceTime()
		sStr = GetStaticTextClient(11011, self:GetNoviceTime())
	elseif g_MainPlayer:CppGetLevel() >= 15 then
		self:SetOriginRebornCountTime(RebornCountTime)
		nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )
		if self:GetOriginRebornCountTime() > 0 then
			sStr = GetStaticTextClient(11002, self:GetOriginRebornCountTime())
		else
			sStr = GetStaticTextClient(11002, nItemCount)
		end
		if nItemCount <= 0 or self:GetOriginRebornCountTime() ~= 0 then
			self.m_BtnCancel:EnableWnd(false)
		else
			self.m_BtnCancel:EnableWnd(true)
		end
	end

--	nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )
--	sStr = "原地复活(" .. nItemCount  .. ")"
--	if nItemCount <= 0  then
--		self.m_BtnCancel:EnableWnd(false)
--	else
--		self.m_BtnCancel:EnableWnd(true)
--	end

	self:SetOkBtnText(self:GetBackRebornCountTime())
	self.m_BtnCancel:SetWndText(sStr)
	self.m_BtnOk:EnableWnd(false)
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(self)
	self:ShowWnd(true)
	
	if self.m_CountDownTick ~= nil then
		LogErr("客户端复活tick没有注销!", "tick剩余时间："..LogTime)
		return
	end
	self.m_CountDownTick = RegClassTick( "AreaFbDeadTick", self.CountLeftTimeTick, 1000, self)	
end

function CAreaFbDeadMsgWnd:CountLeftTimeTick()
	self.m_CountTime = self.m_CountTime + 1
	local leftTime = self:GetBackRebornCountTime()
	
	local sStr = nil
	local nItemCount = 0
	
	if not IsCppBound(g_MainPlayer) then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
		return
	end

	if g_MainPlayer:CppGetLevel() < 15 then
		if self:GetNoviceTime() > 0 then
			sStr = GetStaticTextClient(11011, self:GetNoviceTime())
		else
			self.m_BtnCancel:EnableWnd(true)
			sStr = GetStaticTextClient(11001)
		end
	elseif g_MainPlayer:CppGetLevel() >= 15 then
		nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )
		if self:GetOriginRebornCountTime() > 0 then
			sStr = GetStaticTextClient(11002, self:GetOriginRebornCountTime())
		else
			sStr = GetStaticTextClient(11002, nItemCount)
		end
		if nItemCount <= 0 or self:GetOriginRebornCountTime() > 0 then
			self.m_BtnCancel:EnableWnd(false)
		else
			self.m_BtnCancel:EnableWnd(true)
		end
	end

--		nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )
--		sStr = "原地复活(" .. nItemCount  .. ")"
--		
--		if nItemCount <= 0  then
--			self.m_BtnCancel:EnableWnd(false)
--		else
--			self.m_BtnCancel:EnableWnd(true)
--		end
		
	self.m_BtnCancel:SetWndText(sStr)
		
	if leftTime > 0 then	
		self.m_BtnOk:EnableWnd(false)
		self:SetOkBtnText(leftTime)
	else
		self.m_BtnOk:SetWndText(GetStaticTextClient(11005))
		self.m_BtnOk:EnableWnd(true)
--		self:ShowWnd(false)
--		Gac2Gas:GoIntoFbOrStay(g_Conn, true)
	end
	if leftTime <= 0 and self:GetOriginRebornCountTime() <= 0 then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
end

function CAreaFbDeadMsgWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_BtnCancel == Child then
			--self:HideWnd()
			Gac2Gas:GoIntoFbOrStay(g_Conn, false)
		elseif self.m_BtnOk == Child then
			--self:HideWnd()
			--print(math.floor(GetProcessTime()/1000),"CAreaFbDeadMsgWnd:OnCtrlmsg")
--			g_MainPlayer.m_RebornTimeLog = {self:GetBackRebornCountTime(),g_GameMain.m_DeadTimeLog,math.floor(GetProcessTime()/1000)}
			Gac2Gas:GoIntoFbOrStay(g_Conn, true)
		end
	end
end

function CAreaFbDeadMsgWnd:HideWnd()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
	self:ShowWnd(false)
end

--function Gas2Gac:RebornTimeLog(Conn)
--	local logInfo = g_MainPlayer.m_RebornTimeLog
--	if not logInfo then
--		LogErr("客户端复活失败LOG","玩家("..g_MainPlayer.m_Properties:GetCharName()..")  Log信息不存在!")
--		return
--	end
--	if not IsTable(logInfo) then
--		LogErr("客户端复活失败LOG","玩家("..g_MainPlayer.m_Properties:GetCharName()..")  ",type(logInfo),logInfo)
--		return
--	end
--	local Count, DeadTime, ClickTime = unpack(logInfo, 1, 3)
--	if not Count then
--		Count = "nil"
--	end
--	if not DeadTime then
--		DeadTime = "nil"
--	end
--	LogErr("客户端复活失败LOG","玩家("..g_MainPlayer.m_Properties:GetCharName()..")  复活倒计时时间:"..Count.."  玩家死亡时间:"..DeadTime.."  点击复活时间:"..ClickTime.."  现在时间:"..math.floor(GetProcessTime()/1000))
--end
