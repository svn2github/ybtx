lan_load "message/Lan_Message_Common"
lan_load "message/Lan_SystemFriendMessage_Common"

local ContentTbl = {}
ContentTbl["Lan_Message_Common"] = Lan_Message_Common
ContentTbl["Lan_SystemFriendMessage_Common"] = Lan_SystemFriendMessage_Common

function GetMessageFun(FunName,ContentTblName)
	local funTempTbl = {}
	local funlist = {}
	local MsgIDToFunTbl = {}
	local Keys = ContentTbl[ContentTblName]:GetKeys()
	for i, p in pairs (Keys) do 
		local RowInfo = ContentTbl[ContentTblName](p)
		if RowInfo("Content") ~= nil and RowInfo("Content") ~= "" then
			local funParameter = ""
			for w in string.gfind(RowInfo("Content"), "(%%[sd])") do
				funParameter = funParameter .. w
			end
--			if funParameter ~= "" then
				funParameter = string.gsub(funParameter,"(%%[d])","i")
				funParameter = string.gsub(funParameter,"(%%[s])","S")
				MsgIDToFunTbl[p] = FunName .. funParameter
				if funTempTbl[FunName .. funParameter] ~= true then
					table.insert(funlist,{FunName .. funParameter,"I" .. funParameter})
					funTempTbl[FunName .. funParameter] = true
				end
--			else
--				MsgIDToFunTbl[i] = FunName
--			end
		end
	end
	return funlist,MsgIDToFunTbl
end

function ReceiveSystemFriendMsg(MessageId, ...)
	--local arg = {...}
	if(MessageId > 5000 and MessageId < 5020) then --ÓÊ¼şÌáĞÑ
		 g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreateEMailMsgMinimum(MessageId)
	else
		CRollIntoMsg.CreateRollIntoMsgWnd(MessageId)
	end
end