gac_gas_require "framework/rpc/ConnMgrHandlerInc"

function CConnMgrHandler:OnDataReceived(pConn)	
	self.RegistUnPack:UnPack(pConn)
end

function CConnMgrHandler:RegistCall( AliasPath, szFileName,szClassName )
	local ObjCall = CCallRegist:new()
	local szTableName = ObjCall:Load(AliasPath, szFileName,szClassName)
end

function CConnMgrHandler:RegistDef( AliasPath, szFileName,szClassName )	
	local ObjDef = CDefRegist:new()
	local szTableName = ObjDef:Load(AliasPath, szFileName,szClassName)
	self.strName = szTableName
	self.RegistUnPack = CUnPackerTable_Inst():GetUnPacker(self.strName)
end
