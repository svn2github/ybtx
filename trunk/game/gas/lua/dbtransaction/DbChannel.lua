engine_require "server/thread/DbChannelMgr"

local StmtContainer = class()

local StmtDef = {
		"RequestChannelId",
		[[	
			call RequestChannelId(?)
		]]
}
DefineSql(StmtDef, StmtContainer)



function CDbChannelMgr:RequestGlobalChannel(...)
	local arg = {...}
	local channel_id = {}
	
	if nil ~= arg[1] then
		
		local uChannelNum = 0
		
		for k, v in ipairs(arg) do
			if type(v) == "number" then
				table.insert(channel_id, v)
			else
				error("不正确的channel类型")
			end
			
			uChannelNum = uChannelNum + 1
		end
		
		--给channel排序，防止死锁
		table.sort(channel_id)
		
		StmtContainer.RequestChannelId:BatchExec( uChannelNum , unpack(channel_id) )
	end

end


