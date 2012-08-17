local _IsNull = CValueTable.IsNull
local _IsNullByName = CValueTable.IsNullByName
local _GetType = CValueTable.GetType
local _GetTypeByName = CValueTable.GetTypeByName 
local _GetRowNum = CValueTable.GetRowNum
local _GetColNum = CValueTable.GetColNum

local _GetBool = CValueTable.GetBool
local _GetBoolByName = CValueTable.GetBoolByName

local _GetInt8 = CValueTable.GetInt8
local _GetInt8ByName = CValueTable.GetInt8ByName
local _GetUint8 = CValueTable.GetUint8
local _GetUint8ByName = CValueTable.GetUint8ByName

local _GetInt16 = CValueTable.GetInt16
local _GetInt16ByName = CValueTable.GetInt16ByName
local _GetUint16 = CValueTable.GetUint16
local _GetUint16ByName = CValueTable.GetUint16ByName

local _GetInt32 = CValueTable.GetInt32
local _GetInt32ByName = CValueTable.GetInt32ByName
local _GetUint32 = CValueTable.GetUint32
local _GetUint32ByName = CValueTable.GetUint32ByName

local _GetInt64 = CValueTable.GetInt64
local _GetInt64ByName = CValueTable.GetInt64ByName
local _GetUint64 = CValueTable.GetUint64
local _GetUint64ByName = CValueTable.GetUint64ByName

local _GetFloat = CValueTable.GetFloat
local _GetFloatByName = CValueTable.GetFloatByName
local _GetDouble = CValueTable.GetDouble
local _GetDoubleByName = CValueTable.GetDoubleByName

local _GetString = CValueTable.GetString
local _GetStringByName = CValueTable.GetStringByName

local ResultData = CValueTableData:new()

function CValueTable.Clear()
	ResultData = nil
end

local _GetValue = CValueTable.GetValue
local _GetValueByName = CValueTable.GetValueByName

function CValueTable:GetData(RowIndex, ColIndex)
	return _GetValue(self, ResultData, RowIndex, ColIndex)
end

function CValueTable:GetDataByName(RowIndex, ColName)
	return _GetValueByName(self, ResultData, RowIndex, ColName)
end

local _GetData = CValueTable.GetData
local _GetDataByName = CValueTable.GetDataByName


function CValueTable:GetRowSet(RowIndex)
	local Height = _GetRowNum(self)
	local Width = _GetColNum(self)
	
	assert(RowIndex > 0 and RowIndex <= Height)
	
	return function(ColIndex)
		assert(ColIndex > 0 and ColIndex <= Width)
		return _GetData(self, RowIndex-1, ColIndex-1)		
	end
end

function CValueTable:GetRowSetByName(RowIndex)
	local Height = _GetRowNum(self)
	
	assert(RowIndex > 0 and RowIndex <= Height)
	
	return function(ColName)
		return _GetDataByName(self, RowIndex-1, ColName)		
	end
end

function CValueTable:GetColSet(ColIndex)
	local Height = _GetRowNum(self)
	local Width = _GetColNum(self)
	
	assert(ColIndex > 0 and ColIndex <= Width)
	
	return function(RowIndex)
		assert(RowIndex > 0 and RowIndex <= Height)
		return _GetData(self, RowIndex-1, ColIndex-1)		
	end
end

function CValueTable:GetColSetByName(ColName)
	local Height = _GetRowNum(self)
	
	return function(RowIndex)
		assert(RowIndex > 0 and RowIndex <= Height)
		return _GetDataByName(self, RowIndex-1, ColName)		
	end
end

function CValueTable:GetTableSet()
	local Height = _GetRowNum(self)
	local Width = _GetColNum(self)
	
	return function(RowIndex, ColIndex)
		assert(RowIndex > 0 and RowIndex <= Height and ColIndex > 0 and ColIndex <= Width)
		return _GetData(self, RowIndex - 1, ColIndex - 1)
	end
end

function CValueTable:GetTableSetByName()
	local Height = _GetRowNum(self)

	return function(RowIndex, ColName)
		assert(RowIndex > 0 and RowIndex <= Height)
		return _GetData(self, RowIndex - 1, ColName)
	end
end

CValueTable.__call = function(tbl, RowIndex, ColIndex)
	assert(RowIndex ~= nil or ColIndex ~= nil)
	
	if RowIndex ~= nil and ColIndex ~= nil then
		local Height = _GetRowNum(tbl)
		assert(RowIndex > 0 and RowIndex <= Height and type(RowIndex) == "number")
		if type(ColIndex) == "number" then
			local Width = _GetColNum(tbl)
			assert(ColIndex > 0 and ColIndex <= Width)
			return _GetData(tbl, RowIndex - 1, ColIndex - 1)
		elseif type(ColIndex) == "string" then
			return _GetDataByName(tbl, RowIndex - 1, ColIndex)
		else
			error("Invalid Col Type")	
		end
	elseif RowIndex ~= nil and ColIndex == nil then
		local Height = _GetRowNum(tbl)
		assert(RowIndex > 0 and RowIndex <= Height and type(RowIndex) == "number")
		return function(_ColIndex)
			if type(_ColIndex) == "number" then
				local Width = _GetColNum(tbl)
				assert(_ColIndex > 0 and _ColIndex <= Width)
				return _GetData(tbl, RowIndex - 1, _ColIndex - 1)
			elseif type(_ColIndex) == "string" then
				return _GetDataByName(tbl, RowIndex - 1, _ColIndex)
			else
				error("Invalid Col Type")	
			end
		end
	else
		if type(ColIndex) == "number" then
			local Width = _GetColNum(tbl)
			assert(ColIndex > 0 and ColIndex <= Width)
			return function(_RowIndex)
				local Height = _GetRowNum(tbl)
				assert(_RowIndex > 0 and _RowIndex <= Height and type(_RowIndex) == "number")			
				return _GetData(tbl, _RowIndex - 1, ColIndex - 1)
			end
		elseif type(ColIndex) == "string" then
			return function(_RowIndex)
				local Height = _GetRowNum(tbl)
				assert(_RowIndex > 0 and _RowIndex <= Height and type(_RowIndex) == "number")			
				return _GetDataByName(tbl, _RowIndex - 1, ColIndex)
			end
		else
			error("Invalid Col Type")
		end		
	end
	
end

------------------------------------------------------------
function CValueTable:ToTable()
	local mt = {
		__call = function(tbl, Row, Col)
			local RowSet = rawget(tbl, Row)
			local Data = rawget(RowSet, Col)
			return Data
		end,
	}
	
	local Row = _GetRowNum(self)
	local Col = _GetColNum(self)

	local Tbl = {}

	function mt:GetRowNum()
		return Row
	end
	
	function mt:GetColNum()
		return Col
	end
	
	function mt:GetData(Row, Col)
		return Tbl[Row][Col]		
	end

	mt.__index = function(tbl, key)
		return rawget(mt, key)
	end
	
	
	for i = 1, Row do
		Tbl[i] = {}
		for j = 1, Col do
			Tbl[i][j] = _GetData(self, i - 1, j - 1)
		end
	end
	
	setmetatable(Tbl, mt)
	return Tbl
end

function CValueTable:ToRow(RowIndex)
	local mt = {
		__call = function(tbl, Col)
			return rawget(tbl, Col)	
		end
	}
	
	local Row = _GetRowNum(self)
	local Col = _GetColNum(self)
		
	function mt:GetColNum()
		return Col
	end

	mt.__index = function(tbl, key)
		return rawget(mt, key)
	end

	assert(RowIndex > 0 and RowIndex <= Row)
	
	local Tbl = {}
	for i = 1, Col do
		Tbl[i] = _GetData(self, RowIndex - 1, i - 1)
	end
	
	setmetatable(Tbl, mt)
	return Tbl
end

function CValueTable:ToCol(ColIndex)
	local mt = {
		__call = function(tbl, Row)
			return rawget(tbl, Row)	
		end
	}
	
	local Row = _GetRowNum(self)
	local Col = _GetColNum(self)
	
	function mt:GetRowNum()
		return Row
	end

	mt.__index = function(tbl, key)
		return rawget(mt, key)
	end
	
	assert(ColIndex > 0 and ColIndex <= Col)
	
	local Tbl = {}
	for i = 1, Row do
		Tbl[i] = _GetData(self, i - 1, ColIndex - 1)
	end
	
	setmetatable(Tbl, mt)
	return Tbl
end
