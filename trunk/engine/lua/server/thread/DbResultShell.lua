
CDbResultShell = class()

function CDbResultShell:Ctor(DbResultSet)
	self.DbSet = DbResultSet
	self.ExcessData = {}  --索引从1开始
	self.MaxCol = 0
end

function CDbResultShell.__call(tbl, rowIndex_1, colIndex_1)
	if tbl.ExcessData[rowIndex_1] and tbl.ExcessData[rowIndex_1][colIndex_1] then
		return tbl.ExcessData[rowIndex_1][colIndex_1]
	end
	if tbl.DbSet then
		return tbl.DbSet(rowIndex_1, colIndex_1)
	end
end

function CDbResultShell:SetData(rowIndex_0, colIndex_0, value)
	assert(rowIndex_0 < self:GetRowNum() and colIndex_0 < self:GetColNum())
	local rowIndex_1, colIndex_1 = rowIndex_0 + 1, colIndex_0 + 1
	if not self.ExcessData[rowIndex_1] then
		self.ExcessData[rowIndex_1] = {}
	end
	self.ExcessData[rowIndex_1][colIndex_1]=value
end

function CDbResultShell:GetData(rowIndex_0, colIndex_0)
	self(rowIndex_0 + 1, colIndex_0 + 1)
end

function CDbResultShell:InsertRow(row_set)
	assert(self.DbSet == nil)
	table.insert(self.ExcessData, row_set)
	if self.MaxCol < #row_set then
		self.MaxCol = #row_set
	end
end

function CDbResultShell:GetRowNum()
	if self.DbSet then
		return self.DbSet:GetRowNum()
	else
		return #self.ExcessData
	end
end

function CDbResultShell:GetColNum()
	if self.DbSet then
		return self.DbSet:GetColNum()
	else
		return self.MaxCol
	end
end

function CDbResultShell:GetRowSet(rowIndex_1)
	return function(colIndex_1)
		return self(rowIndex_1, colIndex_1)
	end
end

function CDbResultShell:GetColSet(colIndex_1)
	return function(rowIndex_1)
		return self(rowIndex_1, colIndex_1)
	end
end

function CDbResultShell:GetTableSet()
	return function(rowIndex_1, colIndex_1)
		return self(rowIndex_1, colIndex_1)
	end
end

function CreateDbResultShellByTable(table)
	local res = CDbResultShell:new()
	for i, v in ipairs(table) do
		res:InsertRow(v)
	end
	return res
end