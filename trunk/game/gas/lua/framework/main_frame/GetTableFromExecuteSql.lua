function GetTableByStatement(query_result)
	local ResultAll = {}
	local row = query_result:GetRowNum()
	local col = query_result:GetColNum()
	
	for i = 1, row do
		local resRow = {}
		for j=1, col do
			local isNull = query_result:IsNull(i - 1, j - 1)
			if isNull ~= true then
				local value = query_result:GetData(i - 1, j - 1)			
				resRow[j] = value	
			end
		end
		table.insert(ResultAll,resRow)
	end
	query_result:Release()
	return ResultAll
end

function GetTableByText(result)
	local ResultAll = {}
	local row = result:GetRowNum()
	local col = result:GetColNum()
	
	for i = 1, row do
		local resRow = {}
		for j=1, col do
			local value = result:GetData(i - 1, j - 1)			
			resRow[j] = value			
		end
		table.insert(ResultAll,resRow)
	end
	
	result:Release()
	return ResultAll
end