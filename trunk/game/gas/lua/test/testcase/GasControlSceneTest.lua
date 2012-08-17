
function InitGasSceneControl()
	local TestTest=TestCase("test/testcase/GasSceneControl")
	
	function TestTest:setup()
	end
	
	function TestTest:TestSceneControl()
	
		local FinishEvent = CQueue:new()
		
		local function CoSceneControl()
		--[[
			local Scene=g_SceneMgr:CreateScene("«·∏Ë¡÷µÿ")
			assert(Scene)
			g_SceneMgr:GetScene(Scene.m_SceneId)
			g_SceneMgr:DestroyScene(Scene)
			
			SetEvent( FinishEvent , true )
		--]]
		end
		

		WaitEvent( true , nil, FinishEvent )
		
	end 
	
	function TestTest:teardown()
	end
	
end