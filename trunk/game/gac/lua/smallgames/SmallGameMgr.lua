gac_require "smallgames/SmallGameMgrInc"
gac_require "smallgames/SmallGameEffectWnd"
gac_require "smallgames/AlternateClickMouse/CAlternateClickMouse"
gac_require "smallgames/ClickPicClient/ClickPicGame/CClickPicGame"
gac_require "smallgames/TiggerCageClient/TiggerCageClient"
gac_require "smallgames/FindKnifeClient/CFindKnifeGame"
gac_require "smallgames/GetFungusClient/CGetFungusWnd"
gac_require "smallgames/GetStoneClient/CGetStoneWnd"

cfg_load "smallgame/SmallGame_Common"

function CreatSmallGame()
	SGameMgr = CSmallGameMgr:new()
	SGameMgr:InitSmallGameAll()
	return SGameMgr
end

local function CheckQuestCondition(GameCommon, GameName)
	local mRequireTbl = GameCommon("QuestRequire")
	local mResultTbl = g_SmallGameStrTbl[GameName].QuestResult --GameCommon.QuestResult
	local mQuestRequire = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[mRequireTbl[1]]
	local mQuestResult = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[mResultTbl[1]]
	local QuestVarMaxNum = g_QuestNeedMgr[mResultTbl[1]][mResultTbl[2]].Num
	if mQuestRequire == nil or mQuestResult == nil or QuestVarMaxNum == nil then
		return false
	end
	if mQuestRequire[mRequireTbl[2]] == nil or mQuestResult[mResultTbl[2]] == nil then
		return false
	end
	if mQuestResult[mRequireTbl[2]].DoNum >= QuestVarMaxNum then
		return false
	end
	return true
end

function CSmallGameMgr:InitSmallGameAll()
	g_GameMain.m_ClickMouseGame = CreateClickMouseGame(g_GameMain)
	g_GameMain.m_ClickPicGame = InitClickPicGame(g_GameMain)
	g_GameMain.m_TiggerCageGame = CreateTiggerCageGame(g_GameMain)
	g_GameMain.m_FindKnife = CreateFindKnifeGame(g_GameMain)
	g_GameMain.m_GetStoneGame = CreateGetStoneWnd(g_GameMain)
	g_GameMain.m_GetFungusGame = CreateGetFungusWnd(g_GameMain)
	
	self.SmallGameScriptTbl = 
	{
		["交替点击鼠标左右键"] = g_GameMain.m_ClickMouseGame,
		["点击图片"] = g_GameMain.m_ClickPicGame,
		["左右晃鼠标"] = g_GameMain.m_TiggerCageGame,
		["点击鼠标确定选择"] = g_GameMain.m_GetHorseMedicine,
		["上下晃鼠标"] = g_GameMain.m_FindKnife,
		["点鼠标维持区间"] = g_GameMain.m_GetStoneGame,
		["点击鼠标确定位置"] = g_GameMain.m_GetFungusGame,
	}
	
	
end

function Gas2Gac:RetMsgBeginSmallGame(Conn,GameName,GlobalID)
	local GameCommon = SmallGame_Common(GameName)
	if GameCommon == nil then
		return
	end
	
	g_GameMain.m_SmallGemeMsgWnd:InitSmallGameMsgWnd(GameName,GlobalID)
end

function Gas2Gac:BeginSmallGame(Conn,GameName,ObjID)
	g_GameMain.m_SGMgr:BeginSmallGameFun(GameName,ObjID)
end

function Gas2Gac:ExitSmallGame(Conn)
	if not g_MainPlayer.m_IsWaitShowWnd then
		Gas2Gac:CloseSmallGameMsgWnd(g_Conn)
		g_GameMain.m_SGMgr:CancelSmallGame(true)
	end
end

function CSmallGameMgr:BeginSmallGameFun(GameName,ObjID)
	local GameCommon = SmallGame_Common(GameName)
	if GameCommon == nil then
		return
	end
	self.m_NowScript = GameCommon("ScriptName")
	if self.SmallGameScriptTbl[self.m_NowScript] == nil then
		return
	end
	
	--特效显示窗口
	self.m_SmallGameEffectWnd = CreatSmallGameEffectWnd(g_GameMain)
	self.m_strEffectPath = GameCommon("GifEffectFile")
	self:ShowSmallGameEffectWnd(self.m_strEffectPath)
	--进入小游戏
	self.SmallGameScriptTbl[self.m_NowScript]:BeginSmallGame(GameName,ObjID)
end

function CSmallGameMgr:BeginSmallGameByQuest(GameName)
	local GameCommon = SmallGame_Common(GameName)
	if not GameCommon then
		return
	end

	self.m_NowScript = GameCommon("ScriptName")
	if not self.SmallGameScriptTbl[self.m_NowScript] then
		return
	end
	
	--if not CheckQuestCondition(GameCommon, GameName) then
	--	return
	--end
	
	g_GameMain.m_SmallGame = self.m_NowScript

	self.SmallGameScriptTbl[self.m_NowScript]:BeginSmallGame(GameName)
end

