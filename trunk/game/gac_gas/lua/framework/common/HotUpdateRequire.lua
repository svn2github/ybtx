
--重新写RequierFile函数，先载入文件，然后判断该文件是否进行过热更新，若有，载入热更新代码。
local _RequireFile = RequireFile
function RequireFile(...)
	local arg = {...}
	local filename = arg[2]
	local tbl_required_file = package["loaded"]
	
	--在载入该文件前判断该文件是否已经载入
	local InTable = 0   --0:该文件未载入过; 1:该文件已经载入
	if tbl_required_file[filename] ~= nil then
		InTable = 1
	end
	local result = _RequireFile(arg[1], arg[2])
	
	--未载入，从热更新表中查找记录进行更新
	if InTable == 0 then
		CheckFileFromTable(filename)
	end
	return result
end

tbl_update_code = {} --记录gas/db/gac热更新的文件对应的代码，{ ["filename1"] = {code1, code2, ...}, ......}
tbl_gac_update = {}  --记录所有gac的热更新记录,{1={file,code}, 2={file,code}...}
gas_id = 0
gac_id = 0
db_id = 0
function CheckFileFromTable(filename)
	--判断filename是否在tbl中，是的话载入filename所对应的code，否则什么都不做

	if tbl_update_code[filename] ~= nil then
		--print("载入" .. filename  .. "时发现有更新")
		for _, code in pairs(tbl_update_code[filename]) do
			LoadCode(filename, code)
		end
		tbl_update_code[filename] = nil
	end
end

function LoadCode(filename, code)
	assert(type(code) == "string")	
	local loadfun = loadstring( code )
	local result
	if pcall(loadfun) then
			result = "成功！"
	else
			result = "失败，请查看热更新代码！"
	end
	
	local MD5Digester = CMd5Digester:new()
	MD5Digester:Begin()
	MD5Digester:Append(code, string.len(code))
	local CODE_MD5 = CreateCBufAcr( CreateCBuf(33) )
	MD5Digester:GetMD5Base16Str(CODE_MD5)
	local code_md5 = CODE_MD5:ToString()
	
	local data = {}
	data["filename"] = filename
	data["code"] = code_md5
	data["result"] = result
	local function gas()
		data["type"] = 1
		CallDbTrans("GMToolsDbActionDB", "AddHotUpdateResult", nil, data)
	end
	local function db()
		local GMWebToolsDB = RequireDbBox("GMToolsDbActionDB")
		data["type"] = 2
		GMWebToolsDB.AddHotUpdateResult(data)
	end
	local function gac()
		local type = 3
		Gac2Gas:SendHotUpdateMsg(g_Conn, type, filename, code_md5, result)
	end
	if pcall(gas) then
		print("这是服务端的更新")
	elseif pcall(db) then
		print("这是低速逻辑的更新")
	elseif pcall(gac) then
		print("这是客户端的更新")
	end
	
end	

