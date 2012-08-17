--因为对象（Player,Npc，obj，trap）在lua面调用删除函数后，逻辑代码都是认为该对象已经不存在，
--但是在C++里面该对象任然没有被删除，所以所有lua代码判断对象是否合法都得调用下面的函数进行判断
--该函数只能使用在服务端对象身上，因为客户端对象是同步删除的

function IsServerObjValid(Obj)
	if Obj and IsCppBound(Obj) and Obj:GetObjValidState() then
		return true
	end
	return false
end

--由名称索引是否是召唤兽类型
function IsServantType(NpcTypeName)
	local eNpcType = NpcInfoMgr_GetTypeByTypeName(NpcTypeName)
	return NpcInfoMgr_BeServantType(eNpcType)
end