
local loadstring = loadstring
local pcall = pcall
local tostring = tostring
local GetCurrentRevision = GetCurrentRevision

function GetGlobalId()
	return db_id
end
local GetGlobalId = GetGlobalId

function UpdateForDb(tbl_db, dbId)
	--局部tbl_db存放要热更新的filename和code，如果该文件已载入，更新其代码，然后从表中删除；剩下的即为还没载入的文件的热更新内容
	db_id = dbId
	local tbl_db_required_file = package["loaded"]

	for k,v in pairs(tbl_db) do
		if tbl_db_required_file[k] ~= nil then
			for _,code in pairs(v) do
				LoadCode(k, code)
			end
			tbl_db[k] = nil
		end
	end

	--将还未载入的热更新内容记入全局表tbl_update_code，便于在RequireFile的时候进行比较和查看
	for k,v in pairs(tbl_db) do
		if tbl_update_code[k] ~= nil then
			for _,code in pairs(v) do
				table.insert(tbl_update_code[k], code)
			end
		else
			tbl_update_code[k] = tbl_db[k]
		end
	end
		
end
local UpdateForDb = UpdateForDb

local UpdateDB = CreateDbBox(...)
local StmtOperater = {}	


local StmtDef = {
    "_GetHotUpdateGas",
    [[
        select hug_id, hug_filename, hug_code from tbl_hot_update_gas where hug_revision=? and hug_id > ? order by hug_id asc; 
    ]]
}   
DefineSql(StmtDef,StmtOperater)

local StmtDef = {
    "_GetHotUpdateGac",
    [[
        select hug_id, hug_filename, hug_code from tbl_hot_update_gac where hug_revision=? and hug_id > ? order by hug_id asc; 
    ]]
}   
DefineSql(StmtDef,StmtOperater)

local StmtDef = {
    "_GetHotUpdateDb",
    [[
        select hud_id, hud_filename, hud_code from tbl_hot_update_db where hud_revision=? and hud_id > ? order by hud_id asc; 
    ]]
}   
DefineSql(StmtDef,StmtOperater)

--查询数据库中的db热更新
function UpdateDB.GetHotUpdateDb(data)

	local revision = GetCurrentRevision()
	local dbId = GetGlobalId()
	local tbl_db  = StmtOperater._GetHotUpdateDb:ExecSql("", revision, dbId)
	local tbl_file_code_db = {}
	
	local row = tbl_db:GetRowNum()
	if row > 0 then
		dbId = tbl_db:GetData(row-1, 0)
	end
	for i=0,row-1 do
		local filename = tbl_db:GetData(i, 1)
		local code     = tbl_db:GetData(i, 2)
		if tbl_file_code_db[filename] ~= nil then
			table.insert(tbl_file_code_db[filename], code)
		else
			tbl_file_code_db[filename] = {}
			table.insert(tbl_file_code_db[filename], code)
		end
	end
		
	--对db线程热更新
	UpdateForDb(tbl_file_code_db, dbId)	

end

--查询数据库中的gas/gac热更新
function UpdateDB.GetHotUpdateGasGac(data)
	local revision = GetCurrentRevision()
	local gasId = data["gas_id"]
	local gacId = data["gac_id"]
 	local tbl_gas = StmtOperater._GetHotUpdateGas:ExecSql("", revision, gasId)
 	local tbl_gac = StmtOperater._GetHotUpdateGac:ExecSql("", revision, gacId)
	local tbl_file_code_gas = {}
	local tbl_file_code_gac = {}

	local row = tbl_gas:GetRowNum()
	if row > 0 then
		gasId = tbl_gas:GetData(row-1, 0)
	end
	for i=0,row-1 do
		local filename = tbl_gas:GetData(i, 1)
		local code     = tbl_gas:GetData(i, 2)

		if tbl_file_code_gas[filename] ~= nil then
			table.insert(tbl_file_code_gas[filename], code)
		else
			tbl_file_code_gas[filename] = {}
			table.insert(tbl_file_code_gas[filename], code)
		end
	end

	local row = tbl_gac:GetRowNum()
	if row > 0 then
		gacId = tbl_gac:GetData(row-1, 0)
	end

	for i=0,row-1 do
		local id       = tbl_gac:GetData(i, 0)
		local filename = tbl_gac:GetData(i, 1)
		local code     = tbl_gac:GetData(i, 2)

		tbl_file_code_gac[id] = {filename, code}
		--tbl_file_code_gac = {1={filename1,code1}, 2={filename2,code2}...}
	end
	

	return tbl_file_code_gas, tbl_file_code_gac, gasId, gacId
end


return UpdateDB
