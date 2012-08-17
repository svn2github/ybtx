CDefRegist = class(CLoadPtoBase)

function CDefRegist:Check(name)
	local PtoName = name
	local DefTable = _G[PtoName]
	local ErrorBefore
	ErrorBefore = "Table :" .. PtoName .. " Check error ¡£"
	if type(DefTable)~="table" then
		local err = string.format("can't found Def table:%s",PtoName)
		error(ErrorBefore .. err)
	end;
	if type(DefTable["OnError"]) ~= "function" then
		local err = string.format("can't found OnError in Def table:%s",PtoName)
		error(ErrorBefore .. err)
	end;
end

function CDefRegist:Regist(hHandle,func,ID,arg)
	local RegistUnPack = CUnPackerTable_Inst()
	RegistUnPack:RegistUnPackFunc(hHandle,func,ID,arg)
end

function CDefRegist:RegistTable(name)
	local UnPackerTable = CUnPackerTable_Inst()
	return UnPackerTable:Init(name)
end
