local os = os
local event_type_tbl = event_type_tbl
local StmtContainer = class()

local StmtDef=
{
 "InsertDaTaoShaPoint",
 "insert into tbl_char_dataoshapoint(cs_uId,cs_uWinNum,cs_uLoseNum,cs_uRunNum,cs_udataoshapoint) values(?,?,?,?,?)"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_UpdateDaTaoShaPoint",
	[[
		update
			tbl_char_dataoshapoint
		set
			cs_uWinNum = cs_uWinNum + ?,
			cs_uLoseNum = cs_uLoseNum + ?,
			cs_uRunNum = cs_uRunNum + ?,
			cs_udataoshapoint = cs_udataoshapoint + ?,
			cd_uInFbTimes = ?,
			cd_dDate = from_unixtime(?)
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetPlayerInFbTimes",
	"select cd_uInFbTimes,unix_timestamp(cd_dDate) from tbl_char_dataoshapoint where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetPlayerMatchInfo",
	"select cs_uWinNum,cs_uLoseNum,cs_uRunNum,cs_udataoshapoint from tbl_char_dataoshapoint where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)
------------------------------------------------------------


local StmtDef=
{
	"_InserPointAccum",
	[[
	insert into	tbl_char_dataoshaaccum
		(cs_uId,cs_uUpWeekWinNum,cs_uUpWeekLoseNum,cs_uUpWeekRunNum,cs_uUpWeekPoint,cs_dUpDate,cs_uCurrWeekWinNum,cs_uCurrWeekLoseNum,cs_uCurrWeekRunNum,cs_uCurrWeekPoint,cs_dCurrDate)
	values
		(?,?,?,?,?,from_unixtime(?),?,?,?,?,from_unixtime(?))
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_UpdateCurrWeekPointAccum",
	[[
	update
		tbl_char_dataoshaaccum
	set
		cs_uCurrWeekWinNum = cs_uCurrWeekWinNum + ?,
		cs_uCurrWeekLoseNum = cs_uCurrWeekLoseNum + ?,
		cs_uCurrWeekRunNum = cs_uCurrWeekRunNum + ?,
		cs_uCurrWeekPoint = cs_uCurrWeekPoint + ?,
		cs_dCurrDate = from_unixtime(?)
	where
		cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_UpdateAllPointAccum",
	[[
	update
		tbl_char_dataoshaaccum
	set
		cs_uUpWeekWinNum = cs_uUpWeekWinNum + ?,
		cs_uUpWeekLoseNum = cs_uUpWeekLoseNum + ?,
		cs_uUpWeekRunNum = cs_uUpWeekRunNum + ?,
		cs_uUpWeekPoint = cs_uUpWeekPoint + ?,
		cs_dUpDate = from_unixtime(?),
		cs_uCurrWeekWinNum = cs_uCurrWeekWinNum + ?,
		cs_uCurrWeekLoseNum = cs_uCurrWeekLoseNum + ?,
		cs_uCurrWeekRunNum = cs_uCurrWeekRunNum + ?,
		cs_uCurrWeekPoint = cs_uCurrWeekPoint + ?,
		cs_dCurrDate = from_unixtime(?)
	where
		cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetPointAccumData",
	"select cs_uCurrWeekWinNum,cs_uCurrWeekLoseNum,cs_uCurrWeekRunNum,cs_uCurrWeekPoint,unix_timestamp(cs_dCurrDate) from tbl_char_dataoshaaccum where	cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local DaTaoShaFb = CreateDbBox(...)

function DaTaoShaFb.insertDaTaoShaPoint(uCharId,WinNum,LoseNum,RunNum,Point)
	if uCharId and Point then
		StmtContainer.InsertDaTaoShaPoint:ExecSql("",uCharId,WinNum,LoseNum,RunNum,Point)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
end

function DaTaoShaFb.InserPointAccum(uCharId,WinNum,LoseNum,RunNum,Point,Date)
	if uCharId and Point and Date then
		StmtContainer._InserPointAccum:ExecSql("",uCharId,0,0,0,0,0,WinNum,LoseNum,RunNum,Point,Date)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
end

local function IsSameDay(EndTime,NowTime)
	if NowTime == nil or EndTime == nil then
		return false
	end
	local date1 = os.date("*t", NowTime)
	local date2 = os.date("*t", EndTime)
	if date1.year == date2.year and date1.month == date2.month and date1.day == date2.day then
		return true
	else
		return false
	end
end

function DaTaoShaFb.updateDaTaoShaPoint(data)
	local uCharId = data["PlayerId"]
	local SceneName = "角斗场副本"
	local Point = data["MatchPoint"]
	local GloryPoint = data["GloryPoint"]
	local State = data["State"]
	local addExp = data["AddExp"]
	local Time = data["CurrTime"]
	local MaxPoint = data["MaxPoint"]
	local AwardInfo = nil
	
	if uCharId and Point then
		local WinNum = 0
		local LoseNum = 0
		local RunNum = 0
		if State == 1 then
			WinNum = 1
		elseif State == 2 then
			LoseNum = 1
		else
			GloryPoint = 0--逃跑的没有荣誉积分
			RunNum = 1
		end
		
		--给物品奖励
		local RoleQuestDB = RequireDbBox("RoleQuestDB")
		if data["AwardItem"] and Point > MaxPoint then
			local param = {}
			param["MailTitle"] = 1022
			param["MailContent"] = 6001
			
			param["sceneName"] = SceneName
			param[uCharId] = {}
			param[uCharId].isInGame = true
			param[uCharId].itemTbl = data["AwardItem"]
			local res,Info  = RoleQuestDB.FbActionAward(param)
			if not res then
				return false
			end
			AwardInfo = Info
		end
		
		local LogMgr = RequireDbBox("LogMgrDB")
		local eventId = LogMgr.SaveDaTaoShaPointInfo(uCharId,Point)
		local IsSaveGloryPoint = false
		local MatchTimes = 0
		local res = StmtContainer._GetPlayerInFbTimes:ExecSql("nn",uCharId)
		if res then
			local RowNum = res:GetRowNum()
			if RowNum > 0 then
				MatchTimes = res:GetData(0,0)
				local iEndTime = res:GetData(0,1)
				res:Release()
				if MatchTimes < 50 then	--一天能玩的场数
					IsSaveGloryPoint = true
				end
				local IsSameTime = IsSameDay(iEndTime,Time)
				if IsSameTime then
					MatchTimes = MatchTimes + 1
				else
					MatchTimes = 1
					IsSaveGloryPoint = true
				end
			end
		end
		
		if MatchTimes >= 50 then
			GloryPoint = 0--比赛次数超了,也不给荣誉积分
		end
		
		if RunNum == 0 and IsSaveGloryPoint then
			local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
			if AreaFbPointDB.CountPointLimit(4) > 0 then
				return false,20
			end
			if GloryPoint > 0 then
				local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
				AreaFbPointDB.AddAreaFbPointByType(uCharId,GloryPoint,4,SceneName,eventId)
			end
		end
		
		local CurLevel = nil
		local LevelExp = nil
		if addExp and addExp ~= 0 and IsSaveGloryPoint then
			local param = {}
			param["char_id"] = uCharId
			param["addExp"] = addExp
			param["uEventId"] = eventId
			CurLevel,LevelExp = RoleQuestDB.AddExp(param)
		end
		
		StmtContainer._UpdateDaTaoShaPoint:ExecSql('',WinNum,LoseNum,RunNum,Point,MatchTimes,Time,uCharId)
		if g_DbChannelMgr:LastAffectedRowNum() > 0 then
			DaTaoShaFb.updateDaTaoShaWeekPoint(uCharId,WinNum,LoseNum,RunNum,Point,Time)
			local respoint,MatchInfo = DaTaoShaFb.GetPlayerMatchInfo(uCharId,State)
			if respoint then
				return true,50-MatchTimes,MatchInfo and MatchInfo..Point,CurLevel,LevelExp,AwardInfo
			else
				return false
			end
		end
	end
	return false
end

--判断是不是想要的那一周
--传入0,判断是不是当前周
--传入1,判断是不是上一周
local function IsSameWeek(OldDate, CurrDate, WeekNum)
	if OldDate == nil or CurrDate == nil then
		return false
	end
	
	local DayTime = 24*60*60	--一天的秒数
	local WeekTime = 7*DayTime	--周的秒数
	
	local NowDay = 1
	local date = os.date("*t",OldDate)
	local DayCoef = date.wday
	if DayCoef > NowDay then				--要用到那一天(即星期几,这一天就用1代表)
		DayCoef = DayCoef - NowDay
	else
		DayCoef = DayCoef + (7-NowDay)
	end
	
	local InitWeekTime = (DayCoef-1) * DayTime --这些天的秒数(为了让时间全部回到初始状态,例如:周日[周日为每一周的第一天])
	
	local WeekCoef = math.floor((CurrDate-(OldDate-InitWeekTime)) / WeekTime)
	
	if WeekCoef == WeekNum then
		--print("...........in same")
		return true
	else
		--print("...........in not same")
		return false
	end
end

function DaTaoShaFb.updateDaTaoShaWeekPoint(uCharId,WinNum,LoseNum,RunNum,Point,Time)
	local res = StmtContainer._GetPointAccumData:ExecSql("nnnnn",uCharId)
	if not res or res:GetRowNum() <= 0 then
		res:Release()
		DaTaoShaFb.InserPointAccum(uCharId,WinNum,LoseNum,RunNum,Point,Time)
	else
		local CurrWeekWinNum = res:GetData(0,0)
		local CurrWeekLoseNum = res:GetData(0,1)
		local CurrWeekRunNum = res:GetData(0,2)
		local CurrWeekPoint = res:GetData(0,3)
		local Data = res:GetData(0,4)
		if IsSameWeek(Data,Time,0) then
			StmtContainer._UpdateCurrWeekPointAccum:ExecSql('',WinNum,LoseNum,RunNum,Point,Time,uCharId)
		else
			StmtContainer._UpdateAllPointAccum:ExecSql('',CurrWeekWinNum,CurrWeekLoseNum,CurrWeekRunNum,CurrWeekPoint,Data,WinNum,LoseNum,RunNum,Point,Time,uCharId)
		end
	end
end

function DaTaoShaFb.GetPlayerMatchInfo(PlayerId,State)
	local res1 = StmtContainer._GetPlayerMatchInfo:ExecSql('nnnn',PlayerId)
	if res1 then
		if res1:GetRowNum() > 0 then
			local CountPoint = res1:GetData(0,3)
			
			if State ~= 0 then
				local res2 = StmtContainer._GetPointAccumData:ExecSql('nnnnn',PlayerId)
				if res2:GetRowNum() > 0 then
					local WinNum = res1:GetData(0,0)
					local LoseNum = res1:GetData(0,1)
					local RunNum = res1:GetData(0,2)
					local Point = res1:GetData(0,3)
					
					local CurrWeekWinNum = res2:GetData(0,0)
					local CurrWeekLoseNum = res2:GetData(0,1)
					local CurrWeekRunNum = res2:GetData(0,2)
					local CurrWeekPoint = res2:GetData(0,3)
					
					local wMatchTimes = CurrWeekWinNum + CurrWeekLoseNum + CurrWeekRunNum--比赛次数
					local cMatchTimes = WinNum + LoseNum + RunNum--总的比赛次数
					
					res1:Release()
					res2:Release()
					return Point,CurrWeekWinNum..","..WinNum..","
				end
			end
			
			res1:Release()
			return CountPoint
		end
	end
end

SetDbLocalFuncType(DaTaoShaFb.updateDaTaoShaPoint)
return DaTaoShaFb