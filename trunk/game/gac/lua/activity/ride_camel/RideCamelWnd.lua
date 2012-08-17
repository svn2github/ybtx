gac_require "activity/ride_camel/RideCamelWndInc"

function CreateRideCamelWnd(parent)
	local Wnd = CRideCamelWnd:new()
	Wnd:CreateFromRes("WndPhysical", parent)
	Wnd.m_ProgressTime = Wnd:GetDlgChild("ProPhysical")
	Wnd.m_ProgressTime:SetProgressMode(3)
	Wnd.m_ProgressTime:SetRange(1000)
	Wnd.m_Pos = 1000

	return Wnd
end

function CRideCamelWnd:SendToGas(StateName,RemainTime)
	if StateName=="骑骆驼" then
		Gac2Gas:CheckCamelState(g_Conn, StateName)
		return
	elseif StateName=="骑疯马" then
		Gac2Gas:CheckCrazyHorseState(g_Conn, StateName)
		return
	end
	
	local mCrazyHorseStateTbl = {"疯马加速状态1","疯马加速状态2","疯马加速状态3","疯马加速状态4","疯马加速状态5","疯马加速状态6","疯马加速状态7","疯马加速状态8","疯马加速状态9","疯马加速状态10"}
	for n = 1, #(mCrazyHorseStateTbl) do
		if StateName == mCrazyHorseStateTbl[n] then
			Gac2Gas:CheckCrazyHorseState(g_Conn, StateName)
			return
		end
	end
	
	for questname, p in pairs(g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo) do
		if Quest_Common(questname, "交任务状态需求") then
			local Keys = Quest_Common:GetKeys(questname, "交任务状态需求")
			for k = 1, table.getn(Keys) do
				local tbl = GetCfgTransformValue(true, "Quest_Common", questname, "交任务状态需求", Keys[k], "Function")
				if StateName == tbl[1] then
					local function QuestInfoChange()
						if g_GameMain.m_QuestRecordWnd.m_SelectQuestName == questname then
							g_GameMain.m_QuestRecordWnd:InitQuestInfo(questname)
						end
						g_GameMain.m_QuestTraceBack:ShowTrackByQuestName(questname)
						g_GameMain.m_NpcHeadSignMgr:UpdateNpcAndObjInView()
					end
					local DoNum = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[questname][StateName].DoNum
					if RemainTime == 0 and DoNum == 1 then	
						g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[questname][StateName].DoNum = 0
						QuestInfoChange()
					elseif RemainTime ~= 0 and DoNum == 0 then	
						g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[questname][StateName].DoNum = 1
						QuestInfoChange()
					end
					break
				end
			end
		end
		for questvar, v in pairs(p) do
			if questvar == StateName then
				if v.DoNum < v.NeedNum then
					Gac2Gas:SendSuccStateName(g_Conn, questname, StateName)
				end
			end
		end
	end
end

function Gas2Gac:RetCheckCamelState(Conn, StateName, flag)
	if not g_GameMain.m_CRideCamelWnd:IsShow() then
		g_GameMain.m_CRideCamelWnd:ShowWnd(true)
	end
end

function Gas2Gac:RetPhysicalStrengh(Conn, num)
	g_GameMain.m_CRideCamelWnd.m_Pos = num
	g_GameMain.m_CRideCamelWnd.m_ProgressTime:SetPos(num)
end

function Gas2Gac:RetPhysicalState( Conn, falg)
	if g_GameMain.m_CRideCamelWnd:IsShow() then
		g_GameMain.m_CRideCamelWnd:ShowWnd(flag)
	end
end