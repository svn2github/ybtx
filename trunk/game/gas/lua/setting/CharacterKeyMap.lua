cfg_load "key_map/KeyMap_Common"

CKeyMapMgr = class()
local KeyMapDB = "KeyMapDB"

-----------------------------RPC函数-----------------------------
--设置按键映射
function CKeyMapMgr.SetKeyMap(Conn, sFuncName, sKey, uPosition, Setstyle) --Setstyle 为true时update,为false时delete 此映射恢复到默认,从静态文件读取

	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	
	--检测FuncName是否在配置表存在
	local uCount = KeyMap_Common:GetHeight()
	for k = 1,uCount do
		if(KeyMap_Common(k,"Name") == sFuncName) then
			break
		end
		if k == uCount then
			LogErr("设置了不存在按键功能", sFuncName)
		end
	end

	Player.tblSysSettingInfo = Player.tblSysSettingInfo or {}
	Player.tblSysSettingInfo["KeyMapSetting"] = Player.tblSysSettingInfo["KeyMapSetting"] or {}
	
	if (string.len(sFuncName) > 31 or string.len(sKey) > 15) then
		return 
	end
	assert(IsNumber(uPosition))
	local parameter = {}
	parameter["char_id"] = Conn.m_Player.m_uID
	parameter["FuncName"] = sFuncName
	parameter["Key"] = sKey
	parameter["Position"] = uPosition
	parameter["Style"] = Setstyle
	
	table.insert(Player.tblSysSettingInfo["KeyMapSetting"],parameter)
end

function CKeyMapMgr.InitAllKeyMaps(player,result)
	if not player then
		return
	end
	local KeyMap, MouseMap = unpack(result)
	if KeyMap then
		for i = 1, KeyMap:GetRowNum() do
			Gas2Gac:ReturnAllKeyMaps(player.m_Conn, KeyMap(i,1),KeyMap(i,2),KeyMap(i,3))
		end
	end
	if MouseMap and MouseMap:GetRowNum()>0 then
		Gas2Gac:ReturnMouseKey(player.m_Conn, MouseMap(1,1), MouseMap(1,2) ,MouseMap(1,3),
		 MouseMap(1,4), MouseMap(1,5), MouseMap(1,6), MouseMap(1,7))
	end
	Gas2Gac:ReturnAllKeyMapsEnd(player.m_Conn)
end
--获取所有按键映射
function CKeyMapMgr.GetAllKeyMaps(Conn)
	if not Conn.m_Player then
		return
	end
	
	local function CallBack(result)
		CKeyMapMgr.InitAllKeyMaps(Conn.m_Player,result)
	end
	
	local parameter = {}
	parameter["char_id"] = Conn.m_Player.m_uID

	--local key_map_db = (g_DBTransDef["KeyMapDB"])
	CallAccountLoginTrans(Conn.m_Account,KeyMapDB, "GetAllKeyMap", CallBack, parameter)
end


function CKeyMapMgr.ChangeMouseCtrl(Conn, lockTarget, movekey, selectLeft, selectRight, attrackLeft, attrackRight, distance)
	local Player = Conn.m_Player
	if not Player or not IsCppBound(Player) then
		return
	end
	if distance<1 or distance > 20 then
		return
	end
	
	Player.tblSysSettingInfo = Player.tblSysSettingInfo or {}
	
	local parameter = {}
	parameter["lockTarget"]		= lockTarget
	parameter["movekey"]		= movekey
	parameter["selectLeft"]		= selectLeft
	parameter["selectRight"]		= selectRight
	parameter["attrackLeft"]		= attrackLeft
	parameter["attrackRight"]		= attrackRight
	parameter["distance"]		= distance
	parameter["char_id"]			= Conn.m_Player.m_uID
	
	Player.tblSysSettingInfo["MouseSetting"] = parameter
end

function CKeyMapMgr.ChangeMouseCtrlBeforeGame(Conn, charID, type, bShowMsgBox)
	local parameter = {}
	parameter["type"]			= type
	parameter["char_id"]		= charID
	parameter["bShowMsgBox"]	= bShowMsgBox and 1 or 0
	
	local function CallBack(result)
		Gas2Gac:ReturnChangeMouseCtrlBeforeGame(Conn, result)
	end
	
	CallAccountLoginTrans(Conn.m_Account, "SysSettingDB", "MouseSettingDB", CallBack, parameter)
end

