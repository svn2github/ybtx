local _IsNull = CDbResultSet.IsNull
local _IsNullByName = CDbResultSet.IsNullByName

local _GetType = CDbResultSet.GetType
local _GetTypeByName = CDbResultSet.GetTypeByName 

local _GetRowNum = CDbResultSet.GetRowNum
local _GetColNum = CDbResultSet.GetColNum

local _GetInt8 = CDbResultSet.GetInt8
local _GetInt8ByName = CDbResultSet.GetInt8ByName
local _GetUint8 = CDbResultSet.GetUint8
local _GetUint8ByName = CDbResultSet.GetUint8ByName

local _GetInt16 = CDbResultSet.GetInt16
local _GetInt16ByName = CDbResultSet.GetInt16ByName
local _GetUint16 = CDbResultSet.GetUint16
local _GetUint16ByName = CDbResultSet.GetUint16ByName

local _GetInt32 = CDbResultSet.GetInt32
local _GetInt32ByName = CDbResultSet.GetInt32ByName
local _GetUint32 = CDbResultSet.GetUint32
local _GetUint32ByName = CDbResultSet.GetUint32ByName

local _GetInt64 = CDbResultSet.GetInt64
local _GetInt64ByName = CDbResultSet.GetInt64ByName
local _GetUint64 = CDbResultSet.GetUint64
local _GetUint64ByName = CDbResultSet.GetUint64ByName

local _GetFloat = CDbResultSet.GetFloat
local _GetFloatByName = CDbResultSet.GetFloatByName
local _GetDouble = CDbResultSet.GetDouble
local _GetDoubleByName = CDbResultSet.GetDoubleByName

local _GetString = CDbResultSet.GetString
local _GetStringByName = CDbResultSet.GetStringByName

local ResultData = CValueTableData:new()

local _GetValue = CDbResultSet.GetValue
local _GetValueByName = CDbResultSet.GetValueByName

function CDbResultSet.Clear()
	ResultData = nil
end

function CDbResultSet:GetData(RowIndex, ColIndex)
	return _GetValue(self, ResultData, RowIndex, ColIndex)
end

function CDbResultSet:GetDataByName(RowIndex, ColName)
	return _GetValueByName(self, ResultData, RowIndex, ColName)
end

local _GetData = CDbResultSet.GetData
local _GetDataByName = CDbResultSet.GetDataByName


function CDbResultSet:GetNumber(RowIndex, ColIndex)
	return _GetData(self, RowIndex, ColIndex)
end

function CDbResultSet:GetNumberByName(RowIndex, ColIndex)
	return _GetDataByName(self, RowIndex, ColIndex)
end

function CDbResultSet:GetRowSet(RowIndex)
	local Height = _GetRowNum(self)
	local Width = _GetColNum(self)
	
	assert(RowIndex > 0 and RowIndex <= Height)
	
	return function(ColIndex)
		assert(ColIndex > 0 and ColIndex <= Width)
		return _GetData(self, RowIndex-1, ColIndex-1)		
	end
end

function CDbResultSet:GetRowSetByName(RowIndex)
	local Height = _GetRowNum(self)
	
	assert(RowIndex > 0 and RowIndex <= Height)
	
	return function(ColName)
		return _GetDataByName(self, RowIndex-1, ColName)		
	end
end

function CDbResultSet:GetColSet(ColIndex)
	local Height = _GetRowNum(self)
	local Width = _GetColNum(self)
	
	assert(ColIndex > 0 and ColIndex <= Width)
	
	return function(RowIndex)
		assert(RowIndex > 0 and RowIndex <= Height)
		return _GetData(self, RowIndex-1, ColIndex-1)		
	end
end

function CDbResultSet:GetColSetByName(ColName)
	local Height = _GetRowNum(self)
	
	return function(RowIndex)
		assert(RowIndex >= 0 and RowIndex < Height)
		return _GetDataByName(self, RowIndex, ColName)		
	end
end

function CDbResultSet:GetTableSet()
	local Height = _GetRowNum(self)
	local Width = _GetColNum(self)
	
	return function(RowIndex, ColIndex)
		assert(RowIndex > 0 and RowIndex <= Height and ColIndex > 0 and ColIndex <= Width)
		return _GetData(self, RowIndex - 1, ColIndex - 1)
	end
end

function CDbResultSet:GetTableSetByName()
	local Height = _GetRowNum(self)

	return function(RowIndex, ColName)
		assert(RowIndex > 0 and RowIndex <= Height)
		return _GetDataByName(self, RowIndex - 1, ColName)
	end
end

CDbResultSet.__call = function(tbl, RowIndex, ColIndex)
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

-------------------------------------------------
--下面这些函数极度消耗内存，尽可能不要使用了
function CDbResultSet:GetRow(row)
	local ret = {}
	local r = _GetRowNum(self)
	local c = _GetColNum(self)
	if r == 0 or c == 0 or row == nil or row >= r or row < 0 then
		return ret
	end
	for i = 1, c do
		ret[i] = _GetData(self, row, i - 1)
	end	
	return ret
end

function CDbResultSet:GetCol(col_index)
	local ret = {}
	local r = _GetRowNum(self)
	local c = _GetColNum(self)
	if r == 0 or c == 0 or col_index == nil or col_index >= c or col_index < 0 then
		return ret
	end
	for i = 1, r do
		ret[i] = _GetData(self, i - 1, col_index);
	end
	return ret
end

function CDbResultSet:ToTable(release)
	local ret = {}
	local r = _GetRowNum(self)
	local c = _GetColNum(self)
	for i = 1, r do
		ret[i] = {}
		for j = 1, c do		
			ret[i][j] = _GetData(self, i - 1, j - 1)	
		end
	end
	if release == nil or release then
		self:Release()
	end
	return ret
end
