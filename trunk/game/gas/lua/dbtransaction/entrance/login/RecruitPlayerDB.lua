local recruitTime = GasConfig.RecruitTime
local GasRecruitSql = CreateDbBox(...)
local StmtOperater = {}	

-------------------------------------------------------------------------------------
----计算活跃角色数量
--local  StmtDef=
--{
-- "GetActivePlayerNum",
-- "select count(*) from tbl_char_static s, tbl_char_onlinetime o where s.cs_uId = o.cs_uId and co_dtLastLogOutTime - now() <= 7"
--}
--DefineSql(StmtDef,StmtOperater)
--
---- 获得个阵营活跃人数
--local StmtDef=
--{
--	"GetActivePlayerByCamp",
--	"select count(*) from tbl_char_static s, tbl_char_onlinetime o where s.cs_uCamp = ? and s.cs_uId = o.cs_uId and co_dtLastLogOutTime - now() <= 7"
--}
--DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_AddRecruitCharInfo",
	"insert into tbl_char_recruit_info values(?,?)"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_GetRecruitCharInfo",
	"select cri_uRecFlag from tbl_char_recruit_info where cs_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_AddRecruitCampInfo",
	"replace into tbl_recruit_camp values (?,?)"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_GetRecruitCampInfo",
	"select rc_uCamp, rc_uState from tbl_recruit_camp"
}
DefineSql(StmtDef,StmtOperater)


