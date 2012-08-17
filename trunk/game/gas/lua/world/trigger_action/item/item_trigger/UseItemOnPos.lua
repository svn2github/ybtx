local CallAccountManualTrans = CallAccountManualTrans

function UseItemEffectOnPos(Conn, ItemInfo, pos) 
	local effectName = ItemInfo.effect
	if effectName == "对地点释放技能" then
		DoSkillOnPos(Conn,ItemInfo, pos)
	elseif effectName == "随机刷新" then
		CreatOnRandomPos(Conn,ItemInfo,pos)
	elseif effectName == "放置" then
		CreatOnPos(Conn,ItemInfo,pos)
	elseif effectName == "放置建筑" then
		LayBuild(Conn, ItemInfo, pos)		
	elseif effectName == "放置临时建筑" then
		LayTempBuild(Conn, ItemInfo, pos)
	end
end

--对地点释放技能
function DoSkillOnPos(Conn,ItemInfo,pos)
	local function SeccessCreat(Player,ItemInfo,pos)
		local ArgTbl = ItemInfo.Arg[1]
		local SkillName = ArgTbl[1]
		local SkillLevel = ArgTbl[2] or 1
		local doSkillPos = CPos:new()
		doSkillPos.x = pos.x
		doSkillPos.y = pos.y
		if not LockItem(Conn,ItemInfo) then			--给物品加锁
			UseItemEnd(Conn,ItemInfo,802)
			return
		end
		local PixelPos = CFPos:new()
		PixelPos.x = doSkillPos.x*EUnits.eGridSpanForObj
		PixelPos.y = doSkillPos.y*EUnits.eGridSpanForObj
		local res = Player:PlayerDoPosSkill(SkillName, 1, PixelPos)
		if res == EDoSkillResult.eDSR_Success then
			--print("选择地点使用道具技能"..SkillName.."成功")
			UseItemOtherList(Conn,ItemInfo)
			--print("使用"..ItemInfo.ItemName.."成功!")
		else
			UseItemEnd(Conn,ItemInfo)
		end
		--print("The Result of Player:PlayerDoPosSkill(SkillName,1) is "..res)
	end
	local function FailCreat(Player,ItemInfo)
		UseItemEnd(Conn,ItemInfo)
		--print("选择地点释放技能失败")
	end
	BeginProgress(Conn,ItemInfo,SeccessCreat,FailCreat,pos)
end

--放置
function CreatOnPos(Conn,ItemInfo,pos)
	local function SeccessCreat(Player,ItemInfo,CreatPos)
		local ObjectTbl = ItemInfo.Arg[1]
		local Type = ObjectTbl[1]
		local name = ObjectTbl[2]
		local FbName = ObjectTbl[3]--放一个传送用的OBJ,进领域用
		
		local scene = Player.m_Scene
		if not LockItem(Conn,ItemInfo) then			--给物品加锁
			UseItemEnd(Conn,ItemInfo,802)
			return
		end
	  if Type == "Object" then
			local obj = UseItemCreateIntObj(Conn,name,CreatPos)
			if obj == nil then
				UseItemEnd(Conn,ItemInfo)
				return
			end
			if FbName then
				obj.m_Properties:SetDropOwnerId(0)
				local ScopesExploration = CScopesExploration:new()
				ScopesExploration:CreateSpecilAreaFb(Conn, obj, FbName)
			end
		elseif Type == "Trap" then
			local trap = UseItemCreateTrap(Conn, name, CreatPos)
			if trap == nil then
				UseItemEnd(Conn,ItemInfo)
				return
			end
		elseif Type == "Npc" then
			local npc = UseItemCreateNpc(Conn, ItemInfo.ItemName, name, CreatPos)
			if npc == nil then
				UseItemEnd(Conn,ItemInfo)
				return
			end 
		end
		UseItemOtherList(Conn,ItemInfo)
	end
	
	local function FailCreat(Player,ItemInfo)
		UseItemEnd(Conn,ItemInfo)
		--print("在目标点放置物品失败")
	end
	BeginProgress(Conn,ItemInfo,SeccessCreat,FailCreat,pos)
end

--在主角为中心的指定范围内随机刷新多个Npc或Obj
function CreatOnRandomPos(Conn,ItemInfo,pos)

	local function SuccessCreat(Player,ItemInfo,pos)
		local ArgTbl = ItemInfo.Arg
		local PosOnUse = {}
		if not LockItem(Conn,ItemInfo) then			--给物品加锁
			UseItemEnd(Conn,ItemInfo,802)
			return
		end
		for i,v in pairs(ArgTbl) do
			local CreatTbl = v
			local CreatType = CreatTbl[1]
			local CreatName = CreatTbl[2]
			local CreatNum  = CreatTbl[3]
			local range = CreatTbl[4]
			if range == nil or range == "" then
				range = 1
			end
			local creatPos
	 		if CreatType == "Npc" then
	 			--print("随机刷新名字为"..CreatName.."的Npc"..CreatNum.."个")
	 			for i = 1, CreatNum,1 do
	 				creatPos = CreatRandomPos(pos,PosOnUse,range)
					PosOnUse[i] = creatPos
					local npc = UseItemCreateNpc(Conn, ItemInfo.ItemName, CreatName, creatPos)
					if npc == nil then
						UseItemEnd(Conn,ItemInfo)
						return
					end 
				end
			elseif CreatType == "Object" then
				--print("随机刷新名字为"..CreatName.."的Object"..CreatNum.."个")
				for i = 1, CreatNum,1 do
					creatPos = CreatRandomPos(pos,PosOnUse,range)
					PosOnUse[i] = creatPos
					local Obj = UseItemCreateIntObj(Conn,CreatName,creatPos)
					if Obj == nil then
						UseItemEnd(Conn,ItemInfo)
						return
					end
				end
			end
		end	
		UseItemOtherList(Conn,ItemInfo)
	end
	
	local function FailedCreat(Player,ItemInfo)
		UseItemEnd(Conn,ItemInfo)
		--print("在随机范围内创建npc失败")
	end
	BeginProgress(Conn,ItemInfo,SuccessCreat,FailedCreat,pos)
end

--创建随机坐标 以玩家坐标为中心range的范围内
function CreatRandomPos(pos,posInUseTbl,range)
	local bContinue = true
	local x,y
	while bContinue do
		x = math.random(-range, range)
	 	y = math.random(-range, range)
	 	if x ~= 0 or y ~= 0 then
			bContinue = false
			for i,v in pairs(posInUseTbl) do
				if x == (v.x - pos.x) and y == (v.y - pos.y) then
					bContinue = true
				end
			end
		end
	end
	local CreatPos = CPos:new()
	CreatPos.x = pos.x + x
	CreatPos.y = pos.y + y
	return CreatPos
end

function LayTempBuild(Conn, ItemInfo, pos)
	local Player = Conn.m_Player
	local scene = Player.m_Scene
	local TongId = Conn.m_Player.m_Properties:GetTongID()
	local uPlayerID = Conn.m_Player.m_uID
	local ret = g_BuildingMgr:IsCanLayTempBuildingOnPos(Player, pos)
	if not ret[1] then
		UseItemEnd(Conn,ItemInfo)
		MsgToConn(Conn, ret[2])
		return
	end

	local function CallBack(num)
		if scene.m_TongBuildingTbl[TongId] and #scene.m_TongBuildingTbl[TongId] >= (num - 3) then
			MsgToConn(Conn, 9418)
			UseItemEnd(Conn,ItemInfo)
			return
		end
		
		local function SuccessUse(Player, ItemInfo)
			if IsCppBound(Conn) and IsCppBound(Player) then
				local itemData =  Conn.m_Player.UseItemParam[ItemInfo.ItemName]
				if not itemData then
					return
				end
				g_BuildingMgr:CreateTongTempBuilding(Conn, ItemInfo.ItemName, itemData.RoomIndex, itemData.RoomPos, ret[2])
				LayBuildUseItemOtherList(Conn, ItemInfo)
			end
		end
		
		local function FailedUse(Player, ItemInfo)
			if IsCppBound(Conn) then
				UseItemEnd(Conn,ItemInfo)
			end
		end
		BeginProgress(Conn,ItemInfo,SuccessUse,FailedUse,pos)
	end
	
	local data = {}
	data["TongId"] = TongId
	
	CallAccountManualTrans(Conn.m_Account, "GasTongBasicDB", "GetTongBuildingMaxNum", CallBack, data, TongId)
end

--调用创建建筑
function LayBuild(Conn, ItemInfo, pos)
	local Player = Conn.m_Player
	local uPlayerID = Conn.m_Player.m_uID
	local ret = g_BuildingMgr:IsCanLayBuildingOnPos(Player, pos, ItemInfo.ItemName)
	if not ret[1] then
		if IsNumber(ret[2]) then
			MsgToConn(Conn, ret[2])
		end
		UseItemEnd(Conn,ItemInfo)
		return
	end

	local function SuccessUse(Player, ItemInfo)
		if IsCppBound(Conn) and IsCppBound(Player) then
			local itemData =  Conn.m_Player.UseItemParam[ItemInfo.ItemName]
			if not itemData then
				return
			end
			g_BuildingMgr:CreateTongBuildingCallDb(Conn, ItemInfo.ItemName, itemData.RoomIndex, itemData.RoomPos, ret[2])
			LayBuildUseItemOtherList(Conn, ItemInfo)
		end
	end
	
	local function FailedUse(Player, ItemInfo)
		if IsCppBound(Conn) then
			UseItemEnd(Conn,ItemInfo)
			--print("创建建筑失败")
		end
	end
	BeginProgress(Conn,ItemInfo,SuccessUse,FailedUse,pos)

end