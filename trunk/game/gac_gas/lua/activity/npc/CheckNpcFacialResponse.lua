cfg_load "npc/Npc_Common"


local ErrorNum = 0					--yy 输入 出错误信息
local function error_print(flags, str)
	if not flags then
		print( str)
		ErrorNum = ErrorNum + 1
	end 
end 

local function BeinActionTbl(ActionName)
	local ActionState = CActorCfg_GetEnumByString(ActionName)
	if ActionState ~= EActionState.eAS_Error then
		return true
	else
		return false
	end
end

local ConditionTbl = {
					 	["任务"] = 1,
					 	["等级"] = 2,
					 	["普通"] = 3,
					 }

--function CheckNpcFacialResponse()
--	ErrorNum = 0
--	for i, v in pairs(NpcFacialResponse_Server) do
--		if BeinActionTbl(v["Action"]) then
--			if ConditionTbl[v["Condition"]] then
--				if v["Condition"] == "任务" then
--					if v["ConditionArg"] == "" then
--						local err_msg = "配置表【NpcFacialResponse_Server.xml】中 【" .. v["NPCName"] .. "】 的 【"  .. v["Condition"] .. "】 的【ConditionArg】项必须有任务参数！"
--						error_print( false, err_msg)
--					end
--				elseif v["Condition"] == "等级" then
--					if tonumber(v["ConditionArg"]) == nil then
--						local err_msg = "配置表【NpcFacialResponse_Server.xml】中 【" .. v["NPCName"] .. "】 的 【"  .. v["Condition"] .. "】 的【ConditionArg】项必须是大于0的数字，不能为空，请查实！"
--						error_print( false, err_msg)
--					end 
--				end 
--				
--				for i = 1, 4 do
--					local Reaction = "Reaction" .. i
--					local ReactionArg = "ReactionArg" .. i
--					if v[Reaction] == "说话" then
--						if tostring(v[ReactionArg]) == nil or tostring(v[ReactionArg]) == "" then
--							local err_msg = "配置表【NpcFacialResponse_Server.xml】中 【" .. v["NPCName"] .. "】 的 【"  .. ReactionArg .. "】 的内容不能为空！"
--							error_print( false, err_msg)
--						end	
--					elseif v[Reaction] == "表情" then
--						if BeinActionTbl(v[ReactionArg]) == false then
--							local err_msg = "配置表【NpcFacialResponse_Server.xml】中 【" .. v["NPCName"] .. "】 的 【"  .. v[ReactionArg] .. "】 动作不在NPC动作配置表中，请查实！"
--							error_print( false, err_msg)
--						end
--					elseif v[Reaction] == "改阵营" then
--						local info = loadstring("return " .. v["ReactionArg" .. tostring(i)])()
--						if type(info) ~= "table" then
--							local err_msg = "配置表【NpcFacialResponse_Server.xml】中 【" .. v["NPCName"] .. "】 的 【"  .. ReactionArg .. "】 的内容填写错误，请查实！"
--							error_print( false, err_msg)
--						end
--					elseif v[Reaction] == "变身" then
--						local Arg = assert(loadstring("return " .. v[ReactionArg]))()
--						if Npc_Common(Arg[1]) == nil then
--							local err_msg = "配置表【NpcFacialResponse_Server.xml】中 【" .. v["NPCName"] .. "】 的 【"  .. ReactionArg .. "】 所创造NPC不再【Npc_Common】配置表中，请查实！"
--							error_print( false, err_msg)
--						end
--					elseif v[Reaction] == "奖励物品" then
--					elseif v[Reaction] == "奖励声望" then
--					elseif v[Reaction] == "奖励友好度" then
--					elseif v[Reaction] == "奖励经验" then				
--					end
--				end
--				
--			else
--				local err_msg = "配置表【NpcFacialResponse_Server.xml】中 【" .. v["NPCName"] .. "】 的 【" .. v["Condition"] .. "】 填写错误，请查实！"	
--				error_print( false, err_msg)
--			end
--		else
--			local err_msg = "配置表【NpcFacialResponse_Server.xml】中 【" .. v["NPCName"] .. "】 的【" .. v["Action"] .. "】 动作不是玩家包含的动作，请查实！"	
--			error_print( false, err_msg)
--		end
--	end
--	if ErrorNum ~= 0 then
--		error("NPC表情配置表检查错误如上")
--	end 
--	print("NPC表情动作配置表检查完毕！")
--end