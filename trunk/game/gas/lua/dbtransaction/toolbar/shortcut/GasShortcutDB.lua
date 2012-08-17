gac_gas_require "shortcut/ShortcutCommon"
cfg_load "skill/InitSkill_Common"
cfg_load "skill/SkillPart_Common"
cfg_load "skill/NonFightSkill_Common"
gac_gas_require "skill/SkillCommon"
gac_gas_require "player/PlayerMgr"

local EShortcutPieceState = EShortcutPieceState
local FightSkillKind = FightSkillKind
local InitSkill_Common = InitSkill_Common
local SkillPart_Common = SkillPart_Common
local NonFightSkill_Common = NonFightSkill_Common
local g_ItemInfoMgr = g_ItemInfoMgr
local g_GetPlayerClassNameByID = g_GetPlayerClassNameByID
local ShortCutDB = CreateDbBox(...)
local ShortCutSql = {}

--保存快捷栏图标
local StmtDef=
{
	"Player_SaveShortcut",
	[[
	replace into tbl_shortcut(cs_uId,sc_uPos,sc_Type, sc_Arg1, sc_Arg2, sc_Arg3) values(?,?,?,?,?,?);
	]]
}

DefineSql(StmtDef,ShortCutSql)

function ShortCutDB.Player_SaveShortcut(CharID, Pos, Type, Name, Level, IsMainBar)
	ShortCutSql.Player_SaveShortcut:ExecSql("", CharID, Pos, Type, Name, Level, IsMainBar)
end

--删除快捷栏图标
local StmtDef=
{
	"Player_DeleteShortcut",
	[[
	delete from tbl_shortcut where cs_uId = ? and sc_uPos = ? and sc_Arg3 = ?
	]]
}

DefineSql(StmtDef,ShortCutSql)

function ShortCutDB.Player_DeleteShortcut(CharID, Pos,IsMainBar)
	ShortCutSql.Player_DeleteShortcut:ExecSql("", CharID, Pos,IsMainBar)
end

function ShortCutDB.SavePlayerShortCut(Data)
	for i = 1,#(Data) do
		local pos = Data[i]["Pos"]
		local type = Data[i]["Type"]
		local arg1 = Data[i]["Name"]
		local arg2 = Data[i]["Level"]
		local arg3 = Data[i]["IsMainBar"]
		local CharID = Data[i]["PlayerId"]
	
		if(type == EShortcutPieceState.eSPS_None) then --该格子已无图标，故将其从数据库中删除（俺们的口号是：只存有的！）
			ShortCutDB.Player_DeleteShortcut(CharID, pos,arg3)
		else --有图标，那就存
			ShortCutDB.Player_SaveShortcut(CharID, pos, type, arg1, arg2, arg3)
		end
	end
end

--更新数据库中普攻技能的名称
local StmtDef=
{
	"Player_UpDataShortcut",
	[[
	update tbl_shortcut set sc_Arg1 = ? where sc_Arg1 = ? and cs_uId = ? 
	]]
}

DefineSql(StmtDef,ShortCutSql)

local StmtDef=
{
	"Dbs_UpdateFightSkill",
	[[
	update tbl_fight_skill set fs_sName = ? where cs_uId = ? and  fs_uKind = ? and fs_sName = ?
	]]
}

DefineSql(StmtDef,ShortCutSql)

function ShortCutDB.UpDateShortcut(PlayerId,new_date,old_date)
	ShortCutSql.Player_UpDataShortcut:ExecSql( "", new_date, old_date, PlayerId)
	ShortCutSql.Dbs_UpdateFightSkill:ExecSql( "", new_date, PlayerId, FightSkillKind.NormSkill ,old_date)
end

--得到快捷栏所有数据信息
local StmtDef=
{
	"Player_GetShortcut",
	[[
	select sc_uPos,sc_Type,sc_Arg1,sc_Arg2,sc_Arg3 from tbl_shortcut where cs_uId=?;
	]]
}
DefineSql( StmtDef , ShortCutSql )

function ShortCutDB.GetShortcutInfo(PlayerId)
	local ret = ShortCutSql.Player_GetShortcut:ExecStat(PlayerId)	
	return ret
end
------------------------------------------------------------------------------------
local MagicWarriorTbl = {
							["火焰武器"] = "2",
							["冰霜武器"] = "3",
							}
							
local OrcWarriorTbl = {
							["猛犸姿态"] = "2",
							["雄狮姿态"] = "3"
							}
							
local StmtDef=
{
	"_GetCharEquipInfo",
	[[
		select 
			ie.is_uId,iss.is_sName,iss.is_uType
		from 
			tbl_item_equip ie,tbl_item_static iss
		where
			ie.is_uId = iss.is_uId and cs_uId = ? and ie.ce_uEquipPart = 1
	]]
}
DefineSql( StmtDef , ShortCutSql )		


local StmtDef = 
{
	"Dbs_SelectFightSkill",
	[[
		select fs_uLevel from tbl_fight_skill
		where cs_uId = ? and fs_sName = ? and fs_uKind = ?
	]]
}					
DefineSql( StmtDef , ShortCutSql )	

							
function ShortCutDB.InitSkillState(PlayerId,skillName,uClass)
	local tblFirstSkill = nil
	local uPos = 0
	local index = nil
	if uClass == 2 then
		if MagicWarriorTbl[skillName] then
			tblFirstSkill = InitSkill_Common(tostring(uClass),MagicWarriorTbl[skillName])
			index = tonumber(MagicWarriorTbl[skillName])
			uPos = (index - 1) * 10 +1
		end
	elseif uClass == 9 then
		if OrcWarriorTbl[skillName] then
			tblFirstSkill = InitSkill_Common(tostring(uClass),OrcWarriorTbl[skillName])
			index = tonumber(OrcWarriorTbl[skillName])
			uPos = ( index- 1) * 10 +1
		end
	end
	
	if tblFirstSkill then
		local arg2 = 1
		local arg3 = 1
		for i = 2, 10 do
			local skillInfo	= tblFirstSkill("SkillName" .. i)
			if( skillInfo and "" ~= skillInfo ) then
				local name		= skillInfo
				local type		= EShortcutPieceState.eSPS_Skill
				local pos		= ((i - 1)%10 + 1) + (index - 1) * 10
				local isSkill = SkillPart_Common(name)
				local skillKind = 0
				if( isSkill ) then
					skillKind = 1
				elseif NonFightSkill_Common(name) then
					skillKind = 3
				end
				
				if skillKind ~= 0 then
					local res = ShortCutSql.Dbs_SelectFightSkill:ExecStat(PlayerId,name,skillKind)
					if res:GetRowNum() >  0 then
						arg2 = res:GetData(0,0)
						ShortCutDB.Player_SaveShortcut(PlayerId, pos, type, name, arg2, arg3)
					end
				end
			end
		end
		local res = ShortCutSql._GetCharEquipInfo:ExecStat(PlayerId)
		local CommonlySkill
		if res:GetRowNum() > 0 then
			CommonlySkill = g_ItemInfoMgr:GetCommonlySkill(uClass,res(1,3),res(1,2))
		else
			CommonlySkill = g_GetPlayerClassNameByID(uClass).. "普通攻击"
		end
		ShortCutDB.Player_SaveShortcut(PlayerId,uPos, 2, CommonlySkill, 1, 1)
		return ShortCutDB.GetShortcutInfo(PlayerId)
	end
	return nil
end

SetDbLocalFuncType(ShortCutDB.SavePlayerShortCut)

return ShortCutDB