gas_require "world/other_activity/CrazyHorseInc"

function CCrazyHorse:Ctor(uPlayerId)
	self.m_uPlayerId = uPlayerId
end

local function PSOnTick(Tick, Connection, Player, StateName, Adder)
	if not Player:IsMoving() then
		return
	else
		Player.m_CHPSPos = Player.m_CHPSPos + Adder
	end
	if Player.m_CHPSPos > 1000 then
		Player.m_CHPSPos = 1000
	elseif Player.m_CHPSPos < 0 then
		Player.m_CHPSPos = 0
		Player:ClearState("骑疯马")
		UnRegisterTickCrazyHorseTick(Player)
	end
	Gas2Gac:RetPhysicalStrengh(Connection,Player.m_CHPSPos)
end

local function CheckCrazyHorseState(data,IsChangeState)
	local Connection,Player,StateName = unpack(data)
	if Player:ExistState(StateName) or IsChangeState then
		Gas2Gac:RetCheckCamelState(Connection, StateName, true)
		if StateName == "疯马加速状态1" then
			Player.m_CHPhysicalStrenghAdder = 0
		elseif StateName == "疯马加速状态2" then
			Player.m_CHPhysicalStrenghAdder = -10
		elseif StateName == "疯马加速状态3" then
			Player.m_CHPhysicalStrenghAdder = -20
		elseif StateName == "疯马加速状态4" then
			Player.m_CHPhysicalStrenghAdder = -30
		elseif StateName == "疯马加速状态5" then
			Player.m_CHPhysicalStrenghAdder = -40
		elseif StateName == "疯马加速状态6" then
			Player.m_CHPhysicalStrenghAdder = -50
		elseif StateName == "疯马加速状态7" then
			Player.m_CHPhysicalStrenghAdder = -60
		elseif StateName == "疯马加速状态8" then
			Player.m_CHPhysicalStrenghAdder = -70
		elseif StateName == "疯马加速状态9" then
			Player.m_CHPhysicalStrenghAdder = -80
		elseif StateName == "疯马加速状态10" then
			Player.m_CHPhysicalStrenghAdder = -90
		end
		local mail_reciever = CCrazyHorse:new(Connection.m_Player.m_uID)
		if not Player.m_CHPSPos or StateName == "疯马加速状态1" then --体力 
			Player.m_CHPSPos = 1000
		end
		UnRegisterTickCrazyHorseTick(Player)
		Player.CrazyHorsePhysicalStrengh_Tick=RegisterTick("PSOnTick", PSOnTick, 2000, Connection, Player, StateName, Player.m_CHPhysicalStrenghAdder)
	elseif (not Player:ExistState("疯马加速状态10")) and (not Player:ExistState("疯马加速状态9")) and (not Player:ExistState("疯马加速状态8")) and (not Player:ExistState("疯马加速状态7")) and (not Player:ExistState("疯马加速状态6"))
		and (not Player:ExistState("疯马加速状态5")) and (not Player:ExistState("疯马加速状态4")) and (not Player:ExistState("疯马加速状态3")) and (not Player:ExistState("疯马加速状态2")) and (not Player:ExistState("疯马加速状态1")) then
			Player.m_CHPSPos = 1000
			Player.m_CHPhysicalStrenghAdder = 0
	end
end

function UnRegisterTickCrazyHorseTick(Player)--  任务完成， 注销Tick
	if Player and Player.CrazyHorsePhysicalStrengh_Tick then
		UnRegisterTick(Player.CrazyHorsePhysicalStrengh_Tick)
		Player.CrazyHorsePhysicalStrengh_Tick = nil
	end
end

function Gac2Gas:CheckCrazyHorseState(Connection, StateName)
	local Player = Connection.m_Player
	local data = {Connection,Player,StateName}
	local questname = "疯狂的旅行"
	if not Player:ExistState("骑疯马") then
		UnRegisterTickCrazyHorseTick(Player)
		Gas2Gac:RetPhysicalState( Connection, false)
		Gas2Gac:ClearCompassHeadDir(Connection)
		return
	elseif StateName == "骑疯马" then
		Gas2Gac:RetPhysicalState( Connection, true)
		GetArrowHeadFromItem(Connection,"Npc","阿斯瑞拉",-1)
	end
	local function CallBack(result)
		if result then
			--TODO   根据StateName,曾经达到状态需求   	任务计数
			if IsCppBound(Player) then
				Gas2Gac:RetQuestVar(Player.m_Conn, questname, StateName, result)
			end
		end
		CheckCrazyHorseState(data,true)
	end
	if Connection.m_Player:ExistState(StateName) then
		CheckCrazyHorseState(data,true)
	else
		CheckCrazyHorseState(data)
	end
end