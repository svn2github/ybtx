function ReadByteNum(acr)
	return acr:Read("c")
end

function ReadUnByteNum(acr)
	return acr:Read("C")
end

function ReadShortNum(acr)
	local num = acr:Read("h")
	return ChangeByteOrder_int16(num)
end

function ReadUnShortNum(acr)
	local num = acr:Read("H")
	return ChangeByteOrder_uint16(num)
end

function ReadIntNum(acr)
	local num = acr:Read("i")
	return ChangeByteOrder_int32(num)
end

function ReadUnIntNum(acr)
	local num = acr:Read("I")
	return ChangeByteOrder_uint32(num)
end

function ReadLongLongNum(acr)
	local num = acr:Read("z")
	return ChangeByteOrder_uint64(num)
end

function WriteByteNum(acr, byte)
	acr:Write("c", byte)
end

function WriteUnByteNum(acr, byte)
	acr:Write("C", byte)
end

function WriteShortNum(acr, num)
	acr:Write("h", ChangeByteOrder_int32(num))
end

function WriteUnShortNum(acr, num)
	acr:Write("H", ChangeByteOrder_uint16(num))
end

function WriteIntNum(acr, num)
	acr:Write("i", ChangeByteOrder_int32(num))
end

function WriteUnIntNum(acr, num)
	acr:Write("I", ChangeByteOrder_uint32(num))
end

function WriteLongLongNum(acr, num)
	acr:Write("z", ChangeByteOrder_uint64(num))
end

function _write_size_str_to_buf(buf, str, size)
	local len = string.len(str)
	if size < len then
		return false
	end
	buf:Write("s", str)
	buf:Seek(-1)
	for i = 1, size - len do
		buf:Write("C", 0)
	end
	return true
end

function _read_size_str_from_buf(buf,size)
	local str = ""
	for i = 1, size do
		str = str .. buf:Read("C")
	end
	return str
end

local function unescape(s)
	s = string.gsub(s, "+", " ")
	s = string.gsub(s, "%%(%x%x)", function (h)
			local c = string.char(tonumber(h, 16))
			return c
		end)
	return s
end

function decode_url(tbl, str)
	for name, value in string.gmatch(str, "([^&=]+)=([^&=]*)") do
		name = unescape(name)
		value = unescape(value)
		tbl[name] = value
	end
end

local function escape(s)
	s = string.gsub(s, "[&=+%%%c]", function(c)
			return string.format("%%%02X", string.byte(c))
		end)
	s = string.gsub(s, " ", "+")
	return s
end

function encode_url(t)
	local b = {}
	for k,v in pairs(t) do
		b[#b+1] = (escape(k) .. "=" .. escape(v))
	end
	return table.concat(b, "&")
end
