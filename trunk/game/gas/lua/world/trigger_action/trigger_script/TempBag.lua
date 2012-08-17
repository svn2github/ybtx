local GridNum = 9
CTempBag = class()

function CTempBag:TempBagAddObj(Conn, IntObj, ObjName)

	local IsSucc = self:ComposeTempBagObj(Conn, ObjName)
	if not IsSucc then
		IsSucc = self:AddTempBagObj(Conn, ObjName)
	end
	if IsSucc then
		if IntObj.m_Properties:GetDropOwnerId() ~= 0 then
			g_IntObjServerMgr:Destroy(IntObj, false)
		else
			g_IntObjServerMgr:Destroy(IntObj, true)
		end
		return
	end
end

function CTempBag:AddTempBagObj(Conn, ObjName)
	
	local Player = Conn.m_Player
	local IsHave = false
	local IsSucc = false
	local Index = nil
	for i = 1, GridNum do
		if self[i] and self[i].m_Name and self[i].m_Name == ObjName then
			IsHave = true
		end
		if self[i] == nil and Index == nil then
			Index = i
		end
		
		if i == GridNum and Index then
			--print("增加了",ObjName)
			self[Index] = {["m_Name"] = ObjName}
			IsSucc = true
			if g_ObjActionArgTbl[ObjName] then
				local Action = g_ObjActionArgTbl[ObjName][1]
				local BuffSkillName = g_ObjActionArgTbl[ObjName][2]
				--print(Action,BuffSkillName)
				if Action == "持有" then
					if not IsHave then
						Player:PlayerDoItemSkill(BuffSkillName, 1)
					end
					self[Index].m_ClearSkillName = g_ObjActionArgTbl[ObjName][3]
				elseif Action == "使用" then
					self[Index].m_SkillName = BuffSkillName
				end
			end
			Gas2Gac:TempBagSetGrid(Conn, Index, ObjName)
		end
	end
	if not IsSucc then
		MsgToConn(Conn, 160000)
	end
	return IsSucc
end

--合成OBJ
--@param name 合成的OBJ名称
function CTempBag:ComposeObj(Conn, name, ObjName)
	--print("合成了",name)
	local DelCollObj = false
	local tbl = GetCfgTransformValue(false,"TempBagObj_Common", name, "Part")
	for i, v in ipairs(tbl) do
		if ObjName and v == ObjName then
			DelCollObj = true
		else
			self:DelTempBagObjByName(Conn, v)
		end
	end
	local res = self:AddTempBagObj(Conn, name)
	if res then
		self:ComposeTempBagObj(Conn)
	end
	return DelCollObj
end

--检查是否有可以合成的OBJ
--@param ObjName			如果传入ObjName,说明是拾取OBJ触发进行合成
--@return DelCollObj	ObjName有值的情况,返回值为true,说明用ObjName合成成功
function CTempBag:ComposeTempBagObj(Conn, ObjName)
	local ComposeTbl = {}
	local DelCollObj = false
	
	--@param name 如果是可以合成的材料记录到ComposeTbl表中
	local function QueryCompose(name)
		local tbl = GetCfgTransformValue(false,"TempBagObj_Common", name, "Compose")
		if tbl == "" then
			return
		end
		for i, v in ipairs(tbl) do
			if not ComposeTbl[v] then
				ComposeTbl[v] = {}
				ComposeTbl[v]["Num"] = table.maxn(GetCfgTransformValue(false,"TempBagObj_Common", v, "Part"))
			end
			if ComposeTbl[v][name] == nil then
				ComposeTbl[v][name] = true
				ComposeTbl[v]["Num"] = ComposeTbl[v]["Num"] - 1
			end
			if ComposeTbl[v]["Num"] == 0 then
				ComposeTbl = {}
				DelCollObj = self:ComposeObj(Conn, v, ObjName)					--合成材料足够,进行合成
				return true
			end
		end
	end
	
	if ObjName then
		QueryCompose(ObjName)
	end
	for i = 1, GridNum do
		if self[i] ~= nil then
			if self[i].m_Name then
				local res = QueryCompose(self[i].m_Name)
				if res then
					return DelCollObj
				end
			end
		end
	end
	return DelCollObj
end

function CTempBag:DelTempBagObjByName(Conn, Name)
	for i = 1 , GridNum do
		if self[i] and self[i].m_Name == Name then
			self:DelTempBagObjByIndex(Conn, i)
			break
		end
	end
end

function CTempBag:DelTempBagObjByIndex(Conn, Index)
	local Player = Conn.m_Player
	if self[Index] then
		local ClearSkillName = self[Index].m_ClearSkillName
		if ClearSkillName and ClearSkillName ~= "" then
			for i = 1 , GridNum do
				if i ~= Index and self[i] and self[i].m_Name and self[i].m_Name == self[Index].m_Name then
					break
				end
				if i == GridNum then
					Player:PlayerDoItemSkill(ClearSkillName, 1)
					--Player:ClearState(ClearSkillName)
				end
			end
		end
		--print("删除了",self[Index].m_Name)
		self[Index] = nil
		Gas2Gac:TempBagClearGrid(Conn, Index)
	end
end

function CTempBag:ClearTempBagObj(Conn, Index)
	local Player = Conn.m_Player
	if self[Index] then
		local pos = GetCreatePos(Player)
		pos = RandomCreatPos(pos,1)
		local Obj = CreateIntObj(Player.m_Scene,pos,self[Index].m_Name)
		self:DelTempBagObjByIndex(Conn, Index)
	end
end

function CTempBag.UseTempBagObj(Conn, Index)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	local TempBag = Player.m_TempBag
	Player:CancelCastingProcess()
	if TempBag[Index] then
		local AreaLimit = GetCfgTransformValue(false,"TempBagObj_Common", TempBag[Index].m_Name, "AreaLimit")
		if AreaLimit and AreaLimit ~= "" then
			if AreaLimit[1] == "Trap" then
				local f = Player.m_Properties.m_InTrapTblName[AreaLimit[2]]
				if f == nil or table.maxn(f) == 0 then
		--			print("不在"..AreaLimit[2].."范围内,不能使用"..TempBag[Index].m_Name)
				 	return
				end
				--AreaNameStr = GetTrapDispalyName(AreaLimit[2])
			elseif AreaLimit[1] == "区域" then
				local pos = CPos:new()
				Player:GetGridPos(pos)
				local AreaName = g_AreaMgr:GetSceneAreaName(Player.m_Scene.m_SceneName, pos)
				if AreaName == nil or AreaName ~= AreaLimit[2] then
		--			print("不在"..AreaLimit[2].."范围内,不能使用"..TempBag[Index].m_Name)
					return
				end
				--AreaNameStr = g_AreaMgr:GetShowAreaName(AreaLimit[2])
			end
		end
		if TempBag[Index].m_SkillName then
			Player:PlayerDoItemSkill(TempBag[Index].m_SkillName, 1)
			TempBag:DelTempBagObjByIndex(Conn, Index)
		end
	end
end

function CTempBag:ClearTempBagAllObj(Player, flag)
--print("CTempBag:ClearTempBagAllObj(Conn)")
	local pos = GetCreatePos(Player)
	local Scene = Player.m_Scene
	for i = 1 , GridNum do
		if self[i] and self[i].m_Name then
			--print("1",self[i].m_Name)
			if flag then
				local Obj = CreateIntObj(Scene,RandomCreatPos(pos,1),self[i].m_Name)
			end
			self:DelTempBagObjByIndex(Player.m_Conn, i)
		end
	end
	Gas2Gac:CloseTempBag(Player.m_Conn)
	Player.m_TempBag = nil
end
