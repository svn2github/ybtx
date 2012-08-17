 --有的RegisterOnceTick 时候 LifecycleObj 需要全局的就使用g_App, 
--尽量使用小的生命周期域的对象 如 scene, npc, obj, playe 之类的

local function OnOnceTickFun(tick, ...)
	--print("OnOnceTickFun,  tickId 为:",  tick.m_Index)
	tick.m_LifecycleObj.m_OnceTickTbl[tick.m_Index] = nil
	UnRegisterTick(tick)
	tick.m_SourceFun(...)
end

local OnceTickIndex = 1
function RegisterOnceTick(LifecycleObj , TickName, TickFun, TickTime, ...)
	assert(LifecycleObj and LifecycleObj.m_OnceTickTbl, "这个对象还不支持 注册oncetick")
	local tick = RegisterTick(TickName, OnOnceTickFun, TickTime, ...)
	tick.m_SourceFun = TickFun
	tick.m_LifecycleObj = LifecycleObj
	tick.m_Index = OnceTickIndex
	LifecycleObj.m_OnceTickTbl[OnceTickIndex] = tick

	OnceTickIndex = OnceTickIndex + 1
end

function RegOnceTickLifecycleObj(name, LifecycleObj)
	assert(LifecycleObj.m_OnceTickTbl == nil, name .. "  重复注册为Oncetick的承载体")
	LifecycleObj.m_OnceTickTbl = {}
	--print(" 注册Oncetick的承载体: " .. name)
	--LifecycleObj.m_OnceTickName = name
	--RegMemCheckTbl( name.. ".m_OnceTickTabl", LifecycleObj.m_OnceTickTbl)
end

function UnRegisterObjOnceTick(LifecycleObj)
	--print(LifecycleObj.m_OnceTickName .. " 的生命周期到了, 注销身上所有onceTick")
	for index, tick in pairs(LifecycleObj.m_OnceTickTbl) do
		--print("tick所挂的对象生命期结束 , 注销未完成的tick   id:", index)
		UnRegisterTick(tick)
		LifecycleObj.m_OnceTickTbl[index] = nil
	end
end