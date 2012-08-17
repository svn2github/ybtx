gac_gas_require "activity/item/FlowerInfoMgr"
CFlower = class()

--收获
function CFlower:GetItem(Conn, IsPerish)		--IsPerish::false表示正常收获,true表示枯萎产出
	
	local Player = Conn.m_Player
	local name = self.m_FlowerName
	local MsgId = nil
	if IsPerish then
		MsgId = 6009
	else
		MsgId = 6008
	end
	
	local function CallBack(IsPerish,result1,result2,result3,skill_info,GetCount)
		if IsCppBound(Conn) and IsCppBound(Player) then
			if not result1 then
				if IsNumber(result2) then
					MsgToConn(Conn, result2)
				end
				if IsPerish then
					MsgToConn(Conn, 9614, GetFlowerDispalyName(name))
					Gas2Gac:FlowerEnd(Conn)
					self:GrowthEnd(Player)
				end
			elseif IsNumber(result1) then
				Gas2Gac:CultivateFlowerStart(Conn, name, result1)
			else
				Gas2Gac:FlowerGiveItemCoolDownCD(Conn)
				if IsPerish or result2["IsDead"] then
					Gas2Gac:FlowerEnd(Conn)
					self:GrowthEnd(Player)
				end
				if IsPerish then
					MsgToConn(Conn, 9614, GetFlowerDispalyName(name))
				end
				if result2 then
					for i,itemtbl in ipairs(result2) do
						CRoleQuest.add_item_DB_Ret(Player, itemtbl["nType"],itemtbl["sName"],itemtbl["nCount"],itemtbl["AddItemRes"])
						Gas2Gac:ShowGatherItemsWnd(Conn, itemtbl["nType"], itemtbl["sName"], itemtbl["nCount"], MsgId, name)
					end
				end
				
				if result3 and result3 ~= 0 then
					MsgToConn(Conn, 9618, result3)
					if skill_info then
						CLiveskillBase.RetLiveskillInfo(Conn,skill_info)
					end
				end
				if IsNumber(GetCount) then
					Gas2Gac:UpdateFlowerGetCount(Conn, GetCount)
				end
			end
		end
	end
	
	local data = {}
	data["PlayerId"] = Player.m_uID
	data["IsPerish"] = IsPerish
	data["sceneName"] = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, "GatherLiveSkillDB", "GetItemAndAddLiveSkillExp", CallBack, data)
end

function CFlower:PerishGiveItem(Conn)

	if IsCppBound(Conn.m_Player) then				--玩家在线和收获的发放方式一样
		self:GetItem(Conn, true)
		return
	end
	local data = {}
	data["PlayerId"]    = self.m_MasterId
	data["MailTitle"]		= 1015 .. "_\"" .. GetFlowerDispalyName(self.m_FlowerName) .. "\""
	data["MailContent"]	= 1016
	
	CallDbTrans("GatherLiveSkillDB", "SendMailGiveGatherItem", nil, data, self.m_MasterId)
end

function CFlower.FlowerPerishGiveItem(Conn, IsPerish)
	if IsCppBound(Conn.m_Player) then
		if Conn.m_Player.m_Flower then
			Conn.m_Player.m_Flower:GetItem(Conn, IsPerish)
		end
	end
end

function CFlower.CareForFlower(Conn, param)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local flower = Conn.m_Player.m_Flower
	if not flower then
		return
	end
	if param ~= flower.m_State and param ~= 1 then
		MsgToConn(Conn, 9603 + param, GetFlowerDispalyName(flower.m_FlowerName))
		return
	end 
	local function CallBack(result, HealthPoint, IsAddCount)
		if not IsCppBound(Player) then
			return
		end
		if result then
			MsgToConn(Conn, 9601 + param, GetFlowerDispalyName(flower.m_FlowerName))
			if param ~= 1 then
				flower:SetFlowerState(Conn, 1)
			end
			Gas2Gac:AddFlowerHealthPoint(Conn, HealthPoint)
			Gas2Gac:CultivateFlowerSkillBtnEnable(Conn, CultivateFlowerParams["SkillCooldownTime"])
			Gas2Gac:PlayFlowerSkillEffect(Conn, param)
			if IsAddCount then
				MsgToConn(Conn, 9623)
				Gas2Gac:UpdateFlowerGetCount(Conn, IsAddCount)
			end
		elseif IsNumber(HealthPoint) then
			Gas2Gac:CultivateFlowerSkillBtnEnable(Conn, HealthPoint)	--冷却时间没到
		end
	end
	local data = {}
	data["PlayerID"]			= Player.m_uID
	data["SkillType"]			=	param
	data["IsSkill"]				= true
	CallAccountManualTrans(Conn.m_Account, "GatherLiveSkillDB", "UpdateFlowerHealthPoint", CallBack, data)
end

function CFlower.FlowerStateClean(Conn)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	if Player.m_Flower then
		Player.m_Flower:SetFlowerState(Conn, 1)
	end
end

function CFlower:SetFlowerState(Conn, state)
	self.m_State = state
	Gas2Gac:UpdateFlowerState(Conn, self.m_State)
end

--生长结束
function CFlower:GrowthEnd(Player)
	if self.m_EventTriggerTick then
		UnRegisterTick(self.m_EventTriggerTick)
	end
	self.m_EventTriggerTick = nil
	if self.m_EventTriggerUnTick then
		UnRegisterTick(self.m_EventTriggerUnTick)
	end
	self.m_EventTriggerUnTick = nil
	self = nil
	Player.m_Flower = nil
end

--每隔120至150秒的随机时间内 触发随机事件(正常生长,长虫,缺少阳光)
function CFlower:RegisterEventTriggerTick(Conn, TickTime, GrowthTime)

	local function EventFun()
		if not IsCppBound(Conn) then
			return
		end
		local Player = Conn.m_Player
		if not IsCppBound(Player) then
			return
		end
		local State = math.random(3)

		local function CallBack(result, HealthPoint)
			if not IsCppBound(Conn) then
				return
			end
			local Player = Conn.m_Player
			if not IsCppBound(Player) then
				return
			end
			if result then
				self.m_State = State
				local uMsgId = 9608 + self.m_State
				MsgToConn(Conn, uMsgId, GetFlowerDispalyName(self.m_FlowerName))
				Gas2Gac:UpdateFlowerState(Conn, self.m_State)
				Gas2Gac:AddFlowerHealthPoint(Conn, HealthPoint)
			end
		end
		local data = {}
		data["PlayerID"] = Player.m_uID
		data["StateType"] = State
		data["IsSkill"] = false
		
		CallAccountAutoTrans(Conn.m_Account, "GatherLiveSkillDB", "UpdateFlowerHealthPoint", CallBack, data)
	end
	local time = math.random(CultivateFlowerParams["TriggerTimeMin"],CultivateFlowerParams["TriggerTimeMax"])
	self.m_EventTriggerTick = RegisterTick("EventTriggerTick", EventFun, time*1000)
	local function TickFun()
		if self.m_EventTriggerTick then
			UnRegisterTick(self.m_EventTriggerTick)
		end
		UnRegisterTick(self.m_EventTriggerUnTick)
		self.m_EventTriggerUnTick = nil
	end
	if TickTime and IsNumber(TickTime) then
		if TickTime > GrowthTime then
			self.m_EventTriggerUnTick = RegisterTick("EventTriggerUnTick", TickFun, (TickTime-GrowthTime)*1000)
		else
			TickFun()
		end
	end
end

function CFlower.CultivateFlower(Conn, FlowerName)
	local Player = Conn.m_Player
	local flower = CFlower:new()
	flower.m_FlowerName = FlowerName
	flower.m_State = 1
	flower.m_MasterId = Player.m_uID
	Player.m_Flower = flower
	local GetCount = g_FlowerInfoMgr:GetFlowerInfo(FlowerName,"收获次数")
	local TickTime = g_FlowerInfoMgr:GetFlowerInfo(FlowerName,"事件时间")
	flower:RegisterEventTriggerTick(Conn, TickTime, 0)
	Gas2Gac:CultivateFlowerStart(Conn, FlowerName, 0)
	Gas2Gac:UpdateFlowerHealthPoint(Conn, 100)
	Gas2Gac:UpdateFlowerGetCount(Conn, GetCount)
end

function CFlower.ResumeCultivateFlower(Conn, Player, data)
	if not data then
		return
	end
	local FlowerName = data["flowerName"]
	local flower = CFlower:new()
	flower.m_FlowerName = FlowerName
	flower.m_State = 1
	flower.m_MasterId = Player.m_uID
	Player.m_Flower = flower
	local TickTime = g_FlowerInfoMgr:GetFlowerInfo(FlowerName,"事件时间")
	flower:RegisterEventTriggerTick(Conn, TickTime, data["GrowthTime"])
	
	Gas2Gac:CultivateFlowerStart(Conn, FlowerName, data["GrowthTime"])
	Gas2Gac:UpdateFlowerHealthPoint(Conn, data["HealthPoint"])
	Gas2Gac:UpdateFlowerGetCount(Conn, data["GetCount"])
	if data["CoolDownTime"] and data["CoolDownTime"] ~= 0 then
		Gas2Gac:CultivateFlowerSkillBtnEnable(Conn, data["CoolDownTime"])	
	end
end