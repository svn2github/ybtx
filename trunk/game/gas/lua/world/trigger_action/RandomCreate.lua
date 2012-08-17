cfg_load "npc/RandomCreate_Server"

--获取一个正方环形区域内随机位置
--返回值 pos 满足
--  min <= math.abs(pos.x - centerPos.x) <= max  或者 min <= math.abs(pos.y - centerPos.y) <= max
local function GetRandomPosFromSquareLoopArea(centerPos, min, max)
	if min < 0 then
		min = 0
	end
	assert(min <= max, "获取随机位置, 参数 min 必须小于等于 max")
	local posIndex = 0;
	local outD = 2 * max + 1
	if min == 0 then
		posIndex = math.random(1, outD * outD)
	else
		local inD = 2 * min - 1
		local outArea = outD * outD
		local inArea = inD * inD
		local loopD = max - min + 1
		local randomRange = outArea - inArea
		posIndex = math.random(1, randomRange)
		if posIndex > randomRange - outD * loopD - loopD then
			posIndex = posIndex + inArea
		elseif posIndex > outD * loopD + loopD then
			local n = posIndex - outD * loopD - loopD
			posIndex = posIndex + math.ceil(n/(2*loopD)) * inD
		end
	end
	local pos = CPos:new()
	pos.x = centerPos.x - max + math.floor((posIndex-1) / outD)
	pos.y = centerPos.y - max + math.floor((posIndex-1) % outD)
	
	return pos
end
--获取一个圆环形区域内随机位置
local function GetRandomPosFromRoundLoopArea(centerPos, minR, maxR)
	if minR < 0 then
		minR = 0
	end
	assert(minR <= maxR, "获取随机位置, 参数 minR 必须小于等于 maxR")
	local radius = minR + math.random()*(maxR - minR)
	local angle = 2 * math.pi * math.random()
	local pos = CPos:new()
	--print("随机角度",angle)
	pos.x = centerPos.x + radius * math.sin(angle)
	pos.y = centerPos.y + radius * math.cos(angle)
	return pos
end

-- 创建连连看矩形NPC
local function CreateFindSameCubeNpc(objList, CreatorEntityID, Pos, Row, Line, Interval, TrapName)  -- objList、场景、左上角位置、行、列、间距
	local Creator = CEntityServerManager_GetEntityByID(CreatorEntityID)
	if not IsServerObjValid(Creator) then
		--print "g_RandomCreateTick(CreatorEntityID)  一个不存在的Creator!!!!"
		return
	end
	local Scene = Creator.m_Scene
	
	if #(Creator.NpcList)==0 then
		return
	end
	for j=1,#(objList) do
		for i=1, 2 do
			-- 随机位置
			local num = math.random(1, #(Creator.NpcList))
			local row = math.floor(Creator.NpcList[num] / Line)
			local line = math.floor(Creator.NpcList[num] % Line)
			--print("创建 num: "..Creator.NpcList[num].."  ".."row: "..row.."line: "..line)
			-- 创建Npc
			local fPos = CFPos:new( (Pos.x + Interval*row + 0.5) * EUnits.eGridSpan, (Pos.y + Interval*line + 0.5) * EUnits.eGridSpan )
			local TrapPos = CFPos:new((Pos.x+Interval*row), (Pos.y+Interval*line))
			local NpcName = objList[j][2]
			local level = g_NpcBornMgr:GetNpcBornLevel(NpcName)
			local otherData = {["m_CreatorEntityID"]=CreatorEntityID}
			local child = g_NpcServerMgr:CreateServerNpc(NpcName, level, Scene, fPos, otherData)
			if not IsServerObjValid(child) then
				return
			end
			table.remove(Creator.NpcList, num)
			
			-- 重生地点
			if not child.m_RebornPos then
				child.m_RebornPos = {}	
			end
			child.m_RebornPos.x = Pos.x + Interval*row + 0.5
			child.m_RebornPos.y = Pos.y + Interval*line + 0.5
			-- 创建Trap
			local Trap = CreateServerTrap(Scene,TrapPos, TrapName)
		end
	end
end

-- 打地鼠
local function CreateMouseCubeNpc(objList, CreatorEntityID, Pos, Row, Line, Interval)
	local Creator = CEntityServerManager_GetEntityByID(CreatorEntityID)
	if not IsServerObjValid(Creator) then
		--print "g_RandomCreateTick(CreatorEntityID)  一个不存在的Creator!!!!"
		return
	end
	local Scene = Creator.m_Scene
	
	if #(Creator.NpcList)==0 then
		return
	end
	local num = math.random(1, #(Creator.NpcList))
	
	local row = math.floor(Creator.NpcList[num] / Line)
	local line = math.floor(Creator.NpcList[num] % Line)
	--print("创建 num: "..Creator.NpcList[num].."  ".."row: "..row.."line: "..line)
	local fPos = CFPos:new( (Pos.x + Interval*row + 0.5) * EUnits.eGridSpan, (Pos.y + Interval*line + 0.5) * EUnits.eGridSpan )
	local objNum = table.getn(objList)
	local Index = math.random(1, objNum)
	local NpcName = objList[Index][2]
	local level = g_NpcBornMgr:GetNpcBornLevel(NpcName)
	local otherData = {["m_CreatorEntityID"]=CreatorEntityID}
	local child = g_NpcServerMgr:CreateServerNpc(NpcName, level, Scene, fPos, otherData)
	if not IsServerObjValid(child) then
		return
	end
	child.RandomCubeNpcNum = Creator.NpcList[num]
	
	table.remove(Creator.NpcList, num)
end

local function CreateCubeNpc(objList, CreatorEntityID, Pos, Row, Line, Interval)
	local Creator = CEntityServerManager_GetEntityByID(CreatorEntityID)
	if not IsServerObjValid(Creator) then
		--print "g_RandomCreateTick(CreatorEntityID)  一个不存在的Creator!!!!"
		return
	end
	local Scene = Creator.m_Scene
	
	for j=1,#(objList) do
		-- 指定位置
		local row = math.floor((j - 1) / Line)
		local line = math.floor((j - 1) % Line)
		-- 创建Npc
		local fPos = CFPos:new( (Pos.x + Interval*row + 0.5) * EUnits.eGridSpan, (Pos.y + Interval*line + 0.5) * EUnits.eGridSpan )
		local TrapPos = CFPos:new((Pos.x+Interval*row), (Pos.y+Interval*line))
		local NpcName = objList[j][2]
		local level = g_NpcBornMgr:GetNpcBornLevel(NpcName)
		local otherData = {["m_CreatorEntityID"]=CreatorEntityID}
		local child = g_NpcServerMgr:CreateServerNpc(NpcName, level, Scene, fPos, otherData)
		if not IsServerObjValid(child) then
			return
		end
		-- 重生地点
		if not child.m_RebornPos then
			child.m_RebornPos = {}	
		end
		child.m_RebornPos.x = Pos.x + Interval*row + 0.5
		child.m_RebornPos.y = Pos.y + Interval*line + 0.5
	end
end

function g_RandomCreateTick(CreatorEntityID)
	local Creator = CEntityServerManager_GetEntityByID(CreatorEntityID)
	if not IsServerObjValid(Creator) then
--		print "g_RandomCreateTick(CreatorEntityID)  一个不存在的Creator!!!!"
		return
	end
	
	local name = Creator.m_Properties:GetCharName()
	local config = RandomCreate_Server(name)
--	local str = "return {".. RandomCreate_Server(name, "ObjectList") .. "}"
--	local objList = assert(loadstring(str), "RandomCreate_Server 表中 " .. name .. " 的ObjectList格式不对")()
	local objList = GetCfgTransformValue(true, "RandomCreate_Server", name, "ObjectList")
	local objNum = table.getn(objList)
	if objNum == 0 then
		return
	end
	
	local BornRate = config("BornRate")

	local IsCreateNpc = nil
	BornRate = tonumber(BornRate)
	if BornRate then
		local RandomNum  = math.random(1, 100)	
--	print("RandomNum : "..RandomNum)
		if RandomNum > BornRate then									--	一定几率刷怪
--			print("不刷怪"..RandomNum)
			IsCreateNpc = false
		else
			IsCreateNpc = true
--			print("刷怪"..RandomNum)
		end
	else
		IsCreateNpc = true
--		print("刷怪")
	end

	
	if IsCreateNpc then
		local pos = CPos:new()
		Creator:GetGridPos(pos)
		local minRadius = config("MinRadius")
		local maxRadius = config("MaxRadius")
		assert(config("MinNum") > 0 and config("MinNum") <= config("MaxNum"), "随机产生最小个数和最大个数不对")
		
		local num = math.random(config("MinNum"),config("MaxNum"))
		
		if config("InCreaseNum") and config("InCreaseNum") ~="" and config("InCreaseNum") ~= "0" then
			if not Creator.TickNum  then
				Creator.TickNum = 0 
			else
				Creator.TickNum = Creator.TickNum + 1
			end
			num = num + config("InCreaseNum") * Creator.TickNum 	
		end
		local getRandomPos = GetRandomPosFromRoundLoopArea
		if config("AreaType") == 1 then
			getRandomPos = GetRandomPosFromSquareLoopArea
		end
		local index
--		print("num =---  "..num)
		for i = 1, num do
			local posTemp = getRandomPos(pos, minRadius, maxRadius)
			index = math.random(1, objNum)
			local child = nil
			if objList[index][1] == "Obj" then
				local DropOwnerId = 0
				local DropTime = 0
				if Creator.m_OwnerId and g_GetPlayerInfo(Creator.m_OwnerId) then
					DropOwnerId = Creator.m_OwnerId
					DropTime = os.time()
				end
				child = CreateIntObj(Creator.m_Scene, posTemp, objList[index][2], true, DropOwnerId, nil,DropTime)
			elseif objList[index][1] == "Npc" then
				local CubeFormat = GetCfgTransformValue(true, "RandomCreate_Server", name, "CubeFormat")
				if #(CubeFormat)==0 then																											
					local level = g_NpcBornMgr:GetNpcBornLevel(objList[index][2])
					local fPos = CFPos:new( posTemp.x * EUnits.eGridSpan, posTemp.y * EUnits.eGridSpan )
					local otherData = {["m_CreatorEntityID"]=CreatorEntityID}
					child = g_NpcServerMgr:CreateServerNpc(objList[index][2], level, Creator.m_Scene, fPos, otherData)
				elseif CubeFormat[1] == "连连看" then		
					if not Creator.NpcList then
						Creator.NpcList = {}
						for i=1, CubeFormat[2]*CubeFormat[3] do
							Creator.NpcList[i] = i-1
						end
					end																																							-- 矩形怪队列
					CreateFindSameCubeNpc(objList, CreatorEntityID, pos, CubeFormat[2], CubeFormat[3], CubeFormat[4], CubeFormat[5])
				elseif CubeFormat[1] == "打地鼠" then
					if not Creator.NpcList then
						Creator.NpcList = {}
						for i=1, CubeFormat[2]*CubeFormat[3] do
							Creator.NpcList[i] = i-1
						end
					end
					CreateMouseCubeNpc(objList, CreatorEntityID, pos, CubeFormat[2], CubeFormat[3], CubeFormat[4])
				elseif CubeFormat[1] == "NPC矩形队列" then
					CreateCubeNpc(objList, CreatorEntityID, pos, CubeFormat[2], CubeFormat[3], CubeFormat[4])
				end	
			
			elseif objList[index][1] == "Trap" then
				local gridfPos = CFPos:new()
				gridfPos.x = posTemp.x + 0.5
				gridfPos.y = posTemp.y + 0.5
				child = CreateServerTrap(Creator.m_Scene,gridfPos,objList[index][2])
			end
			if child ~= nil then
				 child.m_CreatorEntityID = CreatorEntityID
			end
		end
	end
	
	-- 刷怪器只刷一次怪
	if config("IntervalTime") == "" or config("IntervalTime") == 0 then
		if Creator.m_RandomCreateTick ~= nil then
			UnRegisterTick(Creator.m_RandomCreateTick)
			Creator.m_RandomCreateTick = nil
		end
	else
		local function TickCallBack()
			g_RandomCreateTick(CreatorEntityID)
		end
		if Creator.m_isNewRandomCreateTick then
			Creator.m_isNewRandomCreateTick = nil
			UnRegisterTick(Creator.m_RandomCreateTick)
			local time = config("IntervalTime") * 1000
			Creator.m_NowIntervalTime = config("IntervalTime")
			Creator.m_RandomCreateTick = RegisterTick("RandomCreateTick", TickCallBack, time)
			--print ("重新注册RandomCreateTick    ", time)
		else
			local Degression = config("Degression")
			local MinTime = config("MinTime")
			
			MinTime = tonumber(MinTime)
			if not MinTime then
				MinTime = 0
			end

			if Degression and Degression ~= "" and Degression~= 0 and Creator.m_NowIntervalTime + Degression > MinTime then
				UnRegisterTick(Creator.m_RandomCreateTick)
				Creator.m_NowIntervalTime = Creator.m_NowIntervalTime + Degression
				local time = Creator.m_NowIntervalTime * 1000
				Creator.m_RandomCreateTick = RegisterTick("RandomCreateTick", TickCallBack, time)
				--print ("重新注册RandomCreateTick    ", time)
			end
		end
	end
end
