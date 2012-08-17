local loadstring = loadstring

local StmtContainer = class()

local StmtDef=
{
	"InsertRecord",
	[[
	insert into 
		tbl_extra_reward_count(cs_uId, erc_sActivityName, erc_dtTime) 
		values(?,?,now())
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetExtraRewardCount",
	[[
	select 
		count(*) 
	from 
		tbl_extra_reward_count
	where
		cs_uId = ? and erc_sActivityName = ? and erc_dtTime >= from_unixtime(?)
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"ClearPreteritData",
	[[
	delete from  
		tbl_extra_reward_count 
	where 
		erc_dtTime < from_unixtime(?)
	]]
}
DefineSql(StmtDef,StmtContainer)

local ExtraRewardDbBox = CreateDbBox(...)

function ExtraRewardDbBox.GetExtraRewardCount(playerId, actionName, criticalTime)
	local result = StmtContainer.GetExtraRewardCount:ExecSql("n", playerId, actionName, criticalTime)
	local count = result:GetData(0,0)
	result:Release()
	return count
end

local CurCriticalTime
function ExtraRewardDbBox.GetExtraRewardCountInfo(data)
	local uCharId = data["PlayerId"]
	local curActionName = data["ActionName"]
	local criticalTime = data["CriticalTime"]
	local actionTbl = data["ActionTbl"]
	
	if CurCriticalTime ~= criticalTime then
		ExtraRewardDbBox.ClearPreteritData(criticalTime)
		CurCriticalTime = criticalTime
	end
	
	local curActionCount = 0
	local allActionCount = 0
	for _, name in pairs(actionTbl) do
		if name == curActionName then
			curActionCount = ExtraRewardDbBox.GetExtraRewardCount(uCharId, name, criticalTime)
			allActionCount = allActionCount + curActionCount
		else
			allActionCount = allActionCount + ExtraRewardDbBox.GetExtraRewardCount(uCharId, name, criticalTime)
		end
	end
	return curActionCount, allActionCount
end


function ExtraRewardDbBox.AddExtraRewardCount(data)
	local uCharId = data["PlayerId"]
	local actionName = data["ActionName"]
	local maxCount = data["MaxCount"]
--	print("增加一次活动记录 " .. actionName)
	
	local curActionCount, allActionCount = ExtraRewardDbBox.GetExtraRewardCountInfo(data)
	if curActionCount < maxCount then
		StmtContainer.InsertRecord:ExecSql("", uCharId, actionName)
	end
	return curActionCount, allActionCount
end

function ExtraRewardDbBox.ClearPreteritData(criticalTime)
	StmtContainer.ClearPreteritData:ExecSql("", criticalTime)
end


SetDbLocalFuncType(ExtraRewardDbBox.AddExtraRewardCount)
return ExtraRewardDbBox