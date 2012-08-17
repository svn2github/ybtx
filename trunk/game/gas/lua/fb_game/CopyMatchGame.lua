CCopyMatchGame = class()

g_ActivityPanelCount = 200
g_ActivityPanelTickTime = 30000
g_ActivityPanelMaxTimes = 30

function OpenActivityPanel(gameName, recordTbl)
	recordTbl.m_Times = recordTbl.m_Times + 1
	local data = {}
	data["MaxTimes"] = FbActionDirect_Common(gameName, "MaxTimes")
	data["MinLevel"] = tonumber(FbActionDirect_Common(gameName, "MinMsgLevel"))
	data["MaxLevel"] = tonumber(FbActionDirect_Common(gameName, "MaxMsgLevel"))
	data["CueOnce"] = FbActionDirect_Common(gameName, "CueOnce")
	data["gameName"] = gameName
	local function CallBack(result, flag)
		if result then
			local num = result:GetRowNum() 
			local NotifyCount = 0
			for i = 0, num - 1 do
				local charId = result:GetData(i,0)
				if not recordTbl[charId] then
					local times =result:GetData(i,1)
					local fbSceneId = result:GetData(i,2)
					local changeSceneId = result:GetData(i,3)
			
					if flag then
						local count = result:GetData(i,4)
						if fbSceneId == 0 and changeSceneId == 0 then
							if times == "0" then
								NotifyCount = NotifyCount + 1
								recordTbl[charId] = true
								Gas2GacById:CreateMatchGameWnd(charId, gameName)
							else
								if JoinCountLimit_Server(data["MaxTimes"],"Cycle") == "day" then
									if count < JoinCountLimit_Server(data["MaxTimes"],"Count") then
										NotifyCount = NotifyCount + 1
										recordTbl[charId] = true
										Gas2GacById:CreateMatchGameWnd(charId, gameName)
									end
								end
							end
						end
					else
						if times == 0 and fbSceneId == 0 and changeSceneId == 0 then
							NotifyCount = NotifyCount + 1
							recordTbl[charId] = true
							Gas2GacById:CreateMatchGameWnd(charId, gameName)
						end
					end
					
					if NotifyCount >= g_ActivityPanelCount then  --最多一次通知g_ActivityPanelCount人
						break
					end
				end
				
			end
			
			if NotifyCount >= g_ActivityPanelCount and recordTbl.m_Times < g_ActivityPanelMaxTimes then --未通知完
				RegisterOnceTick(g_App, "ActivityPanelTick",  OpenActivityPanel, g_ActivityPanelTickTime, gameName, recordTbl)
			end
			
		end
	end
	CallDbTrans("MultiServerDB", "GetOnlinePlayerUsedAct", CallBack, data, gameName)
end


function OpenActivityPanelTask(gameName)
	local recordTbl = {} --记录哪些人被通知过了
	recordTbl.m_Times = 0
	OpenActivityPanel(gameName, recordTbl)
end



function CCopyMatchGame.AlarmClock()
	if GasConfig.GasType == 1 then
		return
	end
	for _, index in pairs(FbActionDirect_Common:GetKeys()) do
		local triggerTime = GetCfgTransformValue(false, "FbActionDirect_Common", index, "TriggerTime") 
		if triggerTime and triggerTime ~= "" then
			g_AlarmClock:AddTask("AlarmMatchGame", triggerTime, OpenActivityPanelTask, nil, index)
		end
	end
end
