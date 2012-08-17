
local SortCharInfoSql = class()

local SortCharInfoDB = CreateDbBox(...)
------------------------------------------------------------------------------------------
--@brief 排行榜的字典表
--[[
	["个人情况"]				= 1
	["等级排行"]				= 2
	["最佳进步榜"]				= 3
	["财富榜"]					= 4
	["功勋榜"]					= 5
	["死亡榜"]					= 6
	["变身榜"]					= 7
	["累计杀怪榜"]				= 8
	["本周杀怪榜"]				= 9
	["在线时间榜"]				= 10
	["玩法积分榜"]				= 11
	["佣兵团情况"]				= 12
	["大逃杀总参赛次数排行"]	= 13
	["杀人榜"] 					= 14
	["大逃杀总胜利次数榜"]		= 18
	["积分赛总参赛次数榜5v5"]	= 21
	["大逃杀总积分榜"]	= 22
	["战斗力评分榜"]	= 23
	["佣兵小队等级排行"] = 24
	["佣兵小队声望排行"] = 25
	["佣兵小队军资排行"] = 26
	["佣兵小队资金排行"] = 27
	["军资运输排行"] = 28
	["联赛胜利次数榜"] = 29
	["联赛杀人数量榜"] = 30
	["资源点争夺杀人榜"] = 31
	["资源点占领次数榜"] = 32
	["资源点争夺杀人榜"] = 31
	["资源点占领次数榜"] = 32
	["佣兵小队击杀领主次数榜"] = 33
	["玩家击杀领主次数榜"] = 34
	CampType： --4:全部;1:艾米;2修斯;3神圣
--]]
------------------------------------------------------------------------------------------
--@brief 根据传进来的字段名查询当前排行榜中的排名
--@param field_name:字段名
local function CurrentSortByParam(table_name,field_name,LastMinValue,field2,field3)
	local query_string = "select a.cs_uId,b."  .. field_name .. " from tbl_char a," .. table_name .. " b where a.cs_uId = b.cs_uId "
	if field2 and field3 then
		query_string = query_string .. " and b." .. field2 .. " = " .. field3
	end
	query_string = query_string .. " and " .. field_name .. " >= " .. LastMinValue .." order by b." .. field_name .. " desc limit 200 "
	local _, resObj = g_DbChannelMgr:TextExecute(query_string)--目前的排名
	return resObj
end
------------------------------------------------------------------------------------------
--@brief 根据传进来的表名和字段名查询原有排行榜中的排名
--@param table_name:要查询的表名
--@param order_field:名次字段
--@param updown_field:名次升降字段
--@param sort_field:要进行排行的字段名
local function OldSortByParam(table_name,order_field,updown_field,sort_field)
	local query_string = "select a." .. order_field .. ",a." .. updown_field .. ",b.c_sName,c.cs_uClass,a." .. sort_field .. " ,a.cs_uId from " ..  table_name .. " a,tbl_char b,tbl_char_static c where a.cs_uId = b.cs_uId and b.cs_uId = c.cs_uId "
	query_string = query_string .. " order by a." .. order_field
	local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)--原有排行榜中的排名
	return res_oldObj
end
------------------------------------------------------------------------------------------
--@brief 根据所传进来的字段按照玩家所在阵营的目前情况对其进行排行
--@param table_name:要查询的表名
--@param field_name:字段名
--@param campType:要查询的阵营Id
local function CurrentSortByCamp(table_name,field_name,campType,sort_order)
	local query_string = "select a.cs_uId,a." .. field_name .. " from " .. table_name .. " a,tbl_char_static b,tbl_char c where a.cs_uId = b.cs_uId and b.cs_uId = c.cs_uId and b.cs_uCamp = " .. campType
	query_string = query_string .. " order by a." .. sort_order
	local _, resObj = g_DbChannelMgr:TextExecute(query_string)--目前的排名
	return resObj
end
------------------------------------------------------------------------------------------
--@brief 根据所传进来的字段按照玩家所在阵营的原有的情况对其进行排行
--@param table_name:要查询的表名
--@param camptable_name:按照阵营排行的存储表
--@param camp_order_field:阵营排行表里面的名次字段
--@param camp_updown_field:阵营排行表里面的名次升降字段
--@param sort_field:要进行排行的字段名
--@param campType:要查询的阵营Id
local function OldSortByCamp(table_name,camptable_name,camp_order_field,camp_updown_field,sort_field,campType)
	local query_string = "select a." .. camp_order_field .. ",a." .. camp_updown_field .. ",c.c_sName,d.cs_uClass,b." .. sort_field .. ",a.cs_uId from " .. camptable_name .. " a," .. table_name .. " b,tbl_char c,tbl_char_static d where a.cs_uId = b.cs_uId and b.cs_uId = c.cs_uId and c.cs_uId = d.cs_uId and d.cs_uCamp = "  .. campType 
	query_string = query_string .. " order by a." .. camp_order_field
	local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)--原有排行榜中的排名
	return res_oldObj
end
------------------------------------------------------------------------------------------
--@brief 对副本里面的大逃杀或者积分赛相关进行排行
--@param field1:字段1
--@param field2:字段2
--@param field3:字段3
local function CurrentSortFBByParam(table_name,field1,field2,field3,jifenfield)
	local query_string = "select a.cs_uId,b." .. field1 .. " + b." .. field2 .. "+ b." .. field3 .. " as sum from tbl_char a," .. table_name .. " b where a.cs_uId = b.cs_uId "
	if jifenfield ~= 0 then
		query_string = query_string .. " and b." .. jifenfield .. " = 2" 
	end
	query_string = query_string .. " order by sum desc limit 200 "
	local _, resObj = g_DbChannelMgr:TextExecute(query_string)--目前的排名
	return resObj
