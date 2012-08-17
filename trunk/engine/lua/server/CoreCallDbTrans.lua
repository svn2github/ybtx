engine_require "server/VMChannelMgr"

g_App.CallDbEndEvent = CEvent:new(false)
g_App.CallDbEndEvent.RefNum = 0

function CheckDbTrans(filename, funcname)
	assert(type(filename) == "string" and type(funcname) == "string")
	 
	if g_App.m_bCallDbHalted and g_App.CallDbEndEvent.RefNum == 0 then
		--如果逻辑层调用了quit，那么这个值会为true，以后所有调用CallDbTrans都会报错，因为数据库已经在关闭了
		local err = "调用" .. filename .. "上数据库操作函数" .. funcname .. "出错，DBVM正处于退出过程中，不允许进行新的Transaction调用"
		error(err)
	end

	g_App.CallDbEndEvent:Set(false)
	g_App.CallDbEndEvent.RefNum = g_App.CallDbEndEvent.RefNum + 1	
end

local CheckDbTrans = CheckDbTrans

function GlobalDbTrans(filename, funcname, callback, data, errfunc, ...)
	CheckDbTrans(filename, funcname)
	
	RequestVM(CallDbTransaction, filename, funcname, callback, data, errfunc,...)
end

function MultiDbTrans(filename, funcname, callback, data, errfunc, ...)
	CheckDbTrans(filename, funcname)
	
	RequestVM(CallDbTransaction, filename, funcname, callback, data, errfunc,...)
end
