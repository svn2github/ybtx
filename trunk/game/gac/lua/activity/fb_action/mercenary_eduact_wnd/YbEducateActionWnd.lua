cfg_load "fb_game/MercCardTooltip_Common"

gac_gas_require "activity/fb/MercenaryEducateActCfgCheck"
cfg_load "message/Description_Client"
lan_load "fb_game/Lan_MercenaryEducateAction_Server"

CYbEducateActionWnd = class(SQRDialog)

local m_FbName = "佣兵训练活动"--GetStaticTextClient(9402)--佣兵训练活动

local m_YbActStage = 
{
	CommonStarted = 1, 
	ReWardStarted = 2,
	Waiting = 3,
}

g_XiuXingTaLastMusic = nil

function CYbEducateActionWnd:Ctor()
	self:CreateFromRes("Fb_PVPApplyWnd", g_GameMain) 
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_JoinBtn = self:GetDlgChild("JoinBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_FbGameInfo = self:GetDlgChild("FbGameInfo")
	self.m_Title = self:GetDlgChild("WndName")
	
	self.m_Title:SetWndText(GetStaticTextClient(9402))
	self.m_JoinBtn:SetWndText(GetStaticTextClient(8806))
	self.m_CancelBtn:SetWndText(GetStaticTextClient(8807))
	--self.m_FbGameInfo:SetWndText(Description_Client[m_FbName].Content)
	self.m_FbGameInfo:SetWndText(GetDescriptionDisplayName(m_FbName))
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
	
	--其它的窗口
--	g_GameMain.m_YbEducateActInfoWnd = CYbEducateActionInfoWnd:new()
--	g_GameMain.m_YbEducateActAwardWnd = CYbEducateActionAwardWnd:new()
end

function CYbEducateActionWnd:ClearAllTick()
	g_GameMain.m_YbEducateActInfoWnd:CloseWnd()
	g_GameMain.m_YbEducateActAwardWnd:CloseWnd()
end

function CYbEducateActionWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_JoinBtn then
			Gac2Gas:JoinYbEducateAction(g_Conn, "佣兵训练营")
			self:ShowWnd(false)
		elseif Child == self.m_CancelBtn then
			self:ShowWnd(false)
		elseif Child == self.m_CloseBtn then
			--关闭窗口
			self:ShowWnd(false)
		end
	end
end

--local function OverGame()
--	local function CallBack(g_GameMain,uButton)
--		local bFlag = (uButton == MB_BtnOK)
--		if bFlag then
--			Gac2Gas:NotJoinFbAction(g_Conn,m_FbName)
--		end
--		g_GameMain.m_MsgBox = nil
--		return true
--	end
--	
--	local strInfo = MsgBoxMsg(5010)
--	g_GameMain.m_MsgBox = MessageBox(g_GameMain, strInfo, BitOr( MB_BtnOK), CallBack, g_GameMain)
--end

--==================================================
function CYbEducateActionWnd:SendPlayXiuXingTaBgm(Conn, MusicType)
	--print("上线！！")
	if not MusicType then
		--print("修行塔播放音乐出错!")
		return
	end
	if g_XiuXingTaLastMusic then
		StopCue(g_XiuXingTaLastMusic)
	end
	local AreaMusic = nil

	if MusicType == 1 then
--		print("奖励关")
		AreaMusic = "music活动01"
	elseif MusicType == 2 then
--		print("普通关")
		AreaMusic = "music19"
	elseif MusicType == 3 then
--		print("等待")
		if g_XiuXingTaLastMusic then
			CYbEducateActionWnd:StopXXTBackgroundMusic(g_XiuXingTaLastMusic)
		end
	end
	if AreaMusic then
		local cfgTbl = BackgroundMusic_Client(AreaMusic)
		PlayBackgroundMusic(nil, AreaMusic, cfgTbl)
	end
	g_XiuXingTaLastMusic = AreaMusic
end

function CYbEducateActionWnd:RetStopXiuXingTaBgm(Conn)
	if g_XiuXingTaLastMusic then
		StopBackgroundMusic(nil, g_XiuXingTaLastMusic, nil)
	end
end

--右边活动说明
function CYbEducateActionWnd:RetShowSmallActInfoWnd(Conn, NowGameID)
	local Wnd = g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd
	Wnd:SamllInfoWnd(NowGameID)
end

-- 上线后恢复数字
function CYbEducateActionWnd:SendResumeNum(Conn, IsOver, NameLocal, Num, GameID)
	local MercMgr = Lan_MercenaryEducateAction_Server(GameID)
	if MercMgr and MercMgr("ShowName") then
		local NameTbl = loadstring("return {"..MercMgr("ShowName").."}")()
		if NameTbl and NameTbl[NameLocal] then
			-- 更新右下角
			local ListInfoWnd = g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd
			local InfoWnd = ListInfoWnd.m_MercenaryInfo
			--InfoWnd.ResumeList[NameTbl[NameLocal][1]] = Num
			InfoWnd.ScoreList[NameTbl[NameLocal][1]].Score = Num
		end
	end	
end

function CYbEducateActionWnd:RetShowYbEducateActInfoWnd(Conn, RulesText, DownTime, NowGameID)
	local function ShowYbEducateActInfo()
		local Wnd = g_GameMain.m_YbEducateActInfoWnd
		Wnd.m_RulesText = RulesText
		Wnd.m_NowGameID = NowGameID
		Wnd:InfoWnd(RulesText, DownTime, NowGameID)
	end
	
	RegisterOnceTick(g_GameMain, "ShowYbEducateActInfoTick", ShowYbEducateActInfo, 1500)
end

function CYbEducateActionWnd:RetRecordGameInfo(Conn, RulesText, NowGameID)
	local Wnd = g_GameMain.m_YbEducateActInfoWnd
	Wnd.m_RulesText = RulesText
	Wnd.m_NowGameID = NowGameID
end

function CYbEducateActionWnd:RetCloseYbEducateActInfoWnd(Conn)
	local Wnd = g_GameMain.m_YbEducateActInfoWnd
	if Wnd and IsFunction(Wnd.IsShow) and Wnd:IsShow() then
		Wnd:CloseWnd()
	end
	
	local AwardWnd = g_GameMain.m_YbEducateActAwardWnd
	if AwardWnd and IsFunction(AwardWnd.IsShow) and AwardWnd:IsShow() then
		AwardWnd:CloseWnd()
	end
end

function CYbEducateActionWnd:RetUpDateSmallInfo(Conn)
	local ListInfoWnd = g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd
	local InfoWnd = ListInfoWnd.m_MercenaryInfo
	ListInfoWnd:UpDateSmallInfoWnd()
end

--系统区的消息提示内容
function CYbEducateActionWnd:RetYbEducateActShowMsg(Connection, GameID, NameLocal, Num, CountNum)
	local MercMgr = Lan_MercenaryEducateAction_Server(GameID)
	if MercMgr and MercMgr("ShowName") then
		local NameTbl = loadstring("return {"..MercMgr("ShowName").."}")()
		if NameTbl and NameTbl[NameLocal] then
			MsgClient(3604,NameTbl[NameLocal][1],Num,CountNum)--NPC(2/5)个
			
			-- 更新右下角
			local ListInfoWnd = g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd
			local InfoWnd = ListInfoWnd.m_MercenaryInfo
			InfoWnd.ScoreList[NameTbl[NameLocal][1]].Score = Num
			ListInfoWnd:UpDateSmallInfoWnd()
		end
	end
end

function CYbEducateActionWnd:RetYbEducateActShowLoseGameMsg(Connection, GameID, NameLocal, Num, CountNum)
	local MercMgr = Lan_MercenaryEducateAction_Server(GameID)
	if MercMgr and MercMgr("ShowName") then
		local NameTbl = loadstring("return {"..MercMgr("LoseGameShowName").."}")()
		if NameTbl and NameTbl[NameLocal] then
			local ListInfoWnd = g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd
			local InfoWnd = ListInfoWnd.m_MercenaryInfo
			-- 右下角更新次数
			if InfoWnd.ScoreList[NameTbl[NameLocal][1]] then
				InfoWnd.ScoreList[NameTbl[NameLocal][1]].Score = Num
			end
			MsgClient(3604,NameTbl[NameLocal][1],Num,CountNum)--NPC(2/5)个
			ListInfoWnd:UpDateSmallInfoWnd()
		end
	end
end


function CYbEducateActionWnd:ReturnAddMoneySlowly(Connection, AddMoney, DividNum)
	-- 避免出现小数
	local AddNum1 = math.floor(AddMoney / DividNum)
	local AddNum2 = AddMoney - AddNum1 * (DividNum - 1)
	function AddMoneyTick(Tick)
		if not Tick.AwardTimes then
			Tick.AwardTimes = 1
		end
		--print("加"..Tick.AwardTimes.."次钱")
		if Tick.AwardTimes ~= DividNum then
			Gas2Gac:ReturnAddMoney(g_Conn, AddNum1)
		else
			Gas2Gac:ReturnAddMoney(g_Conn, AddNum2)
		end
		
		Tick.AwardTimes = Tick.AwardTimes + 1
		if Tick.AwardTimes == DividNum + 1 then
			--print("delete Tick PlayerID:",PlayerId)
			UnRegisterTick(Tick)
			Tick = nil
		end
		
	end
	g_GameMain.m_XiuXingTaAddMoneyTick = RegisterTick("XiuXingTaAddMoneyTick", AddMoneyTick, 500, nil)
end


function CYbEducateActionWnd:ReturnModifyPlayerSoulNumSlowly(Connection, SoulNum, AddSoulNum, DividNum)	
	local AddSoulNum = AddSoulNum / DividNum
	function AddSoulTick(Tick)
		if not Tick.AwardTimes then
			Tick.AwardTimes = 1
		end
		--print("加"..Tick.AwardTimes.."魂值"..SoulNum - (DividNum - Tick.AwardTimes) * AddSoulNum)
		Gas2Gac:ReturnModifyPlayerSoulNum(g_Conn, math.floor(SoulNum - (DividNum - Tick.AwardTimes) * AddSoulNum))
		Tick.AwardTimes = Tick.AwardTimes + 1
		
		if Tick.AwardTimes == DividNum + 1 then
			--print("delete Tick PlayerID:",PlayerId)
			UnRegisterTick(Tick)
			Tick = nil
		end
		
	end
	g_GameMain.m_XiuXingTaAddSoulTick = RegisterTick("XiuXingTaAddMoneyTick", AddSoulTick, 500, nil)
end

function CYbEducateActionWnd:RetShowYbEducateActionWnd(Conn)
	local Wnd = g_GameMain.m_YbEducateActionWnd
	Wnd:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(Wnd)
end

-- 修行塔结束后显示给奖励的窗口
function CYbEducateActionWnd:RetOverYbXlAction(Conn, Type, ItemName, ItemNum)
	local Wnd = g_GameMain.m_FbActionMgrWnd
	local YbWnd = g_GameMain.m_YbEducateActAwardWnd
	if Wnd.m_DownTimeTick then
		UnRegisterTick(Wnd.m_DownTimeTick)
		Wnd.m_DownTimeTick = nil
		Wnd.m_TitleWnd:SetWndText("")
	end
	
	if Type == 1 then
		YbWnd:InitListInfo()
	elseif Type == 2 then
		YbWnd:InsertListInfo(ItemName, ItemNum)
	else
		YbWnd:InfoWnd(ItemNum)
	end
end

function CYbEducateActionWnd:RetIsContinueYbXlAction(Conn, LifeNum, Is15Floor)
	local Wnd = g_GameMain.m_FbActionMgrWnd
	if Wnd.m_DownTimeTick then
		UnRegisterTick(Wnd.m_DownTimeTick)
		Wnd.m_DownTimeTick = nil
		Wnd.m_TitleWnd:SetWndText("")
	end
	-- 询问玩家是重新挑战本关，还是进入下一关,先屏蔽
	--CYbEducateActionWnd:ShowNextFloor:ShowNextFloor(LifeNum,Is15Floor)
end

function CYbEducateActionWnd:StopXXTBackgroundMusic(musicName)
	StopCue(musicName)
--	print("停止播放背景音乐 ".. musicName)
	if g_GameMain.m_StopBackgroundMusicTick then
		--print("注销  -- StopBackgroundMusicTick")
		UnRegisterTick(g_GameMain.m_StopBackgroundMusicTick)
		g_GameMain.m_StopBackgroundMusicTick = nil
	end
	if g_GameMain.m_PlayBackgroundMusicTick then
		UnRegisterTick(g_GameMain.m_PlayBackgroundMusicTick)
		g_GameMain.m_PlayBackgroundMusicTick = nil
	end	
end

function CYbEducateActionWnd:ShowNextFloor(LifeNum,Is15Floor)
	local function CallBack(g_GameMain,uButton)
		local bFlag = (uButton == MB_BtnOK)
		Gac2Gas:ContinueYbXlAction(g_Conn,bFlag)
		g_GameMain.m_MsgBox = nil
		return true
	end
	
	local BtnType = BitOr(MB_BtnOK, MB_BtnCancel)
	if Is15Floor then
		BtnType = BitOr(MB_BtnOK)
	end
	
	local strInfo = MsgBoxMsg(5009,LifeNum)
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, strInfo, BtnType, CallBack, g_GameMain)
	local Btn1 = g_GameMain.m_MsgBox:GetDlgChild("BtnOK")
	Btn1:SetWndText(MsgBoxMsg(5011))
	if not Is15Floor then
		local Btn2 = g_GameMain.m_MsgBox:GetDlgChild("BtnCancel")
		if Btn2 then
			Btn2:SetWndText(MsgBoxMsg(5012))
		end
	end
