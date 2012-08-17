local function _ReloadFile(file_name,func)
	local szLowerFile = string.lower( file_name )
	local i,j = string.find( szLowerFile,"%w*inc$" )
	if i ~= nil then
		print("Reload file include \"inc\". ")
		return false
	end
	
	package.loaded[file_name] = nil
		
	local ret, msg = pcall(func, file_name)
	if not ret then
		print(msg)
	end
	return ret
end

function ReloadFile(file_name)
	local ext_pos = string.find(file_name, ".lua", 1, true)
	if not ext_pos then
		print(file_name .. "不是lua文件，不能载入")
		return
	end

	local name = string.sub(file_name, 1, ext_pos - 1)

	--载入的是一个lua文件，因为我们有些lua是另一个虚拟机的，所以不能载入
	--所以我们需要先判断是否在这个vm里面被载入了
	local loaded = package.loaded
	for k, v in pairs(loaded) do
		local pos = string.find(name, k)
		if pos then
			local sub_name = string.sub(name, pos)
			if sub_name == k then
					_ReloadFile(sub_name, require)
					return
			end
		end
	end
	
	
	--如果都不存在，那么我们不需要载入这个文件，因为这个文件可能是另一个lua虚拟机的
end
