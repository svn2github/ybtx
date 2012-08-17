lan_load "MsgBox_Client"
engine_require "common/Misc/TypeCheck"

local function MsgBoxAssembleArgs(MessageId,arg)
	assert(IsNumber(MessageId))
	assert(not IsNil(MsgBox_Client(MessageId)))
	local strRes
	if(next(arg) == nil) then
		strRes = MsgBox_Client(MessageId,"Content")
	else
		strRes = GetStrAccordWildcard(MsgBox_Client(MessageId,"Content"), unpack(arg))
	end
	return strRes
end

function MsgBoxMsg(MessageId,...)
	local arg = {...}
	return MsgBoxAssembleArgs(MessageId,arg)
end