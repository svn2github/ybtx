gac_gas_require "reload/UpdateInc"


function CUpdate:Reload( szFile )
	local RetString = "Reload Successful"
	local RetFlag = true
	local szLowerFile = string.lower( szFile )
	local i,j = string.find( szLowerFile,"%w*inc$" )
	local k,l = string.find( szLowerFile,"%w*inc.lua$" )
	if i ~= nil or k ~= nil then
		RetFlag = false
		RetString = "Reload file include \"inc\". "
	else
		local loadfun, error = LoadFile( szFile )	
		if loadfun then
			local rte, ret = pcall( loadfun )
			if ( false == rte ) then
				RetFlag = false
				RetString = ret
			end
		else
			RetString = error
			RetFlag = false
		end
	end
	return RetFlag,RetString
end
