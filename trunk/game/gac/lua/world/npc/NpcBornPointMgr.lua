gac_require "world/npc/NpcBornPointMgrInc"
gac_require "world/area/AreaTransport"
gac_require "activity/playerautotrack/TransportMap"
cfg_load "npc/Npc_Common"
cfg_load "int_obj/IntObj_Common"
cfg_load "scene/Transport_Server"
cfg_load "scene/Trap_Common"
gac_gas_require "scene_mgr/SceneCfg"
gac_gas_require "activity/Trigger/NpcTrigger"

g_NpcBornMgr = CNpcBornPointMgr:new()
g_NpcBornMgr.m_FunNpcBornPos = {}
g_NpcBornMgr.m_MailBoxBornPos = {} -- 邮箱位置
g_NpcBornMgr.m_MercQuestBornPos = {} -- 日常任务发布榜位置
g_NpcBornMgr.m_RobResBornPos = {}
g_IntObjServerMgr = CNpcBornPointMgr:new()
g_TrapServerMgr = CNpcBornPointMgr:new()
g_BetweenSceneTransportTrapPos = {} --地图间传送Trap点
g_SceneIntestineTransportTrapPos = {} --地图内传送Trap点

function CNpcBornPointMgr:AddSinglePointBornTbl(key, tbl)
	self.m_FunNpcBornPos[key] = {}
	for i, postbl in pairs(tbl) do
		local NpcName = postbl.Name
		if Npc_Common(NpcName) ~= nil then
			local funcnametbl = Npc_Common(NpcName,"FuncName")
			if funcnametbl ~= "" then
				funcnametbl = loadstring("return {"..funcnametbl.."}")()
				local funcname = funcnametbl[1]
				local funindex = CNpcDialogBox.DlgTextType[funcname]
				if (funindex~=nil and funindex>=2) or funcname == "特殊NPC" then  -- "特殊NPC"里有一种为传送NPC,见表designer_NpcShowContent_Common.dif
					if not self.m_FunNpcBornPos[key][NpcName] then
						self.m_FunNpcBornPos[key][NpcName] = {}
					end
					table.insert(self.m_FunNpcBornPos[key][NpcName],{math.floor(postbl.GridX),math.floor(postbl.GridY)})
					--self.m_FunNpcBornPos[key][NpcName] = {postbl.GridX,postbl.GridY}
				end
			end
		end
	end
end

function CNpcBornPointMgr:AddIntObjTbl(key, tbl)
	--self.m_MailBoxBornPos = {} -- 邮箱位置
	if not g_NpcBornMgr.m_RobResBornPos[key] then
		g_NpcBornMgr.m_RobResBornPos[key] = {}
	end
	for i, postbl in pairs(tbl) do
		local ObjName = postbl.Name
		if ObjName == "邮箱" then
			if g_NpcBornMgr.m_MailBoxBornPos[key] == nil then			
				g_NpcBornMgr.m_MailBoxBornPos[key] = { {math.floor(postbl.GridX), math.floor(postbl.GridY)} }
			else
				table.insert( g_NpcBornMgr.m_MailBoxBornPos[key], {math.floor(postbl.GridX), math.floor(postbl.GridY)} )
			end
		elseif string.find(ObjName, "日常任务发布榜") then
			if g_NpcBornMgr.m_MercQuestBornPos[key] == nil then
				g_NpcBornMgr.m_MercQuestBornPos[key] = {}
			end
			table.insert(g_NpcBornMgr.m_MercQuestBornPos[key], {math.floor(postbl.GridX), math.floor(postbl.GridY)})
		elseif string.find(ObjName, "资源点") then
			if g_NpcBornMgr.m_RobResBornPos[key][ObjName] == nil then
				g_NpcBornMgr.m_RobResBornPos[key][ObjName] = {}
			end
			table.insert(g_NpcBornMgr.m_RobResBornPos[key][ObjName], {math.floor(postbl.GridX), math.floor(postbl.GridY)})

		end
	end -- end for
end
function CNpcBornPointMgr:AddTrapTbl(key, tbl)
	for i, postbl in pairs(tbl) do
		local TrapName = postbl.Name
		if Transport_Server(TrapName) then
			for _, i in pairs(Transport_Server:GetKeys(TrapName, "陷阱")) do
				local TransInfo = Transport_Server(TrapName, "陷阱", i.."")
				if TransInfo("BeginScene") ~= ""
					and Scene_Common[TransInfo("BeginScene")].NpcSceneFile == key then
					
					local TriggerCondition = GetTriggerCondition("Trap", TrapName, "进", "任务需求")
					local QuestNameTbl = TriggerCondition and TriggerCondition.Arg
					local QuestName = QuestNameTbl and QuestNameTbl[1]
					QuestName = QuestName or ""
					
					local ConditionTbl = {["Lev"] = TransInfo("LeastLev"),["Quest"] = QuestName,["Camp"] = TransInfo("Camp")}
					local BeginPos = CPos:new(postbl.GridX, postbl.GridY)
					local bArea = GetSceneTransportAreaName(TransInfo("BeginScene"), BeginPos)
					
					local EndPos = CPos:new(TransInfo("PosX"), TransInfo("PosY"))
					local eArea = GetSceneTransportAreaName(TransInfo("EndScene"), EndPos)
					g_CTransportMap:AddConnected(bArea, eArea, BeginPos, EndPos, ConditionTbl)
					
					
					local beginpostbl = {postbl.GridX,postbl.GridY,["ConditionTbl"] = ConditionTbl}
					local endpostbl = {TransInfo("PosX"),TransInfo("PosY")}
					
					if TransInfo("BeginScene") ~= TransInfo("EndScene") then
						if not g_BetweenSceneTransportTrapPos[TransInfo("BeginScene")] then
							g_BetweenSceneTransportTrapPos[TransInfo("BeginScene")] = {}
						end
						
						if not g_BetweenSceneTransportTrapPos[TransInfo("BeginScene")][TransInfo("EndScene")] then
							g_BetweenSceneTransportTrapPos[TransInfo("BeginScene")][TransInfo("EndScene")] = {}
						end
						table.insert(g_BetweenSceneTransportTrapPos[TransInfo("BeginScene")][TransInfo("EndScene")],{beginpostbl,endpostbl})
					end
					
				end
			end
		end
	end
end
