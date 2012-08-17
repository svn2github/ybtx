
CGasShortcut = class()

function CGasShortcut.SaveShortCut(Conn, pos, type, arg1, arg2, arg3)
	if Conn.m_Player == nil then
		return
	end
	local data = {["Pos"] = pos, ["Type"] = type, 
	["Name"] = arg1, ["Level"] = arg2, ["IsMainBar"] = arg3,
	["PlayerId"] = Conn.m_Player.m_uID }
	Conn.m_Player.m_ShortCutTbl = Conn.m_Player.m_ShortCutTbl ~= nil and  Conn.m_Player.m_ShortCutTbl or {}
	table.insert(Conn.m_Player.m_ShortCutTbl,data)
end

function CGasShortcut.SaveShortCutEnd(Conn)
	if Conn.m_Player == nil then
		return
	end
	if Conn.m_Player.m_ShortCutTbl ~= nil and #(Conn.m_Player.m_ShortCutTbl) >= 1 then
		local data  = Conn.m_Player.m_ShortCutTbl
		Conn.m_Player.m_ShortCutTbl = nil
		CallAccountAutoTrans(Conn.m_Account, "ShortcutDB", "SavePlayerShortCut", nil, data)
	end
end