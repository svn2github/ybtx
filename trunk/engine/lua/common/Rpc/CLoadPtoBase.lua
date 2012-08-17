CLoadPtoBase = class()

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
