local function DummyFunc()
end

function ExportCfgLoad(Language, ModuleName, LoadFunc)		
	local Data = {}
	
	local function GetRealIndex(TableFile, key)
		local value = TableFile[key]
		if type(value) == "function" then
			local function GetFunc(tbl, ...)
				return value(TableFile, ...)
			end
			return GetFunc
		else
			return value
		end
	end
	
	local RealMt = {
		__index = function(tbl, key)
			local TableFile = rawget(tbl, "__TableFile")
			return GetRealIndex(TableFile, key)
		end,
		__newindex = function(tbl, key, value)
			error("can't set value in readonly table")
		end,
		__call = function(tbl, ...)
			local TableFile = rawget(tbl, "__TableFile")
			return TableFile(...)
		end,
	}
	
	local function ResetData(tbl, TableFile)
		rawset(tbl, "__TableFile", TableFile)
		setmetatable(tbl, RealMt)
	end
	
	local DummyMt = {
		__index = function(tbl, key)
			if key == "Release" then
				return DummyFunc
			end
			
			local TableFile = LoadFunc()
			ResetData(tbl, TableFile)
			return GetRealIndex(TableFile, key)
		end,
		__newindex = function(tbl, key, value)
			error("can't set value in readonly table")
		end,
		__call = function(tbl, ...)
			local TableFile = LoadFunc()
			ResetData(tbl, TableFile)
			
			return TableFile(...)
		end,
	}
	
	setmetatable(Data, DummyMt)
	
	if Language == "" then
		_G[ModuleName] = Data
	else
		if _G["Lan_" .. Language] == nil then
			_G["Lan_" .. Language] = {}
		end
		_G["Lan_" .. Language][ModuleName] = Data
	end
	LoadedCfgTbl[ModuleName] = Data
	
	AddCheckLeakFilterObj(Data)	
	
	return Data
end