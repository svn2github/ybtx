cfg_load "npc/NpcTalk_Common"
lan_load "npc/Lan_NpcTalk_Common"

local function GetNpcTalkCfg()
	local tbl = {}
	
	for _, TalkName in pairs(NpcTalk_Common:GetKeys()) do
		for i, key in pairs(NpcTalk_Common(TalkName):GetKeys()) do
			local NpcTalkCommon = NpcTalk_Common(TalkName)
			local str = i..""
			local Subject = NpcTalkCommon(str, "Subject")
			local Param1 = NpcTalkCommon(str, "Param1")
			local Param2 = NpcTalkCommon(str, "Param2")
			local Describe = Lan_NpcTalk_Common(MemH64(TalkName..Subject), "Describe")
			
			local _,j = string.find(Subject, "对话")
			if not tbl[TalkName] then
				tbl[TalkName] = {}
			end
			if j then
				local num = tonumber( string.sub(Subject, j+1, -1) )
				tbl[TalkName][num] = {}
				tbl[TalkName][num].Index = TalkName..Subject
				tbl[TalkName][num].Param1 = Param1
				tbl[TalkName][num].Param2 = Param2
			else
				tbl[TalkName][Subject] = {}
				tbl[TalkName][Subject].Index = TalkName..Subject
				if Subject == "触发Npc" then
					tbl[TalkName][Subject].Param1 = assert(loadstring("return " .."{"..Param1.."}"))()
					tbl[TalkName][Subject].Param2 = Param2
				elseif Subject=="触发事件" then
					tbl[TalkName][Subject].Param1 = tonumber(Param1)
				end
			end
		end
	end
	return tbl
end

g_NpcTalkTbl = GetNpcTalkCfg()
AddCheckLeakFilterObj(g_NpcTalkTbl)
