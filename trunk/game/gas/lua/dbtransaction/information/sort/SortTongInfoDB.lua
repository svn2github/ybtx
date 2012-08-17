
local SortTongInfoSql = class()

local SortTongInfoDB = CreateDbBox(...)
------------------------------------------------------------------------------------------
--@brief 根据传进来的字段名查询当前排行榜中的排名
--@param field_name:字段名
local function CurrentSortTongByField(tbl_name,field_name)
	local query_string = "select t_uId,"  .. field_name .. " from " .. tbl_name .. " order by " .. field_name .. " desc limit 200 "
	local _, resObj = g_DbChannelMgr:TextExecute(query_string)--目前的排名
	return resObj
end
------------------------------------------------------------------------------------------
--@brief 根据传进来的表名和字段名查询原有排行榜中的排名
--@param table_name:要查询的表名
--@param order_field:名次字段
--@param updown_field:名次升降字段
--@param sort_field:要进行排行的字段名
function SortTongInfoDB.OldSortTongByParam(table_name,order_field,updown_field,sort_field)
	local query_string = "select a." .. order_field .. ",a." .. updown_field .. ",b.t_sName,b.t_uType,a." .. sort_field .. " ,a.t_uId from " ..  table_name .. " a,tbl_tong b where a.t_uId = b.t_uId order by a." .. order_field
	local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)--原有排行榜中的排名
	return res_oldObj
end
------------------------------------------------------------------------------------------
local StmtDef=
{
	--根据帮会声望排行
	"_SortByTongExploit",
	[[
		select 
			mt.t_uId ,sum(te.te_uExploit) as num 
		from tbl_tong_exploit te 
		join tbl_member_tong mt on 
		te.cs_uId = mt.cs_uId group by mt.t_uId  order by num desc limit 200;
	]]
}
DefineSql(StmtDef,SortTongInfoSql)

--@brief 根据玩家等级进行排序
local function SortByTongExploit()
	local resObj = SortTongInfoSql._SortByTongExploit:ExecStat()--目前的排名
	return resObj
end
------------------------------------------------------------------------------------------
--@brief 根据所传进来的字段按照玩家所在阵营的目前情况对其进行排行
--@param table_name:要查询的表名
--@param field_name:字段名
--@param campType:要查询的阵营Id
local function CurrentTongSortByCamp(table_name,field_name,campType,sort_order)
	local query_string = "select a.t_uId,a." .. field_name .. " from " .. table_name .. " a,tbl_tong b where a.t_uId = b.t_uId and b.t_uCamp = " .. campType .. " order by a." .. sort_order
	local _, resObj = g_DbChannelMgr:TextExecute(query_string)--目前的排名
	return resObj
end
------------------------------------------------------------------------------------------
function SortTongInfoDB.OldTongSortByCamp(table_name,camptable_name,camp_order_field,camp_updown_field,sort_field,campType)
	local query_string = "select a." .. camp_order_field .. ",a." .. camp_updown_field .. ",c.t_sName,c.t_uType,b." .. sort_field .. ",a.t_uId from " .. camptable_name .. " a," .. table_name .. " b,tbl_tong c where a.t_uId = b.t_uId and b.t_uId = c.t_uId and c.t_uCamp = "  .. campType .. " order by a." .. camp_order_field
	local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)--原有排行榜中的排名
	return res_oldObj
end
------------------------------------------------------------------------------------------
--@brief 清空上次的排行情况
--@param tablename:要删除的表名
local function DeleteOldInfo(tablename,campType)
	local delete_string 
	if IsNumber(campType) then
		delete_string = "delete a from " .. tablename .. " as a,tbl_tong as b where a.t_uId = b.t_uId and b.t_uCamp = " .. campType
	else
		delete_string = "delete from " .. tablename
	end
	local _, res_oldObj = g_DbChannelMgr:TextExecute(delete_string)
end
------------------------------------------------------------------------------------------
--@brief 按照传进来的参数在固定时间内排序
--@param table_name:要查询的表名
--@param order_field:每个表里面的名次字段
--@param updown_field:每个表里面的名次升降字段
--@param sort_field:要进行排行的字段名
--@param basic_sort_field:基础表里面的字段
local function CreateTongSortByParam(table_name,order_field,updown_field,sort_field,basic_table,basic_sort_field)
	local res_new_set = nil
	if basic_table == "tbl_tong_exploit" then
		 res_new_set = SortByTongExploit()								--目前的排名
	else
		 res_new_set = CurrentSortTongByField(basic_table,basic_sort_field)								--目前的排名
	end

	if res_new_set:GetRowNum() == 0 then
		return
	end

	local res_new_tbl = res_new_set:GetTableSet()
	local res_old_set = OldSortTongByParam(table_name,order_field,updown_field,sort_field) --原有排行榜中的排名
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
local function CreateTongSortByCamp(table_name,camptable_name,camp_order_field,camp_updown_field,sort_field,sort_order,campType)
	local res_new_set = CurrentTongSortByCamp(table_name,sort_field,campType,sort_order)			--目前的排名
	local res_old_set = OldTongSortByCamp(table_name,camptable_name,camp_order_field,camp_updown_field,sort_field,campType)		--原有排行榜中的排名
	
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
	--根据帮会等级及荣誉排行
	"_SortByTongLevelAndHonor",
	[[
	select
		t_uId,
		t_uLevel,
		t_uHonor
	from
		tbl_tong
	order by t_uLevel desc,t_uHonor desc limit 200
	]]
}
DefineSql(StmtDef,SortTongInfoSql)

--@brief 根据玩家等级进行排序
local function SortByTongLevelAndExp()
	local resObj = SortTongInfoSql._SortByTongLevelAndHonor:ExecStat()--目前的排名
	return resObj
end
--------------------------------------------------------------------------------------------
local StmtDef=
{
	"_SelectOldSortTong",
	[[
		select 
			stl_uOrder,stl_uUpdown,t.t_sName,t.t_uType,stl.stl_uLevel,stl.t_uId
		from 
			tbl_sort_tong_level stl,tbl_tong t
		where
			stl.t_uId = t.t_uId
	]]
}
DefineSql(StmtDef,SortTongInfoSql)

local function OldSortTong()
	local resObj = SortTongInfoSql._SelectOldSortTong:ExecStat()
	return resObj
end
--------------------------------------------------------------------------------------------
--@brief 按照等级排行
local function CreateSortByLevel(tbl_name)
	local res_new_set = SortByTongLevelAndExp()
	local res_old_set = OldSortTong()

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
	DeleteOldInfo("tbl_sort_tong_level")
	for i = 1,res_new_set:GetRowNum() do
		for j = 1,#playerinto do
			if res_new_tbl(i,1) == playerinto[j][1] then
				local query_string = "insert ignore into " .. tbl_name .. " values(" .. orderTbl[i][1] .. "," .. playerinto[j][2] .. "," .. res_new_tbl(i,1) .. "," .. res_new_tbl(i,2) .. "," .. res_new_tbl(i,3) .. ")"
				local _, res_oldObj = g_DbChannelMgr:TextExecute(query_string)
				break
			end
		end
	end
end
------------------------------------------------------------------------------------------
--@brief 按照阵营对等级排行
--@param campType:要查询的阵营Id
local function CreateSortLevelByCamp(campType)
	local res_new_set = SortLevelByCamp(campType)--目前的排名
	local res_old_set = OldTongSortByCamp("tbl_sort_level","tbl_sort_level_by_camp","slbc_uOrder","slbc_uUpdown","sl_uLevel","stl_uOrder",campType)--原有排行榜中的排名
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
------------------------------------------------------------------------------------------
function SortTongInfoDB.CreateSortTbl1(data)
	--创建佣兵小队等级排行榜
	CreateSortByLevel("tbl_sort_tong_level")
	--创建佣兵小队声望排行榜
	CreateTongSortByParam("tbl_sort_tong_exploit","ste_uOrder","ste_uUpdown","ste_uExploit","tbl_tong_exploit","te_uExploit")
	--创建佣兵小队军资排行榜
	CreateTongSortByParam("tbl_sort_tong_resource","str_uOrder","str_uUpdown","str_uResource","tbl_tong","t_uResource")
	--创建佣兵小队资金排行榜
	CreateTongSortByParam("tbl_sort_tong_money","stm_uOrder","stm_uUpdown","stm_uMoney","tbl_tong","t_uMoney")
	--创建佣兵小队资源点占领次数排行榜
	CreateTongSortByParam("tbl_sort_tong_resource_occupy_times","strot_uOrder","strot_uUpdown","strot_uOccupyTimes","tbl_tong_other","to_uNum")
	--创建佣兵小队击杀领主次数排行榜
	CreateTongSortByParam("tbl_sort_tong_kill_boss_num","stkbn_uOrder","stkbn_uUpdown","stkbn_uKillNum","tbl_tong_boss","tb_uNum")
end
------------------------------------------------------------------------------------------
function SortTongInfoDB.CreateSortTbl2(data)
	local uCamp = data["uCamp"]
	--创建阵营为uCamp的佣兵小队等级榜
	CreateTongSortByCamp("tbl_sort_tong_level","tbl_sort_tong_level_by_camp","stlbc_uOrder","stlbc_uUpdown","stl_uLevel","stl_uOrder",uCamp) 
	--创建阵营为uCamp的佣兵小队声望排行
	CreateTongSortByCamp("tbl_sort_tong_exploit","tbl_sort_tong_exploit_by_camp","stebc_uOrder","stebc_uUpdown","ste_uExploit","ste_uOrder",uCamp)
	--创建阵营为uCamp的佣兵小队军资榜
	CreateTongSortByCamp("tbl_sort_tong_resource","tbl_sort_tong_resource_by_camp","strbc_uOrder","strbc_uUpdown","str_uResource","str_uOrder",uCamp)
	--创建阵营为uCamp的佣兵小队资金榜
	CreateTongSortByCamp("tbl_sort_tong_money","tbl_sort_tong_money_by_camp","stmbc_uOrder","stmbc_uUpdown","stm_uMoney","stm_uOrder",uCamp)
	--创建阵营为uCamp的佣兵小队资源点占领次数排行榜
	CreateTongSortByCamp("tbl_sort_tong_resource_occupy_times","tbl_sort_tong_resource_occupy_times_by_camp","strotbc_uOrder","strotbc_uUpdown","strot_uOccupyTimes","strot_uOrder",uCamp)
	--创建阵营为uCamp的佣兵小队击杀领主次数排行榜
	CreateTongSortByCamp("tbl_sort_tong_kill_boss_num","tbl_sort_tong_kill_boss_num_by_camp","stkbnbc_uOrder","stkbnbc_uUpdown","stkbn_uKillNum","stkbn_uOrder",uCamp)
end

return SortTongInfoDB


