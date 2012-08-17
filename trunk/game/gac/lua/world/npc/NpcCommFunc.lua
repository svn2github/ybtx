local NpcAndPlayerType = 
{
	["普通NPC"]				= -1,
	["接取任务"]			= 5,	--因为c++中非功能npc和玩家又有仔细的区分，从0~4，所以功能npc从5开始继续
	["完成任务"] 			= 6,
	["任务进行中"]          = -1
}

function GetFuncTypeByNpcName(NpcName)
	local npcType, TexSrc = CQuest.GetFuncNameByNpcName(NpcName)
	
	if npcType == nil then
		return NpcAndPlayerType["普通NPC"]
	end
	if IsNumber(npcType) then
		if npcType == 0 then
				return NpcAndPlayerType["普通NPC"]--2代表传给c++的值（enemyNpc or friendNpc）
		else
			return npcType, TexSrc
		end
	else
		return NpcAndPlayerType[npcType]
	end
end