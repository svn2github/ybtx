
--判断 点击Npc(Obj)的距离
function CheckDistance(Player, CoreObj, distance)
	if distance == nil or distance == "" or g_GetDistance(Player,CoreObj) <= distance then
		return true	
	else
		return false	
	end
end

--判断等级需求(传进的参数为"" 或nil时 代表无要求)
function CheckPlayerLev(playerlev ,minlev, maxlev)
	local checkmin = false
	local checkmax = false
	if minlev == "" or minlev == nil then
		checkmin = true
	end
	if maxlev == "" or maxlev == nil then
		checkmax = true
	end
	if playerlev >= minlev then
		checkmin = true
	end
	if playerlev <= maxlev then
		checkmax = true
	end
	if checkmin and checkmax then
		return true
	end
	return false
end

--function CheckQuestRequireVar(Player,ItemInfo)
--	local mRequireTbl = assert(loadstring("return {" .. ItemInfo.QuestRequire .."}"))()
--	local mResultTbl = assert(loadstring("return {" .. ItemInfo.QuestResult .."}"))()
--	local PlayerQuestTbl = Player.m_tblDoingQuests
--	local mQuestRequire = PlayerQuestTbl[mRequireTbl[1]]
--	local mQuestResult = PlayerQuestTbl[mResultTbl[1]]
--	local QuestVarMaxNum = g_QuestNeedMgr[mResultTbl[1]][mResultTbl[2]].Num
--	
--	if mQuestRequire == nil or mQuestResult == nil or QuestVarMaxNum == nil then
--		return false
--	end
--	if mQuestRequire[mRequireTbl[2]] == nil or mQuestResult[mResultTbl[2]] == nil then
--		return false
--	end
--	if mQuestRequire[mRequireTbl[2]] < mRequireTbl[3]
--		or mQuestResult[mRequireTbl[2]] >= QuestVarMaxNum then
--		return false
--	end
--	return true
--end

--function CheckQuestRequire(Player,ItemInfo)
--	local RequireQuestTbl = {}
--	if ItemInfo.QuestRequire ~= "" then
--		RequireQuestTbl = assert(loadstring("return " .. ItemInfo.QuestRequire))()
--	end
--	--判断物品使用时的任务需求是否满足
--	local PlayerQuestTbl = Player.m_tblDoingQuests
--	local RequireNum = 0
--	for i = 1, #(RequireQuestTbl) do
--		for j,p in pairs(PlayerQuestTbl) do
--			if RequireQuestTbl[i] == j then
--				RequireNum = RequireNum + 1
--			end
--		end
--	end
--	if (RequireNum ~= table.getn(RequireQuestTbl)) then
--		return false
--	end
--	return true
--end

function IsNpc(Npc)
	if not Npc then
		return
	end
	if not Npc.m_Properties then
		return
	end
	local Type = Npc.m_Properties:GetType()
	if Type == ECharacterType.Npc then
		return true
	end
end

function IsObj(Obj)
	if not Obj then
		return
	end
	if not Obj.m_Properties then
		return
	end
	local Type = Obj.m_Properties:GetType()
	if Type == ECharacterType.IntObj then
		return true
	end
end

function IsTrap(Trap)
	if not Trap then
		return
	end
	if not Trap.m_Properties then
		return
	end
	local Type = Trap.m_Properties:GetType()
	if Type == ECharacterType.Trap then
		return true
	end
end

function IsPlayer(Player)
	if not Player then
		return
	end
	if not Player.m_Properties then
		return
	end
	local Type = Player.m_Properties:GetType()
	if Type == ECharacterType.Player then
		return true
	end
end