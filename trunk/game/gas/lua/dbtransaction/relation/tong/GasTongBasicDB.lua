cfg_load "tong/TongUpLevelNeedDevelopDegree"
gac_gas_require "relation/tong/TongPurviewMgr"
gac_gas_require "framework/common/CMoney"
gas_require "message/ServerMsg"
gac_gas_require "relation/tong/TongProdItemMgr"
gas_require "relation/RevertValidateMgr"
gac_gas_require "relation/tong/TongMgr"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
gac_gas_require "relation/army_corps/ArmyCorpsMgr"

local RevertValidateMgr = RevertValidateMgr:new()
local g_MoneyMgr = CMoney:new()
local StmtOperater = {}	
local g_ItemInfoMgr = CItemInfoMgr:new()	
local g_TongMgr = CTongMgr:new()
local textFilter = CTextFilterMgr:new()
local g_TongPurviewMgr = g_TongPurviewMgr
local GetStaticTextServer = GetStaticTextServer
local event_type_tbl = event_type_tbl
local BuildingItemMgr  = CTongProdItemInfoMgr:new()
local g_ArmyCorpsMgr = CArmyCorpsMgr:new()
local LogErr = LogErr
local TongUpLevelNeedDevelopDegree = TongUpLevelNeedDevelopDegree
local ECamp = ECamp
------------------------------------
local CTongBasicBox = CreateDbBox(...)
----------------------------------SQL相关-------------------------------------------------
--【查询所有成员信息】
local StmtDef = {
    	"_SelectTongMemberInfo",
    	[[ 
    		select 	mt.cs_uId, mt.mt_sPosition, cb.cb_uLevel, cs.cs_uClass, tc.c_sName,
    	  				ifnull(co_uOnServerId,0), tp.tp_uProffer, mt.mt_uPorfferExpend, mt.mt_uTongProffer, date(online.co_dtLastLogOutTime),date(now())
    	  	from 	tbl_member_tong mt, tbl_char_basic cb, tbl_char_static cs,tbl_char_onlinetime as online,
    	  				tbl_char tc 
    	  	left join tbl_tong_proffer as tp
    	  		on(tc.cs_uId = tp.cs_uId)
    	  	left join tbl_char_online co on co.cs_uId = tc.cs_uId
    	  	where 	mt.t_uId = ?
    	  	and 	mt.cs_uId = cb.cs_uId 
    	  	and 	cb.cs_uId = cs.cs_uId
    	  	and 	cs.cs_uId = online.cs_uId
    	  	and   online.cs_uId = tc.cs_uId
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_SelectGetExploit",
	[[
		select be_uGetExploit 
			from tbl_tong_battle_exploit 
			where cs_uId = ?
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_GetGetExploit",
	[[
		select cs_uId, be_uGetExploit 
			from tbl_tong_battle_exploit 
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_InsertGetExploit",
	[[
		insert into tbl_tong_battle_exploit (cs_uId,be_uGetExploit)
		value(?,?)
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_UpdateGetExploit",
	[[
		update tbl_tong_battle_exploit  set be_uGetExploit = be_uGetExploit + ?
    		where cs_uId = ?
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_DeleteGetExploit",
	[[
		delete  from tbl_tong_battle_exploit 
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_SelectCampExploit",
	[[
		select ce_uAmCampExploit,ce_uSsCampExploit,ce_uXsCampExploit 
			from tbl_battle_camp_exploit 
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_InsertCampExploit",
	[[
		insert into tbl_battle_camp_exploit (ce_uAmCampExploit,ce_uSsCampExploit,ce_uXsCampExploit)
		value(?,?,?)	
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_UpdateCampExploit",
	[[
		update tbl_battle_camp_exploit  set ce_uAmCampExploit = ce_uAmCampExploit + ?,ce_uSsCampExploit = ce_uSsCampExploit + ?,
																				ce_uXsCampExploit = ce_uXsCampExploit + ?
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_DeleteCampExploit",
	[[
		delete from tbl_battle_camp_exploit
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"FindAllTongOnlineChar",
	[[
		select 
			co.cs_uId 
		from 
			tbl_member_tong as mt, tbl_char_online as co 
		where
			mt.cs_uId = co.cs_uId and  
			mt.t_uId = ?
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"FindAllTongOnlineByPos",
	[[
		select 
			co.cs_uId 
		from 
			tbl_member_tong as mt, tbl_char_online as co, tbl_tong_rob_res as trr
		where
			mt.cs_uId = co.cs_uId and  
			mt.t_uId = ? and mt.mt_sPosition >= 2
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"FindAllTongOnlineMember",
	[[
		select 
			co.cs_uId, co.co_uOnServerId 
		from 
			tbl_member_tong as mt, tbl_char_online as co 
		where
			mt.cs_uId = co.cs_uId and  
			mt.t_uId = ?
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"_UpdateTongName",
	[[
		update tbl_tong set t_sName = ? where t_uId = ?
	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_CountBuildByName",
		[[
			select count(*) from tbl_building_tong
			where  bt_sBuildName = ? and t_uId = ? and bt_uState = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )
--【获得所有帮会的信息】
--ps：返回帮会id、名称、人数
local StmtDef = {
    	"_SelectAllTongInfo",
    	[[ 
    	  select  t.t_uId,t.t_sName,count(mt.cs_uId),t.t_uLevel,
    	  				ifnull(wzs.wz_uId,0),ifnull(wzs.wzs_uIndex,0)
    	  from    tbl_member_tong mt ,tbl_tong t
    	  left join tbl_war_zone_station wzs
    	  on (wzs.t_uId = t.t_uId)
    	  where   t.t_uId = mt.t_uId 
    	  and t.t_uCamp = ?
    	  group by  mt.t_uId
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


--【获得所有未加入佣兵小队的玩家的信息】
--ps：返回玩家id、名称、等级、职业、战斗力、国贡和声望
local StmtDef = {
    	"_SelectAllPlayerInfo",
    	[[ 
    	  select co.cs_uId, tc.c_sName, cb.cb_uLevel, cs.cs_uClass, fe.cfe_uPoint, ifnull(tp.tp_uProffer,0),ifnull(te.te_uExploit,0)
    	  from tbl_char_online co,tbl_char_static cs, tbl_char tc, tbl_char_fighting_evaluation fe,tbl_char_basic cb
    	  left join tbl_tong_proffer tp on tp.cs_uId = cb.cs_uId
    	  left join tbl_tong_exploit te on te.cs_uId = cb.cs_uId
    	  where 
    	  	co.cs_uId = cs.cs_uId and
    	  	cs.cs_uId = tc.cs_uId and
    	  	tc.cs_uId = fe.cs_uId and
    	  	fe.cs_uId = cb.cs_uId 
    	  	and cs.cs_uCamp = ?  
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_SelectPlayerOnlineInfo",
    	[[ 
    	  select count(*) from tbl_char_online where cs_uId = ?  
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【查询帮会信息】
--ps：获得某帮会的基本信息
local StmtDef = {
    	"_SelectTongInfo",	
    	[[ 
    	  select t.t_sName, t.t_dtCreateTime, t.t_uHonor,t.t_uMoney, t.t_uResource, 
    	  count(mt.cs_uId), t.t_sPurpose,t.t_uLevel,t.t_uType,t.t_uDevelopDegree,t_uPermitUpLevel
    	  from    tbl_tong t, tbl_member_tong mt
    	  where   t.t_uId = mt.t_uId  
    	  and 	  t.t_uId = ?
    	  group by 	 mt.t_uId
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【查询帮会信息】
--ps：获得某帮会的基本信息
local StmtDef = {
    	"_SelectTongLevel",	
    	[[ 
    	  select t_uLevel from    tbl_tong where t_uId = ?
    	 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

 
--【查询所有申请信息】
local StmtDef = {
    	"_SelectRequestInfo",
    	[[ 
    		select rt.cs_uId,tc1.c_sName,cb.cb_uLevel,cs.cs_uClass,ifnull(te.te_uExploit,0)
    		from tbl_char_basic cb, tbl_char_static cs, tbl_char tc1,tbl_request_tong rt
    		left join tbl_tong_exploit te
    			on(rt.cs_uId = te.cs_uId)
    		where 
    			rt.cs_uId = cb.cs_uId 
    	  	and cb.cs_uId = cs.cs_uId
    	  	and cs.cs_uId = tc1.cs_uId
    	  	and rt.t_uId = ?
    	  	and unix_timestamp(now())-unix_timestamp(rt.rt_dtRequestTime) <= ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelRequestInfo",
    	[[ 
    		delete from tbl_request_tong where unix_timestamp(now())-unix_timestamp(rt_dtRequestTime) >= ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【得到帮会id】
--ps：没加入帮会则返回0
local StmtDef = {
    	"_SelectTongId",
    	[[ 
    		select tong.t_uId,tong.t_sName,member.mt_sPosition  from tbl_member_tong as member,tbl_tong as tong  
    		where tong.t_uId = member.t_uId 
    		and   member.cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongNameById",
    	[[ 
    		select t_sName from tbl_tong  
    		where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


--【查询玩家离开帮会的时间】
local StmtDef = {
    	"_SelectLeaveTime",
    	[[ 
    		select unix_timestamp(now())-unix_timestamp(lt_dtQuitTime)  from  tbl_leave_tong where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【添加帮会信息】
local StmtDef = {
    	"AddATong",
    	[[ insert into tbl_tong(t_sName, t_dtCreateTime, t_uCamp,t_sPurpose, t_uDepotID,t_uMoney,t_uResource,t_uLevel,t_uHonor,t_uInitiatorId) 
    			values(?, now(),?, ?, ?,?,?,?,?,?)
     	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【增加帮会成员】
local StmtDef = {
    	"AddTongMembers",
    	[[ 
    		insert into tbl_member_tong(cs_uId, t_uId, mt_sPosition, mt_uPorfferExpend, mt_uTongProffer, mt_dtJoinTime) 
    		values(?,?,?,0,0,now())
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddExpendProffer",
    	[[ 
    		update tbl_member_tong  set mt_uPorfferExpend = mt_uPorfferExpend + ?
    		where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddTongProffer",
    	[[ 
    		update tbl_member_tong  set mt_uTongProffer = mt_uTongProffer + ?
    		where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--查询玩家队贡
local StmtDef = {
    	"_SelectTongProffer",
    	[[ 
    		select mt_uTongProffer from tbl_member_tong
    		where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【增加帮会日志】
local StmtDef = {
    	"AddTongLog",
    	[[ 
    		insert into tbl_log_tong(t_uId, lt_sContent, lt_Type, lt_dtCreateTime)
    		 values(?, ?, ?, now())
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【增加帮会建筑】
local StmtDef = {
    	"_AddTongBuild",
    	[[ 
    		insert into tbl_building_tong(t_uId,bt_sBuildName,
    		 bt_uPosX, bt_uPosY,bt_uState, bt_uLevel,bt_uStep) 
    		 values(?, ?, ?, ?,?,?,?)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddBuildLife",
    	[[ 
    		insert into tbl_building_life(bt_uId, bl_uLife, bl_dtLastTime) 
    		 values(?, ?, now())
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【根据角色id得到职位】
local StmtDef = {
    	"_GetPosition",
    	[[ 
    		 select  mt_sPosition  from tbl_member_tong  where cs_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【根据职位得到角色id】
local StmtDef = {
    	"_GetMemberByPosition",
    	[[
    		 select  cs_uId  from tbl_member_tong  where mt_sPosition = ? and t_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【删除申请信息】
local StmtDef = {
    	"_DeleteRequestInfo",
    	[[ 
    		 delete from tbl_request_tong where cs_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【修改职位】
local StmtDef = {
    	"_UpdatePosition",
    	[[ 
    		update tbl_member_tong set mt_sPosition = ? where cs_uId = ? and mt_sPosition = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【获取某职位人数个数】
local StmtDef = {
    	"_GetMemberByPos",
    	[[ 
    		select count(*) from tbl_member_tong where mt_sPosition = ? and t_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【查询日志信息】
--获取某帮会所有日志
local StmtDef = {
    	"_SelectAllLogs",
    	[[ 
    		select lt_uId, t_uId, lt_sContent, lt_Type, unix_timestamp(lt_dtCreateTime) 
    		from tbl_log_tong 
    		where t_uId = ? 
    		order by lt_dtCreateTime desc  limit 100
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--获取某类型的所有日志
local StmtDef = {
    	"_SelectLogsByType",
    	[[ 
    		select 	lt_uId, t_uId, lt_sContent, lt_Type, unix_timestamp(lt_dtCreateTime) 
    		from 		tbl_log_tong 
    		where 	t_uId = ? 
    		and 		lt_Type = ? 
    		order by 	lt_dtCreateTime desc  limit 100
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【修改帮会宗旨】
local StmtDef = {
    	"_UpdatePurpose",
    	[[ update tbl_tong set t_sPurpose = ? where t_uId = ? ]]
}    
DefineSql ( StmtDef, StmtOperater )

--【离开帮会(开除或自己离开)】
local StmtDef = {
    	"_DeleteMember",
    	--从帮会成员表删除某成员
    	[[ 
    		delete from tbl_member_tong  where  cs_uId = ? and t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"_AddDeleteMember",
    	--把退出帮会成员加入退出帮会表
    	[[ 
    		insert into tbl_leave_tong  values(?,now()) 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelLeaveTong",
    	[[ 
    		delete from  tbl_leave_tong where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_CountTongMembers",
    	--统计帮会成员个数 
    	[[ 
    		select count(*) from tbl_member_tong where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddRequestInfor",
    	[[ 
    		replace into  tbl_request_tong(cs_uId,t_uId,rt_dtRequestTime)  
    		values(?,?,now())
     	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_CountRequest",
    	[[ 
    		select count(distinct(cs_uId)) from tbl_request_tong 
    		where t_uId = ?
    		and unix_timestamp(now())-unix_timestamp(rt_dtRequestTime) <= ?
     	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongLevelMemNum",
    	[[ 
    		select count(M.cs_uId),T.t_uLevel from tbl_tong as T,tbl_member_tong as M 
    		where T.t_uId = M.t_uId and T.t_uId = ?
    		group by M.t_uId
     	]]
}    
DefineSql ( StmtDef, StmtOperater )

--删除帮会基本信息
local StmtDef = {
    	"_DelAllTongStaticInfo",
    	[[ 
				delete from tbl_tong where t_uId = ?
     	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"_GetTongDepotID",
    	--得到帮会仓库id
    	[[ 
				select t_uDepotID from tbl_tong where t_uId = ?
     	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"_DeleteTongDepotItem",
    	--删除帮会仓库物品
    	[[ 
				delete from tbl_item_collectivity_depot where cds_uId = ?
     	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"_DeleteTongDepot",
    	--删除帮会仓库
    	[[ 
				delete from tbl_collectivity_depot where cds_uId = ?
     	]]
}    
DefineSql ( StmtDef, StmtOperater )


--【根据帮会id得到帮会所属阵营】
local StmtDef = {
    	"_GetTongCampByID",
    	[[ 
    		 select t_uCamp from tbl_tong where t_uId = ?
     	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【根据帮会id得到帮会数量】
local StmtDef = {
    	"_CountTongByID",
    	[[ 
    		 select count(*) from tbl_tong where t_uId = ?
     	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_CountTongByName",
    	[[ select count(*) from tbl_tong  where t_sName = ?  ]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongIdAndCampByName",
    	[[ select t_uId, t_uCamp from tbl_tong  where t_sName = ?  ]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetCampByCharID",
    	[[ select cs_uCamp from tbl_char_static  where cs_uId = ?  ]]
}    
DefineSql ( StmtDef, StmtOperater )

--得到帮主名称
local StmtDef = 
{
		"_GetTongLeaderName",
		[[ select ch.c_sName from tbl_char as ch, tbl_member_tong as tm 
				where ch.cs_uId = tm.cs_uId 
				and 	tm.mt_sPosition = ? and tm.t_uId = ?
		]]	
}
DefineSql ( StmtDef, StmtOperater )

--得到帮主id
local StmtDef = 
{
		"_GetTongLeaderId",
		[[ select cs_uId from tbl_member_tong
				where mt_sPosition = ?
				and t_uId = ?
		]]	
}
DefineSql ( StmtDef, StmtOperater )


--得到佣兵小队等级大于2的小队队长ID
local StmtDef = 
{
		"_GetTongCaptainsId",
		[[select cs_uId from tbl_member_tong as mt,tbl_tong as tt
				where mt.mt_sPosition = 5
				and tt.t_uLevel >=2
				and mt.t_uId = tt.t_uId
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_GetRequestInfoByPlayerID",
		[[ select count(*) from tbl_request_tong
				where cs_uId = ? 
				and 	unix_timestamp(now())-unix_timestamp(rt_dtRequestTime) <= ?
				and t_uId = ?
		]]	
}
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
	"_GetTongLevelAndHonor",
	[[
		select t_uLevel,t_uHonor from tbl_tong where t_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_UpdateHonor",
	[[
		update tbl_tong set t_uHonor = t_uHonor + ? where t_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_UpdateExploit",
	[[
		update tbl_tong_exploit set te_uExploit = te_uExploit + ? where cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_GetMemberExploit",
	[[
		select te_uExploit from tbl_tong_exploit where cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_InsertExploit",
	[[
		insert into tbl_tong_exploit set  cs_uId = ?,te_uExploit = ?
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
    	"_UpdateTongMoney",
    	[[ 
    		update tbl_tong set t_uMoney = t_uMoney + ? where t_uMoney+? >= 0 and t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
	"_UpdateLevel",
	[[
		update tbl_tong set t_uLevel = ? where t_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_GetTongResource",
	[[
		select t_uResource from tbl_tong where t_uId=?
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_AddTongProperty",
	[[
		update tbl_tong set t_uMoney = t_uMoney + ?, t_uResource = t_uResource + ? where t_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_GetChallengeBattleState",
	[[
		select tcb_nState from tbl_tong_challenge_battle
    where tcb_nActiveTongID = ?
    and tcb_nPassiveTongID = ?
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_GetTongChallengeBattleState",
	[[
		select tcb_nState from tbl_tong_challenge_battle
    where tcb_nActiveTongID = ?
    or tcb_nPassiveTongID = ?
	]]
}
DefineSql(StmtDef, StmtOperater)
local StmtDef = 
{
	"_GetAllMemByTongID",
	[[
		select cs_uId from tbl_member_tong where t_uId = ?
	]]
}
DefineSql ( StmtDef, StmtOperater )

--【得到帮会成员等级信息】
local StmtDef = {
    	"GetTongMemberLevelInfo",
    	[[ 
    		select cb_uLevel
				from tbl_char_basic, tbl_member_tong
				where tbl_char_basic.cs_uId = tbl_member_tong.cs_uId
				and tbl_member_tong.t_uId = ?
				order by cb_uLevel desc
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetItemInfoByBindingType",
    	[[ 
    		select 
    			item.is_uType,item.is_sName,room.gir_uRoomIndex,room.gir_uPos,count(*) as nCount
				from 
					tbl_item_in_grid as grid, tbl_item_is_binding as bind,tbl_item_static as item,tbl_grid_in_room as room
				where 
					grid.gir_uGridID = room.gir_uGridID
					and grid.is_uId = bind.is_uId 
					and bind.is_uId = item.is_uId 
					and room.cs_uId = ?
					and bind.isb_bIsbind = ?
				group by 
					item.is_uType,item.is_sName,room.gir_uRoomIndex,room.gir_uPos
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【得到玩家帮贡】
local StmtDef = {
    	"_GetProffer",
    	[[ 
    		select tp_uProffer from tbl_tong_proffer where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_InsertProffer",
    	[[ 
    		insert into tbl_tong_proffer set  tp_uProffer = ?, cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【增加玩家帮贡】
local StmtDef = {
    	"_AddProffer",
    	[[ 
    		update tbl_tong_proffer set  tp_uProffer = tp_uProffer + ?  where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetExploitByTong",
    	[[ 
    		select sum(te.te_uExploit) from tbl_member_tong as mt,tbl_tong_exploit as te
    		where te.cs_uId = mt.cs_uId and mt.t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddWarTongInfo",
    	"insert into tbl_war_tong(cs_uId, wt_uId, wt_uTime) values(?,?,?)"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetWarTongInfo",
    	"select wt_uTime from tbl_war_tong where cs_uId = ? and wt_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateWarTongInfo",
    	"update tbl_war_tong set wt_uTime = ? where cs_uId = ? and wt_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddWarExploitInfo",
    	"insert into tbl_war_exploit_time(cs_uId, we_uId, we_uTime) values(?,?,?)"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetWarExploitInfo",
    	"select we_uTime from tbl_war_exploit_time where cs_uId = ? and we_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateWarExploitInfo",
    	"update tbl_war_exploit_time set we_uTime = ? where cs_uId = ? and we_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongMoney",
    	"select t_uMoney from tbl_tong where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

--Boss争夺战佣兵小队杀Boss次数
local StmtDef = {
    	"_AddTongBoss",
    	"insert ignore into tbl_tong_boss (t_uId, tb_uNum) values (?, ?) "
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongBoss",
    	"select tb_uNum from tbl_tong_boss where t_uId = ?"
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateTongBoss",
    	"update tbl_tong_boss set tb_uNum = tb_uNum + ? where t_uId = ?"
}
DefineSql ( StmtDef, StmtOperater )

--Boss争夺战玩家杀Boss次数
local StmtDef = {
    	"_AddTongCharBoss",
    	"insert ignore into tbl_tong_char_boss (cs_uId, tcb_uNum) values (?, ?) "
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongCharBoss",
    	"select tcb_uNum from tbl_tong_char_boss where cs_uId = ?"
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateTongCharBoss",
    	"update tbl_tong_char_boss set tcb_uNum = tcb_uNum + ? where cs_uId = ?"
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateTongType",
	[[
		update tbl_tong set t_uType = ? where t_uId = ?
	]]
} 
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongType",
	[[
		select t_uType from tbl_tong where t_uId = ?
	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateTongTypeChanged",
	[[
		replace into tbl_tong_type_change(t_uId, ttc_uTypeChanged) values(?,?)
	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_SelectTongTypeChanged",
	[[
		select ttc_uTypeChanged from  tbl_tong_type_change where t_uId = ?
	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
			"_SelectTongDevelopDegreeInfo",
			"select t_uId, t_uLevel, t_uDevelopDegree, t_uPermitUpLevel from tbl_tong"
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
			"_SelectTongDevelopDegreeById",
			"select t_uDevelopDegree from tbl_tong where t_uId = ?"
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
			"_AddTongDevelopDegree",
			"update tbl_tong set t_uDevelopDegree = t_uDevelopDegree + ? where t_uId = ?"
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
			"_ResetTongDevelopDegree",
			"update tbl_tong set t_uDevelopDegree = 0"
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
			"_GetTongPermitUpLevel",
			"select t_uPermitUpLevel from tbl_tong where t_uId = ?"
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
			"_SetTongPermitUpLevel",
			"update tbl_tong set t_uPermitUpLevel = ? where t_uId = ?"
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
			"_GetTongMemberByScene",
			[[
				select 
					tbl_char_position.cs_uId 
				from 
					tbl_char_position, tbl_member_tong 
				where 
					tbl_char_position.sc_uId = ? and tbl_member_tong.t_uId = ? and tbl_char_position.cs_uId = tbl_member_tong.cs_uId
			]]
}
DefineSql ( StmtDef, StmtOperater )



function CTongBasicBox.GetTongMemberByScene(tongId, sceneId)
	local res = StmtOperater._GetTongMemberByScene:ExecStat(sceneId, tongId)
	return res
end


function  CTongBasicBox.GetTongMoney(data)
		local res = StmtOperater._GetTongMoney:ExecStat(data["tong_id"])
		if res:GetRowNum() == 0 then
			return 0
		end
		return res(1,1)
end

function  CTongBasicBox.GetTongMoneyAndLine(data)
		local res = StmtOperater._GetTongMoney:ExecStat(data["tong_id"])
		local WarZoneDB = RequireDbBox("WarZoneDB")
		local tongMarketDB = RequireDbBox("GasTongMarketDB")
		local multiServerDB = RequireDbBox("MultiServerDB")
		local nLine = WarZoneDB.GetTongRelativeLine(data["tong_id"])
		local param = {}
		param["tongId"] = data["tong_id"]
		local sceneType, sceneId, serverId, warZoneId, stationId = WarZoneDB.GetTongStationInfo(param)
		local tong_box = RequireDbBox("GasTongBasicDB")
		local purview = tong_box.JudgePurview(data["charId"], "IssueFetch")
		local tongCamp = tong_box.GetTongCampByID(data["tong_id"])
		if nLine == 0 then
			return 9346
		end
		if not purview then
			return 9457
		end
		if res:GetRowNum() > 0 then
			if res(1,1)  < 5 * 10000 then
				return 9351
			end
		else
			return 
		end
		local parameter = {}
		parameter["tongId"] = data["tong_id"]
		parameter["uStationId"] = stationId
		parameter["uLineId"] = nLine
		parameter["warZoneId"] = warZoneId
		
		local mesId = tongMarketDB.AddStationTime(parameter)
		if mesId then
			return mesId
		end
		
		local parameters = {}
		parameters["uTongID"] = data["tong_id"]
		parameters["uMoney"] = -5 * 10000
		parameters["nEventType"] = event_type_tbl["佣兵小队征粮扣钱"]
		CTongBasicBox.UpdateTongMoney(parameters)
		
		local playerTbl = multiServerDB.GetOnlinePlayerInfoByCamp({tongCamp})
		local tongName = CTongBasicBox.GetTongNameById(data["tong_id"])
		return {res(1,1), nLine, playerTbl, tongName}
end

-----------------------------------------------------------------------------------------
function  CTongBasicBox.DelTongMember(member_id,tong_id,sceneName)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local TongItemDB = RequireDbBox("GasTongItemCreateDB")
	local cdepotMgr = RequireDbBox("GasCDepotMgrDB")
	local tblItemPos = StmtOperater._GetItemInfoByBindingType:ExecStat(member_id,3)
	local tblRetRes = {}
	local nDepotID = CTongBasicBox.GetTongDepotID(tong_id)
	for i = 1,tblItemPos:GetRowNum() do
		local ItemType,ItemName = tblItemPos(i,1),tblItemPos(i,2)
		local nRoomIndex,nPos,nCount = tblItemPos(i,3),tblItemPos(i,4),tblItemPos(i,5)
  	local tblDelRes = nil
  	if cdepotMgr.HaveEnoughRoomPos(nDepotID,ItemType,ItemName,nil,nCount) then
 	 		tblDelRes = g_RoomMgr.MoveItemFromPackageByPos(member_id,nRoomIndex,nPos,nCount)	
 	 		if not IsTable(tblDelRes) then
 	 			CancelTran()
 	 			return false
 	 		end
 	 		local nRes = cdepotMgr.AutoAddItems(nDepotID,ItemType,ItemName,tblDelRes,nil)
 	 		if not IsTable(nRes) then
 	 			CancelTran()
 	 			return false
 	 		end
 	 	else
			tblDelRes = g_RoomMgr.DelItemByPos(member_id,nRoomIndex,nPos,nCount,event_type_tbl["帮会删除成员"])	
 	 		if not IsTable(tblDelRes) then
 	 			CancelTran()
 	 			return false
 	 		end
 	 		local param = {}
			param["PlayerId"] = member_id
			param["ItemType"] = tonumber(ItemType)
			param["ItemName"] = ItemName
			param["nCont"] = nCount
			param["sceneName"] = sceneName
 	 		if tonumber(ItemType) == g_ItemInfoMgr:GetTongItemBigID() then
					local TongItemDB = RequireDbBox("GasTongItemCreateDB")
					TongItemDB.DeleteTongBuildItem(param)
			end
 		end
 		tblDelRes.nRoomIndex = nRoomIndex
 	 	tblDelRes.nPos = nPos
 	 	table.insert(tblRetRes,tblDelRes)
	end
	local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
	local bForce = sceneName=="删除角色"
	local bRet,uMsg = ArmyCorps_box.DelArmyCorpsAdminWhenLeaveTong(member_id, bForce)
	if not bRet then
		return false, uMsg
	end
	local uArmyCorpsId = ArmyCorps_box.GetArmyCorpsIdByPlayerId(member_id)
	if uArmyCorpsId ~= 0 and CTongBasicBox.GetPositionById(member_id) == g_TongMgr:GetPosIndexByName("团长") and not bForce then
		return false, 8538
	end
	local uLastTongExploit = CTongBasicBox.GetTongExploit(tong_id)
	StmtOperater._DeleteMember:ExecSql('', member_id, tong_id)
	if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false
	end
	local uNowTongExploit = CTongBasicBox.GetTongExploit(tong_id)
	Db2CallBack("OnTongExploitChange", CTongBasicBox.OnTongExploitChange(tong_id,uLastTongExploit,uNowTongExploit))
	return true,tblRetRes
end

function CTongBasicBox.GetTongNameById(tong_id)
	local	query_list = StmtOperater._GetTongNameById:ExecStat(tong_id)
	if query_list:GetRowNum() == 0 then return end
	return query_list:GetData(0,0)
end

function CTongBasicBox.GetAllMemByTongID(tong_id)
  
  	local	query_list = StmtOperater._GetAllMemByTongID:ExecStat(tong_id)
  
		return query_list
end

function CTongBasicBox.GetTongResource(tong_id)
	local result =  StmtOperater._GetTongResource:ExecStat(tong_id)
	if result:GetRowNum() == 0 then
		return 0
	end
	return  result:GetData(0,0)
end

function CTongBasicBox.GetAllMemByTongIDInRes(tong_id)
  	local	query_list = StmtOperater._GetAllMemByTongID:ExecStat(tong_id)
  	local num = query_list:GetRowNum()
  	local tbl = {}
  	if num > 0 then
  		for i = 0, num - 1 do
  			local charId = query_list:GetData(i, 0)
  			table.insert(tbl,charId)
  			
  		end
  	end
	return tbl
end

function CTongBasicBox.GetAllMemByTongIDRpc(parameter)
  local	query_list = StmtOperater._GetAllMemByTongID:ExecStat(parameter["uTongID"])
 
	return query_list
end

function CTongBasicBox.GetAllMemAndGeByTongIDRpc(parameter)
  local	query_list = StmtOperater._GetAllMemByTongID:ExecStat(parameter["uTongID"])
  local playerId = parameter["uCharID"]
  local GetExploitTbl = {}
  local row = query_list:GetRowNum()
  if row == 0 then
  	GetExploitTbl[playerId] = 0
  	local result = StmtOperater._SelectGetExploit:ExecStat(playerId)
 		if result:GetRowNum() > 0 then 
 			local nGetExploit = result:GetNumber(0,0)
 			GetExploitTbl[playerId] = nGetExploit	
 		end
 	else
	  for index = 1, row do 
			local playerId = query_list(index, 1)
			GetExploitTbl[playerId] = 0
	 		local result = StmtOperater._SelectGetExploit:ExecStat(playerId)
	 		if result:GetRowNum() > 0 then 
	 			local nGetExploit = result:GetNumber(0,0)
	 			GetExploitTbl[playerId] = nGetExploit	
	 		end
	 	end
	end
	return query_list, GetExploitTbl
end

------获得所有国战期间获得声望的玩家，以及处理数据库
function CTongBasicBox.GetAllMemAndCeByTongIDRpc()
  local	query_list = StmtOperater._GetGetExploit:ExecStat()
 	local GetExploitTbl = {}
 	local GetAwardTbl = {}
 	local row = query_list:GetRowNum()
 	if row == 0 then
 		return
 	end
 	local ex = RequireDbBox("Exchanger")
 	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
  for i=0, row-1 do 
		local playerId = query_list:GetData(i,0)
		local nGetExploit = query_list:GetData(i,1)
		local sPlayerName = ex.getPlayerNameById(playerId)
		local nLevel = CharacterMediatorDB.GetPlayerLevel(playerId)
		local uTongID = CTongBasicBox.GetTongID(playerId)
		GetExploitTbl[playerId] = {sPlayerName, nLevel, nGetExploit,uTongID}
 	end	
 	for playerId, v in pairs (GetExploitTbl) do
 		local nLevel = CharacterMediatorDB.GetPlayerLevel(playerId)
 		local profferMos = 100*0.3/25         --------奖励声望系数
 		local uCamp = CTongBasicBox.GetCampByCharID(playerId)
	 	local nAmCamp,nSsCamp,nXsCamp = CTongBasicBox.GetCampExploitSumDB()
	 	local k1 = 0
	 	local k2 = 0
	 	local k3 = 0
	 	if nAmCamp > nSsCamp and nSsCamp > nXsCamp then
	 		k1 = 0.3
	 		k2 = 0.2
	 		k3 = 0.1
	 	end
	 	if nAmCamp > nXsCamp and nXsCamp > nSsCamp then
	 		k1 = 0.3
	 		k2 = 0.1
	 		k3 = 0.2
	 	end
	 	if nSsCamp > nAmCamp and nAmCamp > nXsCamp then
	 		k1 = 0.2
	 		k2 = 0.3
	 		k3 = 0.1
	 	end
	 	if nSsCamp > nXsCamp and nXsCamp > nAmCamp then
	 		k1 = 0.1
	 		k2 = 0.3
	 		k3 = 0.2
	 	end
	 	if nXsCamp > nAmCamp and nAmCamp > nSsCamp then
	 		k1 = 0.2
	 		k2 = 0.1
	 		k3 = 0.3
	 	end
	 	if nXsCamp > nSsCamp and nSsCamp > nAmCamp then
	 		k1 = 0.1
	 		k2 = 0.2
	 		k3 = 0.3
	 	end
	 	if nAmCamp == nSsCamp and nSsCamp > nXsCamp then
	 		k1 = 0.3
	 		k2 = 0.3
	 		k3 = 0.2
	 	end
	 	if nAmCamp == nSsCamp and nSsCamp < nXsCamp then
	 		k1 = 0.2
	 		k2 = 0.2
	 		k3 = 0.3
	 	end
	 	if nAmCamp == nXsCamp and nXsCamp > nSsCamp then
	 		k1 = 0.3
	 		k2 = 0.2
	 		k3 = 0.3
	 	end
	 	if nAmCamp == nXsCamp and nXsCamp < nSsCamp then
	 		k1 = 0.2
	 		k2 = 0.3
	 		k3 = 0.2
	 	end
	 	if nSsCamp == nXsCamp and nXsCamp > nAmCamp then 
	 		k1 = 0.2
	 		k2 = 0.3
	 		k3 = 0.3
	 	end
	 	if nSsCamp == nXsCamp and nXsCamp < nAmCamp then 
	 		k1 = 0.3
	 		k2 = 0.2
	 		k3 = 0.2
	 	end
	 	if nAmCamp == nSsCamp and nSsCamp == nXsCamp then
	 		k1 = 0.3
	 		k2 = 0.3
	 		k3 = 0.3
	 	end	 	
		if v[3] > 200 then                          --如果获得声望大于200则给于封顶值200
			v[3] = 200
		end
		local nGetProffer = nLevel*profferMos*v[3]
	 	local nCampAward = 0
	 	if uCamp == ECamp.eCamp_AmyEmpire then
	 		nCampAward = nGetProffer*k1
	 	elseif uCamp == ECamp.eCamp_WestEmpire then
	 		nCampAward = nGetProffer*k2
	 	elseif uCamp == ECamp.eCamp_DevilIsland then
	 		nCampAward = nGetProffer*k3
	 	end
	 	GetAwardTbl[playerId] = {nGetProffer, nCampAward}	 		 		
	end		
	return  GetExploitTbl, GetAwardTbl
end

function CTongBasicBox.SendWarAwardDB(data)
	local GetExploitTbl = data.ExploitTbl
	local GetAwardTbl = data.AwardTbl
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	for playerId ,v in pairs(GetExploitTbl) do
		local nGetProffer = GetAwardTbl[playerId][1]
		local nCampAward = GetAwardTbl[playerId][2]
		local nGetAwardSum = nGetProffer + nCampAward
		AreaFbPointDB.AddAreaFbPointByType(playerId,nGetAwardSum,10,"",nil,event_type_tbl["国战奖励国贡"])
	end	
end

function CTongBasicBox.ClearDataInDataBaseDB()
	StmtOperater._DeleteGetExploit:ExecStat()
	StmtOperater._DeleteCampExploit:ExecStat()
end

function CTongBasicBox.GetTongKillMonsCountInfo(parameter)
	local	query_list = StmtOperater._GetAllMemByTongID:ExecStat(parameter["uTongID"])
	local row = query_list:GetRowNum()
	local ex = RequireDbBox("Exchanger")
	local result = {}
	for i = 1, row do
		local nCsId = query_list:GetData(i - 1, 0)
		local nFlag, sPlayerName, nLevel = ex.getPlayerNameAndLevelById(nCsId)
		if nFlag then
			result[nCsId] = {}
			result[nCsId].sName = sPlayerName
			result[nCsId].nLevel = nLevel
		end
	end
	
	return result
end

function CTongBasicBox.GetTongInfoForInit(parameters)
	local TongID = parameters.tong_id
	local server_id = parameters.server_id
	local char_id = parameters.uCharID
	local char_tong_id,char_tong_name = CTongBasicBox.GetTongID(char_id)
	
	local all_members = CTongBasicBox.GetAllMemByTongID(TongID)
	local TongInfo = {}
	TongInfo.m_AllMembers = all_members
	TongInfo.m_TongID = TongID
	TongInfo.m_TongName = CTongBasicBox.GetTongNameById(TongID)
	
	local data = {}
	data.m_tblTongInfo = TongInfo
	data.m_tbl_CharTongInfo = {char_tong_id,char_tong_name} 
	
	local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
	local uArmyCorpsId = ArmyCorps_box.GetArmyCorpsIdByPlayerId(char_id)
	local ArmyCorpsBaseInfo = ArmyCorps_box.GetArmyCorpsBasicInfoDB(uArmyCorpsId)
	if uArmyCorpsId~=0 and ArmyCorpsBaseInfo and ArmyCorpsBaseInfo:GetRowNum() ~= 0 then
		local uArmyCorpsPos	= ArmyCorps_box.GetPositionById(char_id)
		data.m_ArmyCorpInfo = {uArmyCorpsId,ArmyCorpsBaseInfo:GetString(0,0),uArmyCorpsPos}
	else
		data.m_ArmyCorpInfo = {0,"",0}
	end
	
	return data
end

--得到帮会等级和荣誉
function  CTongBasicBox.GetTongLevelAndHonor(tong_id)
  local level_info = StmtOperater._GetTongLevelAndHonor:ExecSql('nn',tong_id)
  if (level_info:GetRowNum() == 0) then
  	return 0,0
  end
  return level_info:GetNumber(0,0),level_info:GetNumber(0,1)
end


--添加荣誉
function  CTongBasicBox.UpdateHonor(parameters)
	local tong_id = parameters["uTongID"]
	local nHonor = parameters["uHonor"]
	
	if nHonor == 0 then
		return true
	end
  StmtOperater._UpdateHonor:ExecSql('',nHonor,tong_id)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return false, 9440
  end
  local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveTongHonorInfo(tong_id,nHonor,parameters["nEventType"])
	
	return CTongBasicBox.UpdateLevel(tong_id,parameters["nEventType"])
end

function CTongBasicBox.UpdateLevel(tong_id, nEventType)
  local now_level,nHonor = CTongBasicBox.GetTongLevelAndHonor(tong_id)
  local nLevel = g_TongMgr:GetLevelByHonor(nHonor)
  if nLevel > now_level and CTongBasicBox.GetTongPermitUpLevel(tong_id) == 1 then
  	StmtOperater._UpdateLevel:ExecSql('',nLevel,tong_id)
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		CancelTran()
  		return false, 9440
  	end
  	if nLevel == 2 then--升到2级后,提示可以转变佣兵小队类型
  		local MemInfo = CTongBasicBox.GetTongOnlineCharId(tong_id)
  		Db2CallBack("OnTongLevelUp", MemInfo,nLevel)
  	end
  	if nLevel == 4 then
  		local TongTypeChanged = CTongBasicBox.SelectTongTypeChanged(tong_id)
  		local NowTongType = CTongBasicBox.GetTongType(tong_id)
  		if TongTypeChanged == 0 and NowTongType == g_TongMgr:GetTongTypeByName("生产") then
  			local MemInfo = CTongBasicBox.GetTongOnlineCharId(tong_id)
  			Db2CallBack("OnTongLevelUp", MemInfo,nLevel)
  		end
  	end
  	local sMsg = "2024_" .. g_TongMgr:LevelN2S(nLevel)
		if not CTongBasicBox.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("内政")) then
			return false, 9107
		end
		local g_LogMgr = RequireDbBox("LogMgrDB")
		g_LogMgr.SaveTongLevelInfo(tong_id,nLevel,nEventType)
  end
  return true
end

local function GetWarTongInfo(Attacker, BeAccacker, nKillTime)
	local result = StmtOperater._GetWarTongInfo:ExecStat(Attacker, BeAccacker)
	local ret = StmtOperater._GetWarExploitInfo:ExecStat(Attacker, BeAccacker)
	local killTime = 0
	if result:GetRowNum() > 0 then
		StmtOperater._UpdateWarTongInfo:ExecStat(nKillTime, Attacker, BeAccacker)
--		if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
--			CancelTran()
--			return false
--		end
		if ret:GetRowNum()>0 then
			local getExploitTime = ret:GetData(0,0)
			if nKillTime - getExploitTime >= 3 * 60 then
				StmtOperater._UpdateWarExploitInfo:ExecStat(nKillTime, Attacker, BeAccacker)
				return true
			end
		end
	else
		StmtOperater._AddWarTongInfo:ExecStat(Attacker, BeAccacker, nKillTime)
		StmtOperater._AddWarExploitInfo:ExecStat(Attacker, BeAccacker, nKillTime)
--		if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
--			CancelTran()
--			return false
--		end
		return true
	end
end

function  CTongBasicBox.UpdateTongExploit(data)
	local nCharID = data["nCharID"]
	local nExploit = data["uExploit"]
	local uTongId = CTongBasicBox.GetTongID(nCharID)
	local uLastTongExploit = CTongBasicBox.GetTongExploit(uTongId)
	uLastTongExploit = uLastTongExploit or 0
	local result = StmtOperater._GetMemberExploit:ExecStat(nCharID)
	if result:GetRowNum() == 0 then
		StmtOperater._InsertExploit:ExecStat(nCharID,nExploit)
	else
  	StmtOperater._UpdateExploit:ExecStat(nExploit, nCharID)
  end
  if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
  	CancelTran()
  	return false, nil, nil, nil
  end
  local uNowTongExploit = uLastTongExploit + nExploit
  local succ, succType, tongMember,bNeedNotify = CTongBasicBox.OnTongExploitChange(uTongId,uLastTongExploit,uNowTongExploit)
  if not succ then
  	return true,0,nil,nil
  end
  return succ, succType, tongMember,bNeedNotify
end

function CTongBasicBox.OnTongExploitChange(uTongId,uLastTongExploit,uNowTongExploit)
	uLastTongExploit = uLastTongExploit and uLastTongExploit or 0
	uNowTongExploit = uNowTongExploit and uNowTongExploit or 0
	local NowTongType = CTongBasicBox.GetTongType(uTongId)
	if uLastTongExploit >= g_TongMgr:GetUpTongExploit() and (uNowTongExploit < g_TongMgr:GetUpTongExploit()) and (NowTongType == g_TongMgr:GetTongTypeByName("精英战斗")) then
  	local parameters = {}
  	local bSucc,bNeedNotify,tongMember = CTongBasicBox.DownTongType(uTongId)
  	return bSucc,-1,tongMember,bNeedNotify
  elseif uLastTongExploit < g_TongMgr:GetUpTongExploit() and (uNowTongExploit >= g_TongMgr:GetUpTongExploit()) and (NowTongType == g_TongMgr:GetTongTypeByName("战斗"))then
  	local TongTypeChanged = CTongBasicBox.SelectTongTypeChanged(uTongId)
  	return true,1,CTongBasicBox.GetTongMemberByTongID(uTongId),TongTypeChanged==0
  end
  return false,nil,nil,nil
end

function  CTongBasicBox.UpdateExploit(parameters)
	local nCharID = parameters["nCharID"]
	local nKilledId = parameters["nKilledId"]
	local nExploit = parameters["uExploit"]
	local nKillTime = parameters["uKillTime"]
	local MemberTbl = parameters["MemberID"]
	if 0 == nExploit then return true end
	--时间判断]

	local flag = GetWarTongInfo(nCharID, nKilledId, nKillTime)
	if not flag then
		return false		
	end
	local uMemberCount = #(MemberTbl)
	for i=1, uMemberCount do
		local data = {}
		data["nCharID"] = MemberTbl[i]
		data["uExploit"] = nExploit
		CTongBasicBox.UpdateTongExploit(data)
		CTongBasicBox.UpdateGetExploit(MemberTbl[i],nExploit)
	end
	CTongBasicBox.UpdateGetExploit(nCharID,nExploit)
	local succ, succType, tongMember,bNeedNotify  = CTongBasicBox.UpdateTongExploit(parameters)
	return succ, succType, tongMember,bNeedNotify,flag
end

function CTongBasicBox.UpdateGetExploit(nCharID,nExploit)
	local result = StmtOperater._SelectGetExploit:ExecStat(nCharID)
	if result:GetRowNum() == 0 then
		StmtOperater._InsertGetExploit:ExecStat(nCharID,nExploit)
	else
  	StmtOperater._UpdateGetExploit:ExecStat(nExploit, nCharID)
  end
end

function CTongBasicBox.UpdateCampExploitDB(data)
	local nAmGetExploit = data["nAmGetExploit"]
	local nSsGetExploit = data["nSsGetExploit"]
	local nXsGetExploit = data["nXsGetExploit"]
	local result = StmtOperater._SelectCampExploit:ExecStat()
	if result:GetRowNum() == 0 then
		StmtOperater._InsertCampExploit:ExecStat(nAmGetExploit,nSsGetExploit,nXsGetExploit)
	else
		if nAmGetExploit ~= 0 or nSsGetExploit ~= 0 or nXsGetExploit ~= 0 then
			StmtOperater._UpdateCampExploit:ExecStat(nAmGetExploit,nSsGetExploit,nXsGetExploit)
		end
	end
	local bFlag = false
	if (g_DbChannelMgr:LastAffectedRowNum()>0) then
		bFlag = true
	end
	return bFlag, data
end

function CTongBasicBox.GetCampExploitSumDB()
	local ret = StmtOperater._SelectCampExploit:ExecStat()
	local nAmGetExploitSum = 0
	local nSsGetExploitSum = 0
	local nXsGetExploitSum = 0
	if ret:GetRowNum() > 0 then
		nAmGetExploitSum = ret:GetData(0,0)
		nSsGetExploitSum = ret:GetData(0,1)
		nXsGetExploitSum = ret:GetData(0,2)
	end
	return nAmGetExploitSum, nSsGetExploitSum, nXsGetExploitSum
end

function CTongBasicBox.UpdateTongMoney(data)
	local nTongID = data["uTongID"]
	local nMoney = data["uMoney"]
	StmtOperater._UpdateTongMoney:ExecSql("", nMoney, nMoney, nTongID)
	if (g_DbChannelMgr:LastAffectedRowNum() > 0) then
		local g_LogMgr = RequireDbBox("LogMgrDB")
	  g_LogMgr.SaveTongMoneyInfo(nTongID,nMoney,data["nEventType"])
	  return true
	end
	return false
end

function  CTongBasicBox.GetTongLeaderName(tong_id)
  local leader_name = StmtOperater._GetTongLeaderName:ExecSql('s[32]',g_TongMgr:GetPosIndexByName("团长"), tong_id)
  if (leader_name:GetRowNum() == 0) then
  	return ""
  end
  return leader_name:GetString(0,0)
end

function  CTongBasicBox.GetTongLeaderId(tong_id)
  local leader_id = StmtOperater._GetTongLeaderId:ExecStat(g_TongMgr:GetPosIndexByName("团长"), tong_id)
  if (leader_id:GetRowNum() == 0) then
  	return -1
  end
  return leader_id:GetNumber(0,0)
end

function  CTongBasicBox.GetMemberByPos(tong_id,nPos)
  local tblCount = StmtOperater._GetMemberByPos:ExecSql('n',nPos, tong_id)
  return tblCount:GetNumber(0,0)
end

--【得到某玩家的帮会id】
--ps：没有返回0
function  CTongBasicBox.GetTongID(player_id)
  local tong_id = StmtOperater._SelectTongId:ExecSql('ns[32]n', player_id)
  if (tong_id:GetRowNum() == 0) then
  	return 0,"",0
  end
  return tong_id:GetNumber(0,0),tong_id:GetString(0,1),tong_id:GetNumber(0,2)
end

function CTongBasicBox.GetTongLevelBackToHighSpeed(param)
	local tongID = param.tongID
	return CTongBasicBox.GetTongLevel(tongID)
end

function CTongBasicBox.GetTongLevel(tong_id)
  local tong_level = StmtOperater._SelectTongLevel:ExecSql('n', tong_id)
  if (tong_level:GetRowNum() == 0) then
  	tong_level:Release()
  	return 0
  end
  local res = tong_level:GetNumber(0,0)
  tong_level:Release()
  return res
end

function CTongBasicBox.GetTongBuildingMaxNum(data)
	local tong_id = data["TongId"]
	local tong_level = StmtOperater._SelectTongLevel:ExecSql('n', tong_id)
  if (tong_level:GetRowNum() == 0) then
  	tong_level:Release()
  	return 0
  end
  local res = tong_level:GetNumber(0,0)
  tong_level:Release()
  return 7 + res
end

--【根据玩家id得到玩家所属阵营】
function CTongBasicBox.GetCampByCharID(uPlayerID)
	local tblCamp = StmtOperater._GetCampByCharID:ExecSql('n',uPlayerID)
	if 0 == tblCamp:GetRowNum() then
		return 0
	end
	return tblCamp:GetNumber(0,0)
end


--【根据帮会id得到帮会所属阵营】
function CTongBasicBox.GetTongCampByID(uTongID)
	local tblCamp = StmtOperater._GetTongCampByID:ExecSql('n',uTongID)
	if 0 == tblCamp:GetRowNum() then
		return 0
	end
	return tblCamp:GetNumber(0,0)
end

function CTongBasicBox.GetTongCampByIDRpc(data)
	local uTongID = data["TongID"]
	return CTongBasicBox.GetTongCampByID(uTongID)
end

--【根据帮会id获得帮会数量】
function CTongBasicBox.CountTongByID(uTongID)
	local tblTong = StmtOperater._CountTongByID:ExecSql('n',uTongID)
	if 0 == tblTong:GetRowNum() then
		return 0
	end
	return tblTong:GetNumber(0,0)
end

--【根据帮会名称获得帮会数量】
function CTongBasicBox.CountTongByName(sTongName)
	local tblTong = StmtOperater._CountTongByName:ExecSql('n',sTongName)
	if 0 == tblTong:GetRowNum() then
		return 0
	end
	return tblTong:GetNumber(0,0)
end

function CTongBasicBox.GetTongIDAndCampByNameRpc(data)
	local tongName = data["TongName"]
	local result = StmtOperater._GetTongIdAndCampByName:ExecSql('n', tongName)
	if 0 == result:GetRowNum() then
		return 0
	end
	
	return result:GetData(0, 0), result:GetData(0, 1)
end

--【增加帮会建筑】
function CTongBasicBox.AddTongBuild(tong_id,build_name, nPosX,nPosY, state, level)
		local nStep,life = 1,1
  	StmtOperater._AddTongBuild:ExecSql('',tong_id,build_name, nPosX,nPosY,state, level,nStep)
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	 	CancelTran()
  	 	return
   	end
  	local build_id =  g_DbChannelMgr:LastInsertId()
  	StmtOperater._AddBuildLife:ExecSql('',build_id,life)
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	 	CancelTran()
  	 	return
   	end
		return build_id
end

--【添加帮会静态信息】
function CTongBasicBox.AddTongStatic(tong_name,tong_porpose,tong_camp,uPlayerID)
		--先创建一个集体仓库作为帮会的仓库
		local g_CDepotSQLMgr = RequireDbBox("GasCDepotSQLMgrDB")
		local uCDepotID = g_CDepotSQLMgr.CreateCDepot()
		if uCDepotID == 0 then
			CancelTran()
			return 9103
		end
		--添加帮会静态信息
  	StmtOperater.AddATong:ExecSql('', tong_name, tong_camp,tong_porpose, uCDepotID,g_TongMgr:GetDefaultMoney(),g_TongMgr:GetDefaultResource(),1,g_TongMgr:GetDefaultHonor(),uPlayerID)
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		CancelTran()
  		return 9104
  	end
  	local tongId = g_DbChannelMgr:LastInsertId()
  	
  	local g_LogMgr = RequireDbBox("LogMgrDB")
  	g_LogMgr.SaveTongHonorInfo(tongId,g_TongMgr:GetDefaultHonor(),event_type_tbl["创建佣兵小队"])
  	g_LogMgr.SaveTongLevelInfo(tongId,1,event_type_tbl["创建佣兵小队"])
  	g_LogMgr.SaveTongMoneyInfo(tongId,g_TongMgr:GetDefaultMoney(),event_type_tbl["创建佣兵小队"])
  	g_LogMgr.SaveTongResourceInfo(tongId,g_TongMgr:GetDefaultResource(),event_type_tbl["创建佣兵小队"])
  	
  	return g_LogMgr.SaveTongStaticInfo(tongId,tong_name,tong_camp,uPlayerID),tongId
end

--【添加帮会成员】
function CTongBasicBox.AddTongMember(uExecutorId, player_id, tong_id, sPosName, uJobType)
	local uposition = g_TongMgr:GetPosIndexByName(sPosName)
	local uLastTongExploit = CTongBasicBox.GetTongExploit(tong_id)
	StmtOperater.AddTongMembers:ExecSql('', player_id, tong_id, uposition)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongMemberEventLog(tong_id,1,uExecutorId,player_id,uJobType)
	
	if (g_DbChannelMgr:LastAffectedRowNum()>0)  then
		local uNowTongExploit = CTongBasicBox.GetTongExploit(tong_id)
		Db2CallBack("OnTongExploitChange", CTongBasicBox.OnTongExploitChange(tong_id,uLastTongExploit,uNowTongExploit))
		return true
	end

	return false
end

--【添加日志信息】
function CTongBasicBox.AddTongLogs(tong_id, msg, index)
  	StmtOperater.AddTongLog:ExecSql('', tong_id, msg , index)
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		CancelTran()
  		return false
  	end
  	return true
end

--【修改帮会宗旨】
function CTongBasicBox.UpdateTongPurpose(purpose, tong_id)
   	StmtOperater._UpdatePurpose:ExecSql('', purpose, tong_id)
   	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	 	CancelTran()
  	 	return false
   	end
   	return true
end
--【得到某玩家在帮会中的职位】
function CTongBasicBox.GetPositionById(player_id)
  	local pos = StmtOperater._GetPosition:ExecSql('n',player_id)
  	if pos:GetRowNum() == 0 then
  		pos:Release()
  		return 0
  	end
  	local position =  pos:GetData(0,0)
  	pos:Release()
  	return position
end

--【获得日志信息】
function CTongBasicBox.GetLogsInfo(tong_id, type)
  	local query_list = {}
  	if type == 0 then
  		query_list = StmtOperater._SelectAllLogs:ExecStat(tong_id)
  	else
  		query_list = StmtOperater._SelectLogsByType:ExecStat(tong_id, type)
  	end
  	
		return query_list
end

local BaseCampTbl = {}
BaseCampTbl[1] = "艾米大本营"
BaseCampTbl[2] = "神圣大本营"
BaseCampTbl[3] = "修斯大本营"

local DepotTbl = {}
DepotTbl[1] = "艾米粮草仓"
DepotTbl[2] = "神圣粮草仓"
DepotTbl[3] = "修斯粮草仓"

function CTongBasicBox.TongBuildingDeadSubResource(parameter)
	local uTongID = parameter["uTongID"]
	local buildingName = parameter["BuildingName"]
	local uCamp = CTongBasicBox.GetTongCampByID(uTongID)
	if buildingName == BaseCampTbl[uCamp] then
		local ownedRes = CTongBasicBox.GetTongResource(uTongID)
			local data = {}
			data["uTongID"] = uTongID
			data["uResource"] = -ownedRes * 0.20
			data["nEventType"] = event_type_tbl["佣兵小队建筑打爆资源掉落"]
			local Flag, AddedRes = CTongBasicBox.AddTongResource(data)
			if Flag then
				return -AddedRes
			end
	elseif buildingName == DepotTbl[uCamp] then
		local moreRes = CTongBasicBox.GetOneDepotAddedLimit(uTongID)
		local nNeedRes = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), buildingName,"NeedRes")
		local needRes = nNeedRes * 0.20
		local ownedRes = CTongBasicBox.GetTongResource(uTongID)
			local data = {}
			data["uTongID"] = uTongID
			data["uResource"] = -moreRes
			data["nEventType"] = event_type_tbl["佣兵小队建筑打爆资源掉落"]
			CTongBasicBox.AddTongResource(data)
			return moreRes + needRes
	else
		local nNeedRes = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), buildingName,"NeedRes")
		local needRes = nNeedRes * 0.20
		return needRes
	end
	return 0
end

function CTongBasicBox.AddTongResource(parameter)

	local uTongID = parameter["uTongID"]
	local uResource = parameter["uResource"]
	local uMaxCapbility = CTongBasicBox.GetUpperLimitResource(uTongID)
	
  	local uUpdateResource, uAddResource
  	local uOwnedResource = CTongBasicBox.GetTongResource(uTongID)
		if uOwnedResource + uResource < 0 then
  		uUpdateResource = 0
  		uAddResource = -uOwnedResource
  	else
  		uUpdateResource = uOwnedResource + uResource
  		uAddResource = uResource
  	end
  	
  	if uOwnedResource + uResource > uMaxCapbility then
  		uUpdateResource = uMaxCapbility
  		uAddResource = uMaxCapbility - uOwnedResource
  	end

  	local CTongBuildingBox = RequireDbBox("GasTongbuildingDB")
  	if CTongBuildingBox.UpdateTongResource(uAddResource,uTongID,parameter["nEventType"]) then
  		return true, uAddResource
  	else
  		--扣除资源不成功
			CancelTran()
			return false
  	end
end

----------------------------------------------------------------------------
--【创建帮会】
function CTongBasicBox.AddTong(uPlayerID,tong_name,tong_porpose,tong_camp, uLevel)
	local name_num = CTongBasicBox.CountTongByName(tong_name)
	--判断帮会名称是否已经存在
	if name_num ~= 0 then
		return 9001
	end
	
	--添加帮会静态信息
	local Ret,uTongID = CTongBasicBox.AddTongStatic(tong_name,tong_porpose,tong_camp,uPlayerID) 
	if IsNumber(Ret) then
		return Ret
	end
	
	--把自己加入帮会成员表
	if not CTongBasicBox.AddTongMember(uPlayerID, uPlayerID, uTongID, "团长", g_TongMgr:GetPosIndexByName("团长")) then
		return 9105
	end

	--+++++++++++++++++++--
	--临时使用坐标
	local x = 0
	local y = 0
	--+++++++++++++++++++--
	local BuildingName = BuildingItemMgr:GetBuildNameByCamp(tong_camp,"大本营")
	local buildind_id = CTongBasicBox.AddTongBuild(uTongID,BuildingName, x, y, g_TongMgr:GetBuildingState("正常"),uLevel) 
	if not buildind_id then return end
	
	--收取注册金
	local regist_money = g_TongMgr:GetRegistMoney()
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("Tong")
		local g_LogMgr = RequireDbBox("LogMgrDB")
		local uEventId = g_LogMgr.LogPlayerGiverToNpc( uPlayerID, "捐献金钱给帮会",{},event_type_tbl["创建佣兵小队"])
		local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["RegistTong"],uPlayerID,2, -regist_money,uEventId)
		if not bFlag then
			CancelTran() 
			if IsNumber(uMsgID) then
				return uMsgID
			end
			return 9003
		end
	--添加日志信息
	local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
	local sMsg = "2000_" .. player_name
	if not CTongBasicBox.AddTongLogs(uTongID,sMsg, g_TongMgr:GetLogType("人员")) then
		return 9107
	end
	return true,uTongID
end


--【得到所有帮会的信息】
function CTongBasicBox.GetAllTong(params)
	local nCharID = params.uPlayerID
	local nCamp = CTongBasicBox.GetCampByCharID(nCharID)
	local maxNum = 0
	local tbl = {}
	local query_result = StmtOperater._SelectAllTongInfo:ExecStat(nCamp)
	if query_result:GetRowNum()== 0 then
		return false,nil,9078
	end
	local WarZoneDB = RequireDbBox("WarZoneDB")
	for i=1, query_result:GetRowNum() do
		maxNum = g_TongMgr:GetTongTotalMem(query_result(i,4))
		local nStationLine = WarZoneDB.GetTongRelativeLine(query_result(i,1))
		local captainName = CTongBasicBox.GetTongLeaderName(query_result(i,1))
		if captainName == "" then
			LogErr("查找佣兵小队队长名字报错","tongId:"..query_result(i,1))	
		else
			table.insert(tbl,{query_result(i,1),query_result(i,2),query_result(i,3),query_result(i,4),captainName,query_result(i,5),query_result(i,6),maxNum, nStationLine})
		end
	end
	return true,tbl
end

--【查询输入的小队名称的信息】
function CTongBasicBox.SearchTongDB(params)
	local sName = params.sName
	local PlayerId = params.uPlayerID
	local data = {}
	data["TongName"] = sName
	local tongid,tongCamp = CTongBasicBox.GetTongIDAndCampByNameRpc(data)
	local playerCamp = CTongBasicBox.GetCampByCharID(PlayerId)
	if tongid == 0 then
		return false, 9068
	end
	if tongCamp ~= playerCamp then
		return false, 9069
	end
	local tong_info = StmtOperater._SelectTongInfo:ExecStat(tongid)
	local memNum = tong_info:GetData(0,5) --帮会人数
	local maxNum = g_TongMgr:GetTongTotalMem(tong_info:GetData(0,7))
	if memNum == maxNum then
		return false, 9063
	end
	return true
end

--【得到所有未加入佣兵小队的玩家信息】
function CTongBasicBox.GetAllPlayer(params)
	local nCharID = params.uPlayerID
	local nCamp = CTongBasicBox.GetCampByCharID(nCharID)
	local tbl = {}
	local query_result = StmtOperater._SelectAllPlayerInfo:ExecStat(nCamp)
	for i=1, query_result:GetRowNum() do
		local tongid = CTongBasicBox.GetTongID(query_result(i,1))
		if tongid == 0 then
			table.insert(tbl,{query_result(i,1),query_result(i,2),query_result(i,3),query_result(i,4),query_result(i,5),query_result(i,6),query_result(i,7)})
		end
	end
	return tbl
end

--【查询输入的玩家名称的信息】
function CTongBasicBox.SearchPlayerDB(params)
	local sName = params.sName
	local PlayerId = params.uPlayerID
	local ex = RequireDbBox("Exchanger")
	local uTargetID = ex.getPlayerIdByName(sName)
	if uTargetID == 0 then
		return false, 9064
	end
	local targetCamp = CTongBasicBox.GetCampByCharID(uTargetID)
	local playerCamp = CTongBasicBox.GetCampByCharID(PlayerId)
	if targetCamp ~= playerCamp then
		return false, 9065
	end
	local uTongID = CTongBasicBox.GetTongID(uTargetID)
	if uTongID ~= 0 then
		return false,9074
	end
	local res = StmtOperater._SelectPlayerOnlineInfo:ExecStat(uTargetID)
	if res:GetRowNum() > 0 and res:GetNumber(0,0) == 0 then
		return false ,9075
	end
	return true
end

--【得到帮会成员信息】
function CTongBasicBox.GetTongMemberInfo(tong_id)
  local query_result = StmtOperater._SelectTongMemberInfo:ExecStat(tong_id)	
	local nLevel = CTongBasicBox.GetTongLevel(tong_id)
	return query_result, g_TongMgr:GetTongTotalMem(nLevel)
end

function CTongBasicBox.GetTongOnlineCharId(tong_id)
	local result = StmtOperater.FindAllTongOnlineChar:ExecSql("n", tong_id)
	local tblMembers = result:GetCol(0)
	result:Release()
	return tblMembers
end

function CTongBasicBox.GetTongOnlineByPos(tong_id)
	local result = StmtOperater.FindAllTongOnlineByPos:ExecSql("n", tong_id)
	local tblMembers = result:GetCol(0)
	result:Release()
	return tblMembers
end

function CTongBasicBox.GetTongOnlineMemberInfo(data)
	local uTongID = data["uTongID"]
	local result = StmtOperater.FindAllTongOnlineMember:ExecStat(uTongID)
	return true, result
end

function CTongBasicBox.CountBuildByNameAddState(tong_id,b_name)
	local tong_camp = CTongBasicBox.GetTongCampByID(tong_id)
	local b_realName = BuildingItemMgr:GetBuildNameByCamp(tong_camp,b_name)
	local tbl = StmtOperater._CountBuildByName:ExecStat(b_realName,tong_id,g_TongMgr:GetBuildingState("正常"))
	local depot_count = tbl:GetNumber(0,0)
	return depot_count
end

--帮会资源存量上限
function CTongBasicBox.GetUpperLimitResource(tong_id)
	local nLevel = CTongBasicBox.GetTongLevel(tong_id)
	local init_res = g_TongMgr:GetTongTotalMem(nLevel)*50*7
	local depot_count = CTongBasicBox.CountBuildByNameAddState(tong_id,"粮草仓")
	local depot_content = g_TongMgr:GetDepotContent()
	return init_res+depot_count*depot_content
end

function CTongBasicBox.GetOneDepotAddedLimit(tongId)
	local originalCapacity = g_TongMgr:GetUpperResource()
		local ownedRes = CTongBasicBox.GetTongResource(tongId)
		if ownedRes <= originalCapacity then
			return 0
		else
			local depotContent = g_TongMgr:GetDepotContent()
			local resAdded = (ownedRes - originalCapacity) % depotContent
			if resAdded == 0 then
				resAdded = depotContent
			end
			return resAdded
		end
end

function CTongBasicBox.GetTongExploit(tong_id)
	local result = StmtOperater._GetExploitByTong:ExecStat(tong_id)
	if result:GetRowNum() == 0 then return 0 end
	return result(1,1)
end

function CTongBasicBox.GetTongInfo(tong_id)
  	local info = {}
  	
  	local tong_info = StmtOperater._SelectTongInfo:ExecStat(tong_id)
  	if tong_info:GetRowNum() == 0 then
  		return info
  	end
  	
  	local leader_name = CTongBasicBox.GetTongLeaderName(tong_id)
  	local upper_resource = CTongBasicBox.GetUpperLimitResource(tong_id)
  	
  	info.tong_id = tong_id
  	info.tong_name = tong_info:GetData(0,0) --帮会名称
  	info.leader_name = leader_name --帮主
  	
  	info.member_num = tong_info:GetData(0,5) --帮会人数
  	info.upper_member = g_TongMgr:GetTongTotalMem(tong_info:GetData(0,7))--人数上限
  	
  	info.tong_level = tong_info:GetData(0,7) --帮会等级
  	
  	info.tong_honor = tong_info:GetData(0,2) --帮会荣誉
  	info.tong_exploit = CTongBasicBox.GetTongExploit(tong_id) --帮会功勋
  	
  	info.tong_money = tong_info:GetData(0,3) --帮会资金
  	info.tong_resource = tong_info:GetData(0,4) --资源 
  	info.upper_resource = upper_resource	--资源上限
  	
  	info.tong_purpose  = tong_info:GetData(0,6)	--帮会宗旨  
  	
  	info.tong_type	   = tong_info:GetData(0,8)	--帮会类型 
  	info.tong_developdegree = tong_info:GetData(0,9)	--帮会发展度 
  	info.tong_permitUpLevel	= tong_info:GetData(0,10)	--帮会是否能升级 
  
  	local WarZoneDB = RequireDbBox("WarZoneDB")
  	local line, warZoneId, stationId = WarZoneDB.GetTongStationRpc({["uTongId"]= tong_id})
  	if 0 == line then 
  		warZoneId, stationId = 0,0
  	end
  	local TongRobRes = RequireDbBox("GasTongRobResourceDB")
		local resName = TongRobRes.GetRobResObjById(tong_id)
  	info.warZoneId = warZoneId --战区
  	info.stationId = stationId --位置(当前阵营、战线、驻地)
  	info.tong_resName   = resName
  	local bFlag = CTongBasicBox.GetTongChallengeBattleState({["nTongID"]= tong_id})
  	info.bWarState = bFlag
  	return info
end

--【获得申请信息】
--ps：返回为申请信息和最大申请信息数量
function CTongBasicBox.GetRequestInfo(tong_id)
	local RequstTimeLimit = g_TongMgr.m_nRequstTimeLimit
	StmtOperater._DelRequestInfo:ExecStat(RequstTimeLimit)
  local query_list = StmtOperater._SelectRequestInfo:ExecStat(tong_id,RequstTimeLimit)
 	local proffer_info = {}
	for i =1,query_list:GetRowNum() do
		local res_proffer = StmtOperater._GetProffer:ExecStat(query_list(i,1))
		if res_proffer:GetRowNum() > 0 then
			proffer_info[query_list(i,1)] = res_proffer(1,1)
		end
	end
  return query_list,proffer_info
end


--【判断角色离开帮会的时间是否在24小时内】
function  CTongBasicBox.JudgeLeaveTongTime(player_id)
  local time = StmtOperater._SelectLeaveTime:ExecSql('n', player_id)
  if time:GetRowNum() == 0 then
  	return 1
  end
  if (time:GetData(0,0) < 40*60) then
  	return 0
  end
  return 1
end

--【判断某人是否有某个权限】
function CTongBasicBox.JudgePurview(player_id, purview_name)
	local pos_index = CTongBasicBox.GetPositionById(player_id)
	return g_TongPurviewMgr:JudgePurview(pos_index,purview_name)
end

--【判断某人是否有某个权限】
function CTongBasicBox.IsPurviewEnough(data)
	local player_id = data["uPlayerId"]
	local purview_name = data["PurviewName"]
	return CTongBasicBox.JudgePurview(player_id, purview_name)
end

function CTongBasicBox.DeleteRequestInfo(target_id)
  	StmtOperater._DeleteRequestInfo:ExecSql('',target_id)
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		return
  	end
  	return true
end

--【修改职位】
function CTongBasicBox.LogUpdateMemberPosition(tong_id, player_id, target_id, new_position, old_position)
	local ex = RequireDbBox("Exchanger")
	local target_name = ex.getPlayerNameById(target_id)
	local player_name = ex.getPlayerNameById(player_id)
	local player_pos = CTongBasicBox.GetPositionById(player_id)
	--local target_pos = CTongBasicBox.GetPositionById(target_id)
  
  --添加帮会日志
  local sMsg = "2006_" .. player_name .. "," .. target_name .. ",msgid:" .. g_TongMgr.m_tblPosLevel2MsgID[new_position]
  if not CTongBasicBox.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("人员")) then
   	 return false, 9107
  end
  
  return true, 9113
end

--【批准某人加入帮会】
function CTongBasicBox.AcceptRequest(uPlayerID,target_id,tong_id)

  --把对方加入帮会人员表
  if not CTongBasicBox.AddTongMember(uPlayerID, target_id, tong_id, "团员", g_TongMgr:GetPosIndexByName("团员")) then
  	return false
  end
  --添加日志信息
	local ex = RequireDbBox("Exchanger")
  local target_name = ex.getPlayerNameById(target_id)
  local player_name = ex.getPlayerNameById(uPlayerID)
  local sMsg = "2007_" .. target_name .. "," .. player_name
  if not CTongBasicBox.AddTongLogs(tong_id, sMsg,g_TongMgr:GetLogType("人员")) then
  	return false
  end
  return true
end
-----------------------------------------------------------------------
function CTongBasicBox.AddLeaveTongInfo(char_id)
	StmtOperater._DelLeaveTong:ExecStat(char_id)
  StmtOperater._AddDeleteMember:ExecStat(char_id)
end

--【开除成员】
function CTongBasicBox.DropMember(target_id,tong_id,sceneName)
		local pos = CTongBasicBox.GetPositionById(target_id)
		
		local tblMemberCount = StmtOperater._CountTongMembers:ExecSql('n', tong_id)
  	if  (pos == g_TongMgr:GetPosIndexByName("团长") and tblMemberCount:GetNumber(0,0) > 1
  		and sceneName ~= "删除角色") then
  		--说明是帮主离开
  		--MsgToConn(Conn, 9005, "帮中有其他成员，帮主不能退帮，必须先转移职位才可离开")
  		return 9115
  	end
		--从成员表删除
  	local bRet, res = CTongBasicBox.DelTongMember(target_id,tong_id,sceneName)
  	if not bRet then return res end
  	--加入离开帮会成员表，用于再次加入帮会时判断时间是否在24小时内
  	--因为离开帮会24小时内不能再加入帮会
  	CTongBasicBox.AddLeaveTongInfo(target_id)
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
			CancelTran()
			return 
 	 	end

		local ex = RequireDbBox("Exchanger")
  	local target_name = ex.getPlayerNameById(target_id)
  	--添加日志信息
  	local sMsg = "2008_" .. target_name
  	if not CTongBasicBox.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("人员")) then
  		return 9107
  	end
  	--如果是帮主退帮，则帮会解散
  	if  (pos == g_TongMgr:GetPosIndexByName("团长") and 1 >= tblMemberCount:GetData(0,0)) then
  		if not CTongBasicBox.BreakTong(tong_id,target_id,sceneName) then
  			return 
  		end
  	end
  	return res
end
---------------------------------------------------------------------


function CTongBasicBox.RequestJoinInTong(tong_id, target_id)
  StmtOperater._AddRequestInfor:ExecSql('', target_id, tong_id)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return false
  end
  
  return true
end
------------------------------------------------------------------------

function CTongBasicBox.RequestBeFull(tong_id)
	local count = StmtOperater._CountRequest:ExecSql('n', tong_id,g_TongMgr.m_nRequstTimeLimit)
	if count:GetRowNum() == 0 then return end
	if count:GetData(0,0) >= g_TongMgr.m_nMaxRequestNum then return end
	return true
end

function CTongBasicBox.MemberBeFull(tong_id)
	local count = StmtOperater._GetTongLevelMemNum:ExecSql('nn', tong_id)
	if count:GetRowNum() == 0 then  return end

	local Num,Level = count:GetData(0,0),count:GetData(0,1)
	if Num >= g_TongMgr:GetTongTotalMem(Level) then  return end

	return true
end

function CTongBasicBox.GetTongDepotID(tong_id)
	local tblDepotID = StmtOperater._GetTongDepotID:ExecSql('n', tong_id)
	return tblDepotID:GetRowNum() > 0 and tblDepotID:GetNumber(0,0) or 0
end
-------------------------------------------------------
local StmtDef = {
	"_SelectTongDepotItem",
	--删除帮会仓库物品
	[[ 
		select is_uId from tbl_item_collectivity_depot where cds_uId = ?
 	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【解散】
--ps：解散帮会时要删除帮会相关的所有信息
function CTongBasicBox.BreakTong(tong_id,tongMangerId,sceneName)
	--帮会解散，必须清除掉所有跟该帮会关的表信息
	--得到帮会仓库id
	local tblDepotID = StmtOperater._GetTongDepotID:ExecSql('n', tong_id)
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local BuildingDB = RequireDbBox("GasTongbuildingDB")
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local sceneType, sceneId, serverId, warZoneId, stationId = WarZoneDB.GetTongStationInfo({["tongId"] = tong_id})
	BuildingDB.ClearTongBuilding(tong_id)
	if sceneType == 6 then
		SceneMgrDB.DeleteTongScene(tong_id)
	elseif sceneType == 7 then
		WarZoneDB.ClearTongStationInfo(tong_id)
	end
	local argTbl = {}
	argTbl.TongId = tong_id
	argTbl.SceneId = sceneId
	argTbl.SceneType = sceneType
	if sceneType == 7 then
		argTbl.WarZoneId = warZoneId
		argTbl.StationId = stationId
	end
	if serverId ~= 0 then
		Db2GasCall("DeleteOldStation", argTbl, serverId)
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local item_res = StmtOperater._SelectTongDepotItem:ExecStat(tblDepotID:GetData(0,0))
	if item_res:GetRowNum() > 0 then
		for i = 1,item_res:GetRowNum() do
			g_RoomMgr.DelItemFromStaticTable(item_res:GetData(i-1,0),tongMangerId,event_type_tbl["帮会解散删除物品"])
		end
	end
	--帮会基本信息
	StmtOperater._DelAllTongStaticInfo:ExecSql('',tong_id)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return
  end
	--删除仓库物品
	StmtOperater._DeleteTongDepotItem:ExecSql('',tblDepotID:GetData(0,0))
	--删除帮会仓库
	StmtOperater._DeleteTongDepot:ExecSql('',tblDepotID:GetData(0,0))
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return
  end
  local g_LogMgr = RequireDbBox("LogMgrDB")
  g_LogMgr.BreakTongLog(tong_id,tongMangerId)
  return true
end



function  CTongBasicBox.HaveSendRequestJoinTong(nCharID,tong_id)
	local res = StmtOperater._GetRequestInfoByPlayerID:ExecStat(nCharID,g_TongMgr.m_nRequstTimeLimit,tong_id)
	
	return res:GetNumber(0,0) > 0
end

function CTongBasicBox.CanEnterWarZone(parameter)
	local uPlayerID = parameter["uPlayerID"]
	if uPlayerID == 0 then
		return false, 9121
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
 	if CharacterMediatorDB.GetPlayerLevel(uPlayerID) < g_TongMgr:GetJoinInLevel() then
 		return false, 9029
 	end
 	return true
end

--【判断是否可以引荐或者招募玩家】
function  CTongBasicBox.CanRecommendOrInvit(uTongID,uPlayerID,target_id)
	
	if target_id == 0 then
		--该玩家不存在
		return 9121
	end
	local ex = RequireDbBox("Exchanger")
	local target_name = ex.getPlayerNameById(target_id)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
 	if CharacterMediatorDB.GetPlayerLevel(target_id) < g_TongMgr:GetJoinInLevel() then
 		--等级不够
 		return 9200,target_name
 	end
	
 	if CTongBasicBox.GetCampByCharID(target_id) ~= CTongBasicBox.GetTongCampByID(uTongID) then
 		--阵营不同
 		return 9314,target_name
 	end
 	
	if not CTongBasicBox.RequestBeFull(uTongID) then
  	--申请信息已满
  	return 9328
  end
  if not CTongBasicBox.MemberBeFull(uTongID) then
  	--该帮会人数已满
  	return 9328
  end
	
	if CTongBasicBox.GetTongID(target_id) ~= 0 then
		--对方已经加入帮会
		return 9315,target_name
	end
	
	if  not (CTongBasicBox.JudgeLeaveTongTime(target_id) == 1)  then
		--target_name .. "退出帮会不到24小时，不能再加入任何帮会"
		return 9316,target_name
	end
	return true
end

--【是否可以邀请】
function  CTongBasicBox.CanInviteJoinTong(uTongID,uPlayerID,target_id)
	if target_id == 0 then
		--该玩家不存在
		return 9121
	end
	local ex = RequireDbBox("Exchanger")
	local target_name = ex.getPlayerNameById(target_id)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
 	if CharacterMediatorDB.GetPlayerLevel(target_id) < g_TongMgr:GetJoinInLevel() then
 		--等级不够
 		return 9317,target_name
 	end
	
 	if CTongBasicBox.GetCampByCharID(target_id) ~= CTongBasicBox.GetTongCampByID(uTongID) then
 		--阵营不同
 		return 9318,target_name
 	end
 	
	if not CTongBasicBox.RequestBeFull(uTongID) then
  	--申请信息已满
  	return 9330
  end
  if not CTongBasicBox.MemberBeFull(uTongID) then
  	--该帮会人数已满
  	return 9330
  end
	
	if CTongBasicBox.GetTongID(target_id) ~= 0 then
		--对方已经加入帮会
		return 9319,target_name
	end
	
	if  not (CTongBasicBox.JudgeLeaveTongTime(target_id) == 1)  then
		--target_name .. "退出帮会不到24小时，不能再加入任何帮会"
		return 9320,target_name
	end
	return true
end
---------------------------------------------RPC函数调用相关-------------------------------------------------------

--【申请创建帮会】
function CTongBasicBox.RequestCreateTong(parameters)
	local uPlayerID = parameters.uPlayerID
	if CTongBasicBox.GetTongID(uPlayerID) > 0 then
		return 9009
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
 	if CharacterMediatorDB.GetPlayerLevel(uPlayerID) < g_TongMgr:GetRegistLevel() then
 		--等级不够
 		return 9004
 	end
	if  not (CTongBasicBox.JudgeLeaveTongTime(uPlayerID) == 1)  then
		--MsgToConn(Conn, 9005,"离开帮会24小时内不能再创建或加入任何帮会")
		return 9102
	end
	--判断金钱是否够用
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	if not MoneyManagerDB.EnoughMoneyByType(uPlayerID,2, g_TongMgr:GetRegistMoney()) then
		return 9003
  end
  return true
end

--【判断是否可以打开帮会改名窗口】
function CTongBasicBox.PreChangeTongName(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local uTongID,sName = CTongBasicBox.GetTongID(uPlayerID)
	--判断是否有修改权限
	if not CTongBasicBox.JudgePurview(uPlayerID, "UpdateTongName") then
  	return 9006
  end
  local nPos = string.find(sName,"&")
  if not nPos then
  	--说明不是合服改名帮会
  	return 8999
  end
  return true
end

--【帮会改名】
function CTongBasicBox.ChangeTongName(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local sTongName = parameters["sTongName"]
	local uTongID,sName = CTongBasicBox.GetTongID(uPlayerID)
	--判断是否有修改权限
	if not CTongBasicBox.JudgePurview(uPlayerID, "UpdateTongName") then
  	return 9006
  end
  local nPos = string.find(sName,"&")
  if not nPos then
  	--说明不是合服改名帮会
  	return 8999
  end
	if( not textFilter:IsValidName(sTongName) ) then
		return 9198
	end
	if string.len(sTongName) == 0 then
		return 9176
	end
	if string.len(sTongName) > 32 then
		return 9177
	end
	
  --判断帮会名称是否已经存在
  if CTongBasicBox.CountTongByName(sTongName) ~= 0 then
    return 9001
  end
  
  StmtOperater._UpdateTongName:ExecSql('',sTongName,uTongID)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return 9000
  end
  --添加日志信息
  local ex = RequireDbBox("Exchanger")
  local player_name = ex.getPlayerNameById(uPlayerID)
  local sMsg = "2009_" .. player_name .. "," .. sTongName
  if not CTongBasicBox.AddTongLogs(uTongID,sMsg, g_TongMgr:GetLogType("人员")) then
   	 return 9107
  end
  return true,uTongID
end

--【创建帮会】
function CTongBasicBox.CreateTong(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local sTongName = parameters["sTongName"]
	local sTongPorpose = parameters["sTongPorpose"]
	local tong_camp	 =  parameters["tong_camp"]
	if CTongBasicBox.GetTongID(uPlayerID) > 0 then
		return 9009
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local uLevel = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
 	if uLevel < g_TongMgr:GetRegistLevel() then
 		--等级不够
 		return 9004
 	end
	if  not (CTongBasicBox.JudgeLeaveTongTime(uPlayerID) == 1)  then
		--MsgToConn(Conn, 9005,"离开帮会24小时内不能再创建或加入任何帮会")
		return 9102
	end
	local bFlag, uTongID = CTongBasicBox.AddTong(uPlayerID, sTongName,sTongPorpose,tong_camp, uLevel)
	return bFlag, uTongID,-(g_TongMgr:GetRegistMoney())
end

--【获得帮会信息】
function CTongBasicBox.GetTongInfoDB(data)
	local uPlayerID = data.uPlayerID
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)
	return CTongBasicBox.GetTongInfo(uTongID)
end

--【获得申请信息】
function CTongBasicBox.GetRequestInfoRPC(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)
	local info,proffer_info = CTongBasicBox.GetRequestInfo(uTongID)
	return info,proffer_info
end

--【获得人员信息】
function CTongBasicBox.GetTongMemberByTongID(uTongId)
	local info,uMaxNum = CTongBasicBox.GetTongMemberInfo(uTongId)
	local exploit_info = {}
	for i =1,info:GetRowNum() do
		local res_exploit = StmtOperater._GetMemberExploit:ExecStat(info(i,1))
		if res_exploit:GetRowNum() > 0 then
			exploit_info[info(i,1)] = res_exploit(1,1)
		end
	end
	return info,uMaxNum,exploit_info 
end
function CTongBasicBox.GetTongMember(parameters)
	local uPlayerID = parameters["uPlayerID"]
	return CTongBasicBox.GetTongMemberByTongID(CTongBasicBox.GetTongID(uPlayerID))
end

--【根据玩家id获得帮会等级】
function CTongBasicBox.GetTongLevelByPlayerID(data)
	local uPlayerID = data["uPlayerID"]
	local tong_id = CTongBasicBox.GetTongID(uPlayerID)
	local nLevel = CTongBasicBox.GetTongLevel(tong_id)   
  local WarZoneDB = RequireDbBox("WarZoneDB")
	local stationLine = WarZoneDB.GetTongRelativeLine(tong_id)
	
	return nLevel, stationLine
end

--【获得日志信息】
function CTongBasicBox.GetLogsInfoRPC(parameters)
	local uLogType = parameters["uLogType"]
	local uPlayerID = parameters["uPlayerID"]
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)

	local info = CTongBasicBox.GetLogsInfo(uTongID,uLogType)
	return info
end

--【修改帮会宗旨】
function CTongBasicBox.ChangeTongPurpose(parameters)
	local purpose = parameters["purpose"]
	local uPlayerID = parameters["uPlayerID"]
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)

	--判断是否有修改权限
	if not CTongBasicBox.JudgePurview(uPlayerID, "UpdatePurpose") then
  	return	false, 9006
  end
  if not CTongBasicBox.UpdateTongPurpose(purpose,uTongID) then
  	return
  end
  
  local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
  local sMsg = "2028_" .. player_name
  if not CTongBasicBox.AddTongLogs(uTongID,sMsg, g_TongMgr:GetLogType("内政")) then
  	return false,9000
  end
  return true, 9108
end

--【获得帮贡】
function CTongBasicBox.GetProffer(data)
	local uPlayerID = data.uPlayerID
	
	local result = StmtOperater._GetProffer:ExecStat(uPlayerID)
	if result:GetRowNum() == 0 then 
		return 0
	end
	return result(1,1)
end

--【增加国贡】
function CTongBasicBox.AddProffer(data)
	local uPlayerID = data.uPlayerID
	local nAddProffer = data.nAddProffer
	local result = StmtOperater._GetProffer:ExecStat(uPlayerID)
	if result:GetRowNum() == 0 then 
		StmtOperater._InsertProffer:ExecStat(nAddProffer,uPlayerID)
	else
		StmtOperater._AddProffer:ExecStat(nAddProffer,uPlayerID)
	end
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		CancelTran()
  		return
	end
	if nAddProffer < 0 then
		local uTongID = CTongBasicBox.GetTongID(uPlayerID)
		if uTongID and uTongID > 0 then
			StmtOperater._AddExpendProffer:ExecStat(-nAddProffer,uPlayerID)
			StmtOperater._AddTongProffer:ExecStat(-nAddProffer,uPlayerID)
			if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
				CancelTran()
				return
			end
		end
	end
	local TempRes = StmtOperater._GetProffer:ExecStat(uPlayerID)
	Db2CallBack("ReturnProfferPoint",uPlayerID,TempRes:GetData(0,0))
	local TeamProffer = CTongBasicBox.GetTongProfferByCharID(uPlayerID)	
	Db2CallBack("ReturnTeamProfferPoint",uPlayerID,TeamProffer)
	return true, nAddProffer
end

local StmtDef = {
	"_GetPlayerCurLevel",
	[[
		select cb_uLevel from tbl_char_basic where cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)
--学习佣兵团科技时，只消耗玩家的国贡，不增加贡献的国贡和队贡，不要和上面的接口混淆了
function CTongBasicBox.ConsumeProffer(charID, consumedTongProffer, SceneName, uEventId, modifyType)
    local data = { }
    data.uPlayerID =charID
    local haveTongProffer = CTongBasicBox.GetProffer(data)
    if haveTongProffer + consumedTongProffer <=0 then
        CancelTran()
        return false 
    end
    
    StmtOperater._AddProffer:ExecStat(consumedTongProffer,charID)
	if g_DbChannelMgr:LastAffectedRowNum() == 0  then
  		CancelTran()
  		return false 
	end
	
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
    local uTotalPoint = AreaFbPointDB.GetPointStatement(charID,10)
	local level_res = StmtOperater._GetPlayerCurLevel:ExecStat(charID)
	local LogMgr = RequireDbBox("LogMgrDB")
	local succ = LogMgr.SaveJiFenPointInfo(charID,consumedTongProffer,10,SceneName,uTotalPoint,uEventId,modifyType,level_res:GetData(0,0))
	if not succ then
		error("记录修改积分log失败")
	end
	return true
end


local StmtDef = {
    "_ConsumeTongProffer",
    [[
        update tbl_member_tong set mt_uTongProffer  = mt_uTongProffer + ? where cs_uId = ?
        and mt_uTongProffer > 0 
    ]]
}DefineSql ( StmtDef, StmtOperater )

function CTongBasicBox.ConsumeTongProffer(charID, consumedProfferCount)
    local haveTongProfferCount = CTongBasicBox.GetTongProfferByCharID(charID)
    if haveTongProfferCount + consumedProfferCount <=0 then
        CancelTran()
        return false, 9072 
    end
    StmtOperater._ConsumeTongProffer:ExecStat(consumedProfferCount, charID)
    if g_DbChannelMgr:LastAffectedRowNum() == 0 then
        CancelTran()
        return false, 9073
    end
    return true
end


--【增加帮贡】
function CTongBasicBox.AddProfferPoint(data)
	if table.getn(data) <= 0 then
		return 
	end
	for _, charInfo in pairs(data) do
		local uCharId			= charInfo[1]
		local Point				= charInfo[2]
		local SceneName		= charInfo[3]
		local modifyType	= charInfo[4]
		local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
		AreaFbPointDB.AddAreaFbPointByType(uCharId,Point,10,SceneName,nil,modifyType)
	end
end

local function OutSceneByPos(charId, pos)
	if pos < 2 then
		local chardb = RequireDbBox("CharacterMediatorDB")
		local data = {}
		data["charId"] = charId
		local sceneType = chardb.PlayerOnFbPos(data)
		if sceneType and sceneType == 26 or sceneType == 8 then
			return false, charId
		end
		return true
	end
	return true
end

--【修改职位】
function CTongBasicBox.UpdatePos(parameters)
	local target_id = parameters["target_id"]
	local old_position = parameters["old_position"]
	local new_position = parameters["new_position"]
	local uPlayerID = parameters["uPlayerID"]
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)
	--判断是否有修改权限
	if not CTongBasicBox.JudgePurview(uPlayerID, "UpdatePos") then
  	return	false, 9006
  end
  local player_pos = CTongBasicBox.GetPositionById(uPlayerID)
  local target_pos = CTongBasicBox.GetPositionById(target_id)
  if target_pos >= player_pos then
  	return false, 9006
  end
  local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
  local target_level = CharacterMediatorDB.GetPlayerLevel(target_id)
  if new_position == g_TongMgr:GetPosIndexByName("团长") and player_pos ~= g_TongMgr:GetPosIndexByName("团长") then
  	return false, 9006
	end
	if ((player_pos < new_position) or (player_pos == new_position and player_pos ~= g_TongMgr:GetPosIndexByName("团长"))) then
		return false,9006
	end
	if new_position == g_TongMgr:GetPosIndexByName("团长") and target_level < 20 then
		return false, 9061
	end
	if new_position == g_TongMgr:GetPosIndexByName("副团长") and target_level < 20 then
		return false, 9062
	end
	local upper_total = g_TongMgr:GetPosUpperTotal(new_position)
--	if new_position == g_TongMgr:GetPosIndexByName("副团长") then
--		local nLevel = CTongBasicBox.GetTongLevel(uTongID)
--		upper_total = upper_total + nLevel - 1
--	end
  local pos_count = CTongBasicBox.GetMemberByPos(uTongID,new_position)
  if not upper_total then
  	return false
  end
  if upper_total ~= -1 and upper_total <= pos_count and 
  	new_position ~= g_TongMgr:GetPosIndexByName("团长") then
  	return false,9179			--该职位人数满了
  end
  
  local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
	local uArmyCorpsId = ArmyCorps_box.GetArmyCorpsIdByPlayerId(uPlayerID)
	if uArmyCorpsId ~= 0 then
		local uArmyCorps_Pos = ArmyCorps_box.GetPositionById(uPlayerID)
   	if uArmyCorps_Pos == g_ArmyCorpsMgr:GetPosIndexByName("团长") and
   		new_position == g_TongMgr:GetPosIndexByName("团长") then
   		return false,8539
  	end
	end
	
	StmtOperater._UpdatePosition:ExecSql('', new_position, target_id,old_position)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return false,9111
  end
  local posFlag, charId = OutSceneByPos(target_id, new_position)
  
  local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongMemberEventLog(uTongID,4,uPlayerID,target_id,new_position)

  if new_position == g_TongMgr:GetPosIndexByName("团长") and player_pos == g_TongMgr:GetPosIndexByName("团长") then
  	--团长转移团长
  	StmtOperater._UpdatePosition:ExecSql('', g_TongMgr:GetPosIndexByName("团员"), uPlayerID, g_TongMgr:GetPosIndexByName("团长"))
   	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	 	CancelTran()
  	 	return false, 9006
   	end
		g_LogMgr.TongMemberEventLog(uTongID,4,uPlayerID,uPlayerID,g_TongMgr:GetPosIndexByName("团员"))
  end

	local bFlag, uMsgID = CTongBasicBox.LogUpdateMemberPosition(uTongID, uPlayerID, target_id, new_position, old_position)
  return bFlag, uMsgID, posFlag, charId
end

--【卸任】
function CTongBasicBox.DismissPosDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)
	local player_pos = CTongBasicBox.GetPositionById(uPlayerID)
	--判断是否是帮主
  if player_pos == g_TongMgr:GetPosIndexByName("团长") then
		--"帮主不可以卸任！")
  	return false, 9160
  elseif player_pos == 0 then
  	return false
  else
  	--修改职位
  	StmtOperater._UpdatePosition:ExecSql('', g_TongMgr:GetPosIndexByName("团员"), uPlayerID, player_pos)
  	local g_LogMgr = RequireDbBox("LogMgrDB")
		g_LogMgr.TongMemberEventLog(uTongID,4,uPlayerID,uPlayerID,g_TongMgr:GetPosIndexByName("团员"))
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		CancelTran()
  		return false ,9161
  	end
  	local posFlag, charId = OutSceneByPos(uPlayerID, g_TongMgr:GetPosIndexByName("团员"))
  	return true, nil, posFlag, charId
	end
	local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
	local sMsg = "2015_" .. player_name
  if not CTongBasicBox.AddTongLogs(uTongID,sMsg, g_TongMgr:GetLogType("人员")) then
  	return false,9017
  end
end

--【剔除成员】
function CTongBasicBox.DropMemberRPC(parameters)
	local target_id = parameters["target_id"]
	local uPlayerID = parameters["uPlayerID"]
	local sceneName = parameters["sceneName"]
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)
	--判断是否有修改权限
	if not CTongBasicBox.JudgePurview(uPlayerID, "FireOutMember") then
  	return	9006
  end
  local player_pos = CTongBasicBox.GetPositionById(uPlayerID)
  local target_pos = CTongBasicBox.GetPositionById(target_id)
  if target_pos >= player_pos then
  	--权限不足
  	return 9006
  end
	local res = CTongBasicBox.DropMember(target_id,uTongID,sceneName)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongMemberEventLog(uTongID,2,uPlayerID,target_id,0)
  local ex = RequireDbBox("Exchanger")
	local target_name = ex.getPlayerNameById(target_id)
  return res,target_name,uTongID
end

local function DeleteInfoByLeave(tongId)
	local RobResource = RequireDbBox("GasTongRobResourceDB")
	local res = StmtOperater._CountTongMembers:ExecSql('n', tongId)
	if res:GetRowNum() > 0 then
		local num = res:GetData(0,0)
		if num <= 1 then
			local data = {}
			data["tongId"] = tongId
			local result = RobResource.SelectObjNameByTongId(data)
			if result then
				RobResource.DeleteInfoByTongId(data)
			end
		end
	end
end

--【退出帮会】
function CTongBasicBox.LeaveTong(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local sceneName = parameters["sceneName"]
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)
	local player_pos = CTongBasicBox.GetPositionById(uPlayerID)
	local res = CTongBasicBox.DropMember(uPlayerID, uTongID,sceneName)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongMemberEventLog(uTongID,3,uPlayerID,uPlayerID,0)
	DeleteInfoByLeave(uTongID)
  return res,uTongID
end

--【有权限玩家响应申请信息】
--ps：拒绝or接受
function CTongBasicBox.ResponseRequest(parameters)
	local target_id = parameters["target_id"]
	local bBlooen = parameters["bBlooen"]
	local uPlayerID = parameters["uPlayerID"]
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)
	--判断是否有权限
	if not CTongBasicBox.JudgePurview(uPlayerID, "InviteMember") then
  	return false, 9006
  end
  
  if not CTongBasicBox.DeleteRequestInfo(target_id) then
  	return false,9118
  end
  if bBlooen then
  	--批准加入
  		if CTongBasicBox.GetTongID(target_id) ~= 0 then
  			--对方已经有帮会了
  			return false, 9116
  		end
  		if CTongBasicBox.GetCampByCharID(target_id) ~= CTongBasicBox.GetTongCampByID(uTongID) then
 				--阵营不同，不能加入本帮会
 				return false,9174
 			end
 			if not CTongBasicBox.MemberBeFull(uTongID) then
  			--该帮会人数已满
  			return false,9119
  		end	
 			local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
 			if CharacterMediatorDB.GetPlayerLevel(target_id) < g_TongMgr:GetJoinInLevel() then
 				return false, 9193
 			end
  		if not CTongBasicBox.AcceptRequest(uPlayerID,target_id,uTongID) then
  			return false, 9117
		  end
  else
  	return false, 9376
  end
  local ex = RequireDbBox("Exchanger")
	local target_name = ex.getPlayerNameById(target_id)
  local GasTongRobResourceDB = RequireDbBox("GasTongRobResourceDB")
  local tempResult = GasTongRobResourceDB.SelectAllInfo()
  local tong_id,tong_name = CTongBasicBox.GetTongID(uPlayerID)
  local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
	local ArmyCorpsId,ArmyCorpsName = ArmyCorps_box.GetArmyCorpsIDAndNameDB(uPlayerID)
  if tempResult then
  	for i, tempTongId in pairs(tempResult) do
  		if tempTongId == tong_id then
  			
  			local data = {}
  			data["uPlayerID"] = uPlayerID
  			data["target_id"] = target_id
  			local joinActiondb = RequireDbBox("JoinActionDB")
  			joinActiondb.GetTeamById(data)
  			
  			--将player加入
  			local flag = true
  			return true,9374,tong_id,tong_name,target_name,ArmyCorpsId,ArmyCorpsName,flag
  		end
  	end
  end
  return true,9374,tong_id,tong_name,target_name,ArmyCorpsId,ArmyCorpsName
end

--【申请加入帮会】
function CTongBasicBox.RequestJoinTongByName(parameters)
	local target_name = parameters["target_name"]
	local ex = RequireDbBox("Exchanger")
	local uTargetID = ex.getPlayerIdByName(target_name)
	if 0 == uTargetID then
		return false,9184
	end
	local uTongID = CTongBasicBox.GetTongID(uTargetID)
	parameters["tong_id"] = uTongID
	return CTongBasicBox.RequestJoinTong(parameters)
end

--【申请加入帮会】
function CTongBasicBox.RequestJoinTong(parameters)
	local tong_camp = parameters["tong_camp"]
	
	local uPlayerID = parameters["uPlayerID"]
	local tong_id = parameters["tong_id"]
	local uTongID = CTongBasicBox.GetTongID(uPlayerID)
	if 0 ~= uTongID then
		return false,9009
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
 	if CharacterMediatorDB.GetPlayerLevel(uPlayerID) < g_TongMgr:GetJoinInLevel() then
 		return false,9193
 	end
 	
 	if CTongBasicBox.HaveSendRequestJoinTong(uPlayerID,tong_id) then
		return false,9197
	end
	
	if CTongBasicBox.CountTongByID(tong_id) == 0 then
		--"该帮会不存在"
		return false,9170
	end
	
	if CTongBasicBox.GetTongCampByID(tong_id) ~= tong_camp then
		--“阵营不同，不能加入该帮会”
		return false, 9171
	end
	
	if not CTongBasicBox.RequestBeFull(tong_id) then
  	--该帮会申请人数已满
  	return false, 9329
  end
  if not CTongBasicBox.MemberBeFull(tong_id) then
  	--该帮会人数已满
  	return false,9329
  end	
  if  not (CTongBasicBox.JudgeLeaveTongTime(uPlayerID) == 1)  then
		--离开帮会24小时内不能再创建或加入任何帮会
		return false, 9324
	end
	
  if not CTongBasicBox.RequestJoinInTong(tong_id, uPlayerID) then
  	return false, 9120
  end
  
  local ViceCaptainIdRet = StmtOperater._GetMemberByPosition:ExecStat(4,tong_id)
  local CaptainIdRet = StmtOperater._GetMemberByPosition:ExecStat(5,tong_id)
  return true, 9113, ViceCaptainIdRet, CaptainIdRet
end

--【推荐】
function CTongBasicBox.RecommendJoinTong(parameters)
	local target_name = parameters["target_name"]
	local sInfor = parameters["sInfor"]
	local uPlayerID = parameters["uPlayerID"]
	local ex = RequireDbBox("Exchanger")
	
	local uTongID,tong_name = CTongBasicBox.GetTongID(uPlayerID)
	local target_id = ex.getPlayerIdByName(target_name)
	if CTongBasicBox.HaveSendRequestJoinTong(target_id,uTongID) then
		return false,9195
	end
	local Ret,sMsg = CTongBasicBox.CanRecommendOrInvit(uTongID,uPlayerID,target_id)
	if IsNumber(Ret) then
		return false,Ret,sMsg
	end
	local LoginServerSql = RequireDbBox("LoginServerDB")
	if not LoginServerSql.IsPlayerOnLine(target_id) then
		return false,9184
	end
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	if not RevertValidateBox.AddValidateInfo(uPlayerID,target_id,RevertValidateMgr:GetFunIndexByStr("RecommendJoinTong")) then
		return false
	end
  return true,target_id,tong_name
end

--【响应推荐】
function CTongBasicBox.ResponseRecommendJoinTong(parameters)
	local inviterId = parameters["inviterId"]
	local uPlayerID = parameters["uPlayerID"]
	local uTongID = CTongBasicBox.GetTongID(inviterId)
	if uTongID == 0 then
		--外挂，对方没有加入帮会
		return
	end
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	if not RevertValidateBox.DelValidateInfo(inviterId,uPlayerID,RevertValidateMgr:GetFunIndexByStr("RecommendJoinTong")) then
		return 
	end
	if CTongBasicBox.GetTongID(uPlayerID) > 0  then
		return 9009
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
 	if CharacterMediatorDB.GetPlayerLevel(uPlayerID) < g_TongMgr:GetJoinInLevel() then
 		--等级不够
 		return 9193
 	end
	if  not (CTongBasicBox.JudgeLeaveTongTime(uPlayerID) == 1)  then
		--"退出帮会不到24小时，不能再加入任何帮会"
		return  9102
	end
	if not CTongBasicBox.RequestJoinInTong(uTongID,uPlayerID) then
  	return 9120
  end
  local ex = RequireDbBox("Exchanger")
	local inviter_name = ex.getPlayerNameById(inviterId)
  return true,inviter_name
end

--------------------------------
--【招募】
function CTongBasicBox.InviteJoinTongDB(parameters)
	local target_id = parameters["target_id"]
	local uPlayerID = parameters["uPlayerID"]
	local uTongID,tong_name = CTongBasicBox.GetTongID(uPlayerID)
	--判断是否有权限
	if not CTongBasicBox.JudgePurview(uPlayerID, "Recommend") then
  	return false,9066
  end
  local LoginServerSql = RequireDbBox("LoginServerDB")
	if not LoginServerSql.IsPlayerOnLine(target_id) then
		return false,9184
	end
	local Ret,sMsg = CTongBasicBox.CanInviteJoinTong(uTongID,uPlayerID,target_id)
	if IsNumber(Ret) then
		return false,Ret,sMsg
	end
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	if not RevertValidateBox.AddValidateInfo(uPlayerID,target_id,RevertValidateMgr:GetFunIndexByStr("InviteJoinTong")) then
		return 
	end
  return true,tong_name
end

function CTongBasicBox.InviteJoinTongByNameDB(parameters)
	local target_name = parameters["target_name"]
	local ex = RequireDbBox("Exchanger")
	local uTargetID = ex.getPlayerIdByName(target_name)
	if 0 == uTargetID then
		return false,9184
	end
	local data = {}
	data["target_id"] = uTargetID
	data["uPlayerID"] = parameters["uPlayerID"]
	local bFlag,uMsgID,sMsg = CTongBasicBox.InviteJoinTongDB(data)
	return bFlag,uMsgID,sMsg,uTargetID
end

--------------------------------
--【响应招募】
function CTongBasicBox.ResponseRecruitJoinTongDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local inviter_id = parameters["inviter_id"]
	local tong_id,tong_name  = CTongBasicBox.GetTongID(inviter_id) 
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	if not RevertValidateBox.DelValidateInfo(inviter_id,uPlayerID,RevertValidateMgr:GetFunIndexByStr("InviteJoinTong")) then
		return
	end
	if not CTongBasicBox.RequestBeFull(tong_id) then
  	--帮会申请人数已满
  	return false, 9119
  end
	if not CTongBasicBox.MemberBeFull(tong_id) then
  	--该帮会人数已满
  	return false,9119
  end	
	if CTongBasicBox.GetTongID(uPlayerID) ~= 0 then
		--已经加入帮会
		return 
	end
		
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
 	if CharacterMediatorDB.GetPlayerLevel(uPlayerID) < g_TongMgr:GetJoinInLevel() then
 		--等级不够
 		return false,9193
 	end
	

	if  not (CTongBasicBox.JudgeLeaveTongTime(uPlayerID) == 1)  then
		--"退出帮会不到24小时，不能再加入任何帮会"
		return false, 9102
	end
	if tong_id == 0 then
		return
	else
		--把对方加入帮会人员表
  	if not CTongBasicBox.AddTongMember(inviter_id, uPlayerID, tong_id, "团员", g_TongMgr:GetPosIndexByName("团员")) then
  		return
  	end
  	CTongBasicBox.DeleteRequestInfo(uPlayerID)
  	local ex = RequireDbBox("Exchanger")
  	local player_name = ex.getPlayerNameById(uPlayerID)
  	local sMsg = "2016_" .. player_name
  	if not CTongBasicBox.AddTongLogs(tong_id, sMsg,g_TongMgr:GetLogType("人员")) then
  		return
  	end
	end
	local tong_name = CTongBasicBox.GetTongNameById(tong_id)
	local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
	local ArmyCorpsId,ArmyCorpsName = ArmyCorps_box.GetArmyCorpsIDAndNameDB(uPlayerID)
  return true,tong_name,tong_id,tong_name,ArmyCorpsId,ArmyCorpsName
end

-----------------------------------------------------------------
--【删除角色时对帮会信息的处理】
local StmtDef = {
    	"_GetLastLoginMember",
    	[[ 
    		select MT.cs_uId from tbl_member_tong as MT,tbl_char_onlinetime as ChrO
    				where MT.cs_uId = ChrO.cs_uId
    				and 	MT.mt_sPosition != ?
    				and 	MT.t_uId = ?
    				order by MT.mt_sPosition DESC,ChrO.co_dtLastLoginTime DESC
    				limit 1
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongCap",
    	[[ 
    		select member.cs_uId,chr.c_sName from  tbl_member_tong as member,tbl_char as chr 
    		where member.cs_uId = chr.cs_uId
    		and		member.mt_sPosition = ?
    		and		member.t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【修改职位】
local StmtDef = {
    	"_UpdateCapPos",
    	[[ 
    		update tbl_member_tong set mt_sPosition = ? where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


function CTongBasicBox.GetLastLoginMember(tong_id)
	local tbl = StmtOperater._GetLastLoginMember:ExecSql('n',g_TongMgr:GetPosIndexByName("团长"),tong_id)
	if tbl:GetRowNum() == 0 then
		return 0
	end
	return tbl:GetData(0,0)
end

function CTongBasicBox.DealWithTongInfoWhenDelChar(nCharID)
	local tong_id =  CTongBasicBox.GetTongID(nCharID) 
	local uArmyCorpsNewAdminId = 0
	local uTongNewLeaderId = 0
	if tong_id and tong_id > 0  then
		local pos = CTongBasicBox.GetPositionById(nCharID)
	
		local ex = RequireDbBox("Exchanger")
		local char_name = ex.getPlayerNameById(nCharID)
		local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
		local uArmyCorpsId = ArmyCorps_box.GetArmyCorpsIdByPlayerId(nCharID)
		local uTime = 36*60*60
		local bFlag = false
		if pos == g_TongMgr:GetPosIndexByName("团长") then
			--如果在佣兵团并且是团长,需要将团长职位转让给新的队长
			if uArmyCorpsId ~= 0 then
				local pos_index = ArmyCorps_box.GetPositionById(nCharID)
				if pos_index == g_ArmyCorpsMgr:GetPosIndexByName("团长") then
					uArmyCorpsNewAdminId = ArmyCorps_box.OnDealArmyCorpsAdminWhenDelChar(nCharID,uArmyCorpsId,uTime)
				end
			end
			--帮主删号，先转移职位
			--转移给职位最高且最近一次上线的人
			uTongNewLeaderId = CTongBasicBox.GetLastLoginMember(tong_id)
			if uTongNewLeaderId > 0 then
				StmtOperater._UpdateCapPos:ExecStat(g_TongMgr:GetPosIndexByName("团长"),uTongNewLeaderId)
				--添加帮会日志
				local player_name = ex.getPlayerNameById(uTongNewLeaderId)
				local sMsg = "2010_" .. player_name .. "," .. char_name
  			CTongBasicBox.AddTongLogs(tong_id,sMsg,g_TongMgr:GetLogType("人员"))
				bFlag = true
			end
		end
		local res = CTongBasicBox.DropMember(nCharID,tong_id,"删除角色")
		if res and not IsNumber(res) then
			local g_LogMgr = RequireDbBox("LogMgrDB")
			g_LogMgr.TongMemberEventLog(tong_id,2,0,nCharID,0)
		end
		CTongBasicBox.AddLeaveTongInfo(nCharID)
 	 	
 	 	local TongNameRet = StmtOperater._GetTongNameById:ExecStat(tong_id)
 	 	--上面的DropMember可能已经导致Tong被删除了,要验证下是否还存在
  	if (TongNameRet:GetRowNum() > 0) and not bFlag then
			local sMsg = "2008_" .. char_name
			CTongBasicBox.AddTongLogs(tong_id,sMsg,g_TongMgr:GetLogType("人员"))
		end
	end
	return uArmyCorpsNewAdminId, uTongNewLeaderId, tong_id
end


--【自动转移团长】
local StmtDef = {
    	"_GetAllTongCaptains",
    	[[ 
				select result1.t_uId,ifnull(result2.cs_uId,0),ifnull(result2.mt_sPosition,0),result1.cs_uId
				from
				(   		
						select TM.t_uId,TM.cs_uId 
		    		from  tbl_member_tong as TM,tbl_char_onlinetime  as CO,tbl_tong as TT
		    		where TM.cs_uId = CO.cs_uId
		    		and TM.t_uId = TT.t_uId
		    		and		TM.mt_sPosition = ?
		    		and 	(unix_timestamp(CO.co_dtLastLogOutTime) - unix_timestamp(CO.co_dtLastLogInTime)) >= 0
		    		and 	(unix_timestamp(now()) - unix_timestamp(CO.co_dtLastLogOutTime)) >= if((? +(TT.t_uLevel-1)*?)>168*3600,168*3600,(? +(TT.t_uLevel-1)*?))
				) as result1  		
				left join
				(    		
					select cs_uId,mt_sPosition,t_uId
					from 
					(	select TM.cs_uId,TM.mt_sPosition,TM.t_uId
											from  tbl_member_tong as TM,tbl_char_onlinetime  as CO,tbl_char_basic as TC,tbl_tong as TT
											where TM.cs_uId = CO.cs_uId
											and TM.t_uId = TT.t_uId
											and CO.cs_uId = TC.cs_uId
											and TC.cb_uLevel >= 20
											and 
											(
												(unix_timestamp(CO.co_dtLastLogOutTime) - unix_timestamp(CO.co_dtLastLogInTime)) < 0 or    		
												(unix_timestamp(now()) - unix_timestamp(CO.co_dtLastLogOutTime)) <= if((? +(TT.t_uLevel-1)*?)>168*3600,168*3600,(? +(TT.t_uLevel-1)*?))
											) order by TM.mt_sPosition desc)  as a 
					group by t_uId
				) as result2 
				on result1.t_uId = result2.t_uId where ifnull(result2.cs_uId,0) <> 0 and ifnull(result2.mt_sPosition,0) <> 0 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetNewCaptainID",
    	[[ 
    		select TM.cs_uId,TM.mt_sPosition
    		from  tbl_member_tong as TM,tbl_char_onlinetime  as CO,tbl_char_basic as TC
    		where TM.cs_uId = CO.cs_uId
    		and CO.cs_uId = TC.cs_uId
    		and TC.cb_uLevel >= 20
    		and TM.t_uId = ?
    		and 
    		(
    			(unix_timestamp(CO.co_dtLastLogOutTime) - unix_timestamp(CO.co_dtLastLogInTime)) < 0 or    		
					(unix_timestamp(now()) - unix_timestamp(CO.co_dtLastLogOutTime)) <= ?
    		)
    		order by TM.mt_sPosition DESC limit 1
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"GetNoCaptainTongId",
    	[[ 
    		select t_uId from tbl_tong where t_uId not in( select t_uId from tbl_member_tong where mt_sPosition = 5)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

function CTongBasicBox.AutoChangeCaptain(data)
	local uTime = data["uTime"]
	local Modulus = data["Modulus"]
	local tbl =  StmtOperater._GetAllTongCaptains:ExecStat(g_TongMgr:GetPosIndexByName("团长"),uTime,Modulus,uTime,Modulus,uTime,Modulus,uTime,Modulus) 
	local row = tbl:GetRowNum()

	if 0 == row then return end
	local ret = {}
	local uTime1 = 24*7*60*60
	local bTongRet = true
	for i=1,row do
		local uTongNewCap, uArmyCorpsNewAdmin, MemberTong
		local tong_id,new_cap_id,old_position,old_cap_id = tbl:GetNumber(i-1,0),tbl:GetNumber(i-1,1),tbl:GetNumber(i-1,2),tbl:GetNumber(i-1,3)
		--如果在佣兵团并且是团长,需要将团长职位转让其他佣兵小队队长
		local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
		local uArmyCorpsId = ArmyCorps_box.GetArmyCorpsIdByPlayerId(old_cap_id)
		if uArmyCorpsId ~= 0  then
			local pos_index = ArmyCorps_box.GetPositionById(old_cap_id)
			if pos_index == g_ArmyCorpsMgr:GetPosIndexByName("团长") then
				uArmyCorpsNewAdmin, MemberTong = ArmyCorps_box.OnDealArmyCorpsAdminWhenDelChar(old_cap_id,uArmyCorpsId,uTime1)
			end
		end
		local posFlag, charId = true, 0
		if new_cap_id ~= 0 then
			uTongNewCap = new_cap_id
			if old_position ~= g_TongMgr:GetPosIndexByName("团长") then
				StmtOperater._UpdatePosition:ExecSql('', g_TongMgr:GetPosIndexByName("团长"), new_cap_id,old_position)
				local g_LogMgr = RequireDbBox("LogMgrDB")
				g_LogMgr.TongMemberEventLog(tong_id,4,0,new_cap_id,g_TongMgr:GetPosIndexByName("团长"))
				
				if g_DbChannelMgr:LastAffectedRowNum() > 0 then
					StmtOperater._UpdatePosition:ExecSql('', g_TongMgr:GetPosIndexByName("团员"), old_cap_id,g_TongMgr:GetPosIndexByName("团长"))
					
					posFlag, charId = OutSceneByPos(old_cap_id, g_TongMgr:GetPosIndexByName("团员"))
					g_LogMgr.TongMemberEventLog(tong_id,4,0,old_cap_id,g_TongMgr:GetPosIndexByName("团员"))
					
					if g_DbChannelMgr:LastAffectedRowNum() > 0 then
						local ex = RequireDbBox("Exchanger")
						local new_cap_name = ex.getPlayerNameById(new_cap_id)
						local old_cap_name = ex.getPlayerNameById(old_cap_id)
						local nLevel = CTongBasicBox.GetTongLevel(tong_id)
						local nCount = 36+(nLevel-1)*12
						if nCount > 168 then
							nCount = 168
						end
						local sMsg = "2037_" .. old_cap_name .. "," .. nCount .. "," .. new_cap_name
						CTongBasicBox.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("人员"))
					else
						bTongRet = false
					end
				else
					bTongRet = false
				end
			end
		end
		if not bTongRet then
			CancelTran()
		end
		table.insert(ret,{uArmyCorpsNewAdmin, uTongNewCap, tong_id, MemberTong,old_cap_id, posFlag, charId})
	end
	return ret	
end

function CTongBasicBox.CheckAndChangeTongCaptain(data)
--	local uTime = data["uTime"]
--	local result =  StmtOperater.GetNoCaptainTongId:ExecStat() 
--	local row = result:GetRowNum()
--	if 0 ~= row then
--		for i=1,row do
--			local tong_id = result:GetNumber(i-1,0)
--			local tblNewCap = StmtOperater._GetNewCaptainID:ExecStat(uTime,tong_id) 
--			if tblNewCap:GetRowNum() > 0 then
--				local char_id,pos = tblNewCap:GetNumber(0,0),tblNewCap:GetNumber(0,1)
--				if pos ~= g_TongMgr:GetPosIndexByName("团长") then
--					StmtOperater._UpdatePosition:ExecSql('', g_TongMgr:GetPosIndexByName("团长"), char_id,pos)
--				end
--			end
--		end
--	end
	local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
	local uArmyCorpsId = ArmyCorps_box.CheckAndChangeArmyCorpsCaptain()
end
--【加帮会属性】
function CTongBasicBox.AddTongProperty(parameters)
	local uTongID = parameters["uTongID"]
	local uHonor = parameters["uHonor"]
	local uMoney = parameters["uMoney"]
	local uResource = parameters["uResource"]
	
	if not CTongBasicBox.UpdateHonor({["uHonor"] = uHonor,["uTongID"] = uTongID,["nEventType"] = event_type_tbl["佣兵小队GM指令加荣誉"]}) then
		return 
	end
	StmtOperater._AddTongProperty:ExecSql("", uMoney, uResource, uTongID)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
  	local g_LogMgr = RequireDbBox("LogMgrDB")
  	g_LogMgr.SaveTongMoneyInfo(uTongID,uMoney,event_type_tbl["佣兵小队GM指令加钱"])
  	g_LogMgr.SaveTongResourceInfo(uTongID,uResource,event_type_tbl["佣兵小队GM指令加资源"])
		return true
	else
		return false
	end
end


function CTongBasicBox.GetTongChallengeBattleList(parameters)
	local res = {}
	for index = 1, #parameters do
		local chlgTongId = parameters[index]["ChlgerTongId"]
		local tgtTongId = parameters[index]["TgtTongId"]
		local chlgTongName, tgtTongName
		local resChlg = StmtOperater._GetTongNameById:ExecSql("n", chlgTongId)
		if resChlg:GetRowNum() > 0 then
			chlgTongName = resChlg:GetData(0, 0)
		else
			chlgTongName = "无"
		end
		local resTgt = StmtOperater._GetTongNameById:ExecSql("n", tgtTongId)
		if resTgt:GetRowNum() > 0 then
			tgtTongName = resTgt:GetData(0, 0)
		else
			tgtTongName = "无"
		end
		resChlg:Release()
		resTgt:Release()
		local warZoneId = parameters[index]["WarZoneId"]
		local stationId = parameters[index]["StationId"]
		table.insert(res, {chlgTongName, tgtTongName, warZoneId, stationId})
	end
	return res
end

function CTongBasicBox.GetChlgerNameAndTgtName(parameters)
	local chlgTongId = parameters["ChlgerTongId"]
	local tgtTongId = parameters["TgtTongId"]
	local chlgTongName, tgtTongName
	local resChlg = StmtOperater._GetTongNameById:ExecSql("n", chlgTongId)
	if resChlg:GetRowNum() > 0 then
		chlgTongName = resChlg:GetData(0, 0)
	else
		return false
	end
	if tgtTongId == 0 then
		tgtTongName = ""
	else
		local resTgt = StmtOperater._GetTongNameById:ExecSql("n", tgtTongId)
		if resTgt:GetRowNum() > 0 then
			tgtTongName = resTgt:GetData(0, 0)
		else
			return false
		end
	end
	
	return true, chlgTongName, tgtTongName
end

function CTongBasicBox.GetTongChallengeBattleState(parameters)
	local nTongID = parameters["nTongID"]
	local battleState
	
	local resBattle = StmtOperater._GetTongChallengeBattleState:ExecStat(nTongID, nTongID)
	if resBattle:GetRowNum() > 0 then
		battleState = resBattle:GetData(0, 0)
	else
		return false
	end
	return true, battleState
end

function CTongBasicBox.GetChallengeBattleState(parameters)
	local chlgTongId = parameters["ChlgerTongId"]
	local tgtTongId = parameters["TgtTongId"]
	local battleState
	
	local resBattle = StmtOperater._GetChallengeBattleState:ExecStat(chlgTongId, tgtTongId)
	if resBattle:GetRowNum() > 0 then
		battleState = resBattle:GetData(0, 0)
	else
		return false
	end
	return true, battleState
end

function CTongBasicBox.GetTongMemberLevelInfo(parameters)
	local nTongID = parameters["nTongID"]
	local resultSet = StmtOperater.GetTongMemberLevelInfo:ExecStat(nTongID)
	local nResult
	local nRowNum = resultSet:GetRowNum()
	if nRowNum > 5 then
		return true, resultSet:GetData(4, 0)
	elseif nRowNum > 0 then
		return true, resultSet:GetData(nRowNum - 1, 0)
	else
		return false
	end
end

--function CTongBasicBox.GetTongMemberAverageLevel(parameters)
--	local nTongID = parameters["tongId"]
--	local memberNum = 0 
--	local totalLevel = 0 
--	local result = StmtOperater._CountTongMembers:ExecStat(nTongID)
--	if result:GetRowNum() > 0 then
--		memberNum = result:GetData(0,0)
--	end
--	local resultSet = StmtOperater.GetTongMemberLevelInfo:ExecStat(nTongID)
--	if resultSet:GetRowNum() > 0 then
--		for i = 0, resultSet:GetRowNum() - 1 do
--			totalLevel = totalLevel + resultSet:GetData(i,0)
--		end
--	end
--	if memberNum ~= 0 then
--		local averageLevel = totalLevel/memberNum
--		return averageLevel
--	end 
--end



function CTongBasicBox.GetTongMaxLevelRpc(parameters)
	local nTongID = parameters["nTongID"]
	local build_box = RequireDbBox("GasTongbuildingDB")
	local nMaxLevel = build_box.GetTongMaxLevel(nTongID)
	return nMaxLevel
end

function CTongBasicBox.GetPosAndMemMaxLev(data)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local res = GetAllMemByTongID(data.TongId)
	local param = {}
	--local MaxLev = 0
	if res and res:GetRowNum()>0 then
	--	for i=1, res:GetRowNum() do
	--		param["char_id"] = res(i,1)
	--		local Lev_Exp = CharacterMediatorDB.GetCharLevelExpDB(param)
	--		local DBLevel = Lev_Exp:GetData(0,0)
	--		if MaxLev < DBLevel then
	--			MaxLev = DBLevel
	--		end
	--	end
		return GetPositionById(data.CharId), GetTongLevel(data.TongId)
	end
end

function CTongBasicBox.GetTongLevelAndHonorById(data)
	local tong_id = data["tongId"]
	local level_info = StmtOperater._GetTongLevelAndHonor:ExecSql('nn',tong_id)
  if (level_info:GetRowNum() == 0) then
  	return 0,0
  end
  return level_info:GetNumber(0,0),level_info:GetNumber(0,1)
end

local StmtDef = {
    	"_CleanLeaveTongTime",
    	[[ 
    		delete from tbl_leave_tong where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
function CTongBasicBox.CleanLeaveTongTimeGM(data)
	StmtOperater._CleanLeaveTongTime:ExecStat(data["nCharID"])
end

function CTongBasicBox.UpdateTongBoss(data)
	local res = StmtOperater._GetTongBoss:ExecStat(data["TongId"])
	if res and res:GetRowNum()>0 then
		StmtOperater._UpdateTongBoss:ExecStat(data["AddNum"], data["TongId"])
	else
		StmtOperater._AddTongBoss:ExecStat(data["TongId"], data["AddNum"])
	end
end

function CTongBasicBox.UpdateTongCharBoss(data)
	local res = StmtOperater._GetTongCharBoss:ExecStat(data["CharId"])
	if res and res:GetRowNum()>0 then
		StmtOperater._UpdateTongCharBoss:ExecStat(data["AddNum"], data["CharId"])
	else
		StmtOperater._AddTongCharBoss:ExecStat(data["CharId"], data["AddNum"])
	end
end

function CTongBasicBox.UpdateTongType(tongId,uNewType,uOldType,uExecutorId)
	StmtOperater._UpdateTongType:ExecStat(uNewType,tongId)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongChangeTypeLog(tongId,uNewType,uOldType,uExecutorId)
end

function CTongBasicBox.GetTongType(TongId)
	local ret = StmtOperater._GetTongType:ExecStat(TongId)
	if ret:GetRowNum() == 0 then return 0 end
	return ret:GetData(0,0)
end

function CTongBasicBox.UpdateTongTypeChanged(data)
	StmtOperater._UpdateTongTypeChanged:ExecStat(data[1],data[2])
end

function CTongBasicBox.SelectTongTypeChanged(charId)
	local res = StmtOperater._SelectTongTypeChanged:ExecStat(charId)
	if res:GetRowNum() == 0 then
		return 0
	end
	return res(1,1)
end

--提升佣兵小队类型
function CTongBasicBox.UpTongType(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local uUpToType = parameters["uUpToType"]
	local uTongId = CTongBasicBox.GetTongID(uPlayerID)
	if uTongId == 0 then
		return false,false,9401
	end
	if not CTongBasicBox.JudgePurview(uPlayerID, "UpTongType") then
  	return false,false,9355
  end
	local NowTongType = CTongBasicBox.GetTongType(uTongId)
	if uUpToType ~= g_TongMgr:GetTongTypeByName("精英战斗") and 
		 uUpToType ~= g_TongMgr:GetTongTypeByName("精英生产") then
  	return false,false,9365
  end
  if NowTongType == g_TongMgr:GetTongTypeByName("普通") then
  	return false,false,9363
  end
  if NowTongType == g_TongMgr:GetTongTypeByName("精英战斗") or 
  	 NowTongType == g_TongMgr:GetTongTypeByName("精英生产") then
  	return false,false,9364
  end
  if uUpToType == g_TongMgr:GetTongTypeByName("精英战斗") and 
		NowTongType ~= g_TongMgr:GetTongTypeByName("战斗") then
		return false,false,9366
	end
	if uUpToType == g_TongMgr:GetTongTypeByName("精英生产") and 
		NowTongType ~= g_TongMgr:GetTongTypeByName("生产") then
		return false,false,9367
	end
	local now_level = CTongBasicBox.GetTongLevelAndHonor(uTongId)
	if uUpToType == g_TongMgr:GetTongTypeByName("精英生产") and now_level < 4 then
		return false,false,9367
	end
	local uTongExploit = CTongBasicBox.GetTongExploit(uTongId)
  if (not uTongExploit or uTongExploit < g_TongMgr:GetUpTongExploit()) and 
  	uUpToType == g_TongMgr:GetTongTypeByName("精英战斗") then
  	return false,false,9354
  end
  local data = {}
  data["tong_id"] = uTongId
	local uTongMoney	= CTongBasicBox.GetTongMoney(data)
	local uUpTongMoney = g_TongMgr:GetUpTongMoney()
	if uTongMoney < uUpTongMoney then
		return false,false,9356
  end
  
  --改变小队类型, 通知所有人
  local data = {}
  data["uTongID"] = uTongId
	data["uMoney"] 	= -uUpTongMoney
	data["nEventType"] = event_type_tbl["佣兵小队转类型扣钱"]
  local bRet = CTongBasicBox.UpdateTongMoney(data)
	if not bRet then
		CancelTran() 
		return false,false,9356
	end
	CTongBasicBox.UpdateTongType(uTongId,uUpToType,NowTongType,uPlayerID)
	
	local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
	local LogMsgId = uUpToType == g_TongMgr:GetTongTypeByName("精英战斗") and 2031 or 2032
  local sMsg = LogMsgId .. "_" .. player_name
  CTongBasicBox.AddTongLogs(uTongId,sMsg, g_TongMgr:GetLogType("内政"))
	
	local TongTypeChanged = CTongBasicBox.SelectTongTypeChanged(uTongId)
	if TongTypeChanged == 0 then
		CTongBasicBox.UpdateTongTypeChanged({uTongId,1})
		return true,true,CTongBasicBox.GetTongMember(parameters),CTongBasicBox.GetTongNameById(uTongId)
	end
  return true,false
end

--将为普通佣兵小队
function CTongBasicBox.DownTongType(uTongId)
	if uTongId == 0 then
		return false
	end
	local NowTongType = CTongBasicBox.GetTongType(uTongId)
	CTongBasicBox.UpdateTongType(uTongId,g_TongMgr:GetTongTypeByName("战斗"),NowTongType,0)
	
	local TongTypeChanged = CTongBasicBox.SelectTongTypeChanged(uTongId)
	if TongTypeChanged == 1 then
		CTongBasicBox.UpdateTongTypeChanged({uTongId,2})
	end
	return true,TongTypeChanged == 1,CTongBasicBox.GetTongMemberByTongID(uTongId)
end

function CTongBasicBox.GetTongBasicData(tong_id)
  	local info = {}
  	local tong_info = StmtOperater._SelectTongInfo:ExecStat(tong_id)
  	if tong_info:GetRowNum() == 0 then
  		return info
  	end
  	local leader_name = CTongBasicBox.GetTongLeaderName(tong_id)
  	info.tong_id = tong_id
		info.tong_type = tong_info:GetData(0,8)
  	info.tong_name = tong_info:GetData(0,0)
  	info.leader_name = leader_name --帮主
  	info.member_num = tong_info:GetData(0,5) --人数
  	info.tong_level = tong_info:GetData(0,7) --等级
  	info.tong_exploit = CTongBasicBox.GetTongExploit(tong_id) --声望
  	local WarZoneDB = RequireDbBox("WarZoneDB")
  	local line, warZoneId, stationId = WarZoneDB.GetTongStationRpc({["uTongId"]= tong_id})
  	if 0 == line then 
  		warZoneId, stationId = 0,0
  	end
  	info.warZoneId = warZoneId
  	info.stationId = stationId --位置(当前阵营、战线、驻地)
  	return info
end

function CTongBasicBox.GetTargetTongMemberInfo(parameters)
	local uTongID = parameters["uTargetTongId"]
	local info,uMaxNum = CTongBasicBox.GetTongMemberInfo(uTongID)
	local exploit_info = {}
	for i =1,info:GetRowNum() do
		local res_exploit = StmtOperater._GetMemberExploit:ExecStat(info(i,1))
		if res_exploit:GetRowNum() > 0 then
			exploit_info[info(i,1)] = res_exploit(1,1)
		end
	end
	return info,uMaxNum,exploit_info 
end

function CTongBasicBox.ChangeTongType(parameters)
	local uPlayerID =	parameters["uPlayerID"]
	local uTongType =	parameters["uTongType"]
	local tong_id = CTongBasicBox.GetTongID(uPlayerID)
	local now_level,nHonor = CTongBasicBox.GetTongLevelAndHonor(tong_id)
	if CTongBasicBox.GetPositionById(uPlayerID) ~= g_TongMgr:GetPosIndexByName("团长") then
		return false,9359
	end
	if now_level < 2 then
		return false,9360
	end
	if CTongBasicBox.GetTongType(tong_id) ~= g_TongMgr:GetTongTypeByName("普通") then
  	return false,9362
  end
	if uTongType <0 or uTongType > 2 then
		return false,9361
	end
	local NowTongType = CTongBasicBox.GetTongType(tong_id)
	CTongBasicBox.UpdateTongType(tong_id,uTongType,NowTongType,uPlayerID)
	
	local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
	local LogMsgId = uTongType == g_TongMgr:GetTongTypeByName("战斗") and 2029 or 2030
  local sMsg = LogMsgId .. "_" .. player_name
  CTongBasicBox.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("内政"))
	
	return true, 0, CTongBasicBox.GetTongMember(parameters)
end

function CTongBasicBox.ResetTongType(parameters)
	local uPlayerID =	parameters["uPlayerID"]
	local tong_id = CTongBasicBox.GetTongID(uPlayerID)
	if tong_id == 0 then
		return false
	end
	if CTongBasicBox.GetPositionById(uPlayerID) ~= g_TongMgr:GetPosIndexByName("团长") then
		return false,9370
	end
	local NowTongType = CTongBasicBox.GetTongType(tong_id)
	if NowTongType == g_TongMgr:GetTongTypeByName("普通") then
		return false,9371
	end

	CTongBasicBox.UpdateTongType(tong_id,g_TongMgr:GetTongTypeByName("普通"),NowTongType,uPlayerID)
	
	local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
  local sMsg = "2036_" .. player_name
  CTongBasicBox.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("内政"))
	return true
end

local StmtDef = {
    	"_GetTongInitiator",
    	[[ 
    		select t_uInitiatorId from tbl_tong where t_uId = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongMemberInitiators",
    	[[ 
    		select a.cs_uId
    		from tbl_member_tong a, tbl_tong b
    		where a.t_uId = ? and a.cs_uId = b.t_uInitiatorId
    	]]
}
DefineSql ( StmtDef, StmtOperater )

function CTongBasicBox.GetTongInitiator(TongId)
	local res = StmtOperater._GetTongInitiator:ExecStat(TongId)
	if res and res:GetRowNum()>0 then
		return res:GetData(0, 0)
	else
		return 0
	end
end

function CTongBasicBox.GetTongMemberInitiators(TongId)
	local res = StmtOperater._GetTongMemberInitiators:ExecStat(TongId)
	local MemberInitiators = {}
	if res and res:GetRowNum()>0 then
		for i=0, res:GetRowNum()-1 do
			table.insert(MemberInitiators, res:GetData(i, 0))
		end
	end
	return MemberInitiators
end

function CTongBasicBox.GetTongDevelopDegree()
	local res = StmtOperater._SelectTongDevelopDegreeById:ExecStat()
	return res(1,1)
end

function CTongBasicBox.AddTongDevelopDegree(data)
	local TongId = data["uTongID"]
	local Point = data["uPoint"]
	local event_type = data["uEventType"]
	local LogMgrDB = RequireDbBox("LogMgrDB")
	StmtOperater._AddTongDevelopDegree:ExecStat(Point, TongId)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		LogMgrDB.SaveTongDevelopDegreeInfo(TongId, Point, event_type)
		return true
	end
end

function CTongBasicBox.CalculateTongDevelopDegree()
	local res = StmtOperater._SelectTongDevelopDegreeInfo:ExecStat()
	local num = res:GetRowNum()
	for i=1,num do
		local tId = res(i,1)
		local tLevel = res(i,2)
		local tDevelopDegree = res(i,3)
		local tPermitUpLevel = res(i,4)
		local uUpLevelNeedDevelopDegree = TongUpLevelNeedDevelopDegree(tLevel, "DevelopDegree")
		if tDevelopDegree >= uUpLevelNeedDevelopDegree and tPermitUpLevel ~= 1 then
			CTongBasicBox.SetTongPermitUpLevel(tId, 1)
		elseif tDevelopDegree < uUpLevelNeedDevelopDegree and tPermitUpLevel ~= 0 then
			CTongBasicBox.SetTongPermitUpLevel(tId, 0)
		end
		CTongBasicBox.UpdateLevel(tId,event_type_tbl["佣兵小队发展度满足升级"])
	end
	StmtOperater._ResetTongDevelopDegree:ExecStat()
end

function CTongBasicBox.SendMailToTongCaptainDB()
	local query_list = StmtOperater._GetTongCaptainsId:ExecStat()
	local row = query_list:GetRowNum()
	if row == 0 then
		return
	end
	local CaptainsIdTbl = {}
	for i=0,row-1 do
		table.insert(CaptainsIdTbl,query_list:GetNumber(i,0))
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sysMailExecutor = RequireDbBox("SysMailDB")	
	for j=1,#CaptainsIdTbl do
		local params = {}
		params.m_nType = g_ItemInfoMgr:GetTongItemBigID()
		params.m_sName = "拉令"
		params.m_nBindingType = 3
		params.m_nCharID = CaptainsIdTbl[j]
		params.m_sCreateType = event_type_tbl["每周给队长发送奖励"]
		local count = 2
		local itemTbl = {}
		for k=1,count do
			local item = g_RoomMgr.CreateItem(params)
			table.insert(itemTbl,{item})
		end
		local sendRet = sysMailExecutor.SendSysMail(1031, CaptainsIdTbl[j], 8003, 8004, itemTbl, 0, event_type_tbl["每周给队长发送奖励"])
		if not sendRet then
			CancelTran()
			return
		end
	end
end

function CTongBasicBox.GetTongPermitUpLevel(TongId)
	local res = StmtOperater._GetTongPermitUpLevel:ExecStat(TongId)
	return res(1,1)
end

function CTongBasicBox.SetTongPermitUpLevel(TongId, bPermitUp)
	StmtOperater._SetTongPermitUpLevel:ExecStat(bPermitUp,TongId)
end

function CTongBasicBox.GetTongProfferByCharID(CharID)
	local res = StmtOperater._SelectTongProffer:ExecStat(CharID)
	if res:GetRowNum() == 0 then 
		return 0
	end
	return res(1,1)
end

SetDbLocalFuncType(CTongBasicBox.GetTongProfferByCharID)
SetDbLocalFuncType(CTongBasicBox.GetTongKillMonsCountInfo)
SetDbLocalFuncType(CTongBasicBox.GetTongMemberLevelInfo)
--------------------------------------
return CTongBasicBox
