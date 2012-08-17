CRecruitMgr = class()

function CRecruitMgr.AutoSetRecruit(data)
	CallDbTrans("RecruitPlayerDB", "SetGmRecruitInfo", nil, data, "Recruit")
end

function CRecruitMgr.GetRecruitInfo(Conn)
	local function CallBack(result)
		if not result then
			return
		end
		Conn.m_RecruitInfo = result
		Gas2Gac:ReturnRecruitInfo(Conn, result[1], result[2], result[3])
	end
	CallDbTrans("RecruitPlayerDB", "GetRecruitCampInfo", CallBack, {})
end


function CRecruitMgr.GetCampInfoByNum(Conn, returnFlag)
	local function CallBack(result)
		if not result then
			return
		end
		Conn.m_RecruitInfo = result
		Gas2Gac:RetReturnRecruitInfo(Conn, result[1], result[2], result[3], returnFlag)
	end
	CallDbTrans("RecruitPlayerDB", "GetRecruitCampInfo", CallBack, {})
end

local function RegisterAutoRecruitTick (setTime)
	if g_App.m_RecruitCamp then
		UnRegisterTick(g_App.m_RecruitCamp)
		g_App.m_RecruitCamp = nil
	end
	local data = {}
	data["method"] = 1
	local function RecruitCamp(Tick, data)
		CallDbTrans("RecruitPlayerDB", "SetGmRecruitInfo", CallBack, data, "Recruit")
	end
	g_App.m_RecruitCamp = RegisterTick("RecruitCamp", RecruitCamp, setTime * 60 * 1000, data)
end

function CRecruitMgr.DoGMCommand(method, camp1, camp2, camp3)
	assert(g_CurServerId == 1, "SetRecruitMethod 未在1服中处理")
	local data = {}
	data["method"] = method
	data["camp1"] = camp1
	data["camp2"] = camp2
	data["camp3"] = camp3
	
	local function CallBack(setTime)
		if method == 1 then
			RegisterAutoRecruitTick(setTime)
		else
			if g_App.m_RecruitCamp then
				UnRegisterTick(g_App.m_RecruitCamp)
				g_App.m_RecruitCamp = nil
			end
		end
	end
	
	CallDbTrans("RecruitPlayerDB", "SetGmRecruitInfo", CallBack, data, "Recruit")
end

function CRecruitMgr.ResumeRecruitState()
	if g_CurServerId ~= 1 then   --只有1服处理tick
		return
	end
	local function CallBack(isAutoRecruit, setTime)
		if isAutoRecruit then
			RegisterAutoRecruitTick(setTime)
		else
			if g_App.m_RecruitCamp then
				UnRegisterTick(g_App.m_RecruitCamp)
				g_App.m_RecruitCamp = nil
			end
		end
	end
	CallDbTrans("RecruitPlayerDB", "IsAutoRecruit", CallBack, {}, "Recruit")
end

function Gas2GasDef:SetRecruitMethod(Conn, method, camp1, camp2, camp3)
	CRecruitMgr.DoGMCommand(method, camp1, camp2, camp3)
end
