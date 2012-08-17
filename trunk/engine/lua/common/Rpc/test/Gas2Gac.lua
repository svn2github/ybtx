Gas2GacDef={}
function Gas2GacDef:OnError(num, fun)
	return false
end
function Gas2GacDef:ByteCall(Pipe,x,y )
	local len = Pipe:GetRecvDataSize()
--	--print(len)
--	Obj = CCorePipe:Attach( Pipe )

end
function Gas2GacDef:WordCall(Pipe,x,y )

end

function Gas2GacDef:DWordCall(Pipe,x,y )

end

function Gas2GacDef:DDWordCall(Pipe,x,y )

end

function Gas2GacDef:BStringCall(Pipe,x )

end

function Gas2GacDef:StringCall(Pipe,x)

end

function Gas2GacDef:TableCall(Pipe,x)

end

function Gas2GacDef:LString2Big(Pipe,x)

end

function Gas2GacDef:BString2Big(Pipe,x)

end

function Gas2GacDef:CallNoArg(Pipe)

end

function Gas2GacDef:ClosePipe(Pipe,x)
	return -2
end

function Gas2GacDef:CallError(Pipe)
	return 2
end