gac_gas_require "relation/tong/TongMantleMgrInc"
cfg_load "tong/TongMantle_Common"

function CTongMantleInfoMgr:Ctor()
	self.tblItemInfo = TongMantle_Common
end

function CTongMantleInfoMgr:GetTable()
	return self.tblItemInfo
end

function CTongMantleInfoMgr:GetTableOrderByLevel()
	local tblMantleInfo = {}
	local tbl = self.tblItemInfo:GetKeys()
	for i, p in pairs(tbl) do
		local MantleName = p
		local Mantlei = self.tblItemInfo(p)
		table.insert(tblMantleInfo,Mantlei )
	end
	table.sort( tblMantleInfo, function(a, b) return a("MantleLevel") < b("MantleLevel") end )
	return tblMantleInfo
end

function CTongMantleInfoMgr:GetMantleInfo(sName)
	return self.tblItemInfo(sName)
end