function CSmallGameMgr:RunSmallGameFun(uMsgID)
	--小游戏进行中
	local Target = g_MainPlayer.m_LockCenter.m_LockingObj or g_MainPlayer.m_LockCenter.m_LockingIntObj
	if self.m_NowScript then
		if self.SmallGameScriptTbl[self.m_NowScript] then
			self.SmallGameScriptTbl[self.m_NowScript]:RunSmallGame(uMsgID)
		end
	end
end

function CSmallGameMgr:EndSmallGameFun(ifSucc)
	--完成小游戏
	local Target = g_MainPlayer.m_LockCenter.m_LockingObj or g_MainPlayer.m_LockCenter.m_LockingIntObj
	if self.m_NowScript then
		if self.SmallGameScriptTbl[self.m_NowScript] then
			--if not Target then--如果连对象都没有了,那就失败吧
			--	ifSucc = false
			--end
			if ifSucc then
				self.SmallGameScriptTbl[self.m_NowScript]:SuccSmallGame()
			else
				self.SmallGameScriptTbl[self.m_NowScript]:FailSmallGame()
			end
		end
	end
	g_GameMain.m_SmallGame = nil
	g_GameMain.m_DoSmallGame = nil
	self.m_NowScript = nil
end

--晃鼠标游戏记录鼠标位置
function CSmallGameMgr:RecMousePosSmallGame(vecPos)
	if g_GameMain.m_DoSmallGame then
		if self.SmallGameScriptTbl[self.m_NowScript] then
			self.SmallGameScriptTbl[self.m_NowScript]:RecMousePosSmallGame(vecPos)
		end
	end
end

--取消进行中的小游戏
function CSmallGameMgr:CancelSmallGame(IsMove)
	if g_GameMain.m_SmallGame or g_GameMain.m_DoSmallGame then
		if self.SmallGameScriptTbl[self.m_NowScript] then
			self.SmallGameScriptTbl[self.m_NowScript]:CancelSmallGame(IsMove)
		end
		g_GameMain.m_SmallGame = nil
		g_GameMain.m_DoSmallGame = nil
		self.m_NowScript = nil
	end
end

--某些限时的小游戏，时间到了后要调用的方法
function EndSmallGlobalFun(ifSucc)
	g_GameMain.m_SGMgr:EndSmallGameFun(ifSucc)
end

--自己特效显示
function CSmallGameMgr:SmallGameAddRender(strFilePath)
	if strFilePath and IsTable(strFilePath) then
		
		local strEfxFile,strState = strFilePath[1],strFilePath[2]
		if g_MainPlayer and strEfxFile and strState then
			if strEfxFile~="" and strState~="" then
				
				g_MainPlayer:GetRenderObject():AddEffect( strEfxFile, strState, 0, nil )
	
			end
		end
		
	end
end

--在窗口上显示动画特效图
function CSmallGameMgr:ShowSmallGameEffectWnd(strFilePath,ShowTime)
	local function LoopShowPic(Tick, FilePath)
		if FilePath and FilePath ~= "" then
			if self.m_SmallGameEffectWnd then
				self.m_SmallGameEffectWnd:AddImage(FilePath)
			end
		end
		
		if self.m_ShowEffectTick then
			UnRegisterTick(self.m_ShowEffectTick)
			self.m_ShowEffectTick = nil
		end
	end
	
	if strFilePath and strFilePath ~= "" then
		
		self.m_SmallGameEffectWnd:AddImage(strFilePath)
		if not self.m_SmallGameEffectWnd:IsShow() then
			self.m_SmallGameEffectWnd:ShowWnd(true)
		end
		if ShowTime and IsNumber(ShowTime) and ShowTime > 0 then
			if self.m_ShowEffectTick then
				UnRegisterTick(self.m_ShowEffectTick)
				self.m_ShowEffectTick = nil
			end
			self.m_ShowEffectTick = RegisterTick("LoopShowPicTick",LoopShowPic,ShowTime*1000,self.m_strEffectPath)
		end
		
	end
end

--关闭动画特效图
function CSmallGameMgr:CloseSmallGameEffectWnd()
	if self.m_SmallGameEffectWnd then
		self.m_SmallGameEffectWnd:ShowWnd(false)
		self.m_SmallGameEffectWnd:DestroyWnd()
		self.m_SmallGameEffectWnd = nil
	end
end

--服务端同步其它人的特效显示
function Gas2Gac:PlayerSmallGameSuccFx(Conn,GameName,EntityID)
	local CharFollower = CCharacterFollower_GetCharacterByID(EntityID)
	if CharFollower == nil then
		return
	end
	if EntityID == g_MainPlayer:GetEntityID() then
		--print(GameName)
		MsgClient(3255,Lan_SmallGame_Common(MemH64(GameName), "ShowName"))
	else
		local SuccEfxFile = g_SmallGameStrTbl[GameName].SuccEfxFile
		if SuccEfxFile and next(SuccEfxFile)  then
	--		local SuccEfxFile = loadstring("return " .. SuccEfxFile)()
			CharFollower:GetRenderObject():AddEffect( SuccEfxFile[1], SuccEfxFile[2], 0, nil)
		end
	end
end
