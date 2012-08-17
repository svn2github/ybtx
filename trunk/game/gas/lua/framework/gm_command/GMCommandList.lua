gac_gas_require "reload/Update"
gas_require "framework/gm_command/PlayerQuestGM"
gas_require "framework/gm_command/PlayerGM"
gas_require "framework/gm_command/PlayerBaseAttrGM"
gas_require "framework/gm_command/PlayerItemGM"
gas_require "framework/gm_command/SceneGM"
gas_require "framework/gm_command/ProgramToolsGM"
cfg_load "gm/GMCommand_Common"
engine_require "server/VMChannelMgr"

function GMCommand.Executor.ps( Pipe, strVars )
	 if ( Remote and Remote._GetValue ) then
			local strRet =  Remote._GetValue( strVars )
			local i, j = string.find( strRet, "%^%^", 1)
			local l = string.sub( strRet, 1, i-1 )
			local r = string.sub( strRet, j + 1)		
			
			if ( "T" == r ) then
				return strVars.." = "..l
			else
				return r
			end
	else
			return "服务器没有打开 Remote 功能"
	end
end

function GMCommand.Executor.rl( Pipe, szFile )
	local Reload = CUpdate:new()
	local RetFlag,RetMessage = Reload:Reload(szFile)
	return RetMessage
end

function GMCommand.Executor.lrl( Pipe, szFile )
--  Gas2Gac:GM--print( Pipe, "client reload 命令已发送。")
  Gas2Gac:Reload(Pipe,szFile)
	return ""
end

function GMCommand.Executor.rlst( Pipe, sFileName )
	local sCommand= "svn up " .. g_sSettingPath .. "/" .. sFileName
	os.execute(sCommand)
	GMCommand.Executor.rl( Pipe, sFileName )
end

function GMCommand.Executor.ts( Pipe, s)
	return "无输入参数GM指令 OK"
end

function GMCommand.Executor.fn1( Pipe, w )
	return "fn1 print "..tostring(w)
end

function GMCommand.Executor.fn2( Pipe, a, b )
	return "fn2 print "..tostring(a).." and "..tostring(b)
end

function GMCommand.Executor.fn4( Pipe, a, b, c, d )
	return "fn4 print "..tostring(a).." , "..tostring(b).." , "..tostring(c).." and "..tostring(d)
end

function GMCommand.Executor.StartGmCmd(Pipe)
	Pipe.m_Account.m_bDBTransBlock = true
	CallAccountAutoTrans(Pipe.m_Account,"GMDB", "StartGmAction", nil, {})
end

function GMCommand.Executor.EndGmCmd(Pipe)
	local function callback()
		Pipe.m_Account.m_bDBTransBlock = false
	end
	CallAccountAutoTrans(Pipe.m_Account,"GMDB", "EndGmAction", callback, {})
end

function GMCommand.Executor.ServerLuaGC(Pipe)
	collectgarbage("collect")
	return ""
end

function GMCommand.Executor.PrintTickInfo( Connection )
	g_App:PrintRegisterTickInfo()
end

function GMCommand.Executor.PrintMemInfo( Connection )
	if g_MemLog then
		local msg = GetAllMemInfo()
		g_MemLog:Write(msg)	
	end

	WatchLeakage_PrintMemInfo()
end

function GMCommand.Executor.ShowDbTranList( Connection )
	local name = "DbTransList_"..GetCurrentPID()--os.time()
	local strpath = CBaseScriptApp_Inst():GetRootFilePath(nil) .. "/" .. "var/gas/" .. name .. ".log"
	local fo = CLuaIO_Open(strpath, "w+")
	if fo then
		--RequestVM
		fo:WriteString("Command\t\t".."Time\t".."Count\t".."Channel\t".."FuncName\n")
		local tblBlockChannel = GetBlockChannelList()
		for i=1, #tblBlockChannel do
			local Channel = ""
			local uSize = #tblBlockChannel[i][1]
			for j=1, uSize do
				if j == uSize then
					Channel = Channel..tblBlockChannel[i][1][j]
				else
					Channel = Channel..tblBlockChannel[i][1][j]..","
				end
			end
			local strInfo = "RequestVM\t\t"..tblBlockChannel[i][3].."\t\t"..Channel.."\t"..tblBlockChannel[i][2]
			fo:WriteString(strInfo.."\n")
		end
		
		local GetDbTransInfoCount = 0
		local TblWaitingLocalChannel = {}
		local TblWaitingConnection = {}
		local TblWaitingGlobalChannel = {}
		local TblExecutingSql = {}

		local function CallBack(WaitingLocalChannel, WaitingConnection, WaitingGlobalChannel, ExecutingSql)
			for i = 1, #WaitingLocalChannel do
				if WaitingLocalChannel[i][1] ~= "GetDbTransInfo" and WaitingLocalChannel[i][1] ~= "StartGmAction" then
					table.insert(TblWaitingLocalChannel, WaitingLocalChannel[i])
				end
			end
			for i = 1, #WaitingConnection do
				if WaitingConnection[i][1] ~= "GetDbTransInfo" and WaitingConnection[i][1] ~= "StartGmAction" then
					table.insert(TblWaitingConnection, WaitingConnection[i])
				end
			end
			for i = 1, #WaitingGlobalChannel do
				if WaitingGlobalChannel[i][1] ~= "GetDbTransInfo" and WaitingGlobalChannel[i][1] ~= "StartGmAction" then
					table.insert(TblWaitingGlobalChannel, WaitingGlobalChannel[i])
				end
			end
			for i = 1, #ExecutingSql do
				if ExecutingSql[i][1] ~= "GetDbTransInfo" and ExecutingSql[i][1] ~= "StartGmAction" then
					table.insert(TblExecutingSql, ExecutingSql[i])
				end
			end	
			
			if GetDbTransInfoCount == g_ChildVMNum then

				for i = 1, #TblWaitingLocalChannel do
					local Channels = ""
					local Size = #TblWaitingLocalChannel[i][3]
					for j = 1, Size do
						if j == Size then
							Channels = Channels..TblWaitingLocalChannel[i][3][j]
						else
							Channels = Channels..TblWaitingLocalChannel[i][3][j]..","
						end
					end
					local strInfo = "WaitingLocalChannel\t"..TblWaitingLocalChannel[i][2].."\t"..TblWaitingLocalChannel[i][4].."\t"..Channels.."\t"..TblWaitingLocalChannel[i][1]
					fo:WriteString(strInfo.."\n")
				end
				for i = 1, #TblWaitingConnection do
					local Channels = ""
					local Size = #TblWaitingConnection[i][3]
					for j = 1, Size do
						if j == Size then
							Channels = Channels..TblWaitingConnection[i][3][j]
						else
							Channels = Channels..TblWaitingConnection[i][3][j]..","
						end
					end
					local strInfo = "WaitingConnection\t"..TblWaitingConnection[i][2].."\t"..TblWaitingConnection[i][4].."\t"..Channels.."\t"..TblWaitingConnection[i][1]
					fo:WriteString(strInfo.."\n")
				end		
				for i = 1, #TblWaitingGlobalChannel do
					local Channels = ""
					local Size = #TblWaitingGlobalChannel[i][3]
					for j = 1, Size do
						if j == Size then
							Channels = Channels..TblWaitingGlobalChannel[i][3][j]
						else
							Channels = Channels..TblWaitingGlobalChannel[i][3][j]..","
						end
					end
					local strInfo = "WaitingGlobalChannel\t"..TblWaitingGlobalChannel[i][2].."\t"..TblWaitingGlobalChannel[i][4].."\t"..Channels.."\t"..TblWaitingGlobalChannel[i][1]
					fo:WriteString(strInfo.."\n")
				end
				
				for i=1, #TblExecutingSql do
					local Channels = ""
					local Size = #TblExecutingSql[i][3]
					for j=1, Size do
						if j == Size then
							Channels = Channels..TblExecutingSql[i][3][j]
						else
							Channels = Channels..TblExecutingSql[i][3][j]..","
						end
					end
					local strInfo = "ExecutingSql\t"..TblExecutingSql[i][2].."\t"..TblExecutingSql[i][4].."\t"..Channels.."\t"..TblExecutingSql[i][1]
					fo:WriteString(strInfo.."\n")
				end
				
				fo:WriteString("End")
				fo:Close()
				
				GetDbTransInfoCount = 0
				TblWaitingLocalChannel = {}
				TblWaitingConnection = {}
				TblWaitingGlobalChannel = {}
				TblExecutingSql = {}
			else
				GetDbTransInfoCount = GetDbTransInfoCount + 1
			end
		end
		for i = 0, g_ChildVMNum do
			CallSpecialVMDbTrans(i, "GMDB", "GetDbTransInfo", CallBack, nil, {})
		end
	end
end

function GMCommand.Executor.StartDbTranProfiler( Connection )
	StartDbTranProfiler()
end
function GMCommand.Executor.StopDbTranProfiler( Connection )
	StopDbTranProfiler()
end
function GMCommand.Executor.PrintDbTranProfile( Connection )
	local name = "DbTran_"..GetCurrentPID()
	local strpath = CBaseScriptApp_Inst():GetRootFilePath(nil) .. "/" .. "var/gas/" .. name .. ".log"
	local fo = CLuaIO_Open(strpath, "w+")
	if fo then
		local DBTransInfo = GetDbTranProfile()
		local tbl_temp = {}
		local TotalTime = 0
		fo:WriteString("Percent".."\t".."TotalTime".."\t".."Count".."\t".."SingleTime".."\t".."TransName".."\n")
		for v, p in pairs(DBTransInfo) do
			table.insert(tbl_temp,{v,p[1],p[2]})
			TotalTime = TotalTime + p[2]
		end
		table.sort(tbl_temp, function (a, b) return a[3] > b[3] end)
		for i=1,# tbl_temp do
			if tbl_temp[i][2] ~= 0 then
				local Time = string.format("%.0f",tbl_temp[i][3]/tbl_temp[i][2])
				local percent = string.format("%.3f",tbl_temp[i][3]*100/TotalTime)
				local StrInfo = percent .. "\t\t" .. tbl_temp[i][3] .. "\t\t" .. tbl_temp[i][2]
							.. "\t" .. Time .. "\t\t"..tbl_temp[i][1]
				fo:WriteString(StrInfo.."\n")
			end
		end
		fo:WriteString("End")
		fo:Close()
	end
end

function GMCommand.Executor.StartDbCallProfiler( Connection )
	local data = {}
	data["Open"] = true
	for i = 0, g_ChildVMNum do
		CallSpecialVMDbTrans(i, "GMDB", "EnableSqlProfiler", nil, nil, data)
	end
end

function GMCommand.Executor.StopDbCallProfiler( Connection )
	local data = {}
	data["Open"] = false
	for i = 0, g_ChildVMNum do
		CallSpecialVMDbTrans(i, "GMDB", "EnableSqlProfiler", nil, nil, data)
	end
end

local SqlProfileData = {}
local SqlTotalTime = 0
local CallBackCount = 0

function GMCommand.Executor.PrintDbCallProfile( Connection )
	local name = "DbCall_"..GetCurrentPID()
	local strpath = CBaseScriptApp_Inst():GetRootFilePath(nil) .. "/" .. "var/gas/" .. name .. ".log"
	local fo = CLuaIO_Open(strpath, "w+")
	if fo then
		local function CallBack(childVMSqlData, childVMTotalTime)
			for name, data in pairs(childVMSqlData) do
				if SqlProfileData[name] ~= nil then
					SqlProfileData[name].m_uCount = SqlProfileData[name].m_uCount + data.m_uCount
					SqlProfileData[name].m_uAllTime = SqlProfileData[name].m_uAllTime + data.m_uAllTime
				else
					SqlProfileData[name] = {}
					SqlProfileData[name].m_uCount = data.m_uCount
					SqlProfileData[name].m_uAllTime = data.m_uAllTime
				end
			end
			SqlTotalTime = SqlTotalTime + childVMTotalTime

			if CallBackCount == g_ChildVMNum then
				local tblSqlProfileData = {}
				for name, data in pairs(SqlProfileData) do
					if data.m_AllTime ~= 0 and data.m_uCount ~= 0 then
						local percentTime = string.format("%.2f", data.m_uAllTime*100 / SqlTotalTime)
						local averTime = string.format("%.0f", data.m_uAllTime / data.m_uCount)
						table.insert(tblSqlProfileData, {percentTime, data.m_uAllTime, data.m_uCount, averTime, name})
					end
				end
				table.sort(tblSqlProfileData, function (a, b) return a[2] > b[2] end)
				
				fo:WriteString("Percent\tTotal\tCount\tSingle\tFuncName\n")
				for i = 1, #tblSqlProfileData do
					if tblSqlProfileData[i][2] ~= 0 then
						local strInfo = tblSqlProfileData[i][1].."\t"..tblSqlProfileData[i][2].."\t"..tblSqlProfileData[i][3].."\t"..tblSqlProfileData[i][4].."\t"..tblSqlProfileData[i][5]
						fo:WriteString(strInfo.."\n")
					end
				end
				fo:WriteString("End\n")
				fo:Close()

				SqlProfileData = {}
				SqlTotalTime = 0
				CallBackCount = 0
			else
				CallBackCount = CallBackCount + 1
			end
		end

		for i = 0, g_ChildVMNum do
			CallSpecialVMDbTrans(i, "GMDB", "GetDbSqlProfilerData", CallBack, nil, {})
		end
	end
end

function GMCommand.Executor.LogCreatedPipe( Connection )
	Gas2GasAutoCall:LogCreatedPipe(GetServerAutoConn(1))
end

-- GMCommand.GMCommandList 是 GM 指令注册表
-- 格式为: [ "$指令名" ] = { "参数列表", 指令执行函数, "指令描述",指令等级 },
-- 参数列表 为该 GM 指令需要输入的参数, i 表示数字, s 表示字符串
-- 指令执行函数 格式: GMCommand.Executor.函数名( Pipe, v1, v2, v3, .... )
-- (1) Pipe 为固定传入参数，表示当前GM指令的提交者Connection
-- (2) v1、v2、v3 .. 等参数是按 "参数列表" 同序的传入参数
-- (3) 所有GM指令返回结果均为一格字符串, 字符串全部内容将在Client端GM窗口显示。若字符串为nil,则显示"Done";若字符串为"",则什么都不显示。
--
-- 在 GM Console 窗口使用时,格式为: 
-- $指令名 ( 参数1, 参数2, ... )  或  $指令名 参数1, 参数2, ...  自由格式,不固定空格数
-- 执行时将由GMCommand作 语法解释 和 Connection-Command 权限判断, 通过后将用 ( 参数1, 参数2, ... ) 
-- 调用该 GM指令的执行函数, 注意: 所有执行函数的第一个参数为 Pipe

GMCommand.GMCommandList = {}

local Keys = GMCommand_Common:GetKeys()
for i, p in pairs( Keys ) do
	local RowInfo = GMCommand_Common(p)
	local tblInfo = {}
	local func = loadstring("return GMCommand.Executor." .. RowInfo("Func"))()
	tblInfo[1] = RowInfo("ParamType")
	tblInfo[2] = func
	tblInfo[3] = RowInfo("Level")
	local newi = string.lower( p )
	GMCommand.GMCommandList[ newi ] = tblInfo
end
GMCommand_Common = nil
