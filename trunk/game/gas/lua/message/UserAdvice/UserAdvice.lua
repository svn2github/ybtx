gac_gas_require "framework/text_filter_mgr/TextFilterMgr"


UserAdvice = class()

function UserAdvice.SendUserAdviceInfo(Conn,questionText,questionTypeId, typeOptionId)
	local player = Conn.m_Player
	if not player then return end
	local textFltMgr = CTextFilterMgr:new()
	questionText = textFltMgr:RemoveTab1(questionText)
	if string.len(questionText) ==  0 then
		Gas2Gac:RetSendUserAdviceInfo(Conn,7043)
		return
	end
	
	EService.InsertQuestionFromClient(Conn, questionTypeId, typeOptionId, questionText)
	
	local data = {}
	data.m_sMsg = questionText
	data.m_uType = questionTypeId
	data.m_uCharId = player.m_uID
	data.m_sIP = Conn.m_strIP
	CallAccountManualTrans(Conn.m_Account, "UserAdviceDB", "GetUserLogCode", nil,data,"UserAdvice")
end


function UserAdvice.QueryQuestionList(Conn)
	EService.ListQueryGameQuestions(Conn)
end