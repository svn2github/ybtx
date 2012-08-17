CGameGridRadiusCallback = class(IGameGridRadiusCallback)

function CGameGridRadiusCallback:Ctor()
	self.m_QueryObjTbl = {}
end

function CGameGridRadiusCallback:QueryObjs(Scene, Pos, Radius)
	self:ForEachInRadius(Scene, Pos, Radius)
end

function CGameGridRadiusCallback:QueryAllPlayer(Scene, Pos, Radius)
	self:ForEachMediatorInRadius(Scene, Pos, Radius)
end

function CGameGridRadiusCallback:Exec(Entity)
	table.insert(self.m_QueryObjTbl, Entity)
end

function CGameGridRadiusCallback:QueryObjByName(ObjName, ECharacterType)
	local objtbl = self.m_QueryObjTbl
	local restbl = {}
	for i = 1, table.getn(objtbl) do
		if objtbl[i] then
			local Type = objtbl[i].m_Properties:GetType()
			local Name = objtbl[i].m_Properties:GetCharName()
			if Name == ObjName and Type == ECharacterType  then
				table.insert(restbl, objtbl[i])
			end
		end
	end
	return restbl
end

function CGameGridRadiusCallback:QueryByECharacterType(ECharacterType)
	local objtbl = self.m_QueryObjTbl
	local restbl = {}
	for i = 1, table.getn(objtbl) do
		if objtbl[i] then
			local Type = objtbl[i].m_Properties:GetType()
			if Type == ECharacterType  then
				table.insert(restbl, objtbl[i])
			end
		end
	end
	return restbl
end