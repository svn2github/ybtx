gac_require "smallgames/AlternateClickMouse/CAlternateClickMouseInc"

local NpcGlobalID = nil
local LeftNum = 0
local RightNum = 0

function CreateClickMouseGame()
	Game = CAlternateClickMouse:new()
	Game:InitVar()
	return Game
end

function CAlternateClickMouse:InitVar()

end

--参数 小游戏的名字,小游戏的时间, 点击鼠标正确后提高的百分点
function CAlternateClickMouse:BeginSmallGame(GameName,ObjID)
	local Game_Common = SmallGame_Common(GameName) 
	local ScriptArgTbl = g_SmallGameStrTbl[GameName].ScriptArg --Game_Common("ScriptArg")
	g_GameMain.m_SmallGame = GameName
	local GameTime = Game_Common("GameTime")
	self.m_AddPer = ScriptArgTbl["交替一次提高百分点"]
	self.m_LeftTimes = ScriptArgTbl["左右单击的次数频率"][1]
	self.m_RightTimes = ScriptArgTbl["左右单击的次数频率"][2]
	self.m_SuccRenderTbl = g_SmallGameStrTbl[GameName].SuccEfxFile --Game_Common("SuccEfxFile")
	g_GameMain.m_ProgressWnd:BeginTimeProgress(GameTime*1000, Game_Common, GetStaticTextClient(4903), EndSmallGlobalFun)
	self.m_MouseState = 0			--当前鼠标状态
	NpcGlobalID = ObjID
end

function CAlternateClickMouse:RunSmallGame(uMsgID)
	if self.m_MouseState ~= uMsgID then
		if uMsgID == 1 then
			LeftNum = 0
		elseif uMsgID == 2 then
			RightNum = 0
		end
	end
	self.m_MouseState = uMsgID
	
	if self.m_MouseState == 1 then
		LeftNum = LeftNum + 1
	elseif self.m_MouseState == 2 then
		RightNum = RightNum + 1
	end
	if LeftNum == self.m_LeftTimes and RightNum == self.m_RightTimes then
		g_GameMain.m_ProgressWnd:AddPerPos(self.m_AddPer)
	end
end

function CAlternateClickMouse:SuccSmallGame()
	Gac2Gas:SmallGameSucceed(g_Conn, g_GameMain.m_SmallGame,NpcGlobalID)
	g_GameMain.m_SGMgr:SmallGameAddRender(self.m_SuccRenderTbl)
	g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
	self.m_SuccRenderTbl = nil
	g_GameMain.m_SmallGame = nil
	NpcGlobalID = nil
end

function CAlternateClickMouse:FailSmallGame()
	if g_GameMain.m_SmallGame then
		Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		MsgClient(3250)
		g_GameMain.m_SmallGame = nil
	end
	NpcGlobalID = nil
end

function CAlternateClickMouse:CancelSmallGame(IsMove)
	if g_GameMain.m_SmallGame then
		if IsMove then
			Gac2Gas:ExitSmallGame(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		else
			Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		end
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		g_GameMain.m_SmallGame = nil
	end
	g_GameMain.m_ProgressWnd:StopProgress()
	NpcGlobalID = nil
end
