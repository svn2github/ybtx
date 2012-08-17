engine_require "common/Module/Module"

local SandBoxMt = GetModuleMt(true)
SandBoxMt.IsNumber = IsNumber
SandBoxMt.IsString = IsString
SandBoxMt.IsTable = IsTable
SandBoxMt.RegisterTick 		= RegisterTick
SandBoxMt.RegClassTick = RegClassTick
SandBoxMt.UnRegisterTick 	= UnRegisterTick
SandBoxMt.CPos 				= CPos
SandBoxMt.CFPos 				= CFPos
SandBoxMt.Gas2Gac 			= Gas2Gac
SandBoxMt.RequireSandBox = RequireSandBox

function CreateSandBox(ModuleName)
	local ret = Module(ModuleName, SandBoxMt, 3)
	return ret
end

function RequireSandBox(FileName)
	return gas_require(FileName)
end

AddCheckLeakFilterObj(CreateSandBox)
AddCheckLeakFilterObj(RequireSandBox)