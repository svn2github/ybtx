CSortMgr = class()

function CSortMgr.CreateSortCharInfoFunc1()

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc1", nil, {},"SortChannel")
end

function CSortMgr.CreateSortCharInfoFunc2()

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc2", nil, {},"SortChannel")
end

function CSortMgr.CreateSortCharInfoFunc3()

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc3", nil, {},"SortChannel")
end

function CSortMgr.CreateSortCharInfoFunc4()

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc4", nil, {},"SortChannel")
end


function CSortMgr.CreateSortCharInfoFunc5(uCamp)
	local data = {["uCamp"] = uCamp}

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc5", nil, data,"SortChannel")
end

function CSortMgr.CreateSortCharInfoFunc6(uCamp)
	local data = {["uCamp"] = uCamp}

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc5", nil, data,"SortChannel")
end

function CSortMgr.CreateSortCharInfoFunc7(uCamp)
	local data = {["uCamp"] = uCamp}

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc5", nil, data,"SortChannel")
end


function CSortMgr.CreateSortCharInfoFunc8(uCamp)
	local data = {["uCamp"] = uCamp}

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc6", nil, data,"SortChannel")
end

function CSortMgr.CreateSortCharInfoFunc9(uCamp)
	local data = {["uCamp"] = uCamp}

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc6", nil, data,"SortChannel")
end

function CSortMgr.CreateSortCharInfoFunc10(uCamp)
	local data = {["uCamp"] = uCamp}

	CallDbTrans("SortCharInfoDB", "CreateSortCharInfoFunc6", nil, data,"SortChannel")
end


function CSortMgr.CreateSortFunc5()


	CallDbTrans("SortTongInfoDB", "CreateSortTbl1", nil, {},"SortChannel")
end

function CSortMgr.CreateSortFunc6(uCamp)
	local data = {["uCamp"] = uCamp}

	CallDbTrans("SortTongInfoDB", "CreateSortTbl2", nil, data,"SortChannel")
end

function CSortMgr.CreateSortFunc7(uCamp)
	local data = {["uCamp"] = uCamp}

	CallDbTrans("SortTongInfoDB", "CreateSortTbl2", nil, data,"SortChannel")
end

function CSortMgr.CreateSortFunc8(uCamp)
	local data = {["uCamp"] = uCamp}

	CallDbTrans("SortTongInfoDB", "CreateSortTbl2", nil, data,"SortChannel")
end

function CSortMgr.CreateCharSortTbl()
	local function CallBack1()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc1()
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc1", {wday = {1,2,3,4,5,6,7},time = {"05:00"}}, CallBack1)
	
	local function CallBack2()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc2()
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc2", {wday = {1,2,3,4,5,6,7},time = {"05:02"}}, CallBack2)
	
	local function CallBack3()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc3()
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc3", {wday = {1,2,3,4,5,6,7},time = {"05:04"}}, CallBack3)
	
	local function CallBack4()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc4()
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc4", {wday = {1,2,3,4,5,6,7},time = {"05:06"}}, CallBack4)
	
	local function CallBack5()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc5(1)
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc5", {wday = {1,2,3,4,5,6,7},time = {"05:08"}}, CallBack5)
	
	local function CallBack6()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc6(2)
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc6", {wday = {1,2,3,4,5,6,7},time = {"05:10"}}, CallBack6)
	
	local function CallBack7()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc7(3)
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc7", {wday = {1,2,3,4,5,6,7},time = {"05:12"}}, CallBack7)
	
	local function CallBack8()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc8(1)
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc8", {wday = {1,2,3,4,5,6,7},time = {"05:14"}}, CallBack8)
	
	local function CallBack9()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc9(2)
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc9", {wday = {1,2,3,4,5,6,7},time = {"05:16"}}, CallBack9)
	
	local function CallBack10()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortCharInfoFunc10(3)
		end
	end
	g_AlarmClock:AddTask("CreateSortCharInfoFunc10", {wday = {1,2,3,4,5,6,7},time = {"05:18"}}, CallBack10)
end

function CSortMgr.CreateTongSortTbl()
	local function CallBack1()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortFunc5()
		end
	end
	g_AlarmClock:AddTask("CreateSortTblTick11", {wday = {1,2,3,4,5,6,7},time = {"05:20"}}, CallBack1)
	
	local function CallBack2()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortFunc6(1)
		end
	end
	g_AlarmClock:AddTask("CreateSortTblTick12", {wday = {1,2,3,4,5,6,7},time = {"05:22"}}, CallBack2)
	
	local function CallBack3()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortFunc7(2)
		end
	end
	g_AlarmClock:AddTask("CreateSortTblTick13", {wday = {1,2,3,4,5,6,7},time = {"05:24"}}, CallBack3)
	
	local function CallBack4()
		if not g_App:CallDbHalted() then
			CSortMgr.CreateSortFunc8(3)
		end
	end
	g_AlarmClock:AddTask("CreateSortTblTick14", {wday = {1,2,3,4,5,6,7},time = {"05:26"}}, CallBack4)
end

local function GetSortList(Conn,res,sType,sName)
	Gas2Gac:RetSortListBegin(Conn)
	if("个人" == sType and "个人情况" == sName) then
		for i, v in ipairs(res) do
			if res[i] then
				Gas2Gac:RetSortList(Conn,v[1],v[2],v[3],v[4],v[5])
			end
		end
	else
		if res:GetRowNum() > 0  then
			local res_tbl = res:GetTableSet()
			for i = 1, res:GetRowNum() do
				local uValue = res_tbl(i,5)
				if("个人" == sType and "战斗力评分榜" == sName) then
					uValue = string.format("%.2f",res_tbl(i,5))
				end
				Gas2Gac:RetSortList(Conn,res_tbl(i,1),res_tbl(i,2),res_tbl(i,3),res_tbl(i,4),uValue)
			end
		end
	end
	Gas2Gac:RetSortListEnd(Conn,sType,sName)
end

function CSortMgr.GetSortList(Conn,sType,sName,campType)
	if nil == Conn.m_Player then
		return 
	end
	
	local data = {
					["sType"] = sType,
					["sName"] = sName,
					["campType"] = campType,
					["playerId"] = Conn.m_Player.m_uID
				}
	local function callback(res)
		GetSortList(Conn,res,sType,sName)
	end
	
	CallAccountManualTrans(Conn.m_Account, "SortCharInfoDB", "GetSortList", callback, data)
end