local StmtDef=
{
	"GetUserCampInfo",
	[[
		select
		cs_ucamp,count(*)
		from
		(
		select 
		us_uid,cs_ucamp
		from
		 (select t1.us_uid,t2.cb_ulevel,t3.co_dtlastlogintime,t1.cs_ucamp 
		 from tbl_char_static t1,tbl_char_basic t2,tbl_char_onlinetime t3  ,tbl_char t4
		 where t1.cs_uid = t2.cs_uid and t3.cs_uid = t2.cs_uid and t3.cs_uId = t4.cs_uId and t2.cb_ulevel > 20 order by us_uid,cb_ulevel desc,co_dtlastlogintime desc) as a
		 group by  us_uid) as b group by cs_ucamp order by cs_ucamp
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_InsertRecruitTime",
	"insert into tbl_char_recruit_time values(now())"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_GetRecruitTime",
	"select unix_timestamp(crt_dtTime) from tbl_char_recruit_time"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_DelRecruitTime",
	"delete  from tbl_char_recruit_time"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_updateRecruitTime",
	"update tbl_char_recruit_time set crt_dtTime = now()"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_InsertRecruitInfo",
	"insert into tbl_recruit_info values(?,?,?)"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_updateRecruitInfo",
	"update tbl_recruit_info set ri_uMinNum = ?, ri_uMaxNum = ?, ri_uTime = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
	"_GetRecruitInfo",
	"select ri_uMinNum, ri_uMaxNum, ri_uTime from tbl_recruit_info"
}
DefineSql(StmtDef,StmtOperater)


--------------------------------------------------------------------------------------------
function GasRecruitSql.UpdateRecruitTime()
	local res = StmtOperater._GetRecruitTime:ExecStat()
	if res:GetRowNum() > 0 then
		StmtOperater._updateRecruitTime:ExecStat()
	else
		StmtOperater._InsertRecruitTime:ExecStat()
	end
end

function GasRecruitSql.GetRecruitCharInfo(data)
	local charId = data["char_id"]
	local res = StmtOperater._GetRecruitCharInfo:ExecStat(charId)
	local recruitFlag = 0
	if res:GetRowNum() > 0 then
		recruitFlag = res:GetData(0,0)
	end
	return recruitFlag
end


local function GetRecruitSetInfo()
	local res = StmtOperater._GetRecruitInfo:ExecStat()
	local minNum, maxNum, time = 100,200,720
	if res:GetRowNum() > 0 then
		minNum, maxNum, time = res:GetData(0,0), res:GetData(0,1),res:GetData(0,2)
	end
	return time
end


--@brief 判断招募令信息
function GasRecruitSql.GetRecruitInfo()
	local GasCreateRoleSql = RequireDbBox("GasCreateRoleDB")
	local totalNum = GasCreateRoleSql.GetTotalChar()
	local tbl = {}
	for i = 1, 3 do 
		local campNum = GasCreateRoleSql.GetCharCountByCamp(i)
		if campNum / totalNum > 0.3 and campNum / totalNum < 0.32 then
			tbl[i] = 1 
		elseif campNum / totalNum <= 0.3 then
			tbl[i] = 2 
		elseif campNum / totalNum >= 0.32 then
			tbl[i] = 0  
		end
	end
	return tbl
end

local function GetCampNum(value1, value2, value3)
	local camp1, camp2, camp3 = 0,0,0
	if value1 > value2 then
		if value1 > value3 then
			camp1 = 1
			if value2 > value3 then
				camp2 = 2
				camp3 = 3
			else
				camp2 = 3
				camp3 = 2
			end
		else
			camp1 = 3
			camp2 = 1
			camp3 = 2
		end
	else
		if value1 > value3 then
			camp1 = 2
			camp2 = 1
			camp3 = 3
		else
			if value2 > value3 then
				camp1 = 2
				camp2 = 3
				camp3 = 1
			else
				camp1 = 3 
				camp2 = 2
				camp3 = 1
			end
		end
	end
	return camp1, camp2, camp3
end


--@brief 判断招募令信息
function GasRecruitSql.GetCampInfoByNum()
	local tbl = {0,0,0}
	local res = StmtOperater.GetUserCampInfo:ExecStat()
	local num = res:GetRowNum()
	if num > 0 then
		for i = 0, num -1 do
			local camp = res:GetData(i,0)
			local count = res:GetData(i,1)
			tbl[camp] = count
		end
	end
	local camp1, camp2, camp3 = GetCampNum(tbl[1], tbl[2], tbl[3])
	local minNum, maxNum, time = 100,200,720
	local result = StmtOperater._GetRecruitInfo:ExecStat()
	if result:GetRowNum() > 0 then
		minNum = result:GetData(0,0)
		maxNum = result:GetData(0,1)
		time = result:GetData(0,2)
	end
	local campTbl = {0,0,0}
	if tbl[camp1] - tbl[camp2] >= minNum and tbl[camp1] - tbl[camp2] <= maxNum then
		campTbl[camp2] = 1
	elseif  tbl[camp1] - tbl[camp2] > maxNum then
		campTbl[camp2] = 2
	end
	if tbl[camp1] - tbl[camp3] >= minNum and tbl[camp1] - tbl[camp3] <= maxNum then
		campTbl[camp3] = 1
	elseif  tbl[camp1] - tbl[camp3] > maxNum then
		campTbl[camp3] = 2
	end
	return campTbl
end

--@brief 添加招募的人员信息
function GasRecruitSql.AddRecruitCharInfo(charId, recruitFlag)
	StmtOperater._AddRecruitCharInfo:ExecStat(charId, recruitFlag)
end

--@brief 设置阵营信息
function GasRecruitSql.SetGmRecruitInfo(data)
	local method = data["method"]
	local camp1 = data["camp1"]
	local camp2 = data["camp2"]
	local camp3 = data["camp3"]
	local camptbl = {0,0,0}
	if method == 1 then
		GasRecruitSql.UpdateRecruitTime()
		camptbl =  GasRecruitSql.GetCampInfoByNum()
	elseif method == 2 then
		StmtOperater._DelRecruitTime:ExecStat() --清理时间意味关闭自动招募
		camptbl = {camp1, camp2, camp3}
	else
		StmtOperater._DelRecruitTime:ExecStat() --清理时间意味关闭自动招募
	end
	
	for i, campState in pairs(camptbl) do
		StmtOperater._AddRecruitCampInfo:ExecStat(i, campState)
	end
	local setTime = GetRecruitSetInfo()
	return setTime
end

function GasRecruitSql.IsAutoRecruit(data)
	local res = StmtOperater._GetRecruitTime:ExecStat()
	local setTime = GetRecruitSetInfo()
	return res:GetRowNum() > 0, setTime
end

--获得各阵营的状态
function GasRecruitSql.GetRecruitCampInfo()
	local res = StmtOperater._GetRecruitCampInfo:ExecStat()
	if res:GetRowNum() <= 0 then
		for i = 1, 3 do
			StmtOperater._AddRecruitCampInfo:ExecStat(i, 0)
		end
		res = StmtOperater._GetRecruitCampInfo:ExecStat()
	end
	local tbl = {}
	local num = res:GetRowNum()
	for i = 1, num do
		table.insert(tbl, res:GetData(i-1, 1))
	end
	return tbl
end

function GasRecruitSql.UpdateRecruitInfo(data)
	local minNum = data["minNum"]
	local maxNum = data["maxNum"]
	local time = data["time"]
	local res = StmtOperater._GetRecruitInfo:ExecStat()
	local num = res:GetRowNum ()
	if num <= 0 then
		StmtOperater._InsertRecruitInfo:ExecStat(minNum, maxNum, time)
	else
		StmtOperater._updateRecruitInfo:ExecStat(minNum, maxNum, time)
	end
end

function GasRecruitSql.GetRecruitSetting()
	local campInfo = GasRecruitSql.GetRecruitCampInfo()
	local res = StmtOperater._GetRecruitInfo:ExecStat()
	local minNum, maxNum, time = 100,200,720
	if res:GetRowNum() > 0 then
		minNum, maxNum, time = res:GetData(0,0), res:GetData(0,1),res:GetData(0,2)
	end
	return campInfo, {minNum, maxNum, time}
end
----------------------------------------------------------------------------------
return GasRecruitSql
