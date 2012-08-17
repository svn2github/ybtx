
CItemBagLock	= class( SQRDialog )

function CItemBagLock.OpenItemBagLockWnd(Conn)
	if not IsCppBound(Conn.m_Player) then
		return 
	end
	
	local data = {["char_id"] = Conn.m_Player.m_uID}

	local function CallBack(result)
		if result[1] then
			Gas2Gac:ReturnLockItemBagByTime(Conn,result[2])
		else
			Gas2Gac:RetOpenIdentityAffirmWnd(Conn)
		end
	end
	
	CallAccountManualTrans(Conn.m_Account, "ItemBagLockDB","JudgeItemBagLock", CallBack, data)
end

function CItemBagLock.IdentityAffirm(Conn,sPassword)
	if not IsCppBound(Conn.m_Player) then
		return 
	end
	local data = {["charId"] = Conn.m_Player.m_uID,["sPassword"] = sPassword}

	local function CallBack(result)
		Gas2Gac:ReturnIdentityAffirm(Conn,result)
	end
	
	CallAccountManualTrans(Conn.m_Account, "ItemBagLockDB","IdentityAffirm", CallBack, data)
end

function CItemBagLock.LockItemBagByTime(Conn,lock_time)
	local data = {["charId"] = Conn.m_Player.m_uID,["lock_time"] = lock_time }
	
	local function CallBack(result)
		if result then
			Conn.m_Player.m_ItemBagLock = true
			Gas2Gac:ReturnLockItemBagByTime(Conn,lock_time)
		end
	end
	
	CallAccountManualTrans(Conn.m_Account, "ItemBagLockDB","LockItemBagByTime", CallBack, data)
end

function CItemBagLock.ItemBagLockTimeOut(Conn)
	local data = {["char_id"] = Conn.m_Player.m_uID}

	local function CallBack(result)
		if result[1] then
			Gas2Gac:ReturnLockItemBagByTime(Conn,result[2])
		else
			Conn.m_Player.m_ItemBagLock = false
			Gas2Gac:RetItemBagLockTimeOut(Conn)
		end
	end
	
	CallAccountManualTrans(Conn.m_Account, "ItemBagLockDB","JudgeItemBagLock", CallBack, data)
end