end
------------------------------------------------------------------------------------------
--@brief 清空上次的排行情况
--@param tablename:要删除的表名
local function DeleteOldInfo(tablename,campType)
	local delete_string 
	if IsNumber(campType) then
		delete_string = "delete a from " .. tablename .. " as a,tbl_char_static as b where a.cs_uId = b.cs_uId and b.cs_uCamp = " .. campType
	else
		delete_string = "delete from " .. tablename
	end
	local _, res_oldObj = g_DbChannelMgr:TextExecute(delete_string)
end
------------------------------------------------------------------------------------------
local function GetLastMinValue(table_name,field_name)
	local query_str = "select ifnull(min(" .. field_name .. "),0) from " .. table_name
	local _,res = g_DbChannelMgr:TextExecute(query_str)
	return res:GetData(0,0)
end
------------------------------------------------------------------------------------------
local StmtDef=
{
	"_SortByMoney",
	[[
		select  cm.cs_uId, cm.cm_uMoney + ifnull(dm.dm_uMoney,0) as money from tbl_char_money as cm 
		left join tbl_depot_money as dm on(cm.cs_uId = dm.cs_uId) 
		left join tbl_char c on(cm.cs_uId = c.cs_uId) 
		where cm.cs_uId = c.cs_uId  order by money desc limit 200
	]]
}
DefineSql(StmtDef,SortCharInfoSql)

--@brief 按照传进来的参数在固定时间内排序
--@param table_name:要查询的表名
--@param order_field:每个表里面的名次字段
--@param updown_field:每个表里面的名次升降字段
--@param sort_field:要进行排行的字段名
--@param basic_sort_field:tbl_char_basic表里面的字段
--@param field2:有可能是大逃杀要计算总参赛次数三个字段中的一个，有可能是积分赛积分排行的类型1、2、3(2v2、3v3、5v5),视情况而定
local function CreateSortByParam(table_name,order_field,updown_field,sort_field,basic_table,basic_sort_field,field2,field3,jifenfield)
	local res_new_tbl ,res_new_set
	if field2 and field3 and jifenfield then
		res_new_set = CurrentSortFBByParam(basic_table,basic_sort_field,field2,field3,jifenfield)				--大逃杀总参赛次数目前的排名
	elseif basic_sort_field == "cm_uMoney" then
		res_new_set = SortCharInfoSql._SortByMoney:ExecStat()
	else
		local LastMinValue
		if basic_sort_field == "cfe_uPoint" then
			LastMinValue = 0
		else
			LastMinValue = GetLastMinValue(table_name,sort_field)
		end 
		res_new_set = CurrentSortByParam(basic_table,basic_sort_field,LastMinValue,field2,field3)								--目前的排名
	end
	
	if res_new_set:GetRowNum() == 0 then
		return
	end
	res_new_tbl = res_new_set:GetTableSet()
	
	local res_old_set = OldSortByParam(table_name,order_field,updown_field,sort_field) --原有排行榜中的排名
	local res_old_tbl = res_old_set:GetTableSet()
	
	local orderTbl = {}
	orderTbl[1] = {}
	orderTbl[1][1] = 1
	for i = 2, res_new_set:GetRowNum() do
		orderTbl[i] = {}
		orderTbl[i][1] = i --第五字段为order
		if res_new_tbl(i,2) == res_new_tbl(i-1,2) then
			orderTbl[i][1] = orderTbl[i-1][1]
		end
	end
	
	local playerinto = {}
	for i = 1,res_new_set:GetRowNum() do
		local bool = true
		for j = 1,res_old_set:GetRowNum() do
			if(res_new_tbl(i,1) == res_old_tbl(j,6)) then --判断玩家是不是在原有的排行榜里面出现过
				table.insert(playerinto,{res_new_tbl(i,1),res_old_tbl(j,1) - orderTbl[i][1]})
				bool = false
			end
		end
		if bool then
			table.insert(playerinto,{res_new_tbl(i,1),0})
		end
	end
	
	DeleteOldInfo(table_name)
	
	for i = 1,res_new_set:GetRowNum() do
		for j = 1,#playerinto do
			if res_new_tbl(i,1) == playerinto[j][1] then
				local query_string = "insert ignore into " ..  table_name .. " values(" .. orderTbl[i][1] .. "," .. playerinto[j][2] .. "," .. res_new_tbl(i,1) .. "," .. res_new_tbl(i,2) .. ")"
				local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)
				break
			end
		end
	end
end
------------------------------------------------------------------------------------------
--@brief 按照传进来的参数在固定时间内根据不同阵营进行排序
--@param table_name:要查询的表名
--@param camptable_name:按照阵营排行的存储表
--@param camp_order_field:阵营排行表里面的名次字段
--@param camp_updown_field:阵营排行表里面的名次升降字段
--@param sort_field:要进行排行的字段名
--@param campType:要查询的阵营Id
local function CreateSortByCamp(table_name,camptable_name,camp_order_field,camp_updown_field,sort_field,sort_order,campType)
	local res_new_set = CurrentSortByCamp(table_name,sort_field,campType,sort_order)			--目前的排名
	local res_old_set = OldSortByCamp(table_name,camptable_name,camp_order_field,camp_updown_field,sort_field,campType)		--原有排行榜中的排名
	
	if res_new_set:GetRowNum() == 0 then
		return
	end
	
	local res_new_tbl = res_new_set:GetTableSet()
	local res_old_tbl = res_old_set:GetTableSet() 
	local orderTbl = {}
	orderTbl[1] = {}
	orderTbl[1][1] = 1
	for i = 2, res_new_set:GetRowNum() do
		orderTbl[i] = {}
		orderTbl[i][1] = i --第五字段为order
		if res_new_tbl(i,2) == res_new_tbl(i-1,2) then
			orderTbl[i][1] = orderTbl[i-1][1]
		end
	end
	
	local playerinto = {}
	for i = 1,res_new_set:GetRowNum() do
		local bool = true 
		for j = 1,res_old_set:GetRowNum() do
			if(res_new_tbl(i,1) == res_old_tbl(j,6)) then --判断玩家是不是在原有的排行榜里面出现过
				table.insert(playerinto,{res_new_tbl(i,1),res_old_tbl(j,1)-orderTbl[i][1]})
				bool = false
			end
		end
		if bool then
			table.insert(playerinto,{res_new_tbl(i,1),0})
		end
	end
	
	DeleteOldInfo(camptable_name,campType)
	for i = 1,res_new_set:GetRowNum() do
		for j = 1,table.maxn(playerinto) do
			if res_new_tbl(i,1) == playerinto[j][1] then
				local query_string = "insert ignore into " ..  camptable_name .. " values(" .. orderTbl[i][1] .. "," .. playerinto[j][2] .. "," .. res_new_tbl(i,1) .. ")"
				local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)
				break
			end
		end
	end
end
------------------------------------------------------------------------------------------
local StmtDef=
{
	--根据玩家等级进行排序
	"SortByLevelAndExp",
	[[
	select
		a.cs_uId,
		b.cb_uLevel,
		c.cs_uLevelExp
	from
		tbl_char a,
		tbl_char_basic b,
		tbl_char_experience c
	where
		a.cs_uId = b.cs_uId and b.cs_uId = c.cs_uId and b.cb_uLevel >= ?
		order by b.cb_uLevel desc,c.cs_uLevelExp desc limit 200
	]]
}
DefineSql(StmtDef,SortCharInfoSql)

--@brief 根据玩家等级进行排序
local function SortByLevelAndExp(uLevel)
	local resObj = SortCharInfoSql.SortByLevelAndExp:ExecStat(uLevel)--目前的排名
	return resObj
end
--------------------------------------------------------------------------------------------
local StmtDef=
{
	--根据玩家等级进行排序
	"_SelectLastMinLevel",
	[[
		select ifnull(min(sl_uLevel),0) from tbl_sort_level;
	]]
}
DefineSql(StmtDef,SortCharInfoSql)

--@brief 根据上次等级榜最低的等级
local function GetLastMinLevel()
	local resObj = SortCharInfoSql._SelectLastMinLevel:ExecStat()
	return resObj:GetData(0,0)
end
--------------------------------------------------------------------------------------------
--@brief 按照等级排行
local function CreateSortByLevel()
	local last_low_level = GetLastMinLevel()
	local res_new_set = SortByLevelAndExp(last_low_level)				--目前的排名
	local res_old_set = OldSortByParam("tbl_sort_level","sl_uOrder","sl_uUpdown","sl_uLevel")	--原有排行榜中的排名
	if res_new_set:GetRowNum() == 0 then
		return
	end
	local res_new_tbl = res_new_set:GetTableSet()
	local res_old_tbl = res_old_set:GetTableSet()
	
	local orderTbl = {}
	orderTbl[1] = {}
	orderTbl[1][1] = 1
	for i = 2, res_new_set:GetRowNum() do
		orderTbl[i] = {}
		orderTbl[i][1] = i --第五字段为order
		if res_new_tbl(i,2) == res_new_tbl(i-1,2) and res_new_tbl(i,3) == res_new_tbl(i-1,3) then
			orderTbl[i][1] = orderTbl[i-1][1]
		end
	end
	
	local playerinto = {}
	for i = 1,res_new_set:GetRowNum() do
		local bool = true 
		for j = 1,res_old_set:GetRowNum() do
			if(res_new_tbl(i,1) == res_old_tbl(j,6)) then --判断玩家是不是在原有的排行榜里面出现过
				table.insert(playerinto,{res_new_tbl(i,1),res_old_tbl(j,1)-orderTbl[i][1]})
				bool = false
			end
		end
		if bool then
			table.insert(playerinto,{res_new_tbl(i,1),0})
		end
	end
	DeleteOldInfo("tbl_sort_level")
	for i = 1,res_new_set:GetRowNum() do
		for j = 1,#playerinto do
			if res_new_tbl(i,1) == playerinto[j][1] then
				local query_string = "insert ignore into tbl_sort_level values(" .. orderTbl[i][1] .. "," .. playerinto[j][2] .. "," .. res_new_tbl(i,1) .. "," .. res_new_tbl(i,2) .. "," .. res_new_tbl(i,3) .. ")"
				local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)
				break
			end
		end
	end
end
------------------------------------------------------------------------------------------
local StmtDef=
{
	--查询按照玩家所在阵营对等级进行排序
	"_SortLevelByCamp",
	[[
		select
			a.cs_uId,
			a.sl_uLevel,
			a.sl_uLevelExp
		from
			tbl_sort_level a,
			tbl_char_static b
		where
			a.cs_uId = b.cs_uId and b.cs_uCamp = ?
			order by sl_uOrder
	]]
}
DefineSql(StmtDef,SortCharInfoSql)

--@brief 按照玩家所在阵营对等级进行排行
--@param campType:要查询的阵营Id
local function SortLevelByCamp(campType)
	local resObj = SortCharInfoSql._SortLevelByCamp:ExecStat(campType)--目前的排名
	return resObj