end

--function CYbEducateActionWnd:PlayXiuXingTaBgm(Tick, musicName, cfgTbl)
----	if g_MusicCDTbl[musicName] then
----		--print("音乐", musicName, "CD 中")
----		return
----	end
--	PlayCue(musicName)
--	print("播放背景音乐 ".. musicName)
--	if g_GameMain.m_PlayXiuXingTaBgmTick then
--		UnRegisterTick(g_GameMain.m_PlayXiuXingTaBgmTick)
--		g_GameMain.m_PlayXiuXingTaBgmTick = nil
--	end
--	if cfgTbl then
--		local playLength = cfgTbl.MusicTime -- * math.random(cfgTbl.MinPlayCount, cfgTbl.MaxPlayCount)
--		if g_GameMain.m_StopXiuXingTaBgmTick then
--			UnRegisterTick(g_GameMain.m_StopXiuXingTaBgmTick)
--			g_GameMain.m_StopXiuXingTaBgmTick = nil
--		end
--		g_GameMain.m_StopXiuXingTaBgmTick = RegClassTick("StopXiuXingTaBgmTick", StopXiuXingTaBgm, playLength * 1000, musicName, cfgTbl)
--	else
--		--print("背景音乐 (" ..  musicName .. ") 没有填写对应的播放规则.请检测配置表BackgroundMusic_Client")
--	end	
--end
--
--function CYbEducateActionWnd:StopXiuXingTaBgm(Tick, musicName, cfgTbl)
--	StopCue(musicName)
--	print("停止播放背景音乐 ".. musicName)
--	if g_GameMain.m_StopXiuXingTaBgmTick then
--		UnRegisterTick(g_GameMain.m_StopXiuXingTaBgmTick)
--		g_GameMain.m_StopXiuXingTaBgmTick = nil
--	end
--	if cfgTbl then
--		local stopLength = cfgTbl.WaitTime -- * math.random(cfgTbl.MinWaitCount, cfgTbl.MaxWaitCount)
--		if g_GameMain.m_PlayXiuXingTaBgmTick then
--			UnRegisterTick(g_GameMain.m_PlayXiuXingTaBgmTick)
--			g_GameMain.m_PlayXiuXingTaBgmTick = nil
--		end
--		g_GameMain.m_PlayXiuXingTaBgmTick = RegClassTick("PlayXiuXingTaBgmTick", PlayXiuXingTaBgm, stopLength * 1000, musicName, cfgTbl)
--	else
--		--print("背景音乐 (" ..  musicName .. ") 没有填写对应的播放规则.请检测配置表BackgroundMusic_Client")
--	end
--end
--
