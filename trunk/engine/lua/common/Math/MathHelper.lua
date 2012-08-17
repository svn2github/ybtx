function MAKEWORD( a, b )	return uint8(a)		+ uint8(b)*256		end
function MAKELONG( a, b )	return uint16(a)	+ uint16(b)*65536	end
function LOWORD( l )		return uint16(l)						end
function HIWORD( l )		return uint32(l)/65536					end
function LOBYTE( w )		return uint8(w)							end
function HIBYTE( w )		return uint16(w)/256					end


