Gas2Gac={}
function Gas2Gac:OnError(num, fun)
	return false
end
function Gas2Gac:ByteCall(Pipe,x,y )
	local len = Pipe:GetRecvDataSize()
--	--print(len)
--	Obj = CCorePipe:Attach( Pipe )

end
function Gas2Gac:WordCall(Pipe,x,y )
--	--print("In the WordCall x=",x,"y=",y)
end

function Gas2Gac:DWordCall(Pipe,x,y )

end

function Gas2Gac:DDWordCall(Pipe,x,y )

end

function Gas2Gac:BStringCall(Pipe,x )

end

function Gas2Gac:StringCall(Pipe,x)

end

function Gas2Gac:TableCall(Pipe,x)

end

function Gas2Gac:LString2Big(Pipe,x)

end

function Gas2Gac:BString2Big(Pipe,x)

end

function Gas2Gac:ClosePipe(Pipe,x)
	return -2
end

function Gas2Gac:CallError(Pipe)
	return 2
end

function Gas2Gac:CallNoArg(Pipe)

end

return "Gas2Gac",
{
	{"ByteCall","cc"},
	{"WordCall","hh"},
	{"DWordCall","ii"},
	{"DDWordCall","dd"},
	{"BStringCall","S"},
	{"StringCall","s"},
--	{"TableCall","{iiii}"},
	{"LString2Big","s"},
	{"BString2Big","S"},
	{"NoFoundFun","i"},
	{"ClosePipe","i"},
	{"CallError",""},
	{"CallNoArg",""}
	
}

