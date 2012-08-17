--[[
	Module创建一个module，模仿lua的module机制
	ModuleName：该module的名字
	ModuleMeta：Module的metatable，如果为nil，则meta为global
	FuncEnv：该module所属的environment，可以是function level和func本身
--]]

local debug = debug
local package = package

local _ExportTbl = "_ExportTblAsClosure"

local _ModuleTbl = {}

function Module(ModuleName, ModuleMeta, FuncEnv)
	local ModuleTbl = {}
	
	assert(type(ModuleName) == "string")
	assert(not _ModuleTbl[ModuleName])
	_ModuleTbl[ModuleName] = ModuleTbl
	
	if not ModuleMeta then
		ModuleMeta = {
			__index = _G,
			__newindex = function(tbl, key, value)
				rawset(tbl, key, value)
			end		
		}
	else
		assert(type(ModuleMeta) == "table")
	end
	
	setmetatable(ModuleTbl, ModuleMeta)
	
	--给module增加一个导入表，我们所有的导入函数只能存在这个表里面
	rawset(ModuleTbl, _ExportTbl, {})
	
	--如果没有FuncEnv，那么我们设置调用Module的函数的environment
	if type(FuncEnv) == "nil" then
		FuncEnv = 2
	end
	
	--这里为啥不用sefenv而使用debug库，是因为lua以后的版本会将setfenv，getfenv给干掉了
	local info = debug.getinfo(FuncEnv, "f")
	
	debug.setfenv(info.func, ModuleTbl)
	
	return ModuleTbl
end


--[[
	这个函数用于module创建的时候传给创建的module作为其metatable的
	一般我们只会让创建的module访问一些lua提供的全局函数，但是是不能访问全局空间的
	如果设置了SandBox类型的，那么我们创建的module不能有全局变量
--]]
function GetModuleMt(SandBoxEnabled)
	local mt = {
		coroutine		= _G.coroutine,
		table			= _G.table,
		math			= _G.math,
		string			= _G.string,
		os				= _G.os,
		package			= _G.package,
		file			= _G.file,
		debug			= _G.debug,
		io				= _G.io,

		_VERSION		= _G.VERSION,
		assert 			= _G.assert,
		collectgarbage 	= _G.collectgarbage,
		dofile			= _G.dofile,
		error 			= _G.error,
		getfenv			= _G.getfenv,
		getmetatable 	= _G.getmetatable,
		ipairs 			= _G.ipairs,
		load			= _G.load,
		loadfile		= _G.loadfile,
		loadstring		= _G.loadstring,
		module			= _G.module,
		next 			= _G.next,
		pairs			= _G.pairs,
		pcall			= _G.pcall,
		print			= _G.print,
		rawequal		= _G.rawequal,
		rawget			= _G.rawget,
		rawset			= _G.rawset,
		require			= _G.require,
		select			= _G.select,
		setfenv			= _G.setfenv,
		setmetatable 	= _G.setmetatable,
		tonumber		= _G.tonumber,
		tostring		= _G.tostring,
		type			= _G.type,
		unpack			= _G.unpack,
		xpcall			= _G.xpcall,

		
		db				= 	_G.db,
		Export			=	_G.Export
	}
	
	for k, v in pairs(LoadPathTbl) do
		local ImportFunc = k .. "_import"
		mt[ImportFunc] = _G[ImportFunc]	

		local RequireFunc = k .. "_require"
		mt[RequireFunc] = _G[RequireFunc]	
	end
	
	mt.__index = function(tbl, key)
		return rawget(mt, key)
	end
	
	if SandBoxEnabled then
		mt.__newindex = function(tbl, key, value)
			if type(value) ~= "function" then
				error("不允许设置除函数以外的其他全局变量")
			end
			
			rawset(tbl, key, value)
		end	
	end
	
	return mt
end


AddCheckLeakFilterObj(Module)
AddCheckLeakFilterObj(GetModuleMt)





