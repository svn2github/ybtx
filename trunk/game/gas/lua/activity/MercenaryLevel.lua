gac_gas_require "activity/quest/MercenaryLevelCfg"
gas_require "activity/quest/RoleQuest"
cfg_load "appellation/MercenaryAppellationText_Server"

CMercenaryLevel = class()

local function CreateMercenaryLevel(Conn)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local function CallBack(MLRes)
		if not (IsCppBound(Conn) and Conn.m_Player) then
			return
		end
		CMercenaryLevel.UpdateMercenaryLevelTraceWnd(Conn, MLRes)
		Gas2Gac:ShowMercenaryLevelMessage(Conn, 0)
	end
	local data = {}
	data["CharId"] = Conn.m_Player.m_uID
	CallAccountManualTrans(Conn.m_Account, "MercenaryLevelDB", "AddNewMercenaryLevel", CallBack, data)
end

local function MercenaryLevelUp(Conn)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local function CallBack(MLRes)
		local Player = Conn.m_Player
		if not (IsCppBound(Conn) and Player) then
			return
		end
		if MLRes then
			Player.m_uMercenaryLevel = Player.m_uMercenaryLevel + 1
			Gas2Gac:RetSetMercenaryLevel(Player.m_Conn, Player.m_uMercenaryLevel, true)
			Gas2Gac:ClearMercenaryLevelFinishAwards(Conn)
			Gas2Gac:ShowMercenaryLevelMessage(Conn, 6)
			CMercenaryLevel.UpdateMercenaryLevelTraceWnd(Conn, MLRes)
			Gas2Gac:RetShowOpenYbAssessWnd(Conn)
		end
	end
	local data = {}
	data["CharId"] = Conn.m_Player.m_uID
	data["Defence"] = Conn.m_Player:GetDefence()
	CallAccountManualTrans(Conn.m_Account, "MercenaryLevelDB", "MercenaryLevelUp", CallBack, data)
end

--服务端调用，初始化佣兵追踪面板及相关信息
function CMercenaryLevel.InitMercenaryLevelTraceWnd(Conn, MLRes, Awards)
	if not IsCppBound(Conn) then
		return
	end
	if Awards then
		Gas2Gac:ClearMercenaryLevelFinishAwards(Conn)
		for award,_ in pairs(Awards) do
			Gas2Gac:SetMercenaryLevelFinishAwards(Conn, award)
		end
	end
	CMercenaryLevel.UpdateMercenaryLevelTraceWnd(Conn, MLRes)
end

function CMercenaryLevel.AddMercenaryLevelCount(Conn, Subject, Param)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local function CallBack(MLRes)
		if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
		CMercenaryLevel.UpdateMercenaryLevelTraceWnd(Conn, MLRes)
	end
	local data = {}
	data["CharId"] = Conn.m_Player.m_uID
	data["Subject"] = Subject
	data["Param"] = Param
	CallAccountAutoTrans(Conn.m_Account, "MercenaryLevelDB", "AddMercenaryLevelCount", CallBack, data)
end

function CMercenaryLevel.UpdateMercenaryLevelTraceWnd(Conn, MLRes)
	if not IsCppBound(Conn) then
		return
	end
	if MLRes then
		local MercenaryLevel	= MLRes[1]
		local Status 					= MLRes[2]
		local CountTbl 				= MLRes[3]
		local Challenge 			= MLRes[4]
		local Integral 				= MLRes[5]
		local Level 					= MLRes[6]
		Gas2Gac:RetUpdateMercenaryLevelTrace(Conn,MercenaryLevel,Status,CountTbl.Subject1,CountTbl.Subject2,CountTbl.Subject3,CountTbl.Count1,CountTbl.Count2,CountTbl.Count3,Challenge,Integral,Level)
	end
end

function CMercenaryLevel.AddLevelUpdateMercenaryLevelTraceWnd(Player)
	if not (Player and IsCppBound(Player.m_Conn)) then
		return
	end
	local function CallBack(MercenaryLevel, Status, CountTbl, Challenge, Integral, Level)
		if not (Player and IsCppBound(Player.m_Conn)) then
			return
		end
		if MercenaryLevel then
			Gas2Gac:RetUpdateMercenaryLevelTrace(Player.m_Conn,MercenaryLevel,Status,CountTbl.Subject1,CountTbl.Subject2,CountTbl.Subject3,CountTbl.Count1,CountTbl.Count2,CountTbl.Count3,Challenge,Integral,Level)
		end
	end
	local data = {}
	data["CharId"] = Player.m_uID
	CallAccountAutoTrans(Player.m_Conn.m_Account, "MercenaryLevelDB", "GetMercenaryLevelInfo", CallBack, data)
end

local function DoMercenaryLevelAppraise(Conn)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local function CallBack(MLRes)
		if not (IsCppBound(Conn) and Conn.m_Player) then
			return
		end
		if MLRes then
			if tonumber(MLRes) then
				Gas2Gac:ShowMercenaryLevelMessage(Conn, 9)
			else
				CMercenaryLevel.UpdateMercenaryLevelTraceWnd(Conn, MLRes)
			end
		else
			Gas2Gac:ShowMercenaryLevelMessage(Conn, 7)
		end
	end
	local data = {}
	data["CharId"] = Conn.m_Player.m_uID
	CallAccountManualTrans(Conn.m_Account, "MercenaryLevelDB", "MercenaryLevelAppraise", CallBack, data)
end

local function GetMercenaryLevelAward(Conn, Type)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local function CallBack(Awards)
		if not (IsCppBound(Conn) and Conn.m_Player) then
			return
		end
		if Awards then
			Gas2Gac:ClearMercenaryLevelFinishAwards(Conn)
			for award,_ in pairs(Awards) do
				Gas2Gac:SetMercenaryLevelFinishAwards(Conn, award)
			end
			if Type then
				Gas2Gac:RetShowMercenaryLevelAward(Conn)
			end
		else
			Gas2Gac:ShowMercenaryLevelMessage(Conn, 8)
		end
	end
	local data = {}
	data["CharId"] = Conn.m_Player.m_uID
	data["Type"] = Type
	CallAccountManualTrans(Conn.m_Account, "MercenaryLevelDB", "GetMercenaryLevelAward", CallBack, data)
end

local function SetMercenaryLevelAward(Conn, Award)
	if not Award or Award == "" then
		return
	end
	if not (IsCppBound(Conn) and Player) then
		return
	end
	
	local tbl = g_MercenaryLevelTbl[Player.m_uMercenaryLevel]["特权"]
	if string.find(Award, "技能") and tbl[Award] and tbl[Award].Arg then
		Player:Lua_AddNonFightSkill(tbl[Award].Arg)
		return
	end
	local PlayerId = Player.m_uID
	local function CallBack(result)
		if result then
			--高速处理
			if string.find(Award, "物品") then
				CRoleQuest.add_item_DB_Ret_By_Id(PlayerId,	result[1],result[2],result[3],result[4])
			elseif Award == "称号" and g_MercenaryLevelTbl[Player.m_uMercenaryLevel] then
				if IsCppBound(Conn) and IsCppBound(Conn.m_Player) then
					local Player = Conn.m_Player
					--调用加佣兵称谓接口
					local Appellation = g_MercenaryLevelTbl[Player.m_uMercenaryLevel]["特权"][Award].Arg
					AddNewAppellation(Player, Appellation)
				end
			end
			if IsCppBound(Conn) then
				Gas2Gac:RetUpdateMercenaryLevelAwardItem(Conn, Award)
			end
		end
	end
	local data = {}
	data["CharId"] = Player.m_uID
	data["Award"] = Award
	data["SceneName"] = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, "MercenaryLevelDB", "SetMercenaryLevelAward", CallBack, data)
end

function CMercenaryLevel.CreateMercenaryLevel(Conn)
	CreateMercenaryLevel(Conn)
end

function CMercenaryLevel.MercenaryLevelUp(Conn)
	MercenaryLevelUp(Conn)
end

function CMercenaryLevel.DoMercenaryLevelAppraise(Conn)
	DoMercenaryLevelAppraise(Conn)
end

function CMercenaryLevel.GetMercenaryLevelAward(Conn, Type)
	GetMercenaryLevelAward(Conn, Type)
end

function CMercenaryLevel.SetMercenaryLevelAward(Conn, Award)
	SetMercenaryLevelAward(Conn, Award)
end
