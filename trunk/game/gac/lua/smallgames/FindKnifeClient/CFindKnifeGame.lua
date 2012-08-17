gac_require "smallgames/FindKnifeClient/CFindKnifeGameInc"
local RecordPos = nil
local NextDirection = nil
local Up = 0
local Down = 1
local Percentage = 5	--找到的机率为50%
local NpcGlobalID = nil

function CreateFindKnifeGame(parent)
	local Game = CFindKnifeGame:new()
	return Game
end

function CFindKnifeGame:BeginSmallGame(GameName,ObjID)
	local Game_Common = SmallGame_Common(GameName)
	local ScriptArgTabl = g_SmallGameStrTbl[GameName].ScriptArg
	local GameTime = Game_Common("GameTime")
	self.m_Addnum = ScriptArgTabl["交替一次提高百分点"]
	self.m_Swing = ScriptArgTabl["振幅"]
	self.m_SuccRenderTbl = g_SmallGameStrTbl[GameName].SuccEfxFile
	g_GameMain.m_ProgressWnd:BeginTimeProgress(GameTime*1000,Game_Common,"",EndSmallGlobalFun)
	g_GameMain.m_DoSmallGame = GameName
	NpcGlobalID = ObjID
end

function CFindKnifeGame:RecMousePosSmallGame(NowPos)
	if not RecordPos then
		RecordPos = {}
		RecordPos.x = NowPos.x
		RecordPos.y = NowPos.z
		return
	end
	if NowPos.x-RecordPos.x == NowPos.z-RecordPos.y then
		return
	end	
	if math.abs(NowPos.z-RecordPos.y) < self.m_Swing then
		return
	end
	
	if NextDirection then
		if NextDirection == Up then
			if NowPos.z > RecordPos.y then
				NextDirection = Down
				g_GameMain.m_ProgressWnd:AddPerPos(self.m_Addnum)
			end
		else
			if NowPos.z < RecordPos.y then
				NextDirection = Up
				g_GameMain.m_ProgressWnd:AddPerPos(self.m_Addnum)
			end
		end
	else
		if NowPos.z < RecordPos.y then
			NextDirection = Up
			g_GameMain.m_ProgressWnd:AddPerPos(self.m_Addnum)
		else
			NextDirection = Down
			g_GameMain.m_ProgressWnd:AddPerPos(self.m_Addnum)
		end
	end

	if RecordPos then
		RecordPos.x = NowPos.x
		RecordPos.y = NowPos.z
	end
end

function CFindKnifeGame:SuccSmallGame()
	local RandNum = math.random(1,10)
	if RandNum <= Percentage then
		Gac2Gas:SmallGameSucceed(g_Conn, g_GameMain.m_DoSmallGame,NpcGlobalID)
		g_GameMain.m_SGMgr:SmallGameAddRender(self.m_SuccRenderTbl)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		self.m_SuccRenderTbl = nil
		Percentage = 5
	else
		self:FailSmallGame()
		Percentage = Percentage + 1
		if Percentage > 10 then
			Percentage = 10
		end
	end
	g_GameMain.m_DoSmallGame = nil
	NextDirection = nil
	RecordPos = nil
	NpcGlobalID = nil
end

function CFindKnifeGame:FailSmallGame()
	if g_GameMain.m_DoSmallGame then
		Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_DoSmallGame,NpcGlobalID)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		MsgClient(3250)
		g_GameMain.m_DoSmallGame = nil
	end
	NpcGlobalID = nil
	NextDirection = nil
end

function CFindKnifeGame:CancelSmallGame(IsMove)
	if g_GameMain.m_DoSmallGame then
		if IsMove then
			Gac2Gas:ExitSmallGame(g_Conn,g_GameMain.m_DoSmallGame,NpcGlobalID)
		else
			Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_DoSmallGame,NpcGlobalID)
		end
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		g_GameMain.m_DoSmallGame = nil
	end
	NextDirection = nil
	NpcGlobalID = nil
	Percentage = 5
	g_GameMain.m_ProgressWnd:StopProgress()
end