end
------------------------------------------------------------------------------------------
--@brief 按照阵营对等级排行
--@param campType:要查询的阵营Id
local function CreateSortLevelByCamp(campType)
	local res_new_set = SortLevelByCamp(campType)--目前的排名
	local res_old_set = OldSortByCamp("tbl_sort_level","tbl_sort_level_by_camp","slbc_uOrder","slbc_uUpdown","sl_uLevel",campType)--原有排行榜中的排名
	if res_new_set:GetRowNum() == 0 then
		return
	end
	local res_new_tbl = res_new_set:GetTableSet()
	local res_old_tbl = res_old_set:GetTableSet()
	
	local orderTbl = {}
	orderTbl[1] = {}
	orderTbl[1][1] = 1
	for i = 2, res_new_set:GetRowNum() do
		orderTbl[i] = {}
		orderTbl[i][1] = i --第五字段为order
		if res_new_tbl(i,2) == res_new_tbl(i-1,2) and res_new_tbl(i,3) == res_new_tbl(i-1,3) then
			orderTbl[i][1] = orderTbl[i-1][1]
		end
	end
	
	local playerinto = {}
	for i = 1,res_new_set:GetRowNum() do
		local bool = true 
		for j = 1,res_old_set:GetRowNum() do
			if(res_new_tbl(i,1) == res_old_tbl(j,6)) then --判断玩家是不是在原有的排行榜里面出现过
				table.insert(playerinto,{res_new_tbl(i,1),res_old_tbl(j,1)-orderTbl[i][1]})
				bool = false 
			end
		end
		if bool then
			table.insert(playerinto,{res_new_tbl(i,1),0})
		end
	end

	DeleteOldInfo("tbl_sort_level_by_camp",campType)
	for i = 1,res_new_set:GetRowNum() do
		for j = 1,table.maxn(playerinto) do
			if res_new_tbl(i,1) == playerinto[j][1] then
				local query_string = "insert ignore into tbl_sort_level_by_camp values(" .. orderTbl[i][1] .. "," .. playerinto[j][2] .. "," .. res_new_tbl(i,1) .. ")"
				local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)
				break
			end
		end
	end
end
---------------------------------------[个人信息]---------------------------------------------------
--@brief 通过传进来的表名和字段名查看传进来的玩家角色Id的排行
--@param playerId:要查询的玩家Id
--@param table_name:要查询的表名
--@param order_field:名次字段
--@param updown_field:名次升降字段
--@param sort_field:要进行排行的字段名
local function PlayerSortByParam(playerId,table_name,order_field,updown_field,sort_field)
	local query_string = "select a." .. order_field .. ",a." .. updown_field .. ",c.cs_uClass,a." .. sort_field .. " from " .. table_name .. " a,tbl_char b,tbl_char_static c where a.cs_uId = b.cs_uId and b.cs_uId = c.cs_uId and a.cs_uId = " .. playerId
	local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)
	return res_oldObj
end
------------------------------------------------------------------------------------------------------
--@brief 个人情况查看
--@param playerId：要查看的玩家Id
local function PlayerSortInfo(playerId)
	local PlayerSortInfo = {}
	--玩家等级排行榜
	local PlayerSortByLevel = PlayerSortByParam(playerId,"tbl_sort_level","sl_uOrder","sl_uUpdown","sl_uLevel")
	--玩家不绑定金钱排行榜
	local PlayerSortByMoney = PlayerSortByParam(playerId,"tbl_sort_money","sm_uOrder","sm_uUpdown","sm_uMoney")
	--角色死亡次数排行榜
	local PlayerSortByDeadTimes = PlayerSortByParam(playerId,"tbl_sort_deadtimes","sd_uOrder","sd_uUpdown","sd_uDeadTimes")
	--角色杀怪兽个数排行榜
	local PlayerSortByKillNpcCount = PlayerSortByParam(playerId,"tbl_sort_kill_npc_count","sknc_uOrder","sknc_uUpdown","sknc_uKillNpcCount")
	--角色杀人个数排行榜
	local PlayerSortByKillPlayerCount = PlayerSortByParam(playerId,"tbl_sort_kill_player_count","skpc_uOrder","skpc_uUpdown","skpc_uKillPlayerCount")
	--角色在线时间累积排行榜
	local PlayerSortOnTimeTotal = PlayerSortByParam(playerId,"tbl_sort_player_onlinetime","spo_uOrder","spo_uUpdown","spo_uPlayerOnTimeTotal")
	--大逃杀总参赛次数排行
	local PlayerSortDaTaoJoinTimes = PlayerSortByParam(playerId,"tbl_sort_dataosha_all_jointimes","sdaj_uOrder","sdaj_uUpdown","sdaj_uJoinTimes")
	--大逃杀总胜利次数排行
	local PlayerSortDaTaoWinTimes = PlayerSortByParam(playerId,"tbl_sort_dataosha_all_wintimes","sdaw_uOrder","sdaw_uUpdown","sdaw_uWinTimes")
	--战斗力排行榜
	local PlayerFightEvaluation = PlayerSortByParam(playerId,"tbl_sort_fight_evaluation","sfe_uOrder","sfe_uUpdown","sfe_uPoint")	
	
	--大逃杀总积分排行
	local PlayerSortDaTaoShaPoint = PlayerSortByParam(playerId,"tbl_sort_dataosha_point","sdp_uOrder","sdp_uUpdown","sdp_uPoint")
	--玩家等级排行榜
	if nil ~= PlayerSortByLevel and PlayerSortByLevel:GetRowNum() > 0 then
		for i = 1,PlayerSortByLevel:GetRowNum() do
			table.insert(PlayerSortInfo,{PlayerSortByLevel:GetData(i-1,0),PlayerSortByLevel:GetData(i-1,1),"等级",PlayerSortByLevel:GetData(i-1,2),PlayerSortByLevel:GetData(i-1,3)})
		end
	end
	--玩家不绑定金钱排行榜
	if nil ~= PlayerSortByMoney and PlayerSortByMoney:GetRowNum() > 0 then
		for i = 1,PlayerSortByMoney:GetRowNum() do
			table.insert(PlayerSortInfo,{PlayerSortByMoney:GetData(i-1,0),PlayerSortByMoney:GetData(i-1,1),"财富",PlayerSortByMoney:GetData(i-1,2),PlayerSortByMoney:GetData(i-1,3)})
		end
	end
	--玩家死亡次数排行榜
	if nil ~= PlayerSortByDeadTimes and PlayerSortByDeadTimes:GetRowNum() > 0 then
		for i = 1,PlayerSortByDeadTimes:GetRowNum() do
			table.insert(PlayerSortInfo,{PlayerSortByDeadTimes:GetData(i-1,0),PlayerSortByDeadTimes:GetData(i-1,1),"死亡",PlayerSortByDeadTimes:GetData(i-1,2),PlayerSortByDeadTimes:GetData(i-1,3)})
		end
	end
	--玩家杀怪排行榜
	if nil ~= PlayerSortByKillNpcCount and PlayerSortByKillNpcCount:GetRowNum() > 0 then
		for i = 1,PlayerSortByKillNpcCount:GetRowNum() do
			table.insert(PlayerSortInfo,{PlayerSortByKillNpcCount:GetData(i-1,0),PlayerSortByKillNpcCount:GetData(i-1,1),"累计杀怪",PlayerSortByKillNpcCount:GetData(i-1,2),PlayerSortByKillNpcCount:GetData(i-1,3)})
		end
	end
	--玩家杀人排行榜
	if nil ~= PlayerSortByKillPlayerCount and PlayerSortByKillPlayerCount:GetRowNum() > 0 then
		for i = 1,PlayerSortByKillPlayerCount:GetRowNum() do
			table.insert(PlayerSortInfo,{PlayerSortByKillPlayerCount:GetData(i-1,0),PlayerSortByKillPlayerCount:GetData(i-1,1),"杀人",PlayerSortByKillPlayerCount:GetData(i-1,2),PlayerSortByKillPlayerCount:GetData(i-1,3)})
		end
	end
	--玩家在线时间累积排行榜
	if nil ~= PlayerSortOnTimeTotal and PlayerSortOnTimeTotal:GetRowNum() > 0 then
		for i = 1,PlayerSortOnTimeTotal:GetRowNum() do
			table.insert(PlayerSortInfo, {PlayerSortOnTimeTotal:GetData(i-1,0),PlayerSortOnTimeTotal:GetData(i-1,1),"在线时间",PlayerSortOnTimeTotal:GetData(i-1,2),PlayerSortOnTimeTotal:GetData(i-1,3)})
		end
	end
	--大逃杀积分排行
	if nil ~= PlayerSortDaTaoShaPoint and PlayerSortDaTaoShaPoint:GetRowNum() > 0 then
		for i = 1,PlayerSortDaTaoShaPoint:GetRowNum() do
			table.insert(PlayerSortInfo, {PlayerSortDaTaoShaPoint:GetData(i-1,0),PlayerSortDaTaoShaPoint:GetData(i-1,1),"角斗场总积分",PlayerSortDaTaoShaPoint:GetData(i-1,2),PlayerSortDaTaoShaPoint:GetData(i-1,3)})
		end
	end
	--大逃杀总参赛次数排行
	if nil ~= PlayerSortDaTaoJoinTimes and PlayerSortDaTaoJoinTimes:GetRowNum() > 0 then
		for i = 1,PlayerSortDaTaoJoinTimes:GetRowNum() do
			table.insert(PlayerSortInfo, {PlayerSortDaTaoJoinTimes:GetData(i-1,0),PlayerSortDaTaoJoinTimes:GetData(i-1,1),"角斗场总参赛次数",PlayerSortDaTaoJoinTimes:GetData(i-1,2),PlayerSortDaTaoJoinTimes:GetData(i-1,3)})
		end
	end
	--大逃杀总胜利次数排行
	if nil ~= PlayerSortDaTaoWinTimes and PlayerSortDaTaoWinTimes:GetRowNum() > 0 then
		for i = 1,PlayerSortDaTaoWinTimes:GetRowNum() do
			table.insert(PlayerSortInfo, {PlayerSortDaTaoWinTimes:GetData(i-1,0),PlayerSortDaTaoWinTimes:GetData(i-1,1),"角斗场总胜利次数",PlayerSortDaTaoWinTimes:GetData(i-1,2),PlayerSortDaTaoWinTimes:GetData(i-1,3)})
		end
	end
	
	--战斗力排行
	if nil ~= PlayerFightEvaluation and PlayerFightEvaluation:GetRowNum() > 0 then
		for i = 1,PlayerFightEvaluation:GetRowNum() do
			table.insert(PlayerSortInfo, {PlayerFightEvaluation:GetData(i-1,0),PlayerFightEvaluation:GetData(i-1,1),"战斗力总评分",PlayerFightEvaluation:GetData(i-1,2),string.format("%.2f",PlayerFightEvaluation:GetData(i-1,3))})
		end
	end
	
	return PlayerSortInfo
end
------------------------------------------------------------------------------------------
--@brief 根据客户端传过来的排行类型与等级类型进行排序
function SortCharInfoDB.GetSortList(data)
	local sType = data["sType"]
	local sName = data["sName"]
	local campType = data["campType"]
	local playerId = data["playerId"]
	local SortTongInfoDB = RequireDbBox("SortTongInfoDB")
	
	if("个人" == sType) then
		if("个人情况" == sName) then
			return PlayerSortInfo(playerId)
		elseif("等级榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_level","sl_uOrder","sl_uUpdown","sl_uLevel")
			else
				return OldSortByCamp("tbl_sort_level","tbl_sort_level_by_camp","slbc_uOrder","slbc_uUpdown","sl_uLevel",campType)
			end
		elseif("财富榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_money","sm_uOrder","sm_uUpdown","sm_uMoney")
			else
				return OldSortByCamp("tbl_sort_money","tbl_sort_money_by_camp","smbc_uOrder","smbc_uUpdown","sm_uMoney",campType)
			end
		elseif("死亡榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_deadtimes","sd_uOrder","sd_uUpdown","sd_uDeadTimes")
			else
				return OldSortByCamp("tbl_sort_deadtimes","tbl_sort_deadtimes_by_camp","sdbc_uOrder","sdbc_uUpdown","sd_uDeadTimes",campType)
			end
		elseif("杀人榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_kill_player_count","skpc_uOrder","skpc_uUpdown","skpc_uKillPlayerCount")
			else
				return OldSortByCamp("tbl_sort_kill_player_count","tbl_sort_kill_player_count_by_camp","skpcbc_uOrder","skpcbc_uUpdown","skpc_uKillPlayerCount",campType)
			end
		elseif("累计杀怪榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_kill_npc_count","sknc_uOrder","sknc_uUpdown","sknc_uKillNpcCount")
			else
				return OldSortByCamp("tbl_sort_kill_npc_count","tbl_sort_kill_npc_count_by_camp","skncbc_uOrder","skncbc_uUpdown","sknc_uKillNpcCount",campType)
			end
		elseif("在线时间榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_player_onlinetime","spo_uOrder","spo_uUpdown","spo_uPlayerOnTimeTotal")
			else
				return OldSortByCamp("tbl_sort_player_onlinetime","tbl_sort_player_onlinetime_by_camp","spobc_uOrder","spobc_uUpdown","spo_uPlayerOnTimeTotal",campType)
			end
		elseif("战斗力评分榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_fight_evaluation","sfe_uOrder","sfe_uUpdown","sfe_uPoint")
			else
				return OldSortByCamp("tbl_sort_fight_evaluation","tbl_sort_fight_evaluation_by_camp","sfebc_uOrder","sfebc_uUpdown","sfe_uPoint",campType)
			end
		elseif("军资运输榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_char_resource","scr_uOrder","scr_uUpdown","scr_uResource")
			else
				return OldSortByCamp("tbl_sort_char_resource","tbl_sort_char_resource_by_camp","scrbc_uOrder","scrbc_uUpdown","scr_uResource",campType)
			end
		elseif("资源点争夺杀人榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_resource_kill_num","srkn_uOrder","srkn_uUpdown","srkn_uKillNum")
			else
				return OldSortByCamp("tbl_sort_resource_kill_num","tbl_sort_resource_kill_num_by_camp","srknbc_uOrder","srknbc_uUpdown","srkn_uKillNum",campType)
			end
		elseif("玩家击杀领主次数榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_char_kill_boss_num","sckbn_uOrder","sckbn_uUpdown","sckbn_uKillNum")
			else
				return OldSortByCamp("tbl_sort_char_kill_boss_num","tbl_sort_char_kill_boss_num_by_camp","sckbnbc_uOrder","sckbnbc_uUpdown","sckbn_uKillNum",campType)
			end
		end
	elseif("玩法" == sType) then
		if("联赛总参赛次数榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_jifensai_join_times","sjjt_uOrder","sjjt_uUpdown","sjjt_uJoinTimes")
			else
				return OldSortByCamp("tbl_sort_jifensai_join_times","tbl_sort_jifensai_join_times_by_camp","sjjtbc_uOrder","sjjtbc_uUpdown","sjjt_uJoinTimes",campType)
			end
		elseif("联赛总胜利次数榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_jifensai_wintimes","sjw_uOrder","sjw_uUpdown","sjw_uWinTimes")
			else
				return OldSortByCamp("tbl_sort_jifensai_wintimes","tbl_sort_jifensai_wintimes_by_camp","sjwbc_uOrder","sjwbc_uUpdown","sjw_uWinTimes",campType)
			end
		elseif("联赛杀人数量榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_tong_kill_num","stkn_uOrder","stkn_uUpdown","stkn_uKillNum")
			else
				return OldSortByCamp("tbl_sort_tong_kill_num","tbl_sort_tong_kill_num_by_camp","stknbc_uOrder","stknbc_uUpdown","stkn_uKillNum",campType)
			end
		elseif("大逃杀总参赛次数榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_dataosha_all_jointimes","sdaj_uOrder","sdaj_uUpdown","sdaj_uJoinTimes")
			else
				return OldSortByCamp("tbl_sort_dataosha_all_jointimes","tbl_sort_dataosha_all_jointimes_by_camp","sdajbc_uOrder","sdajbc_uUpdown","sdaj_uJoinTimes",campType) 
			end
		elseif("大逃杀总胜利次数榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_dataosha_all_wintimes","sdaw_uOrder","sdaw_uUpdown","sdaw_uWinTimes")
			else
				return OldSortByCamp("tbl_sort_dataosha_all_wintimes","tbl_sort_dataosha_all_wintimes_by_camp","sdawbc_uOrder","sdawbc_uUpdown","sdaw_uWinTimes",campType)
			end
		elseif("大逃杀总积分榜" == sName) then
			if(4 == campType) then
				return OldSortByParam("tbl_sort_dataosha_point","sdp_uOrder","sdp_uUpdown","sdp_uPoint")
			else
				return OldSortByCamp("tbl_sort_dataosha_point","tbl_sort_dataosha_point_by_camp","sdpbc_uOrder","sdpbc_uUpdown","sdp_uPoint",campType)
			end
		end
	elseif("佣兵小队" == sType) then
		if("佣兵小队等级榜" == sName) then
			if(4 == campType) then
				return SortTongInfoDB.OldSortTongByParam("tbl_sort_tong_level","stl_uOrder","stl_uUpdown","stl_uLevel")
			else
				return SortTongInfoDB.OldTongSortByCamp("tbl_sort_tong_level","tbl_sort_tong_level_by_camp","stlbc_uOrder","stlbc_uUpdown","stl_uLevel",campType)
			end
		elseif("佣兵小队声望榜" == sName) then
			if(4 == campType) then
				return SortTongInfoDB.OldSortTongByParam("tbl_sort_tong_exploit","ste_uOrder","ste_uUpdown","ste_uExploit")
			else
				return SortTongInfoDB.OldTongSortByCamp("tbl_sort_tong_exploit","tbl_sort_tong_exploit_by_camp","stebc_uOrder","stebc_uUpdown","ste_uExploit",campType)
			end
		elseif("佣兵小队军资榜" == sName) then
			if(4 == campType) then
				return SortTongInfoDB.OldSortTongByParam("tbl_sort_tong_resource","str_uOrder","str_uUpdown","str_uResource")
			else
				return SortTongInfoDB.OldTongSortByCamp("tbl_sort_tong_resource","tbl_sort_tong_resource_by_camp","strbc_uOrder","strbc_uUpdown","str_uResource",campType)
			end
		elseif("佣兵小队资金榜" == sName) then
			if(4 == campType) then
				return SortTongInfoDB.OldSortTongByParam("tbl_sort_tong_money","stm_uOrder","stm_uUpdown","stm_uMoney")
			else
				return SortTongInfoDB.OldTongSortByCamp("tbl_sort_tong_money","tbl_sort_tong_money_by_camp","stmbc_uOrder","stmbc_uUpdown","stm_uMoney",campType)	
			end
		elseif("佣兵小队击杀领主次数榜" == sName) then
			if(4 == campType) then
				return SortTongInfoDB.OldSortTongByParam("tbl_sort_tong_kill_boss_num","stkbn_uOrder","stkbn_uUpdown","stkbn_uKillNum")
			else
				return SortTongInfoDB.OldTongSortByCamp("tbl_sort_tong_kill_boss_num","tbl_sort_tong_kill_boss_num_by_camp","stkbnbc_uOrder","stkbnbc_uUpdown","stkbn_uKillNum",campType)
			end
		elseif("资源点占领次数榜" == sName) then
			if(4 == campType) then
				return SortTongInfoDB.OldSortTongByParam("tbl_sort_tong_resource_occupy_times","strot_uOrder","strot_uUpdown","strot_uOccupyTimes")
			else
				return SortTongInfoDB.OldTongSortByCamp("tbl_sort_tong_resource_occupy_times","tbl_sort_tong_resource_occupy_times_by_camp","strotbc_uOrder","strotbc_uUpdown","strot_uOccupyTimes",campType)
			end
		end
	end
end
------------------------------------------------------------------------------------------
function SortCharInfoDB.CreateSortCharInfoFunc1(data)
	--创建等级排行
	CreateSortByLevel()
	--创建以不绑定金钱排行
	CreateSortByParam("tbl_sort_money","sm_uOrder","sm_uUpdown","sm_uMoney","tbl_char_money","cm_uMoney")
	--创建角色死亡次数排行榜
	CreateSortByParam("tbl_sort_deadtimes","sd_uOrder","sd_uUpdown","sd_uDeadTimes","tbl_char_fight_info","cfi_uDeadTimes")
	--创建杀怪兽排行榜
	CreateSortByParam("tbl_sort_kill_npc_count","sknc_uOrder","sknc_uUpdown","sknc_uKillNpcCount","tbl_char_fight_info","cfi_uKillNpcCount")
end
------------------------------------------------------------------------------------------
function SortCharInfoDB.CreateSortCharInfoFunc2(data)
	--创建杀人排行榜
	CreateSortByParam("tbl_sort_kill_player_count","skpc_uOrder","skpc_uUpdown","skpc_uKillPlayerCount","tbl_char_fight_info","cfi_uKillPlayerCount")
	--创建角色在线时间累积排行榜
	CreateSortByParam("tbl_sort_player_onlinetime","spo_uOrder","spo_uUpdown","spo_uPlayerOnTimeTotal","tbl_char_fight_info","cfi_uPlayerOnTimeTotal")
	--创建角斗场总参赛次数榜排行榜
	CreateSortByParam("tbl_sort_dataosha_all_jointimes","sdaj_uOrder","sdaj_uUpdown","sdaj_uJoinTimes","tbl_char_dataoshapoint","cs_uWinNum","cs_uLoseNum","cs_uRunNum",0)
	--创建角斗场总胜利次数排行榜
	CreateSortByParam("tbl_sort_dataosha_all_wintimes","sdaw_uOrder","sdaw_uUpdown","sdaw_uWinTimes","tbl_char_dataoshapoint","cs_uWinNum")
end
------------------------------------------------------------------------------------------
function SortCharInfoDB.CreateSortCharInfoFunc3(data)
	--创建角斗场积分排行榜
	CreateSortByParam("tbl_sort_dataosha_point","sdp_uOrder","sdp_uUpdown","sdp_uPoint","tbl_char_dataoshapoint","cs_udataoshapoint")
	--创建战斗力排行榜
	CreateSortByParam("tbl_sort_fight_evaluation","sfe_uOrder","sfe_uUpdown","sfe_uPoint","tbl_char_fighting_evaluation","cfe_uPoint")	
	--玩家联赛杀人数量榜
	CreateSortByParam("tbl_sort_tong_kill_num","stkn_uOrder","stkn_uUpdown","stkn_uKillNum","tbl_tong_jfs","tj_uNum")
	--玩家军资运输排行榜
	CreateSortByParam("tbl_sort_char_resource","scr_uOrder","scr_uUpdown","scr_uResource","tbl_tong_char_other","tco_uNum","tco_uTypeId",1)
end
------------------------------------------------------------------------------------------
function SortCharInfoDB.CreateSortCharInfoFunc4(data)
	--帮会联赛的总参赛次数排行榜
	CreateSortByParam("tbl_sort_jifensai_join_times","sjjt_uOrder","sjjt_uUpdown","sjjt_uJoinTimes","tbl_char_jifensaipoint","cs_uWinNum","cs_uLoseNum","cs_uRunNum","cs_uType")
	--玩家联赛胜利次数榜
	CreateSortByParam("tbl_sort_jifensai_wintimes","sjw_uOrder","sjw_uUpdown","sjw_uWinTimes","tbl_char_jifensaipoint","cs_uWinNum","cs_uType",2)
	--玩家资源争夺杀人排行榜
	CreateSortByParam("tbl_sort_resource_kill_num","srkn_uOrder","srkn_uUpdown","srkn_uKillNum","tbl_tong_char_other","tco_uNum","tco_uTypeId",2)
	--玩家击杀领主次数排行榜
	CreateSortByParam("tbl_sort_char_kill_boss_num","sckbn_uOrder","sckbn_uUpdown","sckbn_uKillNum","tbl_tong_char_boss","tcb_uNum")
end
------------------------------------------------------------------------------------------
function SortCharInfoDB.CreateSortCharInfoFunc5(data)
	local uCamp = data["uCamp"]
	--创建阵营为uCamp的等级排行
	CreateSortLevelByCamp(uCamp)
	--创建阵营为uCamp的不绑定金钱排行榜
	CreateSortByCamp("tbl_sort_money","tbl_sort_money_by_camp","smbc_uOrder","smbc_uUpdown","sm_uMoney","sm_uOrder",uCamp) 
	--创建阵营为uCamp的角色死亡次数排行
	CreateSortByCamp("tbl_sort_deadtimes","tbl_sort_deadtimes_by_camp","sdbc_uOrder","sdbc_uUpdown","sd_uDeadTimes","sd_uOrder",uCamp)
	--创建阵营为uCamp的杀怪兽榜
	CreateSortByCamp("tbl_sort_kill_npc_count","tbl_sort_kill_npc_count_by_camp","skncbc_uOrder","skncbc_uUpdown","sknc_uKillNpcCount","sknc_uOrder",uCamp)
	--创建阵营为uCamp的杀人榜
	CreateSortByCamp("tbl_sort_kill_player_count","tbl_sort_kill_player_count_by_camp","skpcbc_uOrder","skpcbc_uUpdown","skpc_uKillPlayerCount","skpc_uOrder",uCamp)
	--创建阵营为uCamp的角色在线时间累积排行榜
	CreateSortByCamp("tbl_sort_player_onlinetime","tbl_sort_player_onlinetime_by_camp","spobc_uOrder","spobc_uUpdown","spo_uPlayerOnTimeTotal","spo_uOrder",uCamp)
end
------------------------------------------------------------------------------------------
function SortCharInfoDB.CreateSortCharInfoFunc6(data)
	local uCamp = data["uCamp"]
	--创建阵营为uCamp的角斗场总参赛次数榜排行榜
	CreateSortByCamp("tbl_sort_dataosha_all_jointimes","tbl_sort_dataosha_all_jointimes_by_camp","sdajbc_uOrder","sdajbc_uUpdown","sdaj_uJoinTimes","sdaj_uOrder",uCamp)
	--创建阵营为uCamp的角斗场总胜利次数榜排行榜
	CreateSortByCamp("tbl_sort_dataosha_all_wintimes","tbl_sort_dataosha_all_wintimes_by_camp","sdawbc_uOrder","sdawbc_uUpdown","sdaw_uWinTimes","sdaw_uOrder",uCamp)		
	--创建阵营为uCamp积分类型为3v3的帮会联赛的总参赛次数排行榜
	CreateSortByCamp("tbl_sort_jifensai_join_times","tbl_sort_jifensai_join_times_by_camp","sjjtbc_uOrder","sjjtbc_uUpdown","sjjt_uJoinTimes","sjjt_uOrder",uCamp)
	--创建阵营为uCamp的角斗场积分榜排行榜
	CreateSortByCamp("tbl_sort_dataosha_point","tbl_sort_dataosha_point_by_camp","sdpbc_uOrder","sdpbc_uUpdown","sdp_uPoint","sdp_uOrder",uCamp)		
	--创建阵营为uCamp的战斗力榜排行榜
	CreateSortByCamp("tbl_sort_fight_evaluation","tbl_sort_fight_evaluation_by_camp","sfebc_uOrder","sfebc_uUpdown","sfe_uPoint","sfe_uOrder",uCamp)	
	--创建阵营为uCamp的军资运输排行榜
	CreateSortByCamp("tbl_sort_char_resource","tbl_sort_char_resource_by_camp","scrbc_uOrder","scrbc_uUpdown","scr_uResource","scr_uOrder",uCamp)	
	--创建阵营为uCamp的联赛胜利次数排行榜
	CreateSortByCamp("tbl_sort_jifensai_wintimes","tbl_sort_jifensai_wintimes_by_camp","sjwbc_uOrder","sjwbc_uUpdown","sjw_uWinTimes","sjw_uOrder",uCamp)	
	--创建阵营为uCamp的联赛杀人数量排行榜
	CreateSortByCamp("tbl_sort_tong_kill_num","tbl_sort_tong_kill_num_by_camp","stknbc_uOrder","stknbc_uUpdown","stkn_uKillNum","stkn_uOrder",uCamp)	
	--创建阵营为uCamp的资源争夺杀人排行榜
	CreateSortByCamp("tbl_sort_resource_kill_num","tbl_sort_resource_kill_num_by_camp","srknbc_uOrder","srknbc_uUpdown","srkn_uKillNum","srkn_uOrder",uCamp)	
	--创建阵营为uCamp的击杀领主次数排行榜
	CreateSortByCamp("tbl_sort_char_kill_boss_num","tbl_sort_char_kill_boss_num_by_camp","sckbnbc_uOrder","sckbnbc_uUpdown","sckbn_uKillNum","sckbn_uOrder",uCamp)	
end


SetDbLocalFuncType(SortCharInfoDB.GetSortList)

return SortCharInfoDB


