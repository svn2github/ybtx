gac_require "e_protocol/HttpDownload"


--客户端接收到服务器端传来的热更新代码、文件名
function Gas2Gac:SendMsgToGac(Conn, gacId, filename, code)

	gac_id = gacId
	local tbl_gac_required_file = package["loaded"]
	if tbl_gac_required_file[filename] ~= nil then
		LoadCode(filename, code)
	else
		if tbl_update_code[filename] ~= nil then
			table.insert(tbl_update_code[filename], code)
		else
			tbl_update_code[filename] = {}
			table.insert(tbl_update_code[filename], code)
		end
	end
	
end

--客户端收到服务器发来的消息，就发送gac_id给服务端，来查询大于gac_id的热更新记录
function Gas2Gac:NotifyGacReply(Conn)
	Gac2Gas:SendIdToGas(g_Conn, gac_id)
end

--[[以下是下载热更新patch，从文件更新，暂时不用
--下载热更新patch
function GacUpdatePatch()
	local filename = "patchlist.log"

	local http = HttpDownload:new()
	host = "10.10.43.191"
	port = "80"
	url = "http://10.10.43.191/patch_list.php"
	params = {}
	
	local function callback(msg)
		tbl_patch_list = GetPatchList()
		DownAllPatch(tbl_patch_list)	
		
	end
	http:SendData(host, port, url, params, callback, filename)
end

function DownAllPatch(tbl_patch_list)
	co = coroutine.create(
	function()
		for k,v in pairs(tbl_patch_list) do
			DownPatch(v)
			coroutine.yield()
			if k == table.getn(tbl_patch_list) then
				dealpackage()
			end
		end
	end)
	coroutine.resume(co)
end

function DownPatch(version)
	print("download " .. version .. "...")
	local filename = "../../download/" .. version .. ".pkg"
	
	local http = HttpDownload:new()
	host = "10.10.43.191"
	port = "80"
	url = "http://10.10.43.191/patchs/" .. version .. ".pkg" 
	params = {}
	
	local function callback(ret)
		msg = "Down " .. version .. ".pkg over"
		print(msg)
		coroutine.resume(co)
	end
	http:SendData(host, port, url, params, callback, filename)
end



--获取补丁包版本号，暂定是svn版本号
function GetPatchList()
	local tbl_patch_list = {}
	--读文件
	local filename = g_RootPath .. "var/gac/patchlist.log"
	local fp = CLuaIO_Open(filename, "r")
	if fp ~= nil then
		local line = fp:ReadLine()
		while line do
			local b,e = string.find(line, "%w+\.pkg")
			if b then
				local patch = string.sub(line, b, b+5)
				table.insert(tbl_patch_list, 1, patch)
			end
			line = fp:ReadLine()
		end
		fp:Close()
	end

	local version_client = GetVersion()
	local version_patch
	local flag = 0
	for k,v in pairs(tbl_patch_list) do
		version_patch = tonumber(v)
		if version_patch <= version_client then
			flag = k
		end
	end
	
	for i=1,flag do
		table.remove(tbl_patch_list, 1)
	end
	
	for k,v in pairs(tbl_patch_list) do
		print(k,v)
	end
	
	return tbl_patch_list
end

--处理pkg文件
function dealpackage()
	print("down all over and begin unpack")
	do_file("666666", "a.lua")
end

--解包出pkg下的file文件，并用dofile载入该文件
function do_file(pkg, file)
	local b,e = string.find(g_RootPath, ":")
	if b then
		local disk = string.sub(g_RootPath, b-1, b)
		local path = string.sub(g_RootPath, e+2)
		local cd_disk = disk .. "&& cd\\\ "  --进入游戏所在盘的根目录
		local cd_path = "&& cd " .. path .. "download && "  --进入download目录
		local extractpkg = g_RootPath .. "bin/Release/ExtractPackage.exe " .. pkg .."/" .. file
		local cmd = cd_disk .. cd_path .. extractpkg
		print(cmd)
		os.execute(cd_disk .. cd_path .. extractpkg)	
		dofile(g_RootPath .. "download/" .. file)
	end
end

--客户端启动时进行更新
function UpdateWhenGacStart()
	--print("客户端启动时自动去下载新的补丁包")
	GacUpdatePatch()
end

--获取当前客户端版本
function GetVersion()
	--当前的svn版本号
	--return GetCurrentRevision()
	return 555555
	
	--补丁包号（如0.4.22.2）
	--return 0.4.22.2 
end
--]]

