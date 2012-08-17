gas_require "fb_game/match_game/CountRecordInc"


function CCountRecord:Ctor() 
	self.CountTbl = {}
	self.ScoreTbl = {}
end

function CCountRecord:GetCount(name)
	return self.CountTbl[name] or 0
end

function CCountRecord:GetScore(name)
	return self.ScoreTbl[name] or 0
end

function CCountRecord:AddCount(name, score)
	self.CountTbl[name] = self:GetCount(name) + 1
	self.ScoreTbl[name] = self:GetScore(name) + (score or 1)
end

function CCountRecord:RevertScore(name, score)
	self.ScoreTbl[name] = self:GetScore(name) - (score or 1)
end

function CCountRecord:SetCount(name, count)
	self.CountTbl[name] = count
end