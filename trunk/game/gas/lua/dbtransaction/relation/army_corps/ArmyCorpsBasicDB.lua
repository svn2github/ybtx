gac_gas_require "relation/army_corps/ArmyCorpsMgr"
gac_gas_require "relation/tong/TongMgr"
gac_gas_require "framework/common/CMoney"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"

local g_MoneyMgr = CMoney:new()
local g_ArmyCorpsMgr = g_ArmyCorpsMgr or CArmyCorpsMgr:new()
local g_TongMgr = g_TongMgr or CTongMgr:new()
local StmtOperater = {}	
local textFilter = CTextFilterMgr:new()
local LogErr = LogErr
local event_type_tbl = event_type_tbl
------------------------------------
local CArmyCorpsBasicBox = CreateDbBox(...)
----------------------------------SQL相关-------------------------------------------------
--【创建佣兵团】
local StmtDef = {
			"_AddArmyCorps",
			[[
				insert into tbl_army_corps(ac_sName,ac_uLevel,ac_uExp,ac_uMoney,ac_sPurpose) values(?,?,?,?,?)
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【删除佣兵团】
local StmtDef = {
			"_DelArmyCorps",
			[[
				delete from tbl_army_corps where ac_uId= ?
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【查询佣兵团基础信息】
local StmtDef = {
    	"_SelectArmyCorpsInfo",
    	[[
	    		select ac.ac_sName, ac.ac_uLevel, ac.ac_uMoney, ac.ac_sPurpose, tc.c_sName
    	  	from 	tbl_army_corps ac, tbl_army_corps_admin aca, tbl_char tc 
    	  	where ac.ac_uId = ? and aca.aca_uPurview = ? 
    	  	and aca.cs_uId = tc.cs_uId 
    	  	and ac.ac_uId = aca.ac_uId
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【查询佣兵团名字】
local StmtDef = {
    	"_SelectArmyCorpsName",
    	[[
	    		select ac_sName	from tbl_army_corps where ac_uId = ?
			]]
}
DefineSql ( StmtDef, StmtOperater )
--【更新佣兵团名字】
local StmtDef = 
{
	"_UpdateArmyCorpsName",
	[[
		update tbl_army_corps set ac_sName = ? where ac_uId = ?
	]]
}
DefineSql ( StmtDef, StmtOperater )

--【修改佣兵团宗旨】
local StmtDef = {
    	"_UpdatePurpose",
    	[[ update tbl_army_corps set ac_sPurpose = ? where ac_uId = ? ]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_CountArmyCorpsByName",
    	[[ select count(*) from tbl_army_corps  where ac_sName = ?  ]]
}    
DefineSql ( StmtDef, StmtOperater )

--【增加佣兵团成员信息】
local StmtDef = {
			"_AddArmyCorpsMember",
			[[
				insert into tbl_army_corps_member(t_uId,ac_uId) values(?,?)
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【查某佣兵团所有小队ID】
local StmtDef = {
			"_SelectArmyCorpsMember",
			[[
				select t_uId from tbl_army_corps_member where ac_uId = ?
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【查某佣兵团所有小队信息】
local StmtDef = {
			"_SelectArmyCorpsTeamInfo",
			[[
				select t_uId from tbl_army_corps_member where ac_uId = ?
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【查某小队所在佣兵团ID】
local StmtDef = {
			"_GetArmyCorpsByTongID",
			[[
				select ac_uId from tbl_army_corps_member where t_uId = ?
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【踢出某个佣兵团成员】
local StmtDef = {
			"_DelArmyCorpsMember",
			[[
				delete from tbl_army_corps_member where t_uId = ?
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【增加佣兵团管理者信息】
local StmtDef = {
			"_AddArmyCorpsAdmin",
			[[
				insert into tbl_army_corps_admin(cs_uId,ac_uId,aca_uPurview) values(?,?,?)
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【根据角色id得到职位】
local StmtDef = {
    	"_GetPosition",
    	[[ 
    		 select aca_uPurview  from tbl_army_corps_admin  where cs_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【修改佣兵团管理者信息】
local StmtDef = {
			"_UpdateArmyCorpsAdmin",
			[[
				replace into tbl_army_corps_admin(cs_uId, ac_uId, aca_uPurview)	values(?,?,?)
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【删除某个佣兵团管理者】
local StmtDef = {
			"_DelArmyCorpsAdmin",
			[[
				delete from tbl_army_corps_admin where cs_uId = ?
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【某个佣兵团某中管理者的个数】
local StmtDef = {
    	"_CountArmyCorpsAdminByPos",
    	[[ 
    		select count(*) from tbl_army_corps_admin  where ac_uId = ? and aca_uPurview = ?
    	]]
}  
DefineSql ( StmtDef, StmtOperater )

--【佣兵团内小队数量】
local StmtDef = {
    	"_GetTongCount",
    	[[ 
    		select count(*) from tbl_army_corps_member where ac_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetExploitByArmyCorps",
    	[[ 
    		select sum(te.te_uExploit) 
    		from 
    		tbl_army_corps_member as acm,tbl_member_tong as mt,tbl_tong_exploit as te
    		where 
    		acm.t_uId = mt.t_uId and te.cs_uId = mt.cs_uId and acm.ac_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
	"_GetHonorByArmyCorps",
	[[
		select sum(t.t_uHonor) 
		from tbl_tong as t, tbl_army_corps_member as acm
		where acm.t_uId = t.t_uId and acm.ac_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)


--【把退出佣兵团的佣兵小队加入退出佣兵团表】
local StmtDef = {
    	"_AddLeaveArmyCorps",
    	[[ 
    		insert into tbl_leave_army_corps  values(?,now()) 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelLeaveArmyCorps",
    	[[ 
    		delete from tbl_leave_army_corps where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【查询玩家离开佣兵团的时间】
local StmtDef = {
    	"_SelectLeaveArmyCorpsTime",
    	[[ 
    		select unix_timestamp(now())-unix_timestamp(lac_dtQuitTime) from tbl_leave_army_corps where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


--【查找上次登录的佣兵团内佣兵小队长】
local StmtDef = {
    	"_GetLastLoginLeader",
    	[[ 
    		select MT.cs_uId 
    		from tbl_member_tong as MT,tbl_char_onlinetime as ChrO,tbl_army_corps_member as ACM,tbl_char_basic as CB
    		where ACM.t_uId = MT.t_uId
    			and MT.cs_uId = ChrO.cs_uId
    			and ChrO.cs_uId = CB.cs_uId
    			and CB.cb_uLevel >= 40
    			and MT.mt_sPosition = ?
    			and ACM.ac_uId = ?
    			and MT.cs_uId != ?
    			and 
    				(
    					(unix_timestamp(ChrO.co_dtLastLogOutTime) - unix_timestamp(ChrO.co_dtLastLogInTime)) < 0 or    		
    					(unix_timestamp(now()) - unix_timestamp(ChrO.co_dtLastLogOutTime)) <= ?
    				)
    			order by ChrO.co_dtLastLoginTime DESC
    			limit 1
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【查找上次登录的佣兵团内佣兵小队长(有佣兵团职位的)】
local StmtDef = {
    	"_GetLastLoginLeaderHaveACMPos",
    	[[ 
    		select MT.cs_uId 
    		from tbl_member_tong as MT,tbl_char_onlinetime as ChrO,tbl_army_corps_member as ACM,tbl_army_corps_admin as ACA,tbl_char_basic as CB
    		where ACM.t_uId = MT.t_uId
    			and MT.cs_uId = ChrO.cs_uId
    			and MT.cs_uId = ACA.cs_uId
    			and ACA.cs_uId = CB.cs_uId
    			and CB.cb_uLevel >= 40
    			and MT.mt_sPosition = ?
    			and ACM.ac_uId = ?
    			and MT.cs_uId != ?
    			and 
    				(
    					(unix_timestamp(ChrO.co_dtLastLogOutTime) - unix_timestamp(ChrO.co_dtLastLogInTime)) < 0 or    		
    					(unix_timestamp(now()) - unix_timestamp(ChrO.co_dtLastLogOutTime)) <= ?
    				)
    			order by ChrO.co_dtLastLoginTime DESC
    			limit 1
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"GetNoCaptainArmyCorpsId",
    	[[ 
    		select ac_uId from tbl_army_corps where ac_uId not in( select ac_uId from tbl_army_corps_admin where aca_uPurview = 1)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

-----------------------------------Box实现-----------------------------------------------------
--【得到某佣兵团的基础信息】

function CArmyCorpsBasicBox.GetArmyCorpsBasicInfoDB(uArmyCorpsId)
	return StmtOperater._SelectArmyCorpsInfo:ExecStat(uArmyCorpsId,g_ArmyCorpsMgr:GetPosIndexByName("团长"))
end

function CArmyCorpsBasicBox.GetArmyCorpsInfoDB(data)
	local uPlayerID = data["uPlayerID"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongId = tong_box.GetTongID(uPlayerID)
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsByTongID(uTongId)
	if uArmyCorpsId == 0 then
		return false
	end
  local ArmyCorpsBaseInfo = CArmyCorpsBasicBox.GetArmyCorpsBasicInfoDB(uArmyCorpsId)
  --ArmyCorpsName, ArmyCorpsLevel, ArmyCorpsMoney, ArmyCorpsPurpose, CharName
  local ArmyCorpsInfo = {}
  if ArmyCorpsBaseInfo:GetRowNum() ~= 0 then
	  ArmyCorpsInfo.Name = ArmyCorpsBaseInfo:GetString(0,0)
	  ArmyCorpsInfo.Level = ArmyCorpsBaseInfo:GetData(0,1)
	  ArmyCorpsInfo.Money = ArmyCorpsBaseInfo:GetData(0,2)
	  ArmyCorpsInfo.Purpose = ArmyCorpsBaseInfo:GetData(0,3)
	  ArmyCorpsInfo.AdminName = ArmyCorpsBaseInfo:GetData(0,4)
	end
  ArmyCorpsInfo.MemberCount = CArmyCorpsBasicBox.GetArmyCorpsMemberCount(uArmyCorpsId)
  ArmyCorpsInfo.Exploit = CArmyCorpsBasicBox.GetArmyCorpsExploit(uArmyCorpsId)
  ArmyCorpsInfo.Honor = CArmyCorpsBasicBox.GetArmyCorpsHonor(uArmyCorpsId)
  ArmyCorpsBaseInfo:Release()
	return true,ArmyCorpsInfo
end

function CArmyCorpsBasicBox.GetArmyCorpsIDAndNameDB(uPlayerID)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongId = tong_box.GetTongID(uPlayerID)
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsByTongID(uTongId)
	if uArmyCorpsId ~= 0 then
	  local ArmyCorpsNameRet = StmtOperater._SelectArmyCorpsName:ExecStat(uArmyCorpsId)
	  if ArmyCorpsNameRet:GetRowNum() ~= 0 then
	  	return uArmyCorpsId,ArmyCorpsNameRet:GetString(0,0)
	  end
	end
  return uArmyCorpsId,""
end

function CArmyCorpsBasicBox.GetArmyCorpsMemberCount(uArmyCorpsId)
	 local TongCountRet = StmtOperater._GetTongCount:ExecStat(uArmyCorpsId)
	 local TongCount = TongCountRet:GetNumber(0,0)
	 TongCountRet:Release()
	 return TongCount
end

function CArmyCorpsBasicBox.GetArmyCorpsMember(uArmyCorpsId)
  --佣兵小队id
  local MemberRet = StmtOperater._SelectArmyCorpsMember:ExecStat(uArmyCorpsId)
  return MemberRet
end

function CArmyCorpsBasicBox.GetArmyCorpsMemberDB(parameters)
	return CArmyCorpsBasicBox.GetArmyCorpsMember(parameters["uArmyCorpsId"])
end

function CArmyCorpsBasicBox.GetArmyCorpsExploit(uArmyCorpsId)
  --声望佣兵小队之和
  local result = StmtOperater._GetExploitByArmyCorps:ExecStat(uArmyCorpsId)
	local uTotalExploit = 0
	if result:GetRowNum() ~= 0 then
		uTotalExploit = result(1,1)
	end
	result:Release()
	return uTotalExploit and uTotalExploit or 0
end

function CArmyCorpsBasicBox.GetArmyCorpsHonor(uArmyCorpsId)
	--荣誉佣兵小队之和
	local result = StmtOperater._GetHonorByArmyCorps:ExecStat(uArmyCorpsId)
	local uTotalHonor = 0
	if result:GetRowNum() ~= 0 then
		uTotalHonor = result(1,1)
	end
	result:Release()
	return uTotalHonor and uTotalHonor or 0
end

--【根据佣兵团名称获得团数量】
function CArmyCorpsBasicBox.CountArmyCorpsByName(sArmyCorps_idName)
	local tblArmyCorps = StmtOperater._CountArmyCorpsByName:ExecStat(sArmyCorps_idName)
	local CountArmyCorps = 0
	if 0 ~= tblArmyCorps:GetRowNum() then
		CountArmyCorps = tblArmyCorps:GetNumber(0,0)
	end
	tblArmyCorps:Release()
	return CountArmyCorps
end

--【得到某玩家在的职位】
function CArmyCorpsBasicBox.GetPositionById(player_id)
  local pos = StmtOperater._GetPosition:ExecStat(player_id)
  if pos:GetRowNum() == 0 then
  	pos:Release()
  	return 0
  end
  local position =  pos:GetData(0,0)
  pos:Release()
  return position
end

--【判断某人是否有某个权限】
function CArmyCorpsBasicBox.JudgePurview(player_id, purview_name)
	local pos_index = CArmyCorpsBasicBox.GetPositionById(player_id)
	return g_ArmyCorpsMgr:JudgePurview(pos_index,purview_name)
end

function CArmyCorpsBasicBox.GetArmyCorpsByTongID(uTongId)
	local armyCorpsIDRet = StmtOperater._GetArmyCorpsByTongID:ExecStat(uTongId)
	local ArmyCorpsID = 0
	if armyCorpsIDRet:GetRowNum() ~= 0 then
		ArmyCorpsID = armyCorpsIDRet:GetNumber(0,0)
	end
	armyCorpsIDRet:Release()
	return ArmyCorpsID
end

function CArmyCorpsBasicBox.GetArmyCorpsId(data)
	local uPlayerID = data["uTargetID"]
	return CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
end

function CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongId = tong_box.GetTongID(uPlayerID)
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsByTongID(uTongId)
	return uArmyCorpsId
end

function CArmyCorpsBasicBox.RequestCreateArmyCorpsDB(parameters)
	local uPlayerID = parameters.uPlayerID
	local tong_box = RequireDbBox("GasTongBasicDB")
	if tong_box.GetPositionById(uPlayerID) ~= g_TongMgr:GetPosIndexByName("团长") then
		return 8503
	end
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
	if uArmyCorpsId ~= 0 then
		return 8507
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
 	if CharacterMediatorDB.GetPlayerLevel(uPlayerID) < g_ArmyCorpsMgr:GetRegistLevel() then
 		--等级不够
 		return 8501
 	end
	--判断金钱是否够用
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	if not MoneyManagerDB.EnoughMoney(uPlayerID, g_ArmyCorpsMgr:GetRegistMoney()) then
		return 8500
  end
	if not (CArmyCorpsBasicBox.JudgeLeaveTongTime(tong_box.GetTongID(uPlayerID)) == 1) then
		return 8534
	end
  return true
end

function CArmyCorpsBasicBox.AddArmyCorpsStatic(uPlayerID,sArmyCorpsName,sArmyCorpsPorpose)
	StmtOperater._AddArmyCorps:ExecStat(sArmyCorpsName, 1, 0, 0, sArmyCorpsPorpose)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return false,8505
  end
	
	local uArmyCorpsId = g_DbChannelMgr:LastInsertId()
  local g_LogMgr = RequireDbBox("LogMgrDB")
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local uCamp = CharacterMediatorDB.GetCamp(uPlayerID)
	g_LogMgr.ArmyCorpsStaticInfoLog(uArmyCorpsId,sArmyCorpsName,uPlayerID,uCamp)

  local regist_money = g_ArmyCorpsMgr:GetRegistMoney()
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("ArmyCorps")
	local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["RegistArmyCorps"],uPlayerID, -regist_money,nil,event_type_tbl["创建佣兵团"])
	if not bFlag then
		CancelTran() 
		if IsNumber(uMsgID) then
			return false,uMsgID
		end
		return false,8505
	end
  
  return true,uArmyCorpsId
end

function CArmyCorpsBasicBox.AddArmyCorpsMember(uArmyCorpsID, uTongId, uPlayerID, uPosition)
	StmtOperater._AddArmyCorpsMember:ExecStat(uTongId, uArmyCorpsID)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	return false,8505
  end
  local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.ArmyCorpsMemberEventLog(uArmyCorpsID,1,uPlayerID,uTongId,uPosition)
  return true
end

function CArmyCorpsBasicBox.AddArmyCorpsAddmin(uPlayerID,uArmyCorpsID,sPosName)
  local uPosIndex = g_ArmyCorpsMgr:GetPosIndexByName(sPosName)
  StmtOperater._AddArmyCorpsAdmin:ExecStat(uPlayerID,uArmyCorpsID,uPosIndex)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	return false,8505
  end
  return true
end

function CArmyCorpsBasicBox.UpdateArmyCorpsAddmin(uTargetId,uArmyCorpsId,uChangeToPos,uExecutorId,uExecutorPos)
	StmtOperater._UpdateArmyCorpsAdmin:ExecStat(uTargetId,uArmyCorpsId,uChangeToPos)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	return false,8533
  end
  local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.ArmyCorpsMemberEventLog(uArmyCorpsId,3,uExecutorId,uTargetId,uChangeToPos)
  return true
end

function CArmyCorpsBasicBox.CreateArmyCorpsDB(parameters)
	local uRequestRet = CArmyCorpsBasicBox.RequestCreateArmyCorpsDB(parameters)
	if IsNumber(uRequestRet) then
		return false,uRequestRet
	end
	
	local uPlayerID = parameters.uPlayerID
	local sArmyCorpsName = parameters.sArmyCorpsName
	local sArmyCorpsPorpose = parameters.sArmyCorpsPorpose
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongId = tong_box.GetTongID(uPlayerID)
	local name_num = CArmyCorpsBasicBox.CountArmyCorpsByName(sArmyCorpsName)
	--判断佣兵团名称是否已经存在
	if name_num ~= 0 then
		return false,8504
	end
	--添加佣兵团静态信息
	local Ret,uArmyCorpsID = CArmyCorpsBasicBox.AddArmyCorpsStatic(uPlayerID,sArmyCorpsName,sArmyCorpsPorpose) 
	if not Ret then
		return false, uArmyCorpsID
	end
	--把自己小队加入佣兵团成员表
	local Ret, uMsgId = CArmyCorpsBasicBox.AddArmyCorpsMember(uArmyCorpsID, uTongId, uPlayerID, g_ArmyCorpsMgr:GetPosIndexByName("团长"))
	if not Ret then
		CancelTran()
		return false, uMsgId
	end
	Ret, uMsgId = CArmyCorpsBasicBox.AddArmyCorpsAddmin(uPlayerID,uArmyCorpsID,"团长")
	if not Ret then
		CancelTran()
		return false, uMsgId
	end
	return true,8506,-g_ArmyCorpsMgr:GetRegistMoney(),uArmyCorpsID
end

function CArmyCorpsBasicBox.ChangeArmyCorpsPurposeDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local sPurpose = parameters["sPurpose"]
	if CArmyCorpsBasicBox.JudgePurview(uPlayerID, "UpdatePurpose") then
		local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
		if uArmyCorpsId == 0 then
			return false,8509
		end
		StmtOperater._UpdatePurpose:ExecStat(sPurpose,uArmyCorpsId)
		return true
	else
		return false,8508
	end
end

--【判断是否可以打开佣兵团改名窗口】
function CArmyCorpsBasicBox.PreChangeArmyCorpsNameDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local uArmyCorpsId,sName = CArmyCorpsBasicBox.GetArmyCorpsIDAndNameDB(uPlayerID)
	--判断是否有修改权限
	if not CArmyCorpsBasicBox.JudgePurview(uPlayerID, "UpdateArmyCorpsName") then
  	return 8542
  end
  local nPos = string.find(sName,"&")
  if not nPos then
  	--说明不是合服改名佣兵团
  	return 8544
  end
  return true
end

--【佣兵团改名】
function CArmyCorpsBasicBox.ChangeArmyCorpsNameDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local sArmyCorpsName = parameters["sName"]
	local uArmyCorpsId,name = CArmyCorpsBasicBox.GetArmyCorpsIDAndNameDB(uPlayerID)
	--判断是否有修改权限
	if not CArmyCorpsBasicBox.JudgePurview(uPlayerID, "UpdateArmyCorpsName") then
  	return false,8542
  end
  local nPos = string.find(name,"&")
  if not nPos then
  	--说明不是合服改名佣兵团
  	return false,8544
  end
	if( not textFilter:IsValidName(sArmyCorpsName) ) then
		return false,8540
	end
	if string.len(sArmyCorpsName) == 0 then
		return false,8545
	end
	if string.len(sArmyCorpsName) > 32 then
		return false,9177
	end
	
  --判断帮会名称是否已经存在
  if CArmyCorpsBasicBox.CountArmyCorpsByName(sArmyCorpsName) ~= 0 then
    return false,8504
  end
  
  StmtOperater._UpdateArmyCorpsName:ExecSql('',sArmyCorpsName,uArmyCorpsId)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return false,9000
  else
  	local tong_box = RequireDbBox("GasTongBasicDB")
  	local uTongId = tong_box.GetTongID(uPlayerID)
  	return true,8546,uTongId,uArmyCorpsId
  end
end

function CArmyCorpsBasicBox.CanInviteJoinArmyCorpsByNameDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local sTargetName = parameters["sTargetName"]
	local ex = RequireDbBox("Exchanger")
	local uTargetID = ex.getPlayerIdByName(sTargetName)
	if 0 == uTargetID then
		return false,8512
	end
	
	local data = {}
	data["uPlayerID"] = uPlayerID
	data["uTargetID"] = uTargetID
	local uRet,uMsgId,sArmyCorpsName = CArmyCorpsBasicBox.CanInviteJoinArmyCorpsDB(data)
	return uRet,uMsgId,sArmyCorpsName,uTargetID
end

function CArmyCorpsBasicBox.CanInviteJoinArmyCorpsDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local uTargetId =parameters["uTargetID"]
	if CArmyCorpsBasicBox.JudgePurview(uPlayerID, "InviteJoin") then
		local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
		local MemberCount = CArmyCorpsBasicBox.GetArmyCorpsMemberCount(uArmyCorpsId)
		local MemberCountLimit = CArmyCorpsBasicBox.GetArmyCorpsMemMaxLimit(uArmyCorpsId)
		if MemberCount >= MemberCountLimit then
			return false,8517
		end
		local tong_box = RequireDbBox("GasTongBasicDB")
		local uTongId = tong_box.GetTongID(uTargetId)
		if uTongId == 0 then
			return false,8512
		end
		local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uTargetId)
		if uArmyCorpsId ~= 0 then
			return false,8511
		end
		if tong_box.GetPositionById(uTargetId) ~= g_TongMgr:GetPosIndexByName("团长") then
			return false,8513
		end
		if not (CArmyCorpsBasicBox.JudgeLeaveTongTime(uTongId) == 1) then
			return false, 8535
		end
	else
		return false,8510
	end
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
	local ArmyCorpsNameRet = StmtOperater._SelectArmyCorpsName:ExecStat(uArmyCorpsId)
	local sArmyCorpsName = ""
  if ArmyCorpsNameRet:GetRowNum() ~= 0 then
		sArmyCorpsName = ArmyCorpsNameRet:GetString(0,0)
	end
	return true,8514,sArmyCorpsName
end

function CArmyCorpsBasicBox.AddTongToArmyCorpsDB(parameters)
	local bFlag,uMsgId,sArmyCorpsName = CArmyCorpsBasicBox.CanInviteJoinArmyCorpsDB(parameters)
	if not bFlag then
		return bFlag,uMsgId
	end
	
	--将目标小队加入佣兵团
	local uPlayerID = parameters["uPlayerID"]
	local uTargetId =parameters["uTargetID"]
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongId = tong_box.GetTongID(uTargetId)
	local Ret, uMsgId = CArmyCorpsBasicBox.AddArmyCorpsMember(uArmyCorpsId, uTongId, uPlayerID, g_ArmyCorpsMgr:GetPosIndexByName("团员"))
	if not Ret then
		CancelTran()
		return false, uMsgId
	end
	local bFlag ,ArmyCorpsInfo = CArmyCorpsBasicBox.GetArmyCorpsInfoDB(parameters)
	if bFlag then
		return true, 0, uArmyCorpsId, ArmyCorpsInfo, uTongId
	end
	return false, 8514
end

function CArmyCorpsBasicBox.GetArmyCorpsMemMaxLimit(uArmyCorpsId)
--	local MemberCountLimit = 0
--	local ArmyCorpsBaseInfo = StmtOperater._SelectArmyCorpsInfo:ExecStat(uArmyCorpsId)
--  if ArmyCorpsBaseInfo:GetRowNum() ~= 0 then
--	  local Level = ArmyCorpsBaseInfo:GetData(0,1)
--	  MemberCountLimit = m_uInitTeamCountLimit + Level - 1
--	end
--	return MemberCountLimit
	return g_ArmyCorpsMgr:GetTongCountLimit()
end

function CArmyCorpsBasicBox.GetArmyCorpsTeamInfoDB(parameters)
	--小队类型,小队名称,队长,小队等级,小队人数,小队声望
	local uPlayerID = parameters["uPlayerID"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tbl_TongBasicData = {}
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
	if uArmyCorpsId == 0 then
		return false,0,0,tbl_TongBasicData
	end
	local MemberCount = CArmyCorpsBasicBox.GetArmyCorpsMemberCount(uArmyCorpsId)
	local MemberCountLimit = CArmyCorpsBasicBox.GetArmyCorpsMemMaxLimit(uArmyCorpsId)
	local MemberRet = CArmyCorpsBasicBox.GetArmyCorpsMember(uArmyCorpsId)
  for i = 1, MemberRet:GetRowNum() do
  	local uTongId = MemberRet:GetNumber(i-1,0)
  	local TongBasicData = tong_box.GetTongBasicData(uTongId)
  	table.insert(tbl_TongBasicData, TongBasicData)
  end
  MemberRet:Release()
	return true,MemberCount,MemberCountLimit,tbl_TongBasicData
end

function CArmyCorpsBasicBox.DelArmyCorps(uArmyCorpsId,uPlayerID)
	StmtOperater._DelArmyCorps:ExecStat(uArmyCorpsId)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	return false
  end
  local g_LogMgr = RequireDbBox("LogMgrDB")
  g_LogMgr.ArmyCorpsBreakInfoLog(uArmyCorpsId,uPlayerID)
  return true
end

function CArmyCorpsBasicBox.DelArmyCorpsMember(uArmyCorpsId,uTongId,uPlayerID,uPosition)
	StmtOperater._DelArmyCorpsMember:ExecStat(uTongId)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	return false
  end
  local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.ArmyCorpsMemberEventLog(uArmyCorpsId,2,uPlayerID,uTongId,0)
  return true
end

function CArmyCorpsBasicBox.DelArmyCorpsAdmin(uArmyCorpsId,uPlayerId,uExecutorId,uExecutorPos)
	StmtOperater._DelArmyCorpsAdmin:ExecStat(uPlayerId)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	return false
  end
  local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.ArmyCorpsMemberEventLog(uArmyCorpsId,3,uExecutorId,uPlayerId,g_ArmyCorpsMgr:GetPosIndexByName("团员"))
  return true
end

function CArmyCorpsBasicBox.OnDealArmyCorpsAdminWhenDelChar(nCharID,uArmyCorpsId,uTime)
	--从其他佣兵小队中找个队长担任团长职位
	local Ret = StmtOperater._GetLastLoginLeaderHaveACMPos:ExecStat(g_TongMgr:GetPosIndexByName("团长"),uArmyCorpsId,nCharID,uTime)
	if Ret:GetRowNum() == 0 then
		Ret:Release()
		Ret = StmtOperater._GetLastLoginLeader:ExecStat(g_TongMgr:GetPosIndexByName("团长"),uArmyCorpsId,nCharID,uTime)
	end
	if Ret:GetRowNum() > 0 then
		local NewAdminId = Ret:GetData(0,0)
		Ret:Release()
		CArmyCorpsBasicBox.DelArmyCorpsAdmin(uArmyCorpsId,nCharID,0,0)
		CArmyCorpsBasicBox.UpdateArmyCorpsAddmin(NewAdminId,uArmyCorpsId,g_ArmyCorpsMgr:GetPosIndexByName("团长"),0,0)
		return NewAdminId, nil
	else--没有人找个人担任团长时解散佣兵团
		local tong_box = RequireDbBox("GasTongBasicDB")
		local uTongId = tong_box.GetTongID(nCharID)
		local MemberRet = CArmyCorpsBasicBox.GetArmyCorpsMember(uArmyCorpsId)
		local MemberTong = {}
		for i = 1, MemberRet:GetRowNum() do
  		local uTongId = MemberRet:GetNumber(i-1,0)
  		table.insert(MemberTong,uTongId)
  	end
		CArmyCorpsBasicBox.DelAdminWhenTongLeaveArmyCorps(uArmyCorpsId,uTongId,0,0)
		CArmyCorpsBasicBox.DelMemTongWnenBreakArmyCorps(uArmyCorpsId)
		CArmyCorpsBasicBox.DelArmyCorpsMember(uArmyCorpsId,uTongId,0,0)
		CArmyCorpsBasicBox.OnLeaveArmyCorps(uTongId)
		CArmyCorpsBasicBox.DelArmyCorps(uArmyCorpsId,0)
		return 0,MemberTong
	end
end

function CArmyCorpsBasicBox.CheckAndChangeArmyCorpsCaptain()
	local result =  StmtOperater.GetNoCaptainArmyCorpsId:ExecStat() 
	local row = result:GetRowNum()
	if 0 == row then return end
	
	local uTime = 30*24*60*60
	for i=1,row do
		local ArmyCorps_ID = result:GetNumber(i-1,0)
		local Ret = StmtOperater._GetLastLoginLeaderHaveACMPos:ExecStat(g_TongMgr:GetPosIndexByName("团长"),ArmyCorps_ID,0,uTime)
		if Ret:GetRowNum() == 0 then
			Ret:Release()
			Ret = StmtOperater._GetLastLoginLeader:ExecStat(g_TongMgr:GetPosIndexByName("团长"),ArmyCorps_ID,0,uTime)
		end
		if Ret:GetRowNum() > 0 then
			local NewAdminId = Ret:GetData(0,0)
			CArmyCorpsBasicBox.UpdateArmyCorpsAddmin(NewAdminId,ArmyCorps_ID,g_ArmyCorpsMgr:GetPosIndexByName("团长"),0,0)
		end
	end
end

function CArmyCorpsBasicBox.DelArmyCorpsAdminWhenLeaveTong(uPlayerID, bForce)
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
	if uArmyCorpsId == 0 then
		return true
	end
	local pos_index = CArmyCorpsBasicBox.GetPositionById(uPlayerID)
	if pos_index == g_ArmyCorpsMgr:GetPosIndexByName("团长") then
		if bForce then
			CArmyCorpsBasicBox.DelArmyCorpsAdmin(uArmyCorpsId,uPlayerID,uPlayerID,CArmyCorpsBasicBox.GetPositionById(uPlayerID))
			return true
		end
		local MemberCount = CArmyCorpsBasicBox.GetArmyCorpsMemberCount(uArmyCorpsId)
		if MemberCount > 1 then
			return false,8539
		else 
			return false,8538
		end
	elseif pos_index == g_ArmyCorpsMgr:GetPosIndexByName("副团长") then
		CArmyCorpsBasicBox.DelArmyCorpsAdmin(uArmyCorpsId,uPlayerID,uPlayerID,CArmyCorpsBasicBox.GetPositionById(uPlayerID))
	end
	return true
end

function CArmyCorpsBasicBox.CountArmyCorpsAdminByType(uArmyCorpsId,uPos)
	local CountRet = StmtOperater._CountArmyCorpsAdminByPos:ExecStat(uArmyCorpsId,uPos)
	local uCount = CountRet:GetNumber(0,0)
	CountRet:Release()
	return uCount
end

function CArmyCorpsBasicBox.LeaveArmyCorpsDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	if tong_box.GetPositionById(uPlayerID) ~= g_TongMgr:GetPosIndexByName("团长") then
		return false,8516
	end
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
	if uArmyCorpsId == 0 then
		return false,8509
	end
	local pos_index = CArmyCorpsBasicBox.GetPositionById(uPlayerID)
	if pos_index == g_ArmyCorpsMgr:GetPosIndexByName("团长") then
		local MemberCount = CArmyCorpsBasicBox.GetArmyCorpsMemberCount(uArmyCorpsId)
		if MemberCount > 1 then
			return false,8518
		end
	end

	local uTongId = tong_box.GetTongID(uPlayerID)
	if not CArmyCorpsBasicBox.DelArmyCorpsMember(uArmyCorpsId,uTongId,uPlayerID,pos_index) then
		CancelTran()
		return false, 8519
	end
	--有时序需求
	CArmyCorpsBasicBox.DelAdminWhenTongLeaveArmyCorps(uArmyCorpsId,uTongId,uPlayerID,CArmyCorpsBasicBox.GetPositionById(uPlayerID))
	local MemberCount = CArmyCorpsBasicBox.GetArmyCorpsMemberCount(uArmyCorpsId)
	if MemberCount == 0 then
		if not CArmyCorpsBasicBox.DelArmyCorps(uArmyCorpsId,uPlayerID) then
  		CancelTran()
			return false, 8519
		end
	end
	CArmyCorpsBasicBox.OnLeaveArmyCorps(uTongId)
	return true,0,uTongId
end

function CArmyCorpsBasicBox.KickOutOfArmyCorpsDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local uTongId 	= parameters["uTongId"]
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
	if uArmyCorpsId == 0 then
		return false,8509
	end
	local pos_index = CArmyCorpsBasicBox.GetPositionById(uPlayerID)
	if not g_ArmyCorpsMgr:JudgePurview(pos_index,"KickOut") then
		return false,8520
	end
	local uTargetArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsByTongID(uTongId)
	if uTargetArmyCorpsId == 0 then
		return false,8521
	end
	if uArmyCorpsId ~= uTargetArmyCorpsId then
		return false,8522
	end
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uLeaderId = tong_box.GetTongLeaderId(uTongId)
	if uLeaderId == uPlayerID then
		return false,8523
	end
	if uLeaderId == -1 then
		return false,8519
	end
	
	local Target_pos_index = CArmyCorpsBasicBox.GetPositionById(uLeaderId)
	local uPosIndex1 = g_ArmyCorpsMgr:GetPosIndexByName("团长")
	local uPosIndex2 = g_ArmyCorpsMgr:GetPosIndexByName("副团长")
	if Target_pos_index == uPosIndex1 then
		return false,8524
	end
	if Target_pos_index == pos_index and uPosIndex2 == pos_index then
		return false,8524
	end
		
	if not CArmyCorpsBasicBox.DelArmyCorpsMember(uArmyCorpsId,uTongId,uPlayerID,pos_index) then
		CancelTran()
		return false, 8519
	end
	--有时序需求
	CArmyCorpsBasicBox.DelAdminWhenTongLeaveArmyCorps(uArmyCorpsId,uTongId,uPlayerID,pos_index)
	CArmyCorpsBasicBox.OnLeaveArmyCorps(uTongId)
	local tblMembers = tong_box.GetTongOnlineCharId(uTongId)   
	return true,0,tblMembers
end

function CArmyCorpsBasicBox.DelAdminWhenTongLeaveArmyCorps(uArmyCorpsId,uTongId,uExecutorId,uExecutorPos)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local all_members = tong_box.GetAllMemByTongID(uTongId)
	for i=1, all_members:GetRowNum() do
		local uMemberId = all_members:GetData(i-1,0)
		CArmyCorpsBasicBox.DelArmyCorpsAdmin(uArmyCorpsId,uMemberId,uExecutorId,uExecutorPos)
	end
end

function CArmyCorpsBasicBox.DelMemTongWnenBreakArmyCorps(uArmyCorpsId)
	local MemberRet = CArmyCorpsBasicBox.GetArmyCorpsMember(uArmyCorpsId)
  for i = 1, MemberRet:GetRowNum() do
  	local uTongId = MemberRet:GetNumber(i-1,0)
		CArmyCorpsBasicBox.DelArmyCorpsMember(uArmyCorpsId,uTongId,0,0)
  end
end

function CArmyCorpsBasicBox.ChangeArmyCorpsPosDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local uTargetId = parameters["uTargetId"]
	local uChangeToPos = parameters["uChangeToPos"]
	local uArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uPlayerID)
	if uArmyCorpsId == 0 then
		return false,8509
	end
	local pos_index = CArmyCorpsBasicBox.GetPositionById(uPlayerID)
	if not g_ArmyCorpsMgr:JudgePurview(pos_index,"ChangePosition") then
		return false,8531
	end
	local uTargetArmyCorpsId = CArmyCorpsBasicBox.GetArmyCorpsIdByPlayerId(uTargetId)
	if uTargetArmyCorpsId == 0 then
		return false,8521
	end
	if uArmyCorpsId ~= uTargetArmyCorpsId then
		return false,8522
	end
	if uPlayerID == uTargetId then
		return false,8536
	end
	
	--最多2个副团长(打到最大时,不允许再设置副团),1个团长(设置时替换自己)
	if uChangeToPos == g_ArmyCorpsMgr:GetPosIndexByName("团长") then
		--删除自己的团长权限
		local tong_box = RequireDbBox("GasTongBasicDB")
		local uTargetTongPosition = tong_box.GetPositionById(uTargetId)
		if uTargetTongPosition ~= g_TongMgr:GetPosIndexByName("团长") then
			return false, 8527
		end
		
		if not CArmyCorpsBasicBox.DelArmyCorpsAdmin(uArmyCorpsId,uPlayerID,uPlayerID,pos_index) then
			CancelTran()
			return false, 8519
		end
		local Ret, uMsgId = CArmyCorpsBasicBox.UpdateArmyCorpsAddmin(uTargetId,uArmyCorpsId,uChangeToPos,uPlayerID,pos_index)
		if not Ret then
			CancelTran()
			return false, uMsgId
		end
	elseif uChangeToPos == g_ArmyCorpsMgr:GetPosIndexByName("副团长") then
		local uPos = g_ArmyCorpsMgr:GetPosIndexByName("副团长")
		local uCount = CArmyCorpsBasicBox.CountArmyCorpsAdminByType(uArmyCorpsId,uPos)
		if uCount >= g_ArmyCorpsMgr:GetAssistantColonelCountLimit() then
			return false, 8530
		end
		local uPosNow = CArmyCorpsBasicBox.GetPositionById(uTargetId)
	  if uPosNow == uChangeToPos then
	  	return false,8532
	  end
		local Ret, uMsgId = CArmyCorpsBasicBox.UpdateArmyCorpsAddmin(uTargetId,uArmyCorpsId,uChangeToPos,uPlayerID,pos_index)
		if not Ret then
			CancelTran()
			return false, uMsgId
		end
	elseif uChangeToPos == g_ArmyCorpsMgr:GetPosIndexByName("团员") then
		local uPosNow = CArmyCorpsBasicBox.GetPositionById(uTargetId)
		if uPosNow == g_ArmyCorpsMgr:GetPosIndexByName("团长") then
			return false,8537
		elseif uPosNow == g_ArmyCorpsMgr:GetPosIndexByName("副团长") then
			if not CArmyCorpsBasicBox.DelArmyCorpsAdmin(uArmyCorpsId,uTargetId,uPlayerID,pos_index) then
				CancelTran()
				return false, 8519
			end
		elseif uPosNow == g_ArmyCorpsMgr:GetPosIndexByName("团员") then
			return false,8532
		end
	else
		return false,8529
	end
	return true
end

function CArmyCorpsBasicBox.OnLeaveArmyCorps(uTongId)
	StmtOperater._DelLeaveArmyCorps:ExecStat(uTongId)
  StmtOperater._AddLeaveArmyCorps:ExecStat(uTongId)
end

function CArmyCorpsBasicBox.JudgeLeaveTongTime(uTongId)
	local time = StmtOperater._SelectLeaveArmyCorpsTime:ExecStat(uTongId)
  if time:GetRowNum() == 0 then
  	return 1
  end
  if (time:GetData(0,0) < 40*60) then
  	return 0
  end
  return 1
end

return CArmyCorpsBasicBox
