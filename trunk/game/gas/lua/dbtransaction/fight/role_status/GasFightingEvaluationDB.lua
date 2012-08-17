
local CGasFightingEvaluationSql = class()

local GasFightingEvaluationDB = CreateDbBox(...)
-------------------------------------------------------------------------
local StmtDef =
{
	"_GetLeftHandDPS",
	"select iw.iw_uDPS from tbl_item_equip ie, tbl_item_weapon iw	where ie.is_uId = iw.is_uId and ie.cs_uId = ? and ie.ce_uEquipPart = 2"
}
DefineSql( StmtDef , CGasFightingEvaluationSql )	
	
local StmtDef =
{
	"_GetRightHandDPS",
	"select iw.iw_uDPS from tbl_item_equip ie, tbl_item_weapon iw	where ie.is_uId = iw.is_uId and ie.cs_uId = ? and ie.ce_uEquipPart = 1"
}
DefineSql( StmtDef , CGasFightingEvaluationSql )	

local StmtDef =
{
	"_GetIntensifyPoint",
	"select ifnull(sum(iei.iei_uIntensifyPhase),0) from tbl_item_equip ie, tbl_item_equip_intensify iei	where ie.is_uId = iei.is_uId and ie.cs_uId = ? and iei.iei_uIntensifyPhase != 0 "
}
DefineSql( StmtDef , CGasFightingEvaluationSql )	
	
function GasFightingEvaluationDB.GetFightingEvaluationDB(data)
	local charId = data["uCharID"]
	local EquipIntensifyDB = RequireDbBox("EquipIntensifyDB")
	local suitCountTbl = EquipIntensifyDB.GetSuitCountByCharID(charId)
	local uDPS,uIntensifyPoint = 0,0
	local res1 = CGasFightingEvaluationSql._GetLeftHandDPS:ExecStat(charId)
	local res2 = CGasFightingEvaluationSql._GetRightHandDPS:ExecStat(charId)
	local res3 = CGasFightingEvaluationSql._GetIntensifyPoint:ExecStat(charId)
	if res1:GetRowNum() > 0 then
		uDPS = uDPS + res1:GetData(0,0) * 0.5
	end
	if res2:GetRowNum() > 0 then
		uDPS = uDPS + res2:GetData(0,0)
	end
	if res3:GetRowNum() > 0 then
		uIntensifyPoint = uIntensifyPoint + res3:GetData(0,0)
	end

	return suitCountTbl,uDPS,uIntensifyPoint
end
-------------------------------------------------------------------
local StmtDef =
{
	"_SaveFightingEvaluationPoint",
	"replace into tbl_char_fighting_evaluation values(?,?)"
}
DefineSql( StmtDef , CGasFightingEvaluationSql )


--得到玩家的等级
local StmtDef =
{
	"_GetPlayerLevel",
	"select cb_uLevel from tbl_char_basic where cs_uId = ? "
}
DefineSql( StmtDef , CGasFightingEvaluationSql )		

function GasFightingEvaluationDB.SaveFightingEvaluation(data)
	local charId = data["charId"]
	local uTotalPoint = data["uTotalPoint"]

	CGasFightingEvaluationSql._SaveFightingEvaluationPoint:ExecStat(charId,uTotalPoint)
	local level_res = CGasFightingEvaluationSql._GetPlayerLevel:ExecStat(charId)
	local LogMgr = RequireDbBox("LogMgrDB")
	LogMgr.SaveFightingEvaluation(charId,uTotalPoint,level_res:GetData(0,0))
end
-------------------------------------------------------------------
function GasFightingEvaluationDB.SelectFightingEvaluationByCharIDTbl(CharIDTbl)
	if CharIDTbl and IsTable(CharIDTbl) then
		local Str = 
		[[
			select
			cs_uId, 
			cfe_uPoint
			from 
			tbl_char_fighting_evaluation
			where 
			cs_uId in 
		]]
		Str = Str .. "(" .. table.concat(CharIDTbl,",") .. ");"
		local _,res = g_DbChannelMgr:TextExecute(Str)
		return res
	end
end
-------------------------------------------------------------------
local StmtDef =
{
	"_SaveEquipEffectPlayerId",
	[[
	insert into tbl_char_equip_effect
	select 
		cs_uId,sfe_uOrder 
	from 
		tbl_sort_fight_evaluation 
	where 
		sfe_uOrder <= 150 and sfe_uPoint >= 300
	]]
}
DefineSql( StmtDef , CGasFightingEvaluationSql )	

local StmtDef =
{
	"_DelEquipEffectPlayerId",
	[[
		delete from tbl_char_equip_effect 
	]]
}
DefineSql( StmtDef , CGasFightingEvaluationSql )	

local StmtDef =
{
	"_GetEquipEffectPlayerId",
	[[
	select 
		cee.cs_uId,cee_uOrder 
	from 
		tbl_char_equip_effect cee,tbl_char_online co 
	where 
		cee.cs_uId = co.cs_uId
	]]
}
DefineSql( StmtDef , CGasFightingEvaluationSql )

--@brief 更新战斗力排行前150玩家的装备特效
function GasFightingEvaluationDB.UpdateEquipEffectDB()
	local old_res = CGasFightingEvaluationSql._GetEquipEffectPlayerId:ExecStat()
	CGasFightingEvaluationSql._DelEquipEffectPlayerId:ExecStat()
	CGasFightingEvaluationSql._SaveEquipEffectPlayerId:ExecStat()
	local new_res = CGasFightingEvaluationSql._GetEquipEffectPlayerId:ExecStat()
	
	return old_res,new_res
end
-------------------------------------------------------------------
local StmtDef =
{
	"_GetEquipEffectInfo",
	[[
	select 
		cee_uOrder 
	from 
		tbl_char_equip_effect
	where 
		cs_uId = ? 
	]]
}
DefineSql( StmtDef , CGasFightingEvaluationSql )		

--@brief 更新战斗力排行前150玩家的装备特效
function GasFightingEvaluationDB.SelectEquipEffectDB(data)
	local charId = data["char_id"]
	local res = CGasFightingEvaluationSql._GetEquipEffectInfo:ExecStat(charId)
	return res
end

SetDbLocalFuncType(GasFightingEvaluationDB.SelectFightingEvaluationByCharIDTbl)
SetDbLocalFuncType(GasFightingEvaluationDB.GetFightingEvaluationDB)

return GasFightingEvaluationDB
