engine_require "common/Misc/TypeCheck"

gac_gas_require "framework/text_filter_mgr/TextFilterMgr"

-- 确认用户名是否合法
function CheckUserName(sUserName)
		local textFilter = CTextFilterMgr:new()
		local strUserName = textFilter:RemoveTab1(sUserName)
		if not IsString(strUserName) then
			return false,29   --"非法字符！"
		end
		if(string.len(sUserName)<6 or string.len(sUserName)>31) then
			return false,30   --"用户名的长度必须在6~31之间"
		end			
		if string.find(sUserName, "[ ]") then
           return false, 31 --"用户名不允许含有空格" 
        end
		if(string.find(sUserName, "[^%w.@_-]")) then
			return false,32  --"用户名只能包含字母，数字，点和下划线！"
		end
		if(string.find(sUserName, "^[%d%a]") == nil ) then
			return false,33 -- "用户名必须是字母或者数字开头"
		end
		return true
end