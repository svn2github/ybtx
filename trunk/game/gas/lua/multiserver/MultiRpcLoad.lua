CMultiLoadBase = class()
CMultiCallRegist = class(CMultiLoadBase)
CMultiDefRegist = class(CMultiLoadBase)

function CMultiLoadBase:Check(name)
	return
end

function CMultiLoadBase:Load(AliasPath, FileName,sTableName, sClassName)
	--local name,ptos
	local ErrorBefore
	local ptos=AliasDoFile(AliasPath, FileName)	
	ErrorBefore = AliasPath .. "Path :" .. FileName .. " Load error ¡£"
	if type(ptos)~="table" then
		error(ErrorBefore .. "Fun def is not table")
	end;
	
	self:Check(sTableName)
			
	hHandle = self:RegistTable(sTableName)
	for ID,OnePto in ipairs(ptos) do 		
		if type(OnePto)~="table" then
			error(ErrorBefore .. "Every Fun def is not table")
		end;
		if type(OnePto[1])~="string" or type(OnePto[2])~="string" then
			local err = string.format('"%s" not string or "%s" not string', OnePto[1],OnePto[2])
			error(ErrorBefore .. err)
		end;
		self:Regist(hHandle,OnePto[1],ID,"(" .. sClassName .. ")" .. OnePto[2])
	end
end


function CMultiCallRegist:Regist(hHandle,func,ID,arg)	
	local PackerTable = CMultiPackerTable_Inst()
	PackerTable:RegistPackFunc(hHandle,func,ID,arg)
end

function CMultiCallRegist:RegistTable(name)
	local PackerTable = CMultiPackerTable_Inst()
	return PackerTable:Init(name)
end

function CMultiDefRegist:Check(name)
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

function CMultiDefRegist:Regist(hHandle,func,ID,arg)
	local RegistUnPack = CMultiUnPackerTable_Inst()
	RegistUnPack:RegistUnPackFunc(hHandle,func,ID,arg)
end

function CMultiDefRegist:RegistTable(name)
	local UnPackerTable = CMultiUnPackerTable_Inst()
	return UnPackerTable:Init(name)
end




