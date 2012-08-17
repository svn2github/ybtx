CLoadPtoBase = class()
CCallRegist = class(CLoadPtoBase)
CDefRegist = class(CLoadPtoBase)

function CLoadPtoBase:Regist(name,func,arg)
	----print("Base:",name,func,arg)
end

function CLoadPtoBase:Check(name)
	return
end

function CLoadPtoBase:Load(AliasPath, FileName,sClassName)
	local name,ptos
	local ErrorBefore
	name,ptos=AliasDoFile(AliasPath, FileName)	
	ErrorBefore = AliasPath .. "Path :" .. FileName .. " Load error ¡£"
	if type(name)~="string" or type(ptos)~="table" then
		error(ErrorBefore .. "TableName is not string or Fun def is not table")
	end;
	
	hHandle = self:RegistTable(name)
	for ID,OnePto in ipairs(ptos) do 		
		if type(OnePto)~="table" then
			error(ErrorBefore .. "Every Fun def is not table")
		end;
		if type(OnePto[1])~="string" or type(OnePto[2])~="string" then
			local err = string.format('"%s" not string or "%s" not string', OnePto[1],OnePto[2])
			error(ErrorBefore .. err)
		end;
		self:Check(name)
		self:Regist(hHandle,OnePto[1],ID,"(" .. sClassName .. ")" .. OnePto[2])
	end
	return name
end


function CCallRegist:Regist(hHandle,func,ID,arg)	
	local PackerTable = CPackerTable_Inst()
	PackerTable:RegistPackFunc(hHandle,func,ID,arg)
end

function CCallRegist:RegistTable(name)
	local PackerTable = CPackerTable_Inst()
	return PackerTable:Init(name)
end

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




