cfg_load "fb_game/DrinkName_Common"

local g_tiredness = {}
--DrinkTick = {}
local g_drinkedName = {}
--¼ì²é¼ä¸ôÊ±¼ä
local function CheckIntervalTime(Conn, DrinkName)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return false
	end
	
	local playerId = Conn.m_Player.m_uID
	local CurrentTime = os.time()
	local DrinkedName = g_drinkedName[playerId]
	if DrinkedName == nil or DrinkedName == DrinkName then
		return true
	end
	if g_tiredness[playerId] == nil then
		return true
	end
	--Ê®·ÖÖÓÄÚ²»ÄÜ»»ºÈÆäËû¾Æ
	if g_tiredness[playerId][DrinkedName] then
		if CurrentTime - g_tiredness[playerId][DrinkedName].LastDrinkTime < 600 then
			local time = math.floor((600 - CurrentTime + g_tiredness[playerId][DrinkedName].LastDrinkTime) / 60)
			MsgToConn(Conn, 3257, time)
			return false
		end
	end
	return true
end

local function DoDrink(Conn, Money, DrinkName, DrinkND, ClickedItemSkill)
	local function CallBack(result,uMsgID)
		if not result then
			if IsNumber(uMsgID) then
				MsgToConn(Conn, uMsgID)
			else
				MsgToConn(Conn, 3258)
			end
			return
		else
			local Player = Conn.m_Player
			local CurrentTime = os.time()
			local playerId = Conn.m_Player.m_uID
			if g_tiredness[playerId] == nil then
				g_tiredness[playerId] = {}
			end
			if g_tiredness[playerId][DrinkName] == nil then
				g_tiredness[playerId][DrinkName] = {}
			end
			if g_tiredness[playerId][DrinkName].value == nil then
				g_tiredness[playerId][DrinkName].value = 0
			end
			local randomNum = math.random(0, 100)
			local sign = false
			if randomNum <= g_tiredness[playerId][DrinkName].value then
				sign = true
				Player:PlayerDoItemSkill("Çå³ý¾ÆÀàBUFF", 1)
				Player:PlayerDoItemSkill("¾Æ°ÉºÈ×í", 1)
			end
			g_tiredness[playerId][DrinkName].value = g_tiredness[playerId][DrinkName].value + DrinkND
			if g_tiredness[playerId][DrinkName].value >= 100 then
				if not sign then
					sign = true
					Player:PlayerDoItemSkill("Çå³ý¾ÆÀàBUFF", 1)
					Player:PlayerDoItemSkill("¾Æ°ÉºÈ×í", 1)
				end
				g_tiredness[playerId][DrinkName].value = 100
			end
			g_tiredness[playerId][DrinkName].LastDrinkTime = CurrentTime
			if g_tiredness[playerId][DrinkName].StartDrinkTime == nil then
				g_tiredness[playerId][DrinkName].StartDrinkTime = CurrentTime
			end
			Gac2Gas:GetTiredNess(Conn, DrinkName)
			if not sign then
				if not Player:ExistState(ClickedItemSkill.."Ä§·¨×´Ì¬") then
					Player:PlayerDoItemSkill("Çå³ý¾ÆÀàBUFF", 1)
				end
				Player:PlayerDoItemSkill(ClickedItemSkill, 1)
			end
			g_drinkedName[playerId] = DrinkName
		end
	end
	local parameter = {}
	parameter["PlayerId"] = Conn.m_Player.m_uID
	parameter["MoneyNum"] = Money
	parameter["Func"] = "Quest"
	parameter["ModName"] = "Drink"
	CallAccountAutoTrans(Conn.m_Account, "MoneyMgrDB", "IsMoneyEnough", CallBack, parameter)
end

function Gac2Gas:HaveDrink(Conn, DrinkName)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	
	local drink = DrinkName_Common(DrinkName)
	local ClickedItemMoney = drink("Price")
	local ClickedItemConsistence = drink("Consistence")
	local ClickedItemSkill = drink("Effect")
	if not CheckIntervalTime(Conn, DrinkName) then
		return
	end
	DoDrink(Conn, ClickedItemMoney, DrinkName, ClickedItemConsistence, ClickedItemSkill)
end

function Gac2Gas:GetTiredNess(Conn, DrinkName)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	
	local tirednessValue = 0
	if g_tiredness[Conn.m_Player.m_uID] then
		if g_tiredness[Conn.m_Player.m_uID][DrinkName] then
			tirednessValue = g_tiredness[Conn.m_Player.m_uID][DrinkName].value
			local CurrentTime = os.time()
			local times = math.floor((CurrentTime - g_tiredness[Conn.m_Player.m_uID][DrinkName].StartDrinkTime) / 600)
			if tirednessValue - 4 * times > 0 then
				g_tiredness[Conn.m_Player.m_uID][DrinkName].value = tirednessValue - 4 * times
				g_tiredness[Conn.m_Player.m_uID][DrinkName].StartDrinkTime = g_tiredness[Conn.m_Player.m_uID][DrinkName].StartDrinkTime + 600 * times
				tirednessValue = tirednessValue - 4 * times
			else
				g_tiredness[Conn.m_Player.m_uID][DrinkName].value = nil
				g_tiredness[Conn.m_Player.m_uID][DrinkName].StartDrinkTime = nil
				g_tiredness[Conn.m_Player.m_uID][DrinkName].LastDrinkTime = nil
				g_tiredness[Conn.m_Player.m_uID][DrinkName] = nil
				g_tiredness[Conn.m_Player.m_uID] = nil
			end
		end
	end
	Gas2Gac:GetTiredNess(Conn, tirednessValue)
end

function Gac2Gas:IsDrunk(Conn)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	
	local sign1 = Conn.m_Player:ExistState("¾Æ°ÉºÈ×í×´Ì¬")
	local sign2 = Conn.m_Player:ExistState("¾Æ°É´òàÃ×´Ì¬")
	if sign1 or sign2 then
		Gas2Gac:IsDrunk(Conn, true)
	else
		Gas2Gac:IsDrunk(Conn, false)
	end
end