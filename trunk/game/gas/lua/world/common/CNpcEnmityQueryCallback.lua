CNpcEnmityQueryCallback = class(INpcEnmityQueryCallback)

function CNpcEnmityQueryCallback:Ctor()
	self.m_EnemyTbl = {}
	self.m_EnemyCount = 0
end

function CNpcEnmityQueryCallback:QueryEnemy(Obj)
	self.m_EnemyTbl = {}
	self.m_EnemyCount = 0
	self:QueryEnemyJob(Obj)
end

function CNpcEnmityQueryCallback:Exec(uEntityId, Enemy)
	self.m_EnemyCount = self.m_EnemyCount + 1
	self.m_EnemyTbl[uEntityId] = Enemy
end

function CNpcEnmityQueryCallback:GetEnemyCount()
	return self.m_EnemyCount
end

function CNpcEnmityQueryCallback:GetAllEnemyTbl()
	return self.m_EnemyTbl
end

function CNpcEnmityQueryCallback:GetPlayerEnemyTbl()
	local PlayerEnemyTbl = {}
	for Id,Obj in pairs(self.m_EnemyTbl) do 
		local Type = Obj.m_Properties:GetType()
		if Type == ECharacterType.Player then
			PlayerEnemyTbl[Id] = Obj
		end
	end
	return PlayerEnemyTbl
end