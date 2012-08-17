gas_require "message/channel/DySysMessageInc"


------------------------------------rpc functiion--------------------------------------------------
local function get_all_sys_dyn_msg(Conn)
	for i, v in pairs(g_sys_dyn_msg) do
		Gas2Gac:RetGetSysDynMsg(Conn, unpack(v))
	end
end

function Gac2Gas:GetAllSysDynMsg(Conn)
	get_all_sys_dyn_msg(Conn)
end

local function get_recent_sys_dyn_msg()
	local p = 0
	for i, v in pairs(g_sys_dyn_msg) do
		p = p + v.power
	end
	local index = math.random(1, p)
	local recent_positon = g_sys_dyn_msg[1]
	p = 0
	for i, v in pairs(g_sys_dyn_msg) do
		p = p + v.power
		if p >= index then
			return recent_positon
		end
		recent_positon = v
	end
end

function Gac2Gas:GetRecentSysDynMsg(Conn)
	local msg = get_recent_sys_dyn_msg()
	Gas2Gac:RetGetSysDynMsg(Conn, unpack(msg))
end
