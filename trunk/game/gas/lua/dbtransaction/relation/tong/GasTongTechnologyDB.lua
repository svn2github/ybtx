gac_gas_require "relation/tong/TongTechnologyMgr"
gac_gas_require "relation/tong/TongMgr"
gac_gas_require "framework/common/CMoney"

local g_TongMgr = CTongMgr:new()
local g_TongTechMgr = CTongTechnologyMgr:new()
local StmtOperater = {}	
local CMoney    =   CMoney
local event_type_tbl = event_type_tbl
local g_TongFightTechToPlayerTbl = g_TongFightTechToPlayerTbl
local LogErr = LogErr
------------------------------------
local CTongTechBox = CreateDbBox(...)
---------------------------------------------------------------------------------
local StmtDef = {
    	"_GetTechInfoByName",
    	[[ 
    		select  TTS.tts_uId,TTS.tts_uLevel,ifnull(TTO.tto_uState,0)
    	  	from    tbl_tong_tech_static as TTS
    	  	left join tbl_tong_tech_order as TTO 
    	  	on (TTS.tts_uId = TTO.tts_uId) , tbl_tong_tech_info_static TTIS
    		where   TTS.t_uId = ?
    		and		TTIS.ttis_sName = ?
    		and		TTIS.ttis_uType = ?
    		and     TTS.ttis_uId = TTIS.ttis_uId
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelTeachTimeInfoByID",
    	[[ 
    		delete from tbl_tong_tech_order 
     		where   tts_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_CountTechOrderByType",
    	[[ 
    		select  count(1)
    	  	from    tbl_tong_tech_static as TTS,tbl_tong_tech_order as TTO , tbl_tong_tech_info_static TTIS
    		where   
    			TTS.tts_uId = TTO.tts_uId
    			and TTS.t_uId = ?
    			and TTIS.ttis_uType = ?
    			and TTO.tto_uState = ?
    			and TTS.ttis_uId = TTIS.ttis_uId
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddTechStaticInfo",
    	[[ 
    		insert into tbl_tong_tech_static(t_uId,ttis_uId,tts_uLevel) values(?,?,?)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddTechTimeInfo",
    	[[ 
    		insert into tbl_tong_tech_order 
    		set tts_uId = ?,tto_dtStartTime = now(),tto_uState = ?,tto_dtUpdateTime = now()
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--把暂停状态的改为研发状态
local StmtDef = {
    	"_UpdateStayTechToResearh",
    	[[ 
    		update tbl_tong_tech_order 
    		set tto_dtStartTime = date_add(now(),interval -(unix_timestamp(tto_dtUpdateTime)-unix_timestamp(tto_dtStartTime)) second),
    				tto_uState = ?,tto_dtUpdateTime = now()
    		where tts_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateStayTechToWait",
    	[[ 
    		update tbl_tong_tech_order 
    		set tto_dtUpdateTime = date_add(now(),interval (unix_timestamp(tto_dtUpdateTime)-unix_timestamp(tto_dtStartTime)) second),
    				tto_uState = ?,tto_dtStartTime = now()
    		where tts_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetResearchingTechInfo",
    	[[ 
    		select  TTS.tts_uId,TTIS.ttis_sName,TTS.tts_uLevel,TTO.tto_dtStartTime,unix_timestamp(now())-unix_timestamp(TTO.tto_dtStartTime),TTIS.ttis_uType
    	  	from    tbl_tong_tech_static as TTS
    	  	left join tbl_tong_tech_order as TTO 
    	  	on (TTS.tts_uId = TTO.tts_uId) ,tbl_tong_tech_info_static TTIS
    		where   TTS.t_uId = ?
    		and 	TTO.tto_uState = ?
    		and TTIS.ttis_uId = TTS.ttis_uId 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_TeachLevelUpByID",
    	[[ 
    		update tbl_tong_tech_static set tts_uLevel = ? where tts_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_GetOneWaitingTeachInfo",
		[[
			select TTO.tts_uId,TTO.tto_uState,unix_timestamp(tto_dtUpdateTime)-unix_timestamp(tto_dtStartTime)
			from tbl_tong_tech_static as TTS 
			right join tbl_tong_tech_order as TTO
			on(TTS.tts_uId = TTO.tts_uId), tbl_tong_tech_info_static TTIS
			where TTO.tto_uState = ?
			and TTS.t_uId = ?
			and TTIS.ttis_uType = ?
			and TTS.ttis_uId = TTIS.ttis_uId
			order by tto_dtStartTime asc limit 1
		]]
}
DefineSql ( StmtDef, StmtOperater )
local StmtDef = 
{
		"_GetAllTeachInfoByTong",
		[[
			select TTS.tts_uId,TTIS.ttis_sName,TTS.tts_uLevel,TTIS.ttis_uType,ifnull(TTO.tto_uState,0) as uState,ifnull(unix_timestamp(now())-unix_timestamp(TTO.tto_dtStartTime),0) as LastTime
			 from tbl_tong_tech_static as TTS 
			left join tbl_tong_tech_order as TTO
			on(TTS.tts_uId = TTO.tts_uId), tbl_tong_tech_info_static TTIS
			where TTS.t_uId = ? and TTS.ttis_uId = TTIS.ttis_uId
			order by uState ASC,LastTime Desc
		]]
}
DefineSql ( StmtDef, StmtOperater )
local StmtDef = 
{
		"_UpdateTeachStateAndTime",
		[[
			update tbl_tong_tech_order set tto_uState = ?,tto_dtStartTime = date_add(?,interval ? second)
			where 	tts_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_UpdateTeachTimeByTeachID",
		[[
			update tbl_tong_tech_order set tto_uState = ?,tto_dtStartTime = now()
			where 	tts_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_GetTeachLevelByName",
		[[
			select tts_uLevel from tbl_tong_tech_static s, tbl_tong_tech_info_static i
			where t_uId = ?
			and ttis_sName = ?
			and ttis_uType = ?
			and s.ttis_uId = i.ttis_uId
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_ResetTechInfo",
		[[
			update tbl_tong_tech_static set tts_uLevel = 0 where t_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )
local StmtDef = 
{
		"_CountTechInfo",
		[[
			select count(*) from  tbl_tong_tech_static where t_uId = ? and tts_uLevel > 0 
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_CountTechStateInfo",
		[[
			select count(*) from  tbl_tong_tech_order as tto 
			left join tbl_tong_tech_static as tts
			   	on(tto.tts_uId = tts.tts_uId)
			 where tts.t_uId = ? 
		]]
}
DefineSql ( StmtDef, StmtOperater )
local StmtDef = 
{
		"_DelTechStateInfo",
		[[
			delete from  tbl_tong_tech_order 
			where tts_uId in(select tts_uId from tbl_tong_tech_static where t_uId = ?)
		]]
}
DefineSql ( StmtDef, StmtOperater )
function CTongTechBox.ResetTechInfo(params)
	local uTongID = params["uTongID"]
	local tblState = StmtOperater._CountTechStateInfo:ExecStat(uTongID)
	if tblState(1,1) > 0 then
		StmtOperater._DelTechStateInfo:ExecStat(uTongID)
		if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
			CancelTran()
			return  
		end
	end
	
	local tblTech = StmtOperater._CountTechInfo:ExecStat(uTongID)
	if tblTech(1,1) > 0 then
		StmtOperater._ResetTechInfo:ExecStat(uTongID)
		if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
			CancelTran()
			return  
		end
	end
	local TeachInfo = StmtOperater._GetAllTeachInfoByTong:ExecStat(uTongID)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	for i=1,TeachInfo:GetRowNum() do
		local tech_name = TeachInfo(i,2)
		local tech_level = TeachInfo(i,3)
		g_LogMgr.TongTechnologyEventLog(uTongID,4,0,tech_name,tech_level)		
	end
	return true
end

local function LoadTechResByLevel(sTechName, uTechLevel, uTongID)
	local NeedMoney		= g_TongTechMgr:GetFightTechNeedTongMoney(sTechName, uTechLevel)
	local NeedResource	= g_TongTechMgr:GetFightTechNeedRes(sTechName, uTechLevel)
	local NeedTime		= g_TongTechMgr:GetFightTechNeedTime(sTechName, uTechLevel)
	local TimeTechLevel	= StmtOperater._GetTeachLevelByName:ExecStat(uTongID,"战斗学研发速度提升",1)
	local TimeDepress	= 0
	
	if TimeTechLevel:GetRowNum()> 0 then
		local nTimeLevel = TimeTechLevel:GetData(0,0)
		TimeDepress = g_TongTechMgr:GetFightTechTimeDepress("战斗学研发速度提升", nTimeLevel)
	end
	
	local ConsumeTechLevel = StmtOperater._GetTeachLevelByName:ExecStat(uTongID,"战斗学研发消耗降低",1)
	local ConsumeDepress = 0
	if ConsumeTechLevel:GetRowNum()> 0 then
		local nConsumeLevel = ConsumeTechLevel:GetData(0,0)
		ConsumeDepress = g_TongTechMgr:GetFightTechConsumeDepress("战斗学研发速度提升", nTimeLevel)
	end

	NeedTime		= NeedTime - TimeDepress
	NeedResource	= NeedResource*(1 - ConsumeDepress)
	NeedMoney		= NeedMoney*(1 - ConsumeDepress)
	return NeedResource,NeedMoney,NeedTime
end

function CTongTechBox.GetTongTeachInfoDB(params)
	local uTongID = params.uTongID
	local TeachInfo = StmtOperater._GetAllTeachInfoByTong:ExecStat(uTongID)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local TongLevel = tong_box.GetTongLevel(uTongID)
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local stationLine = WarZoneDB.GetTongRelativeLine(uTongID)
		
	local tbl = {}
	tbl.TeachInfo = TeachInfo
	tbl.TongLevel = TongLevel 
	tbl.TongStationLine = stationLine
	return tbl
end

local StmtDef = 
{
    "_GetTongTechInfoByCharID",
    [[
        select ttis_sName, tct_uLevel from tbl_tong_char_tech t, tbl_tong_tech_info_static s
        where t.ttis_uId = s.ttis_uId and cs_uId = ? and tct_uLevel > 0 
    ]]
}DefineSql(StmtDef, StmtOperater)
function CTongTechBox.InitTongTechByCharID(charID)
    local tongTechRet = StmtOperater._GetTongTechInfoByCharID:ExecStat(charID)
    return tongTechRet
end

function CTongTechBox.DealWithRDTechnologyTime(uTongID)
	local bFlag = true
	local g_LogMgr = RequireDbBox("LogMgrDB")
	while(bFlag) do
		local tblInfo = StmtOperater._GetResearchingTechInfo:ExecStat(uTongID,g_TongTechMgr:GetStateIndex("Researching"))
		local row = tblInfo:GetRowNum()
		if row > 0 then
			local Flag = false
			for i =1,row do
				local nTechID,sTechName,nTechLevel= tblInfo:GetData(i-1,0),tblInfo:GetData(i-1,1),tblInfo:GetData(i-1,2)
				local sStartTime,nPastTime,nTechType = tblInfo:GetData(i-1,3),tblInfo:GetData(i-1,4),tblInfo:GetData(i-1,5)
				local _,_,NeedTime = LoadTechResByLevel(sTechName, nTechLevel+1, uTongID)
				if nPastTime >= NeedTime then
					StmtOperater._TeachLevelUpByID:ExecStat(nTechLevel+1,nTechID)
					StmtOperater._DelTeachTimeInfoByID:ExecStat(nTechID)
					local one_waiting = StmtOperater._GetOneWaitingTeachInfo:ExecStat(g_TongTechMgr:GetStateIndex("Waiting"),uTongID,nTechType)
					if one_waiting:GetRowNum() > 0 then
						local ChangeTime = NeedTime
						local wState,wLastTime = one_waiting(1,2),one_waiting(1,3)
						if wLastTime > 0 then
							--被暂停过
							ChangeTime = NeedTime - wLastTime
						end
						StmtOperater._UpdateTeachStateAndTime:ExecStat(g_TongTechMgr:GetStateIndex("Researching"),sStartTime,ChangeTime,one_waiting:GetData(0,0))
						Flag = true
					end
					g_LogMgr.TongTechnologyEventLog(uTongID,3,0,sTechName,nTechLevel+1)
				end
			end
			if not Flag then
				bFlag = false
			end
		else
			bFlag = false
		end
	end
end

local StmtDef = {
    "_GetTongTechId",
    [[
        select ttis_uId from tbl_tong_tech_info_static where ttis_sName = ? and ttis_uType = ?      
    ]]
}DefineSql ( StmtDef, StmtOperater ) 

local StmtDef = {
    "_GetAllTechId",
    [[
        select ttis_uId,ttis_uType,ttis_sName from tbl_tong_tech_info_static     
    ]]
}DefineSql ( StmtDef, StmtOperater ) 

function CTongTechBox.RDTechnologyDB(params)
	local uPlayerID = params.uPlayerID
	local sTechName = params.TechName
	local TechType = params.TechType
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	if 0 == uTongID then return end
	
	if not tong_box.JudgePurview(uPlayerID,"TechnologyManage") then
		return 9006
	end
	
	local nTechLevel,nTechID,nNowState = 0,0,0
	local TechDBInfo = StmtOperater._GetTechInfoByName:ExecStat(uTongID,sTechName,TechType)
	if TechDBInfo:GetRowNum() > 0 then
		nTechLevel = TechDBInfo(1,2)
		nTechID = TechDBInfo(1,1)
		nNowState = TechDBInfo(1,3)
	end
	
	if nTechLevel >= g_TongTechMgr.m_nMaxTeachLevel then 
		return 9341
	end
	if nNowState ~= 0 and nNowState ~= g_TongTechMgr:GetStateIndex("Stay") then
		--已经在等待或者研发
	 	return
 	end 
 	
	if not g_TongTechMgr:HasFightTechInfo(sTechName, nTechLevel+1) then
		return 9341
	end
	local TongLevel = tong_box.GetTongLevel(uTongID)
	local tblCountWaiting = StmtOperater._CountTechOrderByType:ExecStat(uTongID,TechType,g_TongTechMgr:GetStateIndex("Waiting"))
	if tblCountWaiting:GetNumber(0,0) >= g_TongTechMgr:GetMaxOrder(TongLevel) then 
		return 9342  --等待队列满了
	end
	
	local NeedTongLevel	= g_TongTechMgr:GetFightTechNeedTongLevel(sTechName, nTechLevel+1)
	local NeedLine	= g_TongTechMgr:GetFightTechStationLine(sTechName, nTechLevel+1)
	
	local NeedResource,NeedMoney = LoadTechResByLevel(sTechName, nTechLevel+1, uTongID)
	local build_box = RequireDbBox("GasTongbuildingDB")
	if tong_box.GetTongLevel(uTongID) < NeedTongLevel then
		return 9338,NeedTongLevel		--佣兵团等级不足
	end
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local NowLine = WarZoneDB.GetTongRelativeLine(uTongID)
	if NeedLine > NowLine then
		return 9337	--战线不够
	end
	if nNowState ~= g_TongTechMgr:GetStateIndex("Stay") then
		--暂停状态重新开始研发是不扣资源和钱的
		if NeedResource and NeedResource > 0 then
			if not build_box.UpdateTongResource(-NeedResource,uTongID,event_type_tbl["佣兵小队科技研发"]) then
				return 9035	--资源不足
			end
		end
		if NeedMoney and NeedMoney > 0 then
			if not tong_box.UpdateTongMoney({["uTongID"]=uTongID,["uMoney"]= -NeedMoney,["nEventType"] = event_type_tbl["佣兵小队科技研发"]}) then
				CancelTran()
				return 9034	--money不足
			end
		end
	end
	
	if 0 == nTechID then
		local techIdRet = StmtOperater._GetTongTechId:ExecStat(sTechName,TechType)
		if techIdRet:GetRowNum() == 0 then
			local all_tech = StmtOperater._GetAllTechId:ExecStat()
			local str_res = "name:" .. sTechName .. " name_len:" .. string.len(sTechName) .. " type:" .. TechType .. " number_type:" .. type(TechType)
			for i=1,all_tech:GetRowNum() do
				str_res = str_res .. " tech_id:".. all_tech(i,1) .. " tech_type:" .. all_tech(i,2) .. "tech_name:" .. all_tech(i,3) .. " number_type:" .. type(all_tech(i,2)) .. " name_len:" .. string.len(all_tech(i,3))
			end
			LogErr("tbl_tong_tech_info_static查询不到科技信息",str_res)
			CancelTran()
			return 
		end
		local techId = techIdRet:GetData(0,0)
		--第一次研发
		StmtOperater._AddTechStaticInfo:ExecStat(uTongID,techId,nTechLevel)
		nTechID = g_DbChannelMgr:LastInsertId()
		if 0 == nTechID then
			 CancelTran()
			 return
	 	end
	end
	local nNewState = g_TongTechMgr:GetStateIndex("Waiting")
	local tblResearchNum = StmtOperater._CountTechOrderByType:ExecStat(uTongID,TechType,g_TongTechMgr:GetStateIndex("Researching"))
	if tblResearchNum:GetData(0,0) == 0 then
		--没有正在研发的，直接进入研发状态
		nNewState = g_TongTechMgr:GetStateIndex("Researching")
	end
	if nNowState == g_TongTechMgr:GetStateIndex("Stay") and nNewState == g_TongTechMgr:GetStateIndex("Researching") then
		--暂停状态变为研发状态
		StmtOperater._UpdateStayTechToResearh:ExecStat(nNewState,nTechID)
	else
		if nNowState == g_TongTechMgr:GetStateIndex("Stay") then
    	StmtOperater._UpdateStayTechToWait:ExecStat(nNewState,nTechID)
		else
			StmtOperater._AddTechTimeInfo:ExecStat(nTechID,nNewState)
		end
	end
	if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
			CancelTran()
			return  
	end
	--添加log
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongTechnologyEventLog(uTongID,1,uPlayerID,sTechName,nTechLevel+1)
	
	local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
	local tech_name = sTechName .. nTechLevel+1
	local sMsg = "2026_" .. player_name .. ",techname:" .. tech_name
	if not tong_box.AddTongLogs(uTongID,sMsg, g_TongMgr:GetLogType("科技")) then
		return 9017
	end
	return true,uTongID
end

--【取消研发】
function CTongTechBox.CannelRDTeach(params)
	local uPlayerID = params.uPlayerID
	local TechType = params.TechType
	local TechName = params.TechName
	local tong_box = RequireDbBox("GasTongBasicDB")
	if not tong_box.JudgePurview(uPlayerID,"TechnologyManage") then
		return 9006
	end
	local uTongID = tong_box.GetTongID(uPlayerID)
	if 0 == uTongID then return end
	
	local tblState = StmtOperater._GetTechInfoByName:ExecStat(uTongID,TechName,TechType)
	if tblState:GetRowNum() == 0 then return end --还没有研发过
	local nState,nTechLevel,nTechID = tblState:GetData(0,2),tblState:GetData(0,1),tblState:GetData(0,0)
	if (nState == 0) or (nState == g_TongTechMgr:GetStateIndex("Stay")) then
		 --没在队列中或者处于暂停状态的
		 return
	end
	
	local nRes,nRetMoney,nProp = 0,0,1

	local nAddRes,nAddMoney = LoadTechResByLevel(TechName, nTechLevel+1, uTongID)

	nRetMoney	= nAddMoney * nProp
	nRes		= nAddRes * nProp
	local build_box = RequireDbBox("GasTongbuildingDB")
	local tblRetRes = build_box.GivebackResAndMoney(uTongID,nRes,nRetMoney)
	
	StmtOperater._DelTeachTimeInfoByID:ExecStat(nTechID)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
		CancelTran()
		return  
	end
	if nState == g_TongTechMgr:GetStateIndex("Researching") then
		local one_waiting = StmtOperater._GetOneWaitingTeachInfo:ExecStat(g_TongTechMgr:GetStateIndex("Waiting"),uTongID,TechType)
		if one_waiting:GetRowNum() > 0 then
			StmtOperater._UpdateTeachTimeByTeachID:ExecStat(g_TongTechMgr:GetStateIndex("Researching"),one_waiting(1,1))
			if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
				CancelTran()
				return
			end
		end
	end
	--添加log
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongTechnologyEventLog(uTongID,2,uPlayerID,TechName,nTechLevel)
	
	local ex = RequireDbBox("Exchanger")
	local player_name = ex.getPlayerNameById(uPlayerID)
	local tech_name = TechName .. nTechLevel+1
	local sMsg = "2027_" .. player_name .. ",techname:" .. tech_name
	if not tong_box.AddTongLogs(uTongID,sMsg,g_TongMgr:GetLogType("科技")) then
		return 9017
	end
	
	local result = {}
	result.tblRetRes = tblRetRes
	result.uTongID = uTongID
	return result
end

function CTongTechBox.DealWithTeachTimeInfo(parameters)
	local uPlayerID		= parameters.uPlayerID
	local nTechType	= parameters.nTeachType
	local tong_box		= RequireDbBox("GasTongBasicDB")
	local uTongID		= tong_box.GetTongID(uPlayerID)
	CTongTechBox.DealWithRDTechnologyTime(uTongID) 
	return uTongID
end

-----------------------------------------------
--【暂停研发】
local StmtDef = 
{
		"_DelWaitingTeach",
		[[
			delete from tbl_tong_tech_order 
			where tts_uId in (select tts_uId from tbl_tong_tech_static where t_uId = ?)
			and  tto_uState = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )
local StmtDef = 
{
		"_PauseRDTeach",
		[[
			update tbl_tong_tech_order as tto,tbl_tong_tech_static as tts 
			set tto.tto_uState = ?,tto.tto_dtUpdateTime = now()
 			where tto.tts_uId = tts.tts_uId and tts.t_uId = ?
 				and tto.tto_uState = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )
local StmtDef = 
{
		"_UpdateTeachState",
		[[
			update tbl_tong_tech_order set tto_uState = ?,tto_dtUpdateTime = now()
			where 	tts_uId = ?
		]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
		"_GetAllResearchingTeachInfo",
		[[
			select 	TTS.tts_uId,TTIS.ttis_sName,TTS.tts_uLevel,TTIS.ttis_uType,TTO.tto_uState as uState
		 	from 		tbl_tong_tech_static as TTS,tbl_tong_tech_order as TTO, tbl_tong_tech_info_static TTIS
			where 	TTS.tts_uId = TTO.tts_uId and TTS.t_uId = ? and TTIS.ttis_uId = TTS.ttis_uId
			order by uState ASC
		]]
}
DefineSql ( StmtDef, StmtOperater )

function CTongTechBox.PauseRDTeach(parameters)
	local TongID = parameters.TongID
	local tong_box = RequireDbBox("GasTongBasicDB")
	local build_count = tong_box.CountBuildByNameAddState(TongID,"学院")
 	local pause_stat = g_TongTechMgr.m_tblTechStat["Stay"]
 	local wait_stat = g_TongTechMgr.m_tblTechStat["Waiting"]
 	local research_stat = g_TongTechMgr.m_tblTechStat["Researching"]
 	local g_LogMgr = RequireDbBox("LogMgrDB")
 	if build_count == 0 then
 		--建筑被打爆
 		--把所有正在研发的暂停
 		StmtOperater._PauseRDTeach:ExecStat(pause_stat,TongID,research_stat)		
 		
 		--删掉所有的等待序列
 		StmtOperater._DelWaitingTeach:ExecStat(TongID,wait_stat)	
 		local TeachInfo = StmtOperater._GetAllResearchingTeachInfo:ExecStat(TongID) 
		for i=1,TeachInfo:GetRowNum() do
			local tech_name,tech_level = TeachInfo(i,2),TeachInfo(i,3)
			g_LogMgr.TongTechnologyEventLog(TongID,2,0,tech_name,tech_level)
		end	
 	else
 		local WarZoneDB = RequireDbBox("WarZoneDB")
		local now_stationLine = WarZoneDB.GetTongRelativeLine(TongID)
		local TeachInfo = StmtOperater._GetAllResearchingTeachInfo:ExecStat(TongID) 
		for i=1,TeachInfo:GetRowNum() do
			local tech_id,tech_name,tech_level = TeachInfo(i,1),TeachInfo(i,2),TeachInfo(i,3)
			local tech_type,tech_state = TeachInfo(i,4),TeachInfo(i,5)
			local StationLineNeed	= g_TongTechMgr:GetFightTechStationLine(tech_name, tech_level+1)
			if now_stationLine < StationLineNeed then
				--超过战线限制
				if tech_state == wait_stat then
					--删除正在等待的 
					StmtOperater._DelTeachTimeInfoByID:ExecStat(tech_id) 
				elseif tech_state == research_stat then
					--把正在研发的改为暂停
					StmtOperater._UpdateTeachState:ExecStat(pause_stat,tech_id) 
					if g_DbChannelMgr:LastAffectedRowNum()>0 then
						local one_waiting = StmtOperater._GetOneWaitingTeachInfo:ExecStat(wait_stat,TongID,tech_type)
						if one_waiting:GetRowNum() > 0 then
							StmtOperater._UpdateTeachTimeByTeachID:ExecStat(research_stat,one_waiting:GetData(0,0))
						end
					end
				end
				g_LogMgr.TongTechnologyEventLog(TongID,2,0,tech_name,tech_level)
			end
		end
 	end
end


function CTongTechBox.ConsumePropertyByLearningTech(consumeInfo, charID, sSceneName)

    local ConsumeSoul   = consumeInfo["ConsumeSoul"]
    local ConsumeMoney  = consumeInfo["ConsumeMoney"]
    local Proffer       = consumeInfo["Proffer"]
    local ConsumeItem   = consumeInfo["ConsumeItemTbl"]
    local ConsumeTeamProffer = consumeInfo["TeamProffer"]
    local g_MoneyMgr = CMoney:new()
	local fun_info = g_MoneyMgr:GetFuncInfo("LearnTongTech")
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local bFlag,uMsgID, money = MoneyManagerDB.SubtractMoneyBySysFee(ConsumeMoney, charID, fun_info, fun_info["LearnTongTech"],event_type_tbl["学习佣兵小队科技"])
	if not bFlag then
		CancelTran()
		--, "收手续费出错！" 
		if IsNumber(uMsgID) then
			return false, uMsgID
		else
			return false, 9051
		end
	end 
	
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
    local param = {["soulCount"] = -ConsumeSoul,["PlayerId"] = charID,["addSoulType"] = event_type_tbl["学习佣兵小队科技"]}
	local bFlag,total_soul = EquipMgrDB.ModifyPlayerSoul(param)
	if not bFlag then
		CancelTran()
		return false, 9052
	end
	 
	local tong_box = RequireDbBox("GasTongBasicDB")
	if Proffer > 0 then
	    local bFlag = tong_box.ConsumeProffer(charID ,-Proffer, sSceneName,uEventId,event_type_tbl["学习佣兵小队科技"])
        --local bFlag,uMsgID = AreaFbPointDB.AddAreaFbPointByType(charID,-Proffer,10,sSceneName,uEventId,event_type_tbl["学习佣兵小队科技"])
        if bFlag == false then
            CancelTran()
            return false, 9053 
        end
    end
    
   
    local ret, msgID = tong_box.ConsumeTongProffer(charID, -ConsumeTeamProffer)
    if ret == false then
        return false,  msgID
    end
    local TeamProffer = tong_box.GetTongProfferByCharID(charID)
    Db2CallBack("ReturnTeamProfferPoint",charID,TeamProffer) 
    
    local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
    local res
    for i, v in pairs (ConsumeItem) do
        local itemType, itemName, itemCount = v[1], v[2], v[3]
        res = g_RoomMgr.DelItem(charID,itemType,itemName,itemCount,FromRoom,event_type_tbl["学习佣兵小队科技"])	
		if IsNumber(res) then
			CancelTran()
			return false, 9054
		end		
    end
    
    return true, res,total_soul
end

local StmtDef = {
    "_GetTongTechCurLevelByNameAndTongID",
    [[
        select tts_uLevel , s.ttis_uId from tbl_tong_tech_static s, tbl_tong_tech_info_static i
        where s.t_uId = ? and ttis_sName= ? and s.ttis_uId= i.ttis_uId
    ]]
}DefineSql ( StmtDef, StmtOperater ) 


local StmtDef = {
    "_GetCharTechCurLevel",
    [[
        select tct_uLevel from tbl_tong_char_tech t, tbl_tong_tech_info_static s 
        where t.ttis_uId = s.ttis_uId and ttis_sName = ? and cs_uId = ?
    ]]
}DefineSql ( StmtDef, StmtOperater ) 


local StmtDef = {
    "_InsertCharTechInfo",
    [[
        insert into tbl_tong_char_tech (cs_uId,ttis_uId,tct_uLevel) values (?,?,?)
    ]]
}DefineSql(StmtDef, StmtOperater)

local StmtDef = {
    "_UpdateCharTechInfo",
    [[
        update tbl_tong_char_tech set tct_uLevel = tct_uLevel + 1 where cs_uId = ? and ttis_uId = ?
    ]]
}
DefineSql(StmtDef, StmtOperater)


function CTongTechBox.LearnTongTech(data)
    local charID    =   data["CharID"]
    local sSceneName=   data["sSceneName"]
    local tongTechName = data["tongTechName"]

    local consumeInfoTbl = g_TongFightTechToPlayerTbl[tongTechName]
    if consumeInfoTbl == nil then
        return false, 9049
    end
    
    local tong_box = RequireDbBox("GasTongBasicDB")
    --校验要学习的科技名称和等级是否已经研发
    local tongId = tong_box.GetTongID(charID)
    local tongTechRet = StmtOperater._GetTongTechCurLevelByNameAndTongID:ExecStat(tongId, tongTechName)
    if tongTechRet:GetRowNum() < 1 then
        return false, 9055
    end
    local tongTechLevel = tongTechRet:GetData(0, 0)
    
    local charTechLevelRet = StmtOperater._GetCharTechCurLevel:ExecStat(tongTechName, charID)
    local charTechLevel = 0
    if charTechLevelRet:GetRowNum() > 0 then
        charTechLevel = charTechLevelRet:GetData(0,0 )
    end
     
    if charTechLevel >= tongTechLevel then
        return false, 9056
    end
    
    local consumeInfo = consumeInfoTbl[charTechLevel + 1]
    if consumeInfo == nil then
        return false, 9050
    end

    local sucFlag, res, totalSoul = CTongTechBox.ConsumePropertyByLearningTech(consumeInfo, charID, sSceneName)
    if sucFlag == false then
        return false, res
    end
    
    local techIndex = tongTechRet:GetData(0, 1)
    if charTechLevel == 0 then
        StmtOperater._InsertCharTechInfo:ExecStat(charID, techIndex, 1)
        charTechLevel = 1
    else
        StmtOperater._UpdateCharTechInfo:ExecStat(charID, techIndex)   
        charTechLevel = charTechLevel + 1             
    end
	if g_DbChannelMgr:LastAffectedRowNum() < 1 then
    CancelTran()
    return false, 9057
  end
    
  local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongTechnologyEventLog(tongId,5,charID,tongTechName,charTechLevel)
	
  local result = {}
  result["CurTechLevel"] = charTechLevel
  result["totalSoul"] = totalSoul
  return true, result
end


------------------------------------------gm指令------------------
local StmtDef = 
{
		"_SetTeachLevelByName",
		[[
			update  tbl_tong_tech_static s, tbl_tong_tech_info_static i
			set tts_uLevel = ?
			where t_uId = ?
			and ttis_sName = ?
			and ttis_uType = ?
			and s.ttis_uId = i.ttis_uId
		]]
}
DefineSql ( StmtDef, StmtOperater )
function CTongTechBox.SetTongTechLevelGM(data)
	 local charID    =   data["CharID"]
	 local tech_name =  data["TechName"]
	 local tech_level =  data["TechLevel"]
	 local type = 1
	 local tong_box = RequireDbBox("GasTongBasicDB")
   local tongId = tong_box.GetTongID(charID)
   if not tongId or tongId == 0 then
   	return false,9060
   end
   local TechDBInfo = StmtOperater._GetTechInfoByName:ExecStat(tongId,tech_name,type)
	 if TechDBInfo:GetRowNum() > 0 then
   		StmtOperater._SetTeachLevelByName:ExecStat(tech_level,tongId,tech_name,type)
   else
   		local techIdRet = StmtOperater._GetTongTechId:ExecStat(tech_name,type)
			local techId = techIdRet:GetData(0,0)
			StmtOperater._AddTechStaticInfo:ExecStat(tongId,techId,tech_level)
   end
   return true,9059
end

local StmtDef = {
    "_UpdateCharTechLevel",
    [[
        update tbl_tong_char_tech set tct_uLevel = ? where cs_uId = ? and ttis_uId = ?
    ]]
}
DefineSql(StmtDef, StmtOperater)
function CTongTechBox.SetCharTechLevelGM(data)
	local charID    =   data["CharID"]
	local tongTechName =  data["TechName"]
	local tech_level =  tonumber(data["TechLevel"])
	local tong_box = RequireDbBox("GasTongBasicDB")
   local tongId = tong_box.GetTongID(charID)
   if not tongId or tongId == 0 then
   		return false,9060
   end
   local tongTechRet = StmtOperater._GetTongTechCurLevelByNameAndTongID:ExecStat(tongId, tongTechName)
    if tongTechRet:GetRowNum() < 1 then
        return false, 9055
    end
    local tongTechLevel = tongTechRet:GetData(0, 0)
    
    if tech_level > tongTechLevel then
        return false, 9056
    end
    local techIndex = tongTechRet:GetData(0, 1)
    local charTechLevel = 0
    local charTechLevelRet = StmtOperater._GetCharTechCurLevel:ExecStat(tongTechName, charID)
    if charTechLevelRet:GetRowNum() == 0 then
        StmtOperater._InsertCharTechInfo:ExecStat(charID, techIndex, tech_level)
    else
        charTechLevel = charTechLevelRet:GetData(0,0)
        StmtOperater._UpdateCharTechLevel:ExecStat(tech_level,charID, techIndex)   
    end
    local result = {}
    result["CurTechLevel"] = tech_level
    result["PreTechLevel"] = charTechLevel
    result["MsgID"] = 9059
    return true,result
end
-------------------------------------------------
SetDbLocalFuncType(CTongTechBox.SetTongTechLevelGM)
SetDbLocalFuncType(CTongTechBox.SetCharTechLevelGM)
return CTongTechBox