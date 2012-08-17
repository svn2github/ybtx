
local StmtContainer = class()

local StmtDef=
{
	"_GetPlayerFirstTimeInfo",
	"select nd_sFirstTime from tbl_novice_direct where cs_uId = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_GetPlayerFirstTimeCount",
	"select count(*) from tbl_novice_direct where cs_uId = ? and nd_sFirstTime = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_InsertPlayerFirstFinishInfo",
	"insert into tbl_novice_direct (cs_uId, nd_sFirstTime) values (?,?)"
}
DefineSql(StmtDef, StmtContainer)

local NoviceDirect = CreateDbBox(...)


function NoviceDirect.GetPlayerFirstTimeInfo(PlayerId)
	local FirstTimeTbl = nil
	if PlayerId then
		
		local result = StmtContainer._GetPlayerFirstTimeInfo:ExecSql('s[32]',PlayerId)
		if result then
			local RowNum = result:GetRowNum()
			if RowNum > 0 then
				FirstTimeTbl = {}
				for i=1, RowNum do
					table.insert(FirstTimeTbl,result:GetData(i-1,0))
				end
			end
			result:Release()
		end
		
	end
	return FirstTimeTbl
end


function NoviceDirect.GetPlayerFirstTimeCount(PlayerId,FirstTimeName)
	local result = StmtContainer._GetPlayerFirstTimeCount:ExecStat(PlayerId,FirstTimeName)
	return result:GetData(0,0)
end

function NoviceDirect.InsertPlayerFirstFinishInfo(data)
	local PlayerId = data["PlayerId"]
	local FirstTimeName = data["FirstTimeName"]
	if PlayerId and FirstTimeName then
		StmtContainer._InsertPlayerFirstFinishInfo:ExecSql("",PlayerId,FirstTimeName)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
end

SetDbLocalFuncType(NoviceDirect.GetPlayerFirstTimeCount)
SetDbLocalFuncType(NoviceDirect.InsertPlayerFirstFinishInfo)
return NoviceDirect
