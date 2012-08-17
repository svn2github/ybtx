engine_require "common/Loader/export_loader"
engine_require "common/Loader/cfg_shell"

local LoadedCfgTbl = LoadedCfgTbl
local GetLoadPath = GetLoadPath

local _eTFFT_String = ETableFileFieldType.eTFFT_String
local _eTFFT_Number = ETableFileFieldType.eTFFT_Number
local _GetHeight= CTxtTableFile.GetHeight
local _GetWidth = CTxtTableFile.GetWidth
local _GetType = CTxtTableFile.GetType
local _GetTypeByName = CTxtTableFile.GetTypeByName
local _GetString = CTxtTableFile.GetString
local _GetStringByName = CTxtTableFile.GetStringByName
local _GetNumber= CTxtTableFile.GetNumber
local _GetNumberByName = CTxtTableFile.GetNumberByName
local _GetValue = CTxtTableFile.GetValue
local _GetValueByName = CTxtTableFile.GetValueByName
local _Load = CTxtTableFile.Load

local TxtData = CTableFileValue:new()

function CTxtTableFile.Clear()
	TxtData = nil
end

function CTxtTableFile:GetData(RowIndex, ColIndex)
	return _GetValue(self, TxtData, RowIndex, ColIndex)
end

function CTxtTableFile:GetDataByName(RowIndex, ColName)
	return _GetValueByName(self, TxtData, RowIndex, ColName)
end

local _GetData= CTxtTableFile.GetData
local _GetDataByName = CTxtTableFile.GetDataByName

CTxtTableFile.__call = function(tbl, RowKey, ColName)
				if RowKey == nil and ColName == nil then
					return nil
				elseif RowKey ~= nil and ColName ~= nil then
					--RowName和ColName都不为空，我们返回相应的数据
					assert(type(ColName) == "string")
					
					local Row = tbl.RowKey2Index[RowKey]
					if not Row then
						return nil
					end
					
					return _GetDataByName(tbl, Row, ColName)
					
				elseif RowKey ~= nil and ColName == nil then
					local Row = tbl.RowKey2Index[RowKey]
					if not Row then
						return nil
					end
					
					--RowName不为空，我们返回一个函数，这个函数能够直接通过传入的colname得到数据
					
					return function(ColIndexName)
						assert(type(ColIndexName) == "string")
						
						return _GetDataByName(tbl, Row, ColIndexName)
					end
				else
					assert(type(ColName) == "string")
					return function(RowIndexKey)
						local Row = tbl.RowKey2Index[RowKey]
						if not Row then
							return nil
						end
												
						return _GetDataByName(TableFile, Row, ColName)
					end
				end								
			end
			

function CTxtTableFile:GetKeys()
	return self.RowKeys
end
			
CTxtTableFile.__newindex = function(tbl, key, value)
	error("can't set value in readonly table")
end

local function DummyFunc()
end

--新的单行表机制使用函数，最好让单行表都通过索引访问
function LoadSingle(Language, AliasPath, ModuleName, ...)
	if Language == "" and LoadedCfgTbl[ModuleName] then
		return LoadedCfgTbl[ModuleName]
	end
	
	local argc = {...}
	local argn = select("#", ...)

	local function _LoadSingle()
		local TableFile = CTxtTableFile:new()
		local ret = TableFile:Load(AliasPath, unpack(argc, 1, argn))
		if not ret then
			local msg = ModuleName .. " 载入出错，请检查文件"
			error(msg)
		end
	
		local RowKey2Index = {}
		local RowKeys = {}
		
		local Height = _GetHeight(TableFile)
		
		--为每一行的索引名字设置对应的行号，我们的单行表的row是通过第一列的索引来进行的
		for i = 1, Height - 1 do
			local IndexKey = _GetData(TableFile, i, 0)
			assert(not RowKey2Index[IndexKey],"配置表【"..ModuleName.."】的索引【"..IndexKey.."】重复")
			RowKey2Index[IndexKey] = i	
			table.insert(RowKeys, IndexKey)
		end
		
			
		rawset(TableFile, "RowKey2Index", RowKey2Index)
		rawset(TableFile, "RowKeys", RowKeys)	
	
		local function GetHeight()
			return Height - 1
		end
		
		rawset(TableFile, "GetHeight", GetHeight)
		
		if Language == "" then
			_G[ModuleName] = TableFile
		else
			if _G["Lan_" .. Language] == nil then
				_G["Lan_" .. Language] = {}
			end
			_G["Lan_" .. Language][ModuleName] = TableFile
		end
		LoadedCfgTbl[ModuleName] = TableFile
		
		AddCheckLeakFilterObj(TableFile)
	
		return TableFile		
	end

	if ExportClassEnabled() then
		return ExportCfgLoad(Language, ModuleName, _LoadSingle)
	else
		return _LoadSingle()
	end
end


