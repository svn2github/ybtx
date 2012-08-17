cfg_load "tong/TongStartTime_Common"

local ResumeTime = GetCfgTransformValue(false, "TongStartTime_Common", "军资每天发放时间", "OpenTime")
local IssueForageTime = GetCfgTransformValue(false, "TongStartTime_Common", "征粮令结束时间", "OpenTime")
CFetchRes = class()

local function Resume()
	CallDbTrans("GasTongForageDB", "GetAllForageInfo", nil, {}, "DrawForage")
end

local function IssueForageOver()
	CallDbTrans("GasTongMarketDB", "DeleteIssueInfo", nil, {}, "IssueForageOver")
end

function CFetchRes:DoForageNum(Player)
	if not IsCppBound(Player) then
		return 
	end
	local function CallBack(result,res, forageNum)
		if not result then
			return 
		end
		if not res then
			MsgToConn(Player.m_Conn, 9047)
			return
		end
		if IsTable(res) then
			local num = res[1].m_nCount
			MsgToConn(Player.m_Conn, 9046, num, forageNum)
		else
			MsgToConn(Player.m_Conn, res)
			---LogErr("删除军资牌失败","玩家("..Player.m_Properties:GetCharName()..") ID("..Player.m_uID..")", Player.m_Conn)
			return
		end
	end
	local parameters = {}
	parameters["nCharID"]	= Player.m_uID
	parameters["sceneName"]	= Player.m_Scene.m_SceneName
	CallAccountManualTrans(Player.m_Conn.m_Account, "GasTongForageDB", "UpdateForageInfoAndDel", CallBack, parameters)
end

function CFetchRes:GetHandAndResForageNum(Player)
	if not IsCppBound(Player) then
		return 
	end
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InDuel) or Player.m_ChallengeFlag then
		MsgToConn(Player.m_Conn,193526)
		return
	end
	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
	local FetchNum = g_TongMgr.m_tblTruckMaxLoad["中型运输车"]
	if Truck then
		FetchNum = Truck.m_TruckInfo:GetOddResource()
		if FetchNum == 0 then
			MsgToConn(Player.m_Conn, 9023)
			return
		end
	end
	
	local function CallBack(flag, num)
		if not flag then
			return
		end
		if not IsCppBound(Player) then
			return 
		end
		if num == 0 then
			return
		end
		local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
		if Truck then
			Truck.m_TruckInfo:LoadResource(num)
		else
			local res = Player:PlayerDoPassiveSkill("召唤中型运输车",1)
			if res == EDoSkillResult.eDSR_Success then
				Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
				if Truck then
					Truck.m_TruckInfo:LoadResource(num)
				end
			end
		end
		AddVarNumForTeamQuest(Player, "完成军资运输", 1)
	end
	local parameters = {}
	parameters["charId"] = Player.m_uID
	parameters["FetchNum"] = FetchNum
	CallAccountManualTrans(Player.m_Conn.m_Account, "GasTongForageDB", "GetRestForageInfo", CallBack, parameters)
end

function CFetchRes:ResumeForage()  --起服时
	local nowdate =  os.time()
	CallDbTrans("GasTongForageDB", "GetDrawForagTime", nil, {nowdate}, "DrawForageTime")
end

g_AlarmClock:AddTask("WarStart", ResumeTime, Resume)  --每天加50
function CFetchRes:CanIssueFetch(Conn)
	if not IsCppBound(Conn) then
		return 
	end
	if not IsCppBound(Conn.m_Player) then
		return 
	end
	--IssueFetch
	local tongId = Conn.m_Player.m_Properties:GetTongID()
	if tongId == 0 then
		MsgToConn(Conn,9350)
		return
	end
	local function CallBack(result)
		if not result then
			MsgToConn(Conn,9352)
			return 
		end
		Gas2Gac:RetOpenFetchWnd(Conn)
	end
	local data = {}
	data["uPlayerId"] = Conn.m_Player.m_uID
	data["PurviewName"] = "IssueFetch"
	CallAccountManualTrans(Conn.m_Account,"GasTongBasicDB", "IsPurviewEnough", CallBack, data, "IssueFetch")
end

function CFetchRes:RetFetchInfo(Conn)
	local function CallBack(result)
		if not result then
			return 
		end
		for _, tongId in pairs(result) do
			g_IssueForage[tongId] = true	
		end
	end
	CallDbTrans("GasTongMarketDB", "GetStatTongInfo",CallBack, {}, "RetFetchInfo")
end

g_AlarmClock:AddTask("ForageStart", IssueForageTime, IssueForageOver)


function CFetchRes.GetTongFethResInfo(Conn)
	local Player = Conn.m_Player
	if not IsCppBound(Conn) or not IsCppBound(Player) then
		return
	end
	local playerLevel = Player:CppGetLevel()
	if playerLevel < 20 then
		MsgToConn(Conn, 9070)
		return
	end
	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
	local FetchNum = g_TongMgr.m_tblTruckMaxLoad["中型运输车"]
	if Truck then
		FetchNum = Truck.m_TruckInfo:GetOddResource()
	end
	local function CallBack(num)
		if num < FetchNum then
			FetchNum = num
		end
		Gas2Gac:ReturnGetTongFethResInfo(Player.m_Conn, num, FetchNum)
	end
	local parameters = {}
	parameters["charId"] = Player.m_uID
	CallAccountManualTrans(Player.m_Conn.m_Account, "GasTongForageDB", "GetForageInfo", CallBack, parameters)
end

function CFetchRes.SetFetchTimes(Conn, num)
	if not IsCppBound(Conn) then
		return 
	end
		
	if not IsCppBound(Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	local data = {}
	data["charId"] = Player.m_uID
	data["num"] = num
	local function CallBack(result)
		if IsNumber(result) then
			MsgToConn(Player.m_Conn, result)
		end
	end
	CallAccountAutoTrans(Conn.m_Account, "GasTongMarketDB", "SetFetchTimes", CallBack, data)
end
