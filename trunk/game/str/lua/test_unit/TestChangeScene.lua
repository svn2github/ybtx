local ChangeSceneSucMsg = MsgTbl.ChangeSceneSucMsg
local ChangeSceneFailedMsg = MsgTbl.ChangeSceneFailedMsg
local RetEnterOtherServerMsg = MsgTbl.RetEnterOtherServerMsg

local Main_Scene = {
	"新轻歌林地",
	"雁南郡",
	"尼瑞克平原",
	"西林岛",
}

local AllSceneNum = #(Main_Scene)
assert((AllSceneNum) ~= 0)

local SceneNum = 1

local TestChangeSceneCase = TestCase("TestChangeScene")

function TestChangeSceneCase:test_ChangeScene(case_data)
	local Conn = case_data.m_Conn
	
	local scene_name = Main_Scene[SceneNum]
	SceneNum = SceneNum + 1
	if SceneNum > AllSceneNum then
		SceneNum = 1
	end
	
	--print(scene_name)
	local PosX = math.random(1, 10)
	local PosY = math.random(1, 10)
	
	assert(IsCppBound(Conn))
	StressRpcToServer("GM_Execute", Conn, "$changescene(\""  .. scene_name .."\", "..PosX..","..PosY..")")
	
	local waited_msg = WaitForMsg(RetEnterOtherServerMsg, ChangeSceneSucMsg)
	if waited_msg == ChangeSceneSucMsg then
		return
	end
	
	local res = LoginOtherServer(case_data)
	if not res then
		return
	end
	
end

return TestChangeSceneCase
