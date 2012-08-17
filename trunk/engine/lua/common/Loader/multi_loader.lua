engine_require "common/Loader/export_loader"

local LoadedCfgTbl = LoadedCfgTbl
local GetLoadPath = GetLoadPath

local _eTFFT_String = ETableFileFieldType.eTFFT_String
local _eTFFT_Number = ETableFileFieldType.eTFFT_Number
local _eTFFT_Empty = ETableFileType.eTFFT_Empty

local _eRTNT_Tree = ERowTableNodeType.eRTNT_Tree 
local _eRTNT_Leaf = ERowTableNodeType.eRTNT_Leaf

local _GetRowTableNode = CRowTableFile.GetRowTableNode
local _Load = CRowTableFile.Load

local _GetType = CRowTableFileNode.GetType
local _GetFieldType = CRowTableFileNode.GetFieldType

local _GetString = CRowTableFileNode.GetString
local _GetNumber = CRowTableFileNode.GetNumber
local _GetValue = CRowTableFileNode.GetValue

local _GetSubNode = CRowTableFileNode.GetSubNode
local _GetSubNodeNum = CRowTableFileNode.GetSubNodeNum
local _GetSubNodeKeys = CRowTableFileNode.GetSubNodeKeys
local _HasSubNode = CRowTableFileNode.HasSubNode

local _GetFieldDataNum = CRowTableFileNode.GetFieldDataNum

local DataKeys = CRowTableNodeKeys:new()
local NodeValue = CTableFileValue:new()

function CRowTableFile.Clear()
	DataKeys = nil
	NodeValue = nil
end

function CRowTableFileNode:GetKeys(...)
	local keys = {_GetSubNodeKeys(self, DataKeys, ...)}
	DataKeys:Clear()
	return keys
end

function CRowTableFileNode:HasNode(...)
	local ret = _HasSubNode(self, ...)
	return ret
end

--[[
	对于一个row table data来说，他的call可以有如下几个返回值
	假设data名字为a
	a(1) 可能返回的是a的一个子data
	a(1,1) 可能返回的是a的子data上面的数据，也可能是a的子data的子data
	如果a(1)返回的是一个leaf data，但是这个leaf data只有0或者1个field data，那么
	我们a(1)直接返回这些数据，而不是返回leaf data了
--]]

CRowTableFileNode.__call = function(tbl, ...)
	return _GetValue(tbl, NodeValue, ...)
end

CRowTableFileNode.__newindex = function(tbl, key, value)
	error("can't set value in readonly table")
end

function LoadMulti(Language, AliasPath, ModuleName, ...)
	if Language == "" and LoadedCfgTbl[ModuleName] then
		return LoadedCfgTbl[ModuleName]
	end

	local argc = {...}
	local argn = select("#", ...)

	local function _LoadMulti()
		local TableFile = CRowTableFile:new()
		local ret = TableFile:Load(AliasPath, unpack(argc, 1, argn))
		if not ret then
			local msg = ModuleName .. " 载入出错，请检查文件"
			error(msg)
		end
	
		local RootData = _GetRowTableNode(TableFile)
		
		if Language == "" then
			_G[ModuleName] = RootData
		else
			if _G["Lan_" .. Language] == nil then
				_G["Lan_" .. Language] = {}
			end
			_G["Lan_" .. Language][ModuleName] = RootData
		end
		LoadedCfgTbl[ModuleName] = TableFile
		
		AddCheckLeakFilterObj(RootData)
	
		return RootData
	end


	if ExportClassEnabled() then
		return ExportCfgLoad(Language, ModuleName, _LoadMulti)
	else
		return _LoadMulti()
	end
end 


