cfg_load "fb_game/ActionExtraReward_Server"

--以下数据为一天一计算的(或服务器启动时候需要计算的)
local criticalStartTime = nil 	
local criticalEndTime = nil
local extraRewardData = nil



--不要主动调用此函数, 只需调用GetCriticalTime 就会在必要的时机调用 此函数,
local function FindExtraRewardData( date)
	local tbl
	for _, i in pairs(ActionExtraReward_Server:GetKeys()) do
		local v = ActionExtraReward_Server(i)
		local dateTemp = GetCfgTransformValue(true,"ActionExtraReward_Server", i,"Date")--loadstring( "return {" .. v("Date") .. "}")()
		if dateTemp.month == date.month and dateTemp.day == date.day then
			tbl = v
			break
		elseif dateTemp.wday then
			if dateTemp.wday + 1 == date.wday or (dateTemp.wday == 7 and date.wday == 1)then
				tbl = v
			end
		end
	end
	if tbl then
		local data = {}
		data.AllRewardCount = tbl("AllRewardCount")
		data.ActionTbl = {}
		data.ActionCountTbl = {}
		data.ExpModulusTbl = {}
		data.MoneyModulusTbl = {}
		local i = 1
		while(true) do
			local actionName = tbl("Action"..i)
			local actionCount = tbl("Count"..i)
			local expModulus = tbl("ExpModulus"..i)
			local moneyModulus = tbl("MoneyModulus"..i)
			if actionName == nil or actionName == "" then
				break
			end
			table.insert(data.ActionTbl, actionName)
			table.insert(data.ActionCountTbl, actionCount)
			table.insert(data.ExpModulusTbl, expModulus)
			table.insert(data.MoneyModulusTbl, moneyModulus)
			i = i + 1
		end
		if i > 1 then
			return data
		end
	end
end

--参数nowTime 是为了在不同处调用的时间同一, 所以不在函数内部调用 os.time()
function GetTodayExtraRewardInfo(nowTime)
	if criticalStartTime then
		if nowTime >= criticalEndTime then
			criticalStartTime = criticalEndTime
			criticalEndTime = criticalEndTime + 24 * 60 * 60
			extraRewardData = FindExtraRewardData(os.date("*t", nowTime))
		end
		return extraRewardData, criticalStartTime
	end
	local nowDate = os.date("*t", nowTime)
	local criticalDate = {}
	criticalDate.year = nowDate.year
	criticalDate.month = nowDate.month
	criticalDate.day = nowDate.day
	criticalDate.hour = 0
	criticalDate.min = 0
	criticalStartTime = os.time(criticalDate)
	criticalEndTime = criticalStartTime + 24 * 60 * 60
	extraRewardData = FindExtraRewardData(nowDate) 
	return extraRewardData, criticalStartTime
end

--返回nil 说明改活动今天没有额外奖励
function GetExtraRewardActionIndex(actionName, nowTime)
--	print("GetExtraRewardActionIndex",actionName, nowTime)
	local data = GetTodayExtraRewardInfo(nowTime)
	if data then
		for index, name in pairs(data.ActionTbl) do
			if actionName == name then
				return index
			end
		end
	end
end