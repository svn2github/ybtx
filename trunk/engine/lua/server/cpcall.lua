		
function cpcall( conn, func, ... )
	local arg = {...}
	local argc = select("#", ...)
	local function err_handler(msg)
		local ErrCode = LogErr(msg, conn)
		if ErrCode == 0 then
			db()
		end
	end
	return xpcall(	function() return func( unpack(arg, 1, argc) ) end, err_handler )
end
		