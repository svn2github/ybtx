
function OnHandleDbCallback(ret, callback, errfunc, ...)	
	if ret then
		callback(...)
	else
		errfunc()
	end
end