Remote = {}
Remote.ServerMS = {}
Remote.ServerCB = {}
Remote.ClientMS = {}
Remote.g_App = {}

function Remote._GetValues( Vars )

	local RetString = ""
	local i = 0
	
	while true do
	
		local j = i + 1
		i = string.find( Vars, ";", j)
		
		if ( nil == i ) then break end
		
		local parVar	= string.sub( Vars, j, i - 1 )
		
		if ( parVar == "" ) then 
			RetString = RetString.."F|Empty Script^^"
		else		
			local t			= "return "..parVar
			local loadfun	= loadstring( t )	
			
			if loadfun then
				local rte, ret	= pcall( loadfun )
			
				if ( false == rte ) then
					RetString = RetString.."F|RunTime Error --> "..ret.."^^"
				else
					local retType = type(ret)
					if ret then
						if ( "string" == retType ) then
							RetString = RetString.."T|"..ret.."^^"
						elseif ( "number" == retType ) then
							RetString = RetString.."T|"..tostring(ret).."^^"
						else
							RetString = RetString.."F|Return Type Error --> "..retType.."^^"
						end
					else
						RetString = RetString.."T|nil^^"
					end
				end
			else
				RetString = RetString.."F|Script Error --> "..parVar.."^^"
			end		
		end
	end
	
	return RetString
end

function Remote._GetValue( Vars )

	local RetString = ""
	local t			= "return "..Vars
	local loadfun	= loadstring( t )	
	
	if loadfun then
		
		local rte, ret = pcall( loadfun )
		
		if ( false == rte ) then
			RetString = "e^^RunTime Error --> "..ret
		else
			local retType = type(ret)
			if ret then
				if ( "string" == retType ) then
					RetString = ret.."^^T"
				elseif ( "number" == retType ) then
					RetString = tostring(ret).."^^T"
				else
					RetString = "e^^Return Type Error --> "..retType
				end
			else
				RetString = "nil^^T"
			end
		end
	else
		RetString = "e^^Script Error --> "..Vars
	end		
	return RetString
end

function Remote._GetType( Vars )

	local RetString = ""

	local t = "return "..Vars
	local loadfun = loadstring( t )	
	
	if loadfun then
		local rte, ret = pcall( loadfun )
				
		if ( false == rte ) then
			RetString = "e^^RunTime Error --> "..ret
		else
			local retType = type(ret)
			if ret then
				RetString = retType.."^^T"				
			else
				RetString = "nil^^T"
			end
		end
	else
		RetString = "e^^Script Error --> "..Vars
	end		
		
	return RetString
end

function Remote._RunScript( Pipe, Script )
	local RetString = ""
	local _Script = "return function( Pipe ) \n"..Script .."\nend"
	local loadfun = loadstring( _Script )	

	if loadfun then
		local rte, ret = pcall( loadfun(), Pipe )
		
		if ( false == rte ) then
			local ret = tostring( ret)
			RetString = "e^^RunTime Error --> "..(ret or "No Discription")
		else
			if ( ret == nil ) then ret = "Done with no return value" end
			local ret = tostring( ret)
			RetString = (ret or "Done").."^^T"
		end	
	else
		RetString = "e^^Script Error --> \n"..Script
	end		

	return RetString
end


function Remote.ServerMS:GetValues( Connection, Vars )

	if ( nil == Vars or Vars == "") then
		return nil, "Empty Script"
	end
	
	local retTable = {}
	
	Gac2Gas:Remote_MS_GetValues( Connection, Vars )
	Remote.co = coroutine.running()
	local ret = coroutine.yield()
	
	local i = -1
	while true do
		local j = i + 2
		i = string.find( ret, "%^%^", j)
		if ( nil == i ) then break end
		local parVar = string.sub( ret, j, i - 1 )
		table.insert( retTable, parVar )
	end
	return unpack( retTable )	
end

function Remote.ServerMS:GetValue( Connection, Vars )

	if ( nil == Vars or Vars == "") then
		return nil, "Empty Script"
	end
	
	Gac2Gas:Remote_MS_GetValue( Connection, Vars )
	Remote.co = coroutine.running()
	local ret = coroutine.yield()
	
	local i, j = string.find( ret, "%^%^", 1)
	
	local l = string.sub( ret, 1, i-1 )
	local r = string.sub( ret, j + 1)
	
	if ( "T" == r ) then
		return l, nil
	else
		return nil, r
	end

end

function Remote.ClientMS:GetValue( Connection, Vars )

	if ( nil == Vars or Vars == "") then
		return nil, "Empty Script"
	end
	
	Gas2Gac:Remote_MS_GetValue( Connection, Vars )
	Remote.co = coroutine.running()
	local ret = coroutine.yield()
	
	local i, j = string.find( ret, "%^%^", 1)
	
	local l = string.sub( ret, 1, i-1 )
	local r = string.sub( ret, j + 1)
	
	if ( "T" == r ) then
		return l, nil
	else
		return nil, r
	end

end

function Remote.ServerMS:GetType( Connection, Vars )

	if ( nil == Vars or Vars == "") then
		return nil, "Empty Script"
	end
	
	Gac2Gas:Remote_MS_GetType( Connection, Vars )
	Remote.co = coroutine.running()
	local ret = coroutine.yield()
	
	local i, j = string.find( ret, "%^%^", 1)
	
	local l = string.sub( ret, 1, i-1 )
	local r = string.sub( ret, j + 1)
	
	if ( "T" == r ) then
		return l, nil
	else
		return nil, r
	end
end

function Remote.ServerCB:GetType( Connection, Vars )

	if ( nil == Vars or Vars == "") then
		return 
	end

	Gac2Gas:Remote_CB_GetType( Connection, Vars )
end

function Remote.ServerCB:GetValue( Connection, Vars )

	if ( nil == Vars or Vars == "") then
		return
	end
	
	Gac2Gas:Remote_CB_GetValue( Connection, Vars )
end