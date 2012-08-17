local ChangeSceneSucMsg = MsgTbl.ChangeSceneSucMsg
local ChangeSceneFailedMsg = MsgTbl.ChangeSceneFailedMsg
local RetEnterOtherServerMsg = MsgTbl.RetEnterOtherServerMsg
local CreatePlayerMsg = MsgTbl.CreatePlayerMsg


local Main_Scene = {
	"界林山区",
	"轻歌林地",
	"尼瑞克平原",
	--"花语平原1",
}

local AllSceneNum = #Main_Scene
assert((AllSceneNum) ~= 0)

local TestLoginFlowCase = TestCase("TestLoginFlow")

function TestLoginFlowCase:test_LoginFlow(case_data)
	local Conn = case_data.m_Conn
	local user_name = case_data.m_UserName
	
	for i = 1, 4 do
		local SceneNum = math.random(1, AllSceneNum)
		local scene_name = Main_Scene[SceneNum]
		local x_pos = math.random(1, 10)
		local y_pos = math.random(1, 10)
		local cmd = "$changescene(\""  .. scene_name .."\", " .. x_pos .. ", " .. y_pos .. ")"
		--print(user_name, cmd)
		StressRpcToServer("GM_Execute", Conn, cmd )
	
		local waited_msg = WaitForMsg(RetEnterOtherServerMsg, ChangeSceneSucMsg, CreatePlayerMsg)
		--print(user_name, scene_name, waited_msg)

		if waited_msg == RetEnterOtherServerMsg then
			local res = LoginOtherServer(case_data)
			if not res then
				return
			end
			Conn = case_data.m_Conn
			--print(user_name, "LoginOtherServer")		
		end
				
		local wait_time = math.random(10, 30)
	
		TestSleep(wait_time * 1000)
	end

	local wait_time = math.random(30, 100)
	
	print(user_name, "ReConnect ", wait_time)
	
	ReConnect(case_data, wait_time * 1000, ConnAddr)
	
	Login(case_data, 0)	
	
	print(user_name, "login suc")
end

return TestLoginFlowCase
