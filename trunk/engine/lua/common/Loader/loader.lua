language = ""

function find_last(src_str, pattern)
	local tmp = string.reverse(src_str)
	local pos = string.find(tmp, pattern)
	if pos == nil then
		return 0
	end 
	return string.len(src_str) - pos + 1 
end

function SplitFileName(FileName)
	local first_sep = string.find(FileName, "./", 1, true)
	if first_sep then
		FileName = string.sub(FileName, 3)
	end
	return FileName
end

function GetModuleName(FileName)
	local last_find_index = find_last(FileName, "/")
	local ModuleName = string.sub(FileName, last_find_index + 1)
	
	return ModuleName
end

--------------------------------------------------------------

LoadedCfgTbl = LoadedCfgTbl or {}

AddCheckLeakFilterObj(LoadedCfgTbl)

function ClearLoadedCfgTbl()
	for k, v in pairs(LoadedCfgTbl) do
		if type(v) == "table" then
			v:Release()
		end
	end
	
	CTxtTableFile.Clear()
	CMultiLangTxtTableFile.Clear()
	CRowTableFile.Clear()
end

function UnloadCfgTbl(ModuleName)
	LoadedCfgTbl[ModuleName] = nil
	_G[ModuleName] = nil
end

function _load_single(alias, file_name)	
	local file_name = SplitFileName(file_name)
	local module_name = GetModuleName(file_name)

	if LoadedCfgTbl[module_name] then
		return LoadedCfgTbl[ModuleName]
	end
	
	RequireFile(alias, file_name)
	
	local src_tbl = _G[module_name]

	--rawset(_G, module_name, nil)
	rawset(_G, module_name, {})
	local tbl_data = {}
	
	local mt = {}
	local fun = function() error("attempt to update a read-only table", 2) end
	for i, p in pairs(src_tbl) do
		if type(i) == "number" then
			local index = src_tbl[i][src_tbl.__tbl_index]	
			if tbl_data[index] then
				--print(index)
				local str = "\n索引需要唯一, 处于'" .. file_name .. ".lua'文件中，重复的索引值为：'" .. index .. "'\n"
				error(str, 2)
				return
			end
			tbl_data[index] = p
			_G[module_name][index] = {}
			mt  = {  __index = tbl_data[index] , __newindex = fun }
			setmetatable( _G[module_name][index], mt )	
		end
	end
	rawset(src_tbl, "__tbl_index", nil)
	
	--src_tbl = {}
	--rawset(_G, module_name, {})
	mt = {  __index = tbl_data , __newindex = fun }
	--_G[module_name] = tbl_data	
	setmetatable( _G[module_name], mt )
	--rawset(_G, module_name, tbl_data)

	
	LoadedCfgTbl[module_name] = true

	AddCheckLeakFilterObj(_G[module_name])
	
	for k, v in pairs(_G[module_name]) do
		AddCheckLeakFilterObj(v)	
	end

	return tbl_data
	
end

function _load_multi(AliasPath, FileName)
	local FileName = SplitFileName(FileName)
	local ModuleName = GetModuleName(FileName)
	
	if LoadedCfgTbl[ModuleName] then
		return
	end
	
	RequireFile(AliasPath, FileName)

	src_tbl = _G[ModuleName]
	rawset(src_tbl, "__tbl_index", nil)
	
	AddCheckLeakFilterObj(_G[ModuleName])
	
	for k, v in pairs(_G[ModuleName]) do
		AddCheckLeakFilterObj(v)
	end
	
	LoadedCfgTbl[ModuleName] = true
end

-----------------------------------------------------------

engine_require("common/Loader/single_loader")
engine_require("common/Loader/multi_loader")
engine_require("common/Loader/lan_single_loader")

local _GetTableFileType = GetTableFileType
local _eTFT_Txt = ETableFileType.eTFT_Txt
local _eTFT_Row = ETableFileType.eTFT_Row
local _eTFT_None = ETableFileType.eTFT_None


local _LoadSingle = LoadSingle
local _LoadMulti = LoadMulti
local _LoadLanSingle = LoadLanSingle

local LoadPathTbl = LoadPathTbl

local function _load_list(AliasPath, ModuleName, ...)
	local arg = {...}
	local n = select("#", ...)
	
	local path = ""
	if language == "" then
		path = LoadPathTbl[AliasPath] 
	else
		path = language .. "/table/"
	end
	for i = 1, n do
		arg[i] = path .. arg[i]
	end
	
	local eType = _GetTableFileType(AliasPath, arg[1])
	if eType == _eTFT_Txt then
			if AliasPath == "lan" then
				return _LoadLanSingle(language, AliasPath, ModuleName, unpack(arg, 1, n))
		else
		  return _LoadSingle(language, AliasPath, ModuleName, unpack(arg, 1, n))
		end
	elseif eType == _eTFT_Row then
		return _LoadMulti(language, AliasPath, ModuleName, unpack(arg, 1, n))
	elseif eType == _eTFT_None then
		error(arg[1] .. ".xml文件打开失败，可能文件已损坏或丢失")
	else
		error(arg[1] .. "不是single或multi")
	end	
	
end

local function _load(AliasPath, FileName)
	local FileName = SplitFileName(FileName)
	local ModuleName = GetModuleName(FileName)
	--if ModuleName ~= "" then
	--	ModuleName = language .. ModuleName
	--end

	return _load_list(AliasPath, ModuleName, FileName)
end


function cfg_load(FileName)
	language = ""
	return _load("cfg", FileName)
end

function lan_load(FileName)
	language = ""
	return _load("lan", FileName)
end

function mult_lan_load(lang, FileName)
	language = lang
	return _load("lan", FileName)
end

function cfg_load_list(ModuleName, ...)
	language = ""
	return _load_list("cfg", ModuleName, ...)
end

function lan_load_list(ModuleName, ...)
	language = ""
	return _load_list("lan", ModuleName, ...)
end

function mult_lan_load_list(lang, ModuleName, ...)
	language = lang
	return _load_list("lan", ModuleName, ...)
end

function cfg_reload(ModuleName, ...)
	language = ""
	local TableFile = LoadedCfgTbl[ModuleName]
	if not TableFile then
		error(ModuleName .. "配置表不存在，不能reload")
	end
	
	return TableFile:Reload(...)
end




