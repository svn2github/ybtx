gac_require "smallgames/TiggerCageClient/TiggerCageClientInc"

local NextDirection = nil
local RecordPos = nil
local CageGlobalID = nil
local Right = 0
local Left = 1
local Dis = 5
--local Addnum = 2
--########################--

--########################--
function CreateTiggerCageGame(parent)
	local Game = CTiggerCageClient:new()
	return Game
end

function CTiggerCageClient:BeginSmallGame(GameName,ObjID)
	local Game_Common = SmallGame_Common(GameName)
	local ScriptArgTbl = g_SmallGameStrTbl[GameName].ScriptArg
	local GameTime = Game_Common("GameTime")
	self.m_Addnum = ScriptArgTbl["交替一次提高百分点"]
	self.m_Swing = ScriptArgTbl["振幅"]
	self.m_SuccRenderTbl = g_SmallGameStrTbl[GameName].SuccEfxFile--Game_Common("SuccEfxFile")
	
	g_GameMain.m_ProgressWnd:BeginTimeProgress(GameTime*1000,Game_Common,GetStaticTextClient(4902),EndSmallGlobalFun)	--g_GameMain.m_TiggerCageGame.IsSuccTiggerCageGame)
	g_GameMain.m_DoSmallGame = GameName
	CageGlobalID = ObjID
end

function CTiggerCageClient:RecMousePosSmallGame(NowPos)
	if not RecordPos then
		RecordPos = {}
		RecordPos.x = NowPos.x
		RecordPos.y = NowPos.z
		return
	end
	if NowPos.x-RecordPos.x == NowPos.z-RecordPos.y then
		return
	end	
	if math.abs(NowPos.x-RecordPos.x) < self.m_Swing then
		return
	end
	
	if NextDirection then
		if NextDirection == Right then
			if NowPos.x > RecordPos.x then
				NextDirection = Left
				g_GameMain.m_ProgressWnd:AddPerPos(self.m_Addnum)
			end
		else
			if NowPos.x < RecordPos.x then
				NextDirection = Right
				g_GameMain.m_ProgressWnd:AddPerPos(self.m_Addnum)
			end
		end
	else
		if NowPos.x < RecordPos.x then
			NextDirection = Right
			g_GameMain.m_ProgressWnd:AddPerPos(self.m_Addnum)
		else
			NextDirection = Left
			g_GameMain.m_ProgressWnd:AddPerPos(self.m_Addnum)
		end
	end
	if RecordPos then
		RecordPos.x = NowPos.x
		RecordPos.y = NowPos.z
	end
end

function CTiggerCageClient:CancelSmallGame(IsMove)
	if g_GameMain.m_DoSmallGame then
		if IsMove then
			Gac2Gas:ExitSmallGame(g_Conn,g_GameMain.m_DoSmallGame,CageGlobalID)
		else
			Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_DoSmallGame,CageGlobalID)
		end
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		g_GameMain.m_DoSmallGame = nil
		NextDirection = nil
		RecordPos = nil
		CageGlobalID = nil
		g_GameMain.m_ProgressWnd:StopProgress()
	end
end

function CTiggerCageClient:SuccSmallGame()
	if g_GameMain.m_DoSmallGame then
		Gac2Gas:SmallGameSucceed(g_Conn, g_GameMain.m_DoSmallGame,CageGlobalID)
		g_GameMain.m_SGMgr:SmallGameAddRender(self.m_SuccRenderTbl)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		self.m_SuccRenderTbl = nil
		g_GameMain.m_DoSmallGame = nil
	end
	NextDirection = nil
	RecordPos = nil
	CageGlobalID = nil
end

function CTiggerCageClient:FailSmallGame()
	if g_GameMain.m_DoSmallGame then
		Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_DoSmallGame,CageGlobalID)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		MsgClient(3250)
		g_GameMain.m_DoSmallGame = nil
		NextDirection = nil
		RecordPos = nil
		CageGlobalID = nil
		g_GameMain.m_ProgressWnd:StopProgress()
	end
end
