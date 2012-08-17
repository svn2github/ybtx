gac_require "message/message_box/ClientMsg"

CTimeTrans = class()
local sFormat		= GetStaticTextClient(24001) --%d秒
local msFormat		= GetStaticTextClient(24002) --%d分%d秒
local hmsFormat		= GetStaticTextClient(24003) --%d时%%d分%d秒
local dhmsFormat	= GetStaticTextClient(24004) --%d天%d时%%d分%d秒
local hFormat		= GetStaticTextClient(24005) --%d时
local dhFormat		= GetStaticTextClient(24006) --%天%d时
local dhmFormat		= GetStaticTextClient(24007) --%d天%d时%d分
local hmFormat		= GetStaticTextClient(24008) --%d时%d分
local mFormat		= GetStaticTextClient(24009) --%d分

function CTimeTrans:FormatTime(S)
	assert(IsNumber(S))
	local d = math.floor(S/86400)
	local h = math.floor((S - d*86400)/3600)
	local m = math.floor((S - d*86400 - h*3600)/60)
	local s = math.floor(S - d*86400 - h*3600 - m*60)
	if d == 0 and h == 0 and m == 0 then
		return GetStrAccordWildcard(sFormat, s)
	elseif d == 0 and h == 0 and m ~= 0 then
		return GetStrAccordWildcard(msFormat, m,s)
	elseif d == 0 and h ~= 0 then
		return GetStrAccordWildcard(hmsFormat, h,m,s)
	else
		return GetStrAccordWildcard(dhmsFormat,d,h,m,s)
	end
end

function CTimeTrans:FormatTimeDH(S)
	assert(IsNumber(S))
	local d = math.floor(S/86400)
	local h = math.floor((S - d*86400)/3600)
	
	if d == 0 then
		return GetStrAccordWildcard(hFormat, h)
	else
		return GetStrAccordWildcard(dhFormat,d,h)
	end
end

function CTimeTrans:FormatTimeDHM(S)
	assert(IsNumber(S))
	local d = math.floor(S/86400)
	local h = math.floor((S - d*86400)/3600)
	local m = math.floor((S - d*86400 - h*3600)/60)
	local s = math.floor(S - d*86400 - h*3600 - m*60)
	if d == 0 and h == 0 and m == 0 then
		return GetStrAccordWildcard(sFormat, s)
	elseif d == 0 and h == 0 and m ~= 0 then
		return GetStrAccordWildcard(mFormat, m)
	elseif d == 0 and h ~= 0 then
		return GetStrAccordWildcard(hmFormat, h,m)
	else
		return GetStrAccordWildcard(dhmFormat,d,h,m)
	end
end

g_CTimeTransMgr = g_CTimeTransMgr or CTimeTrans:new()
