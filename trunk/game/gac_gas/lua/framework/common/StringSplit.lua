--分割字符串
function SplitString(str, splitChar, tblResult, plain)
	assert(string.len(splitChar) == 1 or (string.len(splitChar) == 2 and string.sub(splitChar,1,1)=='%'))
	local i = 0
	local j = 0
	while true do
		j = i
		i = string.find(str, splitChar, i+1, plain)
		if i == nil then  break end
		assert( i ~= 1, "被分割字符：【" .. str .. "】, 分割标识：【" .. splitChar .. "】")
		table.insert(tblResult, string.sub(str, j+1 ,i-1))
	end
	table.insert(tblResult, string.sub(str, j+1, -1))
end

function SplitStringEx(str, splitChar, tblResult, plain)
	assert(string.len(splitChar) == 1 or (string.len(splitChar) == 2 and string.sub(splitChar,1,1)=='%'))
	local a = 1
	local i = 0
	local j = 0
	while true do
		j = i
		i = string.find(str, splitChar, i+1, plain)
		if i == nil then  break end
		assert( i ~= 1, "被分割字符：【" .. str .. "】, 分割标识：【" .. splitChar .. "】")
		tblResult[string.sub(str, j+1 ,i-1)] = a
		a = a + 1
	end
	tblResult[string.sub(str, j+1, -1)] = a
end