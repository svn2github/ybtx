gac_require "framework/gm_command/GMLanCommandList"

local TextFilterMgr = CTextFilterMgr:new()

-- 通过 Command 返回该命令的 参数描述、执行者 和 功能描述
function GMLanCommand:_GetVarsDefFromCommand( strCommand )
	strCommand = ( strCommand or "___" )
	if ( GMLanCommand.GMLanCommandList[ strCommand ] ~= nil ) then
		return unpack( GMLanCommand.GMLanCommandList[ strCommand ] )
	end
	return nil
end

-- 检验当前 "指令文本流" 是否有效
function GMLanCommand:_Available(strCmd)
	if("" == strCmd) then
		return false, "Wrong GM Command! Standard format:$CommandName(Param1, Param2, ...)"
	end
	
	local strPriCmd = strCmd
	local tbl = {}
	SplitString(strCmd, "%(", tbl)
	
	if( 2 > #tbl ) then
		return false, "Wrong GM command! Standard format:$CommandName(Param1, Param2, ...)"
	end
	
	local strCommand = string.lower( TextFilterMgr:RemoveTab1(tbl[1]) )
	strCmd = string.gsub(tbl[2], "%)", "")
	local strVarDefs, funcExecutor = GMLanCommand:_GetVarsDefFromCommand(strCommand)
	
	if(not strVarDefs) then
		return true, strPriCmd
	end
	
	local tblSIndex = {}
	local tVarTable = {}
	SplitString(strCmd, ",", tVarTable)
	for i = 1, string.len(strVarDefs) do
		if(not tVarTable[i]) then
			return false, "Can`t get parameter " .. i
		end
		tVarTable[i] = TextFilterMgr:RemoveTab1(tVarTable[i])
		if( "i" == string.sub(strVarDefs, i, i) ) then
			tVarTable[i] = tonumber(tVarTable[i])
			if(not tVarTable[i]) then 
				return false, "The type of parameter " .. i .. "is wrong"
			end
		elseif( "s" == string.sub(strVarDefs, i, i) ) then
			tVarTable[i] = string.gsub(tVarTable[i], "\"", "")
			tblSIndex[i] = true
		else
			return false, "A wrong registration of parameter"
		end
	end
	
	local bFlag, StrOrTbl = funcExecutor(tVarTable)
	if( bFlag ) then
		local strResultCommand = strCommand .. "("
		for i, v in ipairs( StrOrTbl ) do
			symbol = #(StrOrTbl) == i and ")" or ","
			v = tblSIndex[i] and "\"" .. v .. "\"" or v
			strResultCommand = strResultCommand .. v .. symbol
		end
		return true, strResultCommand
	else
		return false, StrOrTbl
	end
end

function GMLanCommand:print( Pipe, strReturnString )
	local strRS
	if( strReturnString == nil ) then 
		strRS = "Done with no return value"
	else 
		strRS = tostring( strReturnString )
	end
	
	if( Pipe) then
		Gas2Gac:Remote_ReceiveResult( Pipe, strRS.."^^T" )
	end
end

