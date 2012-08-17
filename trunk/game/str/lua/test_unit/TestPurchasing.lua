local SetLevelMsg = MsgTbl.SetLevelMsg

local TestPurchasingCase = TestCase("TestPurchasing")

local function PrintInfo(...)
	print(...)
end

function TestPurchasingCase:test_Purchasing(case_data)
	local UserName = case_data.m_UserName
	
	if not case_data.m_aa then
		case_data.m_aa = 0
	end
	case_data.m_aa = case_data.m_aa + 1
	--PrintInfo(1,UserName,case_data.m_aa)
	StressRpcToServer("GM_Execute", case_data.m_Conn, "$addlevel(80)" )
	WaitForMsg(SetLevelMsg)
	--PrintInfo(2,UserName,case_data.m_aa)
	PrintInfo(UserName,"完全退出副本................2")
end

return TestPurchasingCase
