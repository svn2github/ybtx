gac_gas_require "framework/text_filter_mgr/TextFilterMgrInc"
engine_require "common/Loader/loader"
lan_load "message/Lan_NameFilter_Common"
lan_load "message/Lan_MsgFilter_Common"
gac_gas_require "framework/common/CMoney"
lan_load "tong/Lan_TongResPos_Common"
lan_load "tong/Lan_TongFightTech_Common"
lan_load "skill/Lan_BuffName"

local string_find = StringFindBaseOnUTF8	
--判断命名合理性,有些名称不允许使用(如伟人名字:毛泽东)
function CTextFilterMgr:IsValidName(sName)
--	if( self:FindFirstSBCOnUTF8(sName) >= 0 ) then
--		return false
--	end
	local Keys = Lan_NameFilter_Common:GetKeys()
	for i,p in pairs (Keys) do
		if(string_find(sName, Lan_NameFilter_Common(p,"Filter"),  0) ~= -1) then
			return false
		end
	end
	return true
end
--判断消息合理性,屏蔽不合法消息(如脏话)
function CTextFilterMgr:IsValidMsg(sMsg)
	local Keys = Lan_MsgFilter_Common:GetKeys()
	for i,p in pairs (Keys) do
		if(string_find(sMsg, Lan_MsgFilter_Common(p,"Filter"), 0) ~= -1) then
			return false
		end
	end
	return true
end

--去掉字符串中的前后空格[第一种方法]
function CTextFilterMgr:RemoveTab1(sMsg)
	local message = ""
	--去掉字符串前面的空格、制表符和回车符
	for i=1, string.len(sMsg) do
		local chari = string.sub(sMsg,i,i)
		if( chari ~= " " and chari ~= "\n" and chari ~= "\t" ) then
			message = string.sub(sMsg,i,-1)
			break
		end
		--如果到最后一个还是空，则返回空字符串
		if i == string.len(sMsg) then
			return message
		end
	end
	--去掉字符串后面的空格、制表符和回车符
	for i=1, string.len(message) do
		local chari = string.sub(message, string.len(message) - i +1, string.len(message) - i +1)
		if( chari ~= " " and chari ~= "\n" and chari ~= "\t" ) then
			message = string.sub(message,1, string.len(message) - i +1)
			break
		end
	end
	return message
end

--去掉字符串中的前后空格[第二种方法]
function CTextFilterMgr:RemoveTab2(sMsg)
	--去掉前面的空值
	local trim_begin = string.gsub(sMsg, "^%s+", "")
	--去掉后面的空格
	local trim_end = string.gsub(sMsg, "%s+$", "")
	--去掉前后所有的空格
	return string.gsub(trim_end,"^%s+", "")
end


--把字符串中的不合法字符替换成“****”
function CTextFilterMgr:ReplaceInvalidChar(sMsg)
	local message = sMsg
	--一次遍历整个表，替换该字符串中所有不合法的字符
	local Keys = Lan_MsgFilter_Common:GetKeys()
	for i ,p in pairs(Keys) do
		if(string_find(sMsg,Lan_MsgFilter_Common(p,"Filter"), 0) ~= -1) then
			message = string.gsub(message, Lan_MsgFilter_Common(p,"Filter"),"*")
		end
	end
	return message
end

function CTextFilterMgr:FindFirstSBCOnUTF8(str)
	return find_sbc_on_utf8(str)
end


--创建角色时，角色名关键字筛选函数
function CTextFilterMgr:CheckRoleName(strRoleName)
	local textFilter = CTextFilterMgr:new()
	if not IsString(strRoleName) then
		return false,28    --非法角色名
	end
	
	if string.find(strRoleName, "[#\\/' \"]") then
		return false, 32   --角色名只能含有中文、英文和数字
	end
	
	if string.find(strRoleName, "", 1, true) then
		return false, 32
	end
	
	if GetCharCount(strRoleName) < 6 or GetCharCount(strRoleName) >16 then
        return false, 24 
	end 

	return true
end

function CTextFilterMgr:GetStringByMsg(sStr)
	local sMsg = ""
		local nIndex = string.find(sStr,"_")
		if IsNumber(nIndex) and nIndex > 0 then
			local MsgID = tonumber(string.sub(sStr, 1,nIndex-1))
			local len = string.len(sStr)
			local str_tbl = string.sub(sStr, nIndex + 1,len)
			if str_tbl then
				local str = loadstring("return {" .. str_tbl .. "}")()
				sMsg = GetLogClient(MsgID,unpack(str))
			end
		else
			sStr = tonumber(sStr)
			if IsNumber(sStr) then
				sMsg = GetLogClient(sStr)
			end
		end
		return sMsg
end

--服务端传到客户端的messageid，客户端读出对应的信息显示
function CTextFilterMgr:GetRealStringByMessageID(sStr,strType)
	if strType == 0 then
		return self:GetStringByMsg(sStr)
	end
	local sMsg = ""
	local nIndex = string.find(sStr,"_")
	if nIndex and nIndex > 0 then
		local MsgID = tonumber(string.sub(sStr, 1,nIndex-1))
		local len = string.len(sStr)
		local str_tbl = string.sub(sStr, nIndex + 1,len)
		local tbl_msg = {}
		if str_tbl then
			SplitString(str_tbl, "%,", tbl_msg)
			for i =1,#tbl_msg do
				local msgi = tbl_msg[i]
				if string.find(msgi,"item:") then
					local item_type = tonumber(string.sub(msgi,string.find(msgi,":")+1,string.find(msgi,"|")-1))
					local item_name = string.sub(msgi,string.find(msgi,"|")+1,string.len(msgi))
					local display_name = g_ItemInfoMgr:GetItemLanInfo(item_type,item_name,"DisplayName")
					tbl_msg[i] = display_name
				elseif string.find(msgi,"money:") then
					local money_type = tonumber(string.sub(msgi,string.find(msgi,":")+1,string.find(msgi,"|")-1))
					local money_count = tonumber(string.sub(msgi,string.find(msgi,"|")+1,string.len(msgi)))
					local g_MoneyMgr = CMoney:new()
					if money_type == 1 then
						tbl_msg[i] = g_MoneyMgr:ChangeMoneyToString(money_count,EGoldType.GoldCoin)
					else
						tbl_msg[i] = g_MoneyMgr:ChangeMoneyToString(money_count,EGoldType.GoldBar)
					end
				elseif string.find(msgi,"msgid:") then
					local msgid = tonumber(string.sub(msgi,string.find(msgi,":")+1,string.len(msgi)))
					tbl_msg[i] = GetLogClient(msgid)
				elseif string.find(msgi,"objname:") then
					local obj_name = string.sub(msgi,string.find(msgi,":")+1,string.len(msgi))
					tbl_msg[i] = Lan_TongResPos_Common(MemH64(obj_name), "ShowName")
				elseif string.find(msgi,"techname:") then
					local tech_name = string.sub(msgi,string.find(msgi,":")+1,string.len(msgi))
					tbl_msg[i] = Lan_TongFightTech_Common(MemH64(tech_name), "DisplayName")
				elseif string.find(msgi,"fuli:") then
					local buffName = string.sub(msgi,string.find(msgi,":")+1,string.len(msgi))
					tbl_msg[i] = Lan_BuffName(MemH64(buffName),"DisplayName")
				end
			end
			sMsg = GetLogClient(MsgID,unpack(tbl_msg))
		end
	else
		sMsg = tonumber(sStr) and GetLogClient(tonumber(sStr)) or sStr
	end
	return sMsg
end