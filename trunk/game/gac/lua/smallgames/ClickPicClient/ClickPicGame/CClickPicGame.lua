gac_require "smallgames/ClickPicClient/ClickPicGame/CClickPicGameInc"
gac_require "smallgames/ClickPicClient/CPopupMsg"

local m_ClickPicOrder = ""

function InitClickPicGame()
	Game = CClickPicGame:new()
	--Game:InitData()
	return Game
end

function CClickPicGame:InitData()
	local RandPicNum = ""
	for i = 1,7,1 do
		RandPicNum = RandPicNum .. tostring(math.random(1,4))
	end
	self.m_state = 2
	self.m_RandPicNum = RandPicNum
end
--窗口消息响应函数
function CClickPicGame:BeginSmallGame(GameName)
	self.GameName = GameName
	self.PopupMsg = CreatePopupMsg()
	self:InitData()
	local strMsg = ""
	if g_GameMain.m_ClickPicGame.m_RandPicNum and g_GameMain.m_ClickPicGame.m_state then
		strMsg = string.sub(g_GameMain.m_ClickPicGame.m_RandPicNum, 1, g_GameMain.m_ClickPicGame.m_state)
	end
	self.PopupMsg:StartPopup("请选择：" .. strMsg)
	g_GameMain.m_ClickPicWnd:ShowWnd(true)
end

function CClickPicGame:RunSmallGameStep(uMsgID)
	m_ClickPicOrder = m_ClickPicOrder .. uMsgID
	if m_ClickPicOrder then
		if string.sub(self.m_RandPicNum,1,#(m_ClickPicOrder)) ~= m_ClickPicOrder then
			self:FailSmallGame()
			return 
		end
	end
		
	if string.sub(self.m_RandPicNum,1,self.m_state) == m_ClickPicOrder then
		if self.m_state == 7 then
			self:SuccSmallGame()
			return
		end
		--db()
		self.m_state = self.m_state + 1
		m_ClickPicOrder = ""
		local strMsg = ""
		if g_GameMain.m_ClickPicGame.m_RandPicNum and g_GameMain.m_ClickPicGame.m_state then
			strMsg = string.sub(g_GameMain.m_ClickPicGame.m_RandPicNum, 1, g_GameMain.m_ClickPicGame.m_state)
		end
		self.PopupMsg:StartPopup("请选择：" .. strMsg)
	end
end

function CClickPicGame:SuccSmallGame()
	if self.GameName then
		Gac2Gas:SmallGameSucceed(g_Conn, self.GameName,0)
		self.PopupMsg:StopPopup()
		g_GameMain.m_ClickPicWnd:ShowWnd(false)
		self.m_state = 2
		m_ClickPicOrder = ""
		g_GameMain.m_SmallGame = nil
	end
end

function CClickPicGame:FailSmallGame()
	if self.GameName then
		Gac2Gas:SmallGameFaild(g_Conn, self.GameName)
		self.PopupMsg:StopPopup()
		g_GameMain.m_ClickPicWnd:ShowWnd(false)
		self.m_state = 2
		m_ClickPicOrder = ""
		self.PopupMsg:StartPopup("你失败了，请重新接此任务！！")
		g_GameMain.m_SmallGame = nil
	end
end

function CClickPicGame:CancelSmallGame()
	if self.GameName then
		Gac2Gas:SmallGameFaild(g_Conn, self.GameName)
		self.PopupMsg:StopPopup()
		g_GameMain.m_ClickPicWnd:ShowWnd(false)
		self.m_state = 2
		m_ClickPicOrder = ""
		self.PopupMsg:StartPopup("你失败了，请重新接此任务！！")
		g_GameMain.m_SmallGame = nil
	end
end
