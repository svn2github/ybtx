
g_SendPairKeyType = {
	["number"] = true,
	["string"] = true,
}
g_SendPairValueType = {
	["number"] = true,
	["string"] = true,
	["boolean"] = true,
	["table"] = true,
	["lstring"] = true,
}

g_PairRpcDef = {}

for keyType in pairs(g_SendPairKeyType) do
	g_PairRpcDef[keyType] = {}
	for valueType in pairs(g_SendPairValueType) do
		g_PairRpcDef[keyType][valueType] = "SendPair_" .. keyType .. valueType
	end
end


ReceiveData = {}
