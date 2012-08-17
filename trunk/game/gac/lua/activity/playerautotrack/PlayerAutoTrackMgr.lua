gac_gas_require "framework/barrier/Barrier"
--CSpanSceneTrackMgr = class( CGraphInt )
g_SpanSceneAutoTrackMgr = {}

--EFindPathType.eFPT_HypoAStarOptiUseWeight
--EFindPathType.eFPT_AStarUseWeight
local function CheckIsNeedAutoTrack(x,y)
	local TargetPos = CPos:new()
	TargetPos.x,TargetPos.y = x,y
	if g_GetDistanceByPos(g_MainPlayer, TargetPos) < 1 then
		return false
	else
		return true
	end
end

local function GetCurTransportAreaName()
	local curPos = CPos:new()
	g_MainPlayer:GetGridPos(curPos)
	return GetSceneTransportAreaName(g_GameMain.m_SceneName, curPos)
end


function ClearAutoTrackState()
	g_SpanSceneAutoTrackMgr = {}
	if g_GameMain.m_AreaInfoWnd then
		g_GameMain.m_AreaInfoWnd:ClearTrackPosWnd()
	end
end

function AutoTrackToFuncNpc(sName)
	if not sName then
		return false
	end
	local ViewObjTbl = g_GameMain.m_NpcHeadSignMgr.m_SeeNpcAndObjTbl
	if Npc_Common(sName) then
		local PlayerCamp = g_MainPlayer:CppGetBirthCamp()
		local PlayerGameCamp = g_MainPlayer:CppGetGameCamp()
		for i,uEntityID in pairs(ViewObjTbl) do
			local NpcObj = CCharacterFollower_GetCharacterByID(uEntityID)
			if NpcObj then
				local CharName = NpcObj.m_Properties:GetCharName()
				if CharName == sName
					and IsPasserbyCamp(NpcObj:CppGetCamp(), PlayerCamp, NpcObj:CppGetGameCamp(), PlayerGameCamp)
					and CQuest.IsObjectContainFun(NpcObj) then
					g_MainPlayer:LockObj( NpcObj )
					g_MainPlayer:OnLButtonClickObj(NpcObj)
					return true
				end
			end
		end
	elseif IntObj_Common(sName) then
		for i,uEntityID in pairs(ViewObjTbl) do
			local IntObjClient = CIntObjClient_GetIntObjByID(uEntityID)
			if IntObjClient then
				local CharName = IntObjClient.m_Properties:GetCharName()
				if CharName == sName and (sName == "邮箱" or CQuest.IsObjectContainFun(IntObjClient)) then
					g_MainPlayer:LockIntObj( IntObjClient )
					g_MainPlayer:OnLButtonClickObj(IntObjClient)
					return true
				end
			end
		end
	end
	return false
end

function GridTransferToPixelPos(ToX,ToY)
	local ResPos = CFPos:new()
	ResPos.x = ToX*EUnits.eGridSpanForObj
	ResPos.y = ToY*EUnits.eGridSpanForObj
	return ResPos
end

function PrepareAutoTrack()
	if( g_MainPlayer.m_LockCenter.m_LockingObj ~= nil ) then
		g_MainPlayer:UnLockObj()
	end
	if( g_MainPlayer.m_LockCenter.m_LockingIntObj ~= nil ) then
		g_MainPlayer:UnLockIntObj()
	end
	g_MainPlayer:LockObj_UnRegisterTick()
	g_MainPlayer:CancelAutoTracker(false)
end


function AutoTrackToGridByAStar(SceneName, x, y)
	PrepareAutoTrack()
	local ToPos = GridTransferToPixelPos(x, y)
	g_MainPlayer:MoveToInAStarPath( ToPos, EFindPathType.eFPT_AStarUseWeight, EBarrierType.eBT_LowBarrier, 0 ,100)
	local Path = g_MainPlayer:GetClonedMovePath()
	if not Path then
		return false
	end
	local InflexionNum = Path:GetPathSize()
	local ResPos = CFPos:new()
	Path:GetWayPointPixelPosOfPath(ResPos,InflexionNum)
	if math.floor(ResPos.x/64) ~= math.floor(x) or math.floor(ResPos.y/64) ~= math.floor(y) then
		Path:Release()
		return false
	end
	g_GameMain.m_AreaInfoWnd:CreateAutoTrackByPath(SceneName,Path)
	Path:Release()
	return true
end

function AutoTrackToPosByHypoAStarOpti(SceneName, x, y)
	local FinalPos = GridTransferToPixelPos(x,y)
	PrepareAutoTrack()
	g_MainPlayer:MoveToInAStarPath( FinalPos, EFindPathType.eFPT_HypoAStarOptiUseWeight, EBarrierType.eBT_LowBarrier, 0 ,100)
	local Path = g_MainPlayer:GetClonedMovePath()
	if Path then
		g_GameMain.m_AreaInfoWnd:CreateAutoTrackByPath(SceneName,Path)
		Path:Release()
	end
	return true
end

function MoveEndCheckAutoTrackState()
	local curTranAreaName = GetCurTransportAreaName()
	if g_SpanSceneAutoTrackMgr.m_EndTransAreaName ~= curTranAreaName then
		return
	end
	MsgClient(3024)
	ClearAutoTrackState()
end

function PlayerAutoTrackNearestPoint(PointList)
	local minStep = 10000
	local curPos = CPos:new()
	g_MainPlayer:GetGridPos(curPos)
	local beginAreaName = GetSceneTransportAreaName(g_GameMain.m_SceneName, curPos)
	local allMinStepList
	for i, v in pairs(PointList) do
		local endPos = CPos:new(v[3],v[4])
		local endAreaName = GetSceneTransportAreaName(v[2], endPos)
		local stepList = g_CTransportMap:FindTrackStepList(beginAreaName, endAreaName, curPos, endPos)
		
		if stepList then
			local stepCount = #stepList
			if stepCount < minStep then
				minStep = stepCount
				allMinStepList = {}
				table.insert(allMinStepList, {stepList, i})
			elseif stepCount == minStep then
				table.insert(allMinStepList, {stepList, i})
			end
		end
		
	end
	
	if not allMinStepList then --都不可到达
		return false
	end
	
	local minDis = nil
	local nearestIndex = nil
	for _, v in pairs(allMinStepList) do
		local lastStep = v[1][minStep]
		local dx = lastStep[2].x - lastStep[3].x
		local dy = lastStep[2].y - lastStep[3].y
		local disSquare = dx * dx + dy * dy
		if minDis ==nil or disSquare < minDis then
			minDis = disSquare
			nearestIndex = v[2]
		end
	end
	return PlayerAutoTrack(unpack(PointList[nearestIndex], 1,4))
end

function PlayerAutoTrack(PointName,SceneName,x,y)
	if not IsCppBound(g_MainPlayer) then
		return false
	end
	--如果玩家已经处于自动寻路状态，而且目标点一致则直接返回，减少计算
	if g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint
		and g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint[1] ==  SceneName
		and g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint[2] == x
		and g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint[3] == y 
		and g_MainPlayer:IsMoving() then
		return true
	end
	
	ClearAutoTrackState()
	local curPos = CPos:new()
	g_MainPlayer:GetGridPos(curPos)
	local beginAreaName = GetSceneTransportAreaName(g_GameMain.m_SceneName, curPos)
	local endPos = CPos:new(x,y)
	local endAreaName = GetSceneTransportAreaName(SceneName, endPos)
	local trankStepList = g_CTransportMap:FindTrackStepList(beginAreaName, endAreaName, curPos, endPos)
	if not trankStepList then --不可到达
		return false
	end
	
	local firsStep = trankStepList[1]
	local isSucceed = false
	if firsStep[1] == endAreaName then --在最后区域内, 就近寻路
		if CheckIsNeedAutoTrack(x,y) then
			isSucceed = AutoTrackToPosByHypoAStarOpti(g_GameMain.m_SceneName, firsStep[2].x, firsStep[2].y)
		else
			AutoTrackToFuncNpc(PointName)
		end
	else
		isSucceed = AutoTrackToGridByAStar(g_GameMain.m_SceneName, firsStep[2].x, firsStep[2].y)
	end
	if isSucceed then
		g_SpanSceneAutoTrackMgr.m_AutoTrackToPointName = PointName
		g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint = {SceneName,x,y}
		g_SpanSceneAutoTrackMgr.m_EndTransAreaName = endAreaName
		g_SpanSceneAutoTrackMgr.m_StepList = trankStepList
	end
end

function IsSpanSceneAutoTrackState()
	if not IsCppBound(g_MainPlayer) then
		return
	end
	if g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint then
		
		local sceneName = g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint[1]
		local x = g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint[2]
		local y = g_SpanSceneAutoTrackMgr.m_AutoTrackToGridPoint[3]
		local PointName = g_SpanSceneAutoTrackMgr.m_AutoTrackToPointName
		
		local curAreaName = GetCurTransportAreaName()
		
		local stepList = g_SpanSceneAutoTrackMgr.m_StepList
		--检测是否按预定的步骤走, 不是则取消寻路,否则很可能循环切场景了
		if  curAreaName ~= stepList[1][1] then --走过第一步,检测是不是第二步
			if not stepList[2] or curAreaName ~= stepList[2][1] then
				ClearAutoTrackState()
				return
			end
		end
		ClearAutoTrackState()
		PlayerAutoTrack(PointName, sceneName, x, y)
		return
	elseif g_GameMain.m_LastMoveToPos then
		if g_GameMain.m_LastMoveToPos.SceneName == g_GameMain.m_SceneName then
			g_MainPlayer:MoveToSelectPos(g_GameMain.m_LastMoveToPos.Pos, nil)
		else
			g_GameMain.m_LastMoveToPos = nil
		end
	end

end



function Gas2Gac:PlayerTransportSetGrid(Conn)
		IsSpanSceneAutoTrackState()
end
