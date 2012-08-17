CWebServerHandler = class(IPipeHandler)
CWebServerProcessorHandler = class(IPipeHandler)
g_WebFuncDef = {}
local function test(tbl)
	return true, [[<html><title>test</title><head></head><body>hello, jiaben's game server<br/></body></html>]]
end
g_WebFuncDef["test"]=test

