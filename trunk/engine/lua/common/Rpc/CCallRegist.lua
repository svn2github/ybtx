CCallRegist = class(CLoadPtoBase)

function CCallRegist:Regist(hHandle,func,ID,arg)	
	local PackerTable = CPackerTable_Inst()
	PackerTable:RegistPackFunc(hHandle,func,ID,arg)
end

function CCallRegist:RegistTable(name)
	local PackerTable = CPackerTable_Inst()
	return PackerTable:Init(name)
end