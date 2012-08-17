lan_load "SysMovementNotifyMessge_Common"
lan_load "StaticText_Common"
lan_load "MenuText_Common"
lan_load "ToolTipsText_Common"
lan_load "message/Lan_GMMessageText_Common"
lan_load "Log_Common"
engine_require "common/Misc/TypeCheck"


local bUseStringFormat = 0
local function ReplaceArg(source,arg)
	local backup = source
	if bUseStringFormat ==1 then
		return string.format(source,unpack(arg))
	end
	local n = #arg
	local i = 1
	local beginPos
	local endPos
	local tempStr
	local destStr
	while i<=n do
		tempStr = "#arg".. i .. "%b()"
		beginPos,endPos = string.find(source,tempStr)
		if beginPos == nil then
			break
		end
		destStr = string.sub(source,beginPos,endPos)
		beginPos,endPos = string.find(destStr,"%b()")
		destStr = string.sub(destStr,beginPos+1,-2)
		if arg[i]==nil then
			assert(false,"传入通配参数为nil，字符串为：" .. backup)
		end
		destStr = string.format(destStr,arg[i])
		destStr = string.gsub(destStr,"%%","%%%%")
		source = string.gsub(source,tempStr,destStr)
		i = i + 1
	end
	source = string.gsub(source,"%%%%","%%")
	return source
end

function GetStrAccordWildcard(source,...)
	local arg = {...}
	return ReplaceArg(source,arg)
end

local function GacGetStringCommom(MessageCommom, MessageId, arg, MessageCommomName)
	assert(not IsNil(MessageCommom(MessageId)), MessageCommomName .. "中没有" .. MessageId)
	local strRes
	if(next(arg) == nil) then
		strRes = MessageCommom(MessageId,"Content")
	else
		strRes = GetStrAccordWildcard(MessageCommom(MessageId,"Content"), unpack(arg))
	end
	return strRes
end

local function GacGetStringTable(MessageCommom, MessageId, arg, MessageCommomName)
	assert(IsNumber(MessageId))
	assert(not IsNil(MessageCommom(MessageId)), MessageCommomName .. "中没有" .. MessageId)
	local tblStrRes = {}
	local strRes = MessageCommom(MessageId,"Content")
	local i = 1
	while( "" ~= string.sub(strRes, i) ) do
		local iBegin	= string.find(strRes, "{", i)
		local iEnd		= string.find(strRes, "}", i)
		if(iBegin and iEnd ) then
			local strRes = string.sub(strRes, iBegin+1, iEnd-1)
			i = iEnd + 1
			if(#arg > 0) then
				strRes = GetStrAccordWildcard(strRes, arg[1])
				table.remove(arg, 1)
			end
			table.insert(tblStrRes, strRes)
		else
			local strRes = string.sub(strRes, i)
			if(#arg > 0) then
				strRes = GetStrAccordWildcard(strRes, arg[1])
				table.remove(arg, 1)
			end
			table.insert(tblStrRes, strRes)
			break
		end
	end
	return tblStrRes
end
--------------------------------------------------

function GacSystemMovementNotifyAssembleArgs(MessageId, arg)
	local strRes = GacGetStringCommom(SysMovementNotifyMessge_Common, MessageId, arg, "SysMovementNotifyMessge_Common")
	return strRes
end

function GacStaticTextAssembleArgs(MessageId, arg)
	local strRes = GacGetStringCommom(StaticText_Common, MessageId, arg, "StaticText_Common")
	return strRes
end

function GacLogAssembleArgs(MessageId, arg)
	local strRes = GacGetStringCommom(Log_Common, MessageId, arg, "Log_Common")
	return strRes
end

function GacMenuTextAssembleArgs(MessageId, arg)
	local strRes = GacGetStringCommom(MenuText_Common, MessageId, arg, "MenuText_Common")
	return strRes
end

function GacToolTipsTextAssembleArgs(MessageId, arg)
	local strRes = GacGetStringCommom(ToolTipsText_Common, MessageId, arg, "ToolTipsText_Common")
	return strRes
end

function GacMenuText(uTextId, ...)
	local arg = {...}
	return GacMenuTextAssembleArgs(uTextId, arg)
end

function GacGMMsgClientAssembleArgs(MessageId, arg)
	local strRes = GacGetStringCommom(Lan_GMMessageText_Common, MessageId, arg, "Lan_GMMessageText_Common")
	return strRes
end
