
local bCheckRegister = true

--记录lua中的tick数目
g_RegisteredLuaTickNum = 0		

function SetCheckRegisterTick(bEnabled)
	bCheckRegister = bEnabled
end

local RegisterTickTbl = {}
setmetatable(RegisterTickTbl, {__mode = "k"})

local function _AddRegisterTick(Tick, TickName)
	if bCheckRegister then
		RegisterTickTbl[Tick] = TickName
	end
	g_RegisteredLuaTickNum = g_RegisteredLuaTickNum + 1 
end

local function _RemoveRegisterTick(Tick)
	if bCheckRegister then
		if Tick then
			RegisterTickTbl[Tick] = nil
		end
	end
	g_RegisteredLuaTickNum = g_RegisteredLuaTickNum - 1
end

function PrintUnRegisterTick()
	if not bCheckRegister then
		return 
	end
	
	collectgarbage("collect")

	if not next(RegisterTickTbl) then
		return
	end
	
	local msg_tbl = {}
	local name_tbl = {}
	for k, v in pairs(RegisterTickTbl) do
		if not name_tbl[v] then
			local msg = string.format("LuaTick %s 在进程关闭时候尚未注销\n", v)
			table.insert(msg_tbl, msg)
			name_tbl[v] = true
		end
	end
	
	local msg = table.concat(msg_tbl)
	
	local ret = LogErr(msg)
	if ret == 0 then
		db()
	end
end

function UnRegisterTick( Tick )
	g_App:UnRegisterTick( Tick )
	
	_RemoveRegisterTick(Tick)
end

local CNewLuaTick = class(CTick)

function CNewLuaTick:OnTick()
	return self.m_TickFunc(self, unpack(self.m_TickArg))
end

function RegisterTick( TickName, TickFun, TickTime, ... )
	local arg = {...}
	local Tick = CNewLuaTick:new()
	Tick.m_TickName = TickName
	Tick.m_TickFunc = TickFun
	Tick.m_TickArg = arg
	g_App:RegisterTick(Tick, TickTime)
	Tick:SetTickName(TickName)

	_AddRegisterTick(Tick, TickName)

	return Tick
end

local CNewLuaClassTick = class(CTick)

function CNewLuaClassTick:OnTick()
	return self.m_TickFunc(self.m_Obj, self, unpack(self.m_TickArg))
end

function RegClassTick( TickName, TickFun, TickTime, Obj, ... )
	local arg = {...}
	local Tick = CNewLuaClassTick:new()
	Tick.m_TickName = TickName
	Tick.m_TickFunc = TickFun
	Tick.m_Obj = Obj
	Tick.m_TickArg = arg
	g_App:RegisterTick(Tick, TickTime)
	Tick:SetTickName(TickName)
	
	_AddRegisterTick(Tick, TickName)

	return Tick
end





local CLuaDistortedTick		= class( CDistortedTick )

function CLuaDistortedTick:OnTick()
	return self.m_TickFun(unpack( self.m_TickArg ) )
end
	
function RegisterDistortedTickByCoreObj(FolObj, TickName, TickFun, TickTime, ... )
	local arg = {...}
	local DistortedTick = CLuaDistortedTick:new()
	DistortedTick.m_TickFun = TickFun
	DistortedTick.m_TickArg = arg
	FolObj:RegistDistortedTick( DistortedTick, TickTime )
	DistortedTick:SetTickName(TickName);

	_AddRegisterTick(DistortedTick, TickName)
	
	return DistortedTick
end

function UnRegisterDistortedTickByCoreObj( FolObj, Tick )
	FolObj:UnRegistDistortedTick( Tick )
	
	_RemoveRegisterTick(Tick)
end

function RegisterDistortedTick(TickName, TickFun, TickTime, ... )
	local arg = {...}
	local DistortedTick = CLuaDistortedTick:new()
	DistortedTick.m_TickFun = TickFun
	DistortedTick.m_TickArg = arg
	g_App:RegistDistortedTick( DistortedTick, TickTime )
	DistortedTick:SetTickName(TickName);

	_AddRegisterTick(DistortedTick, TickName)

	return DistortedTick
end

function UnRegisterDistortedTick(Tick)
	g_App:UnRegisterDistortedTick( Tick )
	
	_RemoveRegisterTick(Tick)
end
