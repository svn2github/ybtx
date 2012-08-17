--gac_gas_require "activity/npc/CheckNpcTalk"

local g_NpcTalkTbl = g_NpcTalkTbl
local g_MercenaryLevelTbl = g_MercenaryLevelTbl

local StmtContainer = class()

local StmtDef = 
{
	"_AddFinishNpcTalk",
	[[
		insert into tbl_finish_npc_talk
			(cs_uId, fnt_sTalkName)
		values
			(?,?)
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_CheckFinishNpcTalk",
	[[
		select *
		from
			tbl_finish_npc_talk
		where
			cs_uId = ? and fnt_sTalkName = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local NpcTalkDB = CreateDbBox(...)


function NpcTalkDB.AddFinishNpcTalk(data)
	local IsFinish = StmtContainer._CheckFinishNpcTalk:ExecSql("ns",CharId,TalkName)
	if not (IsFinish and IsFinish:GetRowNum()>0) then
		StmtContainer._AddFinishNpcTalk:ExecSql("", data["CharId"], data["TalkName"])
	end
end

function NpcTalkDB.CheckFinishNpcTalk(data)
	local CharId = data["CharId"]
	local Camp = data["Camp"]
	local SceneName = data["SceneName"]
	local NpcName = data["NpcName"]
	local TalkTbl = {}
	for name,tbl in pairs(g_NpcTalkTbl) do
		if tbl["触发Npc"].Param1[Camp] == NpcName then
			table.insert( TalkTbl, name )
		end
	end
	
	--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
	--local MercenaryLevel, Status = MercenaryLevelDB.GetMercenaryLevelInfo(data)
	--if MercenaryLevel == g_MercenaryLevelTbl.MaxLevel then
	--	return ""
	--end
	for i=1, #TalkTbl do
		local TalkName = TalkTbl[i]
		local IsFinish = StmtContainer._CheckFinishNpcTalk:ExecSql("ns",CharId,TalkName)
		if not (IsFinish and IsFinish:GetRowNum()>0) then
			for Subject, tbl in pairs( g_NpcTalkTbl[TalkName] ) do
				if Subject == "佣兵等级" then
					TalkName = ""
				--	if tbl.Param1 ~= "佣兵登记" then
				--		if not MercenaryLevel then
				--			TalkName = ""
				--		else
				--			local LevelName = g_MercenaryLevelTbl[MercenaryLevel].LevelName
				--			local StatusTbl = {"基础达成","挑战达成","评定","特权"}
				--			if LevelName~=tbl.Param1 or (tbl.Param2 and StatusTbl[Status]~=tbl.Param2) then
				--				TalkName = ""
				--			end
				--		end
				--	end
				elseif Subject == "触发场景" then
					if SceneName ~= tbl.Param1 then
						TalkName = ""
					end
				end
			end
			if TalkName ~= "" then
				return TalkName
			end
		end
	end
	return ""
end

return NpcTalkDB